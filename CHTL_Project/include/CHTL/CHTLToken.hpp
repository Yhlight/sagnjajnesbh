#pragma once

#include "../Core/CHTLTypes.hpp"

namespace CHTL {

/**
 * @brief CHTL Token类型枚举
 * 
 * 根据CHTL语法文档定义的所有Token类型
 */
enum class CHTLTokenType {
    // 基础Token
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量 "..." '...'
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER,              // 数字
    
    // 分隔符
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COMMA,               // ,
    DOT,                 // .
    
    // 运算符
    COLON,               // :
    EQUALS,              // =
    AT_SYMBOL,           // @
    AMPERSAND,           // &
    
    // 关键字
    TEXT,                // text
    STYLE,               // style
    SCRIPT,              // script
    INHERIT,             // inherit
    DELETE,              // delete
    INSERT,              // insert
    AFTER,               // after
    BEFORE,              // before
    REPLACE,             // replace
    AT_TOP,              // at top
    AT_BOTTOM,           // at bottom
    FROM,                // from
    AS,                  // as
    EXCEPT,              // except
    
    // 块类型关键字
    TEMPLATE,            // [Template]
    CUSTOM,              // [Custom]
    ORIGIN,              // [Origin]
    IMPORT,              // [Import]
    CONFIGURATION,       // [Configuration]
    NAMESPACE,           // [Namespace]
    INFO,                // [Info]
    EXPORT,              // [Export]
    
    // 类型标识符
    STYLE_TYPE,          // @Style
    ELEMENT_TYPE,        // @Element
    VAR_TYPE,            // @Var
    HTML_TYPE,           // @Html
    JAVASCRIPT_TYPE,     // @JavaScript
    CHTL_TYPE,           // @Chtl
    CJMOD_TYPE,          // @CJmod
    CONFIG_TYPE,         // @Config
    
    // 注释
    LINE_COMMENT,        // //
    BLOCK_COMMENT,       // /* */
    GENERATOR_COMMENT,   // --
    
    // HTML元素
    HTML_ELEMENT,        // html, div, span, etc.
    
    // 特殊Token
    NEWLINE,             // 换行符
    WHITESPACE,          // 空白字符
    EOF_TOKEN,           // 文件结束
    ERROR_TOKEN,         // 错误Token
    
    // CHTL JS相关Token
    CHTLJS_SELECTOR,     // {{selector}}
    CHTLJS_ARROW,        // ->
    CHTLJS_LISTEN,       // listen
    CHTLJS_DELEGATE,     // delegate
    CHTLJS_ANIMATE,      // animate
    CHTLJS_VIR,          // vir
    
    UNKNOWN              // 未知Token
};

/**
 * @brief CHTL Token结构
 */
struct CHTLToken {
    CHTLTokenType type;
    String value;
    SourceLocation location;
    
    CHTLToken(CHTLTokenType t = CHTLTokenType::UNKNOWN, 
              const String& v = "", 
              const SourceLocation& loc = {})
        : type(t), value(v), location(loc) {}
    
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsLiteral() const;
    bool IsComment() const;
    String ToString() const;
};

/**
 * @brief Token类型到字符串的转换
 */
String TokenTypeToString(CHTLTokenType type);

/**
 * @brief 关键字映射表
 */
class CHTLKeywordMap {
public:
    CHTLKeywordMap();
    
    CHTLTokenType GetKeywordType(const String& keyword) const;
    bool IsKeyword(const String& word) const;
    void AddCustomKeyword(const String& keyword, CHTLTokenType type);
    
private:
    StringMap keywordMap_;
};

} // namespace CHTL