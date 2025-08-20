#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "chtl_js/token/CHTLJSToken.h"

namespace chtl {
namespace chtljs {

/**
 * CHTL JS 词法分析器
 * 专门处理CHTL JS语法的词法分析
 * 注意：完全独立于CHTL的词法分析器，不处理JavaScript内容
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();
    
    /**
     * 初始化词法分析器
     * @param source CHTL JS源代码（通常来自局部script块）
     * @param filename 文件名（用于错误报告）
     */
    void Initialize(const std::string& source, const std::string& filename = "");
    
    /**
     * 获取下一个Token
     * @return 下一个Token
     */
    CHTLJSToken NextToken();
    
    /**
     * 查看下一个Token但不消费
     * @param lookahead 向前查看的数量
     * @return Token
     */
    CHTLJSToken PeekToken(size_t lookahead = 1);
    
    /**
     * 获取所有Token
     * @return Token列表
     */
    std::vector<CHTLJSToken> GetAllTokens();
    
    /**
     * 重置词法分析器
     */
    void Reset();
    
    /**
     * 检查是否在增强选择器内
     */
    bool IsInEnhancedSelector() const { return m_InEnhancedSelector; }
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 源代码和位置
    std::string m_Source;
    std::string m_Filename;
    size_t m_Position;
    size_t m_Line;
    size_t m_Column;
    
    // 状态
    bool m_InEnhancedSelector;
    int m_BraceDepth;
    
    // Token缓存
    std::vector<CHTLJSToken> m_TokenCache;
    
    // 错误信息
    mutable std::vector<std::string> m_Errors;
    
    // CHTL JS关键字和函数映射
    static const std::unordered_map<std::string, CHTLJSTokenType> s_Keywords;
    static const std::unordered_map<std::string, CHTLJSTokenType> s_Functions;
    
    // 核心方法
    CHTLJSToken ScanToken();
    
    // 扫描具体类型
    CHTLJSToken ScanIdentifierOrKeyword();
    CHTLJSToken ScanEnhancedSelector();
    CHTLJSToken ScanSelectorContent();
    CHTLJSToken ScanString(char delimiter);
    CHTLJSToken ScanNumber();
    CHTLJSToken ScanArrow();
    CHTLJSToken ScanStateMarker();
    
    // 辅助方法
    char CurrentChar() const;
    char NextChar();
    char PeekChar(size_t offset = 1) const;
    bool IsAtEnd() const;
    void Advance();
    void SkipWhitespace();
    bool Match(char expected);
    bool MatchSequence(const std::string& sequence);
    
    // 字符判断
    bool IsAlpha(char ch) const;
    bool IsDigit(char ch) const;
    bool IsAlphaNumeric(char ch) const;
    bool IsIdentifierStart(char ch) const;
    bool IsIdentifierChar(char ch) const;
    bool IsWhitespace(char ch) const;
    bool IsSelectorSpecialChar(char ch) const;
    
    // 创建Token
    CHTLJSToken MakeToken(CHTLJSTokenType type) const;
    CHTLJSToken MakeToken(CHTLJSTokenType type, const std::string& lexeme) const;
    CHTLJSToken MakeToken(CHTLJSTokenType type, const std::string& lexeme,
                         const CHTLJSToken::ValueType& value) const;
    CHTLJSToken MakeErrorToken(const std::string& message) const;
    
    // 错误处理
    void ReportError(const std::string& message) const;
    
    // 位置追踪
    CHTLJSTokenLocation GetCurrentLocation(size_t length = 1) const;
    void UpdatePosition(char ch);
    
    // 初始化静态数据
    static void InitializeKeywords();
    static bool s_Initialized;
};

} // namespace chtljs
} // namespace chtl