#include "CHTL/AST/StyleNodes.h"
#include <sstream>

namespace CHTL {
namespace AST {

// 选择器类型名称转换
std::string GetSelectorTypeName(StyleSelectorNode::SelectorType type) {
    switch (type) {
        case StyleSelectorNode::SelectorType::Class: return "Class";
        case StyleSelectorNode::SelectorType::Id: return "Id";
        case StyleSelectorNode::SelectorType::Element: return "Element";
        case StyleSelectorNode::SelectorType::Universal: return "Universal";
        case StyleSelectorNode::SelectorType::Attribute: return "Attribute";
        case StyleSelectorNode::SelectorType::PseudoClass: return "PseudoClass";
        case StyleSelectorNode::SelectorType::PseudoElement: return "PseudoElement";
        case StyleSelectorNode::SelectorType::Combinator: return "Combinator";
        case StyleSelectorNode::SelectorType::Ampersand: return "Ampersand";
        default: return "Unknown";
    }
}

// 组合器类型名称转换
std::string GetCombinatorTypeName(StyleSelectorNode::CombinatorType type) {
    switch (type) {
        case StyleSelectorNode::CombinatorType::Descendant: return "Descendant";
        case StyleSelectorNode::CombinatorType::Child: return "Child";
        case StyleSelectorNode::CombinatorType::Adjacent: return "Adjacent";
        case StyleSelectorNode::CombinatorType::Sibling: return "Sibling";
        default: return "Unknown";
    }
}

// 样式值类型名称转换
std::string GetStyleValueTypeName(StyleValueNode::ValueType type) {
    switch (type) {
        case StyleValueNode::ValueType::Literal: return "Literal";
        case StyleValueNode::ValueType::Function: return "Function";
        case StyleValueNode::ValueType::Variable: return "Variable";
        case StyleValueNode::ValueType::Calc: return "Calc";
        default: return "Unknown";
    }
}

// StyleRuleNode实现
StyleRuleNode::StyleRuleNode(const Token& token)
    : ASTNode(NodeType::StyleRule, token) {}

void StyleRuleNode::SetSelector(std::shared_ptr<ASTNode> selector) {
    selector_ = selector;
    if (selector) {
        AddChild(selector);
    }
}

void StyleRuleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleRuleNode(this);
}

std::string StyleRuleNode::ToString() const {
    std::stringstream ss;
    ss << "StyleRule(";
    if (IsInlineStyle()) {
        ss << "inline";
    } else {
        ss << "selector";
    }
    ss << ")";
    return ss.str();
}

// StyleSelectorNode实现
StyleSelectorNode::StyleSelectorNode(SelectorType type, const std::string& value, const Token& token)
    : ASTNode(NodeType::StyleSelector, token), selectorType_(type), value_(value) {}

std::string StyleSelectorNode::GetSelectorTypeName() const {
    return ::CHTL::AST::GetSelectorTypeName(selectorType_);
}

void StyleSelectorNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleSelectorNode(this);
}

std::string StyleSelectorNode::ToString() const {
    std::stringstream ss;
    ss << "StyleSelector(" << GetSelectorTypeName();
    if (!value_.empty()) {
        ss << " \"" << value_ << "\"";
    }
    if (selectorType_ == SelectorType::Combinator) {
        ss << " " << ::CHTL::AST::GetCombinatorTypeName(combinatorType_);
    }
    ss << ")";
    return ss.str();
}

// StylePropertyNode实现
StylePropertyNode::StylePropertyNode(const std::string& name, const Token& token)
    : ASTNode(NodeType::StyleProperty, token), name_(name) {}

void StylePropertyNode::AddValue(std::shared_ptr<ASTNode> value) {
    if (value) {
        values_.push_back(value);
        AddChild(value);
    }
}

void StylePropertyNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStylePropertyNode(this);
}

std::string StylePropertyNode::ToString() const {
    std::stringstream ss;
    ss << "StyleProperty(" << name_;
    if (!values_.empty()) {
        ss << ", values=" << values_.size();
    }
    if (isImportant_) {
        ss << ", !important";
    }
    ss << ")";
    return ss.str();
}

// StyleValueNode实现
StyleValueNode::StyleValueNode(ValueType type, const std::string& value, const Token& token)
    : ASTNode(NodeType::UnquotedLiteral, token), valueType_(type), value_(value) {}

void StyleValueNode::AddArgument(std::shared_ptr<ASTNode> arg) {
    if (arg) {
        arguments_.push_back(arg);
        AddChild(arg);
    }
}

void StyleValueNode::Accept(ASTVisitor* visitor) {
    // 样式值通常作为字面量处理
    visitor->VisitStringLiteralNode(reinterpret_cast<StringLiteralNode*>(this));
}

std::string StyleValueNode::ToString() const {
    std::stringstream ss;
    ss << "StyleValue(" << ::CHTL::AST::GetStyleValueTypeName(valueType_) << " \"" << value_ << "\"";
    if (!arguments_.empty()) {
        ss << ", args=" << arguments_.size();
    }
    ss << ")";
    return ss.str();
}

// StyleVariableUseNode实现
StyleVariableUseNode::StyleVariableUseNode(const std::string& varGroupName, 
                                         const std::string& varName, 
                                         const Token& token)
    : ASTNode(NodeType::Identifier, token), 
      varGroupName_(varGroupName), 
      varName_(varName) {}

void StyleVariableUseNode::Accept(ASTVisitor* visitor) {
    visitor->VisitIdentifierNode(reinterpret_cast<IdentifierNode*>(this));
}

std::string StyleVariableUseNode::ToString() const {
    std::stringstream ss;
    ss << "StyleVariableUse(" << varGroupName_ << "." << varName_;
    if (HasSpecialization()) {
        ss << "[" << specialization_ << "]";
    }
    ss << ")";
    return ss.str();
}

} // namespace AST
} // namespace CHTL