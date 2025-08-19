#pragma once
#include <string>
#include <stack>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

namespace chtl {

// 编译器状态类型
enum class CompilerState {
    IDLE,                   // 空闲状态
    PARSING_CHTL,          // 解析CHTL语法
    PARSING_CHTL_JS,       // 解析CHTL JS语法
    PARSING_CSS,           // 解析CSS
    PARSING_JAVASCRIPT,    // 解析JavaScript
    PARSING_HTML,          // 解析HTML
    PARSING_TEMPLATE,      // 解析模板
    PARSING_CUSTOM,        // 解析自定义
    PARSING_ORIGIN,        // 解析原始嵌入
    PARSING_IMPORT,        // 解析导入
    PARSING_NAMESPACE,     // 解析命名空间
    PARSING_CONFIGURATION, // 解析配置
    PARSING_STYLE_BLOCK,   // 解析样式块
    PARSING_SCRIPT_BLOCK,  // 解析脚本块
    PARSING_TEXT_BLOCK,    // 解析文本块
    PARSING_COMMENT,       // 解析注释
    ERROR_STATE            // 错误状态
};

// 语法上下文类型
enum class SyntaxContext {
    GLOBAL,                // 全局上下文
    LOCAL_STYLE,          // 局部样式上下文
    LOCAL_SCRIPT,         // 局部脚本上下文
    TEMPLATE_DEFINITION,  // 模板定义上下文
    CUSTOM_DEFINITION,    // 自定义定义上下文
    NAMESPACE_DEFINITION, // 命名空间定义上下文
    IMPORT_STATEMENT,     // 导入语句上下文
    ELEMENT_DEFINITION,   // 元素定义上下文
    ATTRIBUTE_DEFINITION, // 属性定义上下文
    TEXT_CONTENT,         // 文本内容上下文
    COMMENT_CONTENT,      // 注释内容上下文
    ORIGIN_BLOCK,         // 原始嵌入块上下文
    CHTL_JS_EXPRESSION    // CHTL JS表达式上下文
};

// 状态转换信息
struct StateTransition {
    CompilerState from_state;
    CompilerState to_state;
    SyntaxContext context;
    std::string trigger;    // 触发状态转换的条件
    std::string description; // 状态转换描述
    
    StateTransition(CompilerState from, CompilerState to, SyntaxContext ctx,
                   const std::string& trig = "", const std::string& desc = "")
        : from_state(from), to_state(to), context(ctx), trigger(trig), description(desc) {}
};

// 状态管理器
class StateManager {
public:
    StateManager();
    ~StateManager() = default;
    
    // 状态管理
    CompilerState getCurrentState() const;
    SyntaxContext getCurrentContext() const;
    
    bool pushState(CompilerState new_state, SyntaxContext context);
    bool popState();
    
    // 状态转换
    bool canTransition(CompilerState to_state, SyntaxContext context) const;
    bool transition(CompilerState to_state, SyntaxContext context, const std::string& trigger = "");
    
    // 状态查询
    bool isInState(CompilerState state) const;
    bool isInContext(SyntaxContext context) const;
    bool isInGlobalScope() const;
    bool isInLocalScope() const;
    
    // 状态历史
    std::vector<StateTransition> getStateHistory() const;
    void clearHistory();
    
    // 错误处理
    void setErrorState(const std::string& error_message);
    bool isInErrorState() const;
    std::string getLastError() const;
    void clearError();
    
    // 状态约束检查
    bool isValidStateForSyntax(CompilerState state, const std::string& syntax_element) const;
    bool isValidContextForOperation(SyntaxContext context, const std::string& operation) const;
    
    // 调试和诊断
    std::string getStateDescription() const;
    std::string getContextDescription() const;
    void printStateStack() const;
    
    // 重置
    void reset();
    
private:
    struct StateInfo {
        CompilerState state;
        SyntaxContext context;
        std::string entry_trigger;
        
        StateInfo(CompilerState s, SyntaxContext c, const std::string& trigger = "")
            : state(s), context(c), entry_trigger(trigger) {}
    };
    
    std::stack<StateInfo> state_stack_;           // 状态栈
    std::vector<StateTransition> state_history_;  // 状态历史
    std::string last_error_;                      // 最后的错误信息
    
    // 内部辅助方法
    void initializeValidTransitions();
    bool isValidTransition(CompilerState from, CompilerState to, SyntaxContext context) const;
    std::string stateToString(CompilerState state) const;
    std::string contextToString(SyntaxContext context) const;
    
    // 有效状态转换表
    std::unordered_map<CompilerState, std::vector<CompilerState>> valid_transitions_;
};

} // namespace chtl