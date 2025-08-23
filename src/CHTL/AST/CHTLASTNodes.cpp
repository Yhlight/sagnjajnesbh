#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/AST/CHTLASTVisitor.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace AST {

// ASTNode 基类实现
ASTNode::ASTNode(NodeType type, const Core::CHTLToken& token) 
    : type_(type), token_(token), parent_(nullptr) {}

void ASTNode::AddChild(ASTNodePtr child) {
    if (child) {
        child->SetParent(this);
        children_.push_back(child);
    }
}

ASTNodePtr ASTNode::GetChild(size_t index) const {
    if (index >= children_.size()) {
        return nullptr;
    }
    return children_[index];
}

bool ASTNode::RemoveChild(ASTNodePtr child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->SetParent(nullptr);
        children_.erase(it);
        return true;
    }
    return false;
}

bool ASTNode::RemoveChild(size_t index) {
    if (index >= children_.size()) {
        return false;
    }
    children_[index]->SetParent(nullptr);
    children_.erase(children_.begin() + index);
    return true;
}

std::vector<ASTNodePtr> ASTNode::FindChildrenByType(NodeType type) const {
    std::vector<ASTNodePtr> result;
    for (const auto& child : children_) {
        if (child->GetType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

ASTNodePtr ASTNode::FindFirstChildByType(NodeType type) const {
    for (const auto& child : children_) {
        if (child->GetType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::string ASTNode::ToString() const {
    return GetNodeTypeName(type_) + "(" + token_.GetValue() + ")";
}

std::string ASTNode::GetSourceLocation() const {
    std::ostringstream oss;
    oss << token_.GetLine() << ":" << token_.GetColumn();
    if (!token_.GetFileName().empty()) {
        oss << " in " << token_.GetFileName();
    }
    return oss.str();
}

std::string ASTNode::GetNodeTypeName(NodeType type) {
    switch (type) {
        case NodeType::ROOT: return "ROOT";
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case NodeType::CSS_SELECTOR: return "CSS_SELECTOR";
        case NodeType::CSS_PROPERTY: return "CSS_PROPERTY";
        case NodeType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case NodeType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case NodeType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case NodeType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case NodeType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case NodeType::CUSTOM_VAR: return "CUSTOM_VAR";
        case NodeType::ORIGIN_HTML: return "ORIGIN_HTML";
        case NodeType::ORIGIN_STYLE: return "ORIGIN_STYLE";
        case NodeType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case NodeType::ORIGIN_CUSTOM: return "ORIGIN_CUSTOM";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::INHERITANCE: return "INHERITANCE";
        case NodeType::DELETION: return "DELETION";
        case NodeType::INSERTION: return "INSERTION";
        case NodeType::INDEX_ACCESS: return "INDEX_ACCESS";
        case NodeType::CONSTRAINT: return "CONSTRAINT";
        case NodeType::VARIABLE_GROUP: return "VARIABLE_GROUP";
        case NodeType::VARIABLE_REFERENCE: return "VARIABLE_REFERENCE";
        case NodeType::TEMPLATE_REFERENCE: return "TEMPLATE_REFERENCE";
        case NodeType::CUSTOM_REFERENCE: return "CUSTOM_REFERENCE";
        case NodeType::SPECIALIZATION: return "SPECIALIZATION";
        case NodeType::PROPERTY_DELETION: return "PROPERTY_DELETION";
        case NodeType::INHERITANCE_DELETION: return "INHERITANCE_DELETION";
        case NodeType::STRING_LITERAL: return "STRING_LITERAL";
        case NodeType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case NodeType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case NodeType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN_NODE_TYPE";
    }
}

// RootNode 实现
RootNode::RootNode() : ASTNode(NodeType::ROOT) {}

void RootNode::Accept(ASTVisitor& visitor) {
    visitor.VisitRootNode(*this);
}

ASTNodePtr RootNode::Clone() const {
    auto clone = std::make_shared<RootNode>();
    clone->SetFileName(fileName_);
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string RootNode::ToString() const {
    return "ROOT(" + fileName_ + ")";
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName, const Core::CHTLToken& token)
    : ASTNode(NodeType::ELEMENT, token), tagName_(tagName) {}

void ElementNode::Accept(ASTVisitor& visitor) {
    visitor.VisitElementNode(*this);
}

ASTNodePtr ElementNode::Clone() const {
    auto clone = std::make_shared<ElementNode>(tagName_, token_);
    clone->attributes_ = attributes_;
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string ElementNode::ToString() const {
    std::ostringstream oss;
    oss << "ELEMENT(" << tagName_;
    if (!attributes_.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=" << attr.second;
            first = false;
        }
        oss << "]";
    }
    oss << ")";
    return oss.str();
}

void ElementNode::AddAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string ElementNode::GetAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool ElementNode::HasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void ElementNode::AddClass(const std::string& className) {
    auto it = attributes_.find("class");
    if (it != attributes_.end()) {
        it->second += " " + className;
    } else {
        attributes_["class"] = className;
    }
}

bool ElementNode::IsSelfClosing() const {
    // HTML自闭合标签列表
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input", 
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return selfClosingTags.find(Utils::StringUtils::ToLower(tagName_)) != selfClosingTags.end();
}

// TextNode 实现
TextNode::TextNode(const std::string& content, const Core::CHTLToken& token)
    : ASTNode(NodeType::TEXT, token), content_(content), isLiteral_(false) {}

void TextNode::Accept(ASTVisitor& visitor) {
    visitor.VisitTextNode(*this);
}

ASTNodePtr TextNode::Clone() const {
    auto clone = std::make_shared<TextNode>(content_, token_);
    clone->SetIsLiteral(isLiteral_);
    return clone;
}

std::string TextNode::ToString() const {
    return "TEXT(\"" + content_ + "\")";
}

// AttributeNode 实现
AttributeNode::AttributeNode(const std::string& name, const std::string& value, 
                           const Core::CHTLToken& token)
    : ASTNode(NodeType::ATTRIBUTE, token), name_(name), value_(value), usesCEEquality_(false) {}

void AttributeNode::Accept(ASTVisitor& visitor) {
    visitor.VisitAttributeNode(*this);
}

ASTNodePtr AttributeNode::Clone() const {
    auto clone = std::make_shared<AttributeNode>(name_, value_, token_);
    clone->SetUsesCEEquality(usesCEEquality_);
    return clone;
}

std::string AttributeNode::ToString() const {
    std::string separator = usesCEEquality_ ? "=" : ":";
    return "ATTRIBUTE(" + name_ + separator + value_ + ")";
}

// StyleBlockNode 实现
StyleBlockNode::StyleBlockNode(const Core::CHTLToken& token)
    : ASTNode(NodeType::STYLE_BLOCK, token), isLocalStyleBlock_(true) {}

void StyleBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitStyleBlockNode(*this);
}

ASTNodePtr StyleBlockNode::Clone() const {
    auto clone = std::make_shared<StyleBlockNode>(token_);
    clone->SetIsLocalStyleBlock(isLocalStyleBlock_);
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string StyleBlockNode::ToString() const {
    return std::string("STYLE_BLOCK(") + (isLocalStyleBlock_ ? "local" : "global") + ")";
}

void StyleBlockNode::AddSelector(ASTNodePtr selector) {
    if (selector && selector->GetType() == NodeType::CSS_SELECTOR) {
        AddChild(selector);
    }
}

void StyleBlockNode::AddProperty(ASTNodePtr property) {
    if (property && property->GetType() == NodeType::CSS_PROPERTY) {
        AddChild(property);
    }
}

void StyleBlockNode::AddTemplateReference(ASTNodePtr templateRef) {
    if (templateRef && templateRef->GetType() == NodeType::TEMPLATE_REFERENCE) {
        AddChild(templateRef);
    }
}

// ScriptBlockNode 实现
ScriptBlockNode::ScriptBlockNode(const Core::CHTLToken& token)
    : ASTNode(NodeType::SCRIPT_BLOCK, token), isLocalScriptBlock_(true) {}

void ScriptBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitScriptBlockNode(*this);
}

ASTNodePtr ScriptBlockNode::Clone() const {
    auto clone = std::make_shared<ScriptBlockNode>(token_);
    clone->SetIsLocalScriptBlock(isLocalScriptBlock_);
    clone->SetScriptContent(scriptContent_);
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string ScriptBlockNode::ToString() const {
    return std::string("SCRIPT_BLOCK(") + (isLocalScriptBlock_ ? "local" : "global") + ")";
}

// CSSSelectorNode 实现
CSSSelectorNode::CSSSelectorNode(SelectorType selectorType, const std::string& selector, 
                               const Core::CHTLToken& token)
    : ASTNode(NodeType::CSS_SELECTOR, token), selectorType_(selectorType), 
      selector_(selector), addToGlobalStyle_(false) {
    // 根据语法文档，类选择器、id选择器、伪类选择器、伪元素选择器会自动添加到全局样式块
    if (selectorType_ == SelectorType::CLASS || selectorType_ == SelectorType::ID ||
        selectorType_ == SelectorType::PSEUDO_CLASS || selectorType_ == SelectorType::PSEUDO_ELEMENT) {
        addToGlobalStyle_ = true;
    }
}

void CSSSelectorNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCSSSelectorNode(*this);
}

ASTNodePtr CSSSelectorNode::Clone() const {
    auto clone = std::make_shared<CSSSelectorNode>(selectorType_, selector_, token_);
    clone->SetAddToGlobalStyle(addToGlobalStyle_);
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string CSSSelectorNode::ToString() const {
    std::string typeStr;
    switch (selectorType_) {
        case SelectorType::CLASS: typeStr = "class"; break;
        case SelectorType::ID: typeStr = "id"; break;
        case SelectorType::PSEUDO_CLASS: typeStr = "pseudo-class"; break;
        case SelectorType::PSEUDO_ELEMENT: typeStr = "pseudo-element"; break;
        case SelectorType::AMPERSAND: typeStr = "ampersand"; break;
        case SelectorType::ELEMENT: typeStr = "element"; break;
    }
    return "CSS_SELECTOR(" + typeStr + ": " + selector_ + ")";
}

// CSSPropertyNode 实现
CSSPropertyNode::CSSPropertyNode(const std::string& property, const std::string& value,
                               const Core::CHTLToken& token)
    : ASTNode(NodeType::CSS_PROPERTY, token), property_(property), value_(value), 
      usesCEEquality_(false) {}

void CSSPropertyNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCSSPropertyNode(*this);
}

ASTNodePtr CSSPropertyNode::Clone() const {
    auto clone = std::make_shared<CSSPropertyNode>(property_, value_, token_);
    clone->SetUsesCEEquality(usesCEEquality_);
    return clone;
}

std::string CSSPropertyNode::ToString() const {
    std::string separator = usesCEEquality_ ? "=" : ":";
    return "CSS_PROPERTY(" + property_ + separator + value_ + ")";
}

// TemplateNode 实现
TemplateNode::TemplateNode(TemplateType templateType, const std::string& name,
                         const Core::CHTLToken& token)
    : ASTNode(NodeType::TEMPLATE_STYLE, token), templateType_(templateType), name_(name) {
    // 根据模板类型设置正确的节点类型
    switch (templateType) {
        case TemplateType::STYLE:
            type_ = NodeType::TEMPLATE_STYLE;
            break;
        case TemplateType::ELEMENT:
            type_ = NodeType::TEMPLATE_ELEMENT;
            break;
        case TemplateType::VAR:
            type_ = NodeType::TEMPLATE_VAR;
            break;
    }
}

void TemplateNode::Accept(ASTVisitor& visitor) {
    visitor.VisitTemplateNode(*this);
}

ASTNodePtr TemplateNode::Clone() const {
    auto clone = std::make_shared<TemplateNode>(templateType_, name_, token_);
    for (const auto& inheritance : inheritances_) {
        clone->AddInheritance(inheritance->Clone());
    }
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string TemplateNode::ToString() const {
    std::string typeStr;
    switch (templateType_) {
        case TemplateType::STYLE: typeStr = "Style"; break;
        case TemplateType::ELEMENT: typeStr = "Element"; break;
        case TemplateType::VAR: typeStr = "Var"; break;
    }
    return "TEMPLATE_" + typeStr + "(" + name_ + ")";
}

void TemplateNode::AddInheritance(ASTNodePtr inheritance) {
    if (inheritance && inheritance->GetType() == NodeType::INHERITANCE) {
        inheritances_.push_back(inheritance);
    }
}

// CustomNode 实现
CustomNode::CustomNode(CustomType customType, const std::string& name,
                      const Core::CHTLToken& token)
    : ASTNode(NodeType::CUSTOM_STYLE, token), customType_(customType), name_(name) {
    // 根据自定义类型设置正确的节点类型
    switch (customType) {
        case CustomType::STYLE:
            type_ = NodeType::CUSTOM_STYLE;
            break;
        case CustomType::ELEMENT:
            type_ = NodeType::CUSTOM_ELEMENT;
            break;
        case CustomType::VAR:
            type_ = NodeType::CUSTOM_VAR;
            break;
    }
}

void CustomNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCustomNode(*this);
}

ASTNodePtr CustomNode::Clone() const {
    auto clone = std::make_shared<CustomNode>(customType_, name_, token_);
    for (const auto& inheritance : inheritances_) {
        clone->AddInheritance(inheritance->Clone());
    }
    for (const auto& specialization : specializations_) {
        clone->AddSpecialization(specialization->Clone());
    }
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string CustomNode::ToString() const {
    std::string typeStr;
    switch (customType_) {
        case CustomType::STYLE: typeStr = "Style"; break;
        case CustomType::ELEMENT: typeStr = "Element"; break;
        case CustomType::VAR: typeStr = "Var"; break;
    }
    return "CUSTOM_" + typeStr + "(" + name_ + ")";
}

void CustomNode::AddInheritance(ASTNodePtr inheritance) {
    if (inheritance && inheritance->GetType() == NodeType::INHERITANCE) {
        inheritances_.push_back(inheritance);
    }
}

void CustomNode::AddSpecialization(ASTNodePtr specialization) {
    if (specialization && specialization->GetType() == NodeType::SPECIALIZATION) {
        specializations_.push_back(specialization);
    }
}

// OriginNode 实现
OriginNode::OriginNode(OriginType originType, const std::string& content,
                      const std::string& name, const Core::CHTLToken& token)
    : ASTNode(NodeType::ORIGIN_HTML, token), originType_(originType), 
      content_(content), name_(name) {
    // 根据原始嵌入类型设置正确的节点类型
    switch (originType) {
        case OriginType::HTML:
            type_ = NodeType::ORIGIN_HTML;
            break;
        case OriginType::STYLE:
            type_ = NodeType::ORIGIN_STYLE;
            break;
        case OriginType::JAVASCRIPT:
            type_ = NodeType::ORIGIN_JAVASCRIPT;
            break;
        case OriginType::CUSTOM:
            type_ = NodeType::ORIGIN_CUSTOM;
            break;
    }
}

void OriginNode::Accept(ASTVisitor& visitor) {
    visitor.VisitOriginNode(*this);
}

ASTNodePtr OriginNode::Clone() const {
    auto clone = std::make_shared<OriginNode>(originType_, content_, name_, token_);
    clone->SetCustomTypeName(customTypeName_);
    return clone;
}

std::string OriginNode::ToString() const {
    std::string typeStr;
    switch (originType_) {
        case OriginType::HTML: typeStr = "HTML"; break;
        case OriginType::STYLE: typeStr = "STYLE"; break;
        case OriginType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
        case OriginType::CUSTOM: typeStr = "CUSTOM(" + customTypeName_ + ")"; break;
    }
    
    std::string result = "ORIGIN_" + typeStr;
    if (!name_.empty()) {
        result += "(" + name_ + ")";
    }
    return result;
}

// ImportNode 实现
ImportNode::ImportNode(ImportType importType, const std::string& path,
                      const std::string& name, const std::string& alias,
                      const Core::CHTLToken& token)
    : ASTNode(NodeType::IMPORT, token), importType_(importType), 
      path_(path), name_(name), alias_(alias) {}

void ImportNode::Accept(ASTVisitor& visitor) {
    visitor.VisitImportNode(*this);
}

ASTNodePtr ImportNode::Clone() const {
    return std::make_shared<ImportNode>(importType_, path_, name_, alias_, token_);
}

std::string ImportNode::ToString() const {
    std::string typeStr;
    switch (importType_) {
        case ImportType::HTML: typeStr = "HTML"; break;
        case ImportType::STYLE: typeStr = "STYLE"; break;
        case ImportType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
        case ImportType::CHTL: typeStr = "CHTL"; break;
        case ImportType::CJMOD: typeStr = "CJMOD"; break;
        case ImportType::CONFIG: typeStr = "CONFIG"; break;
        case ImportType::TEMPLATE_STYLE: typeStr = "TEMPLATE_STYLE"; break;
        case ImportType::TEMPLATE_ELEMENT: typeStr = "TEMPLATE_ELEMENT"; break;
        case ImportType::TEMPLATE_VAR: typeStr = "TEMPLATE_VAR"; break;
        case ImportType::CUSTOM_STYLE: typeStr = "CUSTOM_STYLE"; break;
        case ImportType::CUSTOM_ELEMENT: typeStr = "CUSTOM_ELEMENT"; break;
        case ImportType::CUSTOM_VAR: typeStr = "CUSTOM_VAR"; break;
        case ImportType::ORIGIN_HTML: typeStr = "ORIGIN_HTML"; break;
        case ImportType::ORIGIN_STYLE: typeStr = "ORIGIN_STYLE"; break;
        case ImportType::ORIGIN_JAVASCRIPT: typeStr = "ORIGIN_JAVASCRIPT"; break;
        case ImportType::ALL_TEMPLATES: typeStr = "ALL_TEMPLATES"; break;
        case ImportType::ALL_CUSTOMS: typeStr = "ALL_CUSTOMS"; break;
        case ImportType::ALL_ORIGINS: typeStr = "ALL_ORIGINS"; break;
    }
    
    std::string result = "IMPORT(" + typeStr + " from " + path_;
    if (!name_.empty()) {
        result += " name=" + name_;
    }
    if (!alias_.empty()) {
        result += " as=" + alias_;
    }
    result += ")";
    return result;
}

bool ImportNode::IsBatchImport() const {
    return importType_ == ImportType::ALL_TEMPLATES ||
           importType_ == ImportType::ALL_CUSTOMS ||
           importType_ == ImportType::ALL_ORIGINS;
}

// NamespaceNode 实现
NamespaceNode::NamespaceNode(const std::string& name, const Core::CHTLToken& token)
    : ASTNode(NodeType::NAMESPACE, token), name_(name) {}

void NamespaceNode::Accept(ASTVisitor& visitor) {
    visitor.VisitNamespaceNode(*this);
}

ASTNodePtr NamespaceNode::Clone() const {
    auto clone = std::make_shared<NamespaceNode>(name_, token_);
    for (const auto& constraint : constraints_) {
        clone->AddConstraint(constraint->Clone());
    }
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string NamespaceNode::ToString() const {
    return "NAMESPACE(" + name_ + ")";
}

bool NamespaceNode::IsNested() const {
    return parent_ && parent_->GetType() == NodeType::NAMESPACE;
}

void NamespaceNode::AddConstraint(ASTNodePtr constraint) {
    if (constraint && constraint->GetType() == NodeType::CONSTRAINT) {
        constraints_.push_back(constraint);
    }
}

// ConfigurationNode 实现
ConfigurationNode::ConfigurationNode(const std::string& name, const Core::CHTLToken& token)
    : ASTNode(NodeType::CONFIGURATION, token), name_(name) {}

void ConfigurationNode::Accept(ASTVisitor& visitor) {
    visitor.VisitConfigurationNode(*this);
}

ASTNodePtr ConfigurationNode::Clone() const {
    auto clone = std::make_shared<ConfigurationNode>(name_, token_);
    clone->settings_ = settings_;
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string ConfigurationNode::ToString() const {
    return "CONFIGURATION(" + (name_.empty() ? "unnamed" : name_) + ")";
}

void ConfigurationNode::AddSetting(const std::string& key, const std::string& value) {
    settings_[key] = value;
}

std::string ConfigurationNode::GetSetting(const std::string& key) const {
    auto it = settings_.find(key);
    return (it != settings_.end()) ? it->second : "";
}

// InheritanceNode 实现
InheritanceNode::InheritanceNode(const std::string& targetType, const std::string& targetName,
                               bool isExplicit, const Core::CHTLToken& token)
    : ASTNode(NodeType::INHERITANCE, token), targetType_(targetType), 
      targetName_(targetName), isExplicit_(isExplicit) {}

void InheritanceNode::Accept(ASTVisitor& visitor) {
    visitor.VisitInheritanceNode(*this);
}

ASTNodePtr InheritanceNode::Clone() const {
    auto clone = std::make_shared<InheritanceNode>(targetType_, targetName_, isExplicit_, token_);
    clone->SetNamespace(namespaceName_);
    return clone;
}

std::string InheritanceNode::ToString() const {
    std::string result = "INHERITANCE(";
    if (isExplicit_) {
        result += "explicit ";
    }
    result += targetType_ + " " + targetName_;
    if (!namespaceName_.empty()) {
        result += " from " + namespaceName_;
    }
    result += ")";
    return result;
}

// DeletionNode 实现
DeletionNode::DeletionNode(DeletionType deletionType, const std::vector<std::string>& targets,
                         const Core::CHTLToken& token)
    : ASTNode(NodeType::DELETION, token), deletionType_(deletionType), targets_(targets) {}

void DeletionNode::Accept(ASTVisitor& visitor) {
    visitor.VisitDeletionNode(*this);
}

ASTNodePtr DeletionNode::Clone() const {
    return std::make_shared<DeletionNode>(deletionType_, targets_, token_);
}

std::string DeletionNode::ToString() const {
    std::string typeStr;
    switch (deletionType_) {
        case DeletionType::PROPERTY: typeStr = "PROPERTY"; break;
        case DeletionType::INHERITANCE: typeStr = "INHERITANCE"; break;
        case DeletionType::ELEMENT: typeStr = "ELEMENT"; break;
        case DeletionType::ELEMENT_INHERITANCE: typeStr = "ELEMENT_INHERITANCE"; break;
    }
    
    std::string targetsStr = Utils::StringUtils::Join(targets_, ", ");
    return "DELETION(" + typeStr + ": " + targetsStr + ")";
}

void DeletionNode::AddTarget(const std::string& target) {
    targets_.push_back(target);
}

// InsertionNode 实现
InsertionNode::InsertionNode(InsertionPosition position, const std::string& target,
                           const Core::CHTLToken& token)
    : ASTNode(NodeType::INSERTION, token), position_(position), target_(target) {}

void InsertionNode::Accept(ASTVisitor& visitor) {
    visitor.VisitInsertionNode(*this);
}

ASTNodePtr InsertionNode::Clone() const {
    auto clone = std::make_shared<InsertionNode>(position_, target_, token_);
    for (const auto& content : insertContents_) {
        clone->AddInsertContent(content->Clone());
    }
    return clone;
}

std::string InsertionNode::ToString() const {
    std::string posStr;
    switch (position_) {
        case InsertionPosition::AFTER: posStr = "after"; break;
        case InsertionPosition::BEFORE: posStr = "before"; break;
        case InsertionPosition::REPLACE: posStr = "replace"; break;
        case InsertionPosition::AT_TOP: posStr = "at top"; break;
        case InsertionPosition::AT_BOTTOM: posStr = "at bottom"; break;
    }
    return "INSERTION(" + posStr + " " + target_ + ")";
}

void InsertionNode::AddInsertContent(ASTNodePtr content) {
    if (content) {
        insertContents_.push_back(content);
    }
}

// IndexAccessNode 实现
IndexAccessNode::IndexAccessNode(const std::string& elementName, int index,
                                const Core::CHTLToken& token)
    : ASTNode(NodeType::INDEX_ACCESS, token), elementName_(elementName), index_(index) {}

void IndexAccessNode::Accept(ASTVisitor& visitor) {
    visitor.VisitIndexAccessNode(*this);
}

ASTNodePtr IndexAccessNode::Clone() const {
    return std::make_shared<IndexAccessNode>(elementName_, index_, token_);
}

std::string IndexAccessNode::ToString() const {
    return "INDEX_ACCESS(" + elementName_ + "[" + std::to_string(index_) + "])";
}

// ConstraintNode 实现
ConstraintNode::ConstraintNode(ConstraintType constraintType, const std::vector<std::string>& targets,
                             const Core::CHTLToken& token)
    : ASTNode(NodeType::CONSTRAINT, token), constraintType_(constraintType), targets_(targets) {}

void ConstraintNode::Accept(ASTVisitor& visitor) {
    visitor.VisitConstraintNode(*this);
}

ASTNodePtr ConstraintNode::Clone() const {
    return std::make_shared<ConstraintNode>(constraintType_, targets_, token_);
}

std::string ConstraintNode::ToString() const {
    std::string typeStr;
    switch (constraintType_) {
        case ConstraintType::PRECISE: typeStr = "PRECISE"; break;
        case ConstraintType::TYPE: typeStr = "TYPE"; break;
        case ConstraintType::GLOBAL: typeStr = "GLOBAL"; break;
    }
    
    std::string targetsStr = Utils::StringUtils::Join(targets_, ", ");
    return "CONSTRAINT(" + typeStr + ": " + targetsStr + ")";
}

void ConstraintNode::AddTarget(const std::string& target) {
    targets_.push_back(target);
}

// VariableGroupNode 实现
VariableGroupNode::VariableGroupNode(const std::string& name, const Core::CHTLToken& token)
    : ASTNode(NodeType::VARIABLE_GROUP, token), name_(name), isValuelessStyleGroup_(false) {}

void VariableGroupNode::Accept(ASTVisitor& visitor) {
    visitor.VisitVariableGroupNode(*this);
}

ASTNodePtr VariableGroupNode::Clone() const {
    auto clone = std::make_shared<VariableGroupNode>(name_, token_);
    clone->variables_ = variables_;
    clone->SetIsValuelessStyleGroup(isValuelessStyleGroup_);
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string VariableGroupNode::ToString() const {
    std::string result = "VARIABLE_GROUP(" + name_;
    if (isValuelessStyleGroup_) {
        result += " valueless";
    }
    result += ")";
    return result;
}

void VariableGroupNode::AddVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string VariableGroupNode::GetVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

// VariableReferenceNode 实现
VariableReferenceNode::VariableReferenceNode(const std::string& groupName, const std::string& variableName,
                                           const Core::CHTLToken& token)
    : ASTNode(NodeType::VARIABLE_REFERENCE, token), groupName_(groupName), variableName_(variableName) {}

void VariableReferenceNode::Accept(ASTVisitor& visitor) {
    visitor.VisitVariableReferenceNode(*this);
}

ASTNodePtr VariableReferenceNode::Clone() const {
    auto clone = std::make_shared<VariableReferenceNode>(groupName_, variableName_, token_);
    clone->specializationParams_ = specializationParams_;
    return clone;
}

std::string VariableReferenceNode::ToString() const {
    std::string result = "VARIABLE_REFERENCE(" + groupName_ + "(" + variableName_ + ")";
    if (HasSpecialization()) {
        result += " specialized";
    }
    result += ")";
    return result;
}

void VariableReferenceNode::AddSpecializationParam(const std::string& param, const std::string& value) {
    specializationParams_[param] = value;
}

// TemplateReferenceNode 实现
TemplateReferenceNode::TemplateReferenceNode(const std::string& templateType, const std::string& templateName,
                                           const std::string& namespaceName, const Core::CHTLToken& token)
    : ASTNode(NodeType::TEMPLATE_REFERENCE, token), templateType_(templateType), 
      templateName_(templateName), namespaceName_(namespaceName), isFullyQualified_(false) {}

void TemplateReferenceNode::Accept(ASTVisitor& visitor) {
    visitor.VisitTemplateReferenceNode(*this);
}

ASTNodePtr TemplateReferenceNode::Clone() const {
    auto clone = std::make_shared<TemplateReferenceNode>(templateType_, templateName_, namespaceName_, token_);
    clone->SetIsFullyQualified(isFullyQualified_);
    return clone;
}

std::string TemplateReferenceNode::ToString() const {
    std::string result = "TEMPLATE_REFERENCE(" + templateType_ + " " + templateName_;
    if (!namespaceName_.empty()) {
        result += " from " + namespaceName_;
    }
    if (isFullyQualified_) {
        result += " fully-qualified";
    }
    result += ")";
    return result;
}

// CustomReferenceNode 实现
CustomReferenceNode::CustomReferenceNode(const std::string& customType, const std::string& customName,
                                        const std::string& namespaceName, const Core::CHTLToken& token)
    : ASTNode(NodeType::CUSTOM_REFERENCE, token), customType_(customType), 
      customName_(customName), namespaceName_(namespaceName) {}

void CustomReferenceNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCustomReferenceNode(*this);
}

ASTNodePtr CustomReferenceNode::Clone() const {
    auto clone = std::make_shared<CustomReferenceNode>(customType_, customName_, namespaceName_, token_);
    for (const auto& specialization : specializations_) {
        clone->AddSpecialization(specialization->Clone());
    }
    return clone;
}

std::string CustomReferenceNode::ToString() const {
    std::string result = "CUSTOM_REFERENCE(" + customType_ + " " + customName_;
    if (!namespaceName_.empty()) {
        result += " from " + namespaceName_;
    }
    if (HasSpecialization()) {
        result += " specialized";
    }
    result += ")";
    return result;
}

void CustomReferenceNode::AddSpecialization(ASTNodePtr specialization) {
    if (specialization && specialization->GetType() == NodeType::SPECIALIZATION) {
        specializations_.push_back(specialization);
    }
}

// SpecializationNode 实现
SpecializationNode::SpecializationNode(const Core::CHTLToken& token)
    : ASTNode(NodeType::SPECIALIZATION, token) {}

void SpecializationNode::Accept(ASTVisitor& visitor) {
    visitor.VisitSpecializationNode(*this);
}

ASTNodePtr SpecializationNode::Clone() const {
    auto clone = std::make_shared<SpecializationNode>(token_);
    for (const auto& deletion : deletions_) {
        clone->AddDeletion(deletion->Clone());
    }
    clone->propertyOverrides_ = propertyOverrides_;
    return clone;
}

std::string SpecializationNode::ToString() const {
    return "SPECIALIZATION(deletions=" + std::to_string(deletions_.size()) + 
           " overrides=" + std::to_string(propertyOverrides_.size()) + ")";
}

void SpecializationNode::AddDeletion(ASTNodePtr deletion) {
    if (deletion && deletion->GetType() == NodeType::DELETION) {
        deletions_.push_back(deletion);
    }
}

void SpecializationNode::AddPropertyOverride(const std::string& property, const std::string& value) {
    propertyOverrides_[property] = value;
}

// CommentNode 实现
CommentNode::CommentNode(CommentType commentType, const std::string& content,
                        const Core::CHTLToken& token)
    : ASTNode(NodeType::COMMENT, token), commentType_(commentType), content_(content) {}

void CommentNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCommentNode(*this);
}

ASTNodePtr CommentNode::Clone() const {
    return std::make_shared<CommentNode>(commentType_, content_, token_);
}

std::string CommentNode::ToString() const {
    std::string typeStr;
    switch (commentType_) {
        case CommentType::SINGLE_LINE: typeStr = "single-line"; break;
        case CommentType::MULTI_LINE: typeStr = "multi-line"; break;
        case CommentType::GENERATOR: typeStr = "generator"; break;
    }
    
    std::string shortContent = content_;
    if (shortContent.length() > 30) {
        shortContent = shortContent.substr(0, 27) + "...";
    }
    return "COMMENT(" + typeStr + ": \"" + shortContent + "\")";
}

// LiteralNode 实现
LiteralNode::LiteralNode(LiteralType literalType, const std::string& value,
                        const Core::CHTLToken& token)
    : ASTNode(NodeType::STRING_LITERAL, token), literalType_(literalType), value_(value) {
    // 根据字面量类型设置正确的节点类型
    switch (literalType) {
        case LiteralType::STRING:
            type_ = NodeType::STRING_LITERAL;
            break;
        case LiteralType::UNQUOTED:
            type_ = NodeType::UNQUOTED_LITERAL;
            break;
        case LiteralType::NUMBER:
            type_ = NodeType::NUMBER_LITERAL;
            break;
    }
}

void LiteralNode::Accept(ASTVisitor& visitor) {
    visitor.VisitLiteralNode(*this);
}

ASTNodePtr LiteralNode::Clone() const {
    return std::make_shared<LiteralNode>(literalType_, value_, token_);
}

std::string LiteralNode::ToString() const {
    std::string typeStr;
    switch (literalType_) {
        case LiteralType::STRING: typeStr = "STRING"; break;
        case LiteralType::UNQUOTED: typeStr = "UNQUOTED"; break;
        case LiteralType::NUMBER: typeStr = "NUMBER"; break;
    }
    return typeStr + "_LITERAL(\"" + value_ + "\")";
}

} // namespace AST
} // namespace CHTL
