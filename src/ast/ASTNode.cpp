#include "ASTNode.h"
#include <sstream>

namespace chtl {
namespace ast {

// ASTNode实现
void ASTNode::addChild(std::unique_ptr<ASTNode> child) {
    if (child) {
        children.push_back(std::move(child));
    }
}

ASTNode* ASTNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index].get();
    }
    return nullptr;
}

size_t ASTNode::getChildCount() const {
    return children.size();
}

void ASTNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string ASTNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool ASTNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

std::string ASTNode::toString() const {
    std::ostringstream oss;
    oss << getNodeTypeName() << " @" << position.line << ":" << position.column;
    
    if (!attributes.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& attr : attributes) {
            if (!first) oss << ", ";
            oss << attr.first << "=" << attr.second;
            first = false;
        }
        oss << "]";
    }
    
    if (!children.empty()) {
        oss << " (" << children.size() << " children)";
    }
    
    return oss.str();
}

std::string ASTNode::getNodeTypeName() const {
    return nodeTypeToString(type);
}

std::vector<ASTNode*> ASTNode::findChildrenByType(NodeType target_type) const {
    std::vector<ASTNode*> result;
    for (const auto& child : children) {
        if (child->type == target_type) {
            result.push_back(child.get());
        }
        // 递归查找子节点
        auto child_results = child->findChildrenByType(target_type);
        result.insert(result.end(), child_results.begin(), child_results.end());
    }
    return result;
}

ASTNode* ASTNode::findFirstChildByType(NodeType target_type) const {
    for (const auto& child : children) {
        if (child->type == target_type) {
            return child.get();
        }
        // 递归查找
        auto found = child->findFirstChildByType(target_type);
        if (found) return found;
    }
    return nullptr;
}

void ASTNode::cloneBaseProperties(ASTNode* target) const {
    if (!target) return;
    
    target->position = position;
    target->attributes = attributes;
    
    // 克隆子节点
    for (const auto& child : children) {
        target->addChild(child->clone());
    }
}

// ASTVisitor实现
void ASTVisitor::visit(ASTNode& node) {
    node.accept(*this);
}

// 工具函数实现
std::string nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::PROGRAM: return "Program";
        case NodeType::ELEMENT: return "Element";
        case NodeType::TEXT: return "Text";
        case NodeType::ATTRIBUTE: return "Attribute";
        case NodeType::STYLE_BLOCK: return "StyleBlock";
        case NodeType::SCRIPT_BLOCK: return "ScriptBlock";
        case NodeType::TEMPLATE_DEFINITION: return "TemplateDefinition";
        case NodeType::CUSTOM_DEFINITION: return "CustomDefinition";
        case NodeType::ORIGIN_BLOCK: return "OriginBlock";
        case NodeType::IMPORT_STATEMENT: return "ImportStatement";
        case NodeType::NAMESPACE_DEFINITION: return "NamespaceDefinition";
        case NodeType::CONFIGURATION_BLOCK: return "ConfigurationBlock";
        case NodeType::TEMPLATE_STYLE: return "TemplateStyle";
        case NodeType::TEMPLATE_ELEMENT: return "TemplateElement";
        case NodeType::TEMPLATE_VAR: return "TemplateVar";
        case NodeType::CUSTOM_STYLE: return "CustomStyle";
        case NodeType::CUSTOM_ELEMENT: return "CustomElement";
        case NodeType::CUSTOM_VAR: return "CustomVar";
        case NodeType::CSS_PROPERTY: return "CSSProperty";
        case NodeType::CSS_SELECTOR: return "CSSSelector";
        case NodeType::STYLE_INHERITANCE: return "StyleInheritance";
        case NodeType::STYLE_DELETE: return "StyleDelete";
        case NodeType::VARIABLE_REFERENCE: return "VariableReference";
        case NodeType::VARIABLE_SPECIALIZATION: return "VariableSpecialization";
        case NodeType::ELEMENT_INSERT: return "ElementInsert";
        case NodeType::ELEMENT_DELETE: return "ElementDelete";
        case NodeType::ELEMENT_INDEX: return "ElementIndex";
        case NodeType::CHTL_JS_SELECTOR: return "CHTLJSSelector";
        case NodeType::CHTL_JS_LISTEN: return "CHTLJSListen";
        case NodeType::CHTL_JS_DELEGATE: return "CHTLJSDelegate";
        case NodeType::CHTL_JS_ANIMATE: return "CHTLJSAnimate";
        case NodeType::CHTL_JS_VIR: return "CHTLJSVir";
        case NodeType::SINGLE_COMMENT: return "SingleComment";
        case NodeType::MULTI_COMMENT: return "MultiComment";
        case NodeType::GENERATOR_COMMENT: return "GeneratorComment";
        case NodeType::STRING_LITERAL: return "StringLiteral";
        case NodeType::NUMBER_LITERAL: return "NumberLiteral";
        case NodeType::UNQUOTED_LITERAL: return "UnquotedLiteral";
        case NodeType::IDENTIFIER: return "Identifier";
        case NodeType::EXCEPT_CONSTRAINT: return "ExceptConstraint";
        case NodeType::UNKNOWN: return "Unknown";
        default: return "UnknownNodeType";
    }
}

bool isTemplateNode(NodeType type) {
    return type == NodeType::TEMPLATE_DEFINITION ||
           type == NodeType::TEMPLATE_STYLE ||
           type == NodeType::TEMPLATE_ELEMENT ||
           type == NodeType::TEMPLATE_VAR;
}

bool isCustomNode(NodeType type) {
    return type == NodeType::CUSTOM_DEFINITION ||
           type == NodeType::CUSTOM_STYLE ||
           type == NodeType::CUSTOM_ELEMENT ||
           type == NodeType::CUSTOM_VAR;
}

bool isCHTLJSNode(NodeType type) {
    return type == NodeType::CHTL_JS_SELECTOR ||
           type == NodeType::CHTL_JS_LISTEN ||
           type == NodeType::CHTL_JS_DELEGATE ||
           type == NodeType::CHTL_JS_ANIMATE ||
           type == NodeType::CHTL_JS_VIR;
}

bool isStyleNode(NodeType type) {
    return type == NodeType::STYLE_BLOCK ||
           type == NodeType::CSS_PROPERTY ||
           type == NodeType::CSS_SELECTOR ||
           type == NodeType::STYLE_INHERITANCE ||
           type == NodeType::STYLE_DELETE;
}

} // namespace ast
} // namespace chtl