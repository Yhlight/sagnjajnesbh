#pragma once

#include <stack>
#include <string>
#include <memory>

namespace chtl {
namespace compiler {

/**
 * CHTL 编译器状态
 * 表示编译器在解析过程中的不同状态
 */
enum class CHTLCompilerState {
    // 初始状态
    INITIAL,
    
    // 顶层状态
    TOP_LEVEL,
    
    // 元素相关状态
    IN_ELEMENT,
    IN_ELEMENT_BODY,
    IN_ATTRIBUTE,
    IN_TEXT_NODE,
    
    // 模板状态
    IN_TEMPLATE_DECLARATION,
    IN_TEMPLATE_STYLE,
    IN_TEMPLATE_ELEMENT,
    IN_TEMPLATE_VAR,
    
    // 自定义状态
    IN_CUSTOM_DECLARATION,
    IN_CUSTOM_STYLE,
    IN_CUSTOM_ELEMENT,
    IN_CUSTOM_VAR,
    
    // 配置状态
    IN_CONFIGURATION,
    IN_CONFIGURATION_NAME_BLOCK,
    IN_CONFIGURATION_ORIGIN_TYPE_BLOCK,
    
    // 命名空间状态
    IN_NAMESPACE,
    
    // 导入状态
    IN_IMPORT,
    
    // 原始嵌入状态
    IN_ORIGIN_BLOCK,
    
    // 局部样式状态
    IN_LOCAL_STYLE,
    IN_LOCAL_STYLE_SELECTOR,
    IN_LOCAL_STYLE_RULE,
    
    // 局部脚本状态（注意：内容由CHTL JS处理）
    IN_LOCAL_SCRIPT,
    
    // 约束状态
    IN_EXCEPT_CLAUSE,
    
    // 特殊状态
    IN_STRING_LITERAL,
    IN_COMMENT,
    ERROR
};

/**
 * 状态信息
 * 保存状态相关的上下文信息
 */
struct CHTLStateInfo {
    CHTLCompilerState state;
    std::string context;      // 上下文信息（如当前元素名）
    size_t line;             // 进入状态时的行号
    size_t column;           // 进入状态时的列号
    
    CHTLStateInfo(CHTLCompilerState s, const std::string& ctx = "",
                  size_t l = 0, size_t c = 0)
        : state(s), context(ctx), line(l), column(c) {}
};

/**
 * CHTL 状态机
 * 管理编译器的状态转换
 * 使用RAII模式自动管理状态
 */
class CHTLStateMachine {
public:
    CHTLStateMachine();
    ~CHTLStateMachine();
    
    // 状态管理
    void PushState(CHTLCompilerState state, const std::string& context = "");
    void PopState();
    CHTLCompilerState GetCurrentState() const;
    const CHTLStateInfo& GetCurrentStateInfo() const;
    
    // 状态查询
    bool IsInState(CHTLCompilerState state) const;
    bool IsInElementContext() const;
    bool IsInTemplateContext() const;
    bool IsInCustomContext() const;
    bool IsInStyleContext() const;
    bool IsInScriptContext() const;
    
    // 状态验证
    bool CanTransitionTo(CHTLCompilerState newState) const;
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
    std::stack<CHTLStateInfo> m_StateStack;
    std::string m_ErrorMessage;
    
    // 验证状态转换
    bool IsValidTransition(CHTLCompilerState from, CHTLCompilerState to) const;
};

/**
 * RAII 状态管理器
 * 自动管理状态的进入和退出
 */
class CHTLStateGuard {
public:
    CHTLStateGuard(CHTLStateMachine& machine, CHTLCompilerState state,
                   const std::string& context = "")
        : m_Machine(machine)
        , m_Pushed(true) {
        m_Machine.PushState(state, context);
    }
    
    ~CHTLStateGuard() {
        if (m_Pushed) {
            m_Machine.PopState();
        }
    }
    
    // 禁止拷贝
    CHTLStateGuard(const CHTLStateGuard&) = delete;
    CHTLStateGuard& operator=(const CHTLStateGuard&) = delete;
    
    // 允许移动
    CHTLStateGuard(CHTLStateGuard&& other) noexcept
        : m_Machine(other.m_Machine)
        , m_Pushed(other.m_Pushed) {
        other.m_Pushed = false;
    }
    
    void Dismiss() {
        m_Pushed = false;
    }
    
private:
    CHTLStateMachine& m_Machine;
    bool m_Pushed;
};

} // namespace compiler
} // namespace chtl