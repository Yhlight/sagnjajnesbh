#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>
#include <ostream>

namespace chtl {

/**
 * CHTL Token类型定义
 * 严格按照CHTL语法文档定义
 */
enum class CHTLTokenType {
    // 字面量
    STRING_LITERAL,         // "..." 双引号字符串
    SINGLE_STRING_LITERAL,  // '...' 单引号字符串
    UNQUOTED_LITERAL,       // 无引号字面量
    NUMBER,                 // 数字
    
    // 标识符
    IDENTIFIER,             // 标识符（元素名、属性名等）
    
    // 文本和元素
    KEYWORD_TEXT,           // text
    KEYWORD_STYLE,          // style
    KEYWORD_SCRIPT,         // script
    
    // 模板和自定义
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    KEYWORD_TEMPLATE,       // Template
    KEYWORD_CUSTOM,         // Custom
    KEYWORD_ORIGIN,         // Origin
    KEYWORD_IMPORT,         // Import
    KEYWORD_NAMESPACE,      // Namespace
    KEYWORD_CONFIGURATION,  // Configuration
    KEYWORD_INFO,           // Info
    KEYWORD_EXPORT,         // Export
    
    // 模板/自定义类型标识
    AT_STYLE,               // @Style
    AT_ELEMENT,             // @Element
    AT_VAR,                 // @Var
    AT_HTML,                // @Html
    AT_JAVASCRIPT,          // @JavaScript
    AT_CHTL,                // @Chtl
    AT_CJMOD,               // @CJmod
    
    // 操作关键字
    KEYWORD_INHERIT,        // inherit
    KEYWORD_DELETE,         // delete
    KEYWORD_INSERT,         // insert
    KEYWORD_EXCEPT,         // except
    KEYWORD_FROM,           // from
    KEYWORD_AS,             // as
    
    // 插入位置关键字
    KEYWORD_AFTER,          // after
    KEYWORD_BEFORE,         // before
    KEYWORD_REPLACE,        // replace
    KEYWORD_AT,             // at
    KEYWORD_TOP,            // top
    KEYWORD_BOTTOM,         // bottom
    
    // 操作符
    COLON,                  // :
    EQUALS,                 // = (CE对等式: : 与 = 完全等价)
    SEMICOLON,              // ;
    COMMA,                  // ,
    DOT,                    // .
    AMPERSAND,              // &
    AT,                     // @
    
    // 括号
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    
    // 注释
    SINGLE_LINE_COMMENT,    // // 注释
    MULTI_LINE_COMMENT,     // /* */ 注释
    GENERATOR_COMMENT,      // -- 生成器识别的注释
    
    // 特殊标记
    NEWLINE,                // 换行符
    WHITESPACE,             // 空白字符
    EOF_TOKEN,              // 文件结束
    
    // 错误标记
    ERROR                   // 错误token
};

/**
 * CHTL Token结构
 */
struct CHTLToken {
    CHTLTokenType type;
    std::string lexeme;                                          // 原始文本
    std::variant<std::monostate, std::string, double, int> value; // 值
    size_t line;
    size_t column;
    size_t startPos;  // 在源文件中的起始位置
    size_t endPos;    // 在源文件中的结束位置
    
    CHTLToken(CHTLTokenType type, const std::string& lexeme, 
              size_t line, size_t column, size_t startPos, size_t endPos)
        : type(type), lexeme(lexeme), line(line), column(column), 
          startPos(startPos), endPos(endPos) {}
    
    template<typename T>
    CHTLToken(CHTLTokenType type, const std::string& lexeme, T value,
              size_t line, size_t column, size_t startPos, size_t endPos)
        : type(type), lexeme(lexeme), value(value), line(line), column(column),
          startPos(startPos), endPos(endPos) {}
    
    // 获取字符串值
    std::string getStringValue() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return lexeme;
    }
    
    // 获取数字值
    double getNumberValue() const {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        return 0.0;
    }
    
    // 检查是否是特定类型
    bool is(CHTLTokenType t) const { return type == t; }
    
    // 检查是否是关键字
    bool isKeyword() const {
        return type >= CHTLTokenType::KEYWORD_TEXT && 
               type <= CHTLTokenType::KEYWORD_BOTTOM;
    }
    
    // 检查是否是操作符
    bool isOperator() const {
        return type >= CHTLTokenType::COLON && 
               type <= CHTLTokenType::AT;
    }
    
    // 检查是否是字面量
    bool isLiteral() const {
        return type >= CHTLTokenType::STRING_LITERAL && 
               type <= CHTLTokenType::NUMBER;
    }
};

/**
 * 获取Token类型的字符串表示
 */
std::string chtlTokenTypeToString(CHTLTokenType type);

/**
 * Token调试输出
 */
std::ostream& operator<<(std::ostream& os, const CHTLToken& token);

} // namespace chtl

#endif // CHTL_TOKEN_H