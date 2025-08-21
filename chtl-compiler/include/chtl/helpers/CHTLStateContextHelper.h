#pragma once

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "chtl/state/CHTLState.h"
#include "chtl/context/CHTLContext.h"
#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace compiler {
namespace helpers {

/**
 * AST节点状态信息
 * 用于标记和追踪AST节点的编译状态
 */
struct ASTNodeStateInfo {
    // 节点相关状态
    ast::ASTNodeType nodeType;
    CHTLCompilerState compilerState;
    CHTLScopeType scopeType;
    
    // 位置信息
    std::string filename;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    // 语义信息
    std::string identifier;      // 节点标识符（如元素名、变量名等）
    std::string parentIdentifier; // 父节点标识符
    int depth;                   // 嵌套深度
    
    // 约束信息
    bool allowsCHTLSyntax;       // 是否允许CHTL语法
    bool allowsVariables;        // 是否允许变量
    bool allowsTemplates;        // 是否允许模板
    bool allowsCustomElements;   // 是否允许自定义元素
    bool allowsOriginEmbedding;  // 是否允许原始嵌入
    
    // 验证标记
    bool isValidated;            // 是否已验证
    bool hasErrors;              // 是否有错误
    std::vector<std::string> errors; // 错误信息
    
    ASTNodeStateInfo()
        : nodeType(ast::ASTNodeType::UNKNOWN)
        , compilerState(CHTLCompilerState::INITIAL)
        , scopeType(CHTLScopeType::GLOBAL)
        , startLine(0), startColumn(0)
        , endLine(0), endColumn(0)
        , depth(0)
        , allowsCHTLSyntax(true)
        , allowsVariables(true)
        , allowsTemplates(true)
        , allowsCustomElements(true)
        , allowsOriginEmbedding(true)
        , isValidated(false)
        , hasErrors(false) {}
};

/**
 * 状态转换规则
 */
struct StateTransitionRule {
    CHTLCompilerState fromState;
    CHTLCompilerState toState;
    std::function<bool(const ASTNodeStateInfo&)> condition;
    std::string description;
};

/**
 * CHTL状态上下文协助器
 * 提供更精确的状态管理和AST节点标记
 */
class CHTLStateContextHelper {
public:
    CHTLStateContextHelper(CHTLStateMachine& stateMachine, CHTLContext& context);
    ~CHTLStateContextHelper();
    
    /**
     * AST节点状态管理
     */
    class ASTNodeStateGuard {
    public:
        ASTNodeStateGuard(CHTLStateContextHelper& helper,
                         ast::ASTNode* node,
                         CHTLCompilerState state,
                         CHTLScopeType scope);
        ~ASTNodeStateGuard();
        
        // 更新节点状态信息
        void UpdateNodeInfo(const std::function<void(ASTNodeStateInfo&)>& updater);
        
        // 获取当前节点状态
        const ASTNodeStateInfo& GetNodeState() const { return m_NodeState; }
        
        // 验证节点
        bool Validate();
        
        // 标记错误
        void MarkError(const std::string& error);
        
        // 禁止拷贝
        ASTNodeStateGuard(const ASTNodeStateGuard&) = delete;
        ASTNodeStateGuard& operator=(const ASTNodeStateGuard&) = delete;
        
        // 允许移动
        ASTNodeStateGuard(ASTNodeStateGuard&&) noexcept;
        
    private:
        CHTLStateContextHelper& m_Helper;
        ast::ASTNode* m_Node;
        ASTNodeStateInfo m_NodeState;
        bool m_Active;
        
        // 状态守卫
        std::unique_ptr<CHTLStateGuard> m_StateGuard;
        std::unique_ptr<CHTLScopeGuard> m_ScopeGuard;
    };
    
    /**
     * 创建AST节点状态守卫
     */
    std::unique_ptr<ASTNodeStateGuard> CreateNodeGuard(
        ast::ASTNode* node,
        CHTLCompilerState state,
        CHTLScopeType scope = CHTLScopeType::GLOBAL);
    
    /**
     * 语法约束检查
     */
    bool CheckSyntaxConstraints(const ast::ASTNode* node);
    
    /**
     * 获取节点的约束信息
     */
    ASTNodeStateInfo GetNodeConstraints(ast::ASTNodeType nodeType,
                                       CHTLCompilerState state,
                                       CHTLScopeType scope);
    
    /**
     * 状态转换验证
     */
    bool CanTransition(CHTLCompilerState from, CHTLCompilerState to,
                      const ASTNodeStateInfo& nodeInfo);
    
    /**
     * 注册状态转换规则
     */
    void RegisterTransitionRule(const StateTransitionRule& rule);
    
    /**
     * 获取当前上下文的约束
     */
    struct ContextConstraints {
        bool allowsCHTLSyntax;
        bool allowsVariables;
        bool allowsTemplates;
        bool allowsCustomElements;
        bool allowsOriginEmbedding;
        bool allowsComments;
        std::vector<ast::ASTNodeType> allowedNodeTypes;
    };
    
    ContextConstraints GetCurrentConstraints() const;
    
    /**
     * 批量操作支持
     */
    template<typename Func>
    void WithStateContext(CHTLCompilerState state,
                         CHTLScopeType scope,
                         const std::string& context,
                         Func&& func) {
        CHTLStateGuard stateGuard(m_StateMachine, state, context);
        CHTLScopeGuard scopeGuard(m_Context, scope, context);
        func();
    }
    
    /**
     * 错误收集
     */
    void CollectError(const std::string& error, const ASTNodeStateInfo& nodeInfo);
    std::vector<std::string> GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 调试支持
     */
    void EnableDebugMode(bool enable) { m_DebugMode = enable; }
    void DumpNodeState(const ast::ASTNode* node) const;
    void DumpStateHistory() const;
    
private:
    CHTLStateMachine& m_StateMachine;
    CHTLContext& m_Context;
    
    // 节点状态映射
    std::unordered_map<const ast::ASTNode*, ASTNodeStateInfo> m_NodeStates;
    
    // 状态转换规则
    std::vector<StateTransitionRule> m_TransitionRules;
    
    // 错误收集
    std::vector<std::string> m_Errors;
    
    // 调试模式
    bool m_DebugMode;
    
    // 状态历史（用于调试）
    struct StateHistoryEntry {
        std::string timestamp;
        CHTLCompilerState state;
        CHTLScopeType scope;
        std::string context;
        std::string nodeIdentifier;
    };
    std::vector<StateHistoryEntry> m_StateHistory;
    
    // 初始化默认规则
    void InitializeDefaultRules();
    
    // 约束检查实现
    bool CheckElementConstraints(const ast::ASTNode* node, const ASTNodeStateInfo& info);
    bool CheckStyleConstraints(const ast::ASTNode* node, const ASTNodeStateInfo& info);
    bool CheckScriptConstraints(const ast::ASTNode* node, const ASTNodeStateInfo& info);
    bool CheckTemplateConstraints(const ast::ASTNode* node, const ASTNodeStateInfo& info);
    
    // 辅助方法
    void RecordStateChange(const StateHistoryEntry& entry);
    std::string GetTimestamp() const;
};

/**
 * 解析器状态协助器
 * 为解析器提供便捷的状态管理
 */
class ParserStateHelper {
public:
    ParserStateHelper(CHTLStateContextHelper& helper);
    
    // 元素解析状态管理
    class ElementParseGuard {
    public:
        ElementParseGuard(ParserStateHelper& helper, const std::string& elementName);
        ~ElementParseGuard();
        
        void EnterAttribute(const std::string& attrName);
        void ExitAttribute();
        void EnterBody();
        void MarkComplete();
        
    private:
        ParserStateHelper& m_Helper;
        std::string m_ElementName;
        bool m_Complete;
    };
    
    std::unique_ptr<ElementParseGuard> ParseElement(const std::string& elementName);
    
    // 模板解析状态管理
    void EnterTemplate(const std::string& templateType);
    void ExitTemplate();
    
    // 样式块解析状态管理
    void EnterStyleBlock(bool isLocal);
    void ExitStyleBlock();
    
    // 脚本块解析状态管理
    void EnterScriptBlock(bool isLocal);
    void ExitScriptBlock();
    
private:
    CHTLStateContextHelper& m_Helper;
};

/**
 * 生成器状态协助器
 * 为生成器提供便捷的状态管理
 */
class GeneratorStateHelper {
public:
    GeneratorStateHelper(CHTLStateContextHelper& helper);
    
    // 生成上下文管理
    class GenerationGuard {
    public:
        GenerationGuard(GeneratorStateHelper& helper,
                       ast::ASTNode* node,
                       const std::string& outputType);
        ~GenerationGuard();
        
        bool ShouldGenerate() const;
        void MarkGenerated();
        
    private:
        GeneratorStateHelper& m_Helper;
        ast::ASTNode* m_Node;
        std::string m_OutputType;
        bool m_Generated;
    };
    
    std::unique_ptr<GenerationGuard> BeginGeneration(
        ast::ASTNode* node,
        const std::string& outputType = "html");
    
    // 输出类型管理
    void PushOutputType(const std::string& type);
    void PopOutputType();
    std::string GetCurrentOutputType() const;
    
private:
    CHTLStateContextHelper& m_Helper;
    std::vector<std::string> m_OutputTypeStack;
};

} // namespace helpers
} // namespace compiler
} // namespace chtl