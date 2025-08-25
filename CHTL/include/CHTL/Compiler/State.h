#pragma once
#include <memory>
#include <stack>
#include <string>

namespace CHTL {
namespace Compiler {

// CHTL编译状态
enum class CompileState {
    // 顶层状态
    GLOBAL,               // 全局作用域
    
    // 声明状态
    IN_TEMPLATE,          // 在[Template]块中
    IN_CUSTOM,            // 在[Custom]块中
    IN_ORIGIN,            // 在[Origin]块中
    IN_CONFIGURATION,     // 在[Configuration]块中
    IN_NAMESPACE,         // 在[Namespace]块中
    IN_IMPORT,            // 在[Import]语句中
    
    // 元素状态
    IN_HTML_ELEMENT,      // 在HTML元素中
    IN_TEXT_NODE,         // 在text{}块中
    IN_STYLE_BLOCK,       // 在style{}块中（局部样式）
    IN_SCRIPT_BLOCK,      // 在script{}块中（局部脚本）
    
    // 样式相关状态
    IN_STYLE_SELECTOR,    // 在样式选择器中（.class, #id等）
    IN_STYLE_PROPERTIES,  // 在样式属性中
    
    // 特例化状态
    IN_SPECIALIZATION,    // 在特例化块中
    IN_DELETE_STATEMENT,  // 在delete语句中
    IN_INSERT_STATEMENT,  // 在insert语句中
    
    // 表达式状态
    IN_VAR_EXPRESSION,    // 在变量表达式中 VarName(...)
    IN_FROM_EXPRESSION,   // 在from表达式中
    
    // 配置相关状态
    IN_CONFIG_NAME,       // 在[Name]配置块中
    IN_CONFIG_ORIGINTYPE, // 在[OriginType]配置块中
    IN_INFO_BLOCK,        // 在[Info]块中
    IN_EXPORT_BLOCK,      // 在[Export]块中
};

// 状态上下文信息
struct StateContext {
    CompileState state;
    std::string contextName;      // 当前上下文名称（如元素名、样式组名等）
    std::string namespacePath;    // 当前命名空间路径
    size_t depth;                 // 嵌套深度
    size_t line;                  // 进入状态时的行号
    size_t column;                // 进入状态时的列号
    
    StateContext(CompileState s, const std::string& name = "", 
                 const std::string& ns = "", size_t d = 0,
                 size_t l = 0, size_t c = 0)
        : state(s), contextName(name), namespacePath(ns), 
          depth(d), line(l), column(c) {}
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

// 状态管理器
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
    
    // 检查是否在某类状态中
    bool IsInDeclaration() const;     // 是否在声明块中
    bool IsInElement() const;         // 是否在元素中
    bool IsInStyleContext() const;    // 是否在样式上下文中
    bool IsInScriptContext() const;   // 是否在脚本上下文中
    
    // 获取当前命名空间路径
    std::string GetCurrentNamespace() const;
    
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

} // namespace Compiler
} // namespace CHTL