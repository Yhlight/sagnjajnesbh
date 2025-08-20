#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtljs {

/**
 * CHTL JS Token 类型
 * 完全独立的词法分析，不与CHTL共享
 */
enum class CHTLJSTokenType {
    // CHTL JS 特有符号
    DOUBLE_BRACE_OPEN,       // {{
    DOUBLE_BRACE_CLOSE,      // }}
    ARROW,                   // ->
    
    // 关键字
    VIR,                     // vir
    LISTEN,                  // listen
    DELEGATE,                // delegate
    ANIMATE,                 // animate
    
    // 选择器类型
    SELECTOR_CLASS,          // .classname
    SELECTOR_ID,             // #id
    SELECTOR_TAG,            // tagname
    SELECTOR_INDEX,          // [0], [1], etc
    
    // 基本符号
    LBRACE,                  // {
    RBRACE,                  // }
    LPAREN,                  // (
    RPAREN,                  // )
    LBRACKET,                // [
    RBRACKET,                // ]
    SEMICOLON,               // ;
    COLON,                   // :
    COMMA,                   // ,
    DOT,                     // .
    EQUALS,                  // =
    
    // 字面量
    STRING,                  // "..." or '...'
    NUMBER,                  // 123, 0.5
    IDENTIFIER,              // 标识符
    
    // 特殊
    EOF_TOKEN,
    ERROR
};

/**
 * CHTL JS Token
 */
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

/**
 * CHTL JS 词法分析器
 * 完全独立的实现，专门处理CHTL JS语法
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    
    void SetInput(const std::string& input, const std::string& filename = "");
    std::vector<CHTLJSToken> Tokenize();
    
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    CHTLJSToken NextToken();
    
    // 字符处理
    char CurrentChar() const;
    char NextChar() const;
    void Advance();
    bool IsAtEnd() const;
    
    // Token识别
    CHTLJSToken ScanSelector();
    CHTLJSToken ScanIdentifierOrKeyword();
    CHTLJSToken ScanString();
    CHTLJSToken ScanNumber();
    
    // 辅助方法
    void SkipWhitespace();
    void ReportError(const std::string& message);
    
    // 成员变量
    std::string m_Input;
    std::string m_Filename;
    size_t m_Current;
    size_t m_Line;
    size_t m_Column;
    std::vector<std::string> m_Errors;
};

} // namespace chtljs