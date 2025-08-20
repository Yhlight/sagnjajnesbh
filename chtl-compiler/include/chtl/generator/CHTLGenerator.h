#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "chtl/ast/ASTNode.h"
#include "chtl/core/CHTLGlobalMap.h"

namespace chtl {
namespace generator {

/**
 * 代码生成结果
 */
struct GenerateResult {
    bool Success;
    std::string Html;           // 生成的HTML内容
    std::string Css;            // 生成的CSS内容（来自局部样式块）
    std::string JavaScript;     // 生成的JavaScript内容（来自局部脚本块）
    std::vector<std::string> Errors;
    
    // 额外的元数据
    std::unordered_set<std::string> GeneratedClasses;  // 自动生成的类名
    std::unordered_set<std::string> GeneratedIds;      // 自动生成的ID
    std::unordered_map<std::string, std::string> TemplateUsage;  // 模板使用情况
};

/**
 * CHTL代码生成器
 * 负责将AST转换为HTML/CSS/JavaScript
 * 严格按照CHTL语法文档生成代码
 */
class CHTLGenerator : public ast::ASTVisitor {
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    /**
     * 生成代码
     * @param root AST根节点
     * @return 生成结果
     */
    GenerateResult Generate(ast::ASTNode* root);
    
    /**
     * 设置全局映射表
     */
    void SetGlobalMap(std::shared_ptr<compiler::CHTLGlobalMap> globalMap) {
        m_GlobalMap = globalMap;
    }
    
    /**
     * 设置生成选项
     */
    void SetPrettyPrint(bool pretty) { m_PrettyPrint = pretty; }
    void SetIndentSize(int size) { m_IndentSize = size; }
    void SetGenerateComments(bool generate) { m_GenerateComments = generate; }
    
    // 实现访问者接口
    void VisitDocument(ast::DocumentNode* node) override;
    void VisitComment(ast::CommentNode* node) override;
    void VisitText(ast::TextNode* node) override;
    void VisitElement(ast::ElementNode* node) override;
    void VisitAttribute(ast::AttributeNode* node) override;
    void VisitStyleBlock(ast::StyleBlockNode* node) override;
    void VisitScriptBlock(ast::ScriptBlockNode* node) override;
    void VisitTemplate(ast::TemplateNode* node) override;
    void VisitCustom(ast::CustomNode* node) override;
    void VisitImport(ast::ImportNode* node) override;
    void VisitNamespace(ast::NamespaceNode* node) override;
    void VisitConfiguration(ast::ConfigurationNode* node) override;
    void VisitOrigin(ast::OriginNode* node) override;
    void VisitExcept(ast::ExceptNode* node) override;
    void VisitLiteral(ast::LiteralNode* node) override;
    void VisitIdentifier(ast::IdentifierNode* node) override;
    void VisitReference(ast::ReferenceNode* node) override;
    
private:
    // 输出流
    std::stringstream m_HtmlStream;
    std::stringstream m_CssStream;
    std::stringstream m_JsStream;
    std::stringstream* m_CurrentStream;
    
    // 全局映射表
    std::shared_ptr<compiler::CHTLGlobalMap> m_GlobalMap;
    
    // 生成状态
    int m_IndentLevel;
    bool m_InStyleBlock;
    bool m_InScriptBlock;
    std::string m_CurrentElement;
    
    // 生成选项
    bool m_PrettyPrint;
    int m_IndentSize;
    bool m_GenerateComments;
    
    // 生成的元数据
    std::unordered_set<std::string> m_GeneratedClasses;
    std::unordered_set<std::string> m_GeneratedIds;
    std::unordered_map<std::string, int> m_AutoClassCounter;
    std::unordered_map<std::string, int> m_AutoIdCounter;
    
    // 错误收集
    std::vector<std::string> m_Errors;
    
    // 辅助方法
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteIndent();
    void IncreaseIndent() { m_IndentLevel++; }
    void DecreaseIndent() { if (m_IndentLevel > 0) m_IndentLevel--; }
    
    // HTML生成
    void GenerateElement(ast::ElementNode* node);
    void GenerateAttributes(ast::ElementNode* node);
    void GenerateSelfClosingElement(ast::ElementNode* node);
    void GenerateTextContent(const std::string& text);
    
    // 样式生成
    void GenerateStyleBlock(ast::StyleBlockNode* node);
    void GenerateInlineStyles(ast::StyleBlockNode* node);
    void GenerateStyleRules(ast::StyleBlockNode* node);
    void GenerateStyleReference(ast::StyleReferenceNode* node);
    std::string GenerateAutoClassName(const std::string& selector);
    std::string GenerateAutoId(const std::string& selector);
    
    // 脚本生成
    void GenerateScriptBlock(ast::ScriptBlockNode* node);
    std::string WrapLocalScript(const std::string& content, const std::string& element);
    
    // 模板处理
    void ProcessTemplateUsage(ast::TemplateNode* node);
    void ExpandTemplate(const std::string& templateName, const std::string& type);
    
    // 自定义处理
    void ProcessCustomUsage(ast::CustomNode* node);
    void ExpandCustom(const std::string& customName, const std::string& type);
    
    // 变量组处理
    std::string ResolveVarReference(const std::string& varGroup, const std::string& varName);
    
    // 原始嵌入处理
    void ProcessOriginBlock(ast::OriginNode* node);
    
    // 错误处理
    void ReportError(const std::string& message);
    
    // 特殊字符转义
    std::string EscapeHtml(const std::string& text);
    std::string EscapeCss(const std::string& text);
    std::string EscapeJs(const std::string& text);
};

} // namespace generator
} // namespace chtl