#include "compilers/chtl/chtl_token.h"
#include <sstream>

namespace chtl {

std::string chtlTokenTypeToString(CHTLTokenType type) {
    switch (type) {
        // 字面量
        case CHTLTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLTokenType::SINGLE_STRING_LITERAL: return "SINGLE_STRING_LITERAL";
        case CHTLTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLTokenType::NUMBER: return "NUMBER";
        
        // 标识符
        case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
        
        // 文本和元素
        case CHTLTokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case CHTLTokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case CHTLTokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        
        // 模板和自定义
        case CHTLTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLTokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case CHTLTokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case CHTLTokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case CHTLTokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case CHTLTokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case CHTLTokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case CHTLTokenType::KEYWORD_INFO: return "KEYWORD_INFO";
        case CHTLTokenType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        
        // 模板/自定义类型标识
        case CHTLTokenType::AT_STYLE: return "AT_STYLE";
        case CHTLTokenType::AT_ELEMENT: return "AT_ELEMENT";
        case CHTLTokenType::AT_VAR: return "AT_VAR";
        case CHTLTokenType::AT_HTML: return "AT_HTML";
        case CHTLTokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case CHTLTokenType::AT_CHTL: return "AT_CHTL";
        case CHTLTokenType::AT_CJMOD: return "AT_CJMOD";
        
        // 操作关键字
        case CHTLTokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case CHTLTokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case CHTLTokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case CHTLTokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case CHTLTokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case CHTLTokenType::KEYWORD_AS: return "KEYWORD_AS";
        
        // 插入位置关键字
        case CHTLTokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case CHTLTokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case CHTLTokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case CHTLTokenType::KEYWORD_AT: return "KEYWORD_AT";
        case CHTLTokenType::KEYWORD_TOP: return "KEYWORD_TOP";
        case CHTLTokenType::KEYWORD_BOTTOM: return "KEYWORD_BOTTOM";
        
        // 操作符
        case CHTLTokenType::COLON: return "COLON";
        case CHTLTokenType::EQUALS: return "EQUALS";
        case CHTLTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLTokenType::COMMA: return "COMMA";
        case CHTLTokenType::DOT: return "DOT";
        case CHTLTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLTokenType::AT: return "AT";
        
        // 括号
        case CHTLTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        
        // 注释
        case CHTLTokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case CHTLTokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case CHTLTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        // 特殊标记
        case CHTLTokenType::NEWLINE: return "NEWLINE";
        case CHTLTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLTokenType::EOF_TOKEN: return "EOF_TOKEN";
        
        // 错误标记
        case CHTLTokenType::ERROR: return "ERROR";
        
        default: return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const CHTLToken& token) {
    os << "[" << chtlTokenTypeToString(token.type) << "] ";
    os << "'" << token.lexeme << "' ";
    os << "at " << token.line << ":" << token.column;
    
    // 如果有值，输出值
    if (std::holds_alternative<std::string>(token.value)) {
        os << " (value: \"" << std::get<std::string>(token.value) << "\")";
    } else if (std::holds_alternative<double>(token.value)) {
        os << " (value: " << std::get<double>(token.value) << ")";
    } else if (std::holds_alternative<int>(token.value)) {
        os << " (value: " << std::get<int>(token.value) << ")";
    }
    
    return os;
}

} // namespace chtl