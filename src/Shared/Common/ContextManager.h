#pragma once
#include "Context.h"
#include "State.h"
#include "../ast/ASTStateManager.h"
#include <memory>
#include <stack>
#include <unordered_map>
#include <functional>

namespace chtl {

// 前向声明
namespace ast {
    class ASTNode;
}

// 编译阶段
enum class CompilationPhase {
    INITIALIZATION,     // 初始化阶段
    LEXICAL_SCANNING,   // 词法扫描阶段
    SYNTAX_PARSING,     // 语法解析阶段
    SEMANTIC_ANALYSIS,  // 语义分析阶段
    OPTIMIZATION,       // 优化阶段
    CODE_GENERATION,    // 代码生成阶段
    FINALIZATION        // 完成阶段
};

// 上下文作用域类型
enum class ContextScope {
    GLOBAL,             // 全局作用域
    FILE,               // 文件作用域
    NAMESPACE,          // 命名空间作用域
    TEMPLATE,           // 模板作用域
    CUSTOM,             // 自定义作用域
    ELEMENT,            // 元素作用域
    ATTRIBUTE,          // 属性作用域
    STYLE_BLOCK,        // 样式块作用域
    SCRIPT_BLOCK,       // 脚本块作用域
    COMMENT             // 注释作用域
};

// 上下文状态信息
struct ContextState {
    ContextScope scope;
    CompilationPhase phase;
    CompilerState compiler_state;
    SyntaxContext syntax_context;
    std::string scope_name;
    std::string file_path;
    size_t line_number;
    size_t column_number;
    ast::ASTNode* associated_node;
    std::unordered_map<std::string, std::string> local_metadata;
    
    ContextState(ContextScope s = ContextScope::GLOBAL, 
                CompilationPhase p = CompilationPhase::INITIALIZATION,
                const std::string& name = "")
        : scope(s), phase(p), 
          compiler_state(CompilerState::IDLE),
          syntax_context(SyntaxContext::GLOBAL),
          scope_name(name), line_number(1), column_number(1),
          associated_node(nullptr) {}
};

// RAII编译阶段守护器
class CompilationPhaseGuard {
public:
    CompilationPhaseGuard(class ContextManager& manager, CompilationPhase phase);
    ~CompilationPhaseGuard();
    
    // 禁用复制和移动
    CompilationPhaseGuard(const CompilationPhaseGuard&) = delete;
    CompilationPhaseGuard& operator=(const CompilationPhaseGuard&) = delete;
    CompilationPhaseGuard(CompilationPhaseGuard&&) = delete;
    CompilationPhaseGuard& operator=(CompilationPhaseGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    
private:
    class ContextManager& manager_;
    CompilationPhase previous_phase_;
    bool should_restore_;
    bool is_valid_;
};

// RAII上下文作用域守护器
class ContextScopeGuard {
public:
    ContextScopeGuard(class ContextManager& manager, ContextScope scope, 
                     const std::string& name = "", ast::ASTNode* node = nullptr);
    ~ContextScopeGuard();
    
    // 禁用复制和移动
    ContextScopeGuard(const ContextScopeGuard&) = delete;
    ContextScopeGuard& operator=(const ContextScopeGuard&) = delete;
    ContextScopeGuard(ContextScopeGuard&&) = delete;
    ContextScopeGuard& operator=(ContextScopeGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    ContextScope getScope() const { return scope_; }
    
private:
    class ContextManager& manager_;
    ContextScope scope_;
    bool should_pop_;
    bool is_valid_;
};

// RAII复合状态守护器（同时管理编译器状态、AST节点状态和上下文作用域）
class CompoundStateGuard {
public:
    CompoundStateGuard(class ContextManager& manager, 
                      CompilationPhase phase,
                      CompilerState compiler_state,
                      SyntaxContext syntax_context,
                      ContextScope scope,
                      ast::ASTNode* node = nullptr,
                      const std::string& scope_name = "");
    ~CompoundStateGuard();
    
    // 禁用复制和移动
    CompoundStateGuard(const CompoundStateGuard&) = delete;
    CompoundStateGuard& operator=(const CompoundStateGuard&) = delete;
    CompoundStateGuard(CompoundStateGuard&&) = delete;
    CompoundStateGuard& operator=(CompoundStateGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    
private:
    std::unique_ptr<CompilationPhaseGuard> phase_guard_;
    std::unique_ptr<StateGuard> state_guard_;
    std::unique_ptr<ContextScopeGuard> scope_guard_;
    std::unique_ptr<ast::NodeStateGuard> node_guard_;
    bool is_valid_;
};

// 上下文变更监听器
class ContextChangeListener {
public:
    virtual ~ContextChangeListener() = default;
    virtual void onPhaseChanged(CompilationPhase from, CompilationPhase to) = 0;
    virtual void onScopeEntered(ContextScope scope, const std::string& name) = 0;
    virtual void onScopeExited(ContextScope scope, const std::string& name) = 0;
    virtual void onContextError(const std::string& error_message, const ContextState& context) = 0;
};

// 增强的上下文管理器
class ContextManager {
public:
    ContextManager();
    ~ContextManager() = default;
    
    // 基础上下文访问
    CompilerContext& getCompilerContext() { return compiler_context_; }
    const CompilerContext& getCompilerContext() const { return compiler_context_; }
    
    ast::ASTStateManager& getASTStateManager() { return ast_state_manager_; }
    const ast::ASTStateManager& getASTStateManager() const { return ast_state_manager_; }
    
    // 编译阶段管理
    CompilationPhase getCurrentPhase() const;
    bool setPhase(CompilationPhase phase);
    bool canTransitionToPhase(CompilationPhase phase) const;
    std::string getPhaseDescription() const;
    
    // 上下文作用域管理
    ContextScope getCurrentScope() const;
    bool enterScope(ContextScope scope, const std::string& name = "", ast::ASTNode* node = nullptr);
    bool exitScope();
    std::string getCurrentScopeName() const;
    size_t getScopeDepth() const;
    
    // 上下文状态查询
    ContextState getCurrentContextState() const;
    std::vector<ContextState> getContextStack() const;
    bool isInScope(ContextScope scope) const;
    bool isInPhase(CompilationPhase phase) const;
    
    // 位置管理
    void updatePosition(const std::string& file_path, size_t line, size_t column);
    std::tuple<std::string, size_t, size_t> getCurrentPosition() const;
    
    // AST节点关联
    void associateNode(ast::ASTNode* node);
    ast::ASTNode* getCurrentAssociatedNode() const;
    void syncNodeState(ast::ASTNode* node);
    
    // 错误处理
    void reportContextError(const std::string& message);
    std::vector<std::string> getContextErrors() const;
    bool hasContextErrors() const;
    void clearContextErrors();
    
    // 元数据管理
    void setContextMetadata(const std::string& key, const std::string& value);
    std::string getContextMetadata(const std::string& key) const;
    void clearContextMetadata(const std::string& key);
    
    // 状态同步
    void syncWithCompilerContext();
    void syncWithASTStateManager();
    bool validateContextConsistency() const;
    
    // RAII守护器创建
    std::unique_ptr<CompilationPhaseGuard> createPhaseGuard(CompilationPhase phase);
    std::unique_ptr<ContextScopeGuard> createScopeGuard(ContextScope scope, 
                                                        const std::string& name = "", 
                                                        ast::ASTNode* node = nullptr);
    std::unique_ptr<CompoundStateGuard> createCompoundGuard(CompilationPhase phase,
                                                           CompilerState compiler_state,
                                                           SyntaxContext syntax_context,
                                                           ContextScope scope,
                                                           ast::ASTNode* node = nullptr,
                                                           const std::string& scope_name = "");
    
    // 监听器管理
    void addContextChangeListener(std::shared_ptr<ContextChangeListener> listener);
    void removeContextChangeListener(std::shared_ptr<ContextChangeListener> listener);
    
    // 统计和诊断
    std::string getStatistics() const;
    void printContextStack() const;
    void printCurrentState() const;
    
    // 重置和清理
    void reset();
    void clear();
    
    // 调试支持
    std::string compilationPhaseToString(CompilationPhase phase) const;
    std::string contextScopeToString(ContextScope scope) const;
    
private:
    CompilerContext compiler_context_;
    ast::ASTStateManager ast_state_manager_;
    std::stack<ContextState> context_stack_;
    CompilationPhase current_phase_;
    std::vector<std::string> context_errors_;
    std::vector<std::shared_ptr<ContextChangeListener>> listeners_;
    
    // 内部辅助方法
    void notifyPhaseChanged(CompilationPhase from, CompilationPhase to);
    void notifyScopeEntered(ContextScope scope, const std::string& name);
    void notifyScopeExited(ContextScope scope, const std::string& name);
    void notifyContextError(const std::string& error_message);
    
    bool isValidPhaseTransition(CompilationPhase from, CompilationPhase to) const;
    void updateCurrentContextState();
    
    friend class CompilationPhaseGuard;
    friend class ContextScopeGuard;
    friend class CompoundStateGuard;
};

// 便利宏定义
#define CHTL_PHASE_GUARD(manager, phase) \
    auto phase_guard = (manager).createPhaseGuard(phase)

#define CHTL_SCOPE_GUARD(manager, scope, name) \
    auto scope_guard = (manager).createScopeGuard(scope, name)

#define CHTL_COMPOUND_GUARD(manager, phase, compiler_state, syntax_context, scope, node, name) \
    auto compound_guard = (manager).createCompoundGuard(phase, compiler_state, syntax_context, scope, node, name)

#define CHTL_SYNC_NODE_STATE(manager, node) \
    (manager).syncNodeState(node)

#define CHTL_CHECK_CONTEXT_CONSISTENCY(manager) \
    (manager).validateContextConsistency()

} // namespace chtl