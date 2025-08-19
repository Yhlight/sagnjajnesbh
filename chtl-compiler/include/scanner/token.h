#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace chtl {

/**
 * Token类型枚举
 */
enum class TokenType {
    // 字面量
    TEXT,                   // 文本
    STRING_LITERAL,         // 字符串字面量（双引号）
    SINGLE_STRING_LITERAL,  // 字符串字面量（单引号）
    UNQUOTED_LITERAL,       // 无引号字面量
    NUMBER,                 // 数字
    
    // 标识符和关键字
    IDENTIFIER,             // 标识符
    KEYWORD_TEXT,           // text关键字
    KEYWORD_STYLE,          // style关键字
    KEYWORD_SCRIPT,         // script关键字
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    KEYWORD_INFO,           // [Info]
    KEYWORD_EXPORT,         // [Export]
    KEYWORD_INHERIT,        // inherit
    KEYWORD_DELETE,         // delete
    KEYWORD_INSERT,         // insert
    KEYWORD_EXCEPT,         // except
    KEYWORD_FROM,           // from
    KEYWORD_AS,             // as
    KEYWORD_VIR,            // vir
    
    // 操作符
    COLON,                  // :
    SEMICOLON,              // ;
    COMMA,                  // ,
    DOT,                    // .
    ARROW,                  // ->
    AT,                     // @
    AMPERSAND,              // &
    EQUALS,                 // =
    
    // 括号
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    
    // 注释
    COMMENT,                // 注释
    GENERATOR_COMMENT,      // 生成器注释 --
    
    // 特殊标记
    DOUBLE_LEFT_BRACE,      // {{
    DOUBLE_RIGHT_BRACE,     // }}
    
    // 结束标记
    EOF_TOKEN,              // 文件结束
    
    // 错误标记
    ERROR                   // 错误
};

/**
 * Token结构体
 */
struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, std::string, double, int> literal;
    size_t line;
    size_t column;
    
    Token(TokenType type, const std::string& lexeme, size_t line, size_t column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
    
    template<typename T>
    Token(TokenType type, const std::string& lexeme, T literal, size_t line, size_t column)
        : type(type), lexeme(lexeme), literal(literal), line(line), column(column) {}
};

/**
 * 获取Token类型的字符串表示
 */
std::string tokenTypeToString(TokenType type);

} // namespace chtl

#endif // CHTL_TOKEN_H