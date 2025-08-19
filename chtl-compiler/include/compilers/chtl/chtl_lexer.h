#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "compilers/chtl/chtl_token.h"
#include "compilers/chtl/chtl_context.h"

namespace chtl {

/**
 * CHTL词法分析器
 * 将源代码转换为Token流
 */
class CHTLLexer {
public:
    CHTLLexer(CHTLContext& context);
    ~CHTLLexer();
    
    /**
     * 词法分析
     * @param source 源代码
     * @return Token列表
     */
    std::vector<CHTLToken> tokenize(const std::string& source);
    
    /**
     * 扫描下一个Token
     * @return 下一个Token，如果到达末尾返回EOF_TOKEN
     */
    CHTLToken scanToken();
    
    /**
     * 重置词法分析器
     */
    void reset();
    
    /**
     * 获取当前位置
     */
    size_t getCurrentPos() const { return current_; }
    
    /**
     * 设置是否跳过空白和注释
     */
    void setSkipWhitespace(bool skip) { skipWhitespace_ = skip; }
    void setSkipComments(bool skip) { skipComments_ = skip; }
    
private:
    CHTLContext& context_;
    std::string source_;
    size_t start_;      // Token起始位置
    size_t current_;    // 当前扫描位置
    size_t line_;       // 当前行号
    size_t column_;     // 当前列号
    
    // 选项
    bool skipWhitespace_;
    bool skipComments_;
    
    // 关键字映射
    static const std::unordered_map<std::string, CHTLTokenType> keywords_;
    
    // @类型映射
    static const std::unordered_map<std::string, CHTLTokenType> atTypes_;
    
    // HTML标签集合（用于识别元素）
    static const std::unordered_set<std::string> htmlTags_;
    
    // 扫描方法
    CHTLToken identifier();
    CHTLToken number();
    CHTLToken string(char quote);
    CHTLToken unquotedLiteral();
    CHTLToken singleLineComment();
    CHTLToken multiLineComment();
    CHTLToken generatorComment();
    CHTLToken atType();
    
    // 辅助方法
    bool isAtEnd() const;
    char peek(int offset = 0) const;
    char advance();
    bool match(char expected);
    bool matchString(const std::string& expected);
    void skipWhitespace();
    
    // 字符判断
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isWhitespace(char c) const;
    
    // Token创建
    CHTLToken makeToken(CHTLTokenType type);
    CHTLToken makeToken(CHTLTokenType type, const std::string& value);
    template<typename T>
    CHTLToken makeToken(CHTLTokenType type, T value);
    CHTLToken errorToken(const std::string& message);
    
    // 位置更新
    void updatePosition(char ch);
    
    // 特殊处理
    bool isHTMLTag(const std::string& word) const;
    bool isUnquotedLiteralContext() const;
    bool shouldScanAsUnquotedLiteral() const;
    
    // 初始化静态数据
    static std::unordered_map<std::string, CHTLTokenType> initKeywords();
    static std::unordered_map<std::string, CHTLTokenType> initAtTypes();
    static std::unordered_set<std::string> initHTMLTags();
};

} // namespace chtl

#endif // CHTL_LEXER_H