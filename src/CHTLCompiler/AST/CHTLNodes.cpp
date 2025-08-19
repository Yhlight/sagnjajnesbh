#include "CHTLNodes.h"

namespace chtl {
namespace ast {

// ProgramNode实现
void ProgramNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateNode实现
void TemplateNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// CustomNode实现
void CustomNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ElementNode实现
void ElementNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// StyleBlockNode实现
void StyleBlockNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ScriptBlockNode实现
void ScriptBlockNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// TextNode实现
void TextNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ImportNode实现
void ImportNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceNode实现
void NamespaceNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ConfigurationNode实现
void ConfigurationNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ConstraintNode实现
void ConstraintNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// SpecializationNode实现
void SpecializationNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// IndexAccessNode实现
void IndexAccessNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// OriginNode实现
void OriginNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// VariableGroupNode实现
void VariableGroupNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace ast
} // namespace chtl