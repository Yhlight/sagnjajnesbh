#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 文本节点
 * 表示 text { } 块
 */
class TextNode : public ASTNode {
public:
    TextNode() : ASTNode(ASTNodeType::TEXT_NODE) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 设置文本内容（可能是字符串字面量或无引号字面量）
    void SetContent(const std::string& content) { m_Content = content; }
    const std::string& GetContent() const { return m_Content; }
    
    // 是否是字面量（带引号）
    void SetIsLiteral(bool isLiteral) { m_IsLiteral = isLiteral; }
    bool IsLiteral() const { return m_IsLiteral; }
    
private:
    std::string m_Content;
    bool m_IsLiteral = false;
};

} // namespace ast
} // namespace chtl