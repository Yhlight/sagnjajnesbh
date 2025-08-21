#ifndef CHTL_LEXER_V2_H
#define CHTL_LEXER_V2_H

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

namespace chtl {

// Token类型
enum class CHTLTokenType {
    // 结束符
    EOF_TOKEN,
    ERROR,
    
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
    AS,                      // as
    BEFORE,                  // before
    AFTER,                   // after
    REPLACE,                 // replace
    AT,                      // at
    
    // 模板关键字
    AT_STYLE,                // @Style
    AT_ELEMENT,              // @Element
    AT_VAR,                  // @Var
    AT_HTML,                 // @Html
    AT_JAVASCRIPT,           // @JavaScript
    AT_CHTL,                 // @Chtl
    AT_CJMOD,                // @CJmod
    AT_CONFIG,               // @Config
    AT_KEYWORD,              // 通用@关键字
    
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
    STAR,                    // *
    
    // 特殊
    NEWLINE,                 // 换行符
    WHITESPACE               // 空白字符
};

// Token结构
struct CHTLToken {
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t length;
    
    CHTLToken() : type(CHTLTokenType::ERROR), line(1), column(1), length(0) {}
    
    CHTLToken(CHTLTokenType t, const std::string& v, size_t l, size_t c, size_t len)
        : type(t), value(v), line(l), column(c), length(len) {}
};

// 词法分析器
class CHTLLexerV2 {
public:
    CHTLLexerV2();
    
    // 设置输入
    void SetInput(const std::string& input, const std::string& filename = "");
    
    // 获取下一个Token
    CHTLToken NextToken();
    
    // 批量获取所有Token
    std::vector<CHTLToken> Tokenize();
    
    // 获取错误信息
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    // 检查是否到达文件末尾
    bool IsAtEnd() const { return m_Current >= m_Input.length(); }

private:
    // 输入字符串
    std::string m_Input;
    std::string m_Filename;
    
    // 当前位置
    size_t m_Current;
    size_t m_Line;
    size_t m_Column;
    
    // 错误列表
    std::vector<std::string> m_Errors;
    
    // Origin块处理
    bool m_InOriginBlock;
    int m_OriginBraceDepth;
    
    // 辅助方法
    char CurrentChar() const;
    char NextChar() const;
    char PeekChar(int offset = 1) const;
    void Advance();
    bool Match(char expected);
    
    // 跳过空白
    void SkipWhitespace();
    void SkipComment();
    
    // 扫描各种Token
    CHTLToken ScanString(char quote);
    CHTLToken ScanNumber();
    CHTLToken ScanIdentifier();
    CHTLToken ScanUnquotedLiteral();
    CHTLToken ScanBracketKeyword();
    CHTLToken ScanAtKeyword();
    
    // 判断字符类型
    bool IsAlpha(char c) const;
    bool IsDigit(char c) const;
    bool IsAlphaNumeric(char c) const;
    bool IsIdentifierStart(char c) const;
    bool IsIdentifierChar(char c) const;
    bool IsUnquotedLiteralChar(char c) const;
    
    // 创建Token
    CHTLToken MakeToken(CHTLTokenType type, const std::string& value, size_t startPos);
    CHTLToken MakeToken(CHTLTokenType type, size_t startPos);
    
    // 错误报告
    void ReportError(const std::string& message);
};

} // namespace chtl

#endif // CHTL_LEXER_V2_H