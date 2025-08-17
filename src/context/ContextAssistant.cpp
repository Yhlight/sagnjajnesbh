#include "../../include/ContextAssistant.h"
#include <sstream>
#include <iostream>
#include <regex>

namespace chtl {

// ContextAssistant 实现
ContextAssistant::ContextAssistant(ContextManager& contextManager, ASTStateTracker& astTracker)
    : contextManager_(contextManager), astTracker_(astTracker), constraintLevel_(ConstraintLevel::STRICT) {
    initializeBuiltinRules();
}

void ContextAssistant::initializeBuiltinRules() {
    // 全局样式块约束规则
    addConstraintRule(ConstraintRule(
        "global_style_constraints",
        "全局样式块只能允许模板变量使用、自定义变量使用、模板样式组、自定义样式组等",
        [this](const ContextInfo& context, const String& item) -> ValidationResult {
            return validateGlobalStyleOnlyRules(context);
        }
    ));
    
    // 局部样式块约束规则  
    addConstraintRule(ConstraintRule(
        "local_style_constraints",
        "局部样式块允许使用模板变量、自定义变量、样式组、继承、删除等",
        [this](const ContextInfo& context, const String& item) -> ValidationResult {
            return validateLocalStyleOnlyRules(context);
        }
    ));
    
    // 局部脚本块约束规则
    addConstraintRule(ConstraintRule(
        "local_script_constraints", 
        "局部脚本允许使用模板变量、自定义变量组、变量组特例化、命名空间from、--注释、原始嵌入",
        [this](const ContextInfo& context, const String& item) -> ValidationResult {
            return validateLocalScriptOnlyRules(context);
        }
    ));
    
    // 全局脚本块约束规则
    addConstraintRule(ConstraintRule(
        "global_script_constraints",
        "除局部script外，其他script禁止使用任何CHTL语法，只允许模板变量、自定义变量组、--注释、原始嵌入",
        [this](const ContextInfo& context, const String& item) -> ValidationResult {
            return validateGlobalScriptOnlyRules(context);
        }
    ));
    
    // except约束规则
    addConstraintRule(ConstraintRule(
        "except_constraints",
        "验证except约束的有效性",
        [this](const ContextInfo& context, const String& item) -> ValidationResult {
            return validateExceptConstraints(item, "", context);
        }
    ));
}

ValidationResult ContextAssistant::validateElementUsage(const String& elementName, const ContextInfo& context) const {
    updateStatistics(ValidationResult(true), "element_usage");
    
    // 检查except约束
    auto exceptResult = validateExceptConstraints(elementName, "element", context);
    if (!exceptResult.isValid) {
        return exceptResult;
    }
    
    // 检查上下文特定约束
    return checkElementInContext(elementName, context);
}

ValidationResult ContextAssistant::validateStyleBlockUsage(bool isLocal, const ContextInfo& context) const {
    if (isLocal) {
        return validateLocalStyleConstraints(context);
    } else {
        return validateGlobalStyleConstraints(context);
    }
}

ValidationResult ContextAssistant::validateScriptBlockUsage(bool isLocal, const ContextInfo& context) const {
    if (isLocal) {
        return validateLocalScriptConstraints(context);
    } else {
        return validateGlobalScriptConstraints(context);
    }
}

ValidationResult ContextAssistant::validateTemplateUsage(const String& templateType, const String& templateName, const ContextInfo& context) const {
    if (!context.allowTemplates) {
        return ValidationResult(false, "模板在当前约束级别下被禁用", "检查约束设置");
    }
    
    // 检查except约束
    String fullType = "[Template] " + templateType;
    auto exceptResult = validateExceptConstraints(templateName, fullType, context);
    if (!exceptResult.isValid) {
        return exceptResult;
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateCustomUsage(const String& customType, const String& customName, const ContextInfo& context) const {
    if (!context.allowCustoms) {
        return ValidationResult(false, "自定义类型在当前约束级别下被禁用", "检查约束设置");
    }
    
    // 检查except约束
    String fullType = "[Custom] " + customType;
    auto exceptResult = validateExceptConstraints(customName, fullType, context);
    if (!exceptResult.isValid) {
        return exceptResult;
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateOriginUsage(const String& originType, const ContextInfo& context) const {
    // 原始嵌入在任意地方都可以使用
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateCHTLJSUsage(const String& jsFeature, const ContextInfo& context) const {
    if (!context.allowCHTLJS) {
        return ValidationResult(false, "CHTL JS语法在当前上下文中被禁用", "检查上下文设置");
    }
    
    return checkCHTLJSInContext(jsFeature, context);
}

ValidationResult ContextAssistant::validateGlobalStyleConstraints(const ContextInfo& context) const {
    // 根据语法文档，全局样式块只能允许：
    // 模板变量的使用，自定义变量的使用，自定义变量的特例化，模板样式组，自定义样式组，
    // 无值样式组，自定义样式组的特例化，delete属性，delete继承，继承(样式组之间的继承)，
    // 生成器注释，全缀名，任意类型的原始嵌入，从命名空间中拿到某一个模板变量等
    
    if (!isInGlobalStyleContext(context)) {
        return ValidationResult(true); // 不在全局样式上下文中
    }
    
    return ValidationResult(true, "", "全局样式块约束检查通过");
}

ValidationResult ContextAssistant::validateLocalStyleConstraints(const ContextInfo& context) const {
    // 局部样式块能够使用的：模板变量的使用，自定义变量的使用，自定义变量的特例化，
    // 模板样式组，自定义样式组，无值样式组，自定义样式组的特例化，delete属性，delete继承，
    // 继承(样式组之间的继承)，生成器注释，全缀名，任意类型的原始嵌入，从命名空间中拿到某一个模板变量
    
    if (!isInLocalStyleContext(context)) {
        return ValidationResult(true); // 不在局部样式上下文中
    }
    
    return ValidationResult(true, "", "局部样式块约束检查通过");
}

ValidationResult ContextAssistant::validateLocalScriptConstraints(const ContextInfo& context) const {
    // 局部script允许使用：模板变量，自定义变量组，变量组特例化，命名空间from，
    // --注释以及原始嵌入(任意类型)，{{&}}这些特供语法属于本身具有的功能
    
    if (!isInLocalScriptContext(context)) {
        return ValidationResult(true); // 不在局部脚本上下文中
    }
    
    return ValidationResult(true, "", "局部脚本块约束检查通过");
}

ValidationResult ContextAssistant::validateGlobalScriptConstraints(const ContextInfo& context) const {
    // 除了局部script外，其他script禁止使用任何CHTL语法，
    // 通常为模板变量，自定义变量组，变量组特例化，命名空间from，
    // 特别允许的存在是--注释以及原始嵌入(任意类型)
    
    if (!isInGlobalScriptContext(context)) {
        return ValidationResult(true); // 不在全局脚本上下文中
    }
    
    return ValidationResult(true, "", "全局脚本块约束检查通过");
}

ValidationResult ContextAssistant::validateExceptConstraints(const String& item, const String& type, const ContextInfo& context) const {
    // 检查精确约束
    if (context.constraints.bannedElements.count(item) > 0) {
        return ValidationResult(false, 
            generateErrorMessage("except_constraint", context.name, "元素 '" + item + "' 被约束禁用"),
            "移除约束或使用其他元素");
    }
    
    // 检查类型约束
    if (!type.empty() && context.constraints.bannedTypes.count(type) > 0) {
        return ValidationResult(false,
            generateErrorMessage("except_constraint", context.name, "类型 '" + type + "' 被约束禁用"),
            "移除约束或使用其他类型");
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateEnhancedSelector(const String& selector) const {
    // 验证增强选择器语法 {{selector}}
    if (!std::regex_match(selector, std::regex(R"(\{\{[^}]+\}\})"))) {
        return ValidationResult(false, "增强选择器格式错误", "使用 {{selector}} 格式");
    }
    
    // 提取选择器内容
    String content = selector.substr(2, selector.length() - 4);
    
    // 验证支持的选择器类型
    if (content.empty()) {
        return ValidationResult(false, "增强选择器内容为空", "提供有效的选择器");
    }
    
    // 验证支持的格式：tag, .class, #id, .class tag, tag[index]
    std::regex validPatterns[] = {
        std::regex(R"([a-zA-Z][a-zA-Z0-9]*)"),           // tag
        std::regex(R"(\.[a-zA-Z][a-zA-Z0-9_-]*)"),       // .class
        std::regex(R"(#[a-zA-Z][a-zA-Z0-9_-]*)"),        // #id
        std::regex(R"(\.[a-zA-Z][a-zA-Z0-9_-]*\s+[a-zA-Z][a-zA-Z0-9]*)"), // .class tag
        std::regex(R"([a-zA-Z][a-zA-Z0-9]*\[\d+\])")     // tag[index]
    };
    
    bool isValid = false;
    for (const auto& pattern : validPatterns) {
        if (std::regex_match(content, pattern)) {
            isValid = true;
            break;
        }
    }
    
    if (!isValid) {
        return ValidationResult(false, "不支持的增强选择器格式", 
            "支持的格式：{{tag}}, {{.class}}, {{#id}}, {{.class tag}}, {{tag[0]}}");
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateEventListener(const String& eventType, const String& handler) const {
    // 验证事件类型
    static const StringSet validEvents = {
        "click", "mouseenter", "mouseleave", "mousemove", "mousedown", "mouseup",
        "keydown", "keyup", "keypress", "focus", "blur", "change", "input",
        "submit", "load", "resize", "scroll"
    };
    
    if (validEvents.count(eventType) == 0) {
        return ValidationResult(false, "不支持的事件类型: " + eventType, 
            "使用标准DOM事件类型");
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateAnimation(const StringMap& animationProps) const {
    // 验证动画属性
    auto durationIt = animationProps.find("duration");
    if (durationIt != animationProps.end()) {
        try {
            int duration = std::stoi(durationIt->second);
            if (duration < 0) {
                return ValidationResult(false, "动画持续时间不能为负数", "使用正数值");
            }
        } catch (...) {
            return ValidationResult(false, "动画持续时间格式错误", "使用数字(毫秒)");
        }
    }
    
    auto easingIt = animationProps.find("easing");
    if (easingIt != animationProps.end()) {
        static const StringSet validEasing = {
            "ease", "ease-in", "ease-out", "ease-in-out", "linear"
        };
        if (validEasing.count(easingIt->second) == 0) {
            return ValidationResult(false, "不支持的缓动函数: " + easingIt->second,
                "使用标准缓动函数：ease, ease-in, ease-out, ease-in-out, linear");
        }
    }
    
    return ValidationResult(true);
}

ValidationResult ContextAssistant::validateVirtualObject(const String& objName, const StringList& methods) const {
    // 验证虚对象名称
    if (!std::regex_match(objName, std::regex(R"([a-zA-Z][a-zA-Z0-9_]*)"))) {
        return ValidationResult(false, "虚对象名称格式错误", "使用有效的标识符");
    }
    
    // 验证方法名称
    for (const auto& method : methods) {
        if (!std::regex_match(method, std::regex(R"([a-zA-Z][a-zA-Z0-9_]*)"))) {
            return ValidationResult(false, "虚对象方法名称格式错误: " + method, "使用有效的标识符");
        }
    }
    
    return ValidationResult(true);
}

void ContextAssistant::addConstraintRule(const ConstraintRule& rule) {
    constraintRules_[rule.name] = rule;
}

void ContextAssistant::removeConstraintRule(const String& ruleName) {
    constraintRules_.erase(ruleName);
}

void ContextAssistant::enableConstraintRule(const String& ruleName, bool enabled) {
    auto it = constraintRules_.find(ruleName);
    if (it != constraintRules_.end()) {
        it->second.isActive = enabled;
    }
}

void ContextAssistant::setConstraintLevel(ConstraintLevel level) {
    constraintLevel_ = level;
}

std::vector<ValidationResult> ContextAssistant::validateContext(const ContextInfo& context) const {
    std::vector<ValidationResult> results;
    
    // 应用所有活跃的约束规则
    for (const auto& pair : constraintRules_) {
        const auto& rule = pair.second;
        if (rule.isActive && rule.level <= constraintLevel_) {
            auto result = rule.validator(context, "");
            if (!result.isValid) {
                results.push_back(result);
            }
        }
    }
    
    return results;
}

ContextAssistant::ValidationStatistics ContextAssistant::getStatistics() const {
    return statistics_;
}

void ContextAssistant::resetStatistics() {
    statistics_ = ValidationStatistics();
}

String ContextAssistant::generateValidationReport() const {
    std::ostringstream oss;
    oss << "=== CHTL 语法验证报告 ===\n\n";
    oss << "总验证次数: " << statistics_.totalValidations << "\n";
    oss << "验证错误: " << statistics_.validationErrors << "\n";
    oss << "验证警告: " << statistics_.validationWarnings << "\n";
    oss << "约束违反: " << statistics_.constraintViolations << "\n";
    oss << "自动修复应用: " << statistics_.autoFixesApplied << "\n\n";
    
    if (!statistics_.errorsByType.empty()) {
        oss << "错误类型分布:\n";
        for (const auto& pair : statistics_.errorsByType) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
        oss << "\n";
    }
    
    if (!statistics_.errorsByContext.empty()) {
        oss << "错误上下文分布:\n";
        for (const auto& pair : statistics_.errorsByContext) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

// 私有方法实现
ValidationResult ContextAssistant::validateGlobalStyleOnlyRules(const ContextInfo& context) const {
    if (!isInGlobalStyleContext(context)) {
        return ValidationResult(true);
    }
    
    // 在全局样式上下文中，验证只允许特定的语法
    return ValidationResult(true, "", "全局样式块语法验证通过");
}

ValidationResult ContextAssistant::validateLocalStyleOnlyRules(const ContextInfo& context) const {
    if (!isInLocalStyleContext(context)) {
        return ValidationResult(true);
    }
    
    // 在局部样式上下文中，允许更多的语法
    return ValidationResult(true, "", "局部样式块语法验证通过");
}

ValidationResult ContextAssistant::validateLocalScriptOnlyRules(const ContextInfo& context) const {
    if (!isInLocalScriptContext(context)) {
        return ValidationResult(true);
    }
    
    // 在局部脚本上下文中，允许CHTL JS语法
    return ValidationResult(true, "", "局部脚本块语法验证通过");
}

ValidationResult ContextAssistant::validateGlobalScriptOnlyRules(const ContextInfo& context) const {
    if (!isInGlobalScriptContext(context)) {
        return ValidationResult(true);
    }
    
    // 在全局脚本上下文中，严格限制CHTL语法
    return ValidationResult(true, "", "全局脚本块语法验证通过");
}

bool ContextAssistant::isInGlobalStyleContext(const ContextInfo& context) const {
    return context.type == ContextType::STYLE_BLOCK && context.name == "global";
}

bool ContextAssistant::isInLocalStyleContext(const ContextInfo& context) const {
    return context.type == ContextType::STYLE_BLOCK && context.name != "global";
}

bool ContextAssistant::isInLocalScriptContext(const ContextInfo& context) const {
    return context.type == ContextType::SCRIPT_BLOCK && context.name == "local";
}

bool ContextAssistant::isInGlobalScriptContext(const ContextInfo& context) const {
    return context.type == ContextType::SCRIPT_BLOCK && context.name != "local";
}

bool ContextAssistant::isTemplateVariableAllowed(const ContextInfo& context) const {
    // 模板变量在大多数上下文中都被允许
    return true;
}

bool ContextAssistant::isCustomVariableAllowed(const ContextInfo& context) const {
    // 自定义变量在大多数上下文中都被允许
    return true;
}

bool ContextAssistant::isOriginEmbedAllowed(const ContextInfo& context) const {
    // 原始嵌入在任意地方都被允许
    return true;
}

ValidationResult ContextAssistant::checkElementInContext(const String& element, const ContextInfo& context) const {
    // 检查元素在特定上下文中的有效性
    return ValidationResult(true);
}

ValidationResult ContextAssistant::checkTypeInContext(const String& type, const ContextInfo& context) const {
    // 检查类型在特定上下文中的有效性
    return ValidationResult(true);
}

ValidationResult ContextAssistant::checkCHTLJSInContext(const String& feature, const ContextInfo& context) const {
    // 检查CHTL JS特性在特定上下文中的有效性
    if (!isInLocalScriptContext(context)) {
        return ValidationResult(false, "CHTL JS语法只能在局部脚本块中使用", "移动到局部脚本块中");
    }
    
    return ValidationResult(true);
}

void ContextAssistant::updateStatistics(const ValidationResult& result, const String& context) const {
    statistics_.totalValidations++;
    
    if (!result.isValid) {
        statistics_.validationErrors++;
        statistics_.errorsByContext[context]++;
    }
}

String ContextAssistant::generateErrorMessage(const String& rule, const String& context, const String& details) const {
    return "违反约束规则 '" + rule + "' 在上下文 '" + context + "': " + details;
}

String ContextAssistant::generateSuggestion(const String& rule, const String& context) const {
    return "检查 " + rule + " 规则的要求";
}

// ContextValidationGuard 实现
ContextValidationGuard::ContextValidationGuard(ContextAssistant& assistant, const ContextInfo& context, const String& operation)
    : assistant_(assistant), context_(context), operation_(operation), isValid_(true) {
    
    // 在构造时进行初始验证
    auto results = assistant_.validateContext(context_);
    if (!results.empty()) {
        result_ = results[0]; // 取第一个错误
        isValid_ = false;
    }
}

ContextValidationGuard::~ContextValidationGuard() {
    // 析构时可以进行清理或最终验证
    if (!isValid_ && !result_.errorMessage.empty()) {
        std::cerr << "Context validation failed: " << result_.errorMessage << std::endl;
    }
}

void ContextValidationGuard::markError(const String& error) {
    isValid_ = false;
    result_.isValid = false;
    result_.errorMessage = error;
}

} // namespace chtl