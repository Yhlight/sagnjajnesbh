#pragma once

#include "CHTLCommon.h"
#include <unordered_map>

namespace CHTL {

// 扩展Token类型，严格按照CHTL语法文档
enum class TokenType {
    // 基础Token
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量 ""
    SINGLE_STRING,      // 单引号字符串 ''
    UNQUOTED_LITERAL,   // 无修饰字面量(无引号字符串)
    NUMBER,             // 数字字面量
    
    // 注释类型
    COMMENT_SINGLE,     // 单行注释 //
    COMMENT_MULTI,      // 多行注释 /* */
    COMMENT_GENERATOR,  // 生成器注释 --
    
    // 符号
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    SEMICOLON,          // ;
    COLON,              // :
    EQUALS,             // = (CE对等式支持)
    COMMA,              // ,
    DOT,                // .
    SLASH,              // / (路径分隔符，可替代.)
    
    // CHTL关键字
    TEXT,               // text
    STYLE,              // style (局部样式块)
    SCRIPT,             // script (局部脚本块)
    
    // 模板和自定义声明
    TEMPLATE_DECL,      // [Template]
    CUSTOM_DECL,        // [Custom]
    ORIGIN_DECL,        // [Origin]
    IMPORT_DECL,        // [Import]
    NAMESPACE_DECL,     // [Namespace]
    CONFIGURATION_DECL, // [Configuration]
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    
    // 配置块特殊标识
    NAME_BLOCK,         // [Name]
    ORIGIN_TYPE_BLOCK,  // [OriginType]
    INFO_BLOCK,         // [Info]
    EXPORT_BLOCK,       // [Export]
    
    // 关键字
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    FROM,               // from
    AS,                 // as
    EXCEPT,             // except
    
    // CHTL JS特殊符号
    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    ARROW,              // ->
    AMPERSAND,          // &
    VIR,                // vir
    LISTEN,             // listen
    DELEGATE,           // delegate
    ANIMATE,            // animate
    
    // CHTL JS动画关键字
    TARGET,             // target
    DURATION,           // duration
    EASING,             // easing
    BEGIN,              // begin
    WHEN,               // when
    END,                // end
    LOOP,               // loop
    DIRECTION,          // direction
    DELAY,              // delay
    CALLBACK,           // callback
    AT_KEYWORD,         // at (在when块中使用)
    
    // HTML元素 (动态识别)
    HTML_TAG,           // HTML标签名
    
    // 结束符和空白
    END_OF_FILE,        // EOF
    NEWLINE,            // 换行
    WHITESPACE,         // 空白字符
    
    UNKNOWN             // 未知Token
};

// Token值类型
using TokenValue = std::variant<std::string, int, double, bool>;

// 增强的Token结构
struct Token {
    TokenType type;
    std::string value;
    TokenValue parsedValue;  // 解析后的值
    size_t line;
    size_t column;
    size_t position;
    size_t length;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", 
          size_t l = 0, size_t c = 0, size_t p = 0, size_t len = 0)
        : type(t), value(v), parsedValue(v), line(l), column(c), position(p), length(len) {}
    
    // 便利方法
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isComment() const;
    bool isCHTLJS() const;
    std::string toString() const;
};

// 关键字映射表
class KeywordMap {
public:
    static KeywordMap& getInstance();
    
    // 根据配置初始化关键字映射
    void initialize(const std::unordered_map<std::string, std::vector<std::string>>& nameConfig);
    
    // 查找关键字对应的Token类型
    TokenType getTokenType(const std::string& keyword) const;
    
    // 检查是否为关键字
    bool isKeyword(const std::string& word) const;
    
    // 检查是否为HTML标签
    bool isHTMLTag(const std::string& tag) const;
    
    // 添加自定义原始嵌入类型
    void addOriginType(const std::string& type);
    
private:
    KeywordMap() = default;
    std::unordered_map<std::string, TokenType> keywordMap_;
    std::unordered_set<std::string> htmlTags_;
    std::unordered_set<std::string> originTypes_;
    
    void initializeDefaultKeywords();
    void initializeHTMLTags();
};

// Token工具函数
namespace TokenUtils {
    // 转换Token类型为字符串
    std::string tokenTypeToString(TokenType type);
    
    // 检查Token是否为二元操作符
    bool isBinaryOperator(TokenType type);
    
    // 检查Token是否为一元操作符
    bool isUnaryOperator(TokenType type);
    
    // 获取操作符优先级
    int getOperatorPrecedence(TokenType type);
    
    // 检查是否为右结合操作符
    bool isRightAssociative(TokenType type);
    
    // 解析字符串字面量(处理转义字符)
    std::string parseStringLiteral(const std::string& literal);
    
    // 解析数字字面量
    TokenValue parseNumberLiteral(const std::string& literal);
}

} // namespace CHTL