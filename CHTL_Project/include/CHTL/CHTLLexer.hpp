#pragma once

#include "../Core/CHTLTypes.hpp"
#include "CHTLToken.hpp"
#include "CHTLState.hpp"
#include <regex>

namespace CHTL {

/**
 * @brief CHTL词法分析器
 * 
 * 负责将CHTL源代码转换为Token流
 * 支持CHTL语法文档中定义的所有语法元素
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;

    // 禁用拷贝构造和赋值
    CHTLLexer(const CHTLLexer&) = delete;
    CHTLLexer& operator=(const CHTLLexer&) = delete;

    /**
     * @brief 设置源代码
     * @param source 源代码
     * @param filename 文件名
     */
    void SetSource(const String& source, const String& filename = "");

    /**
     * @brief 获取下一个Token
     * @return 下一个Token
     */
    CHTLToken NextToken();

    /**
     * @brief 预览下一个Token（不消费）
     * @param lookahead 向前看的距离，默认为1
     * @return 预览的Token
     */
    CHTLToken PeekToken(size_t lookahead = 1);

    /**
     * @brief 检查是否到达文件结尾
     * @return 是否到达EOF
     */
    bool IsAtEnd() const;

    /**
     * @brief 获取当前位置信息
     * @return 当前源代码位置
     */
    SourceLocation GetCurrentLocation() const;

    /**
     * @brief 设置词法分析选项
     * @param skipWhitespace 是否跳过空白字符
     * @param skipComments 是否跳过注释
     * @param enableDebug 是否启用调试模式
     */
    void SetOptions(bool skipWhitespace = true, bool skipComments = false, bool enableDebug = false);

    /**
     * @brief 获取词法分析统计信息
     */
    struct LexerStatistics {
        size_t totalTokens;
        size_t identifiers;
        size_t keywords;
        size_t literals;
        size_t operators;
        size_t comments;
        size_t errors;
    };

    LexerStatistics GetStatistics() const { return statistics_; }

    /**
     * @brief 重置词法分析器
     */
    void Reset();

    /**
     * @brief 获取所有Token（用于调试）
     * @return Token列表
     */
    std::vector<CHTLToken> GetAllTokens();

private:
    // 词法分析状态
    enum class LexerState {
        Normal,              // 正常状态
        InString,            // 在字符串内部
        InComment,           // 在注释内部
        InCHTLJSSelector,    // 在CHTL JS选择器内部
        InTemplate,          // 在模板内部
        InCustom,            // 在自定义内部
        InOrigin,            // 在原始嵌入内部
        InConfiguration,     // 在配置内部
        InNamespace,         // 在命名空间内部
        Error                // 错误状态
    };

    // 内部数据
    String source_;
    String filename_;
    size_t position_;
    size_t line_;
    size_t column_;
    size_t length_;
    
    LexerState state_;
    std::stack<LexerState> stateStack_;
    
    // 选项
    bool skipWhitespace_;
    bool skipComments_;
    bool debugMode_;
    
    // 统计信息
    LexerStatistics statistics_;
    
    // 关键字映射
    UniquePtr<CHTLKeywordMap> keywordMap_;
    
    // 正则表达式模式
    std::regex identifierPattern_;
    std::regex numberPattern_;
    std::regex stringPattern_;
    std::regex commentPattern_;
    std::regex chtljsSelectorPattern_;
    std::regex htmlElementPattern_;

    // 核心词法分析方法
    CHTLToken ScanToken();
    CHTLToken ScanIdentifierOrKeyword();
    CHTLToken ScanNumber();
    CHTLToken ScanString(char quote);
    CHTLToken ScanUnquotedLiteral();
    CHTLToken ScanComment();
    CHTLToken ScanCHTLJSSelector();
    CHTLToken ScanOperator();
    
    // 字符处理方法
    char CurrentChar() const;
    char NextChar();
    char PeekChar(size_t offset = 1) const;
    bool Match(char expected);
    bool Match(const String& expected);
    void SkipWhitespace();
    void SkipToEndOfLine();
    
    // 状态管理
    void PushState(LexerState newState);
    void PopState();
    
    // 辅助方法
    bool IsAtEnd(size_t position) const;
    bool IsAlpha(char c) const;
    bool IsDigit(char c) const;
    bool IsAlphaNumeric(char c) const;
    bool IsWhitespace(char c) const;
    bool IsNewline(char c) const;
    bool IsHtmlElement(const String& identifier) const;
    
    // 错误处理
    CHTLToken MakeErrorToken(const String& message);
    void UpdateStatistics(CHTLTokenType type);
    
    // 位置跟踪
    void AdvancePosition();
    void UpdateLocation();
    SourceLocation MakeLocation() const;
    
    // Token创建
    CHTLToken MakeToken(CHTLTokenType type, const String& value = "");
    CHTLToken MakeStringToken(const String& value, char quote);
    
    // 特殊处理
    CHTLTokenType DetermineIdentifierType(const String& identifier);
    bool IsValidIdentifierStart(char c) const;
    bool IsValidIdentifierChar(char c) const;
    bool IsValidUnquotedChar(char c) const;
    
    // HTML元素列表初始化
    void InitializeHtmlElements();
    StringSet htmlElements_;
    
    // 调试方法
    void LogDebugInfo(const String& message) const;
};

} // namespace CHTL