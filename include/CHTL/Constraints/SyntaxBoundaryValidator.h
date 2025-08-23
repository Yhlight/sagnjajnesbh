#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include "CHTL/AST/CHTLASTNodes.h"

namespace CHTL {
namespace Constraints {

/**
 * @brief 语法上下文类型
 */
enum class SyntaxBoundaryContext {
    GLOBAL_STYLE,           // 全局样式块
    LOCAL_STYLE,            // 局部样式块
    LOCAL_SCRIPT,           // 局部脚本块
    GLOBAL_SCRIPT,          // 全局脚本块（除局部script外的其他script）
    NAMESPACE_BODY,         // 命名空间内部
    TEMPLATE_BODY,          // 模板内部
    CUSTOM_BODY,            // 自定义内部
    ORIGIN_BODY,            // 原始嵌入内部
    ROOT_LEVEL              // 根级别
};

/**
 * @brief 语法边界违规类型
 */
enum class BoundaryViolationType {
    FORBIDDEN_SYNTAX,       // 禁用的语法
    INVALID_CONTEXT,        // 无效的上下文
    MISSING_PERMISSION,     // 缺少权限
    SCOPE_VIOLATION         // 作用域违规
};

/**
 * @brief 语法边界违规信息
 */
struct BoundaryViolation {
    BoundaryViolationType type;
    SyntaxBoundaryContext context;
    std::string nodeName;
    std::string description;
    
    BoundaryViolation() : type(BoundaryViolationType::FORBIDDEN_SYNTAX), 
                         context(SyntaxBoundaryContext::ROOT_LEVEL) {}
};

/**
 * @brief 语法边界验证器
 * 严格按照目标规划.ini第137-143行要求限制不同上下文中的语法使用
 */
class SyntaxBoundaryValidator {
public:
    SyntaxBoundaryValidator();
    
    /**
     * @brief 验证节点是否在指定上下文中有效
     * @param node AST节点
     * @param context 语法上下文
     * @return 验证结果
     */
    bool ValidateNodeInContext(std::shared_ptr<AST::ASTNode> node, 
                              SyntaxBoundaryContext context);
    
    /**
     * @brief 获取最后一次验证的违规信息
     * @return 违规信息列表
     */
    const std::vector<BoundaryViolation>& GetViolations() const { return violations_; }
    
    /**
     * @brief 清除违规记录
     */
    void ClearViolations() { violations_.clear(); }
    
    /**
     * @brief 检查节点类型是否在上下文中被允许
     * @param nodeType 节点类型
     * @param context 语法上下文
     * @return 是否被允许
     */
    bool IsNodeTypeAllowed(AST::NodeType nodeType, SyntaxBoundaryContext context);
    
    /**
     * @brief 获取上下文允许的节点类型集合
     * @param context 语法上下文
     * @return 允许的节点类型集合
     */
    const std::unordered_set<AST::NodeType>& GetAllowedNodeTypes(SyntaxBoundaryContext context);

private:
    std::vector<BoundaryViolation> violations_;
    
    // 各上下文允许的节点类型
    std::unordered_set<AST::NodeType> globalStyleAllowed_;
    std::unordered_set<AST::NodeType> localStyleAllowed_;
    std::unordered_set<AST::NodeType> localScriptAllowed_;
    std::unordered_set<AST::NodeType> globalScriptAllowed_;
    std::unordered_set<AST::NodeType> alwaysAllowed_;       // 任意地方都可以写的
    
    /**
     * @brief 初始化各上下文的允许语法
     */
    void InitializeContextPermissions();
    
    /**
     * @brief 记录违规信息
     * @param violation 违规信息
     */
    void RecordViolation(const BoundaryViolation& violation);
    
    /**
     * @brief 检查节点是否为生成器注释
     * @param node 节点
     * @return 是否为生成器注释
     */
    bool IsGeneratorComment(std::shared_ptr<AST::ASTNode> node);
    
    /**
     * @brief 检查节点是否为原始嵌入
     * @param node 节点
     * @return 是否为原始嵌入
     */
    bool IsOriginEmbedding(std::shared_ptr<AST::ASTNode> node);
    
    /**
     * @brief 检查节点是否为CHTL JS特供语法
     * @param node 节点
     * @return 是否为特供语法
     */
    bool IsCHTLJSNativeSyntax(std::shared_ptr<AST::ASTNode> node);
    
    /**
     * @brief 获取节点类型的名称
     * @param nodeType 节点类型
     * @return 类型名称
     */
    std::string GetNodeTypeName(AST::NodeType nodeType);
};

} // namespace Constraints
} // namespace CHTL