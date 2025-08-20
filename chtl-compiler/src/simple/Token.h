#pragma once

#include <string>

namespace chtl {
namespace simple {

/**
 * Token 类型枚举
 */
enum class TokenType {
    // 标识符和字面值
    IDENTIFIER,     // div, text, class
    STRING,         // "content"
    NUMBER,         // 123
    
    // 标点符号
    LBRACE,        // {
    RBRACE,        // }
    COLON,         // :
    SEMICOLON,     // ;
    
    // 注释
    COMMENT,
    
    // 特殊
    EOF_TOKEN,
    ERROR
};

/**
 * Token 结构体
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t = TokenType::EOF_TOKEN, const std::string& v = "", 
          size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
        
    /**
     * 获取 Token 类型的字符串表示
     */
    std::string GetTypeName() const {
        switch (type) {
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::STRING: return "STRING";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::LBRACE: return "LBRACE";
            case TokenType::RBRACE: return "RBRACE";
            case TokenType::COLON: return "COLON";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::COMMENT: return "COMMENT";
            case TokenType::EOF_TOKEN: return "EOF";
            case TokenType::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
};

} // namespace simple
} // namespace chtl