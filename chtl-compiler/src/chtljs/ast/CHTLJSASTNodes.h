#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtljs {
namespace ast {

// 前向声明
class CHTLJSVisitor;

/**
 * CHTL JS AST 节点基类
 * 完全独立的AST系统，不与CHTL共享
 */
class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
    virtual void Accept(CHTLJSVisitor* visitor) = 0;
    
    void SetLocation(size_t line, size_t column) {
        m_Line = line;
        m_Column = column;
    }
    
    size_t GetLine() const { return m_Line; }
    size_t GetColumn() const { return m_Column; }
    
protected:
    size_t m_Line = 0;
    size_t m_Column = 0;
};

/**
 * 脚本块节点
 */
class ScriptBlockNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void AddStatement(std::shared_ptr<CHTLJSNode> stmt) {
        m_Statements.push_back(stmt);
    }
    
    const std::vector<std::shared_ptr<CHTLJSNode>>& GetStatements() const {
        return m_Statements;
    }
    
private:
    std::vector<std::shared_ptr<CHTLJSNode>> m_Statements;
};

/**
 * 选择器节点 - {{selector}}
 */
class SelectorNode : public CHTLJSNode {
public:
    enum SelectorType {
        CLASS,      // .classname
        ID,         // #id
        TAG,        // tagname
        COMPLEX     // 复合选择器
    };
    
    SelectorNode(SelectorType type, const std::string& value)
        : m_Type(type), m_Value(value) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    SelectorType GetType() const { return m_Type; }
    const std::string& GetValue() const { return m_Value; }
    
    void SetIndex(int index) { m_Index = index; }
    int GetIndex() const { return m_Index; }
    bool HasIndex() const { return m_Index >= 0; }
    
private:
    SelectorType m_Type;
    std::string m_Value;
    int m_Index = -1;
};

/**
 * 方法调用节点 - ->method()
 */
class MethodCallNode : public CHTLJSNode {
public:
    MethodCallNode(const std::string& method) : m_Method(method) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetMethod() const { return m_Method; }
    
    void SetTarget(std::shared_ptr<CHTLJSNode> target) { m_Target = target; }
    std::shared_ptr<CHTLJSNode> GetTarget() const { return m_Target; }
    
    void AddArgument(std::shared_ptr<CHTLJSNode> arg) {
        m_Arguments.push_back(arg);
    }
    
    const std::vector<std::shared_ptr<CHTLJSNode>>& GetArguments() const {
        return m_Arguments;
    }
    
private:
    std::string m_Method;
    std::shared_ptr<CHTLJSNode> m_Target;
    std::vector<std::shared_ptr<CHTLJSNode>> m_Arguments;
};

/**
 * listen 节点 - 增强监听器
 */
class ListenNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void SetTarget(std::shared_ptr<SelectorNode> target) { m_Target = target; }
    std::shared_ptr<SelectorNode> GetTarget() const { return m_Target; }
    
    void AddEventHandler(const std::string& event, const std::string& handler) {
        m_EventHandlers[event] = handler;
    }
    
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const {
        return m_EventHandlers;
    }
    
private:
    std::shared_ptr<SelectorNode> m_Target;
    std::unordered_map<std::string, std::string> m_EventHandlers;
};

/**
 * delegate 节点 - 事件委托
 */
class DelegateNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void SetParent(std::shared_ptr<SelectorNode> parent) { m_Parent = parent; }
    std::shared_ptr<SelectorNode> GetParent() const { return m_Parent; }
    
    void AddTarget(std::shared_ptr<SelectorNode> target) {
        m_Targets.push_back(target);
    }
    
    const std::vector<std::shared_ptr<SelectorNode>>& GetTargets() const {
        return m_Targets;
    }
    
    void AddEventHandler(const std::string& event, const std::string& handler) {
        m_EventHandlers[event] = handler;
    }
    
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const {
        return m_EventHandlers;
    }
    
private:
    std::shared_ptr<SelectorNode> m_Parent;
    std::vector<std::shared_ptr<SelectorNode>> m_Targets;
    std::unordered_map<std::string, std::string> m_EventHandlers;
};

/**
 * animate 节点 - 动画
 */
class AnimateNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void SetTarget(std::shared_ptr<CHTLJSNode> target) { m_Target = target; }
    std::shared_ptr<CHTLJSNode> GetTarget() const { return m_Target; }
    
    void SetDuration(int duration) { m_Duration = duration; }
    int GetDuration() const { return m_Duration; }
    
    void SetEasing(const std::string& easing) { m_Easing = easing; }
    const std::string& GetEasing() const { return m_Easing; }
    
    // 其他动画属性...
    
private:
    std::shared_ptr<CHTLJSNode> m_Target;
    int m_Duration = 0;
    std::string m_Easing;
};

/**
 * 虚对象节点 - vir
 */
class VirNode : public CHTLJSNode {
public:
    VirNode(const std::string& name) : m_Name(name) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    void SetValue(std::shared_ptr<CHTLJSNode> value) { m_Value = value; }
    std::shared_ptr<CHTLJSNode> GetValue() const { return m_Value; }
    
private:
    std::string m_Name;
    std::shared_ptr<CHTLJSNode> m_Value;
};

/**
 * CHTL JS 访问者接口
 */
class CHTLJSVisitor {
public:
    virtual ~CHTLJSVisitor() = default;
    
    virtual void VisitScriptBlock(ScriptBlockNode* node) = 0;
    virtual void VisitSelector(SelectorNode* node) = 0;
    virtual void VisitMethodCall(MethodCallNode* node) = 0;
    virtual void VisitListen(ListenNode* node) = 0;
    virtual void VisitDelegate(DelegateNode* node) = 0;
    virtual void VisitAnimate(AnimateNode* node) = 0;
    virtual void VisitVir(VirNode* node) = 0;
};

} // namespace ast
} // namespace chtljs