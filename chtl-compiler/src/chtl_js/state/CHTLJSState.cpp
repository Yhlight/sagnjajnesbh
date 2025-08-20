#include "chtl_js/state/CHTLJSState.h"
#include "utils/Logger.h"
#include <sstream>
#include <vector>
#include <iostream>

namespace chtl {
namespace chtljs {

CHTLJSStateMachine::CHTLJSStateMachine() {
    // 初始状态
    m_StateStack.push(CHTLJSStateInfo(CHTLJSCompilerState::INITIAL));
}

CHTLJSStateMachine::~CHTLJSStateMachine() = default;

void CHTLJSStateMachine::PushState(CHTLJSCompilerState state, const std::string& context) {
    if (!CanTransitionTo(state)) {
        SetError("无效的CHTL JS状态转换: " + 
                std::to_string(static_cast<int>(GetCurrentState())) + 
                " -> " + std::to_string(static_cast<int>(state)));
        return;
    }
    
    m_StateStack.push(CHTLJSStateInfo(state, context));
    utils::Logger::GetInstance().Debug("CHTL JS进入状态: " + 
                                     std::to_string(static_cast<int>(state)) + 
                                     " (" + context + ")");
}

void CHTLJSStateMachine::PopState() {
    if (m_StateStack.size() > 1) {
        CHTLJSStateInfo info = m_StateStack.top();
        m_StateStack.pop();
        utils::Logger::GetInstance().Debug("CHTL JS退出状态: " + 
                                         std::to_string(static_cast<int>(info.state)) + 
                                         " (" + info.context + ")");
    }
}

CHTLJSCompilerState CHTLJSStateMachine::GetCurrentState() const {
    return m_StateStack.empty() ? CHTLJSCompilerState::ERROR : m_StateStack.top().state;
}

const CHTLJSStateInfo& CHTLJSStateMachine::GetCurrentStateInfo() const {
    static CHTLJSStateInfo errorInfo(CHTLJSCompilerState::ERROR);
    return m_StateStack.empty() ? errorInfo : m_StateStack.top();
}

bool CHTLJSStateMachine::IsInState(CHTLJSCompilerState state) const {
    std::stack<CHTLJSStateInfo> temp = m_StateStack;
    while (!temp.empty()) {
        if (temp.top().state == state) {
            return true;
        }
        temp.pop();
    }
    return false;
}

bool CHTLJSStateMachine::IsInSelectorContext() const {
    return IsInState(CHTLJSCompilerState::IN_ENHANCED_SELECTOR) ||
           IsInState(CHTLJSCompilerState::IN_SELECTOR_CLASS) ||
           IsInState(CHTLJSCompilerState::IN_SELECTOR_ID) ||
           IsInState(CHTLJSCompilerState::IN_SELECTOR_INDEX) ||
           IsInState(CHTLJSCompilerState::IN_SELECTOR_CHAIN);
}

bool CHTLJSStateMachine::IsInVirtualObjectContext() const {
    return IsInState(CHTLJSCompilerState::IN_VIRTUAL_OBJECT_DECLARATION) ||
           IsInState(CHTLJSCompilerState::IN_VIRTUAL_OBJECT_INIT);
}

bool CHTLJSStateMachine::IsInFunctionContext() const {
    return IsInState(CHTLJSCompilerState::IN_FUNCTION_CALL) ||
           IsInCHTLJSFunctionCall();
}

bool CHTLJSStateMachine::IsInCHTLJSFunctionCall() const {
    return IsInState(CHTLJSCompilerState::IN_LISTEN_CALL) ||
           IsInState(CHTLJSCompilerState::IN_DELEGATE_CALL) ||
           IsInState(CHTLJSCompilerState::IN_ANIMATE_CALL) ||
           IsInState(CHTLJSCompilerState::IN_INEVERAWAY_CALL) ||
           IsInState(CHTLJSCompilerState::IN_PRINTMYLOVE_CALL);
}

bool CHTLJSStateMachine::IsInEnhancedSelector() const {
    return IsInState(CHTLJSCompilerState::IN_ENHANCED_SELECTOR);
}

bool CHTLJSStateMachine::IsInArrowChain() const {
    return IsInState(CHTLJSCompilerState::IN_ARROW_CHAIN);
}

bool CHTLJSStateMachine::IsInObjectLiteral() const {
    return IsInState(CHTLJSCompilerState::IN_OBJECT_LITERAL);
}

bool CHTLJSStateMachine::CanTransitionTo(CHTLJSCompilerState newState) const {
    CHTLJSCompilerState currentState = GetCurrentState();
    return IsValidTransition(currentState, newState);
}

bool CHTLJSStateMachine::IsValidStateSequence() const {
    return !HasError() && GetCurrentState() != CHTLJSCompilerState::ERROR;
}

void CHTLJSStateMachine::SetError(const std::string& message) {
    m_ErrorMessage = message;
    utils::Logger::GetInstance().Error("CHTL JS状态机错误: " + message);
}

bool CHTLJSStateMachine::HasError() const {
    return !m_ErrorMessage.empty();
}

std::string CHTLJSStateMachine::GetError() const {
    return m_ErrorMessage;
}

void CHTLJSStateMachine::Reset() {
    while (!m_StateStack.empty()) {
        m_StateStack.pop();
    }
    m_StateStack.push(CHTLJSStateInfo(CHTLJSCompilerState::INITIAL));
    m_ErrorMessage.clear();
}

std::string CHTLJSStateMachine::GetStateStackTrace() const {
    std::stringstream ss;
    std::stack<CHTLJSStateInfo> temp = m_StateStack;
    std::vector<CHTLJSStateInfo> states;
    
    // 反转栈以正确顺序显示
    while (!temp.empty()) {
        states.push_back(temp.top());
        temp.pop();
    }
    
    ss << "CHTL JS状态栈追踪:\n";
    for (auto it = states.rbegin(); it != states.rend(); ++it) {
        ss << "  " << std::to_string(static_cast<int>(it->state));
        if (!it->context.empty()) {
            ss << " (" << it->context << ")";
        }
        ss << " at " << it->line << ":" << it->column << "\n";
    }
    
    return ss.str();
}

void CHTLJSStateMachine::DumpStateStack() const {
    std::cout << GetStateStackTrace() << std::endl;
}

bool CHTLJSStateMachine::IsValidTransition(CHTLJSCompilerState from, 
                                          CHTLJSCompilerState to) const {
    // 定义CHTL JS的有效状态转换规则
    switch (from) {
        case CHTLJSCompilerState::INITIAL:
            return to == CHTLJSCompilerState::IN_SCRIPT_BLOCK;
            
        case CHTLJSCompilerState::IN_SCRIPT_BLOCK:
            return to == CHTLJSCompilerState::IN_ENHANCED_SELECTOR ||
                   to == CHTLJSCompilerState::IN_VIRTUAL_OBJECT_DECLARATION ||
                   to == CHTLJSCompilerState::IN_FUNCTION_CALL ||
                   to == CHTLJSCompilerState::IN_ARROW_CHAIN;
                   
        case CHTLJSCompilerState::IN_ENHANCED_SELECTOR:
            return to == CHTLJSCompilerState::IN_SELECTOR_CLASS ||
                   to == CHTLJSCompilerState::IN_SELECTOR_ID ||
                   to == CHTLJSCompilerState::IN_SELECTOR_INDEX ||
                   to == CHTLJSCompilerState::IN_SELECTOR_CHAIN;
                   
        case CHTLJSCompilerState::IN_VIRTUAL_OBJECT_DECLARATION:
            return to == CHTLJSCompilerState::IN_VIRTUAL_OBJECT_INIT;
            
        case CHTLJSCompilerState::IN_FUNCTION_CALL:
            return to == CHTLJSCompilerState::IN_LISTEN_CALL ||
                   to == CHTLJSCompilerState::IN_DELEGATE_CALL ||
                   to == CHTLJSCompilerState::IN_ANIMATE_CALL ||
                   to == CHTLJSCompilerState::IN_INEVERAWAY_CALL ||
                   to == CHTLJSCompilerState::IN_PRINTMYLOVE_CALL ||
                   to == CHTLJSCompilerState::IN_OBJECT_LITERAL;
                   
        case CHTLJSCompilerState::IN_OBJECT_LITERAL:
            return to == CHTLJSCompilerState::IN_OBJECT_KEY ||
                   to == CHTLJSCompilerState::IN_OBJECT_VALUE ||
                   to == CHTLJSCompilerState::IN_STATE_MARKER;
                   
        // 允许从任何状态进入字符串或注释
        default:
            return to == CHTLJSCompilerState::IN_STRING_LITERAL ||
                   to == CHTLJSCompilerState::IN_COMMENT ||
                   to == CHTLJSCompilerState::ERROR;
    }
}

} // namespace chtljs
} // namespace chtl