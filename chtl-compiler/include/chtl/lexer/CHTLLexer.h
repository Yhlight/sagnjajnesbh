#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "chtl/token/CHTLToken.h"

namespace chtl {
namespace compiler {

/**
 * CHTL 词法分析器
 * 将源代码转换为Token流
 * 注意：这是CHTL专用的词法分析器，不处理CHTL JS内容
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();
    
    /**
     * 初始化词法分析器
     * @param source 源代码
     * @param filename 文件名（用于错误报告）
     */
    void Initialize(const std::string& source, const std::string& filename = "");
    
    /**
     * 获取下一个Token
     * @return 下一个Token，如果到达文件末尾则返回END_OF_FILE
     */
    CHTLToken NextToken();
    
    /**
     * 查看下一个Token但不消费
     * @param lookahead 向前查看的数量（默认1）
     * @return Token
     */
    CHTLToken PeekToken(size_t lookahead = 1);
    
    /**
     * 获取所有Token
     * @return Token列表
     */
    std::vector<CHTLToken> GetAllTokens();
    
    /**
     * 重置词法分析器
     */
    void Reset();
    
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
    
    // Token缓存（用于Peek）
    std::vector<CHTLToken> m_TokenCache;
    
    // 错误信息
    mutable std::vector<std::string> m_Errors;
    
    // 关键字映射表
    static const std::unordered_map<std::string, CHTLTokenType> s_Keywords;
    static const std::unordered_map<std::string, CHTLTokenType> s_AtKeywords;
    static const std::unordered_map<std::string, CHTLTokenType> s_BracketKeywords;
    
    // 核心方法
    CHTLToken ScanToken();
    
    // 扫描具体类型
    CHTLToken ScanIdentifierOrKeyword();
    CHTLToken ScanString(char delimiter);
    CHTLToken ScanNumber();
    CHTLToken ScanComment();
    CHTLToken ScanGeneratorComment();
    CHTLToken ScanBracketKeyword();
    CHTLToken ScanAtKeyword();
    CHTLToken ScanUnquotedLiteral();
    
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
    
    // 创建Token
    CHTLToken MakeToken(CHTLTokenType type) const;
    CHTLToken MakeToken(CHTLTokenType type, const std::string& lexeme) const;
    CHTLToken MakeToken(CHTLTokenType type, const std::string& lexeme,
                       const CHTLToken::ValueType& value) const;
    CHTLToken MakeErrorToken(const std::string& message) const;
    
    // 错误处理
    void ReportError(const std::string& message) const;
    
    // 位置追踪
    TokenLocation GetCurrentLocation(size_t length = 1) const;
    void UpdatePosition(char ch);
    
    // 初始化静态数据
    static void InitializeKeywords();
    static bool s_Initialized;
};

} // namespace compiler
} // namespace chtl