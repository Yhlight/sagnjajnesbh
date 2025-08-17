#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {

// Token位置信息
struct TokenPosition {
    size_t line;
    size_t column;
    
    TokenPosition(size_t l = 1, size_t c = 1) : line(l), column(c) {}
};

// Token类型枚举
enum class TokenType {
    // 基础类型
    INVALID = 0,
    EOF_TOKEN,
    IDENTIFIER,
    STRING,
    NUMBER,
    
    // 关键字
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    TEXT,
    STYLE,
    SCRIPT,
    INHERIT,
    DELETE,
    FROM,
    AS,
    
    // 操作符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COLON,          // :
    COMMA,          // ,
    AT,             // @
    ARROW,          // ->
    ASSIGN,         // =
    MINUS,          // -
    
    // CHTL JS特殊
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    LISTEN,
    DELEGATE,
    ANIMATE,
    VIR
};

// Token类
class Token {
public:
    TokenType type;
    std::string value;
    TokenPosition position;
    
    Token() : type(TokenType::INVALID), position(1, 1) {}
    
    Token(TokenType t, const std::string& v, const TokenPosition& pos)
        : type(t), value(v), position(pos) {}
    
    // 工具方法
    std::string toString() const;
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    
    // 比较操作
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
    
    // 静态工具方法
    static std::string typeToString(TokenType type);
    static bool isKeywordType(TokenType type);
    static bool isOperatorType(TokenType type);
    
private:
    static std::unordered_map<TokenType, std::string> type_names_;
    static void initializeTypeNames();
};

} // namespace chtl