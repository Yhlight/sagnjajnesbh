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
class CHTLGeneratorV3 : public ast::ASTVisitor {
public:
    CHTLGeneratorV3();
    
    // 主要生成接口
    std::string Generate(std::shared_ptr<ast::DocumentNode> document, 
                        const GeneratorOptions& options = GeneratorOptions());
    
    // 访问者方法
    void VisitDocument(ast::DocumentNode* node) override;
    void VisitElement(ast::ElementNode* node) override;
    void VisitText(ast::TextNode* node) override;
    void VisitStyle(ast::StyleNode* node) override;
    void VisitScript(ast::ScriptNode* node) override;
    void VisitComment(ast::CommentNode* node) override;
    void VisitTemplate(ast::TemplateNode* node) override;
    void VisitCustom(ast::CustomNode* node) override;
    void VisitAttribute(ast::AttributeNode* node) override;
    void VisitVar(ast::VarNode* node) override;
    void VisitVarCall(ast::VarCallNode* node) override;
    void VisitImport(ast::ImportNode* node) override;
    void VisitNamespace(ast::NamespaceNode* node) override;
    void VisitConfiguration(ast::ConfigurationNode* node) override;
    void VisitOrigin(ast::OriginNode* node) override;
    void VisitInherit(ast::InheritNode* node) override;
    void VisitDelete(ast::DeleteNode* node) override;
    void VisitInsert(ast::InsertNode* node) override;
    void VisitExcept(ast::ExceptNode* node) override;

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
    void ProcessLocalStyle(ast::StyleNode* style);
    void CollectGlobalStyle(ast::StyleNode* style);
    std::string GenerateStyleContent();
    void CollectTemplateStyleProperties(const std::string& templateName,
                                       std::unordered_map<std::string, std::string>& properties,
                                       std::unordered_set<std::string>& visitedTemplates);
    void ProcessStyleInheritance(ast::StyleNode* style,
                                std::unordered_map<std::string, std::string>& properties);
    
    // 脚本处理
    void ProcessLocalScript(ast::ScriptNode* script);
    void CollectGlobalScript(ast::ScriptNode* script);
    std::string GenerateScriptContent();
    
    // 模板处理
    void RegisterTemplate(ast::TemplateNode* tmpl);
    void InstantiateTemplate(const std::string& templateName, ast::CustomNode* usage);
    void ProcessTemplateInheritance(ast::TemplateNode* tmpl);
    
    // 自定义处理
    void ProcessCustomDefinition(ast::CustomNode* custom);
    void ProcessCustomUsage(ast::CustomNode* custom);
    void ProcessSpecialization(ast::CustomNode* custom);
    
    // 变量处理
    void RegisterVariable(ast::VarNode* var);
    std::string ResolveVariable(const std::string& varGroup, const std::string& varName);
    std::string ProcessVarCall(ast::VarCallNode* varCall);
    
    // 导入处理
    void ProcessImport(ast::ImportNode* import);
    std::string ResolvePath(const std::string& path, ast::ImportNode::ImportType type);
    void ImportFile(const std::string& resolvedPath, ast::ImportNode* import);
    
    // 命名空间处理
    void EnterNamespace(const std::string& name);
    void ExitNamespace();
    std::string ResolveFullQualifiedName(const std::string& name);
    
    // 配置处理
    void ProcessConfiguration(ast::ConfigurationNode* config);
    std::string GetConfig(const std::string& key);
    
    // 约束检查
    bool CheckConstraints(const std::string& element);
    void AddConstraint(ast::ExceptNode* except);
    
    // 特殊化处理
    void ProcessDelete(ast::DeleteNode* deleteNode);
    void ProcessInsert(ast::InsertNode* insertNode);
    
    // 辅助方法
    void VisitChildren(ast::ASTNode* node);
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
    std::unordered_map<std::string, std::shared_ptr<ast::TemplateNode>> m_Templates;
    std::unordered_map<std::string, std::shared_ptr<ast::CustomNode>> m_Customs;
    
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
    std::vector<ast::ExceptNode*> m_Constraints;
    
    // 导入缓存
    std::unordered_set<std::string> m_ImportedFiles;
    
    // 当前处理上下文
    struct ProcessingContext {
        ast::ElementNode* currentElement = nullptr;
        ast::StyleNode* currentStyle = nullptr;
        ast::CustomNode* currentCustom = nullptr;
        bool inSpecialization = false;
    };
    ProcessingContext m_Context;
};

} // namespace chtl