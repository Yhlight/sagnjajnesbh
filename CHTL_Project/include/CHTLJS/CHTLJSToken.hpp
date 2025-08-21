#pragma once

#include "../Core/CHTLTypes.hpp"

namespace CHTLJS {

/**
 * @brief CHTL JS Token类型枚举
 * 
 * 根据CHTL语法文档中CHTL JS部分定义的所有Token类型
 * 完全独立于CHTL的Token系统
 */
enum class CHTLJSTokenType {
    // 基础Token
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量 "..." '...'
    NUMBER,              // 数字
    BOOLEAN,             // 布尔值
    
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
    
    // CHTL JS特有运算符
    ARROW,               // ->  (CHTL JS特有)
    COLON,               // :
    EQUALS,              // =
    
    // CHTL JS增强选择器
    ENHANCED_SELECTOR,   // {{selector}}
    CLASS_SELECTOR,      // {{.class}}
    ID_SELECTOR,         // {{#id}}
    TAG_SELECTOR,        // {{tag}}
    DESCENDANT_SELECTOR, // {{.class tag}}
    INDEXED_SELECTOR,    // {{selector[index]}}
    
    // CHTL JS关键字
    LISTEN,              // listen
    DELEGATE,            // delegate
    ANIMATE,             // animate
    VIR,                 // vir
    TARGET,              // target
    CLICK,               // click
    MOUSEENTER,          // mouseenter
    MOUSELEAVE,          // mouseleave
    MOUSEMOVE,           // mousemove
    
    // 动画相关关键字
    DURATION,            // duration
    EASING,              // easing
    BEGIN,               // begin
    WHEN,                // when
    END,                 // end
    AT,                  // at
    LOOP,                // loop
    DIRECTION,           // direction
    DELAY,               // delay
    CALLBACK,            // callback
    
    // 缓动函数
    EASE_IN,             // ease-in
    EASE_OUT,            // ease-out
    EASE_IN_OUT,         // ease-in-out
    LINEAR,              // linear
    
    // JavaScript原生关键字（在CHTL JS中使用）
    FUNCTION,            // function
    CONST,               // const
    LET,                 // let
    VAR,                 // var
    IF,                  // if
    ELSE,                // else
    FOR,                 // for
    WHILE,               // while
    RETURN,              // return
    TRUE,                // true
    FALSE,               // false
    NULL,                // null
    UNDEFINED,           // undefined
    
    // 运算符
    PLUS,                // +
    MINUS,               // -
    MULTIPLY,            // *
    DIVIDE,              // /
    MODULO,              // %
    ASSIGN,              // =
    PLUS_ASSIGN,         // +=
    MINUS_ASSIGN,        // -=
    EQUAL,               // ==
    NOT_EQUAL,           // !=
    STRICT_EQUAL,        // ===
    STRICT_NOT_EQUAL,    // !==
    LESS_THAN,           // <
    GREATER_THAN,        // >
    LESS_EQUAL,          // <=
    GREATER_EQUAL,       // >=
    LOGICAL_AND,         // &&
    LOGICAL_OR,          // ||
    LOGICAL_NOT,         // !
    
    // 特殊Token
    NEWLINE,             // 换行符
    WHITESPACE,          // 空白字符
    EOF_TOKEN,           // 文件结束
    ERROR_TOKEN,         // 错误Token
    
    // 注释
    LINE_COMMENT,        // //
    BLOCK_COMMENT,       // /* */
    
    UNKNOWN              // 未知Token
};

/**
 * @brief CHTL JS Token结构
 */
struct CHTLJSToken {
    CHTLJSTokenType type;
    CHTL::String value;
    CHTL::SourceLocation location;
    
    CHTLJSToken(CHTLJSTokenType t = CHTLJSTokenType::UNKNOWN, 
                const CHTL::String& v = "", 
                const CHTL::SourceLocation& loc = {})
        : type(t), value(v), location(loc) {}
    
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsLiteral() const;
    bool IsSelector() const;
    bool IsComment() const;
    CHTL::String ToString() const;
};

/**
 * @brief Token类型到字符串的转换
 */
CHTL::String TokenTypeToString(CHTLJSTokenType type);

/**
 * @brief CHTL JS关键字映射表
 */
class CHTLJSKeywordMap {
public:
    CHTLJSKeywordMap();
    
    CHTLJSTokenType GetKeywordType(const CHTL::String& keyword) const;
    bool IsKeyword(const CHTL::String& word) const;
    bool IsEventType(const CHTL::String& word) const;
    bool IsAnimationProperty(const CHTL::String& word) const;
    void AddCustomKeyword(const CHTL::String& keyword, CHTLJSTokenType type);
    
private:
    CHTL::StringMap keywordMap_;
    CHTL::StringSet eventTypes_;
    CHTL::StringSet animationProperties_;
};

} // namespace CHTLJS