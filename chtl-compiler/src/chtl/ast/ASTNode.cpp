#include "chtl/ast/ASTNode.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace ast {

void ASTNode::AddChild(std::shared_ptr<ASTNode> child) {
    if (child) {
        m_Children.push_back(child);
    }
}

std::shared_ptr<ASTNode> ASTNode::GetChild(size_t index) const {
    if (index < m_Children.size()) {
        return m_Children[index];
    }
    return nullptr;
}

void ASTNode::SetAttribute(const std::string& key, const std::string& value) {
    m_Attributes[key] = value;
}

std::string ASTNode::GetAttribute(const std::string& key) const {
    auto it = m_Attributes.find(key);
    if (it != m_Attributes.end()) {
        return it->second;
    }
    return "";
}

bool ASTNode::HasAttribute(const std::string& key) const {
    return m_Attributes.find(key) != m_Attributes.end();
}

std::string ASTNode::TypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::DOCUMENT: return "Document";
        case ASTNodeType::COMMENT_SINGLE: return "SingleLineComment";
        case ASTNodeType::COMMENT_MULTI: return "MultiLineComment";
        case ASTNodeType::COMMENT_GENERATOR: return "GeneratorComment";
        case ASTNodeType::TEXT_NODE: return "Text";
        case ASTNodeType::ELEMENT: return "Element";
        case ASTNodeType::ATTRIBUTE: return "Attribute";
        case ASTNodeType::STYLE_BLOCK: return "StyleBlock";
        case ASTNodeType::STYLE_RULE: return "StyleRule";
        case ASTNodeType::STYLE_PROPERTY: return "StyleProperty";
        case ASTNodeType::STYLE_SELECTOR: return "StyleSelector";
        case ASTNodeType::SCRIPT_BLOCK: return "ScriptBlock";
        case ASTNodeType::TEMPLATE_STYLE: return "TemplateStyle";
        case ASTNodeType::TEMPLATE_ELEMENT: return "TemplateElement";
        case ASTNodeType::TEMPLATE_VAR: return "TemplateVar";
        case ASTNodeType::CUSTOM_STYLE: return "CustomStyle";
        case ASTNodeType::CUSTOM_ELEMENT: return "CustomElement";
        case ASTNodeType::CUSTOM_VAR: return "CustomVar";
        case ASTNodeType::INHERIT: return "Inherit";
        case ASTNodeType::DELETE: return "Delete";
        case ASTNodeType::INSERT: return "Insert";
        case ASTNodeType::ORIGIN_BLOCK: return "Origin";
        case ASTNodeType::CONFIGURATION: return "Configuration";
        case ASTNodeType::CONFIG_PROPERTY: return "ConfigProperty";
        case ASTNodeType::CONFIG_NAME_BLOCK: return "ConfigNameBlock";
        case ASTNodeType::CONFIG_ORIGIN_TYPE: return "ConfigOriginType";
        case ASTNodeType::IMPORT: return "Import";
        case ASTNodeType::NAMESPACE: return "Namespace";
        case ASTNodeType::EXCEPT: return "Except";
        case ASTNodeType::STRING_LITERAL: return "StringLiteral";
        case ASTNodeType::UNQUOTED_LITERAL: return "UnquotedLiteral";
        case ASTNodeType::NUMBER_LITERAL: return "NumberLiteral";
        case ASTNodeType::IDENTIFIER: return "Identifier";
        case ASTNodeType::STYLE_REFERENCE: return "StyleReference";
        case ASTNodeType::ELEMENT_REFERENCE: return "ElementReference";
        case ASTNodeType::VAR_REFERENCE: return "VarReference";
        case ASTNodeType::VAR_CALL: return "VarCall";
        case ASTNodeType::INDEX_ACCESS: return "IndexAccess";
        case ASTNodeType::FROM_CLAUSE: return "FromClause";
        case ASTNodeType::AS_CLAUSE: return "AsClause";
        case ASTNodeType::ERROR: return "Error";
        default: return "Unknown";
    }
}

std::string ASTNode::ToString() const {
    std::stringstream ss;
    ss << GetTypeString();
    if (!m_Value.empty()) {
        ss << "(" << m_Value << ")";
    }
    ss << " at " << m_Line << ":" << m_Column;
    return ss.str();
}

void ASTNode::Print(int indent) const {
    // 打印缩进
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    
    // 打印节点信息
    std::cout << ToString() << std::endl;
    
    // 打印属性
    if (!m_Attributes.empty()) {
        for (int i = 0; i < indent + 1; ++i) {
            std::cout << "  ";
        }
        std::cout << "Attributes: ";
        for (const auto& attr : m_Attributes) {
            std::cout << attr.first << "=" << attr.second << " ";
        }
        std::cout << std::endl;
    }
    
    // 递归打印子节点
    for (const auto& child : m_Children) {
        if (child) {
            child->Print(indent + 1);
        }
    }
}

} // namespace ast
} // namespace chtl