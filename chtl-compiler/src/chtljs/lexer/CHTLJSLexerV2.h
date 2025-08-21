#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace chtljs {

// CHTL JS Token类型
enum class CHTLJSTokenType {
    // 基础Token
    IDENTIFIER,
    NUMBER,
    STRING,
    
    // CHTL JS特有语法
    DOUBLE_LBRACE,      // {{
    DOUBLE_RBRACE,      // }}
    ARROW,              // ->
    
    // 关键字
    LISTEN,
    DELEGATE,
    ANIMATE,
    VIR,
    
    // 选择器相关
    DOT,                // .
    HASH,               // #
    LBRACKET,           // [
    RBRACKET,           // ]
    
    // 操作符
    LPAREN,             // (
    RPAREN,             // )
    LBRACE,             // {
    RBRACE,             // }
    COMMA,              // ,
    COLON,              // :
    SEMICOLON,          // ;
    EQUALS,             // =
    
    // 其他
    WHITESPACE,
    NEWLINE,
    COMMENT,
    EOF_TOKEN,
    ERROR
};

// CHTL JS Token
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    int line;
    int column;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// CHTL JS词法分析器
class CHTLJSLexerV2 {
public:
    CHTLJSLexerV2();
    
    // 设置输入
    void SetInput(const std::string& input, const std::string& filename = "");
    
    // 词法分析
    std::vector<CHTLJSToken> Tokenize();
    
    // 错误处理
    bool HasErrors() const { return !m_Errors.empty(); }
    const std::vector<std::string>& GetErrors() const { return m_Errors; }

private:
    std::string m_Input;
    std::string m_Filename;
    size_t m_Current;
    int m_Line;
    int m_Column;
    std::vector<std::string> m_Errors;
    
    // 扫描方法
    CHTLJSToken NextToken();
    CHTLJSToken ScanIdentifierOrKeyword();
    CHTLJSToken ScanNumber();
    CHTLJSToken ScanString();
    CHTLJSToken ScanSelector();
    
    // 辅助方法
    char Peek(int offset = 0) const;
    char Advance();
    bool IsAtEnd() const;
    bool Match(char expected);
    bool IsAlpha(char c) const;
    bool IsDigit(char c) const;
    bool IsAlphaNumeric(char c) const;
    
    void SkipWhitespace();
    void SkipComment();
    
    // 错误报告
    void ReportError(const std::string& message);
    
    // 关键字映射
    static std::unordered_map<std::string, CHTLJSTokenType> s_Keywords;
};

} // namespace chtljs