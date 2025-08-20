#pragma once

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "chtl_js/state/CHTLJSState.h"
#include "chtl_js/context/CHTLJSContext.h"
#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace helpers {

/**
 * CHTL JS AST节点状态信息
 * 用于标记和追踪CHTL JS AST节点的编译状态
 * 注意：完全独立于CHTL的状态信息
 */
struct CHTLJSNodeStateInfo {
    // 节点相关状态
    ast::CHTLJSNodeType nodeType;
    compiler::CHTLJSStateType stateType;
    
    // 位置信息
    std::string filename;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    // 语义信息
    std::string identifier;      // 节点标识符
    std::string parentIdentifier; // 父节点标识符
    int depth;                   // 嵌套深度
    
    // CHTL JS特有约束
    bool isInLocalScript;        // 是否在局部脚本中
    bool allowsEnhancedSelector; // 是否允许增强选择器
    bool allowsArrowOperator;    // 是否允许箭头操作符
    bool allowsVirDeclaration;   // 是否允许虚对象声明
    bool allowsCHTLJSFunctions;  // 是否允许CHTL JS函数
    
    // 验证标记
    bool isValidated;            // 是否已验证
    bool hasErrors;              // 是否有错误
    std::vector<std::string> errors; // 错误信息
    
    CHTLJSNodeStateInfo()
        : nodeType(ast::CHTLJSNodeType::UNKNOWN)
        , stateType(compiler::CHTLJSStateType::INITIAL)
        , startLine(0), startColumn(0)
        , endLine(0), endColumn(0)
        , depth(0)
        , isInLocalScript(true)
        , allowsEnhancedSelector(true)
        , allowsArrowOperator(true)
        , allowsVirDeclaration(true)
        , allowsCHTLJSFunctions(true)
        , isValidated(false)
        , hasErrors(false) {}
};

/**
 * CHTL JS状态转换规则
 */
struct CHTLJSStateTransitionRule {
    compiler::CHTLJSStateType fromState;
    compiler::CHTLJSStateType toState;
    std::function<bool(const CHTLJSNodeStateInfo&)> condition;
    std::string description;
};

/**
 * CHTL JS状态上下文协助器
 * 提供CHTL JS特有的状态管理和AST节点标记
 * 注意：完全独立于CHTL的协助器
 */
class CHTLJSStateContextHelper {
public:
    CHTLJSStateContextHelper(compiler::CHTLJSStateMachine& stateMachine,
                            compiler::CHTLJSContext& context);
    ~CHTLJSStateContextHelper();
    
    /**
     * CHTL JS AST节点状态管理
     */
    class CHTLJSNodeStateGuard {
    public:
        CHTLJSNodeStateGuard(CHTLJSStateContextHelper& helper,
                            ast::CHTLJSASTNode* node,
                            compiler::CHTLJSStateType state);
        ~CHTLJSNodeStateGuard();
        
        // 更新节点状态信息
        void UpdateNodeInfo(const std::function<void(CHTLJSNodeStateInfo&)>& updater);
        
        // 获取当前节点状态
        const CHTLJSNodeStateInfo& GetNodeState() const { return m_NodeState; }
        
        // 验证节点
        bool Validate();
        
        // 标记错误
        void MarkError(const std::string& error);
        
        // 禁止拷贝
        CHTLJSNodeStateGuard(const CHTLJSNodeStateGuard&) = delete;
        CHTLJSNodeStateGuard& operator=(const CHTLJSNodeStateGuard&) = delete;
        
        // 允许移动
        CHTLJSNodeStateGuard(CHTLJSNodeStateGuard&&) noexcept;
        
    private:
        CHTLJSStateContextHelper& m_Helper;
        ast::CHTLJSASTNode* m_Node;
        CHTLJSNodeStateInfo m_NodeState;
        bool m_Active;
        
        // 状态守卫
        std::unique_ptr<compiler::CHTLJSStateGuard> m_StateGuard;
    };
    
    /**
     * 创建CHTL JS AST节点状态守卫
     */
    std::unique_ptr<CHTLJSNodeStateGuard> CreateNodeGuard(
        ast::CHTLJSASTNode* node,
        compiler::CHTLJSStateType state);
    
    /**
     * CHTL JS语法约束检查
     */
    bool CheckCHTLJSSyntaxConstraints(const ast::CHTLJSASTNode* node);
    
    /**
     * 获取节点的CHTL JS约束信息
     */
    CHTLJSNodeStateInfo GetNodeConstraints(ast::CHTLJSNodeType nodeType,
                                          compiler::CHTLJSStateType state);
    
    /**
     * 状态转换验证
     */
    bool CanTransition(compiler::CHTLJSStateType from,
                      compiler::CHTLJSStateType to,
                      const CHTLJSNodeStateInfo& nodeInfo);
    
    /**
     * 注册状态转换规则
     */
    void RegisterTransitionRule(const CHTLJSStateTransitionRule& rule);
    
    /**
     * 获取当前上下文的CHTL JS约束
     */
    struct CHTLJSContextConstraints {
        bool allowsEnhancedSelector;
        bool allowsArrowOperator;
        bool allowsVirDeclaration;
        bool allowsCHTLJSFunctions;
        bool allowsJavaScriptCode;
        std::vector<ast::CHTLJSNodeType> allowedNodeTypes;
    };
    
    CHTLJSContextConstraints GetCurrentConstraints() const;
    
    /**
     * 虚对象管理
     */
    void RegisterVirObject(const std::string& name,
                          const std::vector<std::string>& functionKeys);
    bool IsVirObject(const std::string& name) const;
    std::vector<std::string> GetVirFunctionKeys(const std::string& name) const;
    
    /**
     * 增强选择器管理
     */
    void RegisterSelector(const std::string& selector);
    std::vector<std::string> GetUsedSelectors() const;
    
    /**
     * 批量操作支持
     */
    template<typename Func>
    void WithStateContext(compiler::CHTLJSStateType state,
                         const std::string& context,
                         Func&& func) {
        compiler::CHTLJSStateGuard stateGuard(m_StateMachine, state, context);
        func();
    }
    
    /**
     * 错误收集
     */
    void CollectError(const std::string& error, const CHTLJSNodeStateInfo& nodeInfo);
    std::vector<std::string> GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 调试支持
     */
    void EnableDebugMode(bool enable) { m_DebugMode = enable; }
    void DumpNodeState(const ast::CHTLJSASTNode* node) const;
    void DumpStateHistory() const;
    
private:
    compiler::CHTLJSStateMachine& m_StateMachine;
    compiler::CHTLJSContext& m_Context;
    
    // 节点状态映射
    std::unordered_map<const ast::CHTLJSASTNode*, CHTLJSNodeStateInfo> m_NodeStates;
    
    // 状态转换规则
    std::vector<CHTLJSStateTransitionRule> m_TransitionRules;
    
    // 虚对象注册表
    std::unordered_map<std::string, std::vector<std::string>> m_VirObjects;
    
    // 选择器注册表
    std::vector<std::string> m_UsedSelectors;
    
    // 错误收集
    std::vector<std::string> m_Errors;
    
    // 调试模式
    bool m_DebugMode;
    
    // 状态历史（用于调试）
    struct StateHistoryEntry {
        std::string timestamp;
        compiler::CHTLJSStateType state;
        std::string context;
        std::string nodeIdentifier;
    };
    std::vector<StateHistoryEntry> m_StateHistory;
    
    // 初始化默认规则
    void InitializeDefaultRules();
    
    // 约束检查实现
    bool CheckEnhancedSelectorConstraints(const ast::CHTLJSASTNode* node,
                                         const CHTLJSNodeStateInfo& info);
    bool CheckVirConstraints(const ast::CHTLJSASTNode* node,
                           const CHTLJSNodeStateInfo& info);
    bool CheckCHTLJSFunctionConstraints(const ast::CHTLJSASTNode* node,
                                       const CHTLJSNodeStateInfo& info);
    
    // 辅助方法
    void RecordStateChange(const StateHistoryEntry& entry);
    std::string GetTimestamp() const;
};

/**
 * CHTL JS解析器状态协助器
 * 为CHTL JS解析器提供便捷的状态管理
 */
class CHTLJSParserStateHelper {
public:
    CHTLJSParserStateHelper(CHTLJSStateContextHelper& helper);
    
    // 增强选择器解析状态管理
    void EnterEnhancedSelector();
    void ExitEnhancedSelector();
    
    // 虚对象解析状态管理
    void EnterVirDeclaration(const std::string& virName);
    void ExitVirDeclaration();
    
    // CHTL JS函数解析状态管理
    void EnterCHTLJSFunction(const std::string& functionName);
    void ExitCHTLJSFunction();
    
    // JavaScript代码片段管理
    void EnterJSFragment();
    void ExitJSFragment();
    
private:
    CHTLJSStateContextHelper& m_Helper;
};

/**
 * CHTL JS生成器状态协助器
 * 为CHTL JS生成器提供便捷的状态管理
 */
class CHTLJSGeneratorStateHelper {
public:
    CHTLJSGeneratorStateHelper(CHTLJSStateContextHelper& helper);
    
    // 生成上下文管理
    class GenerationGuard {
    public:
        GenerationGuard(CHTLJSGeneratorStateHelper& helper,
                       ast::CHTLJSASTNode* node,
                       const std::string& outputType);
        ~GenerationGuard();
        
        bool ShouldGenerate() const;
        void MarkGenerated();
        
        // 虚对象函数生成
        std::string GenerateVirFunctionName(const std::string& virName,
                                           const std::string& functionKey);
        
    private:
        CHTLJSGeneratorStateHelper& m_Helper;
        ast::CHTLJSASTNode* m_Node;
        std::string m_OutputType;
        bool m_Generated;
    };
    
    std::unique_ptr<GenerationGuard> BeginGeneration(
        ast::CHTLJSASTNode* node,
        const std::string& outputType = "javascript");
    
    // 运行时函数管理
    void RegisterRuntimeFunction(const std::string& name);
    bool IsRuntimeFunctionRegistered(const std::string& name) const;
    std::vector<std::string> GetRuntimeFunctions() const;
    
private:
    CHTLJSStateContextHelper& m_Helper;
    std::unordered_set<std::string> m_RuntimeFunctions;
};

} // namespace helpers
} // namespace chtljs
} // namespace chtl