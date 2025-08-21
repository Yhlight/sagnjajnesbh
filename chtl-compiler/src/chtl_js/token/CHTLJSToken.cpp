#include "chtl_js/token/CHTLJSToken.h"
#include <sstream>

namespace chtl {
namespace chtljs {

bool CHTLJSToken::IsFunction() const {
    return m_Type >= CHTLJSTokenType::FUNCTION_LISTEN &&
           m_Type <= CHTLJSTokenType::FUNCTION_PRINTMYLOVE;
}

bool CHTLJSToken::IsSelector() const {
    return m_Type >= CHTLJSTokenType::ENHANCED_SELECTOR_START &&
           m_Type <= CHTLJSTokenType::SELECTOR_AMPERSAND;
}

bool CHTLJSToken::IsOperator() const {
    return m_Type >= CHTLJSTokenType::ARROW &&
           m_Type <= CHTLJSTokenType::COMMA;
}

bool CHTLJSToken::IsBracket() const {
    return m_Type >= CHTLJSTokenType::LEFT_BRACE &&
           m_Type <= CHTLJSTokenType::RIGHT_ANGLE;
}

std::string CHTLJSToken::ToString() const {
    std::stringstream ss;
    ss << TypeToString(m_Type);
    ss << " '" << m_Lexeme << "'";
    ss << " at " << m_Location.line << ":" << m_Location.column;
    return ss.str();
}

std::string CHTLJSToken::TypeToString(CHTLJSTokenType type) {
    switch (type) {
        case CHTLJSTokenType::END_OF_FILE: return "END_OF_FILE";
        case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLJSTokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        
        // CHTL JS 关键字
        case CHTLJSTokenType::KEYWORD_VIR: return "vir";
        
        // CHTL JS 函数
        case CHTLJSTokenType::FUNCTION_LISTEN: return "listen";
        case CHTLJSTokenType::FUNCTION_DELEGATE: return "delegate";
        case CHTLJSTokenType::FUNCTION_ANIMATE: return "animate";
        case CHTLJSTokenType::FUNCTION_INEVERAWAY: return "iNeverAway";
        case CHTLJSTokenType::FUNCTION_PRINTMYLOVE: return "printMylove";
        
        // 增强选择器
        case CHTLJSTokenType::ENHANCED_SELECTOR_START: return "{{";
        case CHTLJSTokenType::ENHANCED_SELECTOR_END: return "}}";
        case CHTLJSTokenType::SELECTOR_CLASS: return ".";
        case CHTLJSTokenType::SELECTOR_ID: return "#";
        case CHTLJSTokenType::SELECTOR_INDEX: return "[index]";
        case CHTLJSTokenType::SELECTOR_AMPERSAND: return "&";
        
        // 操作符
        case CHTLJSTokenType::ARROW: return "->";
        case CHTLJSTokenType::DOT: return ".";
        case CHTLJSTokenType::EQUALS: return "=";
        case CHTLJSTokenType::COLON: return ":";
        case CHTLJSTokenType::SEMICOLON: return ";";
        case CHTLJSTokenType::COMMA: return ",";
        
        // 括号
        case CHTLJSTokenType::LEFT_BRACE: return "{";
        case CHTLJSTokenType::RIGHT_BRACE: return "}";
        case CHTLJSTokenType::LEFT_BRACKET: return "[";
        case CHTLJSTokenType::RIGHT_BRACKET: return "]";
        case CHTLJSTokenType::LEFT_PAREN: return "(";
        case CHTLJSTokenType::RIGHT_PAREN: return ")";
        case CHTLJSTokenType::LEFT_ANGLE: return "<";
        case CHTLJSTokenType::RIGHT_ANGLE: return ">";
        
        // 特殊标记
        case CHTLJSTokenType::FUNCTION_TYPE: return "FUNCTION_TYPE";
        case CHTLJSTokenType::STATE_MARKER: return "STATE_MARKER";
        
        case CHTLJSTokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

} // namespace chtljs
} // namespace chtl