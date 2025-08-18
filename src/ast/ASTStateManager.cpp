#include "ASTStateManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <mutex>

namespace chtl {
namespace ast {

// NodeStateGuard 实现
NodeStateGuard::NodeStateGuard(ASTStateManager& manager, ASTNode* node, 
                               NodeState target_state, ProcessingPhase phase,
                               const std::string& context)
    : manager_(manager), node_(node), should_restore_(false), is_valid_(false) {
    
    if (!node_) {
        return;
    }
    
    previous_state_ = manager_.getNodeState(node_);
    previous_phase_ = manager_.getNodePhase(node_);
    
    if (manager_.canTransitionTo(node_, target_state)) {
        if (manager_.transitionState(node_, target_state, phase, context)) {
            should_restore_ = true;
            is_valid_ = true;
        }
    }
}

NodeStateGuard::~NodeStateGuard() {
    if (should_restore_ && node_) {
        // 在析构时恢复到之前的状态
        manager_.setNodeState(node_, previous_state_, previous_phase_, "StateGuard restoration");
    }
}

// NodeFlagGuard 实现
NodeFlagGuard::NodeFlagGuard(ASTStateManager& manager, ASTNode* node, NodeFlag flag)
    : manager_(manager), node_(node), flag_(flag) {
    
    was_set_ = manager_.hasNodeFlag(node_, flag_);
    if (!was_set_) {
        manager_.setNodeFlag(node_, flag_);
    }
}

NodeFlagGuard::~NodeFlagGuard() {
    if (!was_set_ && node_) {
        manager_.clearNodeFlag(node_, flag_);
    }
}

// ASTStateManager 实现
ASTStateManager::ASTStateManager() {
    // 初始化时可以添加默认监听器等
}

bool ASTStateManager::setNodeState(ASTNode* node, NodeState state, ProcessingPhase phase, 
                                  const std::string& context) {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    NodeState old_state = info.current_state;
    
    if (old_state == state) {
        return true; // 状态相同，无需变更
    }
    
    // 验证状态转换是否有效
    if (!isValidStateTransition(old_state, state)) {
        setNodeError(node, "Invalid state transition from " + nodeStateToString(old_state) + 
                          " to " + nodeStateToString(state));
        return false;
    }
    
    // 更新状态
    info.current_state = state;
    info.current_phase = phase;
    
    // 记录状态变更历史
    info.state_history.emplace_back(old_state, state, phase, "", context);
    
    // 通知监听器
    notifyStateChanged(node, old_state, state, phase);
    
    return true;
}

NodeState ASTStateManager::getNodeState(ASTNode* node) const {
    if (!node) return NodeState::UNINITIALIZED;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->current_state : NodeState::UNINITIALIZED;
}

ProcessingPhase ASTStateManager::getNodePhase(ASTNode* node) const {
    if (!node) return ProcessingPhase::LEXICAL_ANALYSIS;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->current_phase : ProcessingPhase::LEXICAL_ANALYSIS;
}

bool ASTStateManager::transitionState(ASTNode* node, NodeState target_state, ProcessingPhase phase,
                                     const std::string& context) {
    return setNodeState(node, target_state, phase, context);
}

bool ASTStateManager::canTransitionTo(ASTNode* node, NodeState target_state) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) return target_state == NodeState::CREATED;
    
    return isValidStateTransition(info->current_state, target_state);
}

void ASTStateManager::setNodeFlag(ASTNode* node, NodeFlag flag) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    bool was_set = info.flags.count(flag) > 0;
    
    if (!was_set) {
        info.flags.insert(flag);
        notifyFlagChanged(node, flag, true);
    }
}

void ASTStateManager::clearNodeFlag(ASTNode* node, NodeFlag flag) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    bool was_set = info.flags.count(flag) > 0;
    
    if (was_set) {
        info.flags.erase(flag);
        notifyFlagChanged(node, flag, false);
    }
}

bool ASTStateManager::hasNodeFlag(ASTNode* node, NodeFlag flag) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info && info->flags.count(flag) > 0;
}

std::unordered_set<NodeFlag> ASTStateManager::getNodeFlags(ASTNode* node) const {
    if (!node) return {};
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->flags : std::unordered_set<NodeFlag>{};
}

void ASTStateManager::addDependency(ASTNode* node, ASTNode* dependent_node, const std::string& type) {
    if (!node || !dependent_node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    
    // 检查是否已存在该依赖
    for (const auto& dep : info.dependencies) {
        if (dep.dependent_node == dependent_node && dep.dependency_type == type) {
            return; // 依赖已存在
        }
    }
    
    // 添加依赖
    info.dependencies.emplace_back(dependent_node, type);
    
    // 检测循环依赖
    std::unordered_set<ASTNode*> visited;
    std::unordered_set<ASTNode*> recursion_stack;
    detectCircularDependencies(node, visited, recursion_stack);
    
    // 设置依赖标记
    setNodeFlag(node, NodeFlag::HAS_DEPENDENCIES);
}

void ASTStateManager::removeDependency(ASTNode* node, ASTNode* dependent_node) {
    if (!node || !dependent_node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    
    info.dependencies.erase(
        std::remove_if(info.dependencies.begin(), info.dependencies.end(),
                      [dependent_node](const NodeDependency& dep) {
                          return dep.dependent_node == dependent_node;
                      }),
        info.dependencies.end());
    
    // 如果没有依赖了，清除依赖标记
    if (info.dependencies.empty()) {
        clearNodeFlag(node, NodeFlag::HAS_DEPENDENCIES);
    }
}

std::vector<NodeDependency> ASTStateManager::getDependencies(ASTNode* node) const {
    if (!node) return {};
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->dependencies : std::vector<NodeDependency>{};
}

bool ASTStateManager::hasDependency(ASTNode* node, ASTNode* dependent_node) const {
    if (!node || !dependent_node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) return false;
    
    return std::any_of(info->dependencies.begin(), info->dependencies.end(),
                      [dependent_node](const NodeDependency& dep) {
                          return dep.dependent_node == dependent_node;
                      });
}

bool ASTStateManager::hasCircularDependency(ASTNode* node) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) return false;
    
    return std::any_of(info->dependencies.begin(), info->dependencies.end(),
                      [](const NodeDependency& dep) {
                          return dep.is_circular;
                      });
}

void ASTStateManager::setMetadata(ASTNode* node, const std::string& key, const std::string& value) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    info.metadata[key] = value;
}

std::string ASTStateManager::getMetadata(ASTNode* node, const std::string& key) const {
    if (!node) return "";
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) return "";
    
    auto it = info->metadata.find(key);
    return it != info->metadata.end() ? it->second : "";
}

bool ASTStateManager::hasMetadata(ASTNode* node, const std::string& key) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info && info->metadata.count(key) > 0;
}

void ASTStateManager::clearMetadata(ASTNode* node, const std::string& key) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    info.metadata.erase(key);
}

void ASTStateManager::setNodeError(ASTNode* node, const std::string& error_message) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    info.last_error = error_message;
    info.current_state = NodeState::ERROR;
    
    notifyError(node, error_message);
}

std::string ASTStateManager::getNodeError(ASTNode* node) const {
    if (!node) return "";
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->last_error : "";
}

bool ASTStateManager::hasNodeError(ASTNode* node) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info && !info->last_error.empty();
}

void ASTStateManager::clearNodeError(ASTNode* node) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    info.last_error.clear();
    
    if (info.current_state == NodeState::ERROR) {
        info.current_state = NodeState::CREATED;
    }
}

std::vector<StateChangeRecord> ASTStateManager::getStateHistory(ASTNode* node) const {
    if (!node) return {};
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    return info ? info->state_history : std::vector<StateChangeRecord>{};
}

void ASTStateManager::clearStateHistory(ASTNode* node) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NodeStateInfo& info = getOrCreateNodeInfo(node);
    info.state_history.clear();
}

void ASTStateManager::setSubtreeState(ASTNode* root, NodeState state, ProcessingPhase phase) {
    if (!root) return;
    
    // 设置根节点状态
    setNodeState(root, state, phase);
    
    // 递归设置子节点状态
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        ASTNode* child = root->getChild(i);
        if (child) {
            setSubtreeState(child, state, phase);
        }
    }
}

std::vector<ASTNode*> ASTStateManager::getNodesInState(NodeState state) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ASTNode*> result;
    for (const auto& pair : node_states_) {
        if (pair.second.current_state == state) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<ASTNode*> ASTStateManager::getNodesWithFlag(NodeFlag flag) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ASTNode*> result;
    for (const auto& pair : node_states_) {
        if (pair.second.flags.count(flag) > 0) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<ASTNode*> ASTStateManager::getNodesInPhase(ProcessingPhase phase) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ASTNode*> result;
    for (const auto& pair : node_states_) {
        if (pair.second.current_phase == phase) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool ASTStateManager::validateNodeState(ASTNode* node) const {
    if (!node) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) return false;
    
    // 检查状态一致性
    if (info->current_state == NodeState::ERROR && info->last_error.empty()) {
        return false;
    }
    
    // 检查依赖关系一致性
    if (hasNodeFlag(node, NodeFlag::HAS_DEPENDENCIES) && info->dependencies.empty()) {
        return false;
    }
    
    return true;
}

bool ASTStateManager::validateSubtreeStates(ASTNode* root) const {
    if (!root) return false;
    
    if (!validateNodeState(root)) {
        return false;
    }
    
    // 递归验证子节点
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        ASTNode* child = root->getChild(i);
        if (child && !validateSubtreeStates(child)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ASTStateManager::getValidationErrors(ASTNode* node) const {
    std::vector<std::string> errors;
    
    if (!node) {
        errors.push_back("Node is null");
        return errors;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) {
        errors.push_back("Node has no state information");
        return errors;
    }
    
    // 检查各种一致性问题
    if (info->current_state == NodeState::ERROR && info->last_error.empty()) {
        errors.push_back("Node is in error state but no error message is set");
    }
    
    if (hasNodeFlag(node, NodeFlag::HAS_DEPENDENCIES) && info->dependencies.empty()) {
        errors.push_back("Node has HAS_DEPENDENCIES flag but no dependencies");
    }
    
    if (hasCircularDependency(node)) {
        errors.push_back("Node has circular dependencies");
    }
    
    return errors;
}

void ASTStateManager::addStateChangeListener(std::shared_ptr<StateChangeListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.push_back(std::weak_ptr<StateChangeListener>(listener));
}

void ASTStateManager::removeStateChangeListener(std::shared_ptr<StateChangeListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.erase(
        std::remove(listeners_.begin(), listeners_.end(), listener),
        listeners_.end());
}

std::unique_ptr<NodeStateGuard> ASTStateManager::createStateGuard(ASTNode* node, NodeState target_state, 
                                                                  ProcessingPhase phase, const std::string& context) {
    return std::make_unique<NodeStateGuard>(*this, node, target_state, phase, context);
}

std::unique_ptr<NodeFlagGuard> ASTStateManager::createFlagGuard(ASTNode* node, NodeFlag flag) {
    return std::make_unique<NodeFlagGuard>(*this, node, flag);
}

size_t ASTStateManager::getNodeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return node_states_.size();
}

size_t ASTStateManager::getNodeCountInState(NodeState state) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t count = 0;
    for (const auto& pair : node_states_) {
        if (pair.second.current_state == state) {
            ++count;
        }
    }
    return count;
}

std::string ASTStateManager::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "AST State Manager Statistics:\n";
    oss << "Total nodes: " << node_states_.size() << "\n";
    
    // 统计各状态的节点数量
    std::unordered_map<NodeState, size_t> state_counts;
    std::unordered_map<ProcessingPhase, size_t> phase_counts;
    std::unordered_map<NodeFlag, size_t> flag_counts;
    
    for (const auto& pair : node_states_) {
        const NodeStateInfo& info = pair.second;
        state_counts[info.current_state]++;
        phase_counts[info.current_phase]++;
        
        for (NodeFlag flag : info.flags) {
            flag_counts[flag]++;
        }
    }
    
    oss << "\nState distribution:\n";
    for (const auto& pair : state_counts) {
        oss << "  " << nodeStateToString(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nPhase distribution:\n";
    for (const auto& pair : phase_counts) {
        oss << "  " << processingPhaseToString(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nFlag distribution:\n";
    for (const auto& pair : flag_counts) {
        oss << "  " << nodeFlagToString(pair.first) << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

void ASTStateManager::printNodeStates() const {
    std::cout << getStatistics() << std::endl;
}

void ASTStateManager::printNodeDependencies(ASTNode* node) const {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (!info) {
        std::cout << "Node has no state information\n";
        return;
    }
    
    std::cout << "Dependencies for node " << node << ":\n";
    for (const auto& dep : info->dependencies) {
        std::cout << "  -> " << dep.dependent_node 
                  << " (type: " << dep.dependency_type 
                  << ", circular: " << (dep.is_circular ? "yes" : "no") << ")\n";
    }
}

void ASTStateManager::removeNode(ASTNode* node) {
    if (!node) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    node_states_.erase(node);
    
    // 清理其他节点对该节点的依赖
    for (auto& pair : node_states_) {
        NodeStateInfo& info = pair.second;
        info.dependencies.erase(
            std::remove_if(info.dependencies.begin(), info.dependencies.end(),
                          [node](const NodeDependency& dep) {
                              return dep.dependent_node == node;
                          }),
            info.dependencies.end());
    }
}

void ASTStateManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    node_states_.clear();
}

void ASTStateManager::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    node_states_.clear();
    listeners_.clear();
}

std::string ASTStateManager::nodeStateToString(NodeState state) const {
    switch (state) {
        case NodeState::UNINITIALIZED: return "UNINITIALIZED";
        case NodeState::CREATED: return "CREATED";
        case NodeState::PARSING: return "PARSING";
        case NodeState::PARSED: return "PARSED";
        case NodeState::VALIDATING: return "VALIDATING";
        case NodeState::VALIDATED: return "VALIDATED";
        case NodeState::TRANSFORMING: return "TRANSFORMING";
        case NodeState::TRANSFORMED: return "TRANSFORMED";
        case NodeState::GENERATING: return "GENERATING";
        case NodeState::GENERATED: return "GENERATED";
        case NodeState::ERROR: return "ERROR";
        case NodeState::DEPRECATED: return "DEPRECATED";
        case NodeState::DELETED: return "DELETED";
        default: return "UNKNOWN";
    }
}

std::string ASTStateManager::processingPhaseToString(ProcessingPhase phase) const {
    switch (phase) {
        case ProcessingPhase::LEXICAL_ANALYSIS: return "LEXICAL_ANALYSIS";
        case ProcessingPhase::SYNTAX_ANALYSIS: return "SYNTAX_ANALYSIS";
        case ProcessingPhase::SEMANTIC_ANALYSIS: return "SEMANTIC_ANALYSIS";
        case ProcessingPhase::OPTIMIZATION: return "OPTIMIZATION";
        case ProcessingPhase::CODE_GENERATION: return "CODE_GENERATION";
        case ProcessingPhase::FINALIZATION: return "FINALIZATION";
        default: return "UNKNOWN";
    }
}

std::string ASTStateManager::nodeFlagToString(NodeFlag flag) const {
    switch (flag) {
        case NodeFlag::TEMPLATE_NODE: return "TEMPLATE_NODE";
        case NodeFlag::CUSTOM_NODE: return "CUSTOM_NODE";
        case NodeFlag::DYNAMIC_NODE: return "DYNAMIC_NODE";
        case NodeFlag::STATIC_NODE: return "STATIC_NODE";
        case NodeFlag::OPTIMIZED: return "OPTIMIZED";
        case NodeFlag::CACHEABLE: return "CACHEABLE";
        case NodeFlag::REQUIRES_VALIDATION: return "REQUIRES_VALIDATION";
        case NodeFlag::HAS_DEPENDENCIES: return "HAS_DEPENDENCIES";
        case NodeFlag::CROSS_REFERENCE: return "CROSS_REFERENCE";
        case NodeFlag::DEBUG_INFO: return "DEBUG_INFO";
        default: return "UNKNOWN";
    }
}

// 私有方法实现
NodeStateInfo& ASTStateManager::getOrCreateNodeInfo(ASTNode* node) {
    auto it = node_states_.find(node);
    if (it == node_states_.end()) {
        it = node_states_.emplace(node, NodeStateInfo()).first;
    }
    return it->second;
}

const NodeStateInfo* ASTStateManager::getNodeInfo(ASTNode* node) const {
    auto it = node_states_.find(node);
    return it != node_states_.end() ? &it->second : nullptr;
}

void ASTStateManager::notifyStateChanged(ASTNode* node, NodeState from, NodeState to, ProcessingPhase phase) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onStateChanged(node, from, to, phase);
        }
    }
}

void ASTStateManager::notifyFlagChanged(ASTNode* node, NodeFlag flag, bool added) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onFlagChanged(node, flag, added);
        }
    }
}

void ASTStateManager::notifyError(ASTNode* node, const std::string& error_message) {
    for (auto& listener : listeners_) {
        if (auto shared_listener = listener.lock()) {
            shared_listener->onError(node, error_message);
        }
    }
}

bool ASTStateManager::isValidStateTransition(NodeState from, NodeState to) const {
    // 定义有效的状态转换规则
    switch (from) {
        case NodeState::UNINITIALIZED:
            return to == NodeState::CREATED || to == NodeState::ERROR;
            
        case NodeState::CREATED:
            return to == NodeState::PARSING || to == NodeState::ERROR || to == NodeState::DELETED;
            
        case NodeState::PARSING:
            return to == NodeState::PARSED || to == NodeState::ERROR;
            
        case NodeState::PARSED:
            return to == NodeState::VALIDATING || to == NodeState::TRANSFORMING || 
                   to == NodeState::GENERATING || to == NodeState::ERROR;
            
        case NodeState::VALIDATING:
            return to == NodeState::VALIDATED || to == NodeState::ERROR;
            
        case NodeState::VALIDATED:
            return to == NodeState::TRANSFORMING || to == NodeState::GENERATING || to == NodeState::ERROR;
            
        case NodeState::TRANSFORMING:
            return to == NodeState::TRANSFORMED || to == NodeState::ERROR;
            
        case NodeState::TRANSFORMED:
            return to == NodeState::GENERATING || to == NodeState::ERROR;
            
        case NodeState::GENERATING:
            return to == NodeState::GENERATED || to == NodeState::ERROR;
            
        case NodeState::GENERATED:
            return to == NodeState::DEPRECATED || to == NodeState::DELETED || to == NodeState::ERROR;
            
        case NodeState::ERROR:
            return to == NodeState::CREATED || to == NodeState::DELETED; // 可以从错误状态恢复
            
        case NodeState::DEPRECATED:
            return to == NodeState::DELETED;
            
        case NodeState::DELETED:
            return false; // 删除状态不能转换到其他状态
            
        default:
            return false;
    }
}

void ASTStateManager::detectCircularDependencies(ASTNode* node, std::unordered_set<ASTNode*>& visited,
                                                std::unordered_set<ASTNode*>& recursion_stack) {
    if (recursion_stack.count(node) > 0) {
        // 发现循环依赖
        NodeStateInfo& info = getOrCreateNodeInfo(node);
        for (auto& dep : info.dependencies) {
            if (recursion_stack.count(dep.dependent_node) > 0) {
                dep.is_circular = true;
            }
        }
        return;
    }
    
    if (visited.count(node) > 0) {
        return;
    }
    
    visited.insert(node);
    recursion_stack.insert(node);
    
    const NodeStateInfo* info = getNodeInfo(node);
    if (info) {
        for (const auto& dep : info->dependencies) {
            detectCircularDependencies(dep.dependent_node, visited, recursion_stack);
        }
    }
    
    recursion_stack.erase(node);
}

} // namespace ast
} // namespace chtl
