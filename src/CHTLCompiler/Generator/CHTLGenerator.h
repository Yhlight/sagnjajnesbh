#pragma once
#include "../AST/CHTLNodes.h"
#include <string>
#include <sstream>

namespace chtl {
namespace generator {

/**
 * CHTL HTML生成器
 * 将CHTL AST转换为HTML代码
 */
class HTMLGenerator : public ast::ASTVisitor {
public:
    HTMLGenerator();
    ~HTMLGenerator() = default;
    
    // 主生成接口
    std::string generateHTML(ast::ASTNode* root);
    
    // 访问者模式实现
    void visit(ast::ProgramNode& node) override;
    void visit(ast::TemplateNode& node) override;
    void visit(ast::CustomNode& node) override;
    void visit(ast::ElementNode& node) override;
    void visit(ast::StyleBlockNode& node) override;
    void visit(ast::ScriptBlockNode& node) override;
    void visit(ast::TextNode& node) override;
    void visit(ast::ImportNode& node) override;
    void visit(ast::NamespaceNode& node) override;
    void visit(ast::OriginNode& node) override;
    void visit(ast::ConfigurationNode& node) override;
    void visit(ast::ConstraintNode& node) override;
    void visit(ast::SpecializationNode& node) override;
    void visit(ast::IndexAccessNode& node) override;
    void visit(ast::VariableGroupNode& node) override;
    
    // 配置
    void setIndentSize(size_t size) { indent_size_ = size; }
    void setPrettyPrint(bool pretty) { pretty_print_ = pretty; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
private:
    std::ostringstream output_;
    size_t indent_level_;
    size_t indent_size_;
    bool pretty_print_;
    std::vector<std::string> errors_;
    
    // 辅助方法
    void writeIndent();
    void writeLine(const std::string& line);
    void write(const std::string& text);
    void addError(const std::string& error);
    
    // HTML生成辅助
    void generateElement(const std::string& tag, const std::string& content = "");
    void generateOpenTag(const std::string& tag);
    void generateCloseTag(const std::string& tag);
    void generateStyle(const std::string& css);
    void generateScript(const std::string& js);
};

} // namespace generator
} // namespace chtl