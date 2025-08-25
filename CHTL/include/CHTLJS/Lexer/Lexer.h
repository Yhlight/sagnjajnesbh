#pragma once
#include <memory>
#include <vector>
#include <string>
#include "CHTLJS/Lexer/Token.h"

namespace CHTL {
namespace JSCompiler {

// CHTL JS词法分析器配置
struct LexerConfig {
    bool enableDebug = false;           // 调试模式
    bool preserveJSCode = true;         // 保留原始JS代码
};

// CHTL JS词法分析器
class Lexer {
public:
    explicit Lexer(const LexerConfig& config = LexerConfig());
    ~Lexer();
    
    // 对字符串进行词法分析
    std::vector<Token> Tokenize(const std::string& source);
    
    // 对文件进行词法分析
    std::vector<Token> TokenizeFile(const std::string& filePath);
    
    // 获取最后的错误信息
    std::string GetLastError() const;
    bool HasError() const;
    
    // 调试功能
    void DumpTokens(const std::vector<Token>& tokens) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace JSCompiler
} // namespace CHTL