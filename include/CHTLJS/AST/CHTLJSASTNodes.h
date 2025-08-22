#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace CHTLJS {
namespace AST {

// 前向声明
class ASTVisitor;
class ASTNode;

using ASTNodePtr = std::shared_ptr<ASTNode>;
using ASTNodeList = std::vector<ASTNodePtr>;

/**
 * @brief CHTL JS AST节点类型枚举
 */
enum class NodeType {
    // 根节点
    ROOT = 0,
    
    // 基础节点
    IDENTIFIER,                 // 标识符
    LITERAL,                    // 字面量
    
    // CHTL JS核心语法节点（严格按照语法文档第1099-1531行）
    ENHANCED_SELECTOR,          // 增强选择器 {{selector}} - 语法文档第1130行
    VIRTUAL_OBJECT,             // 虚对象 vir - 语法文档第1274行
    LISTEN_BLOCK,               // 监听器块 listen({...}) - 语法文档第1184行
    DELEGATE_BLOCK,             // 事件委托块 delegate({...}) - 语法文档第1215行
    ANIMATE_BLOCK,              // 动画块 animate({...}) - 语法文档第1233行
    ARROW_FUNCTION,             // 箭头函数 () => {} - 语法文档第1202行支持
    
    // 事件相关
    EVENT_HANDLER,              // 事件处理器
    EVENT_TARGET,               // 事件目标
    
    // 动画相关
    ANIMATION_KEYFRAME,         // 动画关键帧
    ANIMATION_PROPERTY,         // 动画属性
    CSS_IN_ANIMATION,           // 动画中的CSS
    
    // 注释
    COMMENT,
    
    NODE_TYPE_COUNT
};

/**
 * @brief CHTL JS AST节点基类
 */
class ASTNode {
public:
    /**
     * @brief 构造函数
     */
    ASTNode(NodeType type, const Core::CHTLJSToken& token = Core::CHTLJSToken());
    
    /**
     * @brief 虚析构函数
     */
    virtual ~ASTNode() = default;
    
    /**
     * @brief 访问者模式接口
     */
    virtual void Accept(ASTVisitor& visitor) = 0;
    
    /**
     * @brief 克隆节点
     */
    virtual ASTNodePtr Clone() const = 0;
    
    /**
     * @brief 转换为字符串
     */
    virtual std::string ToString() const = 0;
    
    /**
     * @brief 获取节点类型
     */
    NodeType GetType() const { return type_; }
    
    /**
     * @brief 获取Token
     */
    const Core::CHTLJSToken& GetToken() const { return token_; }
    
    /**
     * @brief 添加子节点
     */
    void AddChild(ASTNodePtr child);
    
    /**
     * @brief 获取子节点
     */
    const ASTNodeList& GetChildren() const { return children_; }
    
    /**
     * @brief 获取子节点数量
     */
    size_t GetChildCount() const { return children_.size(); }
    
    /**
     * @brief 设置父节点
     */
    void SetParent(ASTNode* parent) { parent_ = parent; }
    
    /**
     * @brief 获取父节点
     */
    ASTNode* GetParent() const { return parent_; }

protected:
    NodeType type_;                 // 节点类型
    Core::CHTLJSToken token_;       // 关联的Token
    ASTNodeList children_;          // 子节点列表
    ASTNode* parent_;               // 父节点指针
};

/**
 * @brief 根节点
 */
class RootNode : public ASTNode {
public:
    RootNode();
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
};

/**
 * @brief 标识符节点
 */
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    const std::string& GetName() const { return name_; }

private:
    std::string name_;
};

/**
 * @brief 字面量节点
 */
class LiteralNode : public ASTNode {
public:
    enum class LiteralType {
        STRING,
        NUMBER,
        BOOLEAN,
        NULL_LITERAL
    };
    
    LiteralNode(LiteralType literalType, const std::string& value, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    LiteralType GetLiteralType() const { return literalType_; }
    const std::string& GetValue() const { return value_; }

private:
    LiteralType literalType_;
    std::string value_;
};

/**
 * @brief 增强选择器节点
 */
class EnhancedSelectorNode : public ASTNode {
public:
    enum class SelectorType {
        TAG,            // {{button}}
        CLASS,          // {{.box}}
        ID,             // {{#box}}
        COMPLEX,        // {{.box button}}
        INDEXED         // {{button[0]}}
    };
    
    EnhancedSelectorNode(const std::string& selector, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    const std::string& GetSelector() const { return selector_; }
    SelectorType GetSelectorType() const { return selectorType_; }
    const std::string& GetParsedSelector() const { return parsedSelector_; }
    int GetIndex() const { return index_; }

private:
    std::string selector_;          // 原始选择器
    SelectorType selectorType_;     // 选择器类型
    std::string parsedSelector_;    // 解析后的选择器
    int index_;                     // 索引（用于indexed类型）
    
    void ParseSelector();
};

/**
 * @brief 虚对象节点
 */
class VirtualObjectNode : public ASTNode {
public:
    VirtualObjectNode(const std::string& name, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    const std::string& GetName() const { return name_; }
    void SetAssignment(ASTNodePtr assignment) { assignment_ = assignment; }
    ASTNodePtr GetAssignment() const { return assignment_; }

private:
    std::string name_;
    ASTNodePtr assignment_;
};

/**
 * @brief 监听器块节点
 */
class ListenBlockNode : public ASTNode {
public:
    ListenBlockNode(const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    void AddEventHandler(const std::string& eventType, ASTNodePtr handler);
    const std::unordered_map<std::string, ASTNodePtr>& GetEventHandlers() const { return eventHandlers_; }

private:
    std::unordered_map<std::string, ASTNodePtr> eventHandlers_;
};

/**
 * @brief 事件委托块节点
 */
class DelegateBlockNode : public ASTNode {
public:
    DelegateBlockNode(const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    void SetTarget(ASTNodePtr target) { target_ = target; }
    ASTNodePtr GetTarget() const { return target_; }
    void AddEventHandler(const std::string& eventType, ASTNodePtr handler);
    const std::unordered_map<std::string, ASTNodePtr>& GetEventHandlers() const { return eventHandlers_; }

private:
    ASTNodePtr target_;
    std::unordered_map<std::string, ASTNodePtr> eventHandlers_;
};

/**
 * @brief 动画块节点
 */
class AnimateBlockNode : public ASTNode {
public:
    AnimateBlockNode(const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    void SetTarget(ASTNodePtr target) { target_ = target; }
    void SetDuration(int duration) { duration_ = duration; }
    void SetEasing(const std::string& easing) { easing_ = easing; }
    void SetBegin(ASTNodePtr begin) { begin_ = begin; }
    void SetEnd(ASTNodePtr end) { end_ = end; }
    void AddKeyframe(ASTNodePtr keyframe) { keyframes_.push_back(keyframe); }
    void SetLoop(int loop) { loop_ = loop; }
    void SetDelay(int delay) { delay_ = delay; }
    void SetCallback(ASTNodePtr callback) { callback_ = callback; }
    
    ASTNodePtr GetTarget() const { return target_; }
    int GetDuration() const { return duration_; }
    const std::string& GetEasing() const { return easing_; }
    ASTNodePtr GetBegin() const { return begin_; }
    ASTNodePtr GetEnd() const { return end_; }
    const ASTNodeList& GetKeyframes() const { return keyframes_; }
    int GetLoop() const { return loop_; }
    int GetDelay() const { return delay_; }
    ASTNodePtr GetCallback() const { return callback_; }

private:
    ASTNodePtr target_;
    int duration_;
    std::string easing_;
    ASTNodePtr begin_;
    ASTNodePtr end_;
    ASTNodeList keyframes_;
    int loop_;
    int delay_;
    ASTNodePtr callback_;
};

// INeverAwayBlockNode已移除 - iNeverAway是CJMOD扩展功能，不属于CHTL JS核心语法
// 语法文档第1485行明确说明iNeverAway属于CJMOD扩展

// FunctionDefinitionNode已移除 - function是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

/**
 * @brief 箭头函数节点
 */
class ArrowFunctionNode : public ASTNode {
public:
    ArrowFunctionNode(const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    void AddParameter(const std::string& param) { parameters_.push_back(param); }
    const std::vector<std::string>& GetParameters() const { return parameters_; }
    void SetBody(ASTNodePtr body) { body_ = body; }
    ASTNodePtr GetBody() const { return body_; }

private:
    std::vector<std::string> parameters_;
    ASTNodePtr body_;
};

// ObjectLiteralNode已移除 - 对象字面量是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// ArrayLiteralNode已移除 - 数组字面量是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

/**
 * @brief 方法调用节点
 */
class MethodCallNode : public ASTNode {
public:
    MethodCallNode(ASTNodePtr object, const std::string& method, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    ASTNodePtr GetObject() const { return object_; }
    const std::string& GetMethod() const { return method_; }
    void AddArgument(ASTNodePtr arg) { arguments_.push_back(arg); }
    const ASTNodeList& GetArguments() const { return arguments_; }
    bool IsArrowCall() const { return isArrowCall_; }
    void SetIsArrowCall(bool isArrow) { isArrowCall_ = isArrow; }

private:
    ASTNodePtr object_;
    std::string method_;
    ASTNodeList arguments_;
    bool isArrowCall_;  // 是否使用箭头操作符调用
};

/**
 * @brief 虚对象方法调用节点
 */
class VirtualMethodCallNode : public ASTNode {
public:
    VirtualMethodCallNode(const std::string& objectName, const std::string& method, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    const std::string& GetObjectName() const { return objectName_; }
    const std::string& GetMethod() const { return method_; }
    void AddArgument(ASTNodePtr arg) { arguments_.push_back(arg); }
    const ASTNodeList& GetArguments() const { return arguments_; }
    bool IsVoidStateCall() const { return isVoidStateCall_; }
    void SetIsVoidStateCall(bool isVoidState) { isVoidStateCall_ = isVoidState; }
    const std::string& GetVoidState() const { return voidState_; }
    void SetVoidState(const std::string& state) { voidState_ = state; }

private:
    std::string objectName_;
    std::string method_;
    ASTNodeList arguments_;
    bool isVoidStateCall_;      // 是否为Void<State>调用
    std::string voidState_;     // Void状态名称
};

/**
 * @brief 动画关键帧节点
 */
class AnimationKeyframeNode : public ASTNode {
public:
    AnimationKeyframeNode(double time, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    double GetTime() const { return time_; }
    void AddCSSProperty(const std::string& property, const std::string& value);
    const std::unordered_map<std::string, std::string>& GetCSSProperties() const { return cssProperties_; }

private:
    double time_;  // 动画时间点 (0.0 - 1.0)
    std::unordered_map<std::string, std::string> cssProperties_;
};

// VariableDeclarationNode已移除 - 变量声明是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// AssignmentExpressionNode已移除 - 赋值表达式是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// PropertyAccessNode已移除 - 属性访问是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

/**
 * @brief 注释节点
 */
class CommentNode : public ASTNode {
public:
    CommentNode(const std::string& content, const Core::CHTLJSToken& token);
    void Accept(ASTVisitor& visitor) override;
    ASTNodePtr Clone() const override;
    std::string ToString() const override;
    
    const std::string& GetContent() const { return content_; }

private:
    std::string content_;
};

} // namespace AST
} // namespace CHTLJS
} // namespace CHTL