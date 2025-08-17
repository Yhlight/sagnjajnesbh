#include "ContextManager.h"
#include "../ast/ASTNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chtl {

// CompilationPhaseGuard 实现
CompilationPhaseGuard::CompilationPhaseGuard(ContextManager& manager, CompilationPhase phase)
    : manager_(manager), should_restore_(false), is_valid_(false) {
    
    previous_phase_ = manager_.getCurrentPhase();
    
    if (manager_.canTransitionToPhase(phase)) {
        if (manager_.setPhase(phase)) {
            should_restore_ = true;
            is_valid_ = true;
        }
    }
}

CompilationPhaseGuard::~CompilationPhaseGuard() {
    if (should_restore_) {
        manager_.setPhase(previous_phase_);
    }
}

// ContextScopeGuard 实现
ContextScopeGuard::ContextScopeGuard(ContextManager& manager, ContextScope scope, 
                                     const std::string& name, ast::ASTNode* node)
    : manager_(manager), scope_(scope), should_pop_(false), is_valid_(false) {
    
    if (manager_.enterScope(scope_, name, node)) {
        should_pop_ = true;
        is_valid_ = true;
    }
}

ContextScopeGuard::~ContextScopeGuard() {
    if (should_pop_) {
        manager_.exitScope();
    }
}

// CompoundStateGuard 实现
CompoundStateGuard::CompoundStateGuard(ContextManager& manager, 
                                      CompilationPhase phase,
                                      CompilerState compiler_state,
                                      SyntaxContext syntax_context,
                                      ContextScope scope,
                                      ast::ASTNode* node,
                                      const std::string& scope_name)
    : is_valid_(true) {
    
    // 创建编译阶段守护器
    phase_guard_ = manager.createPhaseGuard(phase);
    if (!phase_guard_ || !phase_guard_->isValid()) {
        is_valid_ = false;
        return;
    }
    
    // 创建编译器状态守护器
    state_guard_ = manager.getCompilerContext().createStateGuard(compiler_state, syntax_context);
    if (!state_guard_ || !state_guard_->isValid()) {
        is_valid_ = false;
        return;
    }
    
    // 创建上下文作用域守护器
    scope_guard_ = manager.createScopeGuard(scope, scope_name, node);
    if (!scope_guard_ || !scope_guard_->isValid()) {
        is_valid_ = false;
        return;
    }
    
    // 如果有AST节点，创建节点状态守护器
    if (node) {
        ast::NodeState node_state = ast::NodeState::PARSING;
        ast::ProcessingPhase node_phase = ast::ProcessingPhase::SYNTAX_ANALYSIS;
        
        // 根据编译阶段确定节点状态
        switch (phase) {
            case CompilationPhase::LEXICAL_SCANNING:
                node_phase = ast::ProcessingPhase::LEXICAL_ANALYSIS;
                break;
            case CompilationPhase::SYNTAX_PARSING:
                node_phase = ast::ProcessingPhase::SYNTAX_ANALYSIS;
                node_state = ast::NodeState::PARSING;
                break;
            case CompilationPhase::SEMANTIC_ANALYSIS:
                node_phase = ast::ProcessingPhase::SEMANTIC_ANALYSIS;
                node_state = ast::NodeState::VALIDATING;
                break;
            case CompilationPhase::OPTIMIZATION:
                node_phase = ast::ProcessingPhase::OPTIMIZATION;
                node_state = ast::NodeState::TRANSFORMING;
                break;
            case CompilationPhase::CODE_GENERATION:
                node_phase = ast::ProcessingPhase::CODE_GENERATION;
                node_state = ast::NodeState::GENERATING;
                break;
            default:
                break;
        }
        
        node_guard_ = manager.getASTStateManager().createStateGuard(node, node_state, node_phase, scope_name);
        if (!node_guard_ || !node_guard_->isValid()) {
            is_valid_ = false;
            return;
        }
    }
}

CompoundStateGuard::~CompoundStateGuard() {
    // RAII守护器会自动析构，按照创建的逆序
}

// ContextManager 实现
ContextManager::ContextManager() : current_phase_(CompilationPhase::INITIALIZATION) {
    // 初始化全局作用域
    ContextState global_state(ContextScope::GLOBAL, CompilationPhase::INITIALIZATION, "global");
    context_stack_.push(global_state);
}

CompilationPhase ContextManager::getCurrentPhase() const {
    return current_phase_;
}

bool ContextManager::setPhase(CompilationPhase phase) {
    if (!canTransitionToPhase(phase)) {
        reportContextError("Invalid phase transition from " + 
                          compilationPhaseToString(current_phase_) + 
                          " to " + compilationPhaseToString(phase));
        return false;
    }
    
    CompilationPhase old_phase = current_phase_;
    current_phase_ = phase;
    
    // 更新当前上下文状态
    updateCurrentContextState();
    
    // 通知监听器
    notifyPhaseChanged(old_phase, phase);
    
    return true;
}

bool ContextManager::canTransitionToPhase(CompilationPhase phase) const {
    return isValidPhaseTransition(current_phase_, phase);
}

std::string ContextManager::getPhaseDescription() const {
    return compilationPhaseToString(current_phase_);
}

ContextScope ContextManager::getCurrentScope() const {
    if (context_stack_.empty()) {
        return ContextScope::GLOBAL;
    }
    return context_stack_.top().scope;
}

bool ContextManager::enterScope(ContextScope scope, const std::string& name, ast::ASTNode* node) {
    ContextState new_state(scope, current_phase_, name);
    new_state.compiler_state = compiler_context_.getStateManager().getCurrentState();
    new_state.syntax_context = compiler_context_.getStateManager().getCurrentContext();
    
    // 获取当前位置信息
    auto [file_path, line, column] = getCurrentPosition();
    new_state.file_path = file_path;
    new_state.line_number = line;
    new_state.column_number = column;
    new_state.associated_node = node;
    
    context_stack_.push(new_state);
    
    // 如果有关联的AST节点，设置相应的标记
    if (node) {
        switch (scope) {
            case ContextScope::TEMPLATE:
                ast_state_manager_.setNodeFlag(node, ast::NodeFlag::TEMPLATE_NODE);
                break;
            case ContextScope::CUSTOM:
                ast_state_manager_.setNodeFlag(node, ast::NodeFlag::CUSTOM_NODE);
                break;
            case ContextScope::STYLE_BLOCK:
            case ContextScope::SCRIPT_BLOCK:
                ast_state_manager_.setNodeFlag(node, ast::NodeFlag::DYNAMIC_NODE);
                break;
            default:
                ast_state_manager_.setNodeFlag(node, ast::NodeFlag::STATIC_NODE);
                break;
        }
    }
    
    // 通知监听器
    notifyScopeEntered(scope, name);
    
    return true;
}

bool ContextManager::exitScope() {
    if (context_stack_.size() <= 1) {
        reportContextError("Cannot exit global scope");
        return false;
    }
    
    ContextState exiting_state = context_stack_.top();
    context_stack_.pop();
    
    // 通知监听器
    notifyScopeExited(exiting_state.scope, exiting_state.scope_name);
    
    return true;
}

std::string ContextManager::getCurrentScopeName() const {
    if (context_stack_.empty()) {
        return "global";
    }
    return context_stack_.top().scope_name;
}

size_t ContextManager::getScopeDepth() const {
    return context_stack_.size();
}

ContextState ContextManager::getCurrentContextState() const {
    if (context_stack_.empty()) {
        return ContextState();
    }
    return context_stack_.top();
}

std::vector<ContextState> ContextManager::getContextStack() const {
    std::vector<ContextState> stack_copy;
    std::stack<ContextState> temp_stack = context_stack_;
    
    while (!temp_stack.empty()) {
        stack_copy.push_back(temp_stack.top());
        temp_stack.pop();
    }
    
    std::reverse(stack_copy.begin(), stack_copy.end());
    return stack_copy;
}

bool ContextManager::isInScope(ContextScope scope) const {
    std::stack<ContextState> temp_stack = context_stack_;
    
    while (!temp_stack.empty()) {
        if (temp_stack.top().scope == scope) {
            return true;
        }
        temp_stack.pop();
    }
    
    return false;
}

bool ContextManager::isInPhase(CompilationPhase phase) const {
    return current_phase_ == phase;
}

void ContextManager::updatePosition(const std::string& file_path, size_t line, size_t column) {
    compiler_context_.setCurrentFile(file_path);
    compiler_context_.setCurrentPosition(line, column);
    
    // 更新当前上下文状态
    if (!context_stack_.empty()) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.file_path = file_path;
        current.line_number = line;
        current.column_number = column;
    }
}

std::tuple<std::string, size_t, size_t> ContextManager::getCurrentPosition() const {
    std::string file = compiler_context_.getCurrentFile();
    auto [line, column] = compiler_context_.getCurrentPosition();
    return std::make_tuple(file, line, column);
}

void ContextManager::associateNode(ast::ASTNode* node) {
    if (!context_stack_.empty() && node) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.associated_node = node;
        
        // 同步节点状态
        syncNodeState(node);
    }
}

ast::ASTNode* ContextManager::getCurrentAssociatedNode() const {
    if (context_stack_.empty()) {
        return nullptr;
    }
    return context_stack_.top().associated_node;
}

void ContextManager::syncNodeState(ast::ASTNode* node) {
    if (!node) return;
    
    // 根据当前编译阶段设置节点状态
    ast::NodeState node_state = ast::NodeState::CREATED;
    ast::ProcessingPhase node_phase = ast::ProcessingPhase::LEXICAL_ANALYSIS;
    
    switch (current_phase_) {
        case CompilationPhase::LEXICAL_SCANNING:
            node_state = ast::NodeState::CREATED;
            node_phase = ast::ProcessingPhase::LEXICAL_ANALYSIS;
            break;
        case CompilationPhase::SYNTAX_PARSING:
            node_state = ast::NodeState::PARSING;
            node_phase = ast::ProcessingPhase::SYNTAX_ANALYSIS;
            break;
        case CompilationPhase::SEMANTIC_ANALYSIS:
            node_state = ast::NodeState::VALIDATING;
            node_phase = ast::ProcessingPhase::SEMANTIC_ANALYSIS;
            break;
        case CompilationPhase::OPTIMIZATION:
            node_state = ast::NodeState::TRANSFORMING;
            node_phase = ast::ProcessingPhase::OPTIMIZATION;
            break;
        case CompilationPhase::CODE_GENERATION:
            node_state = ast::NodeState::GENERATING;
            node_phase = ast::ProcessingPhase::CODE_GENERATION;
            break;
        case CompilationPhase::FINALIZATION:
            node_state = ast::NodeState::GENERATED;
            node_phase = ast::ProcessingPhase::FINALIZATION;
            break;
        default:
            break;
    }
    
    ast_state_manager_.setNodeState(node, node_state, node_phase, getCurrentScopeName());
}

void ContextManager::reportContextError(const std::string& message) {
    context_errors_.push_back(message);
    
    // 同时报告给编译器上下文
    compiler_context_.reportError(message);
    
    // 通知监听器
    notifyContextError(message);
}

std::vector<std::string> ContextManager::getContextErrors() const {
    return context_errors_;
}

bool ContextManager::hasContextErrors() const {
    return !context_errors_.empty();
}

void ContextManager::clearContextErrors() {
    context_errors_.clear();
}

void ContextManager::setContextMetadata(const std::string& key, const std::string& value) {
    if (!context_stack_.empty()) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.local_metadata[key] = value;
    }
}

std::string ContextManager::getContextMetadata(const std::string& key) const {
    if (context_stack_.empty()) {
        return "";
    }
    
    const auto& metadata = context_stack_.top().local_metadata;
    auto it = metadata.find(key);
    return it != metadata.end() ? it->second : "";
}

void ContextManager::clearContextMetadata(const std::string& key) {
    if (!context_stack_.empty()) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.local_metadata.erase(key);
    }
}

void ContextManager::syncWithCompilerContext() {
    if (!context_stack_.empty()) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.compiler_state = compiler_context_.getStateManager().getCurrentState();
        current.syntax_context = compiler_context_.getStateManager().getCurrentContext();
    }
}

void ContextManager::syncWithASTStateManager() {
    // 同步所有关联的AST节点状态
    std::stack<ContextState> temp_stack = context_stack_;
    
    while (!temp_stack.empty()) {
        const ContextState& state = temp_stack.top();
        if (state.associated_node) {
            syncNodeState(state.associated_node);
        }
        temp_stack.pop();
    }
}

bool ContextManager::validateContextConsistency() const {
    // 检查上下文栈的一致性
    if (context_stack_.empty()) {
        return false;
    }
    
    // 检查编译阶段与上下文状态的一致性
    const ContextState& current = context_stack_.top();
    if (current.phase != current_phase_) {
        return false;
    }
    
    // 检查关联的AST节点状态一致性
    if (current.associated_node) {
        ast::NodeState node_state = ast_state_manager_.getNodeState(current.associated_node);
        ast::ProcessingPhase node_phase = ast_state_manager_.getNodePhase(current.associated_node);
        
        // 验证节点状态与编译阶段的对应关系
        bool phase_consistent = false;
        switch (current_phase_) {
            case CompilationPhase::LEXICAL_SCANNING:
                phase_consistent = (node_phase == ast::ProcessingPhase::LEXICAL_ANALYSIS);
                break;
            case CompilationPhase::SYNTAX_PARSING:
                phase_consistent = (node_phase == ast::ProcessingPhase::SYNTAX_ANALYSIS);
                break;
            case CompilationPhase::SEMANTIC_ANALYSIS:
                phase_consistent = (node_phase == ast::ProcessingPhase::SEMANTIC_ANALYSIS);
                break;
            case CompilationPhase::OPTIMIZATION:
                phase_consistent = (node_phase == ast::ProcessingPhase::OPTIMIZATION);
                break;
            case CompilationPhase::CODE_GENERATION:
                phase_consistent = (node_phase == ast::ProcessingPhase::CODE_GENERATION);
                break;
            case CompilationPhase::FINALIZATION:
                phase_consistent = (node_phase == ast::ProcessingPhase::FINALIZATION);
                break;
            default:
                phase_consistent = true;
                break;
        }
        
        if (!phase_consistent) {
            return false;
        }
    }
    
    return true;
}

std::unique_ptr<CompilationPhaseGuard> ContextManager::createPhaseGuard(CompilationPhase phase) {
    return std::make_unique<CompilationPhaseGuard>(*this, phase);
}

std::unique_ptr<ContextScopeGuard> ContextManager::createScopeGuard(ContextScope scope, 
                                                                   const std::string& name, 
                                                                   ast::ASTNode* node) {
    return std::make_unique<ContextScopeGuard>(*this, scope, name, node);
}

std::unique_ptr<CompoundStateGuard> ContextManager::createCompoundGuard(CompilationPhase phase,
                                                                       CompilerState compiler_state,
                                                                       SyntaxContext syntax_context,
                                                                       ContextScope scope,
                                                                       ast::ASTNode* node,
                                                                       const std::string& scope_name) {
    return std::make_unique<CompoundStateGuard>(*this, phase, compiler_state, syntax_context, scope, node, scope_name);
}

void ContextManager::addContextChangeListener(std::shared_ptr<ContextChangeListener> listener) {
    listeners_.push_back(listener);
}

void ContextManager::removeContextChangeListener(std::shared_ptr<ContextChangeListener> listener) {
    listeners_.erase(
        std::remove(listeners_.begin(), listeners_.end(), listener),
        listeners_.end());
}

std::string ContextManager::getStatistics() const {
    std::ostringstream oss;
    oss << "Context Manager Statistics:\n";
    oss << "Current Phase: " << compilationPhaseToString(current_phase_) << "\n";
    oss << "Current Scope: " << contextScopeToString(getCurrentScope()) << "\n";
    oss << "Scope Depth: " << getScopeDepth() << "\n";
    oss << "Context Errors: " << context_errors_.size() << "\n";
    oss << "AST Nodes Managed: " << ast_state_manager_.getNodeCount() << "\n";
    
    return oss.str();
}

void ContextManager::printContextStack() const {
    std::cout << "Context Stack (top to bottom):\n";
    std::vector<ContextState> stack = getContextStack();
    
    for (int i = stack.size() - 1; i >= 0; --i) {
        const ContextState& state = stack[i];
        std::cout << "  [" << i << "] " << contextScopeToString(state.scope) 
                  << " (" << state.scope_name << ") - " 
                  << compilationPhaseToString(state.phase) << "\n";
    }
}

void ContextManager::printCurrentState() const {
    std::cout << getStatistics() << std::endl;
}

void ContextManager::reset() {
    // 清空上下文栈，保留全局作用域
    while (context_stack_.size() > 1) {
        context_stack_.pop();
    }
    
    current_phase_ = CompilationPhase::INITIALIZATION;
    context_errors_.clear();
    
    // 重置子组件
    compiler_context_.reset();
    ast_state_manager_.reset();
}

void ContextManager::clear() {
    // 完全清空
    while (!context_stack_.empty()) {
        context_stack_.pop();
    }
    
    current_phase_ = CompilationPhase::INITIALIZATION;
    context_errors_.clear();
    listeners_.clear();
    
    // 清空子组件
    compiler_context_.reset();
    ast_state_manager_.clear();
    
    // 重新初始化全局作用域
    ContextState global_state(ContextScope::GLOBAL, CompilationPhase::INITIALIZATION, "global");
    context_stack_.push(global_state);
}

std::string ContextManager::compilationPhaseToString(CompilationPhase phase) const {
    switch (phase) {
        case CompilationPhase::INITIALIZATION: return "INITIALIZATION";
        case CompilationPhase::LEXICAL_SCANNING: return "LEXICAL_SCANNING";
        case CompilationPhase::SYNTAX_PARSING: return "SYNTAX_PARSING";
        case CompilationPhase::SEMANTIC_ANALYSIS: return "SEMANTIC_ANALYSIS";
        case CompilationPhase::OPTIMIZATION: return "OPTIMIZATION";
        case CompilationPhase::CODE_GENERATION: return "CODE_GENERATION";
        case CompilationPhase::FINALIZATION: return "FINALIZATION";
        default: return "UNKNOWN";
    }
}

std::string ContextManager::contextScopeToString(ContextScope scope) const {
    switch (scope) {
        case ContextScope::GLOBAL: return "GLOBAL";
        case ContextScope::FILE: return "FILE";
        case ContextScope::NAMESPACE: return "NAMESPACE";
        case ContextScope::TEMPLATE: return "TEMPLATE";
        case ContextScope::CUSTOM: return "CUSTOM";
        case ContextScope::ELEMENT: return "ELEMENT";
        case ContextScope::ATTRIBUTE: return "ATTRIBUTE";
        case ContextScope::STYLE_BLOCK: return "STYLE_BLOCK";
        case ContextScope::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case ContextScope::COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

// 私有方法实现
void ContextManager::notifyPhaseChanged(CompilationPhase from, CompilationPhase to) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onPhaseChanged(from, to);
        }
    }
}

void ContextManager::notifyScopeEntered(ContextScope scope, const std::string& name) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onScopeEntered(scope, name);
        }
    }
}

void ContextManager::notifyScopeExited(ContextScope scope, const std::string& name) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onScopeExited(scope, name);
        }
    }
}

void ContextManager::notifyContextError(const std::string& error_message) {
    ContextState current_state = getCurrentContextState();
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onContextError(error_message, current_state);
        }
    }
}

bool ContextManager::isValidPhaseTransition(CompilationPhase from, CompilationPhase to) const {
    // 定义有效的编译阶段转换
    switch (from) {
        case CompilationPhase::INITIALIZATION:
            return to == CompilationPhase::LEXICAL_SCANNING;
            
        case CompilationPhase::LEXICAL_SCANNING:
            return to == CompilationPhase::SYNTAX_PARSING || to == CompilationPhase::INITIALIZATION;
            
        case CompilationPhase::SYNTAX_PARSING:
            return to == CompilationPhase::SEMANTIC_ANALYSIS || to == CompilationPhase::LEXICAL_SCANNING;
            
        case CompilationPhase::SEMANTIC_ANALYSIS:
            return to == CompilationPhase::OPTIMIZATION || to == CompilationPhase::SYNTAX_PARSING;
            
        case CompilationPhase::OPTIMIZATION:
            return to == CompilationPhase::CODE_GENERATION || to == CompilationPhase::SEMANTIC_ANALYSIS;
            
        case CompilationPhase::CODE_GENERATION:
            return to == CompilationPhase::FINALIZATION || to == CompilationPhase::OPTIMIZATION;
            
        case CompilationPhase::FINALIZATION:
            return to == CompilationPhase::INITIALIZATION; // 可以重新开始
            
        default:
            return false;
    }
}

void ContextManager::updateCurrentContextState() {
    if (!context_stack_.empty()) {
        ContextState& current = const_cast<ContextState&>(context_stack_.top());
        current.phase = current_phase_;
        current.compiler_state = compiler_context_.getStateManager().getCurrentState();
        current.syntax_context = compiler_context_.getStateManager().getCurrentContext();
    }
}

} // namespace chtl