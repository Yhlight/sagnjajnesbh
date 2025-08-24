#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace AST {
    class ASTNode;
    class ElementNode;
    class StyleBlockNode;
    class ScriptBlockNode;
    class NamespaceNode;
}

namespace Constraints {

/**
 * @brief 约束类型枚举
 */
enum class ConstraintType {
    PRECISE,    // 精确约束：禁止特定的HTML元素、自定义与模板对象
    TYPE,       // 类型约束：禁止@Html、[Custom]、[Template]类型
    GLOBAL      // 全局约束：在命名空间内的全局约束
};

/**
 * @brief 约束目标类型
 */
enum class ConstraintTarget {
    HTML_ELEMENT,           // HTML元素
    CUSTOM_ELEMENT,         // 自定义元素
    TEMPLATE_ELEMENT,       // 模板元素
    CUSTOM_STYLE,           // 自定义样式组
    TEMPLATE_STYLE,         // 模板样式组
    CUSTOM_VAR,             // 自定义变量组
    TEMPLATE_VAR,           // 模板变量组
    HTML_TYPE,              // @Html类型
    CUSTOM_TYPE,            // [Custom]类型
    TEMPLATE_TYPE,          // [Template]类型
    
    // 新增：完整的约束目标类型
    CUSTOM_VAR_SPECIALIZATION,     // 自定义变量的特例化
    TEMPLATE_VAR_SPECIALIZATION,   // 模板变量的特例化  
    VALUELESS_STYLE,               // 无值样式组
    CUSTOM_STYLE_SPECIALIZATION,   // 自定义样式组的特例化
    DELETE_PROPERTY,               // delete属性
    DELETE_INHERITANCE,            // delete继承
    STYLE_INHERITANCE,             // 继承(样式组之间的继承)
    GENERATOR_COMMENT,             // 生成器注释
    FULL_QUALIFIED_NAME,           // 全缀名
    NAMESPACE_FROM,                // 命名空间from
    CHTL_JS_SPECIAL_SYNTAX,        // CHTL JS特供语法（{{&}}等）
    
    ORIGIN_EMBED                   // 原始嵌入（总是允许）
};

/**
 * @brief 语法上下文类型
 */
enum class SyntaxContext {
    GLOBAL_STYLE,           // 全局样式块
    LOCAL_STYLE,            // 局部样式块
    GLOBAL_SCRIPT,          // 全局script
    LOCAL_SCRIPT,           // 局部script
    ELEMENT_BODY,           // 元素主体
    NAMESPACE_BODY,         // 命名空间主体
    TEMPLATE_BODY,          // 模板主体
    CUSTOM_BODY             // 自定义主体
};

/**
 * @brief 约束规则定义
 */
struct ConstraintRule {
    ConstraintType type;                        // 约束类型
    ConstraintTarget target;                    // 约束目标
    std::string specificName;                   // 特定名称（用于精确约束）
    std::string description;                    // 约束描述
    bool isActive;                              // 是否激活
};

/**
 * @brief 语法约束验证器
 * 
 * 目标规划.ini第137-143行：
 * - 全局样式块只能允许模板变量、自定义变量、样式组、继承、delete等
 * - 非局部script禁止使用任何CHTL语法
 * - 局部样式块允许特定的CHTL语法
 * - 局部script允许模板变量、自定义变量组、变量组特例化等
 */
class CHTLConstraintValidator {
public:
    /**
     * @brief 构造函数
     */
    CHTLConstraintValidator();
    
    /**
     * @brief 验证AST节点是否符合约束
     * @param node AST节点
     * @param context 语法上下文
     * @return 是否通过验证
     */
    bool ValidateNode(const std::shared_ptr<AST::ASTNode>& node, 
                     SyntaxContext context) const;
    
    /**
     * @brief 添加except约束规则
     * @param context 上下文
     * @param rule 约束规则
     */
    void AddExceptRule(SyntaxContext context, const ConstraintRule& rule);
    
    /**
     * @brief 移除约束规则
     * @param context 上下文
     * @param target 约束目标
     * @param specificName 特定名称（可选）
     */
    void RemoveExceptRule(SyntaxContext context, ConstraintTarget target, 
                         const std::string& specificName = "");
    
    /**
     * @brief 检查是否允许在指定上下文中使用特定语法
     * @param context 语法上下文
     * @param target 约束目标
     * @param specificName 特定名称（可选）
     * @return 是否允许
     */
    bool IsAllowed(SyntaxContext context, ConstraintTarget target, 
                   const std::string& specificName = "") const;
    
    /**
     * @brief 获取约束错误信息
     * @return 最后一次验证的错误信息
     */
    const std::string& GetLastError() const { return lastError_; }
    
    /**
     * @brief 重置验证器状态
     */
    void Reset();

private:
    /**
     * @brief 初始化默认约束规则
     */
    void InitializeDefaultConstraints();
    
    /**
     * @brief 验证全局样式块约束
     * @param node AST节点
     * @return 是否通过验证
     */
    bool ValidateGlobalStyleConstraints(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 验证局部样式块约束
     * @param node AST节点
     * @return 是否通过验证
     */
    bool ValidateLocalStyleConstraints(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 验证全局script约束
     * @param node AST节点
     * @return 是否通过验证
     */
    bool ValidateGlobalScriptConstraints(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 验证局部script约束
     * @param node AST节点
     * @return 是否通过验证
     */
    bool ValidateLocalScriptConstraints(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 验证except约束
     * @param node AST节点
     * @param context 语法上下文
     * @return 是否通过验证
     */
    bool ValidateExceptConstraints(const std::shared_ptr<AST::ASTNode>& node, 
                                  SyntaxContext context) const;
    
    /**
     * @brief 获取节点的约束目标类型
     * @param node AST节点
     * @return 约束目标类型
     */
    ConstraintTarget GetNodeConstraintTarget(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 检查是否为原始嵌入（总是允许）
     * @param node AST节点
     * @return 是否为原始嵌入
     */
    bool IsOriginEmbed(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 检查是否为注释（总是允许）
     * @param node AST节点
     * @return 是否为注释
     */
    bool IsComment(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    void SetError(const std::string& error) const;
    
    /**
     * @brief 检查是否为CHTL JS特供语法
     * @param node AST节点
     * @return 是否为特供语法
     */
    bool IsSpecialCHTLJSSyntax(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 获取节点名称
     * @param node AST节点
     * @return 节点名称
     */
    std::string GetNodeName(const std::shared_ptr<AST::ASTNode>& node) const;
    
    /**
     * @brief 获取约束目标名称
     * @param target 约束目标
     * @return 目标名称
     */
    std::string GetConstraintTargetName(ConstraintTarget target) const;

private:
    // 各上下文的约束规则映射
    std::unordered_map<SyntaxContext, std::vector<ConstraintRule>> contextConstraints_;
    
    // 允许的语法映射（按上下文分类）
    std::unordered_map<SyntaxContext, std::unordered_set<ConstraintTarget>> allowedSyntax_;
    
    // 错误信息
    mutable std::string lastError_;
};

/**
 * @brief 语法上下文检测器
 * 
 * 用于检测当前AST节点所处的语法上下文
 */
class SyntaxContextDetector {
public:
    /**
     * @brief 检测节点的语法上下文
     * @param node AST节点
     * @param parentContext 父节点上下文
     * @return 语法上下文
     */
    static SyntaxContext DetectContext(const std::shared_ptr<AST::ASTNode>& node,
                                      SyntaxContext parentContext = SyntaxContext::ELEMENT_BODY);
    
    /**
     * @brief 检查是否为全局样式块
     * @param node AST节点
     * @return 是否为全局样式块
     */
    static bool IsGlobalStyleBlock(const std::shared_ptr<AST::ASTNode>& node);
    
    /**
     * @brief 检查是否为局部样式块
     * @param node AST节点
     * @return 是否为局部样式块
     */
    static bool IsLocalStyleBlock(const std::shared_ptr<AST::ASTNode>& node);
    
    /**
     * @brief 检查是否为全局script块
     * @param node AST节点
     * @return 是否为全局script块
     */
    static bool IsGlobalScriptBlock(const std::shared_ptr<AST::ASTNode>& node);
    
    /**
     * @brief 检查是否为局部script块
     * @param node AST节点
     * @return 是否为局部script块
     */
    static bool IsLocalScriptBlock(const std::shared_ptr<AST::ASTNode>& node);
};

/**
 * @brief 约束规则构建器
 * 
 * 用于构建各种约束规则
 */
class ConstraintRuleBuilder {
public:
    /**
     * @brief 创建精确约束规则
     * @param target 约束目标
     * @param specificName 特定名称
     * @param description 描述
     * @return 约束规则
     */
    static ConstraintRule CreatePreciseRule(ConstraintTarget target,
                                           const std::string& specificName,
                                           const std::string& description = "");
    
    /**
     * @brief 创建类型约束规则
     * @param target 约束目标
     * @param description 描述
     * @return 约束规则
     */
    static ConstraintRule CreateTypeRule(ConstraintTarget target,
                                        const std::string& description = "");
    
    /**
     * @brief 创建全局约束规则
     * @param target 约束目标
     * @param description 描述
     * @return 约束规则
     */
    static ConstraintRule CreateGlobalRule(ConstraintTarget target,
                                          const std::string& description = "");
};

} // namespace Constraints
} // namespace CHTL