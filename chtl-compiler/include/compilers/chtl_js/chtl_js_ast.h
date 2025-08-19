#ifndef CHTL_JS_AST_H
#define CHTL_JS_AST_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {

/**
 * CHTL JS AST节点类型
 */
enum class CHTLJSNodeType {
    PROGRAM,
    ENHANCED_SELECTOR,
    VIRTUAL_OBJECT,
    ARROW_CALL,
    LISTEN_CALL,
    DELEGATE_CALL,
    ANIMATE_CALL,
    REGULAR_JS
};

/**
 * CHTL JS AST节点基类
 */
class CHTLJSASTNode {
public:
    CHTLJSASTNode(CHTLJSNodeType type) : type_(type) {}
    virtual ~CHTLJSASTNode() = default;
    
    CHTLJSNodeType getType() const { return type_; }
    
    void addChild(std::shared_ptr<CHTLJSASTNode> child) {
        children_.push_back(child);
    }
    
    const std::vector<std::shared_ptr<CHTLJSASTNode>>& getChildren() const {
        return children_;
    }
    
    virtual void accept(class CHTLJSASTVisitor* visitor) = 0;
    
protected:
    CHTLJSNodeType type_;
    std::vector<std::shared_ptr<CHTLJSASTNode>> children_;
};

/**
 * 程序节点
 */
class CHTLJSProgramNode : public CHTLJSASTNode {
public:
    CHTLJSProgramNode() : CHTLJSASTNode(CHTLJSNodeType::PROGRAM) {}
    void accept(CHTLJSASTVisitor* visitor) override;
};

/**
 * 增强选择器节点 {{selector}}
 */
class EnhancedSelectorNode : public CHTLJSASTNode {
public:
    EnhancedSelectorNode(const std::string& selector) 
        : CHTLJSASTNode(CHTLJSNodeType::ENHANCED_SELECTOR), selector_(selector) {}
    
    const std::string& getSelector() const { return selector_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string selector_;
};

/**
 * 虚对象节点 vir
 */
class VirtualObjectNode : public CHTLJSASTNode {
public:
    VirtualObjectNode(const std::string& name, const std::string& definition)
        : CHTLJSASTNode(CHTLJSNodeType::VIRTUAL_OBJECT), 
          name_(name), definition_(definition) {}
    
    const std::string& getName() const { return name_; }
    const std::string& getDefinition() const { return definition_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string name_;
    std::string definition_;
};

/**
 * 箭头调用节点 ->
 */
class ArrowCallNode : public CHTLJSASTNode {
public:
    ArrowCallNode(const std::string& object, const std::string& method)
        : CHTLJSASTNode(CHTLJSNodeType::ARROW_CALL), 
          object_(object), method_(method) {}
    
    const std::string& getObject() const { return object_; }
    const std::string& getMethod() const { return method_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string object_;
    std::string method_;
};

/**
 * listen调用节点
 */
class ListenCallNode : public CHTLJSASTNode {
public:
    ListenCallNode(const std::string& target, const std::string& events)
        : CHTLJSASTNode(CHTLJSNodeType::LISTEN_CALL), 
          target_(target), events_(events) {}
    
    const std::string& getTarget() const { return target_; }
    const std::string& getEvents() const { return events_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string target_;
    std::string events_;
};

/**
 * delegate调用节点
 */
class DelegateCallNode : public CHTLJSASTNode {
public:
    DelegateCallNode(const std::string& parent, const std::string& config)
        : CHTLJSASTNode(CHTLJSNodeType::DELEGATE_CALL), 
          parent_(parent), config_(config) {}
    
    const std::string& getParent() const { return parent_; }
    const std::string& getConfig() const { return config_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string parent_;
    std::string config_;
};

/**
 * animate调用节点
 */
class AnimateCallNode : public CHTLJSASTNode {
public:
    AnimateCallNode(const std::string& config)
        : CHTLJSASTNode(CHTLJSNodeType::ANIMATE_CALL), config_(config) {}
    
    const std::string& getConfig() const { return config_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string config_;
};

/**
 * 普通JavaScript代码节点
 */
class RegularJSNode : public CHTLJSASTNode {
public:
    RegularJSNode(const std::string& code)
        : CHTLJSASTNode(CHTLJSNodeType::REGULAR_JS), code_(code) {}
    
    const std::string& getCode() const { return code_; }
    void accept(CHTLJSASTVisitor* visitor) override;
    
private:
    std::string code_;
};

/**
 * CHTL JS AST访问者接口
 */
class CHTLJSASTVisitor {
public:
    virtual ~CHTLJSASTVisitor() = default;
    
    virtual void visitProgram(CHTLJSProgramNode* node) = 0;
    virtual void visitEnhancedSelector(EnhancedSelectorNode* node) = 0;
    virtual void visitVirtualObject(VirtualObjectNode* node) = 0;
    virtual void visitArrowCall(ArrowCallNode* node) = 0;
    virtual void visitListenCall(ListenCallNode* node) = 0;
    virtual void visitDelegateCall(DelegateCallNode* node) = 0;
    virtual void visitAnimateCall(AnimateCallNode* node) = 0;
    virtual void visitRegularJS(RegularJSNode* node) = 0;
};

} // namespace chtl

#endif // CHTL_JS_AST_H