#include "ASTVisitor.h"
#include "ASTNodeV2.h"

namespace chtl {
namespace simple {

void DefaultASTVisitor::VisitDocument(DocumentNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitElement(ElementNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitText(TextNode* node) {
    // 文本节点没有子节点
}

void DefaultASTVisitor::VisitStyle(StyleNode* node) {
    // 样式节点没有子节点
}

void DefaultASTVisitor::VisitScript(ScriptNode* node) {
    // 脚本节点没有子节点
}

void DefaultASTVisitor::VisitTemplate(TemplateNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitCustom(CustomNode* node) {
    VisitChildren(node);
}

void DefaultASTVisitor::VisitAttribute(AttributeNode* node) {
    // 属性节点没有子节点
}

void DefaultASTVisitor::VisitChildren(ASTNode* node) {
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

} // namespace simple
} // namespace chtl