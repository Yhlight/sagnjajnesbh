#include "chtl/ast/DocumentNode.h"
#include "chtl/ast/CommentNode.h"
#include "chtl/ast/TextNode.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/AttributeNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/ImportNode.h"
#include "chtl/ast/NamespaceNode.h"
#include "chtl/ast/ConfigurationNode.h"
#include "chtl/ast/OriginNode.h"
#include "chtl/ast/ExceptNode.h"
#include "chtl/ast/LiteralNodes.h"
#include "chtl/ast/SpecializationNodes.h"

namespace chtl {
namespace ast {

// DocumentNode
void DocumentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitDocument(this);
}

// CommentNode
void CommentNode::Accept(ASTVisitor* visitor) {
    visitor->VisitComment(this);
}

// TextNode
void TextNode::Accept(ASTVisitor* visitor) {
    visitor->VisitText(this);
}

// ElementNode
void ElementNode::Accept(ASTVisitor* visitor) {
    visitor->VisitElement(this);
}

void ElementNode::AddAttribute(std::shared_ptr<AttributeNode> attribute) {
    if (attribute) {
        AddChild(attribute);
    }
}

std::vector<std::shared_ptr<AttributeNode>> ElementNode::GetAttributes() const {
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    for (const auto& child : GetChildren()) {
        if (child->GetType() == ASTNodeType::ATTRIBUTE) {
            attributes.push_back(std::static_pointer_cast<AttributeNode>(child));
        }
    }
    return attributes;
}

std::shared_ptr<AttributeNode> ElementNode::GetAttribute(const std::string& name) const {
    for (const auto& child : GetChildren()) {
        if (child->GetType() == ASTNodeType::ATTRIBUTE) {
            auto attr = std::static_pointer_cast<AttributeNode>(child);
            if (attr->GetName() == name) {
                return attr;
            }
        }
    }
    return nullptr;
}

bool ElementNode::HasAttribute(const std::string& name) const {
    return GetAttribute(name) != nullptr;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::GetChildElements() const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : GetChildren()) {
        if (child->GetType() == ASTNodeType::ELEMENT) {
            elements.push_back(std::static_pointer_cast<ElementNode>(child));
        }
    }
    return elements;
}

std::vector<std::shared_ptr<TextNode>> ElementNode::GetTextNodes() const {
    std::vector<std::shared_ptr<TextNode>> texts;
    for (const auto& child : GetChildren()) {
        if (child->GetType() == ASTNodeType::TEXT_NODE) {
            texts.push_back(std::static_pointer_cast<TextNode>(child));
        }
    }
    return texts;
}

// AttributeNode
void AttributeNode::Accept(ASTVisitor* visitor) {
    visitor->VisitAttribute(this);
}

// StyleBlockNode
void StyleBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleBlock(this);
}

void StyleBlockNode::AddInlineProperty(const std::string& property, const std::string& value) {
    m_InlineProperties.push_back({property, value});
}

void StyleBlockNode::AddStyleRule(std::shared_ptr<StyleRuleNode> rule) {
    if (rule) {
        AddChild(rule);
        m_StyleRules.push_back(rule);
    }
}

void StyleBlockNode::AddStyleReference(std::shared_ptr<StyleReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_StyleReferences.push_back(reference);
    }
}

// StyleRuleNode
void StyleRuleNode::Accept(ASTVisitor* visitor) {
    visitor->VisitStyleBlock(nullptr); // StyleRule通过StyleBlock访问
}

void StyleRuleNode::AddProperty(const std::string& property, const std::string& value) {
    m_Properties.push_back({property, value});
}

// StyleReferenceNode
void StyleReferenceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitReference(this);
}

void StyleReferenceNode::AddOverride(const std::string& property, const std::string& value) {
    m_Overrides.push_back({property, value});
}

// ScriptBlockNode
void ScriptBlockNode::Accept(ASTVisitor* visitor) {
    visitor->VisitScriptBlock(this);
}

// TemplateNode
void TemplateNode::Accept(ASTVisitor* visitor) {
    visitor->VisitTemplate(this);
}

void TemplateNode::AddInheritedTemplate(const std::string& templateName) {
    m_InheritedTemplates.push_back(templateName);
}

// TemplateStyleNode
void TemplateStyleNode::AddProperty(const std::string& property, const std::string& value) {
    m_Properties.push_back({property, value});
}

void TemplateStyleNode::AddStyleReference(std::shared_ptr<StyleReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_StyleReferences.push_back(reference);
    }
}

// TemplateElementNode
void TemplateElementNode::AddElement(std::shared_ptr<ElementNode> element) {
    if (element) {
        AddChild(element);
        m_Elements.push_back(element);
    }
}

void TemplateElementNode::AddElementReference(std::shared_ptr<ElementReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_ElementReferences.push_back(reference);
    }
}

// TemplateVarNode
void TemplateVarNode::AddVariable(const std::string& varName, const std::string& value) {
    m_Variables.push_back({varName, value});
}

std::string TemplateVarNode::GetVariableValue(const std::string& varName) const {
    for (const auto& var : m_Variables) {
        if (var.first == varName) {
            return var.second;
        }
    }
    return "";
}

bool TemplateVarNode::HasVariable(const std::string& varName) const {
    for (const auto& var : m_Variables) {
        if (var.first == varName) {
            return true;
        }
    }
    return false;
}

// CustomNode
void CustomNode::Accept(ASTVisitor* visitor) {
    visitor->VisitCustom(this);
}

void CustomNode::AddInherited(const std::string& name, bool isTemplate) {
    m_Inherited.push_back({name, isTemplate});
}

// ImportNode
void ImportNode::Accept(ASTVisitor* visitor) {
    visitor->VisitImport(this);
}

// NamespaceNode
void NamespaceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitNamespace(this);
}

std::string NamespaceNode::GetFullyQualifiedName() const {
    if (m_ParentNamespace.empty()) {
        return m_Value;
    }
    return m_ParentNamespace + "." + m_Value;
}

// FromClauseNode
void FromClauseNode::Accept(ASTVisitor* visitor) {
    // FromClause通过其父节点访问
}

// ConfigurationNode
void ConfigurationNode::Accept(ASTVisitor* visitor) {
    visitor->VisitConfiguration(this);
}

void ConfigurationNode::AddProperty(const std::string& key, const std::string& value) {
    m_Properties.push_back({key, value});
}

std::string ConfigurationNode::GetProperty(const std::string& key) const {
    for (const auto& prop : m_Properties) {
        if (prop.first == key) {
            return prop.second;
        }
    }
    return "";
}

bool ConfigurationNode::HasProperty(const std::string& key) const {
    for (const auto& prop : m_Properties) {
        if (prop.first == key) {
            return true;
        }
    }
    return false;
}

void ConfigurationNode::SetNameBlock(std::shared_ptr<ConfigNameBlockNode> nameBlock) {
    m_NameBlock = nameBlock;
    if (nameBlock) {
        AddChild(nameBlock);
    }
}

void ConfigurationNode::SetOriginTypeBlock(std::shared_ptr<ConfigOriginTypeNode> originTypeBlock) {
    m_OriginTypeBlock = originTypeBlock;
    if (originTypeBlock) {
        AddChild(originTypeBlock);
    }
}

// ConfigNameBlockNode
void ConfigNameBlockNode::Accept(ASTVisitor* visitor) {
    // 通过Configuration节点访问
}

void ConfigNameBlockNode::AddNameMapping(const std::string& key, const std::string& value) {
    m_Mappings[key] = {value};
}

void ConfigNameBlockNode::AddNameMapping(const std::string& key, const std::vector<std::string>& values) {
    m_Mappings[key] = values;
}

std::vector<std::string> ConfigNameBlockNode::GetMapping(const std::string& key) const {
    auto it = m_Mappings.find(key);
    if (it != m_Mappings.end()) {
        return it->second;
    }
    return {};
}

bool ConfigNameBlockNode::HasMapping(const std::string& key) const {
    return m_Mappings.find(key) != m_Mappings.end();
}

// ConfigOriginTypeNode
void ConfigOriginTypeNode::Accept(ASTVisitor* visitor) {
    // 通过Configuration节点访问
}

void ConfigOriginTypeNode::AddOriginType(const std::string& typeName, const std::string& typeValue) {
    m_OriginTypes.push_back({typeName, typeValue});
}

std::string ConfigOriginTypeNode::GetOriginType(const std::string& typeName) const {
    for (const auto& type : m_OriginTypes) {
        if (type.first == typeName) {
            return type.second;
        }
    }
    return "";
}

bool ConfigOriginTypeNode::HasOriginType(const std::string& typeName) const {
    for (const auto& type : m_OriginTypes) {
        if (type.first == typeName) {
            return true;
        }
    }
    return false;
}

// OriginNode
void OriginNode::Accept(ASTVisitor* visitor) {
    visitor->VisitOrigin(this);
}

// ExceptNode
void ExceptNode::Accept(ASTVisitor* visitor) {
    visitor->VisitExcept(this);
}

// LiteralNode
void LiteralNode::Accept(ASTVisitor* visitor) {
    visitor->VisitLiteral(this);
}

// IdentifierNode
void IdentifierNode::Accept(ASTVisitor* visitor) {
    visitor->VisitIdentifier(this);
}

// ReferenceNode
void ReferenceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitReference(this);
}

// 特化节点实现
// InheritNode
void InheritNode::Accept(ASTVisitor* visitor) {
    // 通过父节点访问
}

// DeleteNode
void DeleteNode::Accept(ASTVisitor* visitor) {
    // 通过父节点访问
}

// InsertNode
void InsertNode::Accept(ASTVisitor* visitor) {
    // 通过父节点访问
}

// ElementReferenceNode
void ElementReferenceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitReference(this);
}

void ElementReferenceNode::AddSpecialization(const std::string& selector, std::shared_ptr<ASTNode> spec) {
    if (spec) {
        m_Specializations.push_back({selector, spec});
        AddChild(spec);
    }
}

void ElementReferenceNode::AddDeleteOperation(std::shared_ptr<DeleteNode> deleteOp) {
    if (deleteOp) {
        m_DeleteOperations.push_back(deleteOp);
        AddChild(deleteOp);
    }
}

void ElementReferenceNode::AddInsertOperation(std::shared_ptr<InsertNode> insert) {
    if (insert) {
        m_InsertOperations.push_back(insert);
        AddChild(insert);
    }
}

// VarReferenceNode
void VarReferenceNode::Accept(ASTVisitor* visitor) {
    visitor->VisitReference(this);
}

void VarReferenceNode::AddSpecialization(const std::string& varName, const std::string& value) {
    m_Specializations.push_back({varName, value});
}

// IndexAccessNode
void IndexAccessNode::Accept(ASTVisitor* visitor) {
    // 通过父节点访问
}

// 自定义节点特化实现
// CustomStyleNode
void CustomStyleNode::AddEmptyProperty(const std::string& property) {
    m_EmptyProperties.push_back(property);
}

void CustomStyleNode::AddProperty(const std::string& property, const std::string& value) {
    m_Properties.push_back({property, value});
}

void CustomStyleNode::AddStyleReference(std::shared_ptr<StyleReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_StyleReferences.push_back(reference);
    }
}

// CustomElementNode
void CustomElementNode::AddElement(std::shared_ptr<ElementNode> element) {
    if (element) {
        AddChild(element);
        m_Elements.push_back(element);
    }
}

void CustomElementNode::AddElementReference(std::shared_ptr<ElementReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_ElementReferences.push_back(reference);
    }
}

void CustomElementNode::AddInsertOperation(std::shared_ptr<InsertNode> insert) {
    if (insert) {
        AddChild(insert);
        m_InsertOperations.push_back(insert);
    }
}

void CustomElementNode::AddDeleteOperation(std::shared_ptr<DeleteNode> deleteOp) {
    if (deleteOp) {
        AddChild(deleteOp);
        m_DeleteOperations.push_back(deleteOp);
    }
}

// CustomVarNode
void CustomVarNode::AddVariable(const std::string& varName, const std::string& value) {
    m_Variables.push_back({varName, value});
}

void CustomVarNode::AddVarReference(std::shared_ptr<VarReferenceNode> reference) {
    if (reference) {
        AddChild(reference);
        m_VarReferences.push_back(reference);
    }
}

} // namespace ast
} // namespace chtl