#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 属性节点
 * 表示元素属性，如 id: "main";
 */
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value = "")
        : ASTNode(ASTNodeType::ATTRIBUTE, name)
        , m_AttributeValue(value) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 属性名
    const std::string& GetName() const { return m_Value; }
    
    // 属性值
    void SetAttributeValue(const std::string& value) { m_AttributeValue = value; }
    const std::string& GetAttributeValue() const { return m_AttributeValue; }
    
    // 是否使用了冒号（:）或等号（=）
    void SetUseColon(bool useColon) { m_UseColon = useColon; }
    bool UseColon() const { return m_UseColon; }
    
    // 值是否是字面量（带引号）
    void SetValueIsLiteral(bool isLiteral) { m_ValueIsLiteral = isLiteral; }
    bool ValueIsLiteral() const { return m_ValueIsLiteral; }
    
private:
    std::string m_AttributeValue;
    bool m_UseColon = true;  // 默认使用冒号
    bool m_ValueIsLiteral = false;
};

} // namespace ast
} // namespace chtl