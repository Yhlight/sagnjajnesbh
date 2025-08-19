#include "CHTLState.h"
#include <iostream>
#include <sstream>

namespace chtl {

StateManager::StateManager() {
    // 初始化为空闲状态
    state_stack_.push(StateInfo(CompilerState::IDLE, SyntaxContext::GLOBAL));
    initializeValidTransitions();
}

CompilerState StateManager::getCurrentState() const {
    if (state_stack_.empty()) return CompilerState::ERROR_STATE;
    return state_stack_.top().state;
}

SyntaxContext StateManager::getCurrentContext() const {
    if (state_stack_.empty()) return SyntaxContext::GLOBAL;
    return state_stack_.top().context;
}

bool StateManager::pushState(CompilerState new_state, SyntaxContext context) {
    CompilerState current_state = getCurrentState();
    
    // 检查状态转换是否有效
    if (!canTransition(new_state, context)) {
        setErrorState("无效的状态转换: " + stateToString(current_state) + 
                     " -> " + stateToString(new_state));
        return false;
    }
    
    // 推入新状态
    state_stack_.push(StateInfo(new_state, context));
    
    // 记录状态转换历史
    state_history_.emplace_back(current_state, new_state, context, "", "pushState");
    
    return true;
}

bool StateManager::popState() {
    if (state_stack_.size() <= 1) {
        setErrorState("无法弹出根状态");
        return false;
    }
    
    CompilerState old_state = getCurrentState();
    SyntaxContext old_context = getCurrentContext();
    
    state_stack_.pop();
    
    CompilerState new_state = getCurrentState();
    SyntaxContext new_context = getCurrentContext();
    
    // 记录状态转换历史
    state_history_.emplace_back(old_state, new_state, new_context, "", "popState");
    
    return true;
}

bool StateManager::canTransition(CompilerState to_state, SyntaxContext context) const {
    CompilerState current_state = getCurrentState();
    return isValidTransition(current_state, to_state, context);
}

bool StateManager::transition(CompilerState to_state, SyntaxContext context, const std::string& trigger) {
    CompilerState current_state = getCurrentState();
    
    if (!canTransition(to_state, context)) {
        setErrorState("无效的状态转换: " + stateToString(current_state) + 
                     " -> " + stateToString(to_state) + " (触发器: " + trigger + ")");
        return false;
    }
    
    // 替换当前状态
    if (!state_stack_.empty()) {
        state_stack_.pop();
    }
    state_stack_.push(StateInfo(to_state, context, trigger));
    
    // 记录状态转换历史
    state_history_.emplace_back(current_state, to_state, context, trigger, "transition");
    
    return true;
}

bool StateManager::isInState(CompilerState state) const {
    return getCurrentState() == state;
}

bool StateManager::isInContext(SyntaxContext context) const {
    return getCurrentContext() == context;
}

bool StateManager::isInGlobalScope() const {
    return getCurrentContext() == SyntaxContext::GLOBAL;
}

bool StateManager::isInLocalScope() const {
    SyntaxContext context = getCurrentContext();
    return context == SyntaxContext::LOCAL_STYLE || 
           context == SyntaxContext::LOCAL_SCRIPT ||
           context == SyntaxContext::ELEMENT_DEFINITION;
}

std::vector<StateTransition> StateManager::getStateHistory() const {
    return state_history_;
}

void StateManager::clearHistory() {
    state_history_.clear();
}

void StateManager::setErrorState(const std::string& error_message) {
    last_error_ = error_message;
    if (!state_stack_.empty()) {
        state_stack_.pop();
    }
    state_stack_.push(StateInfo(CompilerState::ERROR_STATE, SyntaxContext::GLOBAL, error_message));
}

bool StateManager::isInErrorState() const {
    return getCurrentState() == CompilerState::ERROR_STATE;
}

std::string StateManager::getLastError() const {
    return last_error_;
}

void StateManager::clearError() {
    last_error_.clear();
    if (isInErrorState()) {
        // 恢复到空闲状态
        if (!state_stack_.empty()) {
            state_stack_.pop();
        }
        if (state_stack_.empty()) {
            state_stack_.push(StateInfo(CompilerState::IDLE, SyntaxContext::GLOBAL));
        }
    }
}

bool StateManager::isValidStateForSyntax(CompilerState state, const std::string& syntax_element) const {
    // 根据语法元素检查状态是否有效
    if (syntax_element.find("@") == 0) {
        // @开头的元素（类型标识符）
        return state == CompilerState::PARSING_CHTL || 
               state == CompilerState::PARSING_TEMPLATE ||
               state == CompilerState::PARSING_CUSTOM ||
               state == CompilerState::PARSING_IMPORT;
    }
    
    if (syntax_element == "text") {
        return state == CompilerState::PARSING_CHTL;
    }
    
    if (syntax_element == "style") {
        return state == CompilerState::PARSING_CHTL || 
               state == CompilerState::PARSING_STYLE_BLOCK;
    }
    
    if (syntax_element == "script") {
        return state == CompilerState::PARSING_CHTL || 
               state == CompilerState::PARSING_SCRIPT_BLOCK;
    }
    
    return true; // 默认允许
}

bool StateManager::isValidContextForOperation(SyntaxContext context, const std::string& operation) const {
    // 根据操作检查上下文是否有效
    if (operation == "template_variable") {
        return context != SyntaxContext::COMMENT_CONTENT;
    }
    
    if (operation == "chtl_js_selector") {
        return context == SyntaxContext::LOCAL_SCRIPT || 
               context == SyntaxContext::CHTL_JS_EXPRESSION;
    }
    
    if (operation == "css_property") {
        return context == SyntaxContext::LOCAL_STYLE || 
               context == SyntaxContext::GLOBAL;
    }
    
    return true; // 默认允许
}

std::string StateManager::getStateDescription() const {
    return stateToString(getCurrentState());
}

std::string StateManager::getContextDescription() const {
    return contextToString(getCurrentContext());
}

void StateManager::printStateStack() const {
    std::cout << "=== 状态栈 ===" << std::endl;
    std::stack<StateInfo> temp_stack = state_stack_;
    std::vector<StateInfo> states;
    
    while (!temp_stack.empty()) {
        states.push_back(temp_stack.top());
        temp_stack.pop();
    }
    
    for (int i = states.size() - 1; i >= 0; --i) {
        std::cout << "  " << (states.size() - 1 - i) << ": " 
                  << stateToString(states[i].state) << " (" 
                  << contextToString(states[i].context) << ")";
        if (!states[i].entry_trigger.empty()) {
            std::cout << " [" << states[i].entry_trigger << "]";
        }
        std::cout << std::endl;
    }
}

void StateManager::reset() {
    // 清空状态栈
    while (!state_stack_.empty()) {
        state_stack_.pop();
    }
    
    // 重置为初始状态
    state_stack_.push(StateInfo(CompilerState::IDLE, SyntaxContext::GLOBAL));
    
    // 清空历史和错误
    state_history_.clear();
    last_error_.clear();
}

void StateManager::initializeValidTransitions() {
    // 定义有效的状态转换
    valid_transitions_[CompilerState::IDLE] = {
        CompilerState::PARSING_CHTL,
        CompilerState::PARSING_TEMPLATE,
        CompilerState::PARSING_CUSTOM,
        CompilerState::PARSING_IMPORT,
        CompilerState::PARSING_NAMESPACE,
        CompilerState::PARSING_CONFIGURATION
    };
    
    valid_transitions_[CompilerState::PARSING_CHTL] = {
        CompilerState::PARSING_STYLE_BLOCK,
        CompilerState::PARSING_SCRIPT_BLOCK,
        CompilerState::PARSING_TEXT_BLOCK,
        CompilerState::PARSING_COMMENT,
        CompilerState::PARSING_TEMPLATE,
        CompilerState::PARSING_CUSTOM,
        CompilerState::PARSING_ORIGIN,
        CompilerState::IDLE
    };
    
    valid_transitions_[CompilerState::PARSING_STYLE_BLOCK] = {
        CompilerState::PARSING_CSS,
        CompilerState::PARSING_CHTL,
        CompilerState::PARSING_COMMENT
    };
    
    valid_transitions_[CompilerState::PARSING_SCRIPT_BLOCK] = {
        CompilerState::PARSING_JAVASCRIPT,
        CompilerState::PARSING_CHTL_JS,
        CompilerState::PARSING_CHTL,
        CompilerState::PARSING_COMMENT
    };
    
    valid_transitions_[CompilerState::PARSING_TEMPLATE] = {
        CompilerState::PARSING_CHTL,
        CompilerState::IDLE
    };
    
    valid_transitions_[CompilerState::PARSING_CUSTOM] = {
        CompilerState::PARSING_CHTL,
        CompilerState::IDLE
    };
    
    valid_transitions_[CompilerState::PARSING_ORIGIN] = {
        CompilerState::PARSING_HTML,
        CompilerState::PARSING_CSS,
        CompilerState::PARSING_JAVASCRIPT,
        CompilerState::PARSING_CHTL
    };
    
    // 所有状态都可以转换到错误状态
    for (auto& pair : valid_transitions_) {
        pair.second.push_back(CompilerState::ERROR_STATE);
    }
}

bool StateManager::isValidTransition(CompilerState from, CompilerState to, SyntaxContext context) const {
    auto it = valid_transitions_.find(from);
    if (it == valid_transitions_.end()) {
        return false;
    }
    
    const std::vector<CompilerState>& valid_states = it->second;
    return std::find(valid_states.begin(), valid_states.end(), to) != valid_states.end();
}

std::string StateManager::stateToString(CompilerState state) const {
    switch (state) {
        case CompilerState::IDLE: return "IDLE";
        case CompilerState::PARSING_CHTL: return "PARSING_CHTL";
        case CompilerState::PARSING_CHTL_JS: return "PARSING_CHTL_JS";
        case CompilerState::PARSING_CSS: return "PARSING_CSS";
        case CompilerState::PARSING_JAVASCRIPT: return "PARSING_JAVASCRIPT";
        case CompilerState::PARSING_HTML: return "PARSING_HTML";
        case CompilerState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
        case CompilerState::PARSING_CUSTOM: return "PARSING_CUSTOM";
        case CompilerState::PARSING_ORIGIN: return "PARSING_ORIGIN";
        case CompilerState::PARSING_IMPORT: return "PARSING_IMPORT";
        case CompilerState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case CompilerState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
        case CompilerState::PARSING_STYLE_BLOCK: return "PARSING_STYLE_BLOCK";
        case CompilerState::PARSING_SCRIPT_BLOCK: return "PARSING_SCRIPT_BLOCK";
        case CompilerState::PARSING_TEXT_BLOCK: return "PARSING_TEXT_BLOCK";
        case CompilerState::PARSING_COMMENT: return "PARSING_COMMENT";
        case CompilerState::ERROR_STATE: return "ERROR_STATE";
        default: return "UNKNOWN";
    }
}

std::string StateManager::contextToString(SyntaxContext context) const {
    switch (context) {
        case SyntaxContext::GLOBAL: return "GLOBAL";
        case SyntaxContext::LOCAL_STYLE: return "LOCAL_STYLE";
        case SyntaxContext::LOCAL_SCRIPT: return "LOCAL_SCRIPT";
        case SyntaxContext::TEMPLATE_DEFINITION: return "TEMPLATE_DEFINITION";
        case SyntaxContext::CUSTOM_DEFINITION: return "CUSTOM_DEFINITION";
        case SyntaxContext::NAMESPACE_DEFINITION: return "NAMESPACE_DEFINITION";
        case SyntaxContext::IMPORT_STATEMENT: return "IMPORT_STATEMENT";
        case SyntaxContext::ELEMENT_DEFINITION: return "ELEMENT_DEFINITION";
        case SyntaxContext::ATTRIBUTE_DEFINITION: return "ATTRIBUTE_DEFINITION";
        case SyntaxContext::TEXT_CONTENT: return "TEXT_CONTENT";
        case SyntaxContext::COMMENT_CONTENT: return "COMMENT_CONTENT";
        case SyntaxContext::ORIGIN_BLOCK: return "ORIGIN_BLOCK";
        case SyntaxContext::CHTL_JS_EXPRESSION: return "CHTL_JS_EXPRESSION";
        default: return "UNKNOWN";
    }
}

} // namespace chtl