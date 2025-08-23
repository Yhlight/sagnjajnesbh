#pragma once

#include "CHTL/Core/CHTLToken.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include <stack>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {
namespace Core {

/**
 * @brief CHTL编译状态类型
 * 严格按照语法文档第1-1098行定义的所有语法结构
 */
enum class CHTLCompileState {
    // 基础状态
    INITIAL = 0,                        // 初始状态
    
    // 文档结构解析状态 - 语法文档第41-68行
    PARSING_HTML_ELEMENT,               // 解析HTML元素
    PARSING_TEXT_NODE,                  // 解析文本节点 - 语法文档第10-18行
    PARSING_ATTRIBUTES,                 // 解析属性 - 语法文档第70-84行
    
    // 局部样式块状态 - 语法文档第86-156行
    PARSING_LOCAL_STYLE,                // 解析局部样式块
    PARSING_INLINE_STYLE,               // 解析内联样式 - 语法文档第92-108行
    PARSING_AUTO_CLASS_ID,              // 解析自动化类名/id - 语法文档第110-126行
    PARSING_REFERENCE_SELECTOR,        // 解析&引用选择器 - 语法文档第128-156行
    
    // 模板系统状态 - 语法文档第158-287行
    PARSING_TEMPLATE_STYLE,             // 解析样式组模板 - 语法文档第161-178行
    PARSING_TEMPLATE_ELEMENT,           // 解析元素模板 - 语法文档第180-219行
    PARSING_TEMPLATE_VAR,               // 解析变量组模板 - 语法文档第221-252行
    PARSING_TEMPLATE_INHERITANCE,       // 解析模板继承 - 语法文档第254-287行
    
    // 自定义系统状态 - 语法文档第288-627行
    PARSING_CUSTOM_STYLE,               // 解析自定义样式组 - 语法文档第294-385行
    PARSING_CUSTOM_ELEMENT,             // 解析自定义元素 - 语法文档第387-583行
    PARSING_CUSTOM_VAR,                 // 解析自定义变量组 - 语法文档第585-601行
    PARSING_CUSTOM_SPECIALIZATION,     // 解析特例化操作
    PARSING_CUSTOM_DELETION,            // 解析删除操作 - 语法文档第328-385行
    PARSING_CUSTOM_INSERTION,           // 解析插入操作 - 语法文档第477-517行
    PARSING_CUSTOM_INDEX_ACCESS,        // 解析索引访问 - 语法文档第439-475行
    
    // 原始嵌入状态 - 语法文档第629-697行
    PARSING_ORIGIN_HTML,                // 解析HTML原始嵌入 - 语法文档第634-648行
    PARSING_ORIGIN_STYLE,               // 解析CSS原始嵌入 - 语法文档第650-656行
    PARSING_ORIGIN_JAVASCRIPT,          // 解析JS原始嵌入 - 语法文档第658-664行
    PARSING_ORIGIN_NAMED,               // 解析带名原始嵌入 - 语法文档第667-679行
    PARSING_ORIGIN_CUSTOM_TYPE,         // 解析自定义类型原始嵌入 - 语法文档第681-697行
    
    // 配置组状态 - 语法文档第699-879行
    PARSING_CONFIGURATION,              // 解析配置组 - 语法文档第703-767行
    PARSING_CONFIGURATION_NAMED,       // 解析命名配置组 - 语法文档第770-879行
    PARSING_NAME_CONFIG,                // 解析Name配置块 - 语法文档第715-756行
    PARSING_ORIGIN_TYPE_CONFIG,         // 解析OriginType配置块 - 语法文档第758-767行
    
    // 导入系统状态 - 语法文档第881-955行
    PARSING_IMPORT_HTML,                // 解析HTML导入 - 语法文档第884-885行
    PARSING_IMPORT_STYLE,               // 解析CSS导入 - 语法文档第887-888行
    PARSING_IMPORT_JAVASCRIPT,          // 解析JS导入 - 语法文档第890-891行
    PARSING_IMPORT_CHTL,                // 解析CHTL文件导入 - 语法文档第911-912行
    PARSING_IMPORT_CJMOD,               // 解析CJMOD导入 - 语法文档第914-915行
    PARSING_IMPORT_CONFIG,              // 解析配置导入 - 语法文档第917-921行
    PARSING_IMPORT_TEMPLATE,            // 解析模板导入 - 语法文档第902-909行
    PARSING_IMPORT_CUSTOM,              // 解析自定义导入 - 语法文档第893-901行
    PARSING_IMPORT_ORIGIN,              // 解析原始嵌入导入 - 语法文档第922-926行
    PARSING_IMPORT_BATCH,               // 解析批量导入 - 语法文档第927-952行
    
    // 命名空间状态 - 语法文档第956-1061行
    PARSING_NAMESPACE,                  // 解析命名空间 - 语法文档第962行
    PARSING_NAMESPACE_NESTED,           // 解析嵌套命名空间 - 语法文档第993-1051行
    PARSING_NAMESPACE_USAGE,            // 解析命名空间使用 - 语法文档第987-990行
    PARSING_NAMESPACE_BRACE_OMISSION,   // 解析省略大括号命名空间 - 语法文档第998行
    PARSING_NAMESPACE_SINGLE_RELATION,  // 解析单层关系命名空间 - 语法文档第998行
    PARSING_NAMESPACE_PARALLEL_LEVEL,   // 解析平级命名空间 - 语法文档第998行
    
    // 约束系统状态 - 语法文档第1062-1097行
    PARSING_CONSTRAINT_PRECISE,        // 解析精确约束 - 语法文档第1065-1073行
    PARSING_CONSTRAINT_TYPE,            // 解析类型约束 - 语法文档第1075-1085行
    PARSING_CONSTRAINT_GLOBAL,         // 解析全局约束 - 语法文档第1087-1097行
    
    // 局部脚本状态 - 语法文档第1104-1128行
    PARSING_LOCAL_SCRIPT,               // 解析局部script块
    
    // 语义验证和约束检查状态
    PARSING_SEMANTIC_VALIDATION,        // 语义验证状态
    PARSING_CONSTRAINT_VALIDATION,      // 约束验证状态
    
    // 错误和结束状态
    ERROR_STATE,                        // 错误状态
    COMPLETED_STATE,                    // 完成状态
    
    STATE_COUNT
};

/**
 * @brief Context前瞻/后瞻类型
 * 用于Context向前向后看和推断
 */
enum class ContextLookDirection {
    FORWARD,                            // 向前看
    BACKWARD,                           // 向后看
    CURRENT,                            // 当前位置
    SURROUNDING                         // 周围上下文
};

/**
 * @brief Context推断类型
 * 基于前后文内容进行状态推断
 */
enum class ContextInferenceType {
    SYNTAX_COMPLETION,                  // 语法补全推断
    STATE_DISAMBIGUATION,               // 状态消歧推断
    SCOPE_RESOLUTION,                   // 作用域解析推断
    TYPE_INFERENCE,                     // 类型推断
    NAMESPACE_RESOLUTION,               // 命名空间解析推断
    NAMESPACE_BRACE_INFERENCE,          // 命名空间大括号推断 - 语法文档第998行
    SINGLE_RELATION_INFERENCE,          // 单层关系推断 - 语法文档第998行
    PARALLEL_LEVEL_INFERENCE            // 平级关系推断 - 语法文档第998行
};

/**
 * @brief 状态转换条件
 * 定义状态机转换的触发条件
 */
struct StateTransitionCondition {
    TokenType triggerToken;             // 触发Token
    std::function<bool(const class CHTLStateContext&)> contextCheck;  // 上下文检查函数
    std::string description;            // 转换描述
    bool requiresLookahead;             // 是否需要前瞻
    bool requiresLookbehind;            // 是否需要后瞻
};

/**
 * @brief 状态转换规则
 */
struct StateTransitionRule {
    CHTLCompileState fromState;         // 源状态
    CHTLCompileState toState;           // 目标状态
    StateTransitionCondition condition; // 转换条件
    int priority;                       // 优先级（用于冲突解决）
};

/**
 * @brief CHTL状态上下文信息
 * 支持前方/后方内容获取和推断
 */
class CHTLStateContext {
public:
    /**
     * @brief 构造函数
     * @param sourceFile 源文件路径
     * @param tokens Token流
     */
    CHTLStateContext(const std::string& sourceFile, const std::vector<CHTLToken>& tokens);
    
    /**
     * @brief 析构函数
     */
    ~CHTLStateContext() = default;
    
    /**
     * @brief 获取当前Token
     */
    const CHTLToken& GetCurrentToken() const;
    
    /**
     * @brief 前瞻Token
     * @param offset 偏移量（正数向前，负数向后）
     * @return Token指针，如果越界返回nullptr
     */
    const CHTLToken* LookAhead(int offset) const;
    
    /**
     * @brief 后瞻Token
     * @param offset 偏移量
     * @return Token指针，如果越界返回nullptr
     */
    const CHTLToken* LookBehind(int offset) const;
    
    /**
     * @brief 获取周围上下文
     * @param radius 半径（前后各radius个Token）
     * @return 周围Token列表
     */
    std::vector<CHTLToken> GetSurroundingContext(int radius) const;
    
    /**
     * @brief 基于上下文进行推断
     * @param inferenceType 推断类型
     * @param direction 查看方向
     * @param depth 查看深度
     * @return 推断结果
     */
    std::string InferFromContext(ContextInferenceType inferenceType, 
                                ContextLookDirection direction, 
                                int depth = 3) const;
    
    /**
     * @brief 检查语法完整性
     * @param syntaxPattern 语法模式
     * @return 是否完整
     */
    bool CheckSyntaxCompleteness(const std::string& syntaxPattern) const;
    
    /**
     * @brief 消除状态歧义
     * @param possibleStates 可能的状态列表
     * @return 确定的状态
     */
    CHTLCompileState DisambiguateState(const std::vector<CHTLCompileState>& possibleStates) const;
    
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
    std::vector<CHTLToken> tokens_;     // Token流
    size_t currentPosition_;            // 当前位置
    
    // 推断辅助方法
    std::string InferSyntaxCompletion(ContextLookDirection direction, int depth) const;
    std::string InferScopeResolution(ContextLookDirection direction, int depth) const;
    std::string InferTypeFromContext(ContextLookDirection direction, int depth) const;
    std::string InferNamespaceFromContext(ContextLookDirection direction, int depth) const;
    CHTLCompileState DisambiguateByContext(const std::vector<CHTLCompileState>& states) const;
};

/**
 * @brief RAII状态机管理器
 * 真正的RAII自动化管理模式，与Context协作进行精确解析
 */
class CHTLStateMachine {
public:
    /**
     * @brief 构造函数
     * @param context 状态上下文
     */
    explicit CHTLStateMachine(std::shared_ptr<CHTLStateContext> context);
    
    /**
     * @brief 析构函数 - RAII自动清理
     */
    ~CHTLStateMachine();
    
    /**
     * @brief 状态转换
     * @param newState 新状态
     * @param reason 转换原因
     * @return 是否转换成功
     */
    bool TransitionTo(CHTLCompileState newState, const std::string& reason = "");
    
    /**
     * @brief 智能状态转换（基于Context推断）
     * @param triggerToken 触发Token
     * @return 是否转换成功
     */
    bool SmartTransition(const CHTLToken& triggerToken);
    
    /**
     * @brief 获取当前状态
     */
    CHTLCompileState GetCurrentState() const { return currentState_; }
    
    /**
     * @brief 获取状态历史
     */
    const std::vector<CHTLCompileState>& GetStateHistory() const { return stateHistory_; }
    
    /**
     * @brief 回滚到上一个状态
     */
    bool RollbackToPreviousState();
    
    /**
     * @brief 检查状态转换是否有效
     */
    bool IsValidTransition(CHTLCompileState from, CHTLCompileState to) const;
    
    /**
     * @brief 预测下一个可能的状态
     * @return 可能的状态列表
     */
    std::vector<CHTLCompileState> PredictNextStates() const;
    
    /**
     * @brief 获取状态名称
     */
    std::string GetStateName(CHTLCompileState state) const;
    
    /**
     * @brief 重置状态机
     */
    void Reset();
    
    /**
     * @brief 获取状态机状态报告
     */
    std::string GetStatusReport() const;

private:
    std::shared_ptr<CHTLStateContext> context_;     // 状态上下文
    CHTLCompileState currentState_;                 // 当前状态
    std::vector<CHTLCompileState> stateHistory_;    // 状态历史
    std::stack<CHTLCompileState> stateStack_;       // 状态栈（用于嵌套）
    
    // 状态转换规则表
    std::vector<StateTransitionRule> transitionRules_;
    
    // 初始化方法
    void InitializeTransitionRules();
    void InitializeBasicTransitions();
    void InitializeTemplateTransitions();
    void InitializeCustomTransitions();
    void InitializeOriginTransitions();
    void InitializeConfigTransitions();
    void InitializeImportTransitions();
    void InitializeNamespaceTransitions();
    void InitializeConstraintTransitions();
    
    // 状态转换辅助方法
    bool CheckTransitionCondition(const StateTransitionCondition& condition) const;
    std::vector<StateTransitionRule> FindApplicableRules(const CHTLToken& triggerToken) const;
    StateTransitionRule SelectBestRule(const std::vector<StateTransitionRule>& rules) const;
    
    // Context协作方法
    bool RequiresContextInference(CHTLCompileState state) const;
    CHTLCompileState InferStateFromContext(const std::vector<CHTLCompileState>& candidates) const;
};

/**
 * @brief RAII状态守卫
 * 自动管理状态进入和退出，确保状态一致性
 */
class CHTLStateGuard {
public:
    /**
     * @brief 构造函数 - 自动进入状态
     * @param stateMachine 状态机引用
     * @param targetState 目标状态
     * @param reason 进入原因
     */
    CHTLStateGuard(CHTLStateMachine& stateMachine, CHTLCompileState targetState, const std::string& reason = "");
    
    /**
     * @brief 析构函数 - RAII自动退出状态
     */
    ~CHTLStateGuard();
    
    /**
     * @brief 检查状态转换是否成功
     */
    bool IsValid() const { return isValid_; }
    
    /**
     * @brief 获取进入的状态
     */
    CHTLCompileState GetEnteredState() const { return enteredState_; }
    
    /**
     * @brief 手动提交状态（防止析构时回滚）
     */
    void Commit();

private:
    CHTLStateMachine& stateMachine_;    // 状态机引用
    CHTLCompileState previousState_;    // 之前的状态
    CHTLCompileState enteredState_;     // 进入的状态
    bool isValid_;                      // 状态转换是否成功
    bool isCommitted_;                  // 是否已提交（防止回滚）
};

/**
 * @brief 智能状态推断器
 * 基于Context内容进行复杂的状态推断
 */
class CHTLStateInferenceEngine {
public:
    /**
     * @brief 构造函数
     * @param context 状态上下文
     */
    explicit CHTLStateInferenceEngine(std::shared_ptr<CHTLStateContext> context);
    
    /**
     * @brief 推断物体的不同状态
     * @param objectTokens 物体相关的Token序列
     * @return 推断的状态
     */
    CHTLCompileState InferObjectState(const std::vector<CHTLToken>& objectTokens) const;
    
    /**
     * @brief 解决状态分辨问题
     * @param ambiguousTokens 有歧义的Token序列
     * @return 消歧后的状态
     */
    CHTLCompileState ResolveStateAmbiguity(const std::vector<CHTLToken>& ambiguousTokens) const;
    
    /**
     * @brief 基于语法模式推断状态
     * @param pattern 语法模式
     * @return 推断的状态
     */
    CHTLCompileState InferFromSyntaxPattern(const std::string& pattern) const;
    
    /**
     * @brief 基于嵌套层次推断状态
     * @param nestingLevel 嵌套层次
     * @param parentState 父状态
     * @return 推断的状态
     */
    CHTLCompileState InferFromNesting(int nestingLevel, CHTLCompileState parentState) const;
    
    /**
     * @brief 推断命名空间省略大括号情况 - 语法文档第998行
     * @param namespaceTokens 命名空间相关Token序列
     * @return 推断的命名空间状态
     */
    CHTLCompileState InferNamespaceBraceOmission(const std::vector<CHTLToken>& namespaceTokens) const;
    
    /**
     * @brief 检查是否为单层关系 - 语法文档第998行
     * @param tokens Token序列
     * @return 是否为单层关系
     */
    bool IsSingleRelation(const std::vector<CHTLToken>& tokens) const;
    
    /**
     * @brief 检查是否为平级关系 - 语法文档第998行
     * @param tokens Token序列
     * @return 是否为平级关系
     */
    bool IsParallelLevel(const std::vector<CHTLToken>& tokens) const;

private:
    std::shared_ptr<CHTLStateContext> context_;
    
    // 推断规则
    struct InferenceRule {
        std::string pattern;            // 匹配模式
        CHTLCompileState resultState;   // 推断结果状态
        int confidence;                 // 置信度
        std::function<bool(const CHTLStateContext&)> validator;  // 验证函数
    };
    
    std::vector<InferenceRule> inferenceRules_;
    
    void InitializeInferenceRules();
};

} // namespace Core
} // namespace CHTL