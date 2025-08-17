#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace chtl {

// CHTL JS AST节点基类
class CHTLJSASTNode {
public:
    CHTLJSASTNode(ASTNodeType type, size_t line = 0, size_t column = 0)
        : nodeType_(type), line_(line), column_(column) {}
    
    virtual ~CHTLJSASTNode() = default;
    
    // 基础信息
    ASTNodeType getType() const { return nodeType_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLJSASTNode> child);
    const std::vector<std::shared_ptr<CHTLJSASTNode>>& getChildren() const { return children_; }
    
    // 属性管理
    void setAttribute(const String& key, const String& value);
    std::optional<String> getAttribute(const String& key) const;
    const StringMap& getAttributes() const { return attributes_; }
    
    // 访问者模式支持
    virtual void accept(class CHTLJSASTVisitor& visitor) = 0;
    
    // 转换为字符串
    virtual String toString() const;

protected:
    ASTNodeType nodeType_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<CHTLJSASTNode>> children_;
    StringMap attributes_;
};

// 增强选择器节点 {{selector}}
class EnhancedSelectorNode : public CHTLJSASTNode {
public:
    enum class SelectorType {
        TAG,            // {{button}}
        CLASS,          // {{.box}}
        ID,             // {{#box}}
        DESCENDANT,     // {{.box button}}
        AUTO_DETECT,    // {{box}} - 自动检测类名/ID
        INDEXED         // {{button[0]}}
    };
    
    EnhancedSelectorNode(const String& selector, SelectorType type, size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::ENHANCED_SELECTOR, line, column), selector_(selector), selectorType_(type) {}
    
    const String& getSelector() const { return selector_; }
    SelectorType getSelectorType() const { return selectorType_; }
    int getIndex() const { return index_; }
    
    void setSelector(const String& selector) { selector_ = selector; }
    void setSelectorType(SelectorType type) { selectorType_ = type; }
    void setIndex(int index) { index_ = index; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    String selector_;
    SelectorType selectorType_;
    int index_ = -1; // 用于索引访问，-1表示无索引
};

// 事件监听器节点
class EventListenerNode : public CHTLJSASTNode {
public:
    EventListenerNode(size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::EVENT_LISTENER, line, column) {}
    
    // 添加事件处理器
    void addEventHandler(const String& eventType, const String& handlerCode);
    const StringMap& getEventHandlers() const { return eventHandlers_; }
    
    // 目标选择器
    void setTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) { targetSelector_ = selector; }
    std::shared_ptr<EnhancedSelectorNode> getTargetSelector() const { return targetSelector_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    StringMap eventHandlers_; // 事件类型 -> 处理器代码
    std::shared_ptr<EnhancedSelectorNode> targetSelector_;
};

// 事件委托节点
class EventDelegateNode : public CHTLJSASTNode {
public:
    EventDelegateNode(size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::EVENT_LISTENER, line, column) {}
    
    // 父元素选择器
    void setParentSelector(std::shared_ptr<EnhancedSelectorNode> selector) { parentSelector_ = selector; }
    std::shared_ptr<EnhancedSelectorNode> getParentSelector() const { return parentSelector_; }
    
    // 目标子元素选择器
    void addTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) { targetSelectors_.push_back(selector); }
    const std::vector<std::shared_ptr<EnhancedSelectorNode>>& getTargetSelectors() const { return targetSelectors_; }
    
    // 事件处理器
    void addEventHandler(const String& eventType, const String& handlerCode);
    const StringMap& getEventHandlers() const { return eventHandlers_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    std::shared_ptr<EnhancedSelectorNode> parentSelector_;
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targetSelectors_;
    StringMap eventHandlers_;
};

// 动画节点
class AnimationNode : public CHTLJSASTNode {
public:
    struct KeyFrame {
        double at;          // 时间点 (0.0-1.0)
        StringMap styles;   // CSS样式
    };
    
    AnimationNode(size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::ANIMATION, line, column) {}
    
    // 动画目标
    void setTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) { targetSelector_ = selector; }
    void addTargetSelector(std::shared_ptr<EnhancedSelectorNode> selector) { targetSelectors_.push_back(selector); }
    std::shared_ptr<EnhancedSelectorNode> getTargetSelector() const { return targetSelector_; }
    const std::vector<std::shared_ptr<EnhancedSelectorNode>>& getTargetSelectors() const { return targetSelectors_; }
    
    // 动画属性
    void setDuration(int duration) { duration_ = duration; }
    void setEasing(const String& easing) { easing_ = easing; }
    void setLoop(int loop) { loop_ = loop; }
    void setDirection(const String& direction) { direction_ = direction; }
    void setDelay(int delay) { delay_ = delay; }
    void setCallback(const String& callback) { callback_ = callback; }
    
    int getDuration() const { return duration_; }
    const String& getEasing() const { return easing_; }
    int getLoop() const { return loop_; }
    const String& getDirection() const { return direction_; }
    int getDelay() const { return delay_; }
    const String& getCallback() const { return callback_; }
    
    // 关键帧
    void setBeginStyles(const StringMap& styles) { beginStyles_ = styles; }
    void setEndStyles(const StringMap& styles) { endStyles_ = styles; }
    void addKeyFrame(const KeyFrame& keyFrame) { keyFrames_.push_back(keyFrame); }
    
    const StringMap& getBeginStyles() const { return beginStyles_; }
    const StringMap& getEndStyles() const { return endStyles_; }
    const std::vector<KeyFrame>& getKeyFrames() const { return keyFrames_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    std::shared_ptr<EnhancedSelectorNode> targetSelector_;
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targetSelectors_;
    
    int duration_ = 1000;           // 持续时间(ms)
    String easing_ = "ease";        // 缓动函数
    int loop_ = 1;                  // 循环次数，-1表示无限循环
    String direction_ = "normal";   // 播放方向
    int delay_ = 0;                 // 延迟(ms)
    String callback_;               // 回调函数
    
    StringMap beginStyles_;         // 起始样式
    StringMap endStyles_;           // 结束样式
    std::vector<KeyFrame> keyFrames_; // 关键帧
};

// 虚对象节点 (vir语法)
class VirtualObjectNode : public CHTLJSASTNode {
public:
    struct VirtualMethod {
        String name;
        String parameters;
        String body;
        String returnType;
    };
    
    VirtualObjectNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::VIRTUAL_OBJECT, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 虚拟方法管理
    void addMethod(const VirtualMethod& method) { methods_.push_back(method); }
    const std::vector<VirtualMethod>& getMethods() const { return methods_; }
    
    // 关联的CHTL JS函数类型
    enum class FunctionType {
        LISTEN,         // listen函数
        ANIMATE,        // animate函数
        I_NEVER_AWAY,   // iNeverAway函数
        CUSTOM          // 自定义函数
    };
    
    void setFunctionType(FunctionType type) { functionType_ = type; }
    FunctionType getFunctionType() const { return functionType_; }
    
    void setFunctionCall(const String& call) { functionCall_ = call; }
    const String& getFunctionCall() const { return functionCall_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    String name_;
    std::vector<VirtualMethod> methods_;
    FunctionType functionType_ = FunctionType::CUSTOM;
    String functionCall_; // 原始函数调用
};

// 链式调用节点 (->语法)
class ChainCallNode : public CHTLJSASTNode {
public:
    ChainCallNode(size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::JS_EXPRESSION, line, column) {}
    
    // 调用链
    void addCall(const String& methodName, const StringList& arguments = {});
    
    struct MethodCall {
        String methodName;
        StringList arguments;
    };
    
    const std::vector<MethodCall>& getCallChain() const { return callChain_; }
    
    // 起始对象
    void setBaseObject(std::shared_ptr<EnhancedSelectorNode> selector) { baseSelector_ = selector; }
    void setBaseObject(std::shared_ptr<VirtualObjectNode> virtualObj) { baseVirtualObject_ = virtualObj; }
    
    std::shared_ptr<EnhancedSelectorNode> getBaseSelector() const { return baseSelector_; }
    std::shared_ptr<VirtualObjectNode> getBaseVirtualObject() const { return baseVirtualObject_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    std::vector<MethodCall> callChain_;
    std::shared_ptr<EnhancedSelectorNode> baseSelector_;
    std::shared_ptr<VirtualObjectNode> baseVirtualObject_;
};

// 变量使用节点 (模板变量和自定义变量的使用)
class VariableUseNode : public CHTLJSASTNode {
public:
    enum class VariableType {
        TEMPLATE_VAR,   // 模板变量组
        CUSTOM_VAR      // 自定义变量组
    };
    
    VariableUseNode(const String& groupName, const String& varName, VariableType type, size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::JS_EXPRESSION, line, column), 
          groupName_(groupName), varName_(varName), variableType_(type) {}
    
    const String& getGroupName() const { return groupName_; }
    const String& getVarName() const { return varName_; }
    VariableType getVariableType() const { return variableType_; }
    
    // 特例化参数 (变量组特例化)
    void setSpecializationValue(const String& value) { specializationValue_ = value; }
    const String& getSpecializationValue() const { return specializationValue_; }
    
    // 命名空间
    void setNamespace(const String& ns) { namespace_ = ns; }
    const String& getNamespace() const { return namespace_; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    String groupName_;
    String varName_;
    VariableType variableType_;
    String specializationValue_; // 特例化值
    String namespace_;           // 命名空间
};

// 原始嵌入节点 (在CHTL JS中的原始嵌入)
class JSOriginNode : public CHTLJSASTNode {
public:
    enum class OriginType {
        HTML,       // @Html
        STYLE,      // @Style
        JAVASCRIPT, // @JavaScript
        CUSTOM      // 自定义类型
    };
    
    JSOriginNode(OriginType type, const String& content, size_t line = 0, size_t column = 0)
        : CHTLJSASTNode(ASTNodeType::JS_EXPRESSION, line, column), originType_(type), content_(content) {}
    
    OriginType getOriginType() const { return originType_; }
    const String& getContent() const { return content_; }
    const String& getName() const { return name_; }
    const String& getCustomTypeName() const { return customTypeName_; }
    
    void setContent(const String& content) { content_ = content; }
    void setName(const String& name) { name_ = name; }
    void setCustomTypeName(const String& typeName) { customTypeName_ = typeName; }
    
    void accept(class CHTLJSASTVisitor& visitor) override;

private:
    OriginType originType_;
    String content_;
    String name_;           // 带名原始嵌入的名称
    String customTypeName_; // 自定义类型名称
};

// 访问者模式接口
class CHTLJSASTVisitor {
public:
    virtual ~CHTLJSASTVisitor() = default;
    
    virtual void visit(EnhancedSelectorNode& node) = 0;
    virtual void visit(EventListenerNode& node) = 0;
    virtual void visit(EventDelegateNode& node) = 0;
    virtual void visit(AnimationNode& node) = 0;
    virtual void visit(VirtualObjectNode& node) = 0;
    virtual void visit(ChainCallNode& node) = 0;
    virtual void visit(VariableUseNode& node) = 0;
    virtual void visit(JSOriginNode& node) = 0;
};

} // namespace chtl