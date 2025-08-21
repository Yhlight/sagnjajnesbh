#pragma once

#include <string>
#include <variant>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS Token 类型
 * 严格按照CHTL语法文档定义的CHTL JS词法单元
 * 注意：CHTL JS不是JavaScript，它只处理CHTL JS特有的语法
 */
enum class CHTLJSTokenType {
    // 结束标记
    END_OF_FILE,
    
    // 标识符
    IDENTIFIER,            // 标识符
    
    // 字面量
    STRING_LITERAL,        // 字符串字面量
    NUMBER_LITERAL,        // 数字字面量
    
    // CHTL JS 特殊关键字
    KEYWORD_VIR,           // vir - 虚对象
    
    // CHTL JS 函数名
    FUNCTION_LISTEN,       // listen
    FUNCTION_DELEGATE,     // delegate
    FUNCTION_ANIMATE,      // animate
    FUNCTION_INEVERAWAY,   // iNeverAway
    FUNCTION_PRINTMYLOVE,  // printMylove
    
    // 增强选择器
    ENHANCED_SELECTOR_START,  // {{
    ENHANCED_SELECTOR_END,    // }}
    SELECTOR_CLASS,           // . (在选择器内)
    SELECTOR_ID,              // # (在选择器内)
    SELECTOR_INDEX,           // [数字] (在选择器内)
    SELECTOR_AMPERSAND,       // & (在选择器内)
    
    // 操作符
    ARROW,                 // -> (CHTL JS推荐的成员访问)
    DOT,                   // . (等价于->，但不推荐)
    EQUALS,                // =
    COLON,                 // :
    SEMICOLON,             // ;
    COMMA,                 // ,
    
    // 括号
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    LEFT_ANGLE,            // < (用于状态标记)
    RIGHT_ANGLE,           // > (用于状态标记)
    
    // 特殊标记
    FUNCTION_TYPE,         // Void等函数类型标记
    STATE_MARKER,          // <A>, <B>等状态标记
    
    // 错误
    INVALID
};

/**
 * Token 位置信息
 */
struct CHTLJSTokenLocation {
    size_t line;
    size_t column;
    size_t offset;
    size_t length;
    
    CHTLJSTokenLocation(size_t l = 1, size_t c = 1, size_t o = 0, size_t len = 0)
        : line(l), column(c), offset(o), length(len) {}
};

/**
 * CHTL JS Token
 * 表示CHTL JS代码中的词法单元
 */
class CHTLJSToken {
public:
    using ValueType = std::variant<std::string, double, bool>;
    
    CHTLJSToken(CHTLJSTokenType type, const std::string& lexeme,
                const CHTLJSTokenLocation& location)
        : m_Type(type)
        , m_Lexeme(lexeme)
        , m_Location(location) {}
    
    CHTLJSToken(CHTLJSTokenType type, const std::string& lexeme,
                const ValueType& value, const CHTLJSTokenLocation& location)
        : m_Type(type)
        , m_Lexeme(lexeme)
        , m_Value(value)
        , m_Location(location) {}
    
    // Getters
    CHTLJSTokenType GetType() const { return m_Type; }
    const std::string& GetLexeme() const { return m_Lexeme; }
    const ValueType& GetValue() const { return m_Value; }
    const CHTLJSTokenLocation& GetLocation() const { return m_Location; }
    
    // 便捷方法
    bool IsFunction() const;
    bool IsSelector() const;
    bool IsOperator() const;
    bool IsBracket() const;
    
    // 获取字符串表示
    std::string ToString() const;
    static std::string TypeToString(CHTLJSTokenType type);
    
private:
    CHTLJSTokenType m_Type;
    std::string m_Lexeme;
    ValueType m_Value;
    CHTLJSTokenLocation m_Location;
};

} // namespace chtljs
} // namespace chtl