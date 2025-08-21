#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 原始嵌入节点
 * 表示 [Origin] 块
 */
class OriginNode : public ASTNode {
public:
    OriginNode() : ASTNode(ASTNodeType::ORIGIN_BLOCK) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 原始类型（@Html, @Style, @JavaScript, 或自定义）
    void SetOriginType(const std::string& type) { m_OriginType = type; }
    const std::string& GetOriginType() const { return m_OriginType; }
    
    // 名称（可选）
    void SetOriginName(const std::string& name) { m_Value = name; }
    const std::string& GetOriginName() const { return m_Value; }
    bool IsNamed() const { return !m_Value.empty(); }
    
    // 原始内容
    void SetContent(const std::string& content) { m_Content = content; }
    const std::string& GetContent() const { return m_Content; }
    
    // 是否是引用（使用已命名的原始块）
    void SetIsReference(bool isRef) { m_IsReference = isRef; }
    bool IsReference() const { return m_IsReference; }
    
private:
    std::string m_OriginType;
    std::string m_Content;
    bool m_IsReference = false;
};

} // namespace ast
} // namespace chtl