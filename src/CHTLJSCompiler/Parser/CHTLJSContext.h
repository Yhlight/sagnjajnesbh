#pragma once
#include "Token.h"
#include "GlobalMap.h"
#include "State.h"
#include <string>
#include <vector>
#include <memory>
#include <stack>

namespace chtl {

// 前向声明
class Lexer;

// 编译上下文信息
struct ContextInfo {
    std::string file_path;          // 当前文件路径
    size_t line_number;             // 当前行号
    size_t column_number;           // 当前列号
    std::string current_function;   // 当前函数名
    std::string current_namespace;  // 当前命名空间
    bool in_template;               // 是否在模板中
    bool in_custom;                 // 是否在自定义中
    
    ContextInfo(const std::string& path = "", size_t line = 1, size_t col = 1)
        : file_path(path), line_number(line), column_number(col),
          in_template(false), in_custom(false) {}
};

// RAII自动管理的上下文助手
class ContextGuard {
public:
    ContextGuard(class CompilerContext& context, const std::string& scope_name);
    ~ContextGuard();
    
    // 禁用复制和移动
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    ContextGuard(ContextGuard&&) = delete;
    ContextGuard& operator=(ContextGuard&&) = delete;
    
private:
    class CompilerContext& context_;
    bool should_pop_;
};

// 状态守护器 - RAII管理状态
class StateGuard {
public:
    StateGuard(StateManager& state_manager, CompilerState new_state, SyntaxContext context);
    ~StateGuard();
    
    // 禁用复制和移动
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    StateGuard(StateGuard&&) = delete;
    StateGuard& operator=(StateGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    
private:
    StateManager& state_manager_;
    bool should_pop_;
    bool is_valid_;
};

// 编译器上下文管理器
class CompilerContext {
public:
    CompilerContext();
    ~CompilerContext() = default;
    
    // 基本信息管理
    void setCurrentFile(const std::string& file_path);
    std::string getCurrentFile() const;
    
    void setCurrentPosition(size_t line, size_t column);
    std::pair<size_t, size_t> getCurrentPosition() const;
    
    // 作用域管理
    void enterScope(const std::string& scope_name);
    void exitScope();
    std::string getCurrentScope() const;
    
    // 命名空间管理
    void enterNamespace(const std::string& namespace_name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    
    // 模板和自定义状态
    void enterTemplate(const std::string& template_name);
    void exitTemplate();
    bool isInTemplate() const;
    std::string getCurrentTemplate() const;
    
    void enterCustom(const std::string& custom_name);
    void exitCustom();
    bool isInCustom() const;
    std::string getCurrentCustom() const;
    
    // 错误处理和诊断
    struct ErrorInfo {
        std::string message;
        std::string file_path;
        size_t line;
        size_t column;
        std::string context;
        
        ErrorInfo(const std::string& msg, const std::string& file = "",
                  size_t l = 0, size_t c = 0, const std::string& ctx = "")
            : message(msg), file_path(file), line(l), column(c), context(ctx) {}
    };
    
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    std::vector<ErrorInfo> getErrors() const;
    std::vector<ErrorInfo> getWarnings() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // 符号表和状态管理器访问
    GlobalMap& getGlobalMap() { return global_map_; }
    const GlobalMap& getGlobalMap() const { return global_map_; }
    
    StateManager& getStateManager() { return state_manager_; }
    const StateManager& getStateManager() const { return state_manager_; }
    
    // 上下文栈管理
    void pushContext(const ContextInfo& info);
    void popContext();
    ContextInfo getCurrentContextInfo() const;
    
    // 调试和诊断
    std::string getContextDescription() const;
    void printContextStack() const;
    std::string getStatistics() const;
    
    // 重置
    void reset();
    
    // 辅助方法
    std::string formatErrorMessage(const std::string& message) const;
    std::string formatWarningMessage(const std::string& message) const;
    
    // RAII助手创建
    std::unique_ptr<ContextGuard> createContextGuard(const std::string& scope_name);
    std::unique_ptr<StateGuard> createStateGuard(CompilerState state, SyntaxContext context);
    
private:
    GlobalMap global_map_;              // 全局符号表
    StateManager state_manager_;        // 状态管理器
    std::stack<ContextInfo> context_stack_; // 上下文栈
    std::vector<ErrorInfo> errors_;     // 错误列表
    std::vector<ErrorInfo> warnings_;   // 警告列表
    
    // 当前状态信息
    std::string current_file_;
    size_t current_line_;
    size_t current_column_;
    std::stack<std::string> template_stack_;
    std::stack<std::string> custom_stack_;
    
    friend class ContextGuard;
};

// 编译上下文的便利宏
#define CHTL_CONTEXT_GUARD(context, scope_name) \
    auto guard = (context).createContextGuard(scope_name)

#define CHTL_STATE_GUARD(context, state, syntax_context) \
    auto state_guard = (context).createStateGuard(state, syntax_context)

} // namespace chtl