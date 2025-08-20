#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 箭头操作符节点
 * 表示 -> 操作符（CHTL JS特有，与.等价）
 */
class ArrowOperatorNode : public CHTLJSASTNode {
public:
    ArrowOperatorNode()
        : CHTLJSASTNode(CHTLJSNodeType::ARROW_OPERATOR, "->") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 左操作数
    void SetLeft(std::shared_ptr<CHTLJSASTNode> left) {
        m_Left = left;
        if (left) {
            AddChild(left);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetLeft() const { return m_Left; }
    
    // 右操作数（通常是方法名或属性）
    void SetRight(std::shared_ptr<CHTLJSASTNode> right) {
        m_Right = right;
        if (right) {
            AddChild(right);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetRight() const { return m_Right; }
    
    // 是否是方法调用
    void SetIsMethodCall(bool isCall) { m_IsMethodCall = isCall; }
    bool IsMethodCall() const { return m_IsMethodCall; }
    
private:
    std::shared_ptr<CHTLJSASTNode> m_Left;
    std::shared_ptr<CHTLJSASTNode> m_Right;
    bool m_IsMethodCall = false;
};

/**
 * 点操作符节点
 * 表示 . 操作符（与->功能相同，但用于明确是否使用CHTL JS语法）
 */
class DotOperatorNode : public CHTLJSASTNode {
public:
    DotOperatorNode()
        : CHTLJSASTNode(CHTLJSNodeType::DOT_OPERATOR, ".") {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 左操作数
    void SetLeft(std::shared_ptr<CHTLJSASTNode> left) {
        m_Left = left;
        if (left) {
            AddChild(left);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetLeft() const { return m_Left; }
    
    // 右操作数
    void SetRight(std::shared_ptr<CHTLJSASTNode> right) {
        m_Right = right;
        if (right) {
            AddChild(right);
        }
    }
    std::shared_ptr<CHTLJSASTNode> GetRight() const { return m_Right; }
    
    // 是否是方法调用
    void SetIsMethodCall(bool isCall) { m_IsMethodCall = isCall; }
    bool IsMethodCall() const { return m_IsMethodCall; }
    
private:
    std::shared_ptr<CHTLJSASTNode> m_Left;
    std::shared_ptr<CHTLJSASTNode> m_Right;
    bool m_IsMethodCall = false;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl