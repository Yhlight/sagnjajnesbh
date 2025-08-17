#pragma once

#include "CHTLCommon.h"
#include <stack>
#include <memory>

namespace CHTL {

// 编译状态枚举
enum class CompilerState {
    IDLE,                   // 空闲状态
    SCANNING,               // 扫描状态
    LEXING,                 // 词法分析状态
    PARSING,                // 语法分析状态
    AST_BUILDING,           // AST构建状态
    SEMANTIC_ANALYSIS,      // 语义分析状态
    CODE_GENERATION,        // 代码生成状态
    ERROR_RECOVERY,         // 错误恢复状态
    COMPLETED               // 完成状态
};

// 解析上下文状态
enum class ParseState {
    ROOT,                   // 根节点
    ELEMENT,                // 元素节点
    ATTRIBUTE,              // 属性
    TEXT_NODE,              // 文本节点
    STYLE_BLOCK,            // 样式块
    SCRIPT_BLOCK,           // 脚本块
    TEMPLATE_DECL,          // 模板声明
    CUSTOM_DECL,            // 自定义声明
    ORIGIN_DECL,            // 原始嵌入声明
    IMPORT_DECL,            // 导入声明
    NAMESPACE_DECL,         // 命名空间声明
    CONFIGURATION_DECL,     // 配置声明
    EXPRESSION,             // 表达式
    STATEMENT,              // 语句
    COMMENT                 // 注释
};

// 作用域类型
enum class ScopeType {
    GLOBAL,                 // 全局作用域
    NAMESPACE,              // 命名空间作用域
    TEMPLATE,               // 模板作用域
    CUSTOM,                 // 自定义作用域
    ELEMENT,                // 元素作用域
    STYLE,                  // 样式作用域
    SCRIPT,                 // 脚本作用域
    FUNCTION,               // 函数作用域
    BLOCK                   // 块作用域
};

// 状态信息
struct StateInfo {
    CompilerState compilerState;
    ParseState parseState;
    ScopeType scopeType;
    String context;         // 上下文描述
    size_t line;           // 行号
    size_t column;         // 列号
    String filename;       // 文件名
    
    StateInfo(CompilerState cs = CompilerState::IDLE,
              ParseState ps = ParseState::ROOT,
              ScopeType st = ScopeType::GLOBAL,
              const String& ctx = "",
              size_t l = 0, size_t c = 0,
              const String& file = "")
        : compilerState(cs), parseState(ps), scopeType(st),
          context(ctx), line(l), column(c), filename(file) {}
};

// RAII状态管理器
class StateGuard {
public:
    StateGuard(class StateMachine& sm, const StateInfo& newState);
    ~StateGuard();
    
    // 禁止拷贝和移动
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    StateGuard(StateGuard&&) = delete;
    StateGuard& operator=(StateGuard&&) = delete;
    
    // 更新当前状态
    void updateState(const StateInfo& newState);
    
    // 获取当前状态
    const StateInfo& getCurrentState() const;
    
private:
    class StateMachine& stateMachine_;
    StateInfo previousState_;
};

/**
 * 状态机管理器
 * 提供RAII自动化管理模式的状态跟踪
 */
class StateMachine {
public:
    StateMachine();
    ~StateMachine() = default;
    
    // 状态管理
    void pushState(const StateInfo& state);
    void popState();
    const StateInfo& getCurrentState() const;
    const StateInfo& getPreviousState() const;
    
    // 状态查询
    bool isInState(CompilerState state) const;
    bool isInParseState(ParseState state) const;
    bool isInScope(ScopeType scope) const;
    
    // 状态更新
    void setCompilerState(CompilerState state);
    void setParseState(ParseState state);
    void setScopeType(ScopeType scope);
    void setContext(const String& context);
    void setPosition(size_t line, size_t column);
    void setFilename(const String& filename);
    
    // 作用域管理
    void enterScope(ScopeType scope, const String& name = "");
    void exitScope();
    String getCurrentScopeName() const;
    std::vector<String> getScopeHierarchy() const;
    
    // 错误处理
    void enterErrorState(const String& errorMessage);
    void exitErrorState();
    bool isInErrorState() const;
    String getLastError() const;
    
    // 状态历史
    std::vector<StateInfo> getStateHistory() const;
    void clearHistory();
    size_t getHistorySize() const;
    
    // 调试功能
    String getCurrentStateString() const;
    String getStateHistoryString() const;
    void dumpState() const;
    
    // 统计信息
    struct Statistics {
        size_t stateChanges = 0;
        size_t scopeEnters = 0;
        size_t scopeExits = 0;
        size_t errorStates = 0;
        double averageStateDepth = 0.0;
    };
    
    Statistics getStatistics() const;
    void resetStatistics();
    
private:
    std::stack<StateInfo> stateStack_;
    std::stack<String> scopeStack_;
    std::vector<StateInfo> stateHistory_;
    Statistics statistics_;
    String lastError_;
    bool inErrorState_;
    
    // 辅助方法
    String stateToString(const StateInfo& state) const;
    void updateStatistics();
    
    friend class StateGuard;
};

// 便利宏定义
#define CHTL_STATE_GUARD(sm, compilerState, parseState, scopeType, context) \
    StateGuard __guard(sm, StateInfo(compilerState, parseState, scopeType, context, __LINE__, 0, __FILE__))

#define CHTL_SCOPE_GUARD(sm, scopeType, name) \
    StateGuard __scope_guard(sm, StateInfo(sm.getCurrentState().compilerState, sm.getCurrentState().parseState, scopeType, name, __LINE__, 0, __FILE__))

#define CHTL_PARSE_GUARD(sm, parseState, context) \
    StateGuard __parse_guard(sm, StateInfo(sm.getCurrentState().compilerState, parseState, sm.getCurrentState().scopeType, context, __LINE__, 0, __FILE__))

// 状态工具函数
namespace StateUtils {
    String compilerStateToString(CompilerState state);
    String parseStateToString(ParseState state);
    String scopeTypeToString(ScopeType scope);
    
    bool isValidStateTransition(CompilerState from, CompilerState to);
    bool isValidParseTransition(ParseState from, ParseState to);
    bool isValidScopeNesting(ScopeType parent, ScopeType child);
}

} // namespace CHTL