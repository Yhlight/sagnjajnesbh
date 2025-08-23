#include "CHTLJS/Core/CHTLJSState.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>

namespace CHTL {
namespace CHTLJS {
namespace Core {

CHTLJSState::CHTLJSState() : currentState_(CompileState::INITIAL) {
    InitializeDefaultTransitions();
}

bool CHTLJSState::TransitionTo(CompileState newState, const std::string& context) {
    if (!IsValidTransition(currentState_, newState)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的状态转换: " + GetStateName(currentState_) + " -> " + GetStateName(newState) +
            (context.empty() ? "" : " (上下文: " + context + ")")
        );
        return false;
    }
    
    CompileState previousState = currentState_;
    currentState_ = newState;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL JS状态转换: " + GetStateName(previousState) + " -> " + GetStateName(newState)
    );
    
    return true;
}

void CHTLJSState::PushState(CompileState state) {
    stateStack_.push(currentState_);
    currentState_ = state;
}

CompileState CHTLJSState::PopState() {
    if (stateStack_.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "尝试从空状态栈弹出状态"
        );
        return currentState_;
    }
    
    CompileState poppedState = currentState_;
    currentState_ = stateStack_.top();
    stateStack_.pop();
    
    return poppedState;
}

bool CHTLJSState::IsValidTransition(CompileState fromState, CompileState toState) const {
    // 检查是否有定义的转换规则
    for (const auto& transition : validTransitions_) {
        if (transition.fromState == fromState && transition.toState == toState) {
            return true;
        }
    }
    
    // 特殊情况：错误状态可以从任何状态转换
    if (toState == CompileState::ERROR_STATE) {
        return true;
    }
    
    // 特殊情况：可以从任何状态回到初始状态
    if (toState == CompileState::INITIAL) {
        return true;
    }
    
    return false;
}

void CHTLJSState::SetContextVariable(const std::string& key, const std::string& value) {
    context_.contextVariables[key] = value;
}

std::string CHTLJSState::GetContextVariable(const std::string& key) const {
    auto it = context_.contextVariables.find(key);
    return (it != context_.contextVariables.end()) ? it->second : "";
}

void CHTLJSState::EnterVirtualObject(const std::string& objectName) {
    context_.virtualObjectStack.push_back(context_.currentVirtualObject);
    context_.currentVirtualObject = objectName;
    SetContextVariable("current_virtual_object", objectName);
}

void CHTLJSState::ExitVirtualObject() {
    if (!context_.virtualObjectStack.empty()) {
        context_.currentVirtualObject = context_.virtualObjectStack.back();
        context_.virtualObjectStack.pop_back();
        SetContextVariable("current_virtual_object", context_.currentVirtualObject);
    } else {
        context_.currentVirtualObject = "";
        SetContextVariable("current_virtual_object", "");
    }
}

void CHTLJSState::EnterSelector(const std::string& selector) {
    context_.selectorStack.push_back(context_.currentSelector);
    context_.currentSelector = selector;
    SetContextVariable("current_selector", selector);
}

void CHTLJSState::ExitSelector() {
    if (!context_.selectorStack.empty()) {
        context_.currentSelector = context_.selectorStack.back();
        context_.selectorStack.pop_back();
        SetContextVariable("current_selector", context_.currentSelector);
    } else {
        context_.currentSelector = "";
        SetContextVariable("current_selector", "");
    }
}

void CHTLJSState::Reset() {
    currentState_ = CompileState::INITIAL;
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    context_ = StateContext();
}

std::string CHTLJSState::GetStateName(CompileState state) {
    switch (state) {
        case CompileState::INITIAL: return "INITIAL";
        case CompileState::PARSING_ENHANCED_SELECTOR: return "PARSING_ENHANCED_SELECTOR";
        case CompileState::PARSING_VIRTUAL_OBJECT: return "PARSING_VIRTUAL_OBJECT";
        case CompileState::PARSING_LISTEN_BLOCK: return "PARSING_LISTEN_BLOCK";
        case CompileState::PARSING_DELEGATE_BLOCK: return "PARSING_DELEGATE_BLOCK";
        case CompileState::PARSING_ANIMATE_BLOCK: return "PARSING_ANIMATE_BLOCK";
        // 错误的状态名称已移除 - 这些状态属于JavaScript语法或CJMOD扩展
        // case CompileState::PARSING_I_NEVER_AWAY: return "PARSING_I_NEVER_AWAY"; - CJMOD扩展
        // case CompileState::PARSING_FUNCTION_DEFINITION: return "PARSING_FUNCTION_DEFINITION"; - JavaScript语法
        // case CompileState::PARSING_OBJECT_LITERAL: return "PARSING_OBJECT_LITERAL"; - JavaScript语法
        // case CompileState::PARSING_ARRAY_LITERAL: return "PARSING_ARRAY_LITERAL"; - JavaScript语法
        // case CompileState::PARSING_VOID_TYPE: return "PARSING_VOID_TYPE"; - CJMOD扩展
        // case CompileState::PARSING_VOID_STATE: return "PARSING_VOID_STATE"; - CJMOD扩展
        
        case CompileState::PARSING_ARROW_FUNCTION: return "PARSING_ARROW_FUNCTION";
        case CompileState::PARSING_EVENT_HANDLER: return "PARSING_EVENT_HANDLER";
        case CompileState::PARSING_ANIMATION_KEYFRAME: return "PARSING_ANIMATION_KEYFRAME";
        case CompileState::PARSING_CSS_IN_ANIMATION: return "PARSING_CSS_IN_ANIMATION";
        case CompileState::PARSING_SELECTOR_CONTENT: return "PARSING_SELECTOR_CONTENT";
        case CompileState::ERROR_STATE: return "ERROR_STATE";
        default: return "UNKNOWN_STATE";
    }
}

void CHTLJSState::InitializeDefaultTransitions() {
    validTransitions_ = {
        // 从初始状态的转换
        {CompileState::INITIAL, CompileState::PARSING_ENHANCED_SELECTOR, TokenType::ENHANCED_SELECTOR},
        {CompileState::INITIAL, CompileState::PARSING_VIRTUAL_OBJECT, TokenType::VIR},
        {CompileState::INITIAL, CompileState::PARSING_LISTEN_BLOCK, TokenType::LISTEN},
        {CompileState::INITIAL, CompileState::PARSING_DELEGATE_BLOCK, TokenType::DELEGATE},
        {CompileState::INITIAL, CompileState::PARSING_ANIMATE_BLOCK, TokenType::ANIMATE},
        // 错误的Token转换已移除：I_NEVER_AWAY、FUNCTION不是CHTL JS的内容
        
        // 增强选择器相关转换
        {CompileState::PARSING_ENHANCED_SELECTOR, CompileState::PARSING_SELECTOR_CONTENT, TokenType::SELECTOR_START},
        {CompileState::PARSING_SELECTOR_CONTENT, CompileState::INITIAL, TokenType::SELECTOR_END},
        
        // 虚对象相关转换
        {CompileState::PARSING_VIRTUAL_OBJECT, CompileState::PARSING_LISTEN_BLOCK, TokenType::LISTEN},
        // 虚对象相关的I_NEVER_AWAY转换已移除
        {CompileState::PARSING_VIRTUAL_OBJECT, CompileState::INITIAL, TokenType::SEMICOLON},
        
        // 监听器相关转换（移除对PARSING_OBJECT_LITERAL的错误引用）
        // {CompileState::PARSING_LISTEN_BLOCK, CompileState::PARSING_OBJECT_LITERAL, TokenType::LEFT_BRACE}, - 对象字面量是JavaScript语法
        {CompileState::PARSING_LISTEN_BLOCK, CompileState::PARSING_EVENT_HANDLER, TokenType::IDENTIFIER},
        {CompileState::PARSING_LISTEN_BLOCK, CompileState::INITIAL, TokenType::RIGHT_BRACE},
        
        // 事件委托相关转换（移除对PARSING_OBJECT_LITERAL的错误引用）
        // {CompileState::PARSING_DELEGATE_BLOCK, CompileState::PARSING_OBJECT_LITERAL, TokenType::LEFT_BRACE}, - 对象字面量是JavaScript语法
        {CompileState::PARSING_DELEGATE_BLOCK, CompileState::PARSING_EVENT_HANDLER, TokenType::TARGET},
        {CompileState::PARSING_DELEGATE_BLOCK, CompileState::INITIAL, TokenType::RIGHT_BRACE},
        
        // 动画相关转换（移除对PARSING_OBJECT_LITERAL的错误引用）
        // {CompileState::PARSING_ANIMATE_BLOCK, CompileState::PARSING_OBJECT_LITERAL, TokenType::LEFT_BRACE}, - 对象字面量是JavaScript语法
        {CompileState::PARSING_ANIMATE_BLOCK, CompileState::PARSING_ANIMATION_KEYFRAME, TokenType::WHEN},
        {CompileState::PARSING_ANIMATE_BLOCK, CompileState::PARSING_CSS_IN_ANIMATION, TokenType::BEGIN},
        {CompileState::PARSING_ANIMATE_BLOCK, CompileState::PARSING_CSS_IN_ANIMATION, TokenType::END},
        {CompileState::PARSING_ANIMATE_BLOCK, CompileState::INITIAL, TokenType::RIGHT_BRACE},
        
        // 箭头函数相关转换
        {CompileState::PARSING_ARROW_FUNCTION, CompileState::INITIAL, TokenType::SEMICOLON},
        
        // 错误的状态转换已移除：FUNCTION、I_NEVER_AWAY、VOID相关转换不是CHTL JS的内容
        
        // 动画关键帧相关转换
        {CompileState::PARSING_ANIMATION_KEYFRAME, CompileState::PARSING_CSS_IN_ANIMATION, TokenType::AT},
        {CompileState::PARSING_CSS_IN_ANIMATION, CompileState::PARSING_ANIMATION_KEYFRAME, TokenType::COMMA},
        {CompileState::PARSING_CSS_IN_ANIMATION, CompileState::PARSING_ANIMATE_BLOCK, TokenType::RIGHT_BRACE},
        
        // 事件处理器相关转换
        // 事件处理器相关的FUNCTION和OBJECT_LITERAL转换已移除 - JavaScript语法
        {CompileState::PARSING_EVENT_HANDLER, CompileState::PARSING_ARROW_FUNCTION, TokenType::ARROW},
        {CompileState::PARSING_EVENT_HANDLER, CompileState::INITIAL, TokenType::COMMA},
        
        // 通用转换（任何状态都可以转换到错误状态）
        {CompileState::INITIAL, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN},
        {CompileState::PARSING_ENHANCED_SELECTOR, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN},
        {CompileState::PARSING_VIRTUAL_OBJECT, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN},
        {CompileState::PARSING_LISTEN_BLOCK, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN},
        {CompileState::PARSING_DELEGATE_BLOCK, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN},
        {CompileState::PARSING_ANIMATE_BLOCK, CompileState::ERROR_STATE, TokenType::ERROR_TOKEN}
    };
}

// CHTLJSStateGuard实现
CHTLJSStateGuard::CHTLJSStateGuard(CHTLJSState& stateManager, CompileState targetState)
    : stateManager_(stateManager), previousState_(stateManager.GetCurrentState()), isValid_(false) {
    
    isValid_ = stateManager_.TransitionTo(targetState);
}

CHTLJSStateGuard::~CHTLJSStateGuard() {
    if (isValid_) {
        stateManager_.TransitionTo(previousState_);
    }
}

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL
