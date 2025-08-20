#pragma once

#include <string>
#include <memory>
#include <vector>

namespace chtl {

// 前向声明
namespace ast { namespace v3 { class DocumentNode; } }
class CHTLParserV3;
class CHTLGeneratorV3;
struct GeneratorOptions;

// 编译选项
struct CompilerOptions {
    bool debugMode = false;
    bool prettyPrint = true;
    bool minify = false;
    bool fragmentOnly = false;
    bool enableImports = true;
    bool strictMode = true;
    std::vector<std::string> importPaths;
};

// 完整的CHTL编译器V3
class CHTLCompilerV3 {
public:
    CHTLCompilerV3();
    ~CHTLCompilerV3();
    
    // 主要编译接口
    bool CompileFile(const std::string& inputFile, const std::string& outputFile,
                    const CompilerOptions& options = CompilerOptions());
    
    std::string CompileString(const std::string& source, const std::string& filename = "",
                             const CompilerOptions& options = CompilerOptions());
    
    // 错误处理
    bool HasErrors() const { return !m_Errors.empty(); }
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    // 警告
    bool HasWarnings() const { return !m_Warnings.empty(); }
    const std::vector<std::string>& GetWarnings() const { return m_Warnings; }
    
    // AST访问（用于调试）
    std::shared_ptr<ast::v3::DocumentNode> GetAST() const { return m_AST; }
    
    // 版本信息
    static std::string GetVersion() { return "3.0.0"; }
    static std::string GetLanguageVersion() { return "CHTL 1.0"; }

private:
    std::unique_ptr<CHTLParserV3> m_Parser;
    std::unique_ptr<CHTLGeneratorV3> m_Generator;
    std::shared_ptr<ast::v3::DocumentNode> m_AST;
    
    std::vector<std::string> m_Errors;
    std::vector<std::string> m_Warnings;
    
    CompilerOptions m_Options;
    
    // 编译流程
    bool Parse(const std::string& source, const std::string& filename);
    std::string Generate();
    
    // 文件处理
    std::string ReadFile(const std::string& filename);
    bool WriteFile(const std::string& filename, const std::string& content);
    
    // 错误收集
    void CollectParserErrors();
    void AddError(const std::string& error);
    void AddWarning(const std::string& warning);
};

} // namespace chtl