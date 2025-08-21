#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace chtljs {
namespace ast {

// 前向声明
class CHTLJSVisitor;

// 基础AST节点
class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
    
    virtual void Accept(CHTLJSVisitor* visitor) = 0;
    
    void AddChild(std::shared_ptr<CHTLJSNode> child) {
        m_Children.push_back(child);
    }
    
    const std::vector<std::shared_ptr<CHTLJSNode>>& GetChildren() const {
        return m_Children;
    }
    
    void SetLocation(int line, int column) {
        m_Line = line;
        m_Column = column;
    }

protected:
    std::vector<std::shared_ptr<CHTLJSNode>> m_Children;
    int m_Line = 0;
    int m_Column = 0;
};

// 脚本块节点（根节点）
class ScriptBlockNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
};

// 选择器节点
class SelectorNode : public CHTLJSNode {
public:
    enum SelectorType {
        TAG,        // {{tag}}
        CLASS,      // {{.class}}
        ID,         // {{#id}}
        COMPLEX,    // {{.box button}}
        INDEXED     // {{button[0]}}
    };
    
    SelectorNode(SelectorType type, const std::string& selector)
        : m_Type(type), m_Selector(selector) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    SelectorType GetType() const { return m_Type; }
    const std::string& GetSelector() const { return m_Selector; }
    
    void SetIndex(int index) { m_Index = index; }
    bool HasIndex() const { return m_Index >= 0; }
    int GetIndex() const { return m_Index; }

private:
    SelectorType m_Type;
    std::string m_Selector;
    int m_Index = -1;
};

// 方法调用节点
class MethodCallNode : public CHTLJSNode {
public:
    MethodCallNode(const std::string& methodName)
        : m_MethodName(methodName) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetMethodName() const { return m_MethodName; }
    
    void AddArgument(std::shared_ptr<CHTLJSNode> arg) {
        m_Arguments.push_back(arg);
    }
    
    const std::vector<std::shared_ptr<CHTLJSNode>>& GetArguments() const {
        return m_Arguments;
    }

private:
    std::string m_MethodName;
    std::vector<std::shared_ptr<CHTLJSNode>> m_Arguments;
};

// 监听器节点
class ListenNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void AddEventHandler(const std::string& event, const std::string& handler) {
        m_EventHandlers[event] = handler;
    }
    
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const {
        return m_EventHandlers;
    }

private:
    std::unordered_map<std::string, std::string> m_EventHandlers;
};

// 事件委托节点
class DelegateNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void SetTarget(std::shared_ptr<SelectorNode> target) { m_Target = target; }
    std::shared_ptr<SelectorNode> GetTarget() const { return m_Target; }
    
    void AddTargetSelector(std::shared_ptr<SelectorNode> selector) {
        m_TargetSelectors.push_back(selector);
    }
    
    const std::vector<std::shared_ptr<SelectorNode>>& GetTargetSelectors() const {
        return m_TargetSelectors;
    }
    
    void AddEventHandler(const std::string& event, const std::string& handler) {
        m_EventHandlers[event] = handler;
    }
    
    const std::unordered_map<std::string, std::string>& GetEventHandlers() const {
        return m_EventHandlers;
    }

private:
    std::shared_ptr<SelectorNode> m_Target;
    std::vector<std::shared_ptr<SelectorNode>> m_TargetSelectors;
    std::unordered_map<std::string, std::string> m_EventHandlers;
};

// 动画节点
class AnimateNode : public CHTLJSNode {
public:
    void Accept(CHTLJSVisitor* visitor) override;
    
    void SetTarget(std::shared_ptr<CHTLJSNode> target) { m_Target = target; }
    void SetDuration(int duration) { m_Duration = duration; }
    void SetEasing(const std::string& easing) { m_Easing = easing; }
    
    void SetBeginState(const std::unordered_map<std::string, std::string>& state) {
        m_BeginState = state;
    }
    
    void SetEndState(const std::unordered_map<std::string, std::string>& state) {
        m_EndState = state;
    }
    
    void AddKeyframe(double at, const std::unordered_map<std::string, std::string>& state) {
        m_Keyframes.push_back({at, state});
    }
    
    void SetLoop(int loop) { m_Loop = loop; }
    void SetDirection(const std::string& direction) { m_Direction = direction; }
    void SetDelay(int delay) { m_Delay = delay; }
    void SetCallback(const std::string& callback) { m_Callback = callback; }

private:
    std::shared_ptr<CHTLJSNode> m_Target;
    int m_Duration = 0;
    std::string m_Easing;
    std::unordered_map<std::string, std::string> m_BeginState;
    std::unordered_map<std::string, std::string> m_EndState;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> m_Keyframes;
    int m_Loop = 1;
    std::string m_Direction;
    int m_Delay = 0;
    std::string m_Callback;
};

// 虚对象节点
class VirNode : public CHTLJSNode {
public:
    VirNode(const std::string& name) : m_Name(name) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetName() const { return m_Name; }
    
    void SetTarget(std::shared_ptr<CHTLJSNode> target) { m_Target = target; }
    std::shared_ptr<CHTLJSNode> GetTarget() const { return m_Target; }

private:
    std::string m_Name;
    std::shared_ptr<CHTLJSNode> m_Target;
};

// 虚对象调用节点
class VirCallNode : public CHTLJSNode {
public:
    VirCallNode(const std::string& virName, const std::string& methodName)
        : m_VirName(virName), m_MethodName(methodName) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetVirName() const { return m_VirName; }
    const std::string& GetMethodName() const { return m_MethodName; }

private:
    std::string m_VirName;
    std::string m_MethodName;
};

// 表达式节点（用于参数等）
class ExpressionNode : public CHTLJSNode {
public:
    explicit ExpressionNode(const std::string& expr) : m_Expression(expr) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    const std::string& GetExpression() const { return m_Expression; }

private:
    std::string m_Expression;
};

// CHTL JS访问者接口
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
    virtual void VisitVirCall(VirCallNode* node) = 0;
    virtual void VisitExpression(ExpressionNode* node) = 0;
};

} // namespace ast
} // namespace chtljs