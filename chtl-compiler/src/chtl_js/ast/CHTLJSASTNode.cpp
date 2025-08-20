#include "chtl_js/ast/CHTLJSASTNode.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace chtljs {
namespace ast {

void CHTLJSASTNode::AddChild(std::shared_ptr<CHTLJSASTNode> child) {
    if (child) {
        m_Children.push_back(child);
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSASTNode::GetChild(size_t index) const {
    if (index < m_Children.size()) {
        return m_Children[index];
    }
    return nullptr;
}

void CHTLJSASTNode::SetAttribute(const std::string& key, const std::string& value) {
    m_Attributes[key] = value;
}

std::string CHTLJSASTNode::GetAttribute(const std::string& key) const {
    auto it = m_Attributes.find(key);
    if (it != m_Attributes.end()) {
        return it->second;
    }
    return "";
}

bool CHTLJSASTNode::HasAttribute(const std::string& key) const {
    return m_Attributes.find(key) != m_Attributes.end();
}

std::string CHTLJSASTNode::TypeToString(CHTLJSNodeType type) {
    switch (type) {
        case CHTLJSNodeType::DOCUMENT: return "CHTLJSDocument";
        case CHTLJSNodeType::SCRIPT_BLOCK: return "ScriptBlock";
        case CHTLJSNodeType::ENHANCED_SELECTOR: return "EnhancedSelector";
        case CHTLJSNodeType::SELECTOR_ACCESS: return "SelectorAccess";
        case CHTLJSNodeType::ARROW_OPERATOR: return "ArrowOperator";
        case CHTLJSNodeType::DOT_OPERATOR: return "DotOperator";
        case CHTLJSNodeType::LISTEN_CALL: return "ListenCall";
        case CHTLJSNodeType::DELEGATE_CALL: return "DelegateCall";
        case CHTLJSNodeType::ANIMATE_CALL: return "AnimateCall";
        case CHTLJSNodeType::I_NEVER_AWAY_CALL: return "INeverAwayCall";
        case CHTLJSNodeType::PRINT_MY_LOVE_CALL: return "PrintMyLoveCall";
        case CHTLJSNodeType::VIR_DECLARATION: return "VirDeclaration";
        case CHTLJSNodeType::VIR_ACCESS: return "VirAccess";
        case CHTLJSNodeType::STATE_TAG: return "StateTag";
        case CHTLJSNodeType::FUNCTION_WITH_STATE: return "FunctionWithState";
        case CHTLJSNodeType::OBJECT_LITERAL: return "ObjectLiteral";
        case CHTLJSNodeType::PROPERTY: return "Property";
        case CHTLJSNodeType::FUNCTION_DECLARATION: return "FunctionDeclaration";
        case CHTLJSNodeType::ARROW_FUNCTION: return "ArrowFunction";
        case CHTLJSNodeType::PARAMETER_LIST: return "ParameterList";
        case CHTLJSNodeType::IDENTIFIER: return "Identifier";
        case CHTLJSNodeType::STRING_LITERAL: return "StringLiteral";
        case CHTLJSNodeType::NUMBER_LITERAL: return "NumberLiteral";
        case CHTLJSNodeType::BOOLEAN_LITERAL: return "BooleanLiteral";
        case CHTLJSNodeType::ARRAY_LITERAL: return "ArrayLiteral";
        case CHTLJSNodeType::EXPRESSION_STATEMENT: return "ExpressionStatement";
        case CHTLJSNodeType::BLOCK_STATEMENT: return "BlockStatement";
        case CHTLJSNodeType::JS_CODE_FRAGMENT: return "JSCodeFragment";
        case CHTLJSNodeType::ERROR: return "Error";
        default: return "Unknown";
    }
}

std::string CHTLJSASTNode::ToString() const {
    std::stringstream ss;
    ss << GetTypeString();
    if (!m_Value.empty()) {
        ss << "(" << m_Value << ")";
    }
    ss << " at " << m_Line << ":" << m_Column;
    return ss.str();
}

void CHTLJSASTNode::Print(int indent) const {
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
} // namespace chtljs
} // namespace chtl