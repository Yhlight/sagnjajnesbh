#include "CHTLASTNodes.h"

namespace chtl {
namespace ast {

void DocumentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDocument(this);
}

void CommentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitComment(this);
}

void TextNode::Accept(ASTVisitor* visitor) {
    visitor->VisitText(this);
}

void ElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitElement(this);
}

void StyleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyle(this);
}

void ScriptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitScript(this);
}

void TemplateNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplate(this);
}

void CustomNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustom(this);
}

void VarNode::Accept(ASTVisitor* visitor) {
    visitor->VisitVar(this);
}

void InheritNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInherit(this);
}

void DeleteNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDelete(this);
}

void InsertNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInsert(this);
}

void ImportNode::Accept(ASTVisitor* visitor) {
    visitor->VisitImport(this);
}

void NamespaceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitNamespace(this);
}

void ConfigurationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitConfiguration(this);
}

void OriginNode::Accept(ASTVisitor* visitor) {
    visitor->VisitOrigin(this);
}

void ExceptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitExcept(this);
}

} // namespace ast
} // namespace chtl