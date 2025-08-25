#pragma once
#include <memory>
#include <vector>
#include <string>
#include "CHTL/Lexer/Token.h"

namespace CHTL {
namespace Compiler {

// 词法分析器配置
struct LexerConfig {
    bool skipComments = true;           // 是否跳过普通注释
    bool preserveGeneratorComments = true; // 是否保留生成器注释
    bool enableDebug = false;           // 调试模式
};

// CHTL词法分析器
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

} // namespace Compiler
} // namespace CHTL