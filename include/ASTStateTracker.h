#pragma once

#include "CHTLCommon.h"
#include "CHTLASTNodes.h"
#include "CHTLJSASTNodes.h"
#include "StateMachine.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace chtl {

// AST节点状态
enum class ASTNodeState {
    CREATED,            // 已创建
    PARSING,            // 解析中
    PARSED,             // 解析完成
    VALIDATING,         // 验证中
    VALIDATED,          // 验证完成
    GENERATING,         // 生成中
    GENERATED,          // 生成完成
    ERROR_STATE,        // 错误状态
    DISPOSED            // 已释放
};

// AST节点类型状态
enum class ASTTypeState {
    ELEMENT_PARSING,        // 元素解析状态
    ATTRIBUTE_PARSING,      // 属性解析状态
    TEXT_PARSING,           // 文本解析状态
    STYLE_PARSING,          // 样式解析状态
    SCRIPT_PARSING,         // 脚本解析状态
    TEMPLATE_PARSING,       // 模板解析状态
    CUSTOM_PARSING,         // 自定义解析状态
    IMPORT_PARSING,         // 导入解析状态
    NAMESPACE_PARSING,      // 命名空间解析状态
    ORIGIN_PARSING,         // 原始嵌入解析状态
    CONSTRAINT_PARSING,     // 约束解析状态
    CHTLJS_PARSING          // CHTL JS解析状态
};

// AST节点状态信息
struct ASTStateInfo {
    ASTNodeState state;
    ASTTypeState typeState;
    String context;
    String errorMessage;
    size_t creationTime;
    size_t lastUpdateTime;
    size_t line;
    size_t column;
    String filename;
    
    // 解析进度信息
    bool hasChildren = false;
    bool childrenParsed = false;
    bool attributesParsed = false;
    bool contentParsed = false;
    
    // 验证状态
    bool constraintsChecked = false;
    bool syntaxValidated = false;
    bool semanticsValidated = false;
    
    ASTStateInfo(ASTNodeState s = ASTNodeState::CREATED, 
                 ASTTypeState ts = ASTTypeState::ELEMENT_PARSING,
                 const String& ctx = "",
                 size_t l = 0, size_t c = 0, const String& file = "")
        : state(s), typeState(ts), context(ctx), line(l), column(c), filename(file) {
        creationTime = lastUpdateTime = getCurrentTimeMs();
    }
    
    static size_t getCurrentTimeMs();
};

// AST节点状态跟踪器
class ASTStateTracker {
public:
    ASTStateTracker(StateMachine& stateMachine);
    ~ASTStateTracker() = default;
    
    // 节点状态管理
    void trackNode(std::shared_ptr<CHTLASTNode> node, const ASTStateInfo& stateInfo);
    void trackNode(std::shared_ptr<CHTLJSASTNode> node, const ASTStateInfo& stateInfo);
    void updateNodeState(std::shared_ptr<CHTLASTNode> node, ASTNodeState state);
    void updateNodeState(std::shared_ptr<CHTLJSASTNode> node, ASTNodeState state);
    void updateNodeTypeState(std::shared_ptr<CHTLASTNode> node, ASTTypeState typeState);
    void updateNodeTypeState(std::shared_ptr<CHTLJSASTNode> node, ASTTypeState typeState);
    
    // 状态查询
    ASTStateInfo getNodeState(std::shared_ptr<CHTLASTNode> node) const;
    ASTStateInfo getNodeState(std::shared_ptr<CHTLJSASTNode> node) const;
    bool isNodeInState(std::shared_ptr<CHTLASTNode> node, ASTNodeState state) const;
    bool isNodeInState(std::shared_ptr<CHTLJSASTNode> node, ASTNodeState state) const;
    bool isNodeTracked(std::shared_ptr<CHTLASTNode> node) const;
    bool isNodeTracked(std::shared_ptr<CHTLJSASTNode> node) const;
    
    // 进度管理
    void markChildrenParsed(std::shared_ptr<CHTLASTNode> node);
    void markAttributesParsed(std::shared_ptr<CHTLASTNode> node);
    void markContentParsed(std::shared_ptr<CHTLASTNode> node);
    void markConstraintsChecked(std::shared_ptr<CHTLASTNode> node);
    void markSyntaxValidated(std::shared_ptr<CHTLASTNode> node);
    void markSemanticsValidated(std::shared_ptr<CHTLASTNode> node);
    
    // 错误处理
    void markNodeError(std::shared_ptr<CHTLASTNode> node, const String& errorMessage);
    void markNodeError(std::shared_ptr<CHTLJSASTNode> node, const String& errorMessage);
    void clearNodeError(std::shared_ptr<CHTLASTNode> node);
    void clearNodeError(std::shared_ptr<CHTLJSASTNode> node);
    bool hasNodeError(std::shared_ptr<CHTLASTNode> node) const;
    bool hasNodeError(std::shared_ptr<CHTLJSASTNode> node) const;
    String getNodeError(std::shared_ptr<CHTLASTNode> node) const;
    String getNodeError(std::shared_ptr<CHTLJSASTNode> node) const;
    
    // 批量操作
    void markAllChildrenState(std::shared_ptr<CHTLASTNode> parent, ASTNodeState state);
    void validateAllNodes();
    std::vector<std::shared_ptr<CHTLASTNode>> getNodesInState(ASTNodeState state) const;
    std::vector<std::shared_ptr<CHTLASTNode>> getNodesWithErrors() const;
    
    // 清理和维护
    void removeNode(std::shared_ptr<CHTLASTNode> node);
    void removeNode(std::shared_ptr<CHTLJSASTNode> node);
    void clearAllNodes();
    void cleanupDisposedNodes();
    
    // 统计信息
    struct Statistics {
        size_t totalNodes = 0;
        size_t nodesInError = 0;
        size_t nodesCompleted = 0;
        size_t nodesParsing = 0;
        size_t averageParseTime = 0;
        std::unordered_map<ASTNodeState, size_t> stateDistribution;
        std::unordered_map<ASTTypeState, size_t> typeStateDistribution;
    };
    
    Statistics getStatistics() const;
    void resetStatistics();
    
    // 调试功能
    String dumpNodeState(std::shared_ptr<CHTLASTNode> node) const;
    String dumpAllStates() const;
    void printStateHierarchy(std::shared_ptr<CHTLASTNode> root) const;
    
private:
    StateMachine& stateMachine_;
    std::unordered_map<void*, ASTStateInfo> nodeStates_;
    Statistics statistics_;
    
    // 辅助方法
    void* getNodeKey(std::shared_ptr<CHTLASTNode> node) const;
    void* getNodeKey(std::shared_ptr<CHTLJSASTNode> node) const;
    void updateStatistics();
    String stateToString(ASTNodeState state) const;
    String typeStateToString(ASTTypeState typeState) const;
};

// RAII AST状态守卫
class ASTStateGuard {
public:
    ASTStateGuard(ASTStateTracker& tracker, std::shared_ptr<CHTLASTNode> node, 
                  ASTNodeState targetState, const String& context = "");
    ASTStateGuard(ASTStateTracker& tracker, std::shared_ptr<CHTLJSASTNode> node, 
                  ASTNodeState targetState, const String& context = "");
    ~ASTStateGuard();
    
    // 禁止拷贝和移动
    ASTStateGuard(const ASTStateGuard&) = delete;
    ASTStateGuard& operator=(const ASTStateGuard&) = delete;
    ASTStateGuard(ASTStateGuard&&) = delete;
    ASTStateGuard& operator=(ASTStateGuard&&) = delete;
    
    // 状态更新
    void updateState(ASTNodeState newState);
    void updateContext(const String& context);
    void markSuccess();
    void markError(const String& errorMessage);
    
    // 获取状态
    ASTNodeState getCurrentState() const;
    String getCurrentContext() const;
    bool hasError() const;
    
private:
    ASTStateTracker& tracker_;
    std::shared_ptr<CHTLASTNode> chtlNode_;
    std::shared_ptr<CHTLJSASTNode> chtlJSNode_;
    ASTNodeState originalState_;
    ASTNodeState targetState_;
    bool isSuccess_;
    bool hasError_;
};

// 便利宏定义
#define CHTL_AST_STATE_GUARD(tracker, node, state, context) \
    ASTStateGuard __ast_guard(tracker, node, state, context)

#define CHTL_AST_PARSING_GUARD(tracker, node) \
    ASTStateGuard __parse_guard(tracker, node, ASTNodeState::PARSING, "parsing")

#define CHTL_AST_VALIDATION_GUARD(tracker, node) \
    ASTStateGuard __validate_guard(tracker, node, ASTNodeState::VALIDATING, "validating")

#define CHTL_AST_GENERATION_GUARD(tracker, node) \
    ASTStateGuard __generate_guard(tracker, node, ASTNodeState::GENERATING, "generating")

// 状态工具函数
namespace ASTStateUtils {
    String nodeStateToString(ASTNodeState state);
    String typeStateToString(ASTTypeState state);
    bool isValidStateTransition(ASTNodeState from, ASTNodeState to);
    ASTTypeState getDefaultTypeState(ASTNodeType nodeType);
    bool isTerminalState(ASTNodeState state);
    bool isErrorState(ASTNodeState state);
}

} // namespace chtl