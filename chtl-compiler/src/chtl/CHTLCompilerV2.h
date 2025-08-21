#pragma once

#include <string>
#include <memory>
#include <vector>

namespace chtl {

// 前向声明
namespace ast { class DocumentNode; }
class CHTLParserV2;
class CHTLGeneratorV2;

/**
 * CHTL 编译器主类
 * 整合词法分析、语法分析和代码生成
 */
class CHTLCompilerV2 {
public:
    CHTLCompilerV2();
    ~CHTLCompilerV2();
    
    /**
     * 编译 CHTL 源代码
     */
    std::string Compile(const std::string& source, const std::string& filename = "");
    
    /**
     * 编译文件
     */
    std::string CompileFile(const std::string& filename);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
    /**
     * 配置选项
     */
    void SetPrettyPrint(bool pretty);
    void SetGenerateFullDocument(bool full);
    void SetDebugMode(bool debug);
    
    /**
     * 获取 AST（用于调试）
     */
    std::shared_ptr<ast::DocumentNode> GetAST() const { return m_AST; }
    
private:
    std::unique_ptr<CHTLParserV2> m_Parser;
    std::unique_ptr<CHTLGeneratorV2> m_Generator;
    std::shared_ptr<ast::DocumentNode> m_AST;
    std::vector<std::string> m_Errors;
    bool m_DebugMode;
};

} // namespace chtl