#include "CHTL/AST/ASTVisitor.h"
#include "CHTL/AST/ASTNode.h"

namespace CHTL {
namespace AST {

// BaseASTVisitor实现 - 提供默认的遍历行为
void BaseASTVisitor::VisitChildren(ASTNode* node) {
    if (!node) return;
    
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

void BaseASTVisitor::VisitProgramNode(ProgramNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitElementNode(ElementNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitAttributeNode(AttributeNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitTextBlockNode(TextBlockNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitStyleBlockNode(StyleBlockNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitStyleRuleNode(StyleRuleNode* node) {
    // TODO: 实现样式规则节点的访问
    (void)node;
}

void BaseASTVisitor::VisitStyleSelectorNode(StyleSelectorNode* node) {
    // TODO: 实现样式选择器节点的访问
    (void)node;
}

void BaseASTVisitor::VisitStylePropertyNode(StylePropertyNode* node) {
    // TODO: 实现样式属性节点的访问
    (void)node;
}

void BaseASTVisitor::VisitScriptBlockNode(ScriptBlockNode* node) {
    VisitChildren(node);
}

void BaseASTVisitor::VisitTemplateDeclarationNode(TemplateDeclarationNode* node) {
    // TODO: 实现模板声明节点的访问
    (void)node;
}

void BaseASTVisitor::VisitCustomDeclarationNode(CustomDeclarationNode* node) {
    // TODO: 实现自定义声明节点的访问
    (void)node;
}

void BaseASTVisitor::VisitOriginBlockNode(OriginBlockNode* node) {
    // TODO: 实现原始块节点的访问
    (void)node;
}

void BaseASTVisitor::VisitImportStatementNode(ImportStatementNode* node) {
    // TODO: 实现导入语句节点的访问
    (void)node;
}

void BaseASTVisitor::VisitNamespaceDeclarationNode(NamespaceDeclarationNode* node) {
    // TODO: 实现命名空间声明节点的访问
    (void)node;
}

void BaseASTVisitor::VisitConfigurationBlockNode(ConfigurationBlockNode* node) {
    // TODO: 实现配置块节点的访问
    (void)node;
}

void BaseASTVisitor::VisitSpecializationNode(SpecializationNode* node) {
    // TODO: 实现特例化节点的访问
    (void)node;
}

void BaseASTVisitor::VisitInsertBlockNode(InsertBlockNode* node) {
    // TODO: 实现插入块节点的访问
    (void)node;
}

void BaseASTVisitor::VisitDeleteBlockNode(DeleteBlockNode* node) {
    // TODO: 实现删除块节点的访问
    (void)node;
}

void BaseASTVisitor::VisitExceptBlockNode(ExceptBlockNode* node) {
    // TODO: 实现异常块节点的访问
    (void)node;
}

void BaseASTVisitor::VisitInheritStatementNode(InheritStatementNode* node) {
    // TODO: 实现继承语句节点的访问
    (void)node;
}

void BaseASTVisitor::VisitIdentifierNode(IdentifierNode* node) {
    // 叶子节点，无需访问子节点
    (void)node;
}

void BaseASTVisitor::VisitStringLiteralNode(StringLiteralNode* node) {
    // 叶子节点，无需访问子节点
    (void)node;
}

} // namespace AST
} // namespace CHTL