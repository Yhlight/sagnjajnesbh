#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "CHTL/Core/CHTLToken.h"
#include "Utils/ErrorHandler.h"

namespace CHTL {
namespace Lexer {

/**
 * @brief 词法分析器配置
 */
struct LexerConfig {
    bool preserveWhitespace;        // 是否保留空白字符
    bool preserveComments;          // 是否保留注释
    bool enableDebug;               // 是否启用调试模式
    bool strictMode;                // 是否启用严格模式
    
    LexerConfig() : preserveWhitespace(false), preserveComments(true), 
                   enableDebug(false), strictMode(true) {}
};

/**
 * @brief CHTL词法分析器类
 * 
 * 负责将CHTL源代码转换为Token流，严格按照语法文档规范
 */
class CHTLLexer {
public:
    /**
     * @brief 构造函数
     * @param config 词法分析器配置
     */
    explicit CHTLLexer(const LexerConfig& config = LexerConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLLexer() = default;
    
    /**
     * @brief 对源代码进行词法分析
     * @param source 源代码
     * @param fileName 文件名
     * @return Token流
     */
    Core::TokenStream Tokenize(const std::string& source, const std::string& fileName = "");
    
    /**
     * @brief 设置配置
     * @param config 新配置
     */
    void SetConfig(const LexerConfig& config) { config_ = config; }
    
    /**
     * @brief 获取配置
     * @return 当前配置
     */
    const LexerConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 重置词法分析器状态
     */
    void Reset();
    
    /**
     * @brief 获取错误数量
     * @return 错误数量
     */
    size_t GetErrorCount() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    /**
     * @brief 扫描下一个Token
     * @return 扫描到的Token
     */
    Core::CHTLToken ScanToken();
    
    /**
     * @brief 扫描标识符或关键字
     * @return Token
     */
    Core::CHTLToken ScanIdentifierOrKeyword();
    
    /**
     * @brief 扫描字符串字面量
     * @param quote 引号字符
     * @return Token
     */
    Core::CHTLToken ScanStringLiteral(char quote);
    
    /**
     * @brief 扫描数字字面量
     * @return Token
     */
    Core::CHTLToken ScanNumberLiteral();
    
    /**
     * @brief 扫描单行注释
     * @return Token
     */
    Core::CHTLToken ScanSingleLineComment();
    
    /**
     * @brief 扫描多行注释
     * @return Token
     */
    Core::CHTLToken ScanMultiLineComment();
    
    /**
     * @brief 扫描生成器注释
     * @return Token
     */
    Core::CHTLToken ScanGeneratorComment();
    
    /**
     * @brief 扫描标记关键字（如[Template]）
     * @return Token
     */
    Core::CHTLToken ScanMarkerKeyword();
    
    /**
     * @brief 扫描类型关键字（如@Style）
     * @return Token
     */
    Core::CHTLToken ScanTypeKeyword();
    
    /**
     * @brief 扫描CSS选择器
     * @param prefix 前缀字符（. # : &）
     * @return Token
     */
    Core::CHTLToken ScanCSSSelector(char prefix);
    
    /**
     * @brief 扫描组合关键字（如"at top"）
     * @return Token，如果不是组合关键字返回普通标识符Token
     */
    Core::CHTLToken ScanCompositeKeyword();
    
    /**
     * @brief 检查当前字符
     * @return 当前字符，如果到达末尾返回'\0'
     */
    char Peek() const;
    
    /**
     * @brief 检查指定偏移位置的字符
     * @param offset 偏移量
     * @return 指定位置的字符，如果超出范围返回'\0'
     */
    char Peek(int offset) const;
    
    /**
     * @brief 前进到下一个字符
     * @return 当前字符（前进前的字符）
     */
    char Advance();
    
    /**
     * @brief 检查当前字符是否匹配
     * @param expected 期望的字符
     * @return 是否匹配
     */
    bool Match(char expected);
    
    /**
     * @brief 检查是否到达源代码末尾
     * @return 是否到达末尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * @brief 创建Token
     * @param type Token类型
     * @param value Token值
     * @return 创建的Token
     */
    Core::CHTLToken MakeToken(Core::TokenType type, const std::string& value = "");
    
    /**
     * @brief 创建错误Token
     * @param message 错误消息
     * @return 错误Token
     */
    Core::CHTLToken MakeErrorToken(const std::string& message);
    
    /**
     * @brief 报告词法错误
     * @param message 错误消息
     */
    void ReportError(const std::string& message);
    
    /**
     * @brief 报告词法警告
     * @param message 警告消息
     */
    void ReportWarning(const std::string& message);
    
    /**
     * @brief 检查字符是否为字母
     * @param ch 字符
     * @return 是否为字母
     */
    static bool IsAlpha(char ch);
    
    /**
     * @brief 检查字符是否为数字
     * @param ch 字符
     * @return 是否为数字
     */
    static bool IsDigit(char ch);
    
    /**
     * @brief 检查字符是否为字母或数字
     * @param ch 字符
     * @return 是否为字母或数字
     */
    static bool IsAlphaNumeric(char ch);
    
    /**
     * @brief 检查字符是否为空白字符
     * @param ch 字符
     * @return 是否为空白字符
     */
    static bool IsWhitespace(char ch);
    
    /**
     * @brief 检查字符是否为有效的标识符字符
     * @param ch 字符
     * @param isFirst 是否为标识符的第一个字符
     * @return 是否有效
     */
    static bool IsValidIdentifierChar(char ch, bool isFirst = false);

private:
    LexerConfig config_;                    // 词法分析器配置
    std::string source_;                    // 源代码
    std::string fileName_;                  // 文件名
    size_t current_;                        // 当前位置
    size_t start_;                          // 当前Token的起始位置
    int line_;                              // 当前行号
    int column_;                            // 当前列号
    size_t tokenCount_;                     // Token计数
    
    // 统计信息
    std::unordered_map<Core::TokenType, size_t> tokenStats_;
};

/**
 * @brief 词法分析器工厂类
 */
class LexerFactory {
public:
    /**
     * @brief 创建默认词法分析器
     * @return 词法分析器指针
     */
    static std::unique_ptr<CHTLLexer> CreateDefaultLexer();
    
    /**
     * @brief 创建严格模式词法分析器
     * @return 词法分析器指针
     */
    static std::unique_ptr<CHTLLexer> CreateStrictLexer();
    
    /**
     * @brief 创建调试模式词法分析器
     * @return 词法分析器指针
     */
    static std::unique_ptr<CHTLLexer> CreateDebugLexer();
};

} // namespace Lexer
} // namespace CHTL