#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {
namespace Core {

/**
 * @brief CHTL Token类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 基础Token
    IDENTIFIER,             // 标识符
    STRING_LITERAL,         // 字符串字面量 "..." 或 '...'
    UNQUOTED_LITERAL,       // 无修饰字面量
    NUMBER,                 // 数字
    
    // 分隔符和操作符
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    SEMICOLON,              // ;
    COLON,                  // :
    EQUAL,                  // = (CE对等式)
    DOT,                    // .
    COMMA,                  // ,
    AMPERSAND,              // &
    HASH,                   // #
    AT,                     // @
    
    // 注释
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /* */
    GENERATOR_COMMENT,      // --
    
    // CHTL关键字
    TEXT,                   // text
    STYLE,                  // style
    SCRIPT,                 // script
    INHERIT,                // inherit
    DELETE,                 // delete
    INSERT,                 // insert
    AFTER,                  // after
    BEFORE,                 // before
    REPLACE,                // replace
    AT_TOP,                 // at top
    AT_BOTTOM,              // at bottom
    FROM,                   // from
    AS,                     // as
    EXCEPT,                 // except
    
    // CHTL标记关键字
    TEMPLATE,               // [Template]
    CUSTOM,                 // [Custom]
    ORIGIN,                 // [Origin]
    IMPORT,                 // [Import]
    NAMESPACE,              // [Namespace]
    CONFIGURATION,          // [Configuration]
    INFO,                   // [Info]
    EXPORT,                 // [Export]
    NAME,                   // [Name]
    ORIGIN_TYPE,            // [OriginType]
    
    // CHTL类型关键字
    AT_STYLE,               // @Style
    AT_ELEMENT,             // @Element
    AT_VAR,                 // @Var
    AT_HTML,                // @Html
    AT_JAVASCRIPT,          // @JavaScript
    AT_CHTL,                // @Chtl
    AT_CJMOD,               // @CJmod
    AT_CONFIG,              // @Config
    // 错误的类型已移除：AT_VUE、AT_CSS、AT_CSS_LOWER不存在于语法文档中
    
    // HTML元素（常用）
    HTML_TAG,               // html, head, body, div, span等
    
    // CSS选择器
    CLASS_SELECTOR,         // .className
    ID_SELECTOR,            // #idName
    PSEUDO_CLASS,           // :hover, :active等
    PSEUDO_ELEMENT,         // ::before, ::after等
    
    // 特殊Token
    NEWLINE,                // 换行符
    WHITESPACE,             // 空白字符
    END_OF_FILE,            // 文件结束
    UNKNOWN,                // 未知Token
    
    // 配置相关
    CONFIG_KEY,             // 配置键
    CONFIG_VALUE,           // 配置值
    
    // 索引访问
    INDEX_ACCESS,           // [0], [1]等
    
    // 组合Token
    AT_TOP_KEYWORD,         // "at top"组合关键字
    AT_BOTTOM_KEYWORD       // "at bottom"组合关键字
};

/**
 * @brief CHTL Token类
 */
class CHTLToken {
public:
    /**
     * @brief 构造函数
     * @param type Token类型
     * @param value Token值
     * @param line 行号
     * @param column 列号
     * @param fileName 文件名
     */
    CHTLToken(TokenType type, const std::string& value, int line = 1, int column = 1, 
              const std::string& fileName = "");
    
    /**
     * @brief 默认构造函数
     */
    CHTLToken() = default;
    
    /**
     * @brief 复制构造函数
     */
    CHTLToken(const CHTLToken& other) = default;
    
    /**
     * @brief 赋值操作符
     */
    CHTLToken& operator=(const CHTLToken& other) = default;
    
    /**
     * @brief 获取Token类型
     */
    TokenType GetType() const { return type_; }
    
    /**
     * @brief 获取Token值
     */
    const std::string& GetValue() const { return value_; }
    
    /**
     * @brief 获取行号
     */
    int GetLine() const { return line_; }
    
    /**
     * @brief 获取列号
     */
    int GetColumn() const { return column_; }
    
    /**
     * @brief 获取文件名
     */
    const std::string& GetFileName() const { return fileName_; }
    
    /**
     * @brief 设置Token类型
     */
    void SetType(TokenType type) { type_ = type; }
    
    /**
     * @brief 设置Token值
     */
    void SetValue(const std::string& value) { value_ = value; }
    
    /**
     * @brief 设置位置信息
     */
    void SetPosition(int line, int column) { line_ = line; column_ = column; }
    
    /**
     * @brief 设置文件名
     */
    void SetFileName(const std::string& fileName) { fileName_ = fileName; }
    
    /**
     * @brief 检查是否为关键字
     */
    bool IsKeyword() const;
    
    /**
     * @brief 检查是否为标记关键字
     */
    bool IsMarkerKeyword() const;
    
    /**
     * @brief 检查是否为类型关键字
     */
    bool IsTypeKeyword() const;
    
    /**
     * @brief 检查是否为字面量
     */
    bool IsLiteral() const;
    
    /**
     * @brief 检查是否为操作符
     */
    bool IsOperator() const;
    
    /**
     * @brief 检查是否为分隔符
     */
    bool IsDelimiter() const;
    
    /**
     * @brief 检查是否为注释
     */
    bool IsComment() const;
    
    /**
     * @brief 检查是否为HTML标签
     */
    bool IsHTMLTag() const;
    
    /**
     * @brief 检查是否为CSS选择器
     */
    bool IsCSSSelector() const;
    
    /**
     * @brief 转换为字符串表示
     */
    std::string ToString() const;
    
    /**
     * @brief 获取Token类型名称
     */
    static std::string GetTokenTypeName(TokenType type);
    
    /**
     * @brief 从字符串获取Token类型
     */
    static TokenType GetTokenTypeFromString(const std::string& str);
    
    /**
     * @brief 检查字符串是否为CHTL关键字
     */
    static bool IsKeywordString(const std::string& str);
    
    /**
     * @brief 检查字符串是否为HTML标签
     */
    static bool IsHTMLTagString(const std::string& str);

private:
    TokenType type_ = TokenType::UNKNOWN;
    std::string value_;
    int line_ = 1;
    int column_ = 1;
    std::string fileName_;
    
    // 静态关键字映射表
    static std::unordered_map<std::string, TokenType> keywordMap_;
    static std::unordered_map<std::string, TokenType> htmlTagMap_;
    
    /**
     * @brief 初始化关键字映射表
     */
    static void InitializeKeywordMaps();
    
    /**
     * @brief 确保映射表已初始化
     */
    static void EnsureMapsInitialized();
};

/**
 * @brief Token位置信息结构体
 */
struct TokenPosition {
    int line;
    int column;
    std::string fileName;
    
    TokenPosition(int l = 1, int c = 1, const std::string& f = "") 
        : line(l), column(c), fileName(f) {}
    
    std::string ToString() const;
};

/**
 * @brief Token流类
 */
class TokenStream {
public:
    /**
     * @brief 构造函数
     */
    TokenStream() = default;
    
    /**
     * @brief 添加Token
     */
    void AddToken(const CHTLToken& token);
    
    /**
     * @brief 获取当前Token
     */
    const CHTLToken& Current() const;
    
    /**
     * @brief 前进到下一个Token
     */
    void Advance();
    
    /**
     * @brief 回退到上一个Token
     */
    void Retreat();
    
    /**
     * @brief 查看下一个Token
     */
    const CHTLToken& Peek(int offset = 1) const;
    
    /**
     * @brief 检查是否到达流末尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 获取当前位置
     */
    size_t GetPosition() const { return position_; }
    
    /**
     * @brief 设置位置
     */
    void SetPosition(size_t pos);
    
    /**
     * @brief 获取Token总数
     */
    size_t Size() const { return tokens_.size(); }
    
    /**
     * @brief 清空Token流
     */
    void Clear();
    
    /**
     * @brief 重置到开始位置
     */
    void Reset() { position_ = 0; }

private:
    std::vector<CHTLToken> tokens_;
    size_t position_ = 0;
    static const CHTLToken eofToken_;
};

} // namespace Core
} // namespace CHTL