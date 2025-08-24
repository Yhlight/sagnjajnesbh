#pragma once
#include <memory>
#include <stack>
#include <string>

namespace CHTL {
namespace JSCompiler {

// CHTL JS编译状态
enum class CompileState {
    // 顶层状态
    GLOBAL,                     // 全局作用域
    
    // 表达式状态
    IN_SELECTOR,                // 在{{}}选择器中
    IN_VIR_DECLARATION,         // 在vir声明中
    IN_ARROW_EXPRESSION,        // 在->表达式中
    IN_LISTEN_CALL,             // 在listen调用中
    IN_DELEGATE_CALL,           // 在delegate调用中
    IN_ANIMATE_CALL,            // 在animate调用中
    IN_INEVERAWAY_CALL,         // 在iNeverAway调用中
    
    // 对象字面量状态
    IN_OBJECT_LITERAL,          // 在对象字面量中
    IN_ARRAY_LITERAL,           // 在数组字面量中
    IN_PROPERTY_KEY,            // 在属性键中
    IN_PROPERTY_VALUE,          // 在属性值中
    
    // 函数相关状态
    IN_FUNCTION_PARAMS,         // 在函数参数中
    IN_FUNCTION_BODY,           // 在函数体中
    
    // JS代码状态
    IN_JS_CODE                  // 在普通JS代码中
};

// 状态上下文信息
struct StateContext {
    CompileState state;
    std::string contextName;      // 当前上下文名称
    size_t depth;                 // 嵌套深度
    size_t line;                  // 进入状态时的行号
    size_t column;                // 进入状态时的列号
    
    // CHTL JS特有的上下文信息
    std::string virObjectName;    // 当前虚对象名称
    std::string selectorContent;  // 当前选择器内容
    
    StateContext(CompileState s, const std::string& name = "", 
                 size_t d = 0, size_t l = 0, size_t c = 0)
        : state(s), contextName(name), depth(d), line(l), column(c) {}
};

// RAII状态管理器
class StateGuard {
public:
    StateGuard(class StateManager* manager, CompileState state, 
               const std::string& contextName = "");
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
private:
    class StateManager* manager_;
    bool active_;
};

// CHTL JS状态管理器
class StateManager {
public:
    StateManager();
    ~StateManager();
    
    // 获取当前状态
    CompileState GetCurrentState() const;
    
    // 获取当前状态上下文
    const StateContext& GetCurrentContext() const;
    
    // 检查是否在某个状态中
    bool IsInState(CompileState state) const;
    
    // 特定状态检查
    bool IsInSelector() const;
    bool IsInVirContext() const;
    bool IsInObjectLiteral() const;
    bool IsInFunctionContext() const;
    
    // 获取当前虚对象名称
    std::string GetCurrentVirObject() const;
    
    // 设置当前虚对象名称
    void SetCurrentVirObject(const std::string& name);
    
    // 获取当前选择器内容
    std::string GetCurrentSelector() const;
    
    // 设置当前选择器内容
    void SetCurrentSelector(const std::string& selector);
    
    // 获取状态栈深度
    size_t GetDepth() const;
    
    // 创建RAII状态守卫
    [[nodiscard]] StateGuard EnterState(CompileState state, 
                                        const std::string& contextName = "");
    
    // 状态验证
    bool CanEnterState(CompileState newState) const;
    
    // 调试输出
    void DumpStateStack() const;
    
private:
    friend class StateGuard;
    
    void PushState(CompileState state, const std::string& contextName);
    void PopState();
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace JSCompiler
} // namespace CHTL