#pragma once

#include <string>
#include <stack>
#include <unordered_map>
#include <memory>
#include <functional>
#include "CHTL/Core/CHTLToken.h"

namespace CHTL {
namespace Core {

/**
 * @brief CHTL编译状态枚举
 */
enum class CompileState {
    INITIAL,                // 初始状态
    PARSING_TEMPLATE,       // 解析模板
    PARSING_CUSTOM,         // 解析自定义
    PARSING_ORIGIN,         // 解析原始嵌入
    PARSING_IMPORT,         // 解析导入
    PARSING_NAMESPACE,      // 解析命名空间
    PARSING_CONFIGURATION,  // 解析配置
    PARSING_HTML_ELEMENT,   // 解析HTML元素
    PARSING_ATTRIBUTES,     // 解析属性
    PARSING_TEXT_NODE,      // 解析文本节点
    PARSING_STYLE_BLOCK,    // 解析样式块
    PARSING_SCRIPT_BLOCK,   // 解析脚本块
    PARSING_CSS_SELECTOR,   // 解析CSS选择器
    PARSING_CSS_PROPERTIES, // 解析CSS属性

    PARSING_INHERITANCE,    // 解析继承
    PARSING_DELETION,       // 解析删除操作
    PARSING_INSERTION,      // 解析插入操作
    PARSING_INDEX_ACCESS,   // 解析索引访问
    PARSING_CONSTRAINTS,    // 解析约束
    PARSING_COMMENT,        // 解析注释
    ERROR_STATE,            // 错误状态
    FINISHED                // 完成状态
};

/**
 * @brief 状态转换信息结构体
 */
struct StateTransition {
    CompileState fromState;         // 源状态
    CompileState toState;           // 目标状态
    TokenType triggerToken;         // 触发Token类型
    std::string condition;          // 转换条件
    std::function<bool()> validator; // 转换验证器
    
    StateTransition(CompileState from, CompileState to, TokenType trigger, 
                   const std::string& cond = "", std::function<bool()> val = nullptr)
        : fromState(from), toState(to), triggerToken(trigger), condition(cond), validator(val) {}
};

/**
 * @brief 状态上下文信息结构体
 */
struct StateContext {
    std::string elementType;        // 当前元素类型
    std::string namespaceName;      // 当前命名空间
    std::string templateName;       // 当前模板名称
    std::string customName;         // 当前自定义名称
    std::unordered_map<std::string, std::string> variables;  // 局部变量
    std::vector<std::string> attributeNames;    // 属性名列表
    int nestingLevel;               // 嵌套级别
    bool inLocalStyleBlock;         // 是否在局部样式块中
    bool inLocalScriptBlock;        // 是否在局部脚本块中
    bool inGlobalScope;             // 是否在全局作用域
    
    StateContext() : nestingLevel(0), inLocalStyleBlock(false), 
                    inLocalScriptBlock(false), inGlobalScope(true) {}
};

/**
 * @brief CHTL状态管理器类
 * 
 * 实现RAII自动化管理模式的状态机，用于跟踪CHTL编译过程中的状态转换
 */
class CHTLState {
public:
    /**
     * @brief 构造函数
     */
    CHTLState();
    
    /**
     * @brief 析构函数
     */
    ~CHTLState() = default;
    
    /**
     * @brief 获取当前状态
     * @return 当前编译状态
     */
    CompileState GetCurrentState() const { return currentState_; }
    
    /**
     * @brief 获取前一个状态
     * @return 前一个编译状态
     */
    CompileState GetPreviousState() const;
    
    /**
     * @brief 转换到新状态
     * @param newState 新状态
     * @param token 触发状态转换的Token
     * @return 转换是否成功
     */
    bool TransitionTo(CompileState newState, const CHTLToken& token = CHTLToken());
    
    /**
     * @brief 检查是否可以转换到指定状态
     * @param targetState 目标状态
     * @param token 触发Token
     * @return 是否可以转换
     */
    bool CanTransitionTo(CompileState targetState, const CHTLToken& token = CHTLToken()) const;
    
    /**
     * @brief 推入状态（用于嵌套结构）
     * @param state 要推入的状态
     * @param context 状态上下文
     */
    void PushState(CompileState state, const StateContext& context = StateContext());
    
    /**
     * @brief 弹出状态
     * @return 弹出的状态，如果栈为空返回INITIAL
     */
    CompileState PopState();
    
    /**
     * @brief 获取当前状态上下文
     * @return 当前状态上下文的引用
     */
    StateContext& GetCurrentContext() { return currentContext_; }
    
    /**
     * @brief 获取当前状态上下文（只读）
     * @return 当前状态上下文的常量引用
     */
    const StateContext& GetCurrentContext() const { return currentContext_; }
    
    /**
     * @brief 设置状态上下文
     * @param context 新的状态上下文
     */
    void SetCurrentContext(const StateContext& context) { currentContext_ = context; }
    
    /**
     * @brief 检查是否在指定状态
     * @param state 要检查的状态
     * @return 是否在指定状态
     */
    bool IsInState(CompileState state) const { return currentState_ == state; }
    
    /**
     * @brief 检查是否在任意指定状态中
     * @param states 状态列表
     * @return 是否在任意指定状态中
     */
    bool IsInAnyState(const std::vector<CompileState>& states) const;
    
    /**
     * @brief 获取状态栈深度
     * @return 状态栈深度
     */
    size_t GetStackDepth() const { return stateStack_.size(); }
    
    /**
     * @brief 重置状态机
     */
    void Reset();
    
    /**
     * @brief 获取状态名称
     * @param state 状态
     * @return 状态名称
     */
    static std::string GetStateName(CompileState state);
    
    /**
     * @brief 检查状态是否为解析状态
     * @param state 状态
     * @return 是否为解析状态
     */
    static bool IsParsingState(CompileState state);
    
    /**
     * @brief 检查状态是否为错误状态
     * @param state 状态
     * @return 是否为错误状态
     */
    static bool IsErrorState(CompileState state);
    
    /**
     * @brief 检查状态是否为终止状态
     * @param state 状态
     * @return 是否为终止状态
     */
    static bool IsFinalState(CompileState state);
    
    /**
     * @brief 添加状态转换规则
     * @param transition 转换规则
     */
    void AddTransition(const StateTransition& transition);
    
    /**
     * @brief 移除状态转换规则
     * @param fromState 源状态
     * @param toState 目标状态
     * @param triggerToken 触发Token类型
     */
    void RemoveTransition(CompileState fromState, CompileState toState, TokenType triggerToken);
    
    /**
     * @brief 获取状态转换历史
     * @return 状态转换历史列表
     */
    const std::vector<CompileState>& GetStateHistory() const { return stateHistory_; }
    
    /**
     * @brief 清空状态转换历史
     */
    void ClearHistory() { stateHistory_.clear(); }
    
    /**
     * @brief 获取状态机统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    CompileState currentState_;                         // 当前状态
    std::stack<CompileState> stateStack_;              // 状态栈
    std::stack<StateContext> contextStack_;            // 上下文栈
    StateContext currentContext_;                       // 当前上下文
    std::vector<StateTransition> transitions_;         // 状态转换规则
    std::vector<CompileState> stateHistory_;           // 状态转换历史
    std::unordered_map<CompileState, int> stateCounters_; // 状态计数器
    
    /**
     * @brief 初始化默认状态转换规则
     */
    void InitializeDefaultTransitions();
    
    /**
     * @brief 查找适用的状态转换规则
     * @param fromState 源状态
     * @param toState 目标状态
     * @param token 触发Token
     * @return 找到的转换规则指针，未找到返回nullptr
     */
    const StateTransition* FindTransition(CompileState fromState, CompileState toState, 
                                         const CHTLToken& token) const;
    
    /**
     * @brief 验证状态转换的合法性
     * @param fromState 源状态
     * @param toState 目标状态
     * @param token 触发Token
     * @return 转换是否合法
     */
    bool ValidateTransition(CompileState fromState, CompileState toState, 
                           const CHTLToken& token) const;
    
    /**
     * @brief 执行状态转换的副作用
     * @param fromState 源状态
     * @param toState 目标状态
     * @param token 触发Token
     */
    void ExecuteTransitionSideEffects(CompileState fromState, CompileState toState, 
                                     const CHTLToken& token);
    
    /**
     * @brief 更新状态上下文
     * @param newState 新状态
     * @param token 触发Token
     */
    void UpdateContext(CompileState newState, const CHTLToken& token);
};

/**
 * @brief RAII状态管理器类
 * 
 * 自动管理状态的进入和退出，确保状态的正确恢复
 */
class StateGuard {
public:
    /**
     * @brief 构造函数
     * @param stateManager 状态管理器
     * @param newState 要进入的新状态
     * @param context 状态上下文
     */
    StateGuard(CHTLState& stateManager, CompileState newState, 
               const StateContext& context = StateContext());
    
    /**
     * @brief 析构函数
     */
    ~StateGuard();
    
    /**
     * @brief 禁用复制构造函数
     */
    StateGuard(const StateGuard&) = delete;
    
    /**
     * @brief 禁用赋值操作符
     */
    StateGuard& operator=(const StateGuard&) = delete;
    
    /**
     * @brief 检查状态切换是否成功
     * @return 状态切换是否成功
     */
    bool IsValid() const { return isValid_; }

private:
    CHTLState& stateManager_;
    bool isValid_;
};

} // namespace Core
} // namespace CHTL