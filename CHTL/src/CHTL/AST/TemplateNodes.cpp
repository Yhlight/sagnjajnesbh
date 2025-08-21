#include "CHTL/AST/TemplateNodes.h"
#include <sstream>

namespace CHTL {
namespace AST {

// 模板类型名称转换
std::string GetTemplateTypeName(TemplateType type) {
    switch (type) {
        case TemplateType::Style: return "@Style";
        case TemplateType::Element: return "@Element";
        case TemplateType::Var: return "@Var";
        default: return "Unknown";
    }
}

// 插入位置名称转换
std::string GetInsertPositionName(InsertBlockNode::InsertPosition position) {
    switch (position) {
        case InsertBlockNode::InsertPosition::After: return "after";
        case InsertBlockNode::InsertPosition::Before: return "before";
        case InsertBlockNode::InsertPosition::Replace: return "replace";
        case InsertBlockNode::InsertPosition::AtTop: return "at top";
        case InsertBlockNode::InsertPosition::AtBottom: return "at bottom";
        default: return "unknown";
    }
}

// TemplateDeclarationNode实现
TemplateDeclarationNode::TemplateDeclarationNode(TemplateType templateType, 
                                                 const std::string& name, 
                                                 const Token& token)
    : ASTNode(NodeType::TemplateDeclaration, token), 
      templateType_(templateType), 
      name_(name) {}

std::string TemplateDeclarationNode::GetTemplateTypeName() const {
    return ::CHTL::AST::GetTemplateTypeName(templateType_);
}

void TemplateDeclarationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplateDeclarationNode(this);
}

std::string TemplateDeclarationNode::ToString() const {
    std::stringstream ss;
    ss << "TemplateDeclaration(" << GetTemplateTypeName() << " " << name_ << ")";
    return ss.str();
}

// CustomDeclarationNode实现
CustomDeclarationNode::CustomDeclarationNode(TemplateType customType,
                                           const std::string& name,
                                           const Token& token)
    : ASTNode(NodeType::CustomDeclaration, token),
      customType_(customType),
      name_(name) {}

std::string CustomDeclarationNode::GetCustomTypeName() const {
    return ::CHTL::AST::GetTemplateTypeName(customType_);
}

void CustomDeclarationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustomDeclarationNode(this);
}

std::string CustomDeclarationNode::ToString() const {
    std::stringstream ss;
    ss << "CustomDeclaration(" << GetCustomTypeName() << " " << name_ << ")";
    return ss.str();
}

// TemplateUseNode实现
TemplateUseNode::TemplateUseNode(TemplateType type,
                               const std::string& name,
                               bool isCustom,
                               const Token& token)
    : ASTNode(NodeType::Element, token),  // 模板使用在语法上是元素
      type_(type),
      name_(name),
      isCustom_(isCustom) {}

void TemplateUseNode::AddSpecialization(const std::string& key, const std::string& value) {
    specializations_[key] = value;
}

void TemplateUseNode::Accept(ASTVisitor* visitor) {
    // 模板使用节点的访问逻辑
    visitor->VisitElementNode(reinterpret_cast<ElementNode*>(this));
}

std::string TemplateUseNode::ToString() const {
    std::stringstream ss;
    ss << (isCustom_ ? "CustomUse(" : "TemplateUse(");
    ss << ::CHTL::AST::GetTemplateTypeName(type_) << " " << name_;
    if (!specializations_.empty()) {
        ss << ", specs=" << specializations_.size();
    }
    ss << ")";
    return ss.str();
}

// InheritStatementNode实现
InheritStatementNode::InheritStatementNode(const std::string& target, const Token& token)
    : ASTNode(NodeType::InheritStatement, token), target_(target) {}

void InheritStatementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInheritStatementNode(this);
}

std::string InheritStatementNode::ToString() const {
    return "InheritStatement(" + target_ + ")";
}

// SpecializationNode实现
SpecializationNode::SpecializationNode(const Token& token)
    : ASTNode(NodeType::Specialization, token) {}

void SpecializationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitSpecializationNode(this);
}

std::string SpecializationNode::ToString() const {
    return "Specialization";
}

// DeleteBlockNode实现
DeleteBlockNode::DeleteBlockNode(const Token& token)
    : ASTNode(NodeType::DeleteBlock, token) {}

void DeleteBlockNode::AddDeleteTarget(const std::string& target) {
    deleteTargets_.push_back(target);
}

void DeleteBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDeleteBlockNode(this);
}

std::string DeleteBlockNode::ToString() const {
    std::stringstream ss;
    ss << "DeleteBlock(";
    if (!deleteTargets_.empty()) {
        ss << "targets=" << deleteTargets_.size();
    }
    ss << ")";
    return ss.str();
}

// InsertBlockNode实现
InsertBlockNode::InsertBlockNode(InsertPosition position,
                               const std::string& target,
                               const Token& token)
    : ASTNode(NodeType::InsertBlock, token),
      position_(position),
      target_(target) {}

std::string InsertBlockNode::GetPositionName() const {
    return GetInsertPositionName(position_);
}

void InsertBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitInsertBlockNode(this);
}

std::string InsertBlockNode::ToString() const {
    std::stringstream ss;
    ss << "InsertBlock(" << GetPositionName();
    if (!target_.empty()) {
        ss << " " << target_;
    }
    ss << ")";
    return ss.str();
}

} // namespace AST
} // namespace CHTL