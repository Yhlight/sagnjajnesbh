#pragma once

#include "../ast/CHTLASTNodesV3.h"
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>

namespace chtl {

// 生成器选项
struct GeneratorOptions {
    bool prettyPrint = true;
    bool minify = false;
    bool fragmentOnly = false;
    bool debugMode = false;
};

// 完整实现所有CHTL语法特征的生成器
class CHTLGeneratorV3 : public ast::v3::ASTVisitor {
public:
    CHTLGeneratorV3();
    
    // 主要生成接口
    std::string Generate(std::shared_ptr<ast::v3::DocumentNode> document, 
                        const GeneratorOptions& options = GeneratorOptions());
    
    // 访问者方法
    void VisitDocument(ast::v3::DocumentNode* node) override;
    void VisitElement(ast::v3::ElementNode* node) override;
    void VisitText(ast::v3::TextNode* node) override;
    void VisitStyle(ast::v3::StyleNode* node) override;
    void VisitScript(ast::v3::ScriptNode* node) override;
    void VisitComment(ast::v3::CommentNode* node) override;
    void VisitTemplate(ast::v3::TemplateNode* node) override;
    void VisitCustom(ast::v3::CustomNode* node) override;
    void VisitAttribute(ast::v3::AttributeNode* node) override;
    void VisitVar(ast::v3::VarNode* node) override;
    void VisitVarCall(ast::v3::VarCallNode* node) override;
    void VisitImport(ast::v3::ImportNode* node) override;
    void VisitNamespace(ast::v3::NamespaceNode* node) override;
    void VisitConfiguration(ast::v3::ConfigurationNode* node) override;
    void VisitOrigin(ast::v3::OriginNode* node) override;
    void VisitInherit(ast::v3::InheritNode* node) override;
    void VisitDelete(ast::v3::DeleteNode* node) override;
    void VisitInsert(ast::v3::InsertNode* node) override;
    void VisitExcept(ast::v3::ExceptNode* node) override;

private:
    // 输出管理
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteIndent();
    void IncreaseIndent();
    void DecreaseIndent();
    
    // 最终输出生成
    std::string GenerateFinalOutput();
    
    // 样式处理
    void ProcessLocalStyle(ast::v3::StyleNode* style);
    void CollectGlobalStyle(ast::v3::StyleNode* style);
    std::string GenerateStyleContent();
    void CollectTemplateStyleProperties(const std::string& templateName,
                                       std::unordered_map<std::string, std::string>& properties,
                                       std::unordered_set<std::string>& visitedTemplates);
    void ProcessStyleInheritance(ast::v3::StyleNode* style,
                                std::unordered_map<std::string, std::string>& properties);
    
    // 脚本处理
    void ProcessLocalScript(ast::v3::ScriptNode* script);
    void CollectGlobalScript(ast::v3::ScriptNode* script);
    std::string GenerateScriptContent();
    
    // 模板处理
    void RegisterTemplate(ast::v3::TemplateNode* tmpl);
    void InstantiateTemplate(const std::string& templateName, ast::v3::CustomNode* usage);
    void ProcessTemplateInheritance(ast::v3::TemplateNode* tmpl);
    
    // 自定义处理
    void ProcessCustomDefinition(ast::v3::CustomNode* custom);
    void ProcessCustomUsage(ast::v3::CustomNode* custom);
    void ProcessSpecialization(ast::v3::CustomNode* custom);
    
    // 变量处理
    void RegisterVariable(ast::v3::VarNode* var);
    std::string ResolveVariable(const std::string& varGroup, const std::string& varName);
    std::string ProcessVarCall(ast::v3::VarCallNode* varCall);
    
    // 导入处理
    void ProcessImport(ast::v3::ImportNode* import);
    std::string ResolvePath(const std::string& path, ast::v3::ImportNode::ImportType type);
    void ImportFile(const std::string& resolvedPath, ast::v3::ImportNode* import);
    
    // 命名空间处理
    void EnterNamespace(const std::string& name);
    void ExitNamespace();
    std::string ResolveFullQualifiedName(const std::string& name);
    
    // 配置处理
    void ProcessConfiguration(ast::v3::ConfigurationNode* config);
    std::string GetConfig(const std::string& key);
    
    // 约束检查
    bool CheckConstraints(const std::string& element);
    void AddConstraint(ast::v3::ExceptNode* except);
    
    // 特殊化处理
    void ProcessDelete(ast::v3::DeleteNode* deleteNode);
    void ProcessInsert(ast::v3::InsertNode* insertNode);
    
    // HTML元素验证
    bool IsHtmlElement(const std::string& tag) const;
    
    // 辅助方法
    void VisitChildren(ast::v3::ASTNode* node);
    bool IsInlineElement(const std::string& tag) const;
    bool IsSelfClosingTag(const std::string& tag) const;
    std::string EscapeHTML(const std::string& text) const;
    
    // 成员变量
    std::stringstream m_Output;
    std::stringstream m_GlobalStyles;
    std::stringstream m_GlobalScripts;
    std::vector<std::string> m_LocalStyleRules;
    
    GeneratorOptions m_Options;
    int m_IndentLevel;
    
    // 模板存储
    std::unordered_map<std::string, std::shared_ptr<ast::v3::TemplateNode>> m_Templates;
    std::unordered_map<std::string, std::shared_ptr<ast::v3::CustomNode>> m_Customs;
    
    // 变量存储（支持变量组）
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Variables;
    
    // 命名空间栈
    std::stack<std::string> m_NamespaceStack;
    std::string m_CurrentNamespace;
    
    // 配置存储
    std::unordered_map<std::string, std::string> m_Configuration;
    std::unordered_map<std::string, std::vector<std::string>> m_NameConfiguration;
    std::unordered_map<std::string, std::string> m_OriginTypes;
    
    // 约束存储
    std::vector<ast::v3::ExceptNode*> m_Constraints;
    
    // 导入缓存
    std::unordered_set<std::string> m_ImportedFiles;
    
    // 当前处理上下文
    struct ProcessingContext {
        ast::v3::ElementNode* currentElement = nullptr;
        ast::v3::StyleNode* currentStyle = nullptr;
        ast::v3::CustomNode* currentCustom = nullptr;
        bool inSpecialization = false;
    };
    ProcessingContext m_Context;
};

} // namespace chtl