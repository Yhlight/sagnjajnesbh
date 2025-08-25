#pragma once
#include <memory>
#include <vector>
#include <string>
#include "CHTL/Lexer/Token.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/Compiler/Context.h"

namespace CHTL {
namespace Compiler {

// 解析器配置
struct ParserConfig {
    bool enableDebug = false;
    bool strictMode = false;  // 严格模式，更严格的语法检查
};

// CHTL解析器
class Parser {
public:
    explicit Parser(CompileContext* context, const ParserConfig& config = ParserConfig());
    ~Parser();
    
    // 解析token流，返回AST根节点
    std::shared_ptr<ASTNode> Parse(const std::vector<Token>& tokens);
    
    // 获取解析错误
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Compiler
} // namespace CHTL