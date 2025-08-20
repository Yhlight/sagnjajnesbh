#include "CHTLASTNodesV3.h"

namespace chtl {
namespace ast {
namespace v3 {

// 文档节点
void DocumentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDocument(this);
}

// 元素节点
void ElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitElement(this);
}

// 文本节点
void TextNode::Accept(ASTVisitor* visitor) {
    visitor->VisitText(this);
}

// 样式节点
void StyleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyle(this);
}

// 脚本节点
void ScriptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitScript(this);
}

// 注释节点
void CommentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitComment(this);
}

// 模板节点
void TemplateNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplate(this);
}

// 自定义节点
void CustomNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustom(this);
}

// 继承节点
void InheritNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInherit(this);
}

// 删除节点
void DeleteNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDelete(this);
}

// 插入节点
void InsertNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInsert(this);
}

// 变量节点
void VarNode::Accept(ASTVisitor* visitor) {
    visitor->VisitVar(this);
}

// 变量调用节点
void VarCallNode::Accept(ASTVisitor* visitor) {
    visitor->VisitVarCall(this);
}

// 导入节点
void ImportNode::Accept(ASTVisitor* visitor) {
    visitor->VisitImport(this);
}

// 命名空间节点
void NamespaceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitNamespace(this);
}

// 配置节点
void ConfigurationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitConfiguration(this);
}

// 原始嵌入节点
void OriginNode::Accept(ASTVisitor* visitor) {
    visitor->VisitOrigin(this);
}

// 约束节点
void ExceptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitExcept(this);
}

// 属性节点
void AttributeNode::Accept(ASTVisitor* visitor) {
    visitor->VisitAttribute(this);
}

} // namespace v3
} // namespace ast
} // namespace chtl