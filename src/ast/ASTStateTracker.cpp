#include "../../include/ASTStateTracker.h"
#include <chrono>
#include <iostream>
#include <sstream>

namespace chtl {

// ASTStateInfo 实现
size_t ASTStateInfo::getCurrentTimeMs() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// ASTStateTracker 实现
ASTStateTracker::ASTStateTracker(StateMachine& stateMachine) 
    : stateMachine_(stateMachine) {
}

void ASTStateTracker::trackNode(std::shared_ptr<CHTLASTNode> node, const ASTStateInfo& stateInfo) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    nodeStates_[key] = stateInfo;
    updateStatistics();
}

void ASTStateTracker::trackNode(std::shared_ptr<CHTLJSASTNode> node, const ASTStateInfo& stateInfo) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    nodeStates_[key] = stateInfo;
    updateStatistics();
}

void ASTStateTracker::updateNodeState(std::shared_ptr<CHTLASTNode> node, ASTNodeState state) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.state = state;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

void ASTStateTracker::updateNodeState(std::shared_ptr<CHTLJSASTNode> node, ASTNodeState state) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.state = state;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

void ASTStateTracker::updateNodeTypeState(std::shared_ptr<CHTLASTNode> node, ASTTypeState typeState) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.typeState = typeState;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::updateNodeTypeState(std::shared_ptr<CHTLJSASTNode> node, ASTTypeState typeState) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.typeState = typeState;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

ASTStateInfo ASTStateTracker::getNodeState(std::shared_ptr<CHTLASTNode> node) const {
    if (!node) return ASTStateInfo();
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        return it->second;
    }
    return ASTStateInfo();
}

ASTStateInfo ASTStateTracker::getNodeState(std::shared_ptr<CHTLJSASTNode> node) const {
    if (!node) return ASTStateInfo();
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        return it->second;
    }
    return ASTStateInfo();
}

bool ASTStateTracker::isNodeInState(std::shared_ptr<CHTLASTNode> node, ASTNodeState state) const {
    if (!node) return false;
    
    auto stateInfo = getNodeState(node);
    return stateInfo.state == state;
}

bool ASTStateTracker::isNodeInState(std::shared_ptr<CHTLJSASTNode> node, ASTNodeState state) const {
    if (!node) return false;
    
    auto stateInfo = getNodeState(node);
    return stateInfo.state == state;
}

bool ASTStateTracker::isNodeTracked(std::shared_ptr<CHTLASTNode> node) const {
    if (!node) return false;
    
    void* key = getNodeKey(node);
    return nodeStates_.find(key) != nodeStates_.end();
}

bool ASTStateTracker::isNodeTracked(std::shared_ptr<CHTLJSASTNode> node) const {
    if (!node) return false;
    
    void* key = getNodeKey(node);
    return nodeStates_.find(key) != nodeStates_.end();
}

void ASTStateTracker::markChildrenParsed(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.childrenParsed = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markAttributesParsed(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.attributesParsed = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markContentParsed(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.contentParsed = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markConstraintsChecked(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.constraintsChecked = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markSyntaxValidated(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.syntaxValidated = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markSemanticsValidated(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.semanticsValidated = true;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
    }
}

void ASTStateTracker::markNodeError(std::shared_ptr<CHTLASTNode> node, const String& errorMessage) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.state = ASTNodeState::ERROR_STATE;
        it->second.errorMessage = errorMessage;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

void ASTStateTracker::markNodeError(std::shared_ptr<CHTLJSASTNode> node, const String& errorMessage) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.state = ASTNodeState::ERROR_STATE;
        it->second.errorMessage = errorMessage;
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

void ASTStateTracker::clearNodeError(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.errorMessage.clear();
        if (it->second.state == ASTNodeState::ERROR_STATE) {
            it->second.state = ASTNodeState::CREATED;
        }
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

void ASTStateTracker::clearNodeError(std::shared_ptr<CHTLJSASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    auto it = nodeStates_.find(key);
    if (it != nodeStates_.end()) {
        it->second.errorMessage.clear();
        if (it->second.state == ASTNodeState::ERROR_STATE) {
            it->second.state = ASTNodeState::CREATED;
        }
        it->second.lastUpdateTime = ASTStateInfo::getCurrentTimeMs();
        updateStatistics();
    }
}

bool ASTStateTracker::hasNodeError(std::shared_ptr<CHTLASTNode> node) const {
    if (!node) return false;
    
    auto stateInfo = getNodeState(node);
    return !stateInfo.errorMessage.empty() || stateInfo.state == ASTNodeState::ERROR_STATE;
}

bool ASTStateTracker::hasNodeError(std::shared_ptr<CHTLJSASTNode> node) const {
    if (!node) return false;
    
    auto stateInfo = getNodeState(node);
    return !stateInfo.errorMessage.empty() || stateInfo.state == ASTNodeState::ERROR_STATE;
}

String ASTStateTracker::getNodeError(std::shared_ptr<CHTLASTNode> node) const {
    if (!node) return "";
    
    auto stateInfo = getNodeState(node);
    return stateInfo.errorMessage;
}

String ASTStateTracker::getNodeError(std::shared_ptr<CHTLJSASTNode> node) const {
    if (!node) return "";
    
    auto stateInfo = getNodeState(node);
    return stateInfo.errorMessage;
}

void ASTStateTracker::removeNode(std::shared_ptr<CHTLASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    nodeStates_.erase(key);
    updateStatistics();
}

void ASTStateTracker::removeNode(std::shared_ptr<CHTLJSASTNode> node) {
    if (!node) return;
    
    void* key = getNodeKey(node);
    nodeStates_.erase(key);
    updateStatistics();
}

void ASTStateTracker::clearAllNodes() {
    nodeStates_.clear();
    statistics_ = Statistics();
}

ASTStateTracker::Statistics ASTStateTracker::getStatistics() const {
    return statistics_;
}

void ASTStateTracker::resetStatistics() {
    statistics_ = Statistics();
}

String ASTStateTracker::dumpNodeState(std::shared_ptr<CHTLASTNode> node) const {
    if (!node) return "Node is null";
    
    auto stateInfo = getNodeState(node);
    std::ostringstream oss;
    oss << "Node State: " << stateToString(stateInfo.state) << "\n"
        << "Type State: " << typeStateToString(stateInfo.typeState) << "\n"
        << "Context: " << stateInfo.context << "\n"
        << "Location: " << stateInfo.filename << ":" << stateInfo.line << ":" << stateInfo.column << "\n"
        << "Children Parsed: " << (stateInfo.childrenParsed ? "Yes" : "No") << "\n"
        << "Attributes Parsed: " << (stateInfo.attributesParsed ? "Yes" : "No") << "\n"
        << "Content Parsed: " << (stateInfo.contentParsed ? "Yes" : "No") << "\n"
        << "Constraints Checked: " << (stateInfo.constraintsChecked ? "Yes" : "No") << "\n"
        << "Syntax Validated: " << (stateInfo.syntaxValidated ? "Yes" : "No") << "\n"
        << "Semantics Validated: " << (stateInfo.semanticsValidated ? "Yes" : "No");
    
    if (!stateInfo.errorMessage.empty()) {
        oss << "\nError: " << stateInfo.errorMessage;
    }
    
    return oss.str();
}

String ASTStateTracker::dumpAllStates() const {
    std::ostringstream oss;
    oss << "AST State Tracker - Total Nodes: " << nodeStates_.size() << "\n\n";
    
    for (const auto& pair : nodeStates_) {
        const auto& stateInfo = pair.second;
        oss << "Node " << pair.first << ":\n"
            << "  State: " << stateToString(stateInfo.state) << "\n"
            << "  Type State: " << typeStateToString(stateInfo.typeState) << "\n"
            << "  Context: " << stateInfo.context << "\n"
            << "  Location: " << stateInfo.filename << ":" << stateInfo.line << ":" << stateInfo.column << "\n";
        
        if (!stateInfo.errorMessage.empty()) {
            oss << "  Error: " << stateInfo.errorMessage << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

void* ASTStateTracker::getNodeKey(std::shared_ptr<CHTLASTNode> node) const {
    return node.get();
}

void* ASTStateTracker::getNodeKey(std::shared_ptr<CHTLJSASTNode> node) const {
    return node.get();
}

void ASTStateTracker::updateStatistics() {
    statistics_.totalNodes = nodeStates_.size();
    statistics_.nodesInError = 0;
    statistics_.nodesCompleted = 0;
    statistics_.nodesParsing = 0;
    statistics_.stateDistribution.clear();
    statistics_.typeStateDistribution.clear();
    
    for (const auto& pair : nodeStates_) {
        const auto& stateInfo = pair.second;
        statistics_.stateDistribution[stateInfo.state]++;
        statistics_.typeStateDistribution[stateInfo.typeState]++;
        
        if (stateInfo.state == ASTNodeState::ERROR_STATE) {
            statistics_.nodesInError++;
        } else if (stateInfo.state == ASTNodeState::GENERATED) {
            statistics_.nodesCompleted++;
        } else if (stateInfo.state == ASTNodeState::PARSING) {
            statistics_.nodesParsing++;
        }
    }
}

String ASTStateTracker::stateToString(ASTNodeState state) const {
    switch (state) {
        case ASTNodeState::CREATED: return "CREATED";
        case ASTNodeState::PARSING: return "PARSING";
        case ASTNodeState::PARSED: return "PARSED";
        case ASTNodeState::VALIDATING: return "VALIDATING";
        case ASTNodeState::VALIDATED: return "VALIDATED";
        case ASTNodeState::GENERATING: return "GENERATING";
        case ASTNodeState::GENERATED: return "GENERATED";
        case ASTNodeState::ERROR_STATE: return "ERROR_STATE";
        case ASTNodeState::DISPOSED: return "DISPOSED";
        default: return "UNKNOWN";
    }
}

String ASTStateTracker::typeStateToString(ASTTypeState typeState) const {
    switch (typeState) {
        case ASTTypeState::ELEMENT_PARSING: return "ELEMENT_PARSING";
        case ASTTypeState::ATTRIBUTE_PARSING: return "ATTRIBUTE_PARSING";
        case ASTTypeState::TEXT_PARSING: return "TEXT_PARSING";
        case ASTTypeState::STYLE_PARSING: return "STYLE_PARSING";
        case ASTTypeState::SCRIPT_PARSING: return "SCRIPT_PARSING";
        case ASTTypeState::TEMPLATE_PARSING: return "TEMPLATE_PARSING";
        case ASTTypeState::CUSTOM_PARSING: return "CUSTOM_PARSING";
        case ASTTypeState::IMPORT_PARSING: return "IMPORT_PARSING";
        case ASTTypeState::NAMESPACE_PARSING: return "NAMESPACE_PARSING";
        case ASTTypeState::ORIGIN_PARSING: return "ORIGIN_PARSING";
        case ASTTypeState::CONSTRAINT_PARSING: return "CONSTRAINT_PARSING";
        case ASTTypeState::CHTLJS_PARSING: return "CHTLJS_PARSING";
        default: return "UNKNOWN";
    }
}

// ASTStateGuard 实现
ASTStateGuard::ASTStateGuard(ASTStateTracker& tracker, std::shared_ptr<CHTLASTNode> node, 
                           ASTNodeState targetState, const String& context)
    : tracker_(tracker), chtlNode_(node), chtlJSNode_(nullptr), 
      targetState_(targetState), isSuccess_(false), hasError_(false) {
    
    if (chtlNode_) {
        if (tracker_.isNodeTracked(chtlNode_)) {
            originalState_ = tracker_.getNodeState(chtlNode_).state;
        } else {
            originalState_ = ASTNodeState::CREATED;
            ASTStateInfo stateInfo(originalState_, ASTTypeState::ELEMENT_PARSING, context);
            tracker_.trackNode(chtlNode_, stateInfo);
        }
        tracker_.updateNodeState(chtlNode_, targetState_);
    }
}

ASTStateGuard::ASTStateGuard(ASTStateTracker& tracker, std::shared_ptr<CHTLJSASTNode> node, 
                           ASTNodeState targetState, const String& context)
    : tracker_(tracker), chtlNode_(nullptr), chtlJSNode_(node), 
      targetState_(targetState), isSuccess_(false), hasError_(false) {
    
    if (chtlJSNode_) {
        if (tracker_.isNodeTracked(chtlJSNode_)) {
            originalState_ = tracker_.getNodeState(chtlJSNode_).state;
        } else {
            originalState_ = ASTNodeState::CREATED;
            ASTStateInfo stateInfo(originalState_, ASTTypeState::CHTLJS_PARSING, context);
            tracker_.trackNode(chtlJSNode_, stateInfo);
        }
        tracker_.updateNodeState(chtlJSNode_, targetState_);
    }
}

ASTStateGuard::~ASTStateGuard() {
    if (hasError_) {
        // 保持错误状态
        return;
    }
    
    if (isSuccess_) {
        // 成功完成，更新到下一个状态
        ASTNodeState nextState = targetState_;
        switch (targetState_) {
            case ASTNodeState::PARSING:
                nextState = ASTNodeState::PARSED;
                break;
            case ASTNodeState::VALIDATING:
                nextState = ASTNodeState::VALIDATED;
                break;
            case ASTNodeState::GENERATING:
                nextState = ASTNodeState::GENERATED;
                break;
            default:
                break;
        }
        
        if (chtlNode_) {
            tracker_.updateNodeState(chtlNode_, nextState);
        } else if (chtlJSNode_) {
            tracker_.updateNodeState(chtlJSNode_, nextState);
        }
    } else {
        // 未明确标记成功，恢复原状态
        if (chtlNode_) {
            tracker_.updateNodeState(chtlNode_, originalState_);
        } else if (chtlJSNode_) {
            tracker_.updateNodeState(chtlJSNode_, originalState_);
        }
    }
}

void ASTStateGuard::updateState(ASTNodeState newState) {
    if (chtlNode_) {
        tracker_.updateNodeState(chtlNode_, newState);
    } else if (chtlJSNode_) {
        tracker_.updateNodeState(chtlJSNode_, newState);
    }
}

void ASTStateGuard::markSuccess() {
    isSuccess_ = true;
}

void ASTStateGuard::markError(const String& errorMessage) {
    hasError_ = true;
    if (chtlNode_) {
        tracker_.markNodeError(chtlNode_, errorMessage);
    } else if (chtlJSNode_) {
        tracker_.markNodeError(chtlJSNode_, errorMessage);
    }
}

ASTNodeState ASTStateGuard::getCurrentState() const {
    if (chtlNode_) {
        return tracker_.getNodeState(chtlNode_).state;
    } else if (chtlJSNode_) {
        return tracker_.getNodeState(chtlJSNode_).state;
    }
    return ASTNodeState::CREATED;
}

// ASTStateUtils 实现
namespace ASTStateUtils {

String nodeStateToString(ASTNodeState state) {
    switch (state) {
        case ASTNodeState::CREATED: return "CREATED";
        case ASTNodeState::PARSING: return "PARSING";
        case ASTNodeState::PARSED: return "PARSED";
        case ASTNodeState::VALIDATING: return "VALIDATING";
        case ASTNodeState::VALIDATED: return "VALIDATED";
        case ASTNodeState::GENERATING: return "GENERATING";
        case ASTNodeState::GENERATED: return "GENERATED";
        case ASTNodeState::ERROR_STATE: return "ERROR_STATE";
        case ASTNodeState::DISPOSED: return "DISPOSED";
        default: return "UNKNOWN";
    }
}

String typeStateToString(ASTTypeState state) {
    switch (state) {
        case ASTTypeState::ELEMENT_PARSING: return "ELEMENT_PARSING";
        case ASTTypeState::ATTRIBUTE_PARSING: return "ATTRIBUTE_PARSING";
        case ASTTypeState::TEXT_PARSING: return "TEXT_PARSING";
        case ASTTypeState::STYLE_PARSING: return "STYLE_PARSING";
        case ASTTypeState::SCRIPT_PARSING: return "SCRIPT_PARSING";
        case ASTTypeState::TEMPLATE_PARSING: return "TEMPLATE_PARSING";
        case ASTTypeState::CUSTOM_PARSING: return "CUSTOM_PARSING";
        case ASTTypeState::IMPORT_PARSING: return "IMPORT_PARSING";
        case ASTTypeState::NAMESPACE_PARSING: return "NAMESPACE_PARSING";
        case ASTTypeState::ORIGIN_PARSING: return "ORIGIN_PARSING";
        case ASTTypeState::CONSTRAINT_PARSING: return "CONSTRAINT_PARSING";
        case ASTTypeState::CHTLJS_PARSING: return "CHTLJS_PARSING";
        default: return "UNKNOWN";
    }
}

bool isValidStateTransition(ASTNodeState from, ASTNodeState to) {
    // 定义有效的状态转换
    switch (from) {
        case ASTNodeState::CREATED:
            return to == ASTNodeState::PARSING || to == ASTNodeState::ERROR_STATE || to == ASTNodeState::DISPOSED;
        case ASTNodeState::PARSING:
            return to == ASTNodeState::PARSED || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::PARSED:
            return to == ASTNodeState::VALIDATING || to == ASTNodeState::GENERATING || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::VALIDATING:
            return to == ASTNodeState::VALIDATED || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::VALIDATED:
            return to == ASTNodeState::GENERATING || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::GENERATING:
            return to == ASTNodeState::GENERATED || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::GENERATED:
            return to == ASTNodeState::DISPOSED || to == ASTNodeState::ERROR_STATE;
        case ASTNodeState::ERROR_STATE:
            return to == ASTNodeState::CREATED || to == ASTNodeState::DISPOSED;
        case ASTNodeState::DISPOSED:
            return false; // 终止状态
        default:
            return false;
    }
}

ASTTypeState getDefaultTypeState(ASTNodeType nodeType) {
    switch (nodeType) {
        case ASTNodeType::ELEMENT:
            return ASTTypeState::ELEMENT_PARSING;
        case ASTNodeType::ATTRIBUTE:
            return ASTTypeState::ATTRIBUTE_PARSING;
        case ASTNodeType::TEXT_NODE:
            return ASTTypeState::TEXT_PARSING;
        case ASTNodeType::STYLE_BLOCK:
        case ASTNodeType::CSS_RULE:
        case ASTNodeType::CSS_PROPERTY:
            return ASTTypeState::STYLE_PARSING;
        case ASTNodeType::SCRIPT_BLOCK:
            return ASTTypeState::SCRIPT_PARSING;
        case ASTNodeType::TEMPLATE_STYLE:
        case ASTNodeType::TEMPLATE_ELEMENT:
        case ASTNodeType::TEMPLATE_VAR:
            return ASTTypeState::TEMPLATE_PARSING;
        case ASTNodeType::CUSTOM_STYLE:
        case ASTNodeType::CUSTOM_ELEMENT:
        case ASTNodeType::CUSTOM_VAR:
            return ASTTypeState::CUSTOM_PARSING;
        case ASTNodeType::IMPORT_STATEMENT:
            return ASTTypeState::IMPORT_PARSING;
        case ASTNodeType::NAMESPACE_DECLARATION:
            return ASTTypeState::NAMESPACE_PARSING;
        case ASTNodeType::ENHANCED_SELECTOR:
        case ASTNodeType::EVENT_LISTENER:
        case ASTNodeType::ANIMATION:
        case ASTNodeType::VIRTUAL_OBJECT:
            return ASTTypeState::CHTLJS_PARSING;
        default:
            return ASTTypeState::ELEMENT_PARSING;
    }
}

bool isTerminalState(ASTNodeState state) {
    return state == ASTNodeState::GENERATED || state == ASTNodeState::DISPOSED;
}

bool isErrorState(ASTNodeState state) {
    return state == ASTNodeState::ERROR_STATE;
}

} // namespace ASTStateUtils

} // namespace chtl