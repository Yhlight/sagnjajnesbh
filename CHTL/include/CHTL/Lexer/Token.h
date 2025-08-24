#pragma once
#include <string>
#include <vector>

namespace CHTL {
namespace Compiler {

// CHTL Token类型
enum class TokenType {
    // 字面量
    IDENTIFIER,           // 标识符
    STRING_LITERAL,       // 字符串字面量 "..." 或 '...'
    UNQUOTED_LITERAL,     // 无引号字面量
    NUMBER,               // 数字
    
    // 关键字
    KEYWORD_TEMPLATE,     // [Template]
    KEYWORD_CUSTOM,       // [Custom]
    KEYWORD_ORIGIN,       // [Origin]
    KEYWORD_IMPORT,       // [Import]
    KEYWORD_NAMESPACE,    // [Namespace]
    KEYWORD_CONFIGURATION,// [Configuration]
    KEYWORD_INFO,         // [Info]
    KEYWORD_EXPORT,       // [Export]
    KEYWORD_NAME,         // [Name]
    KEYWORD_ORIGINTYPE,   // [OriginType]
    
    // 类型标记
    AT_STYLE,             // @Style
    AT_ELEMENT,           // @Element
    AT_VAR,               // @Var
    AT_HTML,              // @Html
    AT_JAVASCRIPT,        // @JavaScript
    AT_CHTL,              // @Chtl
    AT_CJMOD,             // @CJmod
    AT_CONFIG,            // @Config
    
    // 关键字
    KEYWORD_INHERIT,      // inherit
    KEYWORD_DELETE,       // delete
    KEYWORD_INSERT,       // insert
    KEYWORD_AFTER,        // after
    KEYWORD_BEFORE,       // before
    KEYWORD_REPLACE,      // replace
    KEYWORD_AT_TOP,       // at top
    KEYWORD_AT_BOTTOM,    // at bottom
    KEYWORD_FROM,         // from
    KEYWORD_AS,           // as
    KEYWORD_EXCEPT,       // except
    KEYWORD_TEXT,         // text
    KEYWORD_STYLE,        // style
    KEYWORD_SCRIPT,       // script
    
    // 运算符和分隔符
    LEFT_BRACE,           // {
    RIGHT_BRACE,          // }
    LEFT_BRACKET,         // [
    RIGHT_BRACKET,        // ]
    LEFT_PAREN,           // (
    RIGHT_PAREN,          // )
    SEMICOLON,            // ;
    COMMA,                // ,
    COLON,                // :
    EQUALS,               // =
    DOT,                  // .
    SLASH,                // /
    STAR,                 // *
    AMPERSAND,            // &
    
    // 注释
    COMMENT,              // // 注释
    MULTI_COMMENT,        // /* */ 注释
    GENERATOR_COMMENT,    // -- 生成器注释
    
    // 特殊
    EOF_TOKEN,            // 文件结束
    ERROR_TOKEN,          // 错误token
    
    // HTML元素（任意HTML标签将被识别为IDENTIFIER，这里列出常用的）
    // 实际使用时通过标识符内容判断
};

// Token结构
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
    bool IsTypeMarker() const;
    bool IsOperator() const;
    std::string ToString() const;
};

// Token工具类
class TokenUtils {
public:
    static bool IsHTMLElement(const std::string& name);
    static bool IsKeyword(const std::string& text);
    static TokenType GetKeywordType(const std::string& text);
    static std::string TokenTypeToString(TokenType type);
};

} // namespace Compiler
} // namespace CHTL