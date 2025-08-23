#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

namespace CHTL {
namespace CHTLJS {
namespace Core {

/**
 * @brief CHTL JS编译状态枚举
 */
enum class CompileState {
    INITIAL = 0,                    // 初始状态
    
    // CHTL JS核心语法状态（严格按照语法文档第1099-1531行）
    PARSING_ENHANCED_SELECTOR,      // 解析增强选择器 - 语法文档第1130行
    PARSING_VIRTUAL_OBJECT,         // 解析虚对象 vir - 语法文档第1274行
    PARSING_LISTEN_BLOCK,           // 解析监听器块 listen - 语法文档第1184行
    PARSING_DELEGATE_BLOCK,         // 解析事件委托块 delegate - 语法文档第1215行
    PARSING_ANIMATE_BLOCK,          // 解析动画块 animate - 语法文档第1233行
    PARSING_ARROW_FUNCTION,         // 解析箭头函数 () => {} - 语法文档第1202行支持
    PARSING_EVENT_HANDLER,          // 解析事件处理器
    PARSING_ANIMATION_KEYFRAME,     // 解析动画关键帧
    PARSING_CSS_IN_ANIMATION,       // 解析动画中的CSS
    PARSING_SELECTOR_CONTENT,       // 解析选择器内容
    ERROR_STATE,                    // 错误状态
    
    // 错误的状态已移除：
    // PARSING_I_NEVER_AWAY - iNeverAway是CJMOD扩展，不是CHTL JS核心
    // PARSING_FUNCTION_DEFINITION - function是JavaScript语法
    // PARSING_OBJECT_LITERAL - 对象字面量是JavaScript语法
    // PARSING_ARRAY_LITERAL - 数组字面量是JavaScript语法
    // PARSING_VOID_TYPE - Void是CJMOD扩展
    // PARSING_VOID_STATE - Void<State>是CJMOD扩展
    
    STATE_COUNT
};

/**
 * @brief 状态转换信息
 */
struct StateTransition {
    CompileState fromState;     // 源状态
    CompileState toState;       // 目标状态
    TokenType triggerToken;     // 触发Token类型
    std::string condition;      // 转换条件
    
    StateTransition(CompileState from, CompileState to, TokenType trigger, const std::string& cond = "")
        : fromState(from), toState(to), triggerToken(trigger), condition(cond) {}
};

/**
 * @brief CHTL JS状态上下文
 */
struct StateContext {
    std::string currentVirtualObject;           // 当前虚对象名称
    std::string currentSelector;                // 当前选择器
    std::string currentEventType;               // 当前事件类型
    std::string currentAnimationTarget;         // 当前动画目标
    std::vector<std::string> virtualObjectStack; // 虚对象栈
    std::vector<std::string> selectorStack;     // 选择器栈
    std::unordered_map<std::string, std::string> contextVariables; // 上下文变量
    
    StateContext() = default;
};

/**
 * @brief CHTL JS状态管理器
 * 
 * 使用RAII模式管理CHTL JS编译状态，支持：
 * - 虚对象状态管理
 * - 增强选择器状态跟踪
 * - 事件委托状态管理
 * - 动画解析状态管理
 */
class CHTLJSState {
public:
    /**
     * @brief 构造函数
     */
    CHTLJSState();
    
    /**
     * @brief 析构函数
     */
    ~CHTLJSState() = default;
    
    /**
     * @brief 获取当前状态
     */
    CompileState GetCurrentState() const { return currentState_; }
    
    /**
     * @brief 转换到新状态
     * @param newState 新状态
     * @param context 上下文信息
     * @return 是否成功转换
     */
    bool TransitionTo(CompileState newState, const std::string& context = "");
    
    /**
     * @brief 推入状态（用于嵌套结构）
     * @param state 要推入的状态
     */
    void PushState(CompileState state);
    
    /**
     * @brief 弹出状态
     * @return 弹出的状态
     */
    CompileState PopState();
    
    /**
     * @brief 检查状态转换是否有效
     * @param fromState 源状态
     * @param toState 目标状态
     * @return 是否有效
     */
    bool IsValidTransition(CompileState fromState, CompileState toState) const;
    
    /**
     * @brief 获取状态上下文
     */
    StateContext& GetContext() { return context_; }
    const StateContext& GetContext() const { return context_; }
    
    /**
     * @brief 设置上下文变量
     */
    void SetContextVariable(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取上下文变量
     */
    std::string GetContextVariable(const std::string& key) const;
    
    /**
     * @brief 进入虚对象上下文
     */
    void EnterVirtualObject(const std::string& objectName);
    
    /**
     * @brief 退出虚对象上下文
     */
    void ExitVirtualObject();
    
    /**
     * @brief 进入选择器上下文
     */
    void EnterSelector(const std::string& selector);
    
    /**
     * @brief 退出选择器上下文
     */
    void ExitSelector();
    
    /**
     * @brief 重置状态
     */
    void Reset();
    
    /**
     * @brief 获取状态名称
     */
    static std::string GetStateName(CompileState state);

private:
    /**
     * @brief 初始化默认状态转换
     */
    void InitializeDefaultTransitions();

private:
    CompileState currentState_;                         // 当前状态
    std::stack<CompileState> stateStack_;               // 状态栈
    StateContext context_;                              // 状态上下文
    std::vector<StateTransition> validTransitions_;    // 有效状态转换
};

/**
 * @brief CHTL JS状态保护类（RAII）
 * 
 * 自动管理状态的进入和退出
 */
class CHTLJSStateGuard {
public:
    /**
     * @brief 构造函数
     * @param stateManager 状态管理器
     * @param targetState 目标状态
     */
    CHTLJSStateGuard(CHTLJSState& stateManager, CompileState targetState);
    
    /**
     * @brief 析构函数（自动恢复状态）
     */
    ~CHTLJSStateGuard();
    
    /**
     * @brief 检查状态转换是否成功
     */
    bool IsValid() const { return isValid_; }

private:
    CHTLJSState& stateManager_;     // 状态管理器引用
    CompileState previousState_;    // 之前的状态
    bool isValid_;                  // 状态转换是否成功
};

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL