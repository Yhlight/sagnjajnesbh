#pragma once
#include "../AST/CHTLNodes.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace chtl {
namespace constraint_system {

// 约束类型
enum class ConstraintType {
    PRECISE,    // 精确约束 - 特定元素或对象
    TYPE,       // 类型约束 - @Html, [Custom], [Template]等
    GLOBAL      // 全局约束 - 在命名空间内的全局约束
};

// 约束目标类型
enum class ConstraintTargetType {
    HTML_ELEMENT,     // HTML元素 (span, div等)
    CUSTOM_ELEMENT,   // [Custom] @Element
    TEMPLATE_ELEMENT, // [Template] @Element
    CUSTOM_STYLE,     // [Custom] @Style
    TEMPLATE_STYLE,   // [Template] @Style
    CUSTOM_VAR,       // [Custom] @Var
    TEMPLATE_VAR,     // [Template] @Var
    HTML_TYPE,        // @Html类型
    CUSTOM_TYPE,      // [Custom]类型
    TEMPLATE_TYPE,    // [Template]类型
    ORIGIN_TYPE       // [Origin]类型
};

// 约束规则
struct ConstraintRule {
    ConstraintType type;
    std::string scope; // 约束作用域（命名空间或元素）
    std::vector<std::string> targets; // 约束目标
    std::vector<ConstraintTargetType> targetTypes; // 约束目标类型
    bool isActive;
};

// 约束管理器
class ConstraintManager {
public:
    ConstraintManager();
    ~ConstraintManager();
    
    // 约束规则管理
    void addConstraintRule(const ConstraintRule& rule);
    void removeConstraintRule(const std::string& scope, const std::string& target);
    bool hasConstraintRule(const std::string& scope, const std::string& target) const;
    
    // 精确约束
    void addPreciseConstraint(const std::string& scope, const std::vector<std::string>& targets);
    bool isPreciselyConstrained(const std::string& scope, const std::string& target) const;
    
    // 类型约束
    void addTypeConstraint(const std::string& scope, ConstraintTargetType targetType);
    bool isTypeConstrained(const std::string& scope, ConstraintTargetType targetType) const;
    
    // 全局约束
    void addGlobalConstraint(const std::string& namespace_, ConstraintTargetType targetType);
    bool isGloballyConstrained(const std::string& namespace_, ConstraintTargetType targetType) const;
    
    // 约束验证
    bool validateElementUsage(const std::string& scope, const std::string& elementName, const std::string& elementType = "");
    bool validateCustomUsage(const std::string& scope, const std::string& customName, const std::string& customType);
    bool validateTemplateUsage(const std::string& scope, const std::string& templateName, const std::string& templateType);
    
    // 约束检查
    std::vector<std::string> checkConstraintViolations(const std::string& scope, 
                                                       const std::vector<std::string>& usedElements,
                                                       const std::vector<std::string>& usedCustoms,
                                                       const std::vector<std::string>& usedTemplates);
    
    // 约束解析
    bool parseConstraintStatement(const std::string& statement, const std::string& scope);
    
    // 约束范围管理
    void enterScope(const std::string& scope);
    void exitScope(const std::string& scope);
    std::string getCurrentScope() const;
    
    // 错误处理
    std::vector<std::string> getConstraintErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
    // 清理
    void clear();
    void clearScope(const std::string& scope);
    
    // 统计
    size_t getConstraintRuleCount() const { return constraintRules_.size(); }
    std::unordered_map<ConstraintType, int> getConstraintStatistics();

private:
    std::vector<ConstraintRule> constraintRules_;
    std::unordered_map<std::string, std::unordered_set<std::string>> preciseConstraints_; // scope -> targets
    std::unordered_map<std::string, std::unordered_set<ConstraintTargetType>> typeConstraints_; // scope -> types
    std::unordered_map<std::string, std::unordered_set<ConstraintTargetType>> globalConstraints_; // namespace -> types
    
    std::vector<std::string> scopeStack_; // 作用域栈
    std::vector<std::string> errors_;
    
    // 内部方法
    ConstraintTargetType parseTargetType(const std::string& target);
    std::string formatTargetType(ConstraintTargetType type);
    bool isValidConstraintTarget(const std::string& target);
    
    // 约束冲突检测
    bool hasConstraintConflict(const ConstraintRule& newRule);
};

// 全局约束管理器实例
extern std::shared_ptr<ConstraintManager> g_constraintManager;

} // namespace constraint_system
} // namespace chtl