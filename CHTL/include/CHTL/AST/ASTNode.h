#pragma once
#include <memory>
#include <vector>
#include <string>

namespace CHTL {
namespace Compiler {

// AST节点类型
enum class ASTNodeType {
    // 根节点
    ROOT,
    
    // 声明节点
    TEMPLATE_DECLARATION,
    CUSTOM_DECLARATION,
    ORIGIN_DECLARATION,
    CONFIGURATION_DECLARATION,
    NAMESPACE_DECLARATION,
    IMPORT_DECLARATION,
    
    // 元素节点
    HTML_ELEMENT,
    TEXT_NODE,
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    
    // 样式节点
    STYLE_RULE,
    STYLE_PROPERTY,
    STYLE_SELECTOR,
    
    // 表达式节点
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    VAR_EXPRESSION,
    FROM_EXPRESSION,
    
    // 语句节点
    DELETE_STATEMENT,
    INSERT_STATEMENT,
    INHERIT_STATEMENT,
    
    // 其他
    COMMENT,
    ATTRIBUTE
};

// AST节点基类
class ASTNode {
public:
    ASTNodeType type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::shared_ptr<ASTNode>> children;
    
    ASTNode(ASTNodeType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    virtual ~ASTNode() = default;
    
    // 添加子节点
    void AddChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
    
    // 获取节点类型名称
    virtual std::string GetTypeName() const {
        switch (type) {
            case ASTNodeType::ROOT: return "ROOT";
            case ASTNodeType::TEMPLATE_DECLARATION: return "TEMPLATE_DECLARATION";
            case ASTNodeType::CUSTOM_DECLARATION: return "CUSTOM_DECLARATION";
            case ASTNodeType::ORIGIN_DECLARATION: return "ORIGIN_DECLARATION";
            case ASTNodeType::CONFIGURATION_DECLARATION: return "CONFIGURATION_DECLARATION";
            case ASTNodeType::NAMESPACE_DECLARATION: return "NAMESPACE_DECLARATION";
            case ASTNodeType::IMPORT_DECLARATION: return "IMPORT_DECLARATION";
            case ASTNodeType::HTML_ELEMENT: return "HTML_ELEMENT";
            case ASTNodeType::TEXT_NODE: return "TEXT_NODE";
            case ASTNodeType::STYLE_BLOCK: return "STYLE_BLOCK";
            case ASTNodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
            case ASTNodeType::STYLE_RULE: return "STYLE_RULE";
            case ASTNodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
            case ASTNodeType::STYLE_SELECTOR: return "STYLE_SELECTOR";
            case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
            case ASTNodeType::STRING_LITERAL: return "STRING_LITERAL";
            case ASTNodeType::NUMBER_LITERAL: return "NUMBER_LITERAL";
            case ASTNodeType::VAR_EXPRESSION: return "VAR_EXPRESSION";
            case ASTNodeType::FROM_EXPRESSION: return "FROM_EXPRESSION";
            case ASTNodeType::DELETE_STATEMENT: return "DELETE_STATEMENT";
            case ASTNodeType::INSERT_STATEMENT: return "INSERT_STATEMENT";
            case ASTNodeType::INHERIT_STATEMENT: return "INHERIT_STATEMENT";
            case ASTNodeType::COMMENT: return "COMMENT";
            case ASTNodeType::ATTRIBUTE: return "ATTRIBUTE";
            default: return "UNKNOWN";
        }
    }
};

} // namespace Compiler
} // namespace CHTL