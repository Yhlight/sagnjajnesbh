#include "../../include/ContextManager.h"

namespace CHTL {

// ContextGuard实现
ContextGuard::ContextGuard(ContextManager& cm, const ContextInfo& context)
    : contextManager_(cm) {
    contextManager_.pushContext(context);
}

ContextGuard::~ContextGuard() {
    contextManager_.popContext();
}

// ContextManager实现
ContextManager::ContextManager(StateMachine& stateMachine, GlobalMap& globalMap)
    : stateMachine_(stateMachine), globalMap_(globalMap) {
    // 初始化全局上下文
    ContextInfo globalContext(ContextType::GLOBAL, "global");
    contextStack_.push(globalContext);
}

void ContextManager::pushContext(const ContextInfo& context) {
    contextStack_.push(context);
    updateStatistics("push");
    statistics_.contextPushes++;
}

void ContextManager::popContext() {
    if (contextStack_.size() > 1) { // 保留全局上下文
        contextStack_.pop();
        updateStatistics("pop");
        statistics_.contextPops++;
    }
}

const ContextInfo& ContextManager::getCurrentContext() const {
    return contextStack_.top();
}

const ContextInfo& ContextManager::getParentContext() const {
    if (contextStack_.size() >= 2) {
        auto temp = contextStack_;
        temp.pop();
        return temp.top();
    }
    return getCurrentContext();
}

bool ContextManager::isInContext(ContextType type) const {
    return getCurrentContext().type == type;
}

bool ContextManager::isInNamespace(const String& namespace_) const {
    return getCurrentContext().namespace_ == namespace_;
}

String ContextManager::getCurrentNamespace() const {
    return getCurrentContext().namespace_;
}

String ContextManager::getCurrentElementName() const {
    if (isInContext(ContextType::ELEMENT)) {
        return getCurrentContext().name;
    }
    return "";
}

void ContextManager::addAttribute(const String& name, const String& value) {
    if (!contextStack_.empty()) {
        contextStack_.top().attributes[name] = value;
        updateStatistics("attribute_add");
        statistics_.attributeAccesses++;
    }
}

void ContextManager::removeAttribute(const String& name) {
    if (!contextStack_.empty()) {
        contextStack_.top().attributes.erase(name);
    }
}

bool ContextManager::hasAttribute(const String& name) const {
    return getCurrentContext().attributes.find(name) != getCurrentContext().attributes.end();
}

String ContextManager::getAttributeValue(const String& name) const {
    const auto& attrs = getCurrentContext().attributes;
    auto it = attrs.find(name);
    return it != attrs.end() ? it->second : "";
}

StringMap ContextManager::getAllAttributes() const {
    return getCurrentContext().attributes;
}

void ContextManager::addVariable(const String& name, const String& value) {
    if (!contextStack_.empty()) {
        contextStack_.top().variables[name] = value;
        updateStatistics("variable_add");
        statistics_.variableAccesses++;
    }
}

void ContextManager::removeVariable(const String& name) {
    if (!contextStack_.empty()) {
        contextStack_.top().variables.erase(name);
    }
}

bool ContextManager::hasVariable(const String& name) const {
    return getCurrentContext().variables.find(name) != getCurrentContext().variables.end();
}

String ContextManager::getVariableValue(const String& name) const {
    const auto& vars = getCurrentContext().variables;
    auto it = vars.find(name);
    return it != vars.end() ? it->second : "";
}

String ContextManager::resolveVariable(const String& name) const {
    // 首先在当前上下文查找
    if (hasVariable(name)) {
        return getVariableValue(name);
    }
    
    // 然后在全局映射中查找
    String resolved = globalMap_.resolveVariableName(name, getCurrentNamespace());
    if (!resolved.empty()) {
        return globalMap_.getVariableValue(name, getCurrentNamespace());
    }
    
    return "";
}

void ContextManager::addAutoClassName(const String& className) {
    if (!contextStack_.empty()) {
        contextStack_.top().classNames.push_back(className);
    }
}

void ContextManager::addAutoIdName(const String& idName) {
    if (!contextStack_.empty()) {
        contextStack_.top().idNames.push_back(idName);
    }
}

StringList ContextManager::getAutoClassNames() const {
    return getCurrentContext().classNames;
}

StringList ContextManager::getAutoIdNames() const {
    return getCurrentContext().idNames;
}

void ContextManager::clearAutoNames() {
    if (!contextStack_.empty()) {
        contextStack_.top().classNames.clear();
        contextStack_.top().idNames.clear();
    }
}

void ContextManager::addConstraint(const ConstraintInfo& constraint) {
    if (!contextStack_.empty()) {
        contextStack_.top().constraints = constraint;
    }
}

void ContextManager::removeConstraint() {
    if (!contextStack_.empty()) {
        contextStack_.top().constraints = ConstraintInfo();
    }
}

bool ContextManager::isElementAllowed(const String& element) const {
    return checkElementConstraint(element);
}

bool ContextManager::isTypeAllowed(const String& type) const {
    return checkTypeConstraint(type);
}

bool ContextManager::checkConstraints(const String& element, const String& type) const {
    bool elementOk = element.empty() || checkElementConstraint(element);
    bool typeOk = type.empty() || checkTypeConstraint(type);
    
    statistics_.constraintChecks++;
    if (!elementOk || !typeOk) {
        statistics_.constraintViolations++;
    }
    
    return elementOk && typeOk;
}

bool ContextManager::isCHTLJSAllowed() const {
    return getCurrentContext().allowCHTLJS;
}

bool ContextManager::areTemplatesAllowed() const {
    return getCurrentContext().allowTemplates;
}

bool ContextManager::areCustomsAllowed() const {
    return getCurrentContext().allowCustoms;
}

void ContextManager::setCHTLJSAllowed(bool allowed) {
    if (!contextStack_.empty()) {
        contextStack_.top().allowCHTLJS = allowed;
    }
}

void ContextManager::setTemplatesAllowed(bool allowed) {
    if (!contextStack_.empty()) {
        contextStack_.top().allowTemplates = allowed;
    }
}

void ContextManager::setCustomsAllowed(bool allowed) {
    if (!contextStack_.empty()) {
        contextStack_.top().allowCustoms = allowed;
    }
}

bool ContextManager::isInGlobalStyleBlock() const {
    // 全局样式块：不在任何元素内的style块
    return isInContext(ContextType::STYLE_BLOCK) && getContextDepth() <= 2;
}

bool ContextManager::isInLocalStyleBlock() const {
    // 局部样式块：在元素内的style块
    return isInContext(ContextType::STYLE_BLOCK) && getContextDepth() > 2;
}

bool ContextManager::isInLocalScriptBlock() const {
    // 局部脚本块：在元素内的script块
    return isInContext(ContextType::SCRIPT_BLOCK);
}

bool ContextManager::isInTemplateScope() const {
    return isInContext(ContextType::TEMPLATE);
}

bool ContextManager::isInCustomScope() const {
    return isInContext(ContextType::CUSTOM);
}

bool ContextManager::validateElementUsage(const String& element) const {
    return checkConstraints(element);
}

bool ContextManager::validateTypeUsage(const String& type) const {
    return checkConstraints("", type);
}

bool ContextManager::validateCHTLJSUsage() const {
    if (!isCHTLJSAllowed()) {
        reportConstraintViolation("CHTL JS", "CHTL JS语法在当前上下文中不被允许");
        return false;
    }
    return true;
}

bool ContextManager::validateTemplateUsage(const String& templateName) const {
    if (!areTemplatesAllowed()) {
        reportConstraintViolation(templateName, "模板在当前上下文中不被允许");
        return false;
    }
    return true;
}

bool ContextManager::validateCustomUsage(const String& customName) const {
    if (!areCustomsAllowed()) {
        reportConstraintViolation(customName, "自定义在当前上下文中不被允许");
        return false;
    }
    return true;
}

String ContextManager::resolveTemplateName(const String& name) const {
    return globalMap_.resolveTemplateName(name, getCurrentNamespace());
}

String ContextManager::resolveCustomName(const String& name) const {
    return globalMap_.resolveCustomName(name, getCurrentNamespace());
}

String ContextManager::resolveOriginName(const String& name) const {
    // 原始嵌入名称解析逻辑
    return name; // 暂时简单实现
}

String ContextManager::resolveFullName(const String& name, const String& type) const {
    if (type == "template") {
        return resolveTemplateName(name);
    } else if (type == "custom") {
        return resolveCustomName(name);
    } else if (type == "origin") {
        return resolveOriginName(name);
    }
    return name;
}

std::vector<String> ContextManager::getContextPath() const {
    std::vector<String> path;
    std::stack<ContextInfo> temp = contextStack_;
    
    while (!temp.empty()) {
        path.push_back(contextTypeToString(temp.top().type) + ":" + temp.top().name);
        temp.pop();
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

String ContextManager::getContextPathString() const {
    auto path = getContextPath();
    String result;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) result += " -> ";
        result += path[i];
    }
    return result;
}

size_t ContextManager::getContextDepth() const {
    return contextStack_.size();
}

void ContextManager::enterInheritanceContext(const String& parentName) {
    inheritanceStack_.push(parentName);
}

void ContextManager::exitInheritanceContext() {
    if (!inheritanceStack_.empty()) {
        inheritanceStack_.pop();
    }
}

bool ContextManager::isInInheritanceContext() const {
    return !inheritanceStack_.empty();
}

String ContextManager::getCurrentInheritanceParent() const {
    return inheritanceStack_.empty() ? "" : inheritanceStack_.top();
}

void ContextManager::reportContextError(const String& message) {
    contextErrors_.push_back(message);
}

void ContextManager::reportConstraintViolation(const String& element, const String& constraint) const {
    const_cast<ContextManager*>(this)->contextErrors_.push_back(
        "约束违反: " + element + " - " + constraint);
}

std::vector<String> ContextManager::getContextErrors() const {
    return contextErrors_;
}

void ContextManager::clearContextErrors() {
    contextErrors_.clear();
}

String ContextManager::dumpCurrentContext() const {
    const auto& ctx = getCurrentContext();
    
    String result = "=== Current Context ===\n";
    result += "Type: " + contextTypeToString(ctx.type) + "\n";
    result += "Name: " + ctx.name + "\n";
    result += "Namespace: " + ctx.namespace_ + "\n";
    result += "CHTL JS Allowed: " + (ctx.allowCHTLJS ? String("Yes") : String("No")) + "\n";
    result += "Templates Allowed: " + (ctx.allowTemplates ? String("Yes") : String("No")) + "\n";
    result += "Customs Allowed: " + (ctx.allowCustoms ? String("Yes") : String("No")) + "\n";
    
    return result;
}

String ContextManager::dumpContextStack() const {
    String result = "=== Context Stack ===\n";
    result += "Path: " + getContextPathString() + "\n";
    return result;
}

void ContextManager::printContextHierarchy() const {
    // 简化实现，避免iostream依赖
}

ContextManager::Statistics ContextManager::getStatistics() const {
    return statistics_;
}

void ContextManager::resetStatistics() {
    statistics_ = Statistics{};
}

String ContextManager::contextTypeToString(ContextType type) const {
    switch (type) {
        case ContextType::GLOBAL: return "GLOBAL";
        case ContextType::ELEMENT: return "ELEMENT";
        case ContextType::ATTRIBUTE: return "ATTRIBUTE";
        case ContextType::TEXT: return "TEXT";
        case ContextType::STYLE_BLOCK: return "STYLE_BLOCK";
        case ContextType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case ContextType::TEMPLATE: return "TEMPLATE";
        case ContextType::CUSTOM: return "CUSTOM";
        case ContextType::NAMESPACE: return "NAMESPACE";
        case ContextType::IMPORT: return "IMPORT";
        case ContextType::CONFIGURATION: return "CONFIGURATION";
        default: return "UNKNOWN";
    }
}

bool ContextManager::isValidContextTransition(ContextType from, ContextType to) const {
    // 上下文转换规则验证
    return true; // 暂时允许所有转换
}

void ContextManager::updateStatistics(const String& operation) {
    // 统计信息更新逻辑
}

bool ContextManager::checkElementConstraint(const String& element) const {
    const auto& constraints = getCurrentContext().constraints;
    if (constraints.type == ConstraintType::ELEMENT_CONSTRAINT ||
        constraints.type == ConstraintType::GLOBAL_CONSTRAINT) {
        return constraints.bannedElements.find(element) == constraints.bannedElements.end();
    }
    return true;
}

bool ContextManager::checkTypeConstraint(const String& type) const {
    const auto& constraints = getCurrentContext().constraints;
    if (constraints.type == ConstraintType::TYPE_CONSTRAINT ||
        constraints.type == ConstraintType::GLOBAL_CONSTRAINT) {
        return constraints.bannedTypes.find(type) == constraints.bannedTypes.end();
    }
    return true;
}

bool ContextManager::checkGlobalConstraint() const {
    // 全局约束检查逻辑
    return true;
}

String ContextManager::resolveInCurrentNamespace(const String& name, const String& type) const {
    // 在当前命名空间解析名称
    return name;
}

String ContextManager::resolveInParentNamespaces(const String& name, const String& type) const {
    // 在父命名空间解析名称
    return name;
}

String ContextManager::resolveInGlobalScope(const String& name, const String& type) const {
    // 在全局作用域解析名称
    return name;
}

} // namespace CHTL