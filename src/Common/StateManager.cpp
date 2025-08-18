#include "StateManager.h"
#include <iostream>
#include <algorithm>

namespace chtl {
namespace state {

// === RAIIStateManager实现 ===

RAIIStateManager::RAIIStateManager() : currentState_(CompilerState::INITIAL) {
    // 初始化根上下文
    ContextInfo rootContext;
    rootContext.state = CompilerState::INITIAL;
    rootContext.contextName = "root";
    rootContext.startPosition = 0;
    rootContext.currentPosition = 0;
    rootContext.nestingLevel = 0;
    rootContext.parentContext = "";
    
    contextStack_.push_back(rootContext);
}

RAIIStateManager::~RAIIStateManager() = default;

// === StateGuard实现 ===

RAIIStateManager::StateGuard::StateGuard(RAIIStateManager& manager, CompilerState newState, const std::string& contextName)
    : manager_(manager), previousState_(manager.getCurrentState()), contextName_(contextName), guardId_(manager.nextGuardId_++) {
    
    if (manager_.debugMode_) {
        std::cout << "[StateGuard " << guardId_ << "] 进入状态: " << manager_.stateToString(newState) 
                  << " (" << contextName << ")" << std::endl;
    }
    
    manager_.enterState(newState, contextName);
}

RAIIStateManager::StateGuard::~StateGuard() {
    if (manager_.debugMode_) {
        std::cout << "[StateGuard " << guardId_ << "] 退出状态: " << manager_.stateToString(manager_.getCurrentState()) 
                  << " (" << contextName_ << ")" << std::endl;
    }
    
    manager_.exitState();
}

void RAIIStateManager::StateGuard::updatePosition(size_t position) {
    if (!manager_.contextStack_.empty()) {
        manager_.contextStack_.back().currentPosition = position;
    }
}

void RAIIStateManager::StateGuard::setContextData(const std::string& key, const std::string& value) {
    if (!manager_.contextStack_.empty()) {
        manager_.contextStack_.back().contextData[key] = value;
    }
}

std::string RAIIStateManager::StateGuard::getContextData(const std::string& key) const {
    if (!manager_.contextStack_.empty()) {
        const auto& data = manager_.contextStack_.back().contextData;
        auto it = data.find(key);
        return it != data.end() ? it->second : "";
    }
    return "";
}

const ContextInfo& RAIIStateManager::StateGuard::getContext() const {
    return manager_.getCurrentContext();
}

// === 状态管理 ===

CompilerState RAIIStateManager::getCurrentState() const {
    return currentState_;
}

const ContextInfo& RAIIStateManager::getCurrentContext() const {
    return contextStack_.empty() ? contextStack_[0] : contextStack_.back();
}

const std::vector<ContextInfo>& RAIIStateManager::getContextStack() const {
    return contextStack_;
}

bool RAIIStateManager::isInState(CompilerState state) const {
    return currentState_ == state;
}

bool RAIIStateManager::isValidStateTransition(CompilerState from, CompilerState to) const {
    // 定义合法的状态转换
    // 严格按照CHTL语法文档
    
    if (from == CompilerState::ERROR_STATE) {
        return to == CompilerState::INITIAL; // 只能从错误状态回到初始状态
    }
    
    if (to == CompilerState::ERROR_STATE) {
        return true; // 任何状态都可以进入错误状态
    }
    
    // 基本的状态转换规则
    switch (from) {
        case CompilerState::INITIAL:
            return to == CompilerState::PARSING_ELEMENT ||
                   to == CompilerState::PARSING_TEMPLATE ||
                   to == CompilerState::PARSING_CUSTOM ||
                   to == CompilerState::PARSING_ORIGIN ||
                   to == CompilerState::PARSING_CONFIGURATION ||
                   to == CompilerState::PARSING_IMPORT ||
                   to == CompilerState::PARSING_NAMESPACE;
            
        case CompilerState::PARSING_ELEMENT:
            return to == CompilerState::PARSING_ATTRIBUTE ||
                   to == CompilerState::PARSING_TEXT ||
                   to == CompilerState::PARSING_STYLE_LOCAL ||
                   to == CompilerState::PARSING_SCRIPT_LOCAL ||
                   to == CompilerState::PARSING_ELEMENT || // 嵌套元素
                   to == CompilerState::COMPLETED;
            
        case CompilerState::PARSING_STYLE_LOCAL:
        case CompilerState::PARSING_STYLE_GLOBAL:
            return to == CompilerState::PARSING_ELEMENT ||
                   to == CompilerState::COMPLETED;
            
        case CompilerState::PARSING_SCRIPT_LOCAL:
        case CompilerState::PARSING_SCRIPT_GLOBAL:
            return to == CompilerState::PARSING_ELEMENT ||
                   to == CompilerState::COMPLETED;
            
        default:
            return true; // 其他状态暂时允许所有转换
    }
}

// === AST节点状态管理 ===

void RAIIStateManager::setNodeState(void* nodePtr, NodeState state) {
    if (nodePtr) {
        nodeStates_[nodePtr] = state;
        
        if (debugMode_) {
            std::cout << "[NodeState] 节点 " << nodePtr << " 状态: " << nodeStateToString(state) << std::endl;
        }
    }
}

NodeState RAIIStateManager::getNodeState(void* nodePtr) const {
    if (!nodePtr) return NodeState::ERROR;
    
    auto it = nodeStates_.find(nodePtr);
    return it != nodeStates_.end() ? it->second : NodeState::CREATED;
}

bool RAIIStateManager::isNodeInState(void* nodePtr, NodeState state) const {
    return getNodeState(nodePtr) == state;
}

// === 状态转换回调 ===

void RAIIStateManager::registerStateTransitionCallback(CompilerState from, CompilerState to, 
                                                       std::function<void(const ContextInfo&)> callback) {
    std::string key = stateToString(from) + "->" + stateToString(to);
    stateTransitionCallbacks_[key] = callback;
}

void RAIIStateManager::registerStateEnterCallback(CompilerState state, 
                                                  std::function<void(const ContextInfo&)> callback) {
    stateEnterCallbacks_[state] = callback;
}

void RAIIStateManager::registerStateExitCallback(CompilerState state, 
                                                 std::function<void(const ContextInfo&)> callback) {
    stateExitCallbacks_[state] = callback;
}

// === 错误处理 ===

void RAIIStateManager::enterErrorState(const std::string& errorMessage) {
    errors_.push_back(errorMessage);
    
    if (debugMode_) {
        std::cout << "[ERROR] " << errorMessage << std::endl;
    }
    
    // 创建错误上下文
    ContextInfo errorContext;
    errorContext.state = CompilerState::ERROR_STATE;
    errorContext.contextName = "error";
    errorContext.startPosition = getCurrentContext().currentPosition;
    errorContext.currentPosition = getCurrentContext().currentPosition;
    errorContext.nestingLevel = static_cast<int>(contextStack_.size());
    errorContext.parentContext = getCurrentContext().contextName;
    errorContext.contextData["error_message"] = errorMessage;
    
    contextStack_.push_back(errorContext);
    currentState_ = CompilerState::ERROR_STATE;
}

bool RAIIStateManager::isInErrorState() const {
    return currentState_ == CompilerState::ERROR_STATE;
}

std::vector<std::string> RAIIStateManager::getErrors() const {
    return errors_;
}

void RAIIStateManager::clearErrors() {
    errors_.clear();
    
    // 如果当前在错误状态，退出到正常状态
    if (currentState_ == CompilerState::ERROR_STATE && contextStack_.size() > 1) {
        contextStack_.pop_back();
        currentState_ = contextStack_.back().state;
    }
}

// === 调试和监控 ===

void RAIIStateManager::setDebugMode(bool enabled) {
    debugMode_ = enabled;
}

void RAIIStateManager::dumpState() const {
    std::cout << "=== 状态机状态 ===" << std::endl;
    std::cout << "当前状态: " << stateToString(currentState_) << std::endl;
    std::cout << "上下文栈深度: " << contextStack_.size() << std::endl;
    std::cout << "AST节点数量: " << nodeStates_.size() << std::endl;
    std::cout << "错误数量: " << errors_.size() << std::endl;
}

void RAIIStateManager::dumpContextStack() const {
    std::cout << "=== 上下文栈 ===" << std::endl;
    for (size_t i = 0; i < contextStack_.size(); ++i) {
        const auto& context = contextStack_[i];
        std::cout << "  [" << i << "] " << context.contextName 
                  << " (" << stateToString(context.state) << ")"
                  << " 层级:" << context.nestingLevel << std::endl;
    }
}

// === 私有方法 ===

void RAIIStateManager::enterState(CompilerState newState, const std::string& contextName) {
    // 验证状态转换
    if (!isValidStateTransition(currentState_, newState)) {
        enterErrorState("非法状态转换: " + stateToString(currentState_) + " -> " + stateToString(newState));
        return;
    }
    
    // 触发退出回调
    auto exitIt = stateExitCallbacks_.find(currentState_);
    if (exitIt != stateExitCallbacks_.end()) {
        exitIt->second(getCurrentContext());
    }
    
    // 创建新上下文
    ContextInfo newContext;
    newContext.state = newState;
    newContext.contextName = contextName;
    newContext.startPosition = getCurrentContext().currentPosition;
    newContext.currentPosition = getCurrentContext().currentPosition;
    newContext.nestingLevel = static_cast<int>(contextStack_.size());
    newContext.parentContext = getCurrentContext().contextName;
    
    // 触发转换回调
    triggerCallbacks(currentState_, newState, newContext);
    
    // 更新状态
    CompilerState previousState = currentState_;
    currentState_ = newState;
    contextStack_.push_back(newContext);
    
    // 触发进入回调
    auto enterIt = stateEnterCallbacks_.find(newState);
    if (enterIt != stateEnterCallbacks_.end()) {
        enterIt->second(getCurrentContext());
    }
}

void RAIIStateManager::exitState() {
    if (contextStack_.size() <= 1) {
        return; // 不能退出根上下文
    }
    
    // 触发退出回调
    auto exitIt = stateExitCallbacks_.find(currentState_);
    if (exitIt != stateExitCallbacks_.end()) {
        exitIt->second(getCurrentContext());
    }
    
    // 退出当前上下文
    contextStack_.pop_back();
    
    // 恢复上一个状态
    if (!contextStack_.empty()) {
        currentState_ = contextStack_.back().state;
    }
}

void RAIIStateManager::triggerCallbacks(CompilerState from, CompilerState to, const ContextInfo& context) {
    std::string key = stateToString(from) + "->" + stateToString(to);
    auto it = stateTransitionCallbacks_.find(key);
    if (it != stateTransitionCallbacks_.end()) {
        it->second(context);
    }
}

std::string RAIIStateManager::stateToString(CompilerState state) const {
    switch (state) {
        case CompilerState::INITIAL: return "INITIAL";
        case CompilerState::PARSING_ELEMENT: return "PARSING_ELEMENT";
        case CompilerState::PARSING_ATTRIBUTE: return "PARSING_ATTRIBUTE";
        case CompilerState::PARSING_TEXT: return "PARSING_TEXT";
        case CompilerState::PARSING_STYLE_LOCAL: return "PARSING_STYLE_LOCAL";
        case CompilerState::PARSING_STYLE_GLOBAL: return "PARSING_STYLE_GLOBAL";
        case CompilerState::PARSING_SCRIPT_LOCAL: return "PARSING_SCRIPT_LOCAL";
        case CompilerState::PARSING_SCRIPT_GLOBAL: return "PARSING_SCRIPT_GLOBAL";
        case CompilerState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
        case CompilerState::PARSING_CUSTOM: return "PARSING_CUSTOM";
        case CompilerState::PARSING_ORIGIN: return "PARSING_ORIGIN";
        case CompilerState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
        case CompilerState::PARSING_IMPORT: return "PARSING_IMPORT";
        case CompilerState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case CompilerState::ERROR_STATE: return "ERROR_STATE";
        case CompilerState::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

std::string RAIIStateManager::nodeStateToString(NodeState state) const {
    switch (state) {
        case NodeState::CREATED: return "CREATED";
        case NodeState::PARSING: return "PARSING";
        case NodeState::PARSED: return "PARSED";
        case NodeState::VALIDATING: return "VALIDATING";
        case NodeState::VALIDATED: return "VALIDATED";
        case NodeState::GENERATING: return "GENERATING";
        case NodeState::GENERATED: return "GENERATED";
        case NodeState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// === ContextHelper实现 ===

ContextHelper::ContextHelper(RAIIStateManager& stateManager) : stateManager_(stateManager) {
    initializeSyntaxConstraints();
}

ContextHelper::~ContextHelper() = default;

// === 解析器支持 ===

bool ContextHelper::isAllowedInCurrentContext(const std::string& syntaxElement) const {
    CompilerState currentState = stateManager_.getCurrentState();
    
    auto it = allowedSyntax_.find(currentState);
    if (it != allowedSyntax_.end()) {
        const auto& allowed = it->second;
        return std::find(allowed.begin(), allowed.end(), syntaxElement) != allowed.end();
    }
    
    // 检查是否在禁止列表中
    auto forbiddenIt = forbiddenSyntax_.find(currentState);
    if (forbiddenIt != forbiddenSyntax_.end()) {
        const auto& forbidden = forbiddenIt->second;
        return std::find(forbidden.begin(), forbidden.end(), syntaxElement) == forbidden.end();
    }
    
    return true; // 默认允许
}

std::vector<std::string> ContextHelper::getAllowedSyntaxInCurrentContext() const {
    CompilerState currentState = stateManager_.getCurrentState();
    
    auto it = allowedSyntax_.find(currentState);
    return it != allowedSyntax_.end() ? it->second : std::vector<std::string>{};
}

bool ContextHelper::validateSyntaxInContext(const std::string& syntaxElement, CompilerState context) const {
    auto it = allowedSyntax_.find(context);
    if (it != allowedSyntax_.end()) {
        const auto& allowed = it->second;
        return std::find(allowed.begin(), allowed.end(), syntaxElement) != allowed.end();
    }
    
    return true; // 默认允许
}

// === 生成器支持 ===

std::string ContextHelper::getGenerationStrategy() const {
    CompilerState currentState = stateManager_.getCurrentState();
    
    switch (currentState) {
        case CompilerState::PARSING_STYLE_LOCAL:
            return "local_style";
        case CompilerState::PARSING_STYLE_GLOBAL:
            return "global_style";
        case CompilerState::PARSING_SCRIPT_LOCAL:
            return "local_script";
        case CompilerState::PARSING_SCRIPT_GLOBAL:
            return "global_script";
        default:
            return "default";
    }
}

bool ContextHelper::needsSpecialHandling() const {
    CompilerState currentState = stateManager_.getCurrentState();
    
    return currentState == CompilerState::PARSING_STYLE_LOCAL ||
           currentState == CompilerState::PARSING_SCRIPT_LOCAL ||
           currentState == CompilerState::PARSING_TEMPLATE ||
           currentState == CompilerState::PARSING_CUSTOM;
}

std::unordered_map<std::string, std::string> ContextHelper::getContextGenerationOptions() const {
    std::unordered_map<std::string, std::string> options;
    
    CompilerState currentState = stateManager_.getCurrentState();
    const ContextInfo& context = stateManager_.getCurrentContext();
    
    options["state"] = stateManager_.stateToString(currentState);
    options["context_name"] = context.contextName;
    options["nesting_level"] = std::to_string(context.nestingLevel);
    options["parent_context"] = context.parentContext;
    
    return options;
}

// === 语法约束支持 ===

bool ContextHelper::isValidInGlobalStyle(const std::string& syntaxElement) const {
    // 全局样式块允许的语法元素（严格按照您的要求）
    std::vector<std::string> allowedInGlobalStyle = {
        "模板变量", "自定义变量", "自定义变量特例化",
        "模板样式组", "自定义样式组", "无值样式组", "自定义样式组特例化",
        "delete属性", "delete继承", "样式组间继承",
        "生成器注释", "全缀名", "任意类型原始嵌入",
        "命名空间from语法"
    };
    
    return std::find(allowedInGlobalStyle.begin(), allowedInGlobalStyle.end(), syntaxElement) != allowedInGlobalStyle.end();
}

bool ContextHelper::isValidInLocalStyle(const std::string& syntaxElement) const {
    // 局部样式块允许的语法元素（与全局样式块相同）
    return isValidInGlobalStyle(syntaxElement);
}

bool ContextHelper::isValidInLocalScript(const std::string& syntaxElement) const {
    // 局部script允许的CHTL语法元素（严格按照您的要求）
    std::vector<std::string> allowedInLocalScript = {
        "模板变量", "自定义变量组", "变量组特例化", "命名空间from",
        "注释", "任意类型原始嵌入", "{{&}}等特供语法"
    };
    
    return std::find(allowedInLocalScript.begin(), allowedInLocalScript.end(), syntaxElement) != allowedInLocalScript.end();
}

bool ContextHelper::isValidInNonLocalScript(const std::string& syntaxElement) const {
    // 除局部script外，其他script禁止使用任何CHTL语法
    // 允许例外：注释及任意类型原始嵌入
    std::vector<std::string> allowedExceptions = {
        "注释", "任意类型原始嵌入"
    };
    
    return std::find(allowedExceptions.begin(), allowedExceptions.end(), syntaxElement) != allowedExceptions.end();
}

// === 私有方法 ===

void ContextHelper::initializeSyntaxConstraints() {
    // 初始化语法约束规则
    // 严格按照CHTL语法文档和您的要求
    
    // 全局样式块允许的语法
    allowedSyntax_[CompilerState::PARSING_STYLE_GLOBAL] = {
        "模板变量", "自定义变量", "自定义变量特例化",
        "模板样式组", "自定义样式组", "无值样式组", "自定义样式组特例化",
        "delete属性", "delete继承", "样式组间继承",
        "生成器注释", "全缀名", "任意类型原始嵌入", "命名空间from语法"
    };
    
    // 局部样式块允许的语法（与全局相同）
    allowedSyntax_[CompilerState::PARSING_STYLE_LOCAL] = allowedSyntax_[CompilerState::PARSING_STYLE_GLOBAL];
    
    // 局部script允许的CHTL语法
    allowedSyntax_[CompilerState::PARSING_SCRIPT_LOCAL] = {
        "模板变量", "自定义变量组", "变量组特例化", "命名空间from",
        "注释", "任意类型原始嵌入", "{{&}}等特供语法"
    };
    
    // 非局部script禁止的语法（除了例外）
    forbiddenSyntax_[CompilerState::PARSING_SCRIPT_GLOBAL] = {
        "模板变量", "自定义变量组", "变量组特例化", "命名空间from"
        // 注释和原始嵌入不在禁止列表中，因为它们是特殊存在
    };
}

} // namespace state
} // namespace chtl