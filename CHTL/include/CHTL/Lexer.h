#ifndef CHTL_CHTL_LEXER_H
#define CHTL_CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "CHTL/Token.h"

namespace CHTL {

// 词法分析器配置
struct LexerConfig {
    bool skipComments = false;          // 是否跳过注释
    bool preserveWhitespace = false;    // 是否保留空白字符
    bool strictMode = false;            // 严格模式
    bool debugMode = false;             // 调试模式
};

// 词法分析器类
class Lexer {
public:
    Lexer();
    explicit Lexer(const LexerConfig& config);
    ~Lexer();
    
    // 设置输入
    void SetInput(const std::string& input, const std::string& sourceName = "<input>");
    
    // Token操作
    Token NextToken();
    Token PeekToken();
    Token PeekToken(size_t ahead);
    void ConsumeToken();
    bool HasMoreTokens() const;
    
    // 位置信息
    size_t GetCurrentLine() const;
    size_t GetCurrentColumn() const;
    size_t GetCurrentPosition() const;
    std::string GetSourceName() const;
    
    // 错误处理
    using ErrorCallback = std::function<void(const std::string& message, 
                                           size_t line, size_t column)>;
    void SetErrorCallback(ErrorCallback callback) { errorCallback_ = callback; }
    
    // 配置
    void SetConfig(const LexerConfig& config) { config_ = config; }
    const LexerConfig& GetConfig() const { return config_; }
    
    // 工具方法
    std::vector<Token> TokenizeAll();
    void Reset();
    
    // 调试
    void DumpTokens();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    LexerConfig config_;
    ErrorCallback errorCallback_;
    
    // 内部方法
    void SkipWhitespace();
    void SkipComment();
    Token ScanComment();
    Token ScanIdentifier();
    Token ScanNumber();
    Token ScanString(char quote);
    Token ScanUnquotedLiteral();
    Token ScanOperator();
    Token ScanSpecialMarker();
    Token ScanAtMarker();
    
    // 辅助方法
    bool IsIdentifierStart(char ch) const;
    bool IsIdentifierPart(char ch) const;
    bool IsDigit(char ch) const;
    bool IsWhitespace(char ch) const;
    bool IsOperator(char ch) const;
    
    // 错误报告
    void ReportError(const std::string& message);
    void ReportWarning(const std::string& message);
};

// 词法分析工具类
class LexerUtils {
public:
    // 转义字符处理
    static std::string UnescapeString(const std::string& str);
    static std::string EscapeString(const std::string& str);
    
    // 字符判断
    static bool IsValidIdentifier(const std::string& str);
    static bool IsValidNumber(const std::string& str);
    static bool IsValidString(const std::string& str);
    
    // Token序列处理
    static std::vector<Token> FilterComments(const std::vector<Token>& tokens);
    static std::vector<Token> FilterWhitespace(const std::vector<Token>& tokens);
    
    // 调试输出
    static std::string TokensToString(const std::vector<Token>& tokens);
};

} // namespace CHTL

#endif // CHTL_CHTL_LEXER_H