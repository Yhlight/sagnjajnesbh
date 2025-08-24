#include "CHTL/AST/ASTNode.h"
#include <iostream>
#include <iomanip>

namespace CHTL {
namespace Compiler {

std::string ASTNode::GetTypeName() const {
    switch (type) {
        case ASTNodeType::ROOT: return "ROOT";
        case ASTNodeType::DOCUMENT: return "DOCUMENT";
        
        // 声明节点
        case ASTNodeType::TEMPLATE_DECLARATION: return "TEMPLATE_DECLARATION";
        case ASTNodeType::CUSTOM_DECLARATION: return "CUSTOM_DECLARATION";
        case ASTNodeType::ORIGIN_DECLARATION: return "ORIGIN_DECLARATION";
        case ASTNodeType::CONFIGURATION_DECLARATION: return "CONFIGURATION_DECLARATION";
        case ASTNodeType::NAMESPACE_DECLARATION: return "NAMESPACE_DECLARATION";
        case ASTNodeType::IMPORT_DECLARATION: return "IMPORT_DECLARATION";
        case ASTNodeType::INFO_DECLARATION: return "INFO_DECLARATION";
        case ASTNodeType::EXPORT_DECLARATION: return "EXPORT_DECLARATION";
        
        // 类型声明节点
        case ASTNodeType::STYLE_DECLARATION: return "STYLE_DECLARATION";
        case ASTNodeType::ELEMENT_DECLARATION: return "ELEMENT_DECLARATION";
        case ASTNodeType::VAR_DECLARATION: return "VAR_DECLARATION";
        
        // 元素节点
        case ASTNodeType::HTML_ELEMENT: return "HTML_ELEMENT";
        case ASTNodeType::TEXT_NODE: return "TEXT_NODE";
        case ASTNodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case ASTNodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        
        // 样式相关节点
        case ASTNodeType::STYLE_RULE: return "STYLE_RULE";
        case ASTNodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
        case ASTNodeType::STYLE_SELECTOR: return "STYLE_SELECTOR";
        case ASTNodeType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case ASTNodeType::ID_SELECTOR: return "ID_SELECTOR";
        case ASTNodeType::PSEUDO_SELECTOR: return "PSEUDO_SELECTOR";
        case ASTNodeType::AMPERSAND_SELECTOR: return "AMPERSAND_SELECTOR";
        
        // 使用节点
        case ASTNodeType::STYLE_USE: return "STYLE_USE";
        case ASTNodeType::ELEMENT_USE: return "ELEMENT_USE";
        case ASTNodeType::VAR_USE: return "VAR_USE";
        
        // 特例化节点
        case ASTNodeType::SPECIALIZATION: return "SPECIALIZATION";
        case ASTNodeType::DELETE_STATEMENT: return "DELETE_STATEMENT";
        case ASTNodeType::INSERT_STATEMENT: return "INSERT_STATEMENT";
        
        // 表达式节点
        case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
        case ASTNodeType::STRING_LITERAL: return "STRING_LITERAL";
        case ASTNodeType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case ASTNodeType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case ASTNodeType::VAR_EXPRESSION: return "VAR_EXPRESSION";
        case ASTNodeType::FROM_EXPRESSION: return "FROM_EXPRESSION";
        case ASTNodeType::INDEX_EXPRESSION: return "INDEX_EXPRESSION";
        
        // 语句节点
        case ASTNodeType::INHERIT_STATEMENT: return "INHERIT_STATEMENT";
        case ASTNodeType::EXCEPT_STATEMENT: return "EXCEPT_STATEMENT";
        case ASTNodeType::ATTRIBUTE_STATEMENT: return "ATTRIBUTE_STATEMENT";
        
        // 配置相关节点
        case ASTNodeType::CONFIG_PROPERTY: return "CONFIG_PROPERTY";
        case ASTNodeType::CONFIG_NAME_BLOCK: return "CONFIG_NAME_BLOCK";
        case ASTNodeType::CONFIG_ORIGINTYPE_BLOCK: return "CONFIG_ORIGINTYPE_BLOCK";
        
        // 其他
        case ASTNodeType::COMMENT: return "COMMENT";
        case ASTNodeType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        default: return "UNKNOWN";
    }
}

void ASTNode::Dump(int indent) const {
    // 输出缩进
    std::cout << std::string(indent * 2, ' ');
    
    // 输出节点类型
    std::cout << GetTypeName();
    
    // 输出值（如果有）
    if (!value.empty()) {
        std::cout << " [" << value << "]";
    }
    
    // 输出属性（如果有）
    if (!attributes.empty()) {
        std::cout << " {";
        bool first = true;
        for (const auto& [key, val] : attributes) {
            if (!first) std::cout << ", ";
            std::cout << key << "=\"" << val << "\"";
            first = false;
        }
        std::cout << "}";
    }
    
    // 输出位置信息
    std::cout << " (" << line << ":" << column << ")";
    
    std::cout << std::endl;
    
    // 递归输出子节点
    for (const auto& child : children) {
        if (child) {
            child->Dump(indent + 1);
        }
    }
}

} // namespace Compiler
} // namespace CHTL