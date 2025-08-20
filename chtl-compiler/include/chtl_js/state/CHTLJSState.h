#pragma once

#include <stack>
#include <string>
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS 编译器状态
 * 表示CHTL JS编译器在解析过程中的不同状态
 * 注意：完全独立于CHTL的状态系统
 */
enum class CHTLJSCompilerState {
    // 初始状态
    INITIAL,
    
    // 顶层状态（在局部script块内）
    IN_SCRIPT_BLOCK,
    
    // 增强选择器状态
    IN_ENHANCED_SELECTOR,      // 在 {{ }} 内
    IN_SELECTOR_CLASS,         // 在 .class 选择器
    IN_SELECTOR_ID,            // 在 #id 选择器
    IN_SELECTOR_INDEX,         // 在 [index] 选择器
    IN_SELECTOR_CHAIN,         // 在选择器链中
    
    // 虚对象状态
    IN_VIRTUAL_OBJECT_DECLARATION,  // vir 声明
    IN_VIRTUAL_OBJECT_INIT,         // vir 初始化
    
    // 函数调用状态
    IN_FUNCTION_CALL,          // 在函数调用中
    IN_ARROW_CHAIN,            // 在 -> 调用链中
    IN_LISTEN_CALL,            // 在 listen() 调用中
    IN_DELEGATE_CALL,          // 在 delegate() 调用中
    IN_ANIMATE_CALL,           // 在 animate() 调用中
    IN_INEVERAWAY_CALL,        // 在 iNeverAway() 调用中
    IN_PRINTMYLOVE_CALL,       // 在 printMylove() 调用中
    
    // 对象字面量状态
    IN_OBJECT_LITERAL,         // 在 {} 对象内
    IN_OBJECT_KEY,             // 在对象键位置
    IN_OBJECT_VALUE,           // 在对象值位置
    
    // 状态标记（用于iNeverAway）
    IN_STATE_MARKER,           // 在 <State> 标记内
    
    // 特殊状态
    IN_STRING_LITERAL,         // 在字符串内
    IN_COMMENT,                // 在注释内
    ERROR                      // 错误状态
};

/**
 * 状态信息
 */
struct CHTLJSStateInfo {
    CHTLJSCompilerState state;
    std::string context;
    size_t line;
    size_t column;
    
    CHTLJSStateInfo(CHTLJSCompilerState s, const std::string& ctx = "",
                    size_t l = 0, size_t c = 0)
        : state(s), context(ctx), line(l), column(c) {}
};

/**
 * CHTL JS 状态机
 * 管理CHTL JS编译器的状态转换
 */
class CHTLJSStateMachine {
public:
    CHTLJSStateMachine();
    ~CHTLJSStateMachine();
    
    // 状态管理
    void PushState(CHTLJSCompilerState state, const std::string& context = "");
    void PopState();
    CHTLJSCompilerState GetCurrentState() const;
    const CHTLJSStateInfo& GetCurrentStateInfo() const;
    
    // 状态查询
    bool IsInState(CHTLJSCompilerState state) const;
    bool IsInSelectorContext() const;
    bool IsInVirtualObjectContext() const;
    bool IsInFunctionContext() const;
    bool IsInCHTLJSFunctionCall() const;
    
    // 特殊状态查询
    bool IsInEnhancedSelector() const;
    bool IsInArrowChain() const;
    bool IsInObjectLiteral() const;
    
    // 状态验证
    bool CanTransitionTo(CHTLJSCompilerState newState) const;
    bool IsValidStateSequence() const;
    
    // 错误处理
    void SetError(const std::string& message);
    bool HasError() const;
    std::string GetError() const;
    
    // 重置
    void Reset();
    
    // 调试
    std::string GetStateStackTrace() const;
    void DumpStateStack() const;
    
private:
    std::stack<CHTLJSStateInfo> m_StateStack;
    std::string m_ErrorMessage;
    
    // 验证状态转换
    bool IsValidTransition(CHTLJSCompilerState from, CHTLJSCompilerState to) const;
};

/**
 * RAII 状态管理器
 * 自动管理CHTL JS状态的进入和退出
 */
class CHTLJSStateGuard {
public:
    CHTLJSStateGuard(CHTLJSStateMachine& machine, CHTLJSCompilerState state,
                     const std::string& context = "")
        : m_Machine(machine)
        , m_Pushed(true) {
        m_Machine.PushState(state, context);
    }
    
    ~CHTLJSStateGuard() {
        if (m_Pushed) {
            m_Machine.PopState();
        }
    }
    
    // 禁止拷贝
    CHTLJSStateGuard(const CHTLJSStateGuard&) = delete;
    CHTLJSStateGuard& operator=(const CHTLJSStateGuard&) = delete;
    
    // 允许移动
    CHTLJSStateGuard(CHTLJSStateGuard&& other) noexcept
        : m_Machine(other.m_Machine)
        , m_Pushed(other.m_Pushed) {
        other.m_Pushed = false;
    }
    
    void Dismiss() {
        m_Pushed = false;
    }
    
private:
    CHTLJSStateMachine& m_Machine;
    bool m_Pushed;
};

} // namespace chtljs
} // namespace chtl