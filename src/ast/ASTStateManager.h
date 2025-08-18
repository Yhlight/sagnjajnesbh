#pragma once
#include "ASTNode.h"
#include "../common/State.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>

namespace chtl {
namespace ast {

// AST节点状态枚举
enum class NodeState {
    UNINITIALIZED,      // 未初始化
    CREATED,            // 已创建
    PARSING,            // 解析中
    PARSED,             // 已解析
    VALIDATING,         // 验证中
    VALIDATED,          // 已验证
    TRANSFORMING,       // 转换中
    TRANSFORMED,        // 已转换
    GENERATING,         // 生成中
    GENERATED,          // 已生成
    ERROR,              // 错误状态
    DEPRECATED,         // 已废弃
    DELETED             // 已删除
};

// 节点处理阶段
enum class ProcessingPhase {
    LEXICAL_ANALYSIS,   // 词法分析阶段
    SYNTAX_ANALYSIS,    // 语法分析阶段
    SEMANTIC_ANALYSIS,  // 语义分析阶段
    OPTIMIZATION,       // 优化阶段
    CODE_GENERATION,    // 代码生成阶段
    FINALIZATION        // 最终化阶段
};

// 节点标记类型
enum class NodeFlag {
    TEMPLATE_NODE,      // 模板节点
    CUSTOM_NODE,        // 自定义节点
    DYNAMIC_NODE,       // 动态节点
    STATIC_NODE,        // 静态节点
    OPTIMIZED,          // 已优化
    CACHEABLE,          // 可缓存
    REQUIRES_VALIDATION,// 需要验证
    HAS_DEPENDENCIES,   // 有依赖关系
    CROSS_REFERENCE,    // 交叉引用
    DEBUG_INFO          // 包含调试信息
};

// 状态变更历史记录
struct StateChangeRecord {
    NodeState from_state;
    NodeState to_state;
    ProcessingPhase phase;
    std::chrono::steady_clock::time_point timestamp;
    std::string description;
    std::string context;
    
    StateChangeRecord(NodeState from, NodeState to, ProcessingPhase p, 
                     const std::string& desc = "", const std::string& ctx = "")
        : from_state(from), to_state(to), phase(p), 
          timestamp(std::chrono::steady_clock::now()),
          description(desc), context(ctx) {}
};

// 节点依赖关系
struct NodeDependency {
    ASTNode* dependent_node;    // 依赖的节点
    std::string dependency_type; // 依赖类型
    bool is_circular;           // 是否循环依赖
    
    NodeDependency(ASTNode* node, const std::string& type, bool circular = false)
        : dependent_node(node), dependency_type(type), is_circular(circular) {}
};

// AST节点状态信息
struct NodeStateInfo {
    NodeState current_state;
    ProcessingPhase current_phase;
    std::unordered_set<NodeFlag> flags;
    std::vector<StateChangeRecord> state_history;
    std::vector<NodeDependency> dependencies;
    std::unordered_map<std::string, std::string> metadata;
    std::string last_error;
    size_t validation_count;
    
    NodeStateInfo() 
        : current_state(NodeState::UNINITIALIZED),
          current_phase(ProcessingPhase::LEXICAL_ANALYSIS),
          validation_count(0) {}
};

// RAII节点状态守护器
class NodeStateGuard {
public:
    NodeStateGuard(class ASTStateManager& manager, ASTNode* node, 
                   NodeState target_state, ProcessingPhase phase,
                   const std::string& context = "");
    ~NodeStateGuard();
    
    // 禁用复制和移动
    NodeStateGuard(const NodeStateGuard&) = delete;
    NodeStateGuard& operator=(const NodeStateGuard&) = delete;
    NodeStateGuard(NodeStateGuard&&) = delete;
    NodeStateGuard& operator=(NodeStateGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    NodeState getPreviousState() const { return previous_state_; }
    
private:
    class ASTStateManager& manager_;
    ASTNode* node_;
    NodeState previous_state_;
    ProcessingPhase previous_phase_;
    bool should_restore_;
    bool is_valid_;
};

// RAII节点标记守护器
class NodeFlagGuard {
public:
    NodeFlagGuard(class ASTStateManager& manager, ASTNode* node, NodeFlag flag);
    ~NodeFlagGuard();
    
    // 禁用复制和移动
    NodeFlagGuard(const NodeFlagGuard&) = delete;
    NodeFlagGuard& operator=(const NodeFlagGuard&) = delete;
    NodeFlagGuard(NodeFlagGuard&&) = delete;
    NodeFlagGuard& operator=(NodeFlagGuard&&) = delete;
    
private:
    class ASTStateManager& manager_;
    ASTNode* node_;
    NodeFlag flag_;
    bool was_set_;
};

// 状态变更监听器接口
class StateChangeListener {
public:
    virtual ~StateChangeListener() = default;
    virtual void onStateChanged(ASTNode* node, NodeState from, NodeState to, ProcessingPhase phase) = 0;
    virtual void onFlagChanged(ASTNode* node, NodeFlag flag, bool added) = 0;
    virtual void onError(ASTNode* node, const std::string& error_message) = 0;
};

// AST节点状态管理器
class ASTStateManager {
public:
    ASTStateManager();
    ~ASTStateManager() = default;
    
    // 节点状态管理
    bool setNodeState(ASTNode* node, NodeState state, ProcessingPhase phase, 
                     const std::string& context = "");
    NodeState getNodeState(ASTNode* node) const;
    ProcessingPhase getNodePhase(ASTNode* node) const;
    
    bool transitionState(ASTNode* node, NodeState target_state, ProcessingPhase phase,
                        const std::string& context = "");
    bool canTransitionTo(ASTNode* node, NodeState target_state) const;
    
    // 节点标记管理
    void setNodeFlag(ASTNode* node, NodeFlag flag);
    void clearNodeFlag(ASTNode* node, NodeFlag flag);
    bool hasNodeFlag(ASTNode* node, NodeFlag flag) const;
    std::unordered_set<NodeFlag> getNodeFlags(ASTNode* node) const;
    
    // 节点依赖管理
    void addDependency(ASTNode* node, ASTNode* dependent_node, const std::string& type);
    void removeDependency(ASTNode* node, ASTNode* dependent_node);
    std::vector<NodeDependency> getDependencies(ASTNode* node) const;
    bool hasDependency(ASTNode* node, ASTNode* dependent_node) const;
    bool hasCircularDependency(ASTNode* node) const;
    
    // 节点元数据管理
    void setMetadata(ASTNode* node, const std::string& key, const std::string& value);
    std::string getMetadata(ASTNode* node, const std::string& key) const;
    bool hasMetadata(ASTNode* node, const std::string& key) const;
    void clearMetadata(ASTNode* node, const std::string& key);
    
    // 错误处理
    void setNodeError(ASTNode* node, const std::string& error_message);
    std::string getNodeError(ASTNode* node) const;
    bool hasNodeError(ASTNode* node) const;
    void clearNodeError(ASTNode* node);
    
    // 状态历史
    std::vector<StateChangeRecord> getStateHistory(ASTNode* node) const;
    void clearStateHistory(ASTNode* node);
    
    // 批量操作
    void setSubtreeState(ASTNode* root, NodeState state, ProcessingPhase phase);
    std::vector<ASTNode*> getNodesInState(NodeState state) const;
    std::vector<ASTNode*> getNodesWithFlag(NodeFlag flag) const;
    std::vector<ASTNode*> getNodesInPhase(ProcessingPhase phase) const;
    
    // 验证和一致性检查
    bool validateNodeState(ASTNode* node) const;
    bool validateSubtreeStates(ASTNode* root) const;
    std::vector<std::string> getValidationErrors(ASTNode* node) const;
    
    // 监听器管理
    void addStateChangeListener(std::shared_ptr<StateChangeListener> listener);
    void removeStateChangeListener(std::shared_ptr<StateChangeListener> listener);
    
    // RAII守护器创建
    std::unique_ptr<NodeStateGuard> createStateGuard(ASTNode* node, NodeState target_state, 
                                                     ProcessingPhase phase, const std::string& context = "");
    std::unique_ptr<NodeFlagGuard> createFlagGuard(ASTNode* node, NodeFlag flag);
    
    // 统计和诊断
    size_t getNodeCount() const;
    size_t getNodeCountInState(NodeState state) const;
    std::string getStatistics() const;
    void printNodeStates() const;
    void printNodeDependencies(ASTNode* node) const;
    
    // 清理和重置
    void removeNode(ASTNode* node);
    void clear();
    void reset();
    
    // 调试支持
    std::string nodeStateToString(NodeState state) const;
    std::string processingPhaseToString(ProcessingPhase phase) const;
    std::string nodeFlagToString(NodeFlag flag) const;
    
private:
    std::unordered_map<ASTNode*, NodeStateInfo> node_states_;
    std::vector<std::weak_ptr<StateChangeListener>> listeners_;
    mutable std::mutex mutex_;  // 线程安全
    
    // 内部辅助方法
    NodeStateInfo& getOrCreateNodeInfo(ASTNode* node);
    const NodeStateInfo* getNodeInfo(ASTNode* node) const;
    void notifyStateChanged(ASTNode* node, NodeState from, NodeState to, ProcessingPhase phase);
    void notifyFlagChanged(ASTNode* node, NodeFlag flag, bool added);
    void notifyError(ASTNode* node, const std::string& error_message);
    
    bool isValidStateTransition(NodeState from, NodeState to) const;
    void detectCircularDependencies(ASTNode* node, std::unordered_set<ASTNode*>& visited,
                                   std::unordered_set<ASTNode*>& recursion_stack);
    
    friend class NodeStateGuard;
    friend class NodeFlagGuard;
};

// 便利宏定义
#define CHTL_NODE_STATE_GUARD(manager, node, state, phase) \
    auto node_guard = (manager).createStateGuard(node, state, phase, __FUNCTION__)

#define CHTL_NODE_FLAG_GUARD(manager, node, flag) \
    auto flag_guard = (manager).createFlagGuard(node, flag)

#define CHTL_SET_NODE_METADATA(manager, node, key, value) \
    (manager).setMetadata(node, key, value)

#define CHTL_CHECK_NODE_STATE(manager, node, expected_state) \
    ((manager).getNodeState(node) == expected_state)

} // namespace ast
} // namespace chtl
