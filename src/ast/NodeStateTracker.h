#pragma once
#include "ASTStateManager.h"
#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace chtl {
namespace ast {

// 状态快照
struct StateSnapshot {
    ASTNode* node;
    NodeState state;
    ProcessingPhase phase;
    std::unordered_set<NodeFlag> flags;
    std::unordered_map<std::string, std::string> metadata;
    std::string error_message;
    std::chrono::steady_clock::time_point timestamp;
    std::string context;
    size_t snapshot_id;
    
    StateSnapshot(ASTNode* n, NodeState s, ProcessingPhase p, const std::string& ctx = "")
        : node(n), state(s), phase(p), context(ctx),
          timestamp(std::chrono::steady_clock::now()),
          snapshot_id(0) {}
};

// 状态变更事件
struct StateChangeEvent {
    ASTNode* node;
    NodeState from_state;
    NodeState to_state;
    ProcessingPhase from_phase;
    ProcessingPhase to_phase;
    std::chrono::steady_clock::time_point timestamp;
    std::string context;
    std::string description;
    size_t event_id;
    
    StateChangeEvent(ASTNode* n, NodeState from_s, NodeState to_s, 
                    ProcessingPhase from_p, ProcessingPhase to_p,
                    const std::string& ctx = "", const std::string& desc = "")
        : node(n), from_state(from_s), to_state(to_s), 
          from_phase(from_p), to_phase(to_p),
          timestamp(std::chrono::steady_clock::now()),
          context(ctx), description(desc), event_id(0) {}
};

// 回滚点
struct RollbackPoint {
    std::string name;
    std::string description;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<StateSnapshot> snapshots;
    size_t rollback_id;
    
    RollbackPoint(const std::string& n, const std::string& desc = "")
        : name(n), description(desc), 
          timestamp(std::chrono::steady_clock::now()),
          rollback_id(0) {}
};

// 状态跟踪配置
struct TrackingConfig {
    bool enable_history;
    bool enable_snapshots;
    bool enable_rollback;
    size_t max_history_size;
    size_t max_snapshot_count;
    std::chrono::seconds snapshot_interval;
    bool auto_cleanup;
    
    TrackingConfig()
        : enable_history(true), enable_snapshots(true), enable_rollback(true),
          max_history_size(1000), max_snapshot_count(100),
          snapshot_interval(std::chrono::seconds(5)), auto_cleanup(true) {}
};

// 节点状态跟踪器
class NodeStateTracker : public StateChangeListener {
public:
    explicit NodeStateTracker(ASTStateManager& state_manager, const TrackingConfig& config = TrackingConfig());
    ~NodeStateTracker();
    
    // StateChangeListener 接口实现
    void onStateChanged(ASTNode* node, NodeState from, NodeState to, ProcessingPhase phase) override;
    void onFlagChanged(ASTNode* node, NodeFlag flag, bool added) override;
    void onError(ASTNode* node, const std::string& error_message) override;
    
    // 快照管理
    size_t createSnapshot(const std::string& name, const std::string& description = "");
    size_t createNodeSnapshot(ASTNode* node, const std::string& context = "");
    size_t createSubtreeSnapshot(ASTNode* root, const std::string& context = "");
    
    bool hasSnapshot(size_t snapshot_id) const;
    StateSnapshot getSnapshot(size_t snapshot_id) const;
    std::vector<StateSnapshot> getNodeSnapshots(ASTNode* node) const;
    std::vector<StateSnapshot> getAllSnapshots() const;
    
    // 回滚点管理
    size_t createRollbackPoint(const std::string& name, const std::string& description = "");
    size_t createNodeRollbackPoint(ASTNode* node, const std::string& name, const std::string& description = "");
    size_t createSubtreeRollbackPoint(ASTNode* root, const std::string& name, const std::string& description = "");
    
    bool hasRollbackPoint(size_t rollback_id) const;
    bool hasRollbackPoint(const std::string& name) const;
    RollbackPoint getRollbackPoint(size_t rollback_id) const;
    RollbackPoint getRollbackPoint(const std::string& name) const;
    std::vector<RollbackPoint> getAllRollbackPoints() const;
    
    // 回滚操作
    bool rollbackToPoint(size_t rollback_id);
    bool rollbackToPoint(const std::string& name);
    bool rollbackNode(ASTNode* node, size_t snapshot_id);
    bool rollbackSubtree(ASTNode* root, size_t snapshot_id);
    
    // 历史记录管理
    std::vector<StateChangeEvent> getNodeHistory(ASTNode* node) const;
    std::vector<StateChangeEvent> getHistoryInRange(
        std::chrono::steady_clock::time_point start,
        std::chrono::steady_clock::time_point end) const;
    std::vector<StateChangeEvent> getRecentHistory(size_t count) const;
    std::vector<StateChangeEvent> getAllHistory() const;
    
    // 状态查询和分析
    size_t getStateChangeCount(ASTNode* node) const;
    size_t getTotalStateChanges() const;
    std::chrono::milliseconds getNodeStateTime(ASTNode* node, NodeState state) const;
    std::vector<ASTNode*> getNodesInState(NodeState state) const;
    
    // 统计和分析
    std::string getTrackingStatistics() const;
    std::string getNodeStatistics(ASTNode* node) const;
    void printTrackingReport() const;
    void exportTrackingData(const std::string& file_path) const;
    
    // 配置管理
    void setConfig(const TrackingConfig& config);
    TrackingConfig getConfig() const;
    void enableTracking();
    void disableTracking();
    bool isTrackingEnabled() const;
    
    // 清理和维护
    void clearHistory();
    void clearSnapshots();
    void clearRollbackPoints();
    void clearAll();
    void cleanup();  // 自动清理过期数据
    
    // 调试和诊断
    void validateTrackingData() const;
    std::vector<std::string> getValidationErrors() const;
    void dumpTrackingState() const;
    
private:
    ASTStateManager& state_manager_;
    TrackingConfig config_;
    bool tracking_enabled_;
    
    // 数据存储
    std::vector<StateChangeEvent> history_;
    std::unordered_map<size_t, StateSnapshot> snapshots_;
    std::unordered_map<size_t, RollbackPoint> rollback_points_;
    std::unordered_map<std::string, size_t> rollback_point_names_;
    
    // ID生成器
    size_t next_event_id_;
    size_t next_snapshot_id_;
    size_t next_rollback_id_;
    
    // 内部辅助方法
    void addToHistory(const StateChangeEvent& event);
    StateSnapshot captureNodeState(ASTNode* node, const std::string& context = "") const;
    void restoreNodeState(ASTNode* node, const StateSnapshot& snapshot);
    
    bool applySnapshot(const StateSnapshot& snapshot);
    void cleanupExpiredData();
    void enforceHistoryLimit();
    void enforceSnapshotLimit();
    
    // 验证方法
    bool validateSnapshot(const StateSnapshot& snapshot) const;
    bool validateRollbackPoint(const RollbackPoint& rollback_point) const;
    
    // 序列化支持
    std::string serializeSnapshot(const StateSnapshot& snapshot) const;
    std::string serializeEvent(const StateChangeEvent& event) const;
    std::string serializeRollbackPoint(const RollbackPoint& rollback_point) const;
};

// RAII状态跟踪守护器
class StateTrackingGuard {
public:
    StateTrackingGuard(NodeStateTracker& tracker, const std::string& name, const std::string& description = "");
    ~StateTrackingGuard();
    
    // 禁用复制和移动
    StateTrackingGuard(const StateTrackingGuard&) = delete;
    StateTrackingGuard& operator=(const StateTrackingGuard&) = delete;
    StateTrackingGuard(StateTrackingGuard&&) = delete;
    StateTrackingGuard& operator=(StateTrackingGuard&&) = delete;
    
    bool isValid() const { return is_valid_; }
    size_t getRollbackPointId() const { return rollback_point_id_; }
    
    // 手动回滚
    bool rollback();
    
private:
    NodeStateTracker& tracker_;
    size_t rollback_point_id_;
    bool should_rollback_on_destroy_;
    bool is_valid_;
};

// 便利宏定义
#define CHTL_STATE_TRACKING_GUARD(tracker, name) \
    StateTrackingGuard tracking_guard(tracker, name, __FUNCTION__)

#define CHTL_CREATE_ROLLBACK_POINT(tracker, name) \
    (tracker).createRollbackPoint(name, __FUNCTION__)

#define CHTL_SNAPSHOT_NODE(tracker, node) \
    (tracker).createNodeSnapshot(node, __FUNCTION__)

} // namespace ast
} // namespace chtl