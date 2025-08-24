#pragma once
#include <string>
#include <vector>

namespace CHTL {
namespace JSCompiler {

// CHTL JS Token类型
enum class TokenType {
    // 增强选择器
    SELECTOR_START,       // {{
    SELECTOR_END,         // }}
    CLASS_SELECTOR,       // . (在选择器内)
    ID_SELECTOR,          // # (在选择器内)
    
    // 操作符
    ARROW,                // ->
    DOT,                  // .
    LEFT_BRACKET,         // [
    RIGHT_BRACKET,        // ]
    LEFT_PAREN,           // (
    RIGHT_PAREN,          // )
    LEFT_BRACE,           // {
    RIGHT_BRACE,          // }
    COMMA,                // ,
    COLON,                // :
    SEMICOLON,            // ;
    EQUALS,               // =
    
    // CHTL JS 关键字
    KEYWORD_VIR,          // vir
    KEYWORD_LISTEN,       // listen
    KEYWORD_DELEGATE,     // delegate
    KEYWORD_ANIMATE,      // animate
    KEYWORD_INEVERAWAY,   // iNeverAway
    
    // 动画相关关键字
    KEYWORD_TARGET,       // target
    KEYWORD_DURATION,     // duration
    KEYWORD_EASING,       // easing
    KEYWORD_BEGIN,        // begin
    KEYWORD_WHEN,         // when
    KEYWORD_END,          // end
    KEYWORD_AT,           // at
    KEYWORD_LOOP,         // loop
    KEYWORD_DIRECTION,    // direction
    KEYWORD_DELAY,        // delay
    KEYWORD_CALLBACK,     // callback
    
    // 字面量
    IDENTIFIER,           // 标识符
    STRING_LITERAL,       // 字符串
    NUMBER_LITERAL,       // 数字
    
    // JavaScript代码片段
    JS_CODE,              // 普通JS代码
    
    // 特殊
    EOF_TOKEN,            // 文件结束
    ERROR_TOKEN           // 错误token
};

// CHTL JS Token结构
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    
    Token(TokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c), endLine(l), endColumn(c + v.length()) {}
    
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t el, size_t ec)
        : type(t), value(v), line(l), column(c), endLine(el), endColumn(ec) {}
    
    bool IsKeyword() const;
    bool IsOperator() const;
    std::string ToString() const;
};

// Token工具类
class TokenUtils {
public:
    static bool IsKeyword(const std::string& text);
    static TokenType GetKeywordType(const std::string& text);
    static std::string TokenTypeToString(TokenType type);
};

} // namespace JSCompiler
} // namespace CHTL