#include "Context.h"
#include <iostream>
#include <sstream>

namespace chtl {

// ContextGuard实现
ContextGuard::ContextGuard(CompilerContext& context, const std::string& scope_name)
    : context_(context), should_pop_(false) {
    context_.enterScope(scope_name);
    should_pop_ = true;
}

ContextGuard::~ContextGuard() {
    if (should_pop_) {
        context_.exitScope();
    }
}

// StateGuard实现
StateGuard::StateGuard(StateManager& state_manager, CompilerState new_state, SyntaxContext context)
    : state_manager_(state_manager), should_pop_(false), is_valid_(false) {
    if (state_manager_.pushState(new_state, context)) {
        should_pop_ = true;
        is_valid_ = true;
    }
}

StateGuard::~StateGuard() {
    if (should_pop_) {
        state_manager_.popState();
    }
}

// CompilerContext实现
CompilerContext::CompilerContext() 
    : current_line_(1), current_column_(1) {
    // 初始化上下文栈
    context_stack_.push(ContextInfo());
}

void CompilerContext::setCurrentFile(const std::string& file_path) {
    current_file_ = file_path;
    if (!context_stack_.empty()) {
        context_stack_.top().file_path = file_path;
    }
}

std::string CompilerContext::getCurrentFile() const {
    return current_file_;
}

void CompilerContext::setCurrentPosition(size_t line, size_t column) {
    current_line_ = line;
    current_column_ = column;
    if (!context_stack_.empty()) {
        context_stack_.top().line_number = line;
        context_stack_.top().column_number = column;
    }
}

std::pair<size_t, size_t> CompilerContext::getCurrentPosition() const {
    return std::make_pair(current_line_, current_column_);
}

void CompilerContext::enterScope(const std::string& scope_name) {
    global_map_.enterScope(scope_name);
}

void CompilerContext::exitScope() {
    global_map_.exitScope();
}

std::string CompilerContext::getCurrentScope() const {
    auto current_scope = global_map_.getCurrentScope();
    return current_scope ? current_scope->getFullPath() : "global";
}

void CompilerContext::enterNamespace(const std::string& namespace_name) {
    global_map_.enterNamespace(namespace_name);
    if (!context_stack_.empty()) {
        context_stack_.top().current_namespace = global_map_.getCurrentNamespace();
    }
}

void CompilerContext::exitNamespace() {
    global_map_.exitNamespace();
    if (!context_stack_.empty()) {
        context_stack_.top().current_namespace = global_map_.getCurrentNamespace();
    }
}

std::string CompilerContext::getCurrentNamespace() const {
    return global_map_.getCurrentNamespace();
}

void CompilerContext::enterTemplate(const std::string& template_name) {
    template_stack_.push(template_name);
    if (!context_stack_.empty()) {
        context_stack_.top().in_template = true;
    }
}

void CompilerContext::exitTemplate() {
    if (!template_stack_.empty()) {
        template_stack_.pop();
    }
    if (!context_stack_.empty()) {
        context_stack_.top().in_template = !template_stack_.empty();
    }
}

bool CompilerContext::isInTemplate() const {
    return !template_stack_.empty();
}

std::string CompilerContext::getCurrentTemplate() const {
    return template_stack_.empty() ? "" : template_stack_.top();
}

void CompilerContext::enterCustom(const std::string& custom_name) {
    custom_stack_.push(custom_name);
    if (!context_stack_.empty()) {
        context_stack_.top().in_custom = true;
    }
}

void CompilerContext::exitCustom() {
    if (!custom_stack_.empty()) {
        custom_stack_.pop();
    }
    if (!context_stack_.empty()) {
        context_stack_.top().in_custom = !custom_stack_.empty();
    }
}

bool CompilerContext::isInCustom() const {
    return !custom_stack_.empty();
}

std::string CompilerContext::getCurrentCustom() const {
    return custom_stack_.empty() ? "" : custom_stack_.top();
}

void CompilerContext::reportError(const std::string& message) {
    errors_.emplace_back(message, current_file_, current_line_, current_column_, 
                        getCurrentScope());
}

void CompilerContext::reportWarning(const std::string& message) {
    warnings_.emplace_back(message, current_file_, current_line_, current_column_, 
                          getCurrentScope());
}

std::vector<CompilerContext::ErrorInfo> CompilerContext::getErrors() const {
    return errors_;
}

std::vector<CompilerContext::ErrorInfo> CompilerContext::getWarnings() const {
    return warnings_;
}

bool CompilerContext::hasErrors() const {
    return !errors_.empty();
}

bool CompilerContext::hasWarnings() const {
    return !warnings_.empty();
}

void CompilerContext::clearErrors() {
    errors_.clear();
}

void CompilerContext::clearWarnings() {
    warnings_.clear();
}

void CompilerContext::pushContext(const ContextInfo& info) {
    context_stack_.push(info);
}

void CompilerContext::popContext() {
    if (context_stack_.size() > 1) {
        context_stack_.pop();
    }
}

ContextInfo CompilerContext::getCurrentContextInfo() const {
    return context_stack_.empty() ? ContextInfo() : context_stack_.top();
}

std::string CompilerContext::getContextDescription() const {
    std::ostringstream oss;
    oss << "文件: " << current_file_ << std::endl;
    oss << "位置: " << current_line_ << ":" << current_column_ << std::endl;
    oss << "作用域: " << getCurrentScope() << std::endl;
    oss << "命名空间: " << getCurrentNamespace() << std::endl;
    
    if (isInTemplate()) {
        oss << "模板: " << getCurrentTemplate() << std::endl;
    }
    
    if (isInCustom()) {
        oss << "自定义: " << getCurrentCustom() << std::endl;
    }
    
    oss << "状态: " << state_manager_.getStateDescription() << std::endl;
    oss << "上下文: " << state_manager_.getContextDescription() << std::endl;
    
    return oss.str();
}

void CompilerContext::printContextStack() const {
    std::cout << "=== 上下文栈 ===" << std::endl;
    std::stack<ContextInfo> temp_stack = context_stack_;
    std::vector<ContextInfo> contexts;
    
    while (!temp_stack.empty()) {
        contexts.push_back(temp_stack.top());
        temp_stack.pop();
    }
    
    for (int i = contexts.size() - 1; i >= 0; --i) {
        const auto& ctx = contexts[i];
        std::cout << "  " << (contexts.size() - 1 - i) << ": " 
                  << ctx.file_path << " (" << ctx.line_number << ":" << ctx.column_number << ")"
                  << " [" << ctx.current_namespace << "]";
        if (ctx.in_template) std::cout << " [模板]";
        if (ctx.in_custom) std::cout << " [自定义]";
        std::cout << std::endl;
    }
}

std::string CompilerContext::getStatistics() const {
    std::ostringstream oss;
    oss << "错误数量: " << errors_.size() << std::endl;
    oss << "警告数量: " << warnings_.size() << std::endl;
    oss << "上下文栈深度: " << context_stack_.size() << std::endl;
    oss << "模板栈深度: " << template_stack_.size() << std::endl;
    oss << "自定义栈深度: " << custom_stack_.size() << std::endl;
    oss << global_map_.getStatistics();
    return oss.str();
}

void CompilerContext::reset() {
    // 清空所有栈
    while (!context_stack_.empty()) {
        context_stack_.pop();
    }
    while (!template_stack_.empty()) {
        template_stack_.pop();
    }
    while (!custom_stack_.empty()) {
        custom_stack_.pop();
    }
    
    // 重置状态
    context_stack_.push(ContextInfo());
    current_file_.clear();
    current_line_ = 1;
    current_column_ = 1;
    
    // 清空错误和警告
    errors_.clear();
    warnings_.clear();
    
    // 重置全局映射和状态管理器
    global_map_.clear();
    state_manager_.reset();
}

std::string CompilerContext::formatErrorMessage(const std::string& message) const {
    std::ostringstream oss;
    oss << "错误: " << current_file_ << ":" << current_line_ << ":" << current_column_ 
        << " - " << message;
    return oss.str();
}

std::string CompilerContext::formatWarningMessage(const std::string& message) const {
    std::ostringstream oss;
    oss << "警告: " << current_file_ << ":" << current_line_ << ":" << current_column_ 
        << " - " << message;
    return oss.str();
}

std::unique_ptr<ContextGuard> CompilerContext::createContextGuard(const std::string& scope_name) {
    return std::make_unique<ContextGuard>(*this, scope_name);
}

std::unique_ptr<StateGuard> CompilerContext::createStateGuard(CompilerState state, SyntaxContext context) {
    return std::make_unique<StateGuard>(state_manager_, state, context);
}

} // namespace chtl