#include "ConstraintManager.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace constraint_system {

// 全局约束管理器实例
std::shared_ptr<ConstraintManager> g_constraintManager = std::make_shared<ConstraintManager>();

ConstraintManager::ConstraintManager() = default;
ConstraintManager::~ConstraintManager() = default;

// === 约束规则管理 ===

void ConstraintManager::addConstraintRule(const ConstraintRule& rule) {
    constraintRules_.push_back(rule);
    
    // 根据约束类型分别存储
    switch (rule.type) {
        case ConstraintType::PRECISE:
            for (const auto& target : rule.targets) {
                preciseConstraints_[rule.scope].insert(target);
            }
            break;
        case ConstraintType::TYPE:
            for (const auto& targetType : rule.targetTypes) {
                typeConstraints_[rule.scope].insert(targetType);
            }
            break;
        case ConstraintType::GLOBAL:
            for (const auto& targetType : rule.targetTypes) {
                globalConstraints_[rule.scope].insert(targetType);
            }
            break;
    }
}

void ConstraintManager::removeConstraintRule(const std::string& scope, const std::string& target) {
    // 从精确约束中移除
    auto preciseIt = preciseConstraints_.find(scope);
    if (preciseIt != preciseConstraints_.end()) {
        preciseIt->second.erase(target);
    }
    
    // 从约束规则列表中移除
    constraintRules_.erase(
        std::remove_if(constraintRules_.begin(), constraintRules_.end(),
            [&scope, &target](const ConstraintRule& rule) {
                return rule.scope == scope && 
                       std::find(rule.targets.begin(), rule.targets.end(), target) != rule.targets.end();
            }),
        constraintRules_.end());
}

bool ConstraintManager::hasConstraintRule(const std::string& scope, const std::string& target) const {
    auto it = preciseConstraints_.find(scope);
    if (it != preciseConstraints_.end()) {
        return it->second.find(target) != it->second.end();
    }
    return false;
}

// === 精确约束 ===

void ConstraintManager::addPreciseConstraint(const std::string& scope, const std::vector<std::string>& targets) {
    ConstraintRule rule;
    rule.type = ConstraintType::PRECISE;
    rule.scope = scope;
    rule.targets = targets;
    rule.isActive = true;
    
    addConstraintRule(rule);
}

bool ConstraintManager::isPreciselyConstrained(const std::string& scope, const std::string& target) const {
    auto it = preciseConstraints_.find(scope);
    if (it != preciseConstraints_.end()) {
        return it->second.find(target) != it->second.end();
    }
    return false;
}

// === 类型约束 ===

void ConstraintManager::addTypeConstraint(const std::string& scope, ConstraintTargetType targetType) {
    ConstraintRule rule;
    rule.type = ConstraintType::TYPE;
    rule.scope = scope;
    rule.targetTypes = {targetType};
    rule.isActive = true;
    
    addConstraintRule(rule);
}

bool ConstraintManager::isTypeConstrained(const std::string& scope, ConstraintTargetType targetType) const {
    auto it = typeConstraints_.find(scope);
    if (it != typeConstraints_.end()) {
        return it->second.find(targetType) != it->second.end();
    }
    return false;
}

// === 全局约束 ===

void ConstraintManager::addGlobalConstraint(const std::string& namespace_, ConstraintTargetType targetType) {
    ConstraintRule rule;
    rule.type = ConstraintType::GLOBAL;
    rule.scope = namespace_;
    rule.targetTypes = {targetType};
    rule.isActive = true;
    
    addConstraintRule(rule);
}

bool ConstraintManager::isGloballyConstrained(const std::string& namespace_, ConstraintTargetType targetType) const {
    auto it = globalConstraints_.find(namespace_);
    if (it != globalConstraints_.end()) {
        return it->second.find(targetType) != it->second.end();
    }
    return false;
}

// === 约束验证 ===

bool ConstraintManager::validateElementUsage(const std::string& scope, const std::string& elementName, const std::string& elementType) {
    // 检查精确约束
    if (isPreciselyConstrained(scope, elementName)) {
        return false; // 被精确约束禁止
    }
    
    // 检查类型约束
    if (isHTMLElement(elementName) && isTypeConstrained(scope, ConstraintTargetType::HTML_TYPE)) {
        return false; // HTML元素被类型约束禁止
    }
    
    // 检查全局约束
    std::string currentNamespace = getCurrentScope();
    if (isHTMLElement(elementName) && isGloballyConstrained(currentNamespace, ConstraintTargetType::HTML_TYPE)) {
        return false; // 被全局约束禁止
    }
    
    return true; // 允许使用
}

bool ConstraintManager::validateCustomUsage(const std::string& scope, const std::string& customName, const std::string& customType) {
    // 检查精确约束
    std::string fullName = "[Custom] @" + customType + " " + customName;
    if (isPreciselyConstrained(scope, fullName)) {
        return false;
    }
    
    // 检查类型约束
    ConstraintTargetType targetType = parseCustomTargetType(customType);
    if (isTypeConstrained(scope, targetType)) {
        return false;
    }
    
    // 检查全局约束
    std::string currentNamespace = getCurrentScope();
    if (isGloballyConstrained(currentNamespace, ConstraintTargetType::CUSTOM_TYPE)) {
        return false;
    }
    
    return true;
}

bool ConstraintManager::validateTemplateUsage(const std::string& scope, const std::string& templateName, const std::string& templateType) {
    // 检查精确约束
    std::string fullName = "[Template] @" + templateType + " " + templateName;
    if (isPreciselyConstrained(scope, fullName)) {
        return false;
    }
    
    // 检查类型约束
    ConstraintTargetType targetType = parseTemplateTargetType(templateType);
    if (isTypeConstrained(scope, targetType)) {
        return false;
    }
    
    // 检查全局约束
    std::string currentNamespace = getCurrentScope();
    if (isGloballyConstrained(currentNamespace, ConstraintTargetType::TEMPLATE_TYPE)) {
        return false;
    }
    
    return true;
}

// === 约束检查 ===

std::vector<std::string> ConstraintManager::checkConstraintViolations(const std::string& scope, 
                                                                      const std::vector<std::string>& usedElements,
                                                                      const std::vector<std::string>& usedCustoms,
                                                                      const std::vector<std::string>& usedTemplates) {
    std::vector<std::string> violations;
    
    // 检查元素使用违规
    for (const auto& element : usedElements) {
        if (!validateElementUsage(scope, element)) {
            violations.push_back("元素使用违反约束: " + element);
        }
    }
    
    // 检查自定义使用违规
    for (const auto& custom : usedCustoms) {
        // 解析自定义名称和类型
        std::regex customRegex(R"(\[Custom\]\s*@(\w+)\s+(\w+))");
        std::smatch match;
        if (std::regex_match(custom, match, customRegex)) {
            std::string customType = match[1].str();
            std::string customName = match[2].str();
            if (!validateCustomUsage(scope, customName, customType)) {
                violations.push_back("自定义使用违反约束: " + custom);
            }
        }
    }
    
    // 检查模板使用违规
    for (const auto& template_ : usedTemplates) {
        // 解析模板名称和类型
        std::regex templateRegex(R"(\[Template\]\s*@(\w+)\s+(\w+))");
        std::smatch match;
        if (std::regex_match(template_, match, templateRegex)) {
            std::string templateType = match[1].str();
            std::string templateName = match[2].str();
            if (!validateTemplateUsage(scope, templateName, templateType)) {
                violations.push_back("模板使用违反约束: " + template_);
            }
        }
    }
    
    return violations;
}

// === 约束解析 ===

bool ConstraintManager::parseConstraintStatement(const std::string& statement, const std::string& scope) {
    // 按语法文档解析约束语句：except span, [Custom] @Element Box;
    
    std::regex constraintRegex(R"(except\s+(.+);)");
    std::smatch match;
    
    if (!std::regex_match(statement, match, constraintRegex)) {
        errors_.push_back("无效的约束语句格式: " + statement);
        return false;
    }
    
    std::string targets = match[1].str();
    
    // 解析约束目标
    std::vector<std::string> targetList = parseCommaSeparatedTargets(targets);
    
    // 确定约束类型
    ConstraintType constraintType = determineConstraintType(targetList);
    
    // 创建约束规则
    ConstraintRule rule;
    rule.type = constraintType;
    rule.scope = scope;
    rule.targets = targetList;
    rule.isActive = true;
    
    // 解析目标类型
    for (const auto& target : targetList) {
        ConstraintTargetType targetType = parseTargetType(target);
        rule.targetTypes.push_back(targetType);
    }
    
    addConstraintRule(rule);
    return true;
}

// === 约束范围管理 ===

void ConstraintManager::enterScope(const std::string& scope) {
    scopeStack_.push_back(scope);
}

void ConstraintManager::exitScope(const std::string& scope) {
    if (!scopeStack_.empty() && scopeStack_.back() == scope) {
        scopeStack_.pop_back();
    }
}

std::string ConstraintManager::getCurrentScope() const {
    return scopeStack_.empty() ? "global" : scopeStack_.back();
}

// === 清理和统计 ===

void ConstraintManager::clear() {
    constraintRules_.clear();
    preciseConstraints_.clear();
    typeConstraints_.clear();
    globalConstraints_.clear();
    scopeStack_.clear();
    clearErrors();
}

void ConstraintManager::clearScope(const std::string& scope) {
    // 清除特定作用域的约束
    constraintRules_.erase(
        std::remove_if(constraintRules_.begin(), constraintRules_.end(),
            [&scope](const ConstraintRule& rule) {
                return rule.scope == scope;
            }),
        constraintRules_.end());
    
    preciseConstraints_.erase(scope);
    typeConstraints_.erase(scope);
    globalConstraints_.erase(scope);
}

std::unordered_map<ConstraintType, int> ConstraintManager::getConstraintStatistics() {
    std::unordered_map<ConstraintType, int> stats;
    
    for (const auto& rule : constraintRules_) {
        stats[rule.type]++;
    }
    
    return stats;
}

// === 内部方法 ===

ConstraintTargetType ConstraintManager::parseTargetType(const std::string& target) {
    if (target.find("[Custom] @Element") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_ELEMENT;
    } else if (target.find("[Custom] @Style") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_STYLE;
    } else if (target.find("[Custom] @Var") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_VAR;
    } else if (target.find("[Template] @Element") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_ELEMENT;
    } else if (target.find("[Template] @Style") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_STYLE;
    } else if (target.find("[Template] @Var") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_VAR;
    } else if (target.find("@Html") != std::string::npos) {
        return ConstraintTargetType::HTML_TYPE;
    } else if (target.find("[Custom]") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_TYPE;
    } else if (target.find("[Template]") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_TYPE;
    } else if (target.find("[Origin]") != std::string::npos) {
        return ConstraintTargetType::ORIGIN_TYPE;
    } else if (isHTMLElement(target)) {
        return ConstraintTargetType::HTML_ELEMENT;
    }
    
    return ConstraintTargetType::HTML_ELEMENT; // 默认
}

ConstraintType ConstraintManager::determineConstraintType(const std::vector<std::string>& targets) {
    bool hasTypeConstraint = false;
    bool hasPreciseConstraint = false;
    
    for (const auto& target : targets) {
        if (target.find("@") != std::string::npos || 
            target.find("[Custom]") != std::string::npos || 
            target.find("[Template]") != std::string::npos) {
            hasTypeConstraint = true;
        } else {
            hasPreciseConstraint = true;
        }
    }
    
    if (hasTypeConstraint && hasPreciseConstraint) {
        // 混合约束，按语法文档处理为精确约束
        return ConstraintType::PRECISE;
    } else if (hasTypeConstraint) {
        return ConstraintType::TYPE;
    } else {
        return ConstraintType::PRECISE;
    }
}

std::string ConstraintManager::formatTargetType(ConstraintTargetType type) {
    switch (type) {
        case ConstraintTargetType::HTML_ELEMENT:     return "HTML元素";
        case ConstraintTargetType::CUSTOM_ELEMENT:   return "[Custom] @Element";
        case ConstraintTargetType::TEMPLATE_ELEMENT: return "[Template] @Element";
        case ConstraintTargetType::CUSTOM_STYLE:     return "[Custom] @Style";
        case ConstraintTargetType::TEMPLATE_STYLE:   return "[Template] @Style";
        case ConstraintTargetType::CUSTOM_VAR:       return "[Custom] @Var";
        case ConstraintTargetType::TEMPLATE_VAR:     return "[Template] @Var";
        case ConstraintTargetType::HTML_TYPE:        return "@Html";
        case ConstraintTargetType::CUSTOM_TYPE:      return "[Custom]";
        case ConstraintTargetType::TEMPLATE_TYPE:    return "[Template]";
        case ConstraintTargetType::ORIGIN_TYPE:      return "[Origin]";
        default:                                     return "未知类型";
    }
}

bool ConstraintManager::isValidConstraintTarget(const std::string& target) {
    if (target.empty()) return false;
    
    // 检查是否为有效的约束目标格式
    return isHTMLElement(target) || 
           target.find("[Custom]") != std::string::npos ||
           target.find("[Template]") != std::string::npos ||
           target.find("@") != std::string::npos;
}

bool ConstraintManager::isHTMLElement(const std::string& element) {
    // 按语法文档：检查是否为HTML元素
    std::vector<std::string> htmlElements = {
        "div", "span", "p", "a", "img", "input", "button", "form", "table", "tr", "td",
        "ul", "ol", "li", "h1", "h2", "h3", "h4", "h5", "h6", "section", "article",
        "header", "footer", "nav", "main", "aside", "figure", "figcaption",
        "html", "head", "body", "title", "meta", "link", "script", "style"
    };
    
    return std::find(htmlElements.begin(), htmlElements.end(), element) != htmlElements.end();
}

bool ConstraintManager::hasConstraintConflict(const ConstraintRule& newRule) {
    // 检查新约束是否与现有约束冲突
    for (const auto& existingRule : constraintRules_) {
        if (existingRule.scope == newRule.scope && existingRule.type == newRule.type) {
            // 检查目标重叠
            for (const auto& target : newRule.targets) {
                if (std::find(existingRule.targets.begin(), existingRule.targets.end(), target) != existingRule.targets.end()) {
                    return true; // 发现冲突
                }
            }
        }
    }
    
    return false;
}

std::vector<std::string> ConstraintManager::parseCommaSeparatedTargets(const std::string& targets) {
    std::vector<std::string> result;
    std::stringstream ss(targets);
    std::string target;
    
    while (std::getline(ss, target, ',')) {
        // 去除前后空白
        target.erase(0, target.find_first_not_of(" \t"));
        target.erase(target.find_last_not_of(" \t") + 1);
        if (!target.empty()) {
            result.push_back(target);
        }
    }
    
    return result;
}

ConstraintTargetType ConstraintManager::parseCustomTargetType(const std::string& customType) {
    if (customType == "Element") return ConstraintTargetType::CUSTOM_ELEMENT;
    if (customType == "Style") return ConstraintTargetType::CUSTOM_STYLE;
    if (customType == "Var") return ConstraintTargetType::CUSTOM_VAR;
    return ConstraintTargetType::CUSTOM_ELEMENT;
}

ConstraintTargetType ConstraintManager::parseTemplateTargetType(const std::string& templateType) {
    if (templateType == "Element") return ConstraintTargetType::TEMPLATE_ELEMENT;
    if (templateType == "Style") return ConstraintTargetType::TEMPLATE_STYLE;
    if (templateType == "Var") return ConstraintTargetType::TEMPLATE_VAR;
    return ConstraintTargetType::TEMPLATE_ELEMENT;
}

} // namespace constraint_system
} // namespace chtl