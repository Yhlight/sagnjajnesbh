#include "chtl/state/CHTLState.h"
#include "utils/Logger.h"
#include <sstream>
#include <vector>
#include <iostream>

namespace chtl {
namespace compiler {

CHTLStateMachine::CHTLStateMachine() {
    // 初始状态
    m_StateStack.push(CHTLStateInfo(CHTLCompilerState::INITIAL));
}

CHTLStateMachine::~CHTLStateMachine() = default;

void CHTLStateMachine::PushState(CHTLCompilerState state, const std::string& context) {
    if (!CanTransitionTo(state)) {
        SetError("无效的状态转换: " + std::to_string(static_cast<int>(GetCurrentState())) + 
                " -> " + std::to_string(static_cast<int>(state)));
        return;
    }
    
    m_StateStack.push(CHTLStateInfo(state, context));
    utils::Logger::GetInstance().Debug("进入状态: " + std::to_string(static_cast<int>(state)) + 
                                     " (" + context + ")");
}

void CHTLStateMachine::PopState() {
    if (m_StateStack.size() > 1) {
        CHTLStateInfo info = m_StateStack.top();
        m_StateStack.pop();
        utils::Logger::GetInstance().Debug("退出状态: " + std::to_string(static_cast<int>(info.state)) + 
                                         " (" + info.context + ")");
    }
}

CHTLCompilerState CHTLStateMachine::GetCurrentState() const {
    return m_StateStack.empty() ? CHTLCompilerState::ERROR : m_StateStack.top().state;
}

const CHTLStateInfo& CHTLStateMachine::GetCurrentStateInfo() const {
    static CHTLStateInfo errorInfo(CHTLCompilerState::ERROR);
    return m_StateStack.empty() ? errorInfo : m_StateStack.top();
}

bool CHTLStateMachine::IsInState(CHTLCompilerState state) const {
    std::stack<CHTLStateInfo> temp = m_StateStack;
    while (!temp.empty()) {
        if (temp.top().state == state) {
            return true;
        }
        temp.pop();
    }
    return false;
}

bool CHTLStateMachine::IsInElementContext() const {
    return IsInState(CHTLCompilerState::IN_ELEMENT) ||
           IsInState(CHTLCompilerState::IN_ELEMENT_BODY);
}

bool CHTLStateMachine::IsInTemplateContext() const {
    return IsInState(CHTLCompilerState::IN_TEMPLATE_DECLARATION) ||
           IsInState(CHTLCompilerState::IN_TEMPLATE_STYLE) ||
           IsInState(CHTLCompilerState::IN_TEMPLATE_ELEMENT) ||
           IsInState(CHTLCompilerState::IN_TEMPLATE_VAR);
}

bool CHTLStateMachine::IsInCustomContext() const {
    return IsInState(CHTLCompilerState::IN_CUSTOM_DECLARATION) ||
           IsInState(CHTLCompilerState::IN_CUSTOM_STYLE) ||
           IsInState(CHTLCompilerState::IN_CUSTOM_ELEMENT) ||
           IsInState(CHTLCompilerState::IN_CUSTOM_VAR);
}

bool CHTLStateMachine::IsInStyleContext() const {
    return IsInState(CHTLCompilerState::IN_LOCAL_STYLE) ||
           IsInState(CHTLCompilerState::IN_LOCAL_STYLE_SELECTOR) ||
           IsInState(CHTLCompilerState::IN_LOCAL_STYLE_RULE);
}

bool CHTLStateMachine::IsInScriptContext() const {
    return IsInState(CHTLCompilerState::IN_LOCAL_SCRIPT);
}

bool CHTLStateMachine::CanTransitionTo(CHTLCompilerState newState) const {
    CHTLCompilerState currentState = GetCurrentState();
    return IsValidTransition(currentState, newState);
}

bool CHTLStateMachine::IsValidStateSequence() const {
    return !HasError() && GetCurrentState() != CHTLCompilerState::ERROR;
}

void CHTLStateMachine::SetError(const std::string& message) {
    m_ErrorMessage = message;
    utils::Logger::GetInstance().Error(message);
}

bool CHTLStateMachine::HasError() const {
    return !m_ErrorMessage.empty();
}

std::string CHTLStateMachine::GetError() const {
    return m_ErrorMessage;
}

void CHTLStateMachine::Reset() {
    while (!m_StateStack.empty()) {
        m_StateStack.pop();
    }
    m_StateStack.push(CHTLStateInfo(CHTLCompilerState::INITIAL));
    m_ErrorMessage.clear();
}

std::string CHTLStateMachine::GetStateStackTrace() const {
    std::stringstream ss;
    std::stack<CHTLStateInfo> temp = m_StateStack;
    std::vector<CHTLStateInfo> states;
    
    // 反转栈以正确顺序显示
    while (!temp.empty()) {
        states.push_back(temp.top());
        temp.pop();
    }
    
    ss << "状态栈追踪:\n";
    for (auto it = states.rbegin(); it != states.rend(); ++it) {
        ss << "  " << std::to_string(static_cast<int>(it->state));
        if (!it->context.empty()) {
            ss << " (" << it->context << ")";
        }
        ss << " at " << it->line << ":" << it->column << "\n";
    }
    
    return ss.str();
}

void CHTLStateMachine::DumpStateStack() const {
    std::cout << GetStateStackTrace() << std::endl;
}

bool CHTLStateMachine::IsValidTransition(CHTLCompilerState from, CHTLCompilerState to) const {
    // 定义有效的状态转换规则
    switch (from) {
        case CHTLCompilerState::INITIAL:
            return to == CHTLCompilerState::TOP_LEVEL;
            
        case CHTLCompilerState::TOP_LEVEL:
            return to == CHTLCompilerState::IN_ELEMENT ||
                   to == CHTLCompilerState::IN_TEMPLATE_DECLARATION ||
                   to == CHTLCompilerState::IN_CUSTOM_DECLARATION ||
                   to == CHTLCompilerState::IN_CONFIGURATION ||
                   to == CHTLCompilerState::IN_NAMESPACE ||
                   to == CHTLCompilerState::IN_IMPORT ||
                   to == CHTLCompilerState::IN_ORIGIN_BLOCK;
                   
        case CHTLCompilerState::IN_ELEMENT:
            return to == CHTLCompilerState::IN_ELEMENT_BODY ||
                   to == CHTLCompilerState::IN_ATTRIBUTE;
                   
        case CHTLCompilerState::IN_ELEMENT_BODY:
            return to == CHTLCompilerState::IN_ELEMENT ||
                   to == CHTLCompilerState::IN_TEXT_NODE ||
                   to == CHTLCompilerState::IN_LOCAL_STYLE ||
                   to == CHTLCompilerState::IN_LOCAL_SCRIPT ||
                   to == CHTLCompilerState::IN_ATTRIBUTE ||
                   to == CHTLCompilerState::IN_EXCEPT_CLAUSE;
                   
        case CHTLCompilerState::IN_TEMPLATE_DECLARATION:
            return to == CHTLCompilerState::IN_TEMPLATE_STYLE ||
                   to == CHTLCompilerState::IN_TEMPLATE_ELEMENT ||
                   to == CHTLCompilerState::IN_TEMPLATE_VAR;
                   
        case CHTLCompilerState::IN_CUSTOM_DECLARATION:
            return to == CHTLCompilerState::IN_CUSTOM_STYLE ||
                   to == CHTLCompilerState::IN_CUSTOM_ELEMENT ||
                   to == CHTLCompilerState::IN_CUSTOM_VAR;
                   
        case CHTLCompilerState::IN_LOCAL_STYLE:
            return to == CHTLCompilerState::IN_LOCAL_STYLE_SELECTOR ||
                   to == CHTLCompilerState::IN_LOCAL_STYLE_RULE;
                   
        case CHTLCompilerState::IN_LOCAL_STYLE_SELECTOR:
            return to == CHTLCompilerState::IN_LOCAL_STYLE_RULE;
            
        // 允许从任何状态进入字符串或注释
        default:
            return to == CHTLCompilerState::IN_STRING_LITERAL ||
                   to == CHTLCompilerState::IN_COMMENT ||
                   to == CHTLCompilerState::ERROR;
    }
}

} // namespace compiler
} // namespace chtl