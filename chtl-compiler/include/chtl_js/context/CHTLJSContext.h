#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <unordered_map>

namespace chtl {
namespace chtljs {

// 前向声明
class VirtualObject;
class EnhancedSelector;
class CHTLJSFunction;

/**
 * CHTL JS 作用域类型
 */
enum class CHTLJSScopeType {
    SCRIPT_BLOCK,         // script块作用域
    VIRTUAL_OBJECT,       // 虚对象作用域
    FUNCTION_CALL,        // 函数调用作用域
    OBJECT_LITERAL,       // 对象字面量作用域
    SELECTOR_CONTEXT      // 选择器上下文作用域
};

/**
 * CHTL JS 作用域信息
 */
struct CHTLJSScope {
    CHTLJSScopeType type;
    std::string name;
    std::unordered_map<std::string, std::string> virtualObjects;  // 虚对象映射
    
    CHTLJSScope(CHTLJSScopeType t, const std::string& n = "")
        : type(t), name(n) {}
};

/**
 * CHTL JS 编译上下文
 * 管理CHTL JS编译过程中的上下文信息
 * 注意：完全独立于CHTL的上下文系统
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();
    
    // 作用域管理
    void PushScope(CHTLJSScopeType type, const std::string& name = "");
    void PopScope();
    CHTLJSScope* GetCurrentScope();
    const CHTLJSScope* GetCurrentScope() const;
    
    // 虚对象管理
    void DefineVirtualObject(const std::string& name, const std::string& functionName);
    bool IsVirtualObjectDefined(const std::string& name) const;
    std::string GetVirtualObjectFunction(const std::string& name) const;
    
    // 增强选择器上下文
    void EnterSelector(const std::string& selector);
    void ExitSelector();
    std::string GetCurrentSelector() const;
    bool IsInSelector() const;
    
    // 函数调用链
    void StartCallChain(const std::string& object);
    void AddMethodToChain(const std::string& method);
    void EndCallChain();
    std::vector<std::string> GetCallChain() const;
    bool IsInCallChain() const;
    
    // 当前函数上下文
    void EnterFunction(const std::string& functionName);
    void ExitFunction();
    std::string GetCurrentFunction() const;
    bool IsInCHTLJSFunction() const;
    
    // 对象字面量上下文
    void EnterObjectLiteral();
    void ExitObjectLiteral();
    void SetCurrentObjectKey(const std::string& key);
    std::string GetCurrentObjectKey() const;
    bool IsInObjectLiteral() const;
    int GetObjectLiteralDepth() const;
    
    // 事件处理函数管理
    void RegisterEventHandler(const std::string& event, const std::string& handler);
    std::string GetEventHandler(const std::string& event) const;
    std::vector<std::string> GetAllEvents() const;
    
    // 动画上下文
    void EnterAnimationContext(const std::string& animId);
    void ExitAnimationContext();
    bool IsInAnimationContext() const;
    std::string GetCurrentAnimationId() const;
    
    // 错误上下文
    void SetCurrentFile(const std::string& filename);
    std::string GetCurrentFile() const;
    void SetCurrentLine(size_t line);
    size_t GetCurrentLine() const;
    void SetCurrentColumn(size_t column);
    size_t GetCurrentColumn() const;
    
    // 清理
    void Clear();
    
    // 调试
    void DumpScopes() const;
    std::string GetContextTrace() const;
    
private:
    // 作用域栈
    std::stack<CHTLJSScope> m_ScopeStack;
    
    // 选择器栈
    std::stack<std::string> m_SelectorStack;
    
    // 函数调用链
    std::vector<std::string> m_CallChain;
    std::string m_CurrentChainObject;
    
    // 函数栈
    std::stack<std::string> m_FunctionStack;
    
    // 对象字面量深度
    int m_ObjectLiteralDepth;
    std::string m_CurrentObjectKey;
    
    // 事件处理器映射
    std::unordered_map<std::string, std::string> m_EventHandlers;
    
    // 动画上下文
    std::stack<std::string> m_AnimationStack;
    
    // 位置信息
    std::string m_CurrentFile;
    size_t m_CurrentLine;
    size_t m_CurrentColumn;
    
    // 辅助方法
    std::string* FindVirtualObject(const std::string& name);
    const std::string* FindVirtualObject(const std::string& name) const;
};

/**
 * RAII 作用域管理器
 */
class CHTLJSScopeGuard {
public:
    CHTLJSScopeGuard(CHTLJSContext& context, CHTLJSScopeType type,
                     const std::string& name = "")
        : m_Context(context)
        , m_Pushed(true) {
        m_Context.PushScope(type, name);
    }
    
    ~CHTLJSScopeGuard() {
        if (m_Pushed) {
            m_Context.PopScope();
        }
    }
    
    // 禁止拷贝
    CHTLJSScopeGuard(const CHTLJSScopeGuard&) = delete;
    CHTLJSScopeGuard& operator=(const CHTLJSScopeGuard&) = delete;
    
    // 允许移动
    CHTLJSScopeGuard(CHTLJSScopeGuard&& other) noexcept
        : m_Context(other.m_Context)
        , m_Pushed(other.m_Pushed) {
        other.m_Pushed = false;
    }
    
    void Dismiss() {
        m_Pushed = false;
    }
    
private:
    CHTLJSContext& m_Context;
    bool m_Pushed;
};

} // namespace chtljs
} // namespace chtl