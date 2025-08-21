#ifndef CHTL_AST_ASTVISITOR_H
#define CHTL_AST_ASTVISITOR_H

namespace CHTL {
namespace AST {

// 前向声明所有节点类
class ASTNode;
class ProgramNode;
class ElementNode;
class AttributeNode;
class TextBlockNode;
class StyleBlockNode;
class ScriptBlockNode;
class IdentifierNode;
class StringLiteralNode;
class TemplateDeclarationNode;
class CustomDeclarationNode;
class OriginBlockNode;
class ImportStatementNode;
class NamespaceDeclarationNode;
class ConfigurationBlockNode;
class SpecializationNode;
class InsertBlockNode;
class DeleteBlockNode;
class ExceptBlockNode;
class InheritStatementNode;
class StyleRuleNode;
class StyleSelectorNode;
class StylePropertyNode;

// AST访问者接口
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础节点
    virtual void VisitProgramNode(ProgramNode* node) = 0;
    virtual void VisitElementNode(ElementNode* node) = 0;
    virtual void VisitAttributeNode(AttributeNode* node) = 0;
    virtual void VisitTextBlockNode(TextBlockNode* node) = 0;
    
    // 样式相关节点
    virtual void VisitStyleBlockNode(StyleBlockNode* node) = 0;
    virtual void VisitStyleRuleNode(StyleRuleNode* node) = 0;
    virtual void VisitStyleSelectorNode(StyleSelectorNode* node) = 0;
    virtual void VisitStylePropertyNode(StylePropertyNode* node) = 0;
    
    // 脚本节点
    virtual void VisitScriptBlockNode(ScriptBlockNode* node) = 0;
    
    // 模板和自定义节点
    virtual void VisitTemplateDeclarationNode(TemplateDeclarationNode* node) = 0;
    virtual void VisitCustomDeclarationNode(CustomDeclarationNode* node) = 0;
    
    // 特殊节点
    virtual void VisitOriginBlockNode(OriginBlockNode* node) = 0;
    virtual void VisitImportStatementNode(ImportStatementNode* node) = 0;
    virtual void VisitNamespaceDeclarationNode(NamespaceDeclarationNode* node) = 0;
    virtual void VisitConfigurationBlockNode(ConfigurationBlockNode* node) = 0;
    
    // 特殊语法节点
    virtual void VisitSpecializationNode(SpecializationNode* node) = 0;
    virtual void VisitInsertBlockNode(InsertBlockNode* node) = 0;
    virtual void VisitDeleteBlockNode(DeleteBlockNode* node) = 0;
    virtual void VisitExceptBlockNode(ExceptBlockNode* node) = 0;
    virtual void VisitInheritStatementNode(InheritStatementNode* node) = 0;
    
    // 表达式节点
    virtual void VisitIdentifierNode(IdentifierNode* node) = 0;
    virtual void VisitStringLiteralNode(StringLiteralNode* node) = 0;
};

// 基础访问者实现，提供默认行为
class BaseASTVisitor : public ASTVisitor {
public:
    // 默认实现：访问所有子节点
    void VisitProgramNode(ProgramNode* node) override;
    void VisitElementNode(ElementNode* node) override;
    void VisitAttributeNode(AttributeNode* node) override;
    void VisitTextBlockNode(TextBlockNode* node) override;
    void VisitStyleBlockNode(StyleBlockNode* node) override;
    void VisitStyleRuleNode(StyleRuleNode* node) override;
    void VisitStyleSelectorNode(StyleSelectorNode* node) override;
    void VisitStylePropertyNode(StylePropertyNode* node) override;
    void VisitScriptBlockNode(ScriptBlockNode* node) override;
    void VisitTemplateDeclarationNode(TemplateDeclarationNode* node) override;
    void VisitCustomDeclarationNode(CustomDeclarationNode* node) override;
    void VisitOriginBlockNode(OriginBlockNode* node) override;
    void VisitImportStatementNode(ImportStatementNode* node) override;
    void VisitNamespaceDeclarationNode(NamespaceDeclarationNode* node) override;
    void VisitConfigurationBlockNode(ConfigurationBlockNode* node) override;
    void VisitSpecializationNode(SpecializationNode* node) override;
    void VisitInsertBlockNode(InsertBlockNode* node) override;
    void VisitDeleteBlockNode(DeleteBlockNode* node) override;
    void VisitExceptBlockNode(ExceptBlockNode* node) override;
    void VisitInheritStatementNode(InheritStatementNode* node) override;
    void VisitIdentifierNode(IdentifierNode* node) override;
    void VisitStringLiteralNode(StringLiteralNode* node) override;
    
protected:
    // 辅助方法：访问节点的所有子节点
    void VisitChildren(ASTNode* node);
};

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_ASTVISITOR_H