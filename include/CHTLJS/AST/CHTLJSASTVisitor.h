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
// 错误的前向声明已移除 - 这些类属于JavaScript语法或CJMOD扩展，不属于CHTL JS核心
// class INeverAwayBlockNode; - CJMOD扩展
// class FunctionDefinitionNode; - JavaScript语法
// class ObjectLiteralNode; - JavaScript语法
// class ArrayLiteralNode; - JavaScript语法
// class MethodCallNode; - JavaScript语法
// class VirtualMethodCallNode; - CJMOD扩展
// class VariableDeclarationNode; - JavaScript语法
// class AssignmentExpressionNode; - JavaScript语法
// class PropertyAccessNode; - JavaScript语法

// ArrowFunctionNode已移除 - CHTL JS不包含JS语法
class AnimationKeyframeNode;
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
    // 错误的访问方法已移除 - 这些节点属于JavaScript语法或CJMOD扩展
    // virtual void VisitINeverAwayBlockNode(INeverAwayBlockNode& node) = 0; - CJMOD扩展
    // virtual void VisitFunctionDefinitionNode(FunctionDefinitionNode& node) = 0; - JavaScript语法
    // virtual void VisitObjectLiteralNode(ObjectLiteralNode& node) = 0; - JavaScript语法
    // virtual void VisitArrayLiteralNode(ArrayLiteralNode& node) = 0; - JavaScript语法
    // virtual void VisitMethodCallNode(MethodCallNode& node) = 0; - JavaScript语法
    // virtual void VisitVirtualMethodCallNode(VirtualMethodCallNode& node) = 0; - CJMOD扩展
    // virtual void VisitVariableDeclarationNode(VariableDeclarationNode& node) = 0; - JavaScript语法
    // virtual void VisitAssignmentExpressionNode(AssignmentExpressionNode& node) = 0; - JavaScript语法
    // virtual void VisitPropertyAccessNode(PropertyAccessNode& node) = 0; - JavaScript语法
    
    // ArrowFunctionNode访问已移除 - CHTL JS不包含JS语法
    
    // 动画节点访问
    virtual void VisitAnimationKeyframeNode(AnimationKeyframeNode& node) = 0;
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
    // 错误的访问方法已移除 - 这些节点属于JavaScript语法或CJMOD扩展
    // void VisitINeverAwayBlockNode(INeverAwayBlockNode& node) override {} - CJMOD扩展
    // void VisitFunctionDefinitionNode(FunctionDefinitionNode& node) override {} - JavaScript语法
    // void VisitObjectLiteralNode(ObjectLiteralNode& node) override {} - JavaScript语法
    // void VisitArrayLiteralNode(ArrayLiteralNode& node) override {} - JavaScript语法
    // void VisitMethodCallNode(MethodCallNode& node) override {} - JavaScript语法
    // void VisitVirtualMethodCallNode(VirtualMethodCallNode& node) override {} - CJMOD扩展
    // void VisitVariableDeclarationNode(VariableDeclarationNode& node) override {} - JavaScript语法
    // void VisitAssignmentExpressionNode(AssignmentExpressionNode& node) override {} - JavaScript语法
    // void VisitPropertyAccessNode(PropertyAccessNode& node) override {} - JavaScript语法
    
    // VisitArrowFunctionNode已移除 - CHTL JS不包含JS语法
    void VisitAnimationKeyframeNode(AnimationKeyframeNode& node) override {}
};

} // namespace AST
} // namespace CHTLJS
} // namespace CHTL