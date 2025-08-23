#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include <stack>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {
namespace CHTLJS {
namespace Core {

/**
 * @brief CHTL JS编译状态类型
 * 严格按照语法文档第1099-1531行定义的CHTL JS扩展语法
 */
enum class CHTLJSCompileState {
    // 基础状态
    INITIAL = 0,                        // 初始状态
    
    // CHTL JS核心语法状态（严格按照语法文档第1099-1531行）
    PARSING_ENHANCED_SELECTOR,          // 解析增强选择器 {{selector}} - 语法文档第1130行
    PARSING_SELECTOR_CONTENT,           // 解析选择器内容 - 语法文档第1147-1158行
    PARSING_VIRTUAL_OBJECT,             // 解析虚对象 vir - 语法文档第1274行
    PARSING_LISTEN_BLOCK,               // 解析监听器块 listen - 语法文档第1184行
    PARSING_DELEGATE_BLOCK,             // 解析事件委托块 delegate - 语法文档第1215行
    PARSING_ANIMATE_BLOCK,              // 解析动画块 animate - 语法文档第1233行
    PARSING_ARROW_FUNCTION,             // 解析箭头函数 () => {} - 语法文档第1202行支持
    PARSING_ARROW_OPERATION,            // 解析箭头操作符 -> - 语法文档第1162行
    
    // 事件相关状态
    PARSING_EVENT_HANDLER,              // 解析事件处理器
    PARSING_EVENT_MAPPING,              // 解析事件映射 - 语法文档第1201-1212行
    PARSING_EVENT_DELEGATION,          // 解析事件委托 - 语法文档第1222-1228行
    
    // 动画相关状态
    PARSING_ANIMATION_CONFIG,           // 解析动画配置 - 语法文档第1239-1270行
    PARSING_ANIMATION_KEYFRAME,         // 解析动画关键帧 - 语法文档第1248-1260行
    PARSING_ANIMATION_CSS,              // 解析动画中的CSS
    
    // 虚对象相关状态
    PARSING_VIRTUAL_ASSIGNMENT,         // 解析虚对象赋值 - 语法文档第1277-1285行
    PARSING_VIRTUAL_CALL,               // 解析虚对象调用 - 语法文档第1284行
    
    // 错误和结束状态
    ERROR_STATE,                        // 错误状态
    COMPLETED_STATE,                    // 完成状态
    
    STATE_COUNT
};

/**
 * @brief CHTL JS Context前瞻/后瞻类型
 */
enum class CHTLJSContextLookDirection {
    FORWARD,                            // 向前看
    BACKWARD,                           // 向后看
    CURRENT,                            // 当前位置
    SURROUNDING                         // 周围上下文
};

/**
 * @brief CHTL JS Context推断类型
 */
enum class CHTLJSContextInferenceType {
    SELECTOR_TYPE_INFERENCE,            // 选择器类型推断
    EVENT_TYPE_INFERENCE,               // 事件类型推断
    ANIMATION_PROPERTY_INFERENCE,       // 动画属性推断
    VIRTUAL_OBJECT_INFERENCE,           // 虚对象推断
    SYNTAX_COMPLETION                   // 语法补全推断
};

/**
 * @brief CHTL JS状态转换条件
 */
struct CHTLJSStateTransitionCondition {
    TokenType triggerToken;             // 触发Token
    std::function<bool(const class CHTLJSStateContext&)> contextCheck;  // 上下文检查函数
    std::string description;            // 转换描述
    bool requiresLookahead;             // 是否需要前瞻
    bool requiresLookbehind;            // 是否需要后瞻
};

/**
 * @brief CHTL JS状态转换规则
 */
struct CHTLJSStateTransitionRule {
    CHTLJSCompileState fromState;       // 源状态
    CHTLJSCompileState toState;         // 目标状态
    CHTLJSStateTransitionCondition condition; // 转换条件
    int priority;                       // 优先级
};

/**
 * @brief CHTL JS状态上下文信息
 * 独立的CHTL JS上下文管理，与CHTL完全分离
 */
class CHTLJSStateContext {
public:
    /**
     * @brief 构造函数
     * @param sourceFile 源文件路径
     * @param tokens CHTL JS Token流
     */
    CHTLJSStateContext(const std::string& sourceFile, const std::vector<CHTLJSToken>& tokens);
    
    /**
     * @brief 析构函数
     */
    ~CHTLJSStateContext() = default;
    
    /**
     * @brief 获取当前Token
     */
    const CHTLJSToken& GetCurrentToken() const;
    
    /**
     * @brief 前瞻Token
     * @param offset 偏移量
     * @return Token指针，如果越界返回nullptr
     */
    const CHTLJSToken* LookAhead(int offset) const;
    
    /**
     * @brief 后瞻Token
     * @param offset 偏移量
     * @return Token指针，如果越界返回nullptr
     */
    const CHTLJSToken* LookBehind(int offset) const;
    
    /**
     * @brief 获取周围上下文
     * @param radius 半径
     * @return 周围Token列表
     */
    std::vector<CHTLJSToken> GetSurroundingContext(int radius) const;
    
    /**
     * @brief 基于上下文进行推断
     * @param inferenceType 推断类型
     * @param direction 查看方向
     * @param depth 查看深度
     * @return 推断结果
     */
    std::string InferFromContext(CHTLJSContextInferenceType inferenceType, 
                                CHTLJSContextLookDirection direction, 
                                int depth = 3) const;
    
    /**
     * @brief 检查CHTL JS语法完整性
     * @param syntaxPattern CHTL JS语法模式
     * @return 是否完整
     */
    bool CheckCHTLJSSyntaxCompleteness(const std::string& syntaxPattern) const;
    
    /**
     * @brief 消除CHTL JS状态歧义
     * @param possibleStates 可能的状态列表
     * @return 确定的状态
     */
    CHTLJSCompileState DisambiguateCHTLJSState(const std::vector<CHTLJSCompileState>& possibleStates) const;
    
    /**
     * @brief 推进Token位置
     */
    void Advance();
    
    /**
     * @brief 设置当前位置
     */
    void SetPosition(size_t position);
    
    /**
     * @brief 获取当前位置
     */
    size_t GetPosition() const { return currentPosition_; }
    
    /**
     * @brief 是否到达末尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 获取源文件路径
     */
    const std::string& GetSourceFile() const { return sourceFile_; }

private:
    std::string sourceFile_;            // 源文件路径
    std::vector<CHTLJSToken> tokens_;   // CHTL JS Token流
    size_t currentPosition_;            // 当前位置
    
    // CHTL JS推断辅助方法
    std::string InferSelectorType(CHTLJSContextLookDirection direction, int depth) const;
    std::string InferEventType(CHTLJSContextLookDirection direction, int depth) const;
    std::string InferAnimationProperty(CHTLJSContextLookDirection direction, int depth) const;
    std::string InferVirtualObject(CHTLJSContextLookDirection direction, int depth) const;
    std::string InferCHTLJSSyntaxCompletion(CHTLJSContextLookDirection direction, int depth) const;
    CHTLJSCompileState DisambiguateByContext(const std::vector<CHTLJSCompileState>& states) const;
};

/**
 * @brief CHTL JS RAII状态机管理器
 * 独立的CHTL JS状态机，与CHTL状态机完全分离
 */
class CHTLJSStateMachine {
public:
    /**
     * @brief 构造函数
     * @param context CHTL JS状态上下文
     */
    explicit CHTLJSStateMachine(std::shared_ptr<CHTLJSStateContext> context);
    
    /**
     * @brief 析构函数 - RAII自动清理
     */
    ~CHTLJSStateMachine();
    
    /**
     * @brief 状态转换
     * @param newState 新状态
     * @param reason 转换原因
     * @return 是否转换成功
     */
    bool TransitionTo(CHTLJSCompileState newState, const std::string& reason = "");
    
    /**
     * @brief 智能状态转换（基于CHTL JS Context推断）
     * @param triggerToken 触发Token
     * @return 是否转换成功
     */
    bool SmartTransition(const CHTLJSToken& triggerToken);
    
    /**
     * @brief 获取当前状态
     */
    CHTLJSCompileState GetCurrentState() const { return currentState_; }
    
    /**
     * @brief 获取状态历史
     */
    const std::vector<CHTLJSCompileState>& GetStateHistory() const { return stateHistory_; }
    
    /**
     * @brief 回滚到上一个状态
     */
    bool RollbackToPreviousState();
    
    /**
     * @brief 检查状态转换是否有效
     */
    bool IsValidTransition(CHTLJSCompileState from, CHTLJSCompileState to) const;
    
    /**
     * @brief 预测下一个可能的状态
     * @return 可能的状态列表
     */
    std::vector<CHTLJSCompileState> PredictNextStates() const;
    
    /**
     * @brief 获取状态名称
     */
    std::string GetStateName(CHTLJSCompileState state) const;
    
    /**
     * @brief 重置状态机
     */
    void Reset();
    
    /**
     * @brief 获取状态机状态报告
     */
    std::string GetStatusReport() const;

private:
    std::shared_ptr<CHTLJSStateContext> context_;   // CHTL JS状态上下文
    CHTLJSCompileState currentState_;               // 当前状态
    std::vector<CHTLJSCompileState> stateHistory_;  // 状态历史
    std::stack<CHTLJSCompileState> stateStack_;     // 状态栈
    
    // CHTL JS状态转换规则表
    std::vector<CHTLJSStateTransitionRule> transitionRules_;
    
    // 初始化方法
    void InitializeCHTLJSTransitionRules();
    void InitializeEnhancedSelectorTransitions();
    void InitializeVirtualObjectTransitions();
    void InitializeListenBlockTransitions();
    void InitializeDelegateBlockTransitions();
    void InitializeAnimateBlockTransitions();
    void InitializeArrowFunctionTransitions();
    
    // 状态转换辅助方法
    bool CheckCHTLJSTransitionCondition(const CHTLJSStateTransitionCondition& condition) const;
    std::vector<CHTLJSStateTransitionRule> FindApplicableCHTLJSRules(const CHTLJSToken& triggerToken) const;
    CHTLJSStateTransitionRule SelectBestCHTLJSRule(const std::vector<CHTLJSStateTransitionRule>& rules) const;
    
    // Context协作方法
    bool RequiresCHTLJSContextInference(CHTLJSCompileState state) const;
    CHTLJSCompileState InferCHTLJSStateFromContext(const std::vector<CHTLJSCompileState>& candidates) const;
};

/**
 * @brief CHTL JS RAII状态守卫
 * 独立的CHTL JS状态守卫，与CHTL状态守卫完全分离
 */
class CHTLJSStateGuard {
public:
    /**
     * @brief 构造函数 - 自动进入CHTL JS状态
     * @param stateMachine CHTL JS状态机引用
     * @param targetState 目标状态
     * @param reason 进入原因
     */
    CHTLJSStateGuard(CHTLJSStateMachine& stateMachine, CHTLJSCompileState targetState, const std::string& reason = "");
    
    /**
     * @brief 析构函数 - RAII自动退出CHTL JS状态
     */
    ~CHTLJSStateGuard();
    
    /**
     * @brief 检查状态转换是否成功
     */
    bool IsValid() const { return isValid_; }
    
    /**
     * @brief 获取进入的状态
     */
    CHTLJSCompileState GetEnteredState() const { return enteredState_; }
    
    /**
     * @brief 手动提交状态（防止析构时回滚）
     */
    void Commit();

private:
    CHTLJSStateMachine& stateMachine_;  // CHTL JS状态机引用
    CHTLJSCompileState previousState_;  // 之前的状态
    CHTLJSCompileState enteredState_;   // 进入的状态
    bool isValid_;                      // 状态转换是否成功
    bool isCommitted_;                  // 是否已提交
};

/**
 * @brief CHTL JS智能状态推断器
 * 专门用于CHTL JS语法的状态推断
 */
class CHTLJSStateInferenceEngine {
public:
    /**
     * @brief 构造函数
     * @param context CHTL JS状态上下文
     */
    explicit CHTLJSStateInferenceEngine(std::shared_ptr<CHTLJSStateContext> context);
    
    /**
     * @brief 推断CHTL JS对象的不同状态
     * @param objectTokens 对象相关的Token序列
     * @return 推断的状态
     */
    CHTLJSCompileState InferCHTLJSObjectState(const std::vector<CHTLJSToken>& objectTokens) const;
    
    /**
     * @brief 解决CHTL JS状态分辨问题
     * @param ambiguousTokens 有歧义的Token序列
     * @return 消歧后的状态
     */
    CHTLJSCompileState ResolveCHTLJSStateAmbiguity(const std::vector<CHTLJSToken>& ambiguousTokens) const;
    
    /**
     * @brief 基于CHTL JS语法模式推断状态
     * @param pattern CHTL JS语法模式
     * @return 推断的状态
     */
    CHTLJSCompileState InferFromCHTLJSSyntaxPattern(const std::string& pattern) const;
    
    /**
     * @brief 推断增强选择器类型
     * @param selectorContent 选择器内容
     * @return 选择器类型
     */
    std::string InferEnhancedSelectorType(const std::string& selectorContent) const;
    
    /**
     * @brief 推断事件类型
     * @param eventContext 事件上下文
     * @return 事件类型
     */
    std::string InferEventType(const std::string& eventContext) const;

private:
    std::shared_ptr<CHTLJSStateContext> context_;
    
    // CHTL JS推断规则
    struct CHTLJSInferenceRule {
        std::string pattern;            // CHTL JS语法模式
        CHTLJSCompileState resultState; // 推断结果状态
        int confidence;                 // 置信度
        std::function<bool(const CHTLJSStateContext&)> validator;  // 验证函数
    };
    
    std::vector<CHTLJSInferenceRule> inferenceRules_;
    
    void InitializeCHTLJSInferenceRules();
};

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL