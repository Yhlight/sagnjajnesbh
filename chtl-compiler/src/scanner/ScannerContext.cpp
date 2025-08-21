#include "scanner/ScannerContext.h"
#include <algorithm>

namespace chtl {

ScannerContext::ScannerContext()
    : m_CurrentLine(1)
    , m_CurrentColumn(1)
    , m_StringDelimiter('\0')
    , m_CurlyBraceCount(0)
    , m_SquareBraceCount(0)
    , m_ParenthesisCount(0) {
    m_StateStack.push(ScannerState::NORMAL);
}

void ScannerContext::PushState(ScannerState state) {
    m_StateStack.push(state);
}

void ScannerContext::PopState() {
    if (m_StateStack.size() > 1) {
        m_StateStack.pop();
    }
}

ScannerState ScannerContext::GetCurrentState() const {
    return m_StateStack.top();
}

bool ScannerContext::IsInState(ScannerState state) const {
    std::stack<ScannerState> temp = m_StateStack;
    while (!temp.empty()) {
        if (temp.top() == state) {
            return true;
        }
        temp.pop();
    }
    return false;
}

void ScannerContext::PushBrace(BraceType type, size_t line, size_t column) {
    m_BraceStack.push({type, line, column});
    
    switch (type) {
        case BraceType::CURLY:
            m_CurlyBraceCount++;
            break;
        case BraceType::SQUARE:
            m_SquareBraceCount++;
            break;
        case BraceType::PARENTHESIS:
            m_ParenthesisCount++;
            break;
    }
}

bool ScannerContext::PopBrace(BraceType type) {
    if (m_BraceStack.empty()) {
        return false;
    }
    
    if (m_BraceStack.top().type != type) {
        return false;
    }
    
    m_BraceStack.pop();
    
    switch (type) {
        case BraceType::CURLY:
            m_CurlyBraceCount--;
            break;
        case BraceType::SQUARE:
            m_SquareBraceCount--;
            break;
        case BraceType::PARENTHESIS:
            m_ParenthesisCount--;
            break;
    }
    
    return true;
}

bool ScannerContext::IsBalanced() const {
    return m_BraceStack.empty() && 
           m_CurlyBraceCount == 0 && 
           m_SquareBraceCount == 0 && 
           m_ParenthesisCount == 0;
}

int ScannerContext::GetBraceDepth(BraceType type) const {
    switch (type) {
        case BraceType::CURLY:
            return m_CurlyBraceCount;
        case BraceType::SQUARE:
            return m_SquareBraceCount;
        case BraceType::PARENTHESIS:
            return m_ParenthesisCount;
    }
    return 0;
}

void ScannerContext::UpdatePosition(char ch) {
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    } else {
        m_CurrentColumn++;
    }
}

void ScannerContext::Reset() {
    while (!m_StateStack.empty()) {
        m_StateStack.pop();
    }
    m_StateStack.push(ScannerState::NORMAL);
    
    while (!m_BraceStack.empty()) {
        m_BraceStack.pop();
    }
    
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_CurrentElementName.clear();
    m_CurrentBlockType.clear();
    m_StringDelimiter = '\0';
    m_CurlyBraceCount = 0;
    m_SquareBraceCount = 0;
    m_ParenthesisCount = 0;
}

bool ScannerContext::IsInCHTLContext() const {
    ScannerState state = GetCurrentState();
    return state == ScannerState::IN_CHTL_ELEMENT ||
           state == ScannerState::IN_TEMPLATE ||
           state == ScannerState::IN_CUSTOM ||
           state == ScannerState::IN_CONFIGURATION ||
           state == ScannerState::IN_NAMESPACE ||
           state == ScannerState::IN_IMPORT ||
           state == ScannerState::IN_TEXT_NODE;
}

bool ScannerContext::IsInCHTLJSContext() const {
    return GetCurrentState() == ScannerState::IN_LOCAL_SCRIPT;
}

bool ScannerContext::IsInCSSContext() const {
    ScannerState state = GetCurrentState();
    return state == ScannerState::IN_STYLE_BLOCK ||
           state == ScannerState::IN_LOCAL_STYLE;
}

bool ScannerContext::IsInJSContext() const {
    return GetCurrentState() == ScannerState::IN_SCRIPT_BLOCK &&
           !IsInState(ScannerState::IN_CHTL_ELEMENT);
}

bool ScannerContext::IsInHTMLContext() const {
    return GetCurrentState() == ScannerState::NORMAL &&
           !IsInCHTLContext() &&
           !IsInCHTLJSContext() &&
           !IsInCSSContext() &&
           !IsInJSContext();
}

} // namespace chtl