#pragma once

#include "CHTLCommon.h"
#include "ContextManager.h"
#include "ASTStateTracker.h"
#include "CHTLASTNodes.h"
#include "CHTLJSASTNodes.h"
#include <unordered_set>
#include <functional>

namespace chtl {

// 语法约束级别
enum class ConstraintLevel {
    STRICT,         // 严格约束，完全按照文档
    PERMISSIVE,     // 宽松约束，允许一些扩展
    DISABLED        // 禁用约束检查
};

// 语法验证结果
struct ValidationResult {
    bool isValid = true;
    String errorMessage;
    String suggestion;
    size_t line = 0;
    size_t column = 0;
    String context;
    
    ValidationResult() = default;
    ValidationResult(bool valid, const String& error = "", const String& suggest = "")
        : isValid(valid), errorMessage(error), suggestion(suggest) {}
};

// 约束规则
struct ConstraintRule {
    String name;
    String description;
    std::function<ValidationResult(const ContextInfo&, const String&)> validator;
    ConstraintLevel level;
    bool isActive = true;
    
    ConstraintRule() = default;
    ConstraintRule(const String& n, const String& desc, 
                   std::function<ValidationResult(const ContextInfo&, const String&)> val,
                   ConstraintLevel lvl = ConstraintLevel::STRICT)
        : name(n), description(desc), validator(val), level(lvl) {}
};

/**
 * 上下文管理协助器
 * 提供增强的约束检查和语法验证功能
 * 严格根据CHTL语法文档进行验证，不允许语法偏差
 */
class ContextAssistant {
public:
    ContextAssistant(ContextManager& contextManager, ASTStateTracker& astTracker);
    ~ContextAssistant() = default;
    
    // 约束检查
    ValidationResult validateElementUsage(const String& elementName, const ContextInfo& context) const;
    ValidationResult validateStyleBlockUsage(bool isLocal, const ContextInfo& context) const;
    ValidationResult validateScriptBlockUsage(bool isLocal, const ContextInfo& context) const;
    ValidationResult validateTemplateUsage(const String& templateType, const String& templateName, const ContextInfo& context) const;
    ValidationResult validateCustomUsage(const String& customType, const String& customName, const ContextInfo& context) const;
    ValidationResult validateImportUsage(const String& importType, const String& path, const ContextInfo& context) const;
    ValidationResult validateOriginUsage(const String& originType, const ContextInfo& context) const;
    ValidationResult validateCHTLJSUsage(const String& jsFeature, const ContextInfo& context) const;
    
    // 特定语法约束检查
    ValidationResult validateGlobalStyleConstraints(const ContextInfo& context) const;
    ValidationResult validateLocalStyleConstraints(const ContextInfo& context) const;
    ValidationResult validateLocalScriptConstraints(const ContextInfo& context) const;
    ValidationResult validateGlobalScriptConstraints(const ContextInfo& context) const;
    ValidationResult validateNamespaceConstraints(const ContextInfo& context) const;
    
    // except约束验证
    ValidationResult validateExceptConstraints(const String& item, const String& type, const ContextInfo& context) const;
    ValidationResult validatePreciseConstraints(const String& element, const ContextInfo& context) const;
    ValidationResult validateTypeConstraints(const String& type, const ContextInfo& context) const;
    ValidationResult validateGlobalConstraints(const ContextInfo& context) const;
    
    // 继承和特例化验证
    ValidationResult validateInheritance(const String& parentName, const String& childName, const String& type) const;
    ValidationResult validateSpecialization(const String& templateName, const StringMap& specializations) const;
    ValidationResult validateDeletion(const String& itemToDelete, const String& context) const;
    
    // 命名空间验证
    ValidationResult validateNamespaceUsage(const String& namespaceName, const String& item) const;
    ValidationResult validateFullQualifiedName(const String& fullName) const;
    ValidationResult validateFromClause(const String& namespaceName, const String& item) const;
    
    // 变量和模板验证
    ValidationResult validateVariableUsage(const String& groupName, const String& varName, const ContextInfo& context) const;
    ValidationResult validateVariableSpecialization(const String& groupName, const String& varName, const String& value) const;
    ValidationResult validateTemplateInheritance(const String& templateType, const StringList& parents) const;
    
    // CHTL JS特定验证
    ValidationResult validateEnhancedSelector(const String& selector) const;
    ValidationResult validateEventListener(const String& eventType, const String& handler) const;
    ValidationResult validateAnimation(const StringMap& animationProps) const;
    ValidationResult validateVirtualObject(const String& objName, const StringList& methods) const;
    ValidationResult validateChainCall(const String& baseObject, const StringList& calls) const;
    
    // 约束规则管理
    void addConstraintRule(const ConstraintRule& rule);
    void removeConstraintRule(const String& ruleName);
    void enableConstraintRule(const String& ruleName, bool enabled = true);
    void setConstraintLevel(ConstraintLevel level);
    ConstraintLevel getConstraintLevel() const { return constraintLevel_; }
    
    // 批量验证
    std::vector<ValidationResult> validateContext(const ContextInfo& context) const;
    std::vector<ValidationResult> validateASTNode(std::shared_ptr<CHTLASTNode> node) const;
    std::vector<ValidationResult> validateASTNode(std::shared_ptr<CHTLJSASTNode> node) const;
    std::vector<ValidationResult> validateAllNodes() const;
    
    // 自动修复建议
    struct FixSuggestion {
        String description;
        String oldCode;
        String newCode;
        bool isAutoFixable = false;
    };
    
    std::vector<FixSuggestion> getFixSuggestions(const ValidationResult& result) const;
    ValidationResult applyAutoFix(const FixSuggestion& suggestion, String& code) const;
    
    // 统计和报告
    struct ValidationStatistics {
        size_t totalValidations = 0;
        size_t validationErrors = 0;
        size_t validationWarnings = 0;
        size_t constraintViolations = 0;
        size_t autoFixesApplied = 0;
        std::unordered_map<String, size_t> errorsByType;
        std::unordered_map<String, size_t> errorsByContext;
    };
    
    ValidationStatistics getStatistics() const;
    void resetStatistics();
    String generateValidationReport() const;
    
    // 调试功能
    void dumpConstraintRules() const;
    void dumpValidationHistory() const;
    String getConstraintRuleInfo(const String& ruleName) const;
    
private:
    ContextManager& contextManager_;
    ASTStateTracker& astTracker_;
    std::unordered_map<String, ConstraintRule> constraintRules_;
    ConstraintLevel constraintLevel_;
    mutable ValidationStatistics statistics_;
    mutable std::vector<ValidationResult> validationHistory_;
    
    // 内置约束规则初始化
    void initializeBuiltinRules();
    
    // 具体约束检查实现
    ValidationResult checkElementInContext(const String& element, const ContextInfo& context) const;
    ValidationResult checkTypeInContext(const String& type, const ContextInfo& context) const;
    ValidationResult checkCHTLJSInContext(const String& feature, const ContextInfo& context) const;
    
    // 语法文档规则验证
    ValidationResult validateGlobalStyleOnlyRules(const ContextInfo& context) const;
    ValidationResult validateLocalStyleOnlyRules(const ContextInfo& context) const;
    ValidationResult validateLocalScriptOnlyRules(const ContextInfo& context) const;
    ValidationResult validateGlobalScriptOnlyRules(const ContextInfo& context) const;
    
    // 辅助方法
    bool isInGlobalStyleContext(const ContextInfo& context) const;
    bool isInLocalStyleContext(const ContextInfo& context) const;
    bool isInLocalScriptContext(const ContextInfo& context) const;
    bool isInGlobalScriptContext(const ContextInfo& context) const;
    bool isTemplateVariableAllowed(const ContextInfo& context) const;
    bool isCustomVariableAllowed(const ContextInfo& context) const;
    bool isOriginEmbedAllowed(const ContextInfo& context) const;
    
    // 统计更新
    void updateStatistics(const ValidationResult& result, const String& context) const;
    
    // 错误消息生成
    String generateErrorMessage(const String& rule, const String& context, const String& details = "") const;
    String generateSuggestion(const String& rule, const String& context) const;
};

// RAII上下文验证守卫
class ContextValidationGuard {
public:
    ContextValidationGuard(ContextAssistant& assistant, const ContextInfo& context, const String& operation);
    ~ContextValidationGuard();
    
    // 禁止拷贝和移动
    ContextValidationGuard(const ContextValidationGuard&) = delete;
    ContextValidationGuard& operator=(const ContextValidationGuard&) = delete;
    ContextValidationGuard(ContextValidationGuard&&) = delete;
    ContextValidationGuard& operator=(ContextValidationGuard&&) = delete;
    
    // 验证结果
    bool isValid() const { return isValid_; }
    const ValidationResult& getResult() const { return result_; }
    void markSuccess() { isValid_ = true; }
    void markError(const String& error);
    
private:
    ContextAssistant& assistant_;
    ContextInfo context_;
    String operation_;
    ValidationResult result_;
    bool isValid_;
};

// 便利宏定义
#define CHTL_CONTEXT_VALIDATION_GUARD(assistant, context, operation) \
    ContextValidationGuard __ctx_val_guard(assistant, context, operation)

#define CHTL_VALIDATE_ELEMENT(assistant, element, context) \
    do { \
        auto result = assistant.validateElementUsage(element, context); \
        if (!result.isValid) { \
            throw std::runtime_error("Element validation failed: " + result.errorMessage); \
        } \
    } while(0)

#define CHTL_VALIDATE_SYNTAX(assistant, type, name, context) \
    do { \
        ValidationResult result; \
        if (type == "template") result = assistant.validateTemplateUsage("", name, context); \
        else if (type == "custom") result = assistant.validateCustomUsage("", name, context); \
        else if (type == "origin") result = assistant.validateOriginUsage(name, context); \
        if (!result.isValid) { \
            throw std::runtime_error("Syntax validation failed: " + result.errorMessage); \
        } \
    } while(0)

} // namespace chtl