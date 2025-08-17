#include "../../include/CHTLASTNodes.h"
#include <sstream>

namespace chtl {

// CHTLASTNode 基类实现
void CHTLASTNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

void CHTLASTNode::setAttribute(const String& key, const String& value) {
    attributes_[key] = value;
}

std::optional<String> CHTLASTNode::getAttribute(const String& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

String CHTLASTNode::toString() const {
    std::ostringstream oss;
    oss << "Node(type=" << static_cast<int>(nodeType_) 
        << ", line=" << line_ 
        << ", column=" << column_ 
        << ", children=" << children_.size() << ")";
    return oss.str();
}

// RootNode 实现
void RootNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// ElementNode 实现
void ElementNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// TextNode 实现
void TextNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// AttributeNode 实现
void AttributeNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// StyleBlockNode 实现
void StyleBlockNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// CSSRuleNode 实现
void CSSRuleNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// CSSPropertyNode 实现
void CSSPropertyNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// ScriptBlockNode 实现
void ScriptBlockNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateStyleNode 实现
void TemplateStyleNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateElementNode 实现
void TemplateElementNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateVarNode 实现
std::optional<String> TemplateVarNode::getVariable(const String& varName) const {
    auto it = variables_.find(varName);
    if (it != variables_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void TemplateVarNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateUsageNode 实现
void TemplateUsageNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// VariableReferenceNode 实现
void VariableReferenceNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// VariableNode 实现
void VariableNode::accept(CHTLASTVisitor& visitor) {
    // 注意：VariableNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// DeleteNode 实现
void DeleteNode::accept(CHTLASTVisitor& visitor) {
    // 注意：DeleteNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// InheritNode 实现
void InheritNode::accept(CHTLASTVisitor& visitor) {
    // 注意：InheritNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// ExceptNode 实现
void ExceptNode::accept(CHTLASTVisitor& visitor) {
    // 注意：ExceptNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// ClassSelectorNode 实现
void ClassSelectorNode::accept(CHTLASTVisitor& visitor) {
    // 注意：ClassSelectorNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// IdSelectorNode 实现
void IdSelectorNode::accept(CHTLASTVisitor& visitor) {
    // 注意：IdSelectorNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// PseudoSelectorNode 实现
void PseudoSelectorNode::accept(CHTLASTVisitor& visitor) {
    // 注意：PseudoSelectorNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// InsertNode 实现
void InsertNode::accept(CHTLASTVisitor& visitor) {
    // 注意：InsertNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// SpecializationNode 实现
void SpecializationNode::accept(CHTLASTVisitor& visitor) {
    // 注意：SpecializationNode 没有在访问者接口中定义，暂时留空或添加到接口
}

// CustomStyleNode 实现
void CustomStyleNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// CustomElementNode 实现
void CustomElementNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// CustomVarNode 实现
std::optional<String> CustomVarNode::getVariable(const String& varName) const {
    auto it = variables_.find(varName);
    if (it != variables_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void CustomVarNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// ImportNode 实现
void ImportNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceNode 实现
void NamespaceNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// OriginNode 实现
void OriginNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

// ConstraintNode 实现
void ConstraintNode::accept(CHTLASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace chtl