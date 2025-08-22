#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include <string>
#include <vector>

namespace CHTL {
namespace CHTLJS {
namespace Lexer {

/**
 * @brief CHTL JS词法分析器配置
 */
struct LexerConfig {
    bool enableDebug;               // 启用调试模式
    bool preserveWhitespace;        // 保留空白字符
    bool preserveComments;          // 保留注释
    bool strictMode;                // 严格模式
    
    LexerConfig() : enableDebug(false), preserveWhitespace(false), 
                   preserveComments(true), strictMode(true) {}
};

/**
 * @brief CHTL JS词法分析器
 * 
 * 专门处理CHTL JS的特殊语法，包括：
 * - 增强选择器 {{selector}}
 * - 箭头操作符 ->
 * - 虚对象 vir
 * - 监听器 listen
 * - 事件委托 delegate
 * - 动画 animate
 * - iNeverAway特殊函数
 */
class CHTLJSLexer {
public:
    /**
     * @brief 构造函数
     * @param config 词法分析器配置
     */
    explicit CHTLJSLexer(const LexerConfig& config = LexerConfig());
    
    /**
     * @brief 词法分析主方法
     * @param source 源代码
     * @param fileName 文件名
     * @return Token流
     */
    Core::CHTLJSTokenStream Tokenize(const std::string& source, const std::string& fileName = "");
    
    /**
     * @brief 设置配置
     */
    void SetConfig(const LexerConfig& config) { config_ = config; }
    
    /**
     * @brief 获取配置
     */
    const LexerConfig& GetConfig() const { return config_; }

private:
    /**
     * @brief 扫描单个Token
     * @return 扫描到的Token
     */
    Core::CHTLJSToken ScanToken();
    
    /**
     * @brief 扫描增强选择器 {{selector}}
     * @return 增强选择器Token
     */
    Core::CHTLJSToken ScanEnhancedSelector();
    
    /**
     * @brief 扫描箭头操作符 ->
     * @return 箭头操作符Token
     */
    Core::CHTLJSToken ScanArrowOperator();
    
    /**
     * @brief 扫描标识符或关键字
     * @return 标识符或关键字Token
     */
    Core::CHTLJSToken ScanIdentifierOrKeyword();
    
    /**
     * @brief 扫描字符串字面量
     * @param quote 引号字符
     * @return 字符串Token
     */
    Core::CHTLJSToken ScanString(char quote);
    
    /**
     * @brief 扫描数字字面量
     * @return 数字Token
     */
    Core::CHTLJSToken ScanNumber();
    
    /**
     * @brief 扫描注释
     * @return 注释Token
     */
    Core::CHTLJSToken ScanComment();
    
    /**
     * @brief 扫描单行注释
     * @return 注释Token
     */
    Core::CHTLJSToken ScanSingleLineComment();
    
    /**
     * @brief 扫描多行注释
     * @return 注释Token
     */
    Core::CHTLJSToken ScanMultiLineComment();
    
    /**
     * @brief 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * @brief 前进一个字符
     * @return 当前字符
     */
    char Advance();
    
    /**
     * @brief 查看当前字符
     * @return 当前字符
     */
    char Peek() const;
    
    /**
     * @brief 查看下一个字符
     * @param offset 偏移量
     * @return 指定偏移的字符
     */
    char PeekNext(int offset = 1) const;
    
    /**
     * @brief 匹配字符
     * @param expected 期望的字符
     * @return 是否匹配
     */
    bool Match(char expected);
    
    /**
     * @brief 检查是否到达结尾
     * @return 是否到达结尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 检查是否为字母
     * @param c 字符
     * @return 是否为字母
     */
    bool IsAlpha(char c) const;
    
    /**
     * @brief 检查是否为数字
     * @param c 字符
     * @return 是否为数字
     */
    bool IsDigit(char c) const;
    
    /**
     * @brief 检查是否为字母或数字
     * @param c 字符
     * @return 是否为字母或数字
     */
    bool IsAlphaNumeric(char c) const;
    
    /**
     * @brief 检查是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    bool IsWhitespace(char c) const;
    
    /**
     * @brief 创建Token
     * @param type Token类型
     * @param value Token值
     * @return 创建的Token
     */
    Core::CHTLJSToken MakeToken(Core::TokenType type, const std::string& value = "");
    
    /**
     * @brief 创建错误Token
     * @param message 错误信息
     * @return 错误Token
     */
    Core::CHTLJSToken MakeErrorToken(const std::string& message);
    
    /**
     * @brief 获取当前位置信息
     * @return 位置信息
     */
    Core::TokenPosition GetCurrentPosition() const;

private:
    LexerConfig config_;        // 配置
    std::string source_;        // 源代码
    std::string fileName_;      // 文件名
    size_t start_;              // Token开始位置
    size_t current_;            // 当前位置
    int line_;                  // 当前行号
    int column_;                // 当前列号
};

} // namespace Lexer
} // namespace CHTLJS
} // namespace CHTL