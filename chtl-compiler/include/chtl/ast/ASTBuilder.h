#pragma once

#include <memory>
#include <stack>
#include "chtl/ast/ASTNode.h"
#include "chtl/token/CHTLToken.h"

namespace chtl {
namespace ast {

// 前向声明所有节点类型
class DocumentNode;
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class ScriptBlockNode;
class TemplateNode;
class CustomNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class OriginNode;

/**
 * AST构建器
 * 负责从Token流构建AST
 */
class ASTBuilder {
public:
    ASTBuilder();
    ~ASTBuilder();
    
    /**
     * 从Token流构建AST
     * @param tokens Token列表
     * @return AST根节点
     */
    std::shared_ptr<DocumentNode> Build(const std::vector<compiler::CHTLToken>& tokens);
    
    /**
     * 获取构建错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 当前处理的Token索引
    size_t m_CurrentIndex;
    std::vector<compiler::CHTLToken> m_Tokens;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    
    // 构建栈（用于嵌套结构）
    std::stack<std::shared_ptr<ASTNode>> m_BuildStack;
    
    // Token访问方法
    const compiler::CHTLToken& Current() const;
    const compiler::CHTLToken& Peek(size_t offset = 1) const;
    bool IsAtEnd() const;
    void Advance();
    bool Match(compiler::CHTLTokenType type);
    bool Check(compiler::CHTLTokenType type) const;
    
    // 构建方法
    std::shared_ptr<ASTNode> BuildNode();
    std::shared_ptr<ElementNode> BuildElement();
    std::shared_ptr<TextNode> BuildText();
    std::shared_ptr<AttributeNode> BuildAttribute();
    std::shared_ptr<StyleBlockNode> BuildStyleBlock();
    std::shared_ptr<ScriptBlockNode> BuildScriptBlock();
    std::shared_ptr<TemplateNode> BuildTemplate();
    std::shared_ptr<CustomNode> BuildCustom();
    std::shared_ptr<ImportNode> BuildImport();
    std::shared_ptr<NamespaceNode> BuildNamespace();
    std::shared_ptr<ConfigurationNode> BuildConfiguration();
    std::shared_ptr<OriginNode> BuildOrigin();
    
    // 辅助方法
    std::string ParseStringLiteral();
    std::string ParseUnquotedLiteral();
    std::string ParseIdentifier();
    
    // 错误处理
    void ReportError(const std::string& message);
    void Synchronize();
};

/**
 * AST打印访问者
 * 用于调试输出AST结构
 */
class ASTPrinter : public ASTVisitor {
public:
    ASTPrinter();
    
    void Print(ASTNode* root);
    
    // 实现访问者接口
    void VisitDocument(DocumentNode* node) override;
    void VisitComment(CommentNode* node) override;
    void VisitText(TextNode* node) override;
    void VisitElement(ElementNode* node) override;
    void VisitAttribute(AttributeNode* node) override;
    void VisitStyleBlock(StyleBlockNode* node) override;
    void VisitScriptBlock(ScriptBlockNode* node) override;
    void VisitTemplate(TemplateNode* node) override;
    void VisitCustom(CustomNode* node) override;
    void VisitImport(ImportNode* node) override;
    void VisitNamespace(NamespaceNode* node) override;
    void VisitConfiguration(ConfigurationNode* node) override;
    void VisitOrigin(OriginNode* node) override;
    void VisitExcept(ExceptNode* node) override;
    void VisitLiteral(LiteralNode* node) override;
    void VisitIdentifier(IdentifierNode* node) override;
    void VisitReference(ReferenceNode* node) override;
    
private:
    int m_Indent;
    
    void PrintIndent();
    void IncreaseIndent() { m_Indent += 2; }
    void DecreaseIndent() { m_Indent -= 2; }
};

} // namespace ast
} // namespace chtl