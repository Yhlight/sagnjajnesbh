#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace simple {

/**
 * AST 节点类型
 */
enum class ASTNodeType {
    ELEMENT,
    TEXT,
    ATTRIBUTE,
    DOCUMENT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    VARIABLE
};

/**
 * AST 节点结构
 */
struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::vector<std::pair<std::string, std::string>> attributes;
    
    explicit ASTNode(ASTNodeType t = ASTNodeType::DOCUMENT) : type(t) {}
    
    /**
     * 添加子节点
     */
    void AddChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
    
    /**
     * 添加属性
     */
    void AddAttribute(const std::string& name, const std::string& val) {
        attributes.push_back({name, val});
    }
    
    /**
     * 获取节点类型名称
     */
    std::string GetTypeName() const {
        switch (type) {
            case ASTNodeType::ELEMENT: return "ELEMENT";
            case ASTNodeType::TEXT: return "TEXT";
            case ASTNodeType::ATTRIBUTE: return "ATTRIBUTE";
            case ASTNodeType::DOCUMENT: return "DOCUMENT";
            case ASTNodeType::STYLE: return "STYLE";
            case ASTNodeType::SCRIPT: return "SCRIPT";
            default: return "UNKNOWN";
        }
    }
};

} // namespace simple
} // namespace chtl