#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace CHTL {
namespace AST {

// ASTNode实现
ASTNode::ASTNode(NodeType type, const Token& token)
    : type_(type), line_(token.line), column_(token.column) {}

std::string ASTNode::GetTypeName() const {
    return ASTNodeUtils::GetNodeTypeName(type_);
}

void ASTNode::AddChild(std::shared_ptr<ASTNode> child) {
    if (child) {
        children_.push_back(child);
        child->SetParent(this);
    }
}

void ASTNode::RemoveChild(std::shared_ptr<ASTNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->SetParent(nullptr);
        children_.erase(it);
    }
}

std::shared_ptr<ASTNode> ASTNode::GetChild(size_t index) const {
    return index < children_.size() ? children_[index] : nullptr;
}

std::string ASTNode::ToString() const {
    return GetTypeName();
}

void ASTNode::Print(int indent) const {
    std::cout << IndentString(indent) << ToString() << std::endl;
    for (const auto& child : children_) {
        if (child) {
            child->Print(indent + 2);
        }
    }
}

std::string ASTNode::IndentString(int indent) const {
    return std::string(indent, ' ');
}

// ProgramNode实现
ProgramNode::ProgramNode(const Token& token)
    : ASTNode(NodeType::Program, token) {}

void ProgramNode::Accept(ASTVisitor* visitor) {
    visitor->VisitProgramNode(this);
}

// ElementNode实现
ElementNode::ElementNode(const std::string& tagName, const Token& token)
    : ASTNode(NodeType::Element, token), tagName_(tagName) {}

void ElementNode::AddAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string ElementNode::GetAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return it != attributes_.end() ? it->second : "";
}

bool ElementNode::HasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void ElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitElementNode(this);
}

std::string ElementNode::ToString() const {
    std::stringstream ss;
    ss << "Element(" << tagName_;
    if (!attributes_.empty()) {
        ss << ", attrs=" << attributes_.size();
    }
    ss << ")";
    return ss.str();
}

// AttributeNode实现
AttributeNode::AttributeNode(const std::string& name, const std::string& value, const Token& token)
    : ASTNode(NodeType::Attribute, token), name_(name), value_(value) {}

void AttributeNode::Accept(ASTVisitor* visitor) {
    visitor->VisitAttributeNode(this);
}

std::string AttributeNode::ToString() const {
    return "Attribute(" + name_ + "=\"" + value_ + "\")";
}

// TextBlockNode实现
TextBlockNode::TextBlockNode(const Token& token)
    : ASTNode(NodeType::TextBlock, token) {}

void TextBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTextBlockNode(this);
}

std::string TextBlockNode::ToString() const {
    std::string preview = content_.substr(0, 20);
    if (content_.length() > 20) preview += "...";
    return "TextBlock(\"" + preview + "\")";
}

// StyleBlockNode实现
StyleBlockNode::StyleBlockNode(bool isLocal, const Token& token)
    : ASTNode(NodeType::StyleBlock, token), isLocal_(isLocal) {}

void StyleBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleBlockNode(this);
}

std::string StyleBlockNode::ToString() const {
    return std::string("StyleBlock(") + (isLocal_ ? "local" : "global") + ")";
}

// ScriptBlockNode实现
ScriptBlockNode::ScriptBlockNode(bool isLocal, const Token& token)
    : ASTNode(NodeType::ScriptBlock, token), isLocal_(isLocal) {}

void ScriptBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitScriptBlockNode(this);
}

std::string ScriptBlockNode::ToString() const {
    return std::string("ScriptBlock(") + (isLocal_ ? "local" : "global") + ")";
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& name, const Token& token)
    : ASTNode(NodeType::Identifier, token), name_(name) {}

void IdentifierNode::Accept(ASTVisitor* visitor) {
    visitor->VisitIdentifierNode(this);
}

std::string IdentifierNode::ToString() const {
    return "Identifier(" + name_ + ")";
}

// StringLiteralNode实现
StringLiteralNode::StringLiteralNode(const std::string& value, const Token& token)
    : ASTNode(NodeType::StringLiteral, token), value_(value) {}

void StringLiteralNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStringLiteralNode(this);
}

std::string StringLiteralNode::ToString() const {
    return "StringLiteral(\"" + value_ + "\")";
}

// ASTNodeUtils实现
std::string ASTNodeUtils::GetNodeTypeName(NodeType type) {
    switch (type) {
        case NodeType::Program: return "Program";
        case NodeType::Element: return "Element";
        case NodeType::Attribute: return "Attribute";
        case NodeType::TextBlock: return "TextBlock";
        case NodeType::StyleBlock: return "StyleBlock";
        case NodeType::StyleRule: return "StyleRule";
        case NodeType::StyleSelector: return "StyleSelector";
        case NodeType::StyleProperty: return "StyleProperty";
        case NodeType::ScriptBlock: return "ScriptBlock";
        case NodeType::TemplateDeclaration: return "TemplateDeclaration";
        case NodeType::CustomDeclaration: return "CustomDeclaration";
        case NodeType::OriginBlock: return "OriginBlock";
        case NodeType::ImportStatement: return "ImportStatement";
        case NodeType::NamespaceDeclaration: return "NamespaceDeclaration";
        case NodeType::ConfigurationBlock: return "ConfigurationBlock";
        case NodeType::Identifier: return "Identifier";
        case NodeType::StringLiteral: return "StringLiteral";
        case NodeType::NumberLiteral: return "NumberLiteral";
        case NodeType::UnquotedLiteral: return "UnquotedLiteral";
        case NodeType::Specialization: return "Specialization";
        case NodeType::InsertBlock: return "InsertBlock";
        case NodeType::DeleteBlock: return "DeleteBlock";
        case NodeType::ExceptBlock: return "ExceptBlock";
        case NodeType::InheritStatement: return "InheritStatement";
        case NodeType::Comment: return "Comment";
        case NodeType::GeneratorComment: return "GeneratorComment";
        default: return "Unknown";
    }
}

bool ASTNodeUtils::IsExpressionNode(NodeType type) {
    return type == NodeType::Identifier ||
           type == NodeType::StringLiteral ||
           type == NodeType::NumberLiteral ||
           type == NodeType::UnquotedLiteral;
}

bool ASTNodeUtils::IsStatementNode(NodeType type) {
    return type == NodeType::ImportStatement ||
           type == NodeType::InheritStatement ||
           type == NodeType::ExceptBlock;
}

bool ASTNodeUtils::IsDeclarationNode(NodeType type) {
    return type == NodeType::TemplateDeclaration ||
           type == NodeType::CustomDeclaration ||
           type == NodeType::NamespaceDeclaration ||
           type == NodeType::ConfigurationBlock;
}

bool ASTNodeUtils::CanHaveChildren(NodeType type) {
    return type != NodeType::Identifier &&
           type != NodeType::StringLiteral &&
           type != NodeType::NumberLiteral &&
           type != NodeType::UnquotedLiteral &&
           type != NodeType::Comment &&
           type != NodeType::GeneratorComment;
}

} // namespace AST
} // namespace CHTL