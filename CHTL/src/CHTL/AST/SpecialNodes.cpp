#include "CHTL/AST/SpecialNodes.h"
#include <sstream>

namespace CHTL {
namespace AST {

// 原始类型名称转换
std::string GetOriginTypeName(OriginBlockNode::OriginType type) {
    switch (type) {
        case OriginBlockNode::OriginType::Html: return "@Html";
        case OriginBlockNode::OriginType::Style: return "@Style";
        case OriginBlockNode::OriginType::JavaScript: return "@JavaScript";
        case OriginBlockNode::OriginType::Custom: return "Custom";
        default: return "Unknown";
    }
}

// 导入类型名称转换
std::string GetImportTypeName(ImportStatementNode::ImportType type) {
    switch (type) {
        case ImportStatementNode::ImportType::Html: return "@Html";
        case ImportStatementNode::ImportType::Style: return "@Style";
        case ImportStatementNode::ImportType::JavaScript: return "@JavaScript";
        case ImportStatementNode::ImportType::Chtl: return "@Chtl";
        case ImportStatementNode::ImportType::CJmod: return "@CJmod";
        case ImportStatementNode::ImportType::Config: return "@Config";
        case ImportStatementNode::ImportType::Template: return "[Template]";
        case ImportStatementNode::ImportType::Custom: return "[Custom]";
        case ImportStatementNode::ImportType::Origin: return "[Origin]";
        case ImportStatementNode::ImportType::TemplateStyle: return "[Template] @Style";
        case ImportStatementNode::ImportType::TemplateElement: return "[Template] @Element";
        case ImportStatementNode::ImportType::TemplateVar: return "[Template] @Var";
        case ImportStatementNode::ImportType::CustomStyle: return "[Custom] @Style";
        case ImportStatementNode::ImportType::CustomElement: return "[Custom] @Element";
        case ImportStatementNode::ImportType::CustomVar: return "[Custom] @Var";
        default: return "Unknown";
    }
}

// 约束类型名称转换
std::string GetExceptTypeName(ExceptBlockNode::ExceptType type) {
    switch (type) {
        case ExceptBlockNode::ExceptType::Precise: return "Precise";
        case ExceptBlockNode::ExceptType::Type: return "Type";
        case ExceptBlockNode::ExceptType::Global: return "Global";
        default: return "Unknown";
    }
}

// OriginBlockNode实现
OriginBlockNode::OriginBlockNode(OriginType type, const std::string& name, const Token& token)
    : ASTNode(NodeType::OriginBlock, token), originType_(type), name_(name) {}

OriginBlockNode::OriginBlockNode(const std::string& customType, const std::string& name, const Token& token)
    : ASTNode(NodeType::OriginBlock, token), 
      originType_(OriginType::Custom), 
      customTypeName_(customType), 
      name_(name) {}

std::string OriginBlockNode::GetOriginTypeName() const {
    if (originType_ == OriginType::Custom) {
        return customTypeName_;
    }
    return ::CHTL::AST::GetOriginTypeName(originType_);
}

void OriginBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitOriginBlockNode(this);
}

std::string OriginBlockNode::ToString() const {
    std::stringstream ss;
    ss << "OriginBlock(" << GetOriginTypeName() << " " << name_;
    if (!content_.empty()) {
        ss << ", content=" << content_.length() << " chars";
    }
    ss << ")";
    return ss.str();
}

// ImportStatementNode实现
ImportStatementNode::ImportStatementNode(ImportType type, const Token& token)
    : ASTNode(NodeType::ImportStatement, token), importType_(type) {}

std::string ImportStatementNode::GetImportTypeName() const {
    return ::CHTL::AST::GetImportTypeName(importType_);
}

void ImportStatementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitImportStatementNode(this);
}

std::string ImportStatementNode::ToString() const {
    std::stringstream ss;
    ss << "ImportStatement(" << GetImportTypeName();
    if (!target_.empty()) {
        ss << " " << target_;
    }
    ss << " from \"" << fromPath_ << "\"";
    if (!asName_.empty()) {
        ss << " as " << asName_;
    }
    ss << ")";
    return ss.str();
}

// NamespaceDeclarationNode实现
NamespaceDeclarationNode::NamespaceDeclarationNode(const std::string& name, const Token& token)
    : ASTNode(NodeType::NamespaceDeclaration, token), name_(name) {}

void NamespaceDeclarationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitNamespaceDeclarationNode(this);
}

std::string NamespaceDeclarationNode::ToString() const {
    return "NamespaceDeclaration(" + name_ + ")";
}

// ConfigurationBlockNode实现
ConfigurationBlockNode::ConfigurationBlockNode(const std::string& name, const Token& token)
    : ASTNode(NodeType::ConfigurationBlock, token), name_(name) {}

void ConfigurationBlockNode::AddConfig(const std::string& key, const std::string& value) {
    configs_[key] = value;
}

std::string ConfigurationBlockNode::GetConfig(const std::string& key) const {
    auto it = configs_.find(key);
    return it != configs_.end() ? it->second : "";
}

bool ConfigurationBlockNode::HasConfig(const std::string& key) const {
    return configs_.find(key) != configs_.end();
}

void ConfigurationBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitConfigurationBlockNode(this);
}

std::string ConfigurationBlockNode::ToString() const {
    std::stringstream ss;
    ss << "ConfigurationBlock(";
    if (!name_.empty()) {
        ss << name_;
    }
    if (!configs_.empty()) {
        ss << ", configs=" << configs_.size();
    }
    ss << ")";
    return ss.str();
}

// ExceptBlockNode实现
ExceptBlockNode::ExceptBlockNode(const Token& token)
    : ASTNode(NodeType::ExceptBlock, token) {}

void ExceptBlockNode::AddExceptTarget(const std::string& target) {
    exceptTargets_.push_back(target);
}

void ExceptBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitExceptBlockNode(this);
}

std::string ExceptBlockNode::ToString() const {
    std::stringstream ss;
    ss << "ExceptBlock(" << ::CHTL::AST::GetExceptTypeName(exceptType_);
    if (!exceptTargets_.empty()) {
        ss << ", targets=" << exceptTargets_.size();
    }
    ss << ")";
    return ss.str();
}

// FromStatementNode实现
FromStatementNode::FromStatementNode(const std::string& symbol, 
                                   const std::string& namespaceName, 
                                   const Token& token)
    : ASTNode(NodeType::ImportStatement, token),  // from语句是导入的一种形式
      symbol_(symbol), 
      namespaceName_(namespaceName) {}

void FromStatementNode::Accept(ASTVisitor* visitor) {
    // from语句可以作为导入语句处理
    visitor->VisitImportStatementNode(reinterpret_cast<ImportStatementNode*>(this));
}

std::string FromStatementNode::ToString() const {
    return "FromStatement(" + symbol_ + " from " + namespaceName_ + ")";
}

} // namespace AST
} // namespace CHTL