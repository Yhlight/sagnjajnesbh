#pragma once

#include "../ast/CHTLASTNodes.h"
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chtl {

/**
 * CHTL HTML 生成器
 * 严格按照 CHTL 语法生成 HTML/CSS/JS
 */
class CHTLGeneratorV2 : public ast::ASTVisitor {
public:
    CHTLGeneratorV2();
    
    /**
     * 生成 HTML
     */
    std::string Generate(std::shared_ptr<ast::ASTNode> root);
    
    /**
     * 配置选项
     */
    void SetPrettyPrint(bool pretty) { m_PrettyPrint = pretty; }
    void SetIndentSize(int size) { m_IndentSize = size; }
    void SetGenerateFullDocument(bool full) { m_GenerateFullDocument = full; }
    
    // 访问者方法
    void VisitDocument(ast::DocumentNode* node) override;
    void VisitComment(ast::CommentNode* node) override;
    void VisitText(ast::TextNode* node) override;
    void VisitElement(ast::ElementNode* node) override;
    void VisitStyle(ast::StyleNode* node) override;
    void VisitScript(ast::ScriptNode* node) override;
    void VisitTemplate(ast::TemplateNode* node) override;
    void VisitCustom(ast::CustomNode* node) override;
    void VisitVar(ast::VarNode* node) override;
    void VisitInherit(ast::InheritNode* node) override;
    void VisitDelete(ast::DeleteNode* node) override;
    void VisitInsert(ast::InsertNode* node) override;
    void VisitImport(ast::ImportNode* node) override;
    void VisitNamespace(ast::NamespaceNode* node) override;
    void VisitConfiguration(ast::ConfigurationNode* node) override;
    void VisitOrigin(ast::OriginNode* node) override;
    void VisitExcept(ast::ExceptNode* node) override;
    
private:
    // 输出管理
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteIndent();
    void IncreaseIndent() { m_CurrentIndent++; }
    void DecreaseIndent() { if (m_CurrentIndent > 0) m_CurrentIndent--; }
    
    // 样式处理
    void ProcessLocalStyle(ast::StyleNode* style);
    void CollectGlobalStyle(ast::StyleNode* style);
    std::string GenerateStyleContent();
    
    // 脚本处理
    void ProcessLocalScript(ast::ScriptNode* script);
    void CollectGlobalScript(ast::ScriptNode* script);
    std::string GenerateScriptContent();
    
    // 模板处理
    void RegisterTemplate(ast::TemplateNode* tmpl);
    std::shared_ptr<ast::ASTNode> InstantiateTemplate(
        const std::string& name, 
        const std::unordered_map<std::string, std::string>& params
    );
    
    // 辅助方法
    void VisitChildren(ast::ASTNode* node);
    bool IsInlineElement(const std::string& tag) const;
    bool IsSelfClosingTag(const std::string& tag) const;
    std::string EscapeHTML(const std::string& text) const;
    
    // 变量替换
    std::string ReplaceVariables(const std::string& text);
    
    // 成员变量
    std::stringstream m_Output;
    std::stringstream m_GlobalStyles;
    std::stringstream m_GlobalScripts;
    std::vector<std::string> m_LocalStyleRules;
    
    bool m_PrettyPrint;
    int m_IndentSize;
    int m_CurrentIndent;
    bool m_GenerateFullDocument;
    
    // 模板存储
    std::unordered_map<std::string, std::shared_ptr<ast::TemplateNode>> m_Templates;
    
    // 变量存储
    std::unordered_map<std::string, std::string> m_Variables;
    
    // 命名空间上下文
    std::string m_CurrentNamespace;
    
    // 配置
    std::unordered_map<std::string, std::string> m_Configuration;
};

} // namespace chtl