#pragma once
#include <memory>
#include <string>
#include "CHTL/AST/ASTNode.h"
#include "CHTL/Compiler/Context.h"

namespace CHTL {
namespace Compiler {

// 生成器配置
struct GeneratorConfig {
    bool prettyPrint = true;      // 美化输出
    bool generateComments = true; // 生成注释
    int indentSize = 2;          // 缩进大小
    bool useSpaces = true;       // 使用空格缩进（false则使用tab）
};

// CHTL代码生成器
class Generator {
public:
    explicit Generator(CompileContext* context, const GeneratorConfig& config = GeneratorConfig());
    ~Generator();
    
    // 生成HTML代码
    std::string Generate(std::shared_ptr<ASTNode> ast);
    
    // 获取生成的CSS（从局部样式块收集的）
    std::string GetGeneratedCSS() const;
    
    // 获取生成的JavaScript（从局部脚本块收集的）
    std::string GetGeneratedJS() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Compiler
} // namespace CHTL