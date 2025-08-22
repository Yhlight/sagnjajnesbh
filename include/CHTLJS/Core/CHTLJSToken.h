#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Core {

/**
 * @brief CHTL JS Token类型枚举
 * 
 * 定义CHTL JS编译器支持的所有Token类型
 * 包括增强选择器、箭头操作符、虚对象等特殊语法
 */
enum class TokenType {
    // 基础Token
    IDENTIFIER = 0,     // 标识符
    NUMBER,             // 数字
    STRING,             // 字符串
    
    // 运算符
    EQUAL,              // =
    ARROW,              // -> (CHTL JS箭头操作符)
    DOT,                // .
    COMMA,              // ,
    SEMICOLON,          // ;
    COLON,              // :
    
    // 括号
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    
    // CHTL JS特殊语法
    ENHANCED_SELECTOR,  // {{selector}} 增强选择器
    VIR,                // vir 虚对象关键字
    LISTEN,             // listen 监听器关键字
    DELEGATE,           // delegate 事件委托关键字
    ANIMATE,            // animate 动画关键字
    
    // 增强选择器内部
    SELECTOR_START,     // {{
    SELECTOR_END,       // }}
    SELECTOR_CONTENT,   // 选择器内容
    
    // 函数和对象
    FUNCTION,           // function关键字
    CONST,              // const关键字
    LET,                // let关键字
    VAR,                // var关键字
    
    // 事件委托相关
    TARGET,             // target属性
    
    // 动画相关
    DURATION,           // duration属性
    EASING,             // easing属性
    BEGIN,              // begin块
    WHEN,               // when数组
    END,                // end块
    LOOP,               // loop属性
    DIRECTION,          // direction属性
    DELAY,              // delay属性
    CALLBACK,           // callback属性
    AT,                 // at关键字（动画时间点）
    
    // 虚对象相关
    I_NEVER_AWAY,       // iNeverAway特殊函数
    VOID,               // Void类型
    VOID_STATE,         // Void<State>状态类型
    
    // 注释
    COMMENT,            // 注释
    
    // 空白和特殊
    WHITESPACE,         // 空白字符
    NEWLINE,            // 换行
    END_OF_FILE,        // 文件结束
    ERROR_TOKEN,        // 错误Token
    
    // Token总数
    TOKEN_COUNT
};

/**
 * @brief Token位置信息
 */
struct TokenPosition {
    int line;       // 行号
    int column;     // 列号
    int offset;     // 偏移量
    
    TokenPosition(int l = 1, int c = 1, int o = 0) : line(l), column(c), offset(o) {}
};

/**
 * @brief CHTL JS Token类
 */
class CHTLJSToken {
public:
    /**
     * @brief 默认构造函数
     */
    CHTLJSToken();
    
    /**
     * @brief 构造函数
     * @param type Token类型
     * @param value Token值
     * @param position Token位置
     */
    CHTLJSToken(TokenType type, const std::string& value, const TokenPosition& position);
    
    /**
     * @brief 获取Token类型
     */
    TokenType GetType() const { return type_; }
    
    /**
     * @brief 获取Token值
     */
    const std::string& GetValue() const { return value_; }
    
    /**
     * @brief 获取Token位置
     */
    const TokenPosition& GetPosition() const { return position_; }
    
    /**
     * @brief 获取行号
     */
    int GetLine() const { return position_.line; }
    
    /**
     * @brief 获取列号
     */
    int GetColumn() const { return position_.column; }
    
    /**
     * @brief 获取偏移量
     */
    int GetOffset() const { return position_.offset; }
    
    /**
     * @brief 检查是否为增强选择器
     */
    bool IsEnhancedSelector() const { return type_ == TokenType::ENHANCED_SELECTOR; }
    
    /**
     * @brief 检查是否为CHTL JS关键字
     */
    bool IsCHTLJSKeyword() const;
    
    /**
     * @brief 检查是否为虚对象相关Token
     */
    bool IsVirtualObjectToken() const;
    
    /**
     * @brief 检查是否为事件相关Token
     */
    bool IsEventToken() const;
    
    /**
     * @brief 检查是否为动画相关Token
     */
    bool IsAnimationToken() const;
    
    /**
     * @brief 转换为字符串表示
     */
    std::string ToString() const;
    
    /**
     * @brief 获取Token类型名称
     */
    static std::string GetTokenTypeName(TokenType type);
    
    /**
     * @brief 检查字符串是否为CHTL JS关键字
     */
    static bool IsKeywordString(const std::string& str);
    
    /**
     * @brief 获取关键字对应的Token类型
     */
    static TokenType GetKeywordTokenType(const std::string& keyword);

private:
    TokenType type_;            // Token类型
    std::string value_;         // Token值
    TokenPosition position_;    // Token位置
    
    // 静态关键字映射表
    static std::unordered_map<std::string, TokenType> keywordMap_;
    static bool keywordMapInitialized_;
    
    /**
     * @brief 初始化关键字映射表
     */
    static void InitializeKeywordMap();
};

/**
 * @brief CHTL JS Token流类
 */
class CHTLJSTokenStream {
public:
    /**
     * @brief 构造函数
     */
    CHTLJSTokenStream();
    
    /**
     * @brief 构造函数
     * @param tokens Token列表
     */
    explicit CHTLJSTokenStream(const std::vector<CHTLJSToken>& tokens);
    
    /**
     * @brief 添加Token
     */
    void AddToken(const CHTLJSToken& token);
    
    /**
     * @brief 获取当前Token
     */
    const CHTLJSToken& Current() const;
    
    /**
     * @brief 前进到下一个Token
     */
    void Advance();
    
    /**
     * @brief 查看下一个Token（不前进）
     */
    const CHTLJSToken& Peek(int offset = 1) const;
    
    /**
     * @brief 检查是否到达结尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 重置到开头
     */
    void Reset();
    
    /**
     * @brief 获取Token数量
     */
    size_t Size() const { return tokens_.size(); }
    
    /**
     * @brief 获取当前位置
     */
    size_t GetPosition() const { return currentPosition_; }
    
    /**
     * @brief 设置当前位置
     */
    void SetPosition(size_t position);
    
    /**
     * @brief 获取所有Token
     */
    const std::vector<CHTLJSToken>& GetTokens() const { return tokens_; }

private:
    std::vector<CHTLJSToken> tokens_;   // Token列表
    size_t currentPosition_;            // 当前位置
    CHTLJSToken eofToken_;             // EOF Token
};

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL