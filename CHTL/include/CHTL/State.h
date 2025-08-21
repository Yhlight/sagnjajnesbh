#ifndef CHTL_CHTL_STATE_H
#define CHTL_CHTL_STATE_H

#include <string>
#include <stack>
#include <unordered_map>
#include <memory>
#include <vector>

namespace CHTL {

// 解析状态枚举
enum class ParseState {
    // 顶层状态
    TopLevel,               // 顶层
    InElement,              // 在元素内
    InAttribute,            // 在属性内
    InText,                 // 在文本块内
    InStyle,                // 在样式块内
    InScript,               // 在脚本块内
    
    // 特殊块状态
    InTemplate,             // 在模板块内
    InCustom,               // 在自定义块内
    InOrigin,               // 在原始嵌入块内
    InImport,               // 在导入块内
    InNamespace,            // 在命名空间块内
    InConfiguration,        // 在配置块内
    
    // 样式相关状态
    InStyleSelector,        // 在样式选择器内
    InStyleProperty,        // 在样式属性内
    InStyleValue,           // 在样式值内
    
    // 特殊语法状态
    InSpecialization,       // 在特例化块内
    InInsertBlock,          // 在插入块内
    InDeleteBlock,          // 在删除块内
    InExceptBlock,          // 在约束块内
    
    // 字符串状态
    InString,               // 在字符串内
    InComment,              // 在注释内
    
    // 错误状态
    Error                   // 错误状态
};

// 状态上下文信息
struct StateContext {
    ParseState state;
    std::string contextName;    // 上下文名称（如元素名、模板名等）
    int nestingLevel;           // 嵌套层级
    size_t startLine;          // 开始行号
    size_t startColumn;        // 开始列号
    
    // 额外信息
    std::unordered_map<std::string, std::string> properties;
    
    StateContext(ParseState s = ParseState::TopLevel) 
        : state(s), nestingLevel(0), startLine(0), startColumn(0) {}
};

// 状态机类
class StateMachine {
public:
    StateMachine();
    ~StateMachine();
    
    // 状态转换
    bool TransitionTo(ParseState newState, const std::string& contextName = "");
    bool CanTransitionTo(ParseState newState) const;
    
    // 状态查询
    ParseState GetCurrentState() const;
    StateContext GetCurrentContext() const;
    std::string GetCurrentStateName() const;
    bool IsInState(ParseState state) const;
    
    // 状态栈管理
    void PushState(ParseState state, const std::string& contextName = "");
    bool PopState();
    size_t GetStateStackDepth() const;
    
    // 上下文属性
    void SetContextProperty(const std::string& key, const std::string& value);
    std::string GetContextProperty(const std::string& key) const;
    bool HasContextProperty(const std::string& key) const;
    
    // 状态验证
    bool ValidateStateTransition(ParseState from, ParseState to) const;
    bool IsTerminalState() const;
    bool IsErrorState() const;
    
    // 重置
    void Reset();
    
    // 调试
    void DumpStateStack() const;
    std::string GetStateHistory() const;
    
    // 状态特定查询
    bool IsInGlobalScope() const;
    bool IsInLocalScope() const;
    bool IsInStyleContext() const;
    bool IsInScriptContext() const;
    bool IsInTemplateContext() const;
    bool IsInCustomContext() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // 状态转换规则
    static const std::unordered_map<ParseState, std::vector<ParseState>> transitionRules_;
};

// 状态工具类
class StateUtils {
public:
    static std::string GetStateName(ParseState state);
    static bool IsBlockState(ParseState state);
    static bool IsSpecialBlockState(ParseState state);
    static bool IsStyleRelatedState(ParseState state);
    static bool CanContainElements(ParseState state);
    static bool CanContainText(ParseState state);
    static bool RequiresBraces(ParseState state);
};

} // namespace CHTL

#endif // CHTL_CHTL_STATE_H