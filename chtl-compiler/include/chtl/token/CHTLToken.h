#pragma once

#include <string>
#include <variant>

namespace chtl {
namespace compiler {

/**
 * CHTL Token 类型
 * 严格按照CHTL语法文档定义的词法单元
 */
enum class CHTLTokenType {
    // 结束标记
    END_OF_FILE,
    
    // 字面量
    STRING_LITERAL,        // "string" 或 'string'
    UNQUOTED_LITERAL,      // 无引号字面量
    NUMBER_LITERAL,        // 数字
    
    // 标识符
    IDENTIFIER,            // 标识符（元素名、属性名等）
    
    // 关键字 - 方括号关键字
    KEYWORD_TEMPLATE,      // [Template]
    KEYWORD_CUSTOM,        // [Custom]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_NAMESPACE,     // [Namespace]
    KEYWORD_IMPORT,        // [Import]
    KEYWORD_ORIGIN,        // [Origin]
    KEYWORD_INFO,          // [Info]
    KEYWORD_EXPORT,        // [Export]
    
    // 关键字 - @前缀
    AT_STYLE,              // @Style
    AT_ELEMENT,            // @Element
    AT_VAR,                // @Var
    AT_HTML,               // @Html
    AT_JAVASCRIPT,         // @JavaScript
    AT_CHTL,               // @Chtl
    AT_CJMOD,              // @CJmod
    AT_CONFIG,             // @Config
    AT_VUE,                // @Vue (自定义原始嵌入类型)
    AT_CUSTOM_TYPE,        // @自定义类型
    
    // 关键字 - 普通关键字
    KEYWORD_TEXT,          // text
    KEYWORD_STYLE,         // style
    KEYWORD_SCRIPT,        // script
    KEYWORD_INHERIT,       // inherit
    KEYWORD_DELETE,        // delete
    KEYWORD_INSERT,        // insert
    KEYWORD_AFTER,         // after
    KEYWORD_BEFORE,        // before
    KEYWORD_REPLACE,       // replace
    KEYWORD_AT_TOP,        // at top
    KEYWORD_AT_BOTTOM,     // at bottom
    KEYWORD_FROM,          // from
    KEYWORD_AS,            // as
    KEYWORD_EXCEPT,        // except
    KEYWORD_VIR,           // vir (CHTL JS关键字，但需要识别)
    
    // 操作符
    COLON,                 // :
    EQUALS,                // =
    SEMICOLON,             // ;
    DOT,                   // .
    COMMA,                 // ,
    AMPERSAND,             // &
    ARROW,                 // -> (CHTL JS操作符)
    SLASH,                 // /
    ASTERISK,              // *
    
    // 括号
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    
    // 注释
    COMMENT_SINGLE,        // // 注释
    COMMENT_MULTI,         // /* */ 注释
    COMMENT_GENERATOR,     // -- 生成器注释
    
    // 特殊标记
    ENHANCED_SELECTOR_START, // {{ (CHTL JS)
    ENHANCED_SELECTOR_END,   // }} (CHTL JS)
    
    // 错误
    INVALID
};

/**
 * Token 位置信息
 */
struct TokenLocation {
    size_t line;
    size_t column;
    size_t offset;     // 在源文件中的偏移
    size_t length;     // Token的长度
    
    TokenLocation(size_t l = 1, size_t c = 1, size_t o = 0, size_t len = 0)
        : line(l), column(c), offset(o), length(len) {}
};

/**
 * CHTL Token
 */
class CHTLToken {
public:
    using ValueType = std::variant<std::string, double, bool>;
    
    CHTLToken(CHTLTokenType type, const std::string& lexeme, 
              const TokenLocation& location)
        : m_Type(type)
        , m_Lexeme(lexeme)
        , m_Location(location) {}
    
    CHTLToken(CHTLTokenType type, const std::string& lexeme,
              const ValueType& value, const TokenLocation& location)
        : m_Type(type)
        , m_Lexeme(lexeme)
        , m_Value(value)
        , m_Location(location) {}
    
    // Getters
    CHTLTokenType GetType() const { return m_Type; }
    const std::string& GetLexeme() const { return m_Lexeme; }
    const ValueType& GetValue() const { return m_Value; }
    const TokenLocation& GetLocation() const { return m_Location; }
    
    // 便捷方法
    bool IsKeyword() const;
    bool IsLiteral() const;
    bool IsOperator() const;
    bool IsBracket() const;
    bool IsAtKeyword() const;
    bool IsCHTLStructure() const;  // [Template], [Custom]等
    
    // 获取字符串表示
    std::string ToString() const;
    static std::string TypeToString(CHTLTokenType type);
    
private:
    CHTLTokenType m_Type;
    std::string m_Lexeme;
    ValueType m_Value;
    TokenLocation m_Location;
};

} // namespace compiler
} // namespace chtl