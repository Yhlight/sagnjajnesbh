#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace chtl {

// Token类型枚举
enum class TokenType {
    // 基础Token
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量
    UNQUOTED_LITERAL,   // 无引号字面量
    NUMBER,             // 数字
    
    // 符号
    LBRACE,             // {
    RBRACE,             // }
    LPAREN,             // (
    RPAREN,             // )
    LBRACKET,           // [
    RBRACKET,           // ]
    SEMICOLON,          // ;
    COLON,              // :
    COMMA,              // ,
    DOT,                // .
    EQUAL,              // =
    ARROW,              // ->
    AMPERSAND,          // &
    
    // CHTL关键字
    TEXT,               // text
    STYLE,              // style
    SCRIPT,             // script
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    
    // 操作关键字
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    FROM,               // from
    AS,                 // as
    EXCEPT,             // except
    
    // CHTL JS特殊语法
    DOUBLE_LBRACE,      // {{
    DOUBLE_RBRACE,      // }}
    LISTEN,             // listen
    DELEGATE,           // delegate
    ANIMATE,            // animate
    VIR,                // vir
    
    // 注释
    SINGLE_COMMENT,     // //
    MULTI_COMMENT,      // /* */
    GENERATOR_COMMENT,  // --
    
    // 特殊
    NEWLINE,            // \n
    WHITESPACE,         // 空白字符
    EOF_TOKEN,          // 文件结束
    INVALID             // 无效Token
};

// Token位置信息
struct TokenPosition {
    size_t line;
    size_t column;
    size_t offset;
    
    TokenPosition(size_t l = 0, size_t c = 0, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

// Token类
class Token {
public:
    TokenType type;
    std::string value;
    TokenPosition position;
    
    Token(TokenType t, const std::string& v, const TokenPosition& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const std::string& v)
        : type(t), value(v), position() {}
    
    Token() : type(TokenType::INVALID), value(""), position() {}
    
    // 判断是否为关键字类型
    bool isKeyword() const;
    
    // 判断是否为操作符
    bool isOperator() const;
    
    // 判断是否为字面量
    bool isLiteral() const;
    
    // 获取Token类型的字符串表示
    std::string getTypeString() const;
    
    // 判断两个Token是否相等
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
};

// Token工厂类
class TokenFactory {
public:
    // 创建关键字映射
    static void initKeywordMap();
    
    // 根据字符串创建Token
    static Token createToken(const std::string& text, const TokenPosition& pos);
    
    // 判断字符串是否为关键字
    static bool isKeyword(const std::string& text);
    
    // 获取关键字对应的TokenType
    static TokenType getKeywordType(const std::string& text);
    
private:
    static std::unordered_map<std::string, TokenType> keywordMap;
    static bool initialized;
};

} // namespace chtl