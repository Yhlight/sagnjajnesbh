#pragma once

namespace CHTL {
namespace CHTLJS {
namespace AST {

// 前向声明
class RootNode;
class IdentifierNode;
class LiteralNode;
class EnhancedSelectorNode;
class VirtualObjectNode;
class ListenBlockNode;
class DelegateBlockNode;
class AnimateBlockNode;
class INeverAwayBlockNode;
class FunctionDefinitionNode;
class ArrowFunctionNode;
class ObjectLiteralNode;
class ArrayLiteralNode;
class MethodCallNode;
class VirtualMethodCallNode;
class AnimationKeyframeNode;
class VariableDeclarationNode;
class AssignmentExpressionNode;
class PropertyAccessNode;
class CommentNode;

/**
 * @brief CHTL JS AST访问者接口
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础节点访问
    virtual void VisitRootNode(RootNode& node) = 0;
    virtual void VisitIdentifierNode(IdentifierNode& node) = 0;
    virtual void VisitLiteralNode(LiteralNode& node) = 0;
    virtual void VisitCommentNode(CommentNode& node) = 0;
    
    // CHTL JS特殊节点访问
    virtual void VisitEnhancedSelectorNode(EnhancedSelectorNode& node) = 0;
    virtual void VisitVirtualObjectNode(VirtualObjectNode& node) = 0;
    virtual void VisitListenBlockNode(ListenBlockNode& node) = 0;
    virtual void VisitDelegateBlockNode(DelegateBlockNode& node) = 0;
    virtual void VisitAnimateBlockNode(AnimateBlockNode& node) = 0;
    virtual void VisitINeverAwayBlockNode(INeverAwayBlockNode& node) = 0;
    
    // 函数相关节点访问
    virtual void VisitFunctionDefinitionNode(FunctionDefinitionNode& node) = 0;
    virtual void VisitArrowFunctionNode(ArrowFunctionNode& node) = 0;
    
    // 对象和数组节点访问
    virtual void VisitObjectLiteralNode(ObjectLiteralNode& node) = 0;
    virtual void VisitArrayLiteralNode(ArrayLiteralNode& node) = 0;
    
    // 方法调用节点访问
    virtual void VisitMethodCallNode(MethodCallNode& node) = 0;
    virtual void VisitVirtualMethodCallNode(VirtualMethodCallNode& node) = 0;
    
    // 动画节点访问
    virtual void VisitAnimationKeyframeNode(AnimationKeyframeNode& node) = 0;
    
    // 表达式节点访问
    virtual void VisitVariableDeclarationNode(VariableDeclarationNode& node) = 0;
    virtual void VisitAssignmentExpressionNode(AssignmentExpressionNode& node) = 0;
    virtual void VisitPropertyAccessNode(PropertyAccessNode& node) = 0;
};

/**
 * @brief 基础AST访问者（提供默认空实现）
 */
class BaseASTVisitor : public ASTVisitor {
public:
    void VisitRootNode(RootNode& node) override {}
    void VisitIdentifierNode(IdentifierNode& node) override {}
    void VisitLiteralNode(LiteralNode& node) override {}
    void VisitCommentNode(CommentNode& node) override {}
    void VisitEnhancedSelectorNode(EnhancedSelectorNode& node) override {}
    void VisitVirtualObjectNode(VirtualObjectNode& node) override {}
    void VisitListenBlockNode(ListenBlockNode& node) override {}
    void VisitDelegateBlockNode(DelegateBlockNode& node) override {}
    void VisitAnimateBlockNode(AnimateBlockNode& node) override {}
    void VisitINeverAwayBlockNode(INeverAwayBlockNode& node) override {}
    void VisitFunctionDefinitionNode(FunctionDefinitionNode& node) override {}
    void VisitArrowFunctionNode(ArrowFunctionNode& node) override {}
    void VisitObjectLiteralNode(ObjectLiteralNode& node) override {}
    void VisitArrayLiteralNode(ArrayLiteralNode& node) override {}
    void VisitMethodCallNode(MethodCallNode& node) override {}
    void VisitVirtualMethodCallNode(VirtualMethodCallNode& node) override {}
    void VisitAnimationKeyframeNode(AnimationKeyframeNode& node) override {}
    void VisitVariableDeclarationNode(VariableDeclarationNode& node) override {}
    void VisitAssignmentExpressionNode(AssignmentExpressionNode& node) override {}
    void VisitPropertyAccessNode(PropertyAccessNode& node) override {}
};

} // namespace AST
} // namespace CHTLJS
} // namespace CHTL