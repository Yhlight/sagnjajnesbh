#ifndef CHTL_COMMON_TOKEN_H
#define CHTL_COMMON_TOKEN_H

#include <string>
#include <variant>
#include <memory>

namespace chtl {

// Token类型枚举
enum class TokenType {
    // 通用Token
    UNKNOWN,
    EOF_TOKEN,
    
    // 字面量
    STRING_LITERAL,         // "string" 或 'string'
    UNQUOTED_LITERAL,       // 无引号字符串
    NUMBER_LITERAL,         // 数字
    
    // 标识符
    IDENTIFIER,             // 标识符
    
    // 符号
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    SEMICOLON,              // ;
    COLON,                  // :
    EQUAL,                  // =
    COMMA,                  // ,
    DOT,                    // .
    ARROW,                  // ->
    AT,                     // @
    DOUBLE_LEFT_BRACE,      // {{
    DOUBLE_RIGHT_BRACE,     // }}
    
    // 注释
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /* */
    GENERATOR_COMMENT,      // --
    
    // 关键字
    KEYWORD_TEXT,           // text
    KEYWORD_STYLE,          // style
    KEYWORD_SCRIPT,         // script
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    KEYWORD_INFO,           // [Info]
    KEYWORD_EXPORT,         // [Export]
    
    // 修饰符关键字
    KEYWORD_INHERIT,        // inherit
    KEYWORD_DELETE,         // delete
    KEYWORD_INSERT,         // insert
    KEYWORD_AFTER,          // after
    KEYWORD_BEFORE,         // before
    KEYWORD_REPLACE,        // replace
    KEYWORD_AT_TOP,         // at top
    KEYWORD_AT_BOTTOM,      // at bottom
    KEYWORD_FROM,           // from
    KEYWORD_AS,             // as
    KEYWORD_EXCEPT,         // except
    
    // 类型标识
    TYPE_STYLE,             // @Style
    TYPE_ELEMENT,           // @Element
    TYPE_VAR,               // @Var
    TYPE_HTML,              // @Html
    TYPE_JAVASCRIPT,        // @JavaScript
    TYPE_CHTL,              // @Chtl
    TYPE_CJMOD,             // @CJmod
    TYPE_CONFIG,            // @Config
    
    // HTML元素
    HTML_ELEMENT,           // div, span, etc.
    
    // CSS属性
    CSS_PROPERTY,           // CSS属性名
    CSS_VALUE,              // CSS属性值
    
    // CHTL JS特殊Token
    KEYWORD_VIR,            // vir
    KEYWORD_ANIMATE,        // animate
    KEYWORD_LISTEN,         // listen
    KEYWORD_DELEGATE,       // delegate
    
    // 原始代码块
    RAW_HTML,               // 原始HTML代码
    RAW_CSS,                // 原始CSS代码
    RAW_JAVASCRIPT,         // 原始JavaScript代码
    RAW_CUSTOM              // 自定义原始代码
};

// Token位置信息
struct TokenLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t offset;
    
    TokenLocation() : line(1), column(1), offset(0) {}
    TokenLocation(const std::string& file, size_t l, size_t c, size_t o)
        : filename(file), line(l), column(c), offset(o) {}
};

// Token值类型
using TokenValue = std::variant<
    std::monostate,    // 无值
    std::string,       // 字符串值
    double,            // 数字值
    bool               // 布尔值
>;

// Token类
class Token {
public:
    TokenType type;
    TokenValue value;
    TokenLocation location;
    std::string raw;  // 原始文本
    
    Token() : type(TokenType::UNKNOWN) {}
    
    Token(TokenType t, const TokenLocation& loc)
        : type(t), location(loc) {}
    
    Token(TokenType t, const std::string& val, const TokenLocation& loc)
        : type(t), value(val), location(loc), raw(val) {}
    
    Token(TokenType t, const TokenValue& val, const TokenLocation& loc, const std::string& r = "")
        : type(t), value(val), location(loc), raw(r) {}
    
    // 获取字符串值
    std::string getStringValue() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return "";
    }
    
    // 获取数字值
    double getNumberValue() const {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return 0.0;
    }
    
    // 获取布尔值
    bool getBoolValue() const {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return false;
    }
    
    // 判断是否是特定类型
    bool is(TokenType t) const { return type == t; }
    
    // 判断是否是关键字
    bool isKeyword() const {
        return type >= TokenType::KEYWORD_TEXT && type <= TokenType::KEYWORD_EXCEPT;
    }
    
    // 判断是否是类型标识
    bool isTypeIdentifier() const {
        return type >= TokenType::TYPE_STYLE && type <= TokenType::TYPE_CONFIG;
    }
    
    // 获取Token类型名称
    static std::string getTypeName(TokenType type);
};

// Token智能指针类型
using TokenPtr = std::shared_ptr<Token>;

} // namespace chtl

#endif // CHTL_COMMON_TOKEN_H