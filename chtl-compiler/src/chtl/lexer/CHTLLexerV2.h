#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * CHTL Token 类型
 * 严格按照 CHTL 语法文档定义
 */
enum class CHTLTokenType {
    // 注释
    COMMENT_SINGLE,          // //
    COMMENT_MULTI,           // /* */
    COMMENT_HTML,            // --
    
    // 字面量
    STRING_DOUBLE,           // "..."
    STRING_SINGLE,           // '...'
    LITERAL_UNQUOTED,        // 无引号字面量
    NUMBER,                  // 数字
    
    // 标识符
    IDENTIFIER,              // 标识符
    
    // 关键字
    TEXT,                    // text
    STYLE,                   // style
    SCRIPT,                  // script
    TEMPLATE,                // [Template]
    CUSTOM,                  // [Custom]
    IMPORT,                  // [Import]
    NAMESPACE,               // [Namespace]
    CONFIGURATION,           // [Configuration]
    ORIGIN,                  // [Origin]
    EXCEPT,                  // except
    DELETE,                  // delete
    INSERT,                  // insert
    FROM,                    // from
    
    // 模板关键字
    AT_STYLE,                // @Style
    AT_ELEMENT,              // @Element
    AT_VAR,                  // @Var
    AT_HTML,                 // @Html
    AT_JAVASCRIPT,           // @JavaScript
    AT_CHTL,                 // @Chtl
    AT_CJMOD,                // @CJmod
    
    // 符号
    LBRACE,                  // {
    RBRACE,                  // }
    LPAREN,                  // (
    RPAREN,                  // )
    LBRACKET,                // [
    RBRACKET,                // ]
    SEMICOLON,               // ;
    COLON,                   // :
    EQUALS,                  // =  (CE对等式)
    DOT,                     // .
    COMMA,                   // ,
    SLASH,                   // /
    DOLLAR,                  // $
    HASH,                    // #
    AMPERSAND,               // &
    ARROW,                   // ->
    
    // 特殊
    EOF_TOKEN,
    ERROR
};

/**
 * CHTL Token
 */
struct CHTLToken {
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t length;
    
    CHTLToken(CHTLTokenType t, const std::string& v, size_t l, size_t c, size_t len = 0)
        : type(t), value(v), line(l), column(c), length(len ? len : v.length()) {}
};

/**
 * CHTL 词法分析器
 * 严格实现 CHTL 语法规范
 */
class CHTLLexerV2 {
public:
    CHTLLexerV2();
    
    /**
     * 设置输入源代码
     */
    void SetInput(const std::string& input, const std::string& filename = "");
    
    /**
     * 执行词法分析
     */
    std::vector<CHTLToken> Tokenize();
    
    /**
     * 获取下一个 Token
     */
    CHTLToken NextToken();
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 字符处理
    char CurrentChar() const;
    char NextChar() const;
    char PeekChar(size_t offset = 1) const;
    void Advance();
    void AdvanceN(size_t n);
    bool IsAtEnd() const;
    
    // 字符判断
    bool IsAlpha(char c) const;
    bool IsDigit(char c) const;
    bool IsAlphaNumeric(char c) const;
    bool IsWhitespace(char c) const;
    bool IsIdentifierStart(char c) const;
    bool IsIdentifierPart(char c) const;
    bool IsUnquotedLiteralChar(char c) const;
    
    // Token 识别
    void SkipWhitespace();
    CHTLToken ScanComment();
    CHTLToken ScanString(char quote);
    CHTLToken ScanNumber();
    CHTLToken ScanIdentifierOrKeyword();
    CHTLToken ScanUnquotedLiteral();
    CHTLToken ScanBracketKeyword();  // [Template], [Custom] 等
    CHTLToken ScanAtKeyword();        // @Style, @Element 等
    
    // 辅助方法
    void ReportError(const std::string& message);
    CHTLToken MakeToken(CHTLTokenType type, size_t startPos);
    CHTLToken MakeToken(CHTLTokenType type, const std::string& value, size_t startPos);
    
    // 成员变量
    std::string m_Input;
    std::string m_Filename;
    size_t m_Current;
    size_t m_Line;
    size_t m_Column;
    std::vector<std::string> m_Errors;
};

} // namespace chtl