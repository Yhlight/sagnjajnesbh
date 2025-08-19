#include "compilers/chtl/chtl_state.h"
#include <algorithm>

namespace chtl {

CHTLStateManager::CHTLStateManager() {
    // 初始状态
    stateStack_.push(StateContext(CHTLCompilerState::INITIAL, 1, 1));
}

CHTLStateManager::~CHTLStateManager() = default;

void CHTLStateManager::enterState(CHTLCompilerState state, size_t line, size_t column, 
                                  const std::string& info) {
    // 验证状态转换
    if (!canTransitionTo(state)) {
        // 记录非法状态转换，但仍然允许（可能需要错误恢复）
        StateContext errorContext(CHTLCompilerState::ERROR, line, column, 
                                "Invalid transition from " + stateToString(getCurrentState()) + 
                                " to " + stateToString(state));
        stateHistory_.push_back(errorContext);
    }
    
    StateContext newContext(state, line, column, info);
    stateStack_.push(newContext);
    stateHistory_.push_back(newContext);
}

void CHTLStateManager::exitState() {
    if (stateStack_.size() > 1) {  // 保留初始状态
        stateStack_.pop();
    }
}

CHTLCompilerState CHTLStateManager::getCurrentState() const {
    if (stateStack_.empty()) {
        return CHTLCompilerState::INITIAL;
    }
    return stateStack_.top().state;
}

const StateContext* CHTLStateManager::getCurrentContext() const {
    if (stateStack_.empty()) {
        return nullptr;
    }
    return &stateStack_.top();
}

bool CHTLStateManager::isInState(CHTLCompilerState state) const {
    return getCurrentState() == state;
}

bool CHTLStateManager::isInAnyState(const std::vector<CHTLCompilerState>& states) const {
    CHTLCompilerState current = getCurrentState();
    return std::find(states.begin(), states.end(), current) != states.end();
}

bool CHTLStateManager::canTransitionTo(CHTLCompilerState newState) const {
    return validateTransition(getCurrentState(), newState);
}

void CHTLStateManager::reset() {
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    stateHistory_.clear();
    stateStack_.push(StateContext(CHTLCompilerState::INITIAL, 1, 1));
}

int CHTLStateManager::getElementDepth() const {
    int depth = 0;
    std::stack<StateContext> temp = stateStack_;
    
    while (!temp.empty()) {
        if (temp.top().state == CHTLCompilerState::IN_ELEMENT) {
            depth++;
        }
        temp.pop();
    }
    
    return depth;
}

bool CHTLStateManager::isInsideElement() const {
    return getElementDepth() > 0;
}

bool CHTLStateManager::validateTransition(CHTLCompilerState from, CHTLCompilerState to) const {
    // 定义合法的状态转换规则
    switch (from) {
        case CHTLCompilerState::INITIAL:
            // 从初始状态可以进入任何顶级结构
            return to == CHTLCompilerState::IN_ELEMENT ||
                   to == CHTLCompilerState::IN_TEMPLATE ||
                   to == CHTLCompilerState::IN_CUSTOM ||
                   to == CHTLCompilerState::IN_ORIGIN ||
                   to == CHTLCompilerState::IN_IMPORT ||
                   to == CHTLCompilerState::IN_NAMESPACE ||
                   to == CHTLCompilerState::IN_CONFIGURATION ||
                   to == CHTLCompilerState::IN_COMMENT ||
                   to == CHTLCompilerState::IN_STYLE_BLOCK;  // 全局style
                   
        case CHTLCompilerState::IN_ELEMENT:
            // 在元素内可以有子元素、文本、样式、脚本、属性
            return to == CHTLCompilerState::IN_ELEMENT ||
                   to == CHTLCompilerState::IN_TEXT_BLOCK ||
                   to == CHTLCompilerState::IN_STYLE_BLOCK ||
                   to == CHTLCompilerState::IN_SCRIPT_BLOCK ||
                   to == CHTLCompilerState::IN_ATTRIBUTE ||
                   to == CHTLCompilerState::IN_COMMENT;
                   
        case CHTLCompilerState::IN_ATTRIBUTE:
            // 属性后面是属性值
            return to == CHTLCompilerState::IN_ATTRIBUTE_VALUE ||
                   to == CHTLCompilerState::IN_STRING;
                   
        case CHTLCompilerState::IN_STYLE_BLOCK:
            // 样式块内可以有CSS规则
            return to == CHTLCompilerState::IN_CSS_RULE ||
                   to == CHTLCompilerState::IN_CSS_SELECTOR ||
                   to == CHTLCompilerState::IN_COMMENT;
                   
        case CHTLCompilerState::IN_TEXT_BLOCK:
            // 文本块内只能有字符串
            return to == CHTLCompilerState::IN_STRING;
            
        default:
            // 其他情况暂时都允许（简化处理）
            return true;
    }
}

std::string stateToString(CHTLCompilerState state) {
    switch (state) {
        case CHTLCompilerState::INITIAL: return "INITIAL";
        case CHTLCompilerState::IN_ELEMENT: return "IN_ELEMENT";
        case CHTLCompilerState::IN_TEXT_BLOCK: return "IN_TEXT_BLOCK";
        case CHTLCompilerState::IN_STYLE_BLOCK: return "IN_STYLE_BLOCK";
        case CHTLCompilerState::IN_SCRIPT_BLOCK: return "IN_SCRIPT_BLOCK";
        case CHTLCompilerState::IN_TEMPLATE: return "IN_TEMPLATE";
        case CHTLCompilerState::IN_CUSTOM: return "IN_CUSTOM";
        case CHTLCompilerState::IN_ORIGIN: return "IN_ORIGIN";
        case CHTLCompilerState::IN_IMPORT: return "IN_IMPORT";
        case CHTLCompilerState::IN_NAMESPACE: return "IN_NAMESPACE";
        case CHTLCompilerState::IN_CONFIGURATION: return "IN_CONFIGURATION";
        case CHTLCompilerState::IN_ATTRIBUTE: return "IN_ATTRIBUTE";
        case CHTLCompilerState::IN_ATTRIBUTE_VALUE: return "IN_ATTRIBUTE_VALUE";
        case CHTLCompilerState::IN_CSS_RULE: return "IN_CSS_RULE";
        case CHTLCompilerState::IN_CSS_SELECTOR: return "IN_CSS_SELECTOR";
        case CHTLCompilerState::IN_CSS_DECLARATION: return "IN_CSS_DECLARATION";
        case CHTLCompilerState::IN_STRING: return "IN_STRING";
        case CHTLCompilerState::IN_COMMENT: return "IN_COMMENT";
        case CHTLCompilerState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace chtl