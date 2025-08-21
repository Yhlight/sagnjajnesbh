#ifndef CHTL_CHTL_TOKEN_H
#define CHTL_CHTL_TOKEN_H

#include <string>
#include <unordered_map>
#include <set>

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 字面量
    Identifier,         // 标识符
    StringLiteral,      // 字符串字面量（双引号）
    SingleQuoteLiteral, // 字符串字面量（单引号）
    UnquotedLiteral,    // 无引号字面量
    Number,             // 数字
    
    // 关键字
    Text,               // text
    Style,              // style
    Script,             // script
    Inherit,            // inherit
    Delete,             // delete
    Insert,             // insert
    After,              // after
    Before,             // before
    Replace,            // replace
    At,                 // at
    Top,                // top
    Bottom,             // bottom
    From,               // from
    As,                 // as
    Except,             // except
    
    // 特殊标记
    Template,           // [Template]
    Custom,             // [Custom]
    Origin,             // [Origin]
    Import,             // [Import]
    Namespace,          // [Namespace]
    Configuration,      // [Configuration]
    Info,               // [Info]
    Export,             // [Export]
    
    // @前缀标记
    AtElement,          // @Element
    AtStyle,            // @Style
    AtVar,              // @Var
    AtHtml,             // @Html
    AtJavaScript,       // @JavaScript
    AtConfig,           // @Config
    AtChtl,             // @Chtl
    AtCJmod,            // @CJmod
    
    // 运算符和标点
    LeftBrace,          // {
    RightBrace,         // }
    LeftParen,          // (
    RightParen,         // )
    LeftBracket,        // [
    RightBracket,       // ]
    Semicolon,          // ;
    Colon,              // :
    Comma,              // ,
    Dot,                // .
    Equal,              // =
    Ampersand,          // &
    
    // 注释
    SingleLineComment,  // //
    MultiLineComment,   // /* */
    GeneratorComment,   // --
    
    // 特殊
    Eof,                // 文件结束
    Unknown,            // 未知Token
    
    // HTML元素（动态识别）
    HtmlElement,        // div, span, p 等
};

// Token结构
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token() : type(TokenType::Unknown), line(1), column(1), position(0) {}
    
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool IsKeyword() const;
    bool IsLiteral() const;
    bool IsOperator() const;
    bool IsSpecialMarker() const;
    bool IsAtMarker() const;
    
    std::string ToString() const;
};

// Token工具类
class TokenUtils {
public:
    // 获取Token类型名称
    static std::string GetTokenTypeName(TokenType type);
    
    // 判断是否为关键字
    static bool IsKeyword(const std::string& word);
    
    // 判断是否为HTML元素
    static bool IsHtmlElement(const std::string& word);
    
    // 判断是否为特殊标记
    static bool IsSpecialMarker(const std::string& word);
    
    // 判断是否为@标记
    static bool IsAtMarker(const std::string& word);
    
    // 获取关键字的Token类型
    static TokenType GetKeywordType(const std::string& word);
    
    // 获取特殊标记的Token类型
    static TokenType GetSpecialMarkerType(const std::string& word);
    
    // 获取@标记的Token类型
    static TokenType GetAtMarkerType(const std::string& word);
    
private:
    static const std::unordered_map<std::string, TokenType> keywordMap_;
    static const std::unordered_map<std::string, TokenType> specialMarkerMap_;
    static const std::unordered_map<std::string, TokenType> atMarkerMap_;
    static const std::set<std::string> htmlElements_;
};

} // namespace CHTL

#endif // CHTL_CHTL_TOKEN_H