#include "chtl/token/CHTLToken.h"
#include <sstream>

namespace chtl {
namespace compiler {

bool CHTLToken::IsKeyword() const {
    return m_Type >= CHTLTokenType::KEYWORD_TEMPLATE && 
           m_Type <= CHTLTokenType::KEYWORD_VIR;
}

bool CHTLToken::IsLiteral() const {
    return m_Type == CHTLTokenType::STRING_LITERAL ||
           m_Type == CHTLTokenType::UNQUOTED_LITERAL ||
           m_Type == CHTLTokenType::NUMBER_LITERAL;
}

bool CHTLToken::IsOperator() const {
    return m_Type >= CHTLTokenType::COLON &&
           m_Type <= CHTLTokenType::ASTERISK;
}

bool CHTLToken::IsBracket() const {
    return m_Type >= CHTLTokenType::LEFT_BRACE &&
           m_Type <= CHTLTokenType::RIGHT_PAREN;
}

bool CHTLToken::IsAtKeyword() const {
    return m_Type >= CHTLTokenType::AT_STYLE &&
           m_Type <= CHTLTokenType::AT_CUSTOM_TYPE;
}

bool CHTLToken::IsCHTLStructure() const {
    return m_Type >= CHTLTokenType::KEYWORD_TEMPLATE &&
           m_Type <= CHTLTokenType::KEYWORD_EXPORT;
}

std::string CHTLToken::ToString() const {
    std::stringstream ss;
    ss << TypeToString(m_Type);
    ss << " '" << m_Lexeme << "'";
    ss << " at " << m_Location.line << ":" << m_Location.column;
    return ss.str();
}

std::string CHTLToken::TypeToString(CHTLTokenType type) {
    switch (type) {
        case CHTLTokenType::END_OF_FILE: return "END_OF_FILE";
        case CHTLTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLTokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
        
        // 方括号关键字
        case CHTLTokenType::KEYWORD_TEMPLATE: return "[Template]";
        case CHTLTokenType::KEYWORD_CUSTOM: return "[Custom]";
        case CHTLTokenType::KEYWORD_CONFIGURATION: return "[Configuration]";
        case CHTLTokenType::KEYWORD_NAMESPACE: return "[Namespace]";
        case CHTLTokenType::KEYWORD_IMPORT: return "[Import]";
        case CHTLTokenType::KEYWORD_ORIGIN: return "[Origin]";
        case CHTLTokenType::KEYWORD_INFO: return "[Info]";
        case CHTLTokenType::KEYWORD_EXPORT: return "[Export]";
        
        // @前缀关键字
        case CHTLTokenType::AT_STYLE: return "@Style";
        case CHTLTokenType::AT_ELEMENT: return "@Element";
        case CHTLTokenType::AT_VAR: return "@Var";
        case CHTLTokenType::AT_HTML: return "@Html";
        case CHTLTokenType::AT_JAVASCRIPT: return "@JavaScript";
        case CHTLTokenType::AT_CHTL: return "@Chtl";
        case CHTLTokenType::AT_CJMOD: return "@CJmod";
        case CHTLTokenType::AT_CONFIG: return "@Config";
        case CHTLTokenType::AT_VUE: return "@Vue";
        case CHTLTokenType::AT_CUSTOM_TYPE: return "@CustomType";
        
        // 普通关键字
        case CHTLTokenType::KEYWORD_TEXT: return "text";
        case CHTLTokenType::KEYWORD_STYLE: return "style";
        case CHTLTokenType::KEYWORD_SCRIPT: return "script";
        case CHTLTokenType::KEYWORD_INHERIT: return "inherit";
        case CHTLTokenType::KEYWORD_DELETE: return "delete";
        case CHTLTokenType::KEYWORD_INSERT: return "insert";
        case CHTLTokenType::KEYWORD_AFTER: return "after";
        case CHTLTokenType::KEYWORD_BEFORE: return "before";
        case CHTLTokenType::KEYWORD_REPLACE: return "replace";
        case CHTLTokenType::KEYWORD_AT_TOP: return "at top";
        case CHTLTokenType::KEYWORD_AT_BOTTOM: return "at bottom";
        case CHTLTokenType::KEYWORD_FROM: return "from";
        case CHTLTokenType::KEYWORD_AS: return "as";
        case CHTLTokenType::KEYWORD_EXCEPT: return "except";
        case CHTLTokenType::KEYWORD_VIR: return "vir";
        
        // 操作符
        case CHTLTokenType::COLON: return ":";
        case CHTLTokenType::EQUALS: return "=";
        case CHTLTokenType::SEMICOLON: return ";";
        case CHTLTokenType::DOT: return ".";
        case CHTLTokenType::COMMA: return ",";
        case CHTLTokenType::AMPERSAND: return "&";
        case CHTLTokenType::ARROW: return "->";
        case CHTLTokenType::SLASH: return "/";
        case CHTLTokenType::ASTERISK: return "*";
        
        // 括号
        case CHTLTokenType::LEFT_BRACE: return "{";
        case CHTLTokenType::RIGHT_BRACE: return "}";
        case CHTLTokenType::LEFT_BRACKET: return "[";
        case CHTLTokenType::RIGHT_BRACKET: return "]";
        case CHTLTokenType::LEFT_PAREN: return "(";
        case CHTLTokenType::RIGHT_PAREN: return ")";
        
        // 注释
        case CHTLTokenType::COMMENT_SINGLE: return "//";
        case CHTLTokenType::COMMENT_MULTI: return "/* */";
        case CHTLTokenType::COMMENT_GENERATOR: return "--";
        
        // 特殊标记
        case CHTLTokenType::ENHANCED_SELECTOR_START: return "{{";
        case CHTLTokenType::ENHANCED_SELECTOR_END: return "}}";
        
        case CHTLTokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

} // namespace compiler
} // namespace chtl