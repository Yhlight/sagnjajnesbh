#include "../../include/common/Token.h"
#include <unordered_map>

namespace chtl {

// Token类型名称映射
static const std::unordered_map<TokenType, std::string> tokenTypeNames = {
    {TokenType::UNKNOWN, "UNKNOWN"},
    {TokenType::EOF_TOKEN, "EOF"},
    
    // 字面量
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
    
    // 标识符
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    
    // 符号
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::ARROW, "ARROW"},
    {TokenType::AT, "AT"},
    {TokenType::DOUBLE_LEFT_BRACE, "DOUBLE_LEFT_BRACE"},
    {TokenType::DOUBLE_RIGHT_BRACE, "DOUBLE_RIGHT_BRACE"},
    
    // 注释
    {TokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
    {TokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
    {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    
    // 关键字
    {TokenType::KEYWORD_TEXT, "KEYWORD_TEXT"},
    {TokenType::KEYWORD_STYLE, "KEYWORD_STYLE"},
    {TokenType::KEYWORD_SCRIPT, "KEYWORD_SCRIPT"},
    {TokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
    {TokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"},
    {TokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"},
    {TokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
    {TokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"},
    {TokenType::KEYWORD_CONFIGURATION, "KEYWORD_CONFIGURATION"},
    {TokenType::KEYWORD_INFO, "KEYWORD_INFO"},
    {TokenType::KEYWORD_EXPORT, "KEYWORD_EXPORT"},
    
    // 修饰符关键字
    {TokenType::KEYWORD_INHERIT, "KEYWORD_INHERIT"},
    {TokenType::KEYWORD_DELETE, "KEYWORD_DELETE"},
    {TokenType::KEYWORD_INSERT, "KEYWORD_INSERT"},
    {TokenType::KEYWORD_AFTER, "KEYWORD_AFTER"},
    {TokenType::KEYWORD_BEFORE, "KEYWORD_BEFORE"},
    {TokenType::KEYWORD_REPLACE, "KEYWORD_REPLACE"},
    {TokenType::KEYWORD_AT_TOP, "KEYWORD_AT_TOP"},
    {TokenType::KEYWORD_AT_BOTTOM, "KEYWORD_AT_BOTTOM"},
    {TokenType::KEYWORD_FROM, "KEYWORD_FROM"},
    {TokenType::KEYWORD_AS, "KEYWORD_AS"},
    {TokenType::KEYWORD_EXCEPT, "KEYWORD_EXCEPT"},
    
    // 类型标识
    {TokenType::TYPE_STYLE, "TYPE_STYLE"},
    {TokenType::TYPE_ELEMENT, "TYPE_ELEMENT"},
    {TokenType::TYPE_VAR, "TYPE_VAR"},
    {TokenType::TYPE_HTML, "TYPE_HTML"},
    {TokenType::TYPE_JAVASCRIPT, "TYPE_JAVASCRIPT"},
    {TokenType::TYPE_CHTL, "TYPE_CHTL"},
    {TokenType::TYPE_CJMOD, "TYPE_CJMOD"},
    {TokenType::TYPE_CONFIG, "TYPE_CONFIG"},
    
    // HTML元素
    {TokenType::HTML_ELEMENT, "HTML_ELEMENT"},
    
    // CSS属性
    {TokenType::CSS_PROPERTY, "CSS_PROPERTY"},
    {TokenType::CSS_VALUE, "CSS_VALUE"},
    
    // CHTL JS特殊Token
    {TokenType::KEYWORD_VIR, "KEYWORD_VIR"},
    {TokenType::KEYWORD_ANIMATE, "KEYWORD_ANIMATE"},
    {TokenType::KEYWORD_LISTEN, "KEYWORD_LISTEN"},
    {TokenType::KEYWORD_DELEGATE, "KEYWORD_DELEGATE"},
    
    // 原始代码块
    {TokenType::RAW_HTML, "RAW_HTML"},
    {TokenType::RAW_CSS, "RAW_CSS"},
    {TokenType::RAW_JAVASCRIPT, "RAW_JAVASCRIPT"},
    {TokenType::RAW_CUSTOM, "RAW_CUSTOM"}
};

std::string Token::getTypeName(TokenType type) {
    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

} // namespace chtl