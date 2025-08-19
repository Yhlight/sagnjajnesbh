#include "CustomManager.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <regex>
#include <sstream>

namespace chtl {
namespace custom_system {

// 全局自定义管理器实例
std::shared_ptr<CustomManager> g_customManager = std::make_shared<CustomManager>();

CustomManager::CustomManager() {
    templateManager_ = template_system::g_templateManager;
}

CustomManager::~CustomManager() = default;

// === 自定义样式组管理 ===

void CustomManager::addCustomStyle(const std::string& name, const CustomStyle& custom) {
    auto customPtr = std::make_shared<CustomStyle>(custom);
    customStyles_[name] = customPtr;
    processCustomInheritance(*customPtr);
}

std::shared_ptr<CustomStyle> CustomManager::getCustomStyle(const std::string& name) {
    auto it = customStyles_.find(name);
    return (it != customStyles_.end()) ? it->second : nullptr;
}

bool CustomManager::hasCustomStyle(const std::string& name) const {
    return customStyles_.find(name) != customStyles_.end();
}

// === 自定义元素管理 ===

void CustomManager::addCustomElement(const std::string& name, CustomElement&& custom) {
    custom.name = name;
    auto customPtr = std::make_shared<CustomElement>(std::move(custom));
    customElements_[name] = customPtr;
    processCustomInheritance(*customPtr);
}

std::shared_ptr<CustomElement> CustomManager::getCustomElement(const std::string& name) {
    auto it = customElements_.find(name);
    return (it != customElements_.end()) ? it->second : nullptr;
}

bool CustomManager::hasCustomElement(const std::string& name) const {
    return customElements_.find(name) != customElements_.end();
}

// === 自定义变量组管理 ===

void CustomManager::addCustomVariable(const std::string& name, const CustomVariable& custom) {
    auto customPtr = std::make_shared<CustomVariable>(custom);
    customVariables_[name] = customPtr;
    processCustomInheritance(*customPtr);
}

std::shared_ptr<CustomVariable> CustomManager::getCustomVariable(const std::string& name) {
    auto it = customVariables_.find(name);
    return (it != customVariables_.end()) ? it->second : nullptr;
}

bool CustomManager::hasCustomVariable(const std::string& name) const {
    return customVariables_.find(name) != customVariables_.end();
}

// === 特例化操作处理 ===

bool CustomManager::applySpecialization(const std::string& targetName, const SpecializationOperation& operation) {
    if (operation.operation == "delete") {
        return applyDeleteOperation(targetName, operation);
    } else if (operation.operation == "insert") {
        return applyInsertOperation(targetName, operation);
    } else if (operation.operation == "replace") {
        return applyReplaceOperation(targetName, operation);
    } else if (operation.operation == "inherit") {
        return applyInheritOperation(targetName, operation);
    }
    
    errors_.push_back("未知的特例化操作: " + operation.operation);
    return false;
}

bool CustomManager::applyDeleteOperation(const std::string& targetName, const SpecializationOperation& operation) {
    // 按语法文档实现删除操作
    
    if (operation.target.find(",") != std::string::npos) {
        // 删除多个属性: delete line-height, border;
        return deleteProperty(targetName, parseCommaSeparatedList(operation.target));
    } else if (operation.target.find("[") != std::string::npos) {
        // 删除索引元素: delete div[1];
        return deleteElementByIndex(targetName, operation.target);
    } else if (operation.target.find("@") != std::string::npos) {
        // 删除继承: delete @Style WhiteText;
        return deleteInheritance(targetName, operation.target);
    } else {
        // 删除单个元素: delete span;
        return deleteElement(targetName, operation.target);
    }
}

bool CustomManager::applyInsertOperation(const std::string& targetName, const SpecializationOperation& operation) {
    // 按语法文档实现插入操作: insert after div[0] { ... }
    
    std::string position = operation.position; // after, before, at top, at bottom
    std::string selector = operation.target;   // div[0]
    
    return insertElement(targetName, position, selector, 
                        std::vector<std::unique_ptr<ast::ASTNode>>()); // TODO: 复制operation.content
}

bool CustomManager::applyReplaceOperation(const std::string& targetName, const SpecializationOperation& operation) {
    // 按语法文档实现替换操作
    
    // 先删除目标元素
    if (!deleteElement(targetName, operation.target)) {
        return false;
    }
    
    // 然后插入新内容
    return insertElement(targetName, "replace", operation.target,
                        std::vector<std::unique_ptr<ast::ASTNode>>()); // TODO: 复制operation.content
}

bool CustomManager::applyInheritOperation(const std::string& targetName, const SpecializationOperation& operation) {
    // 按语法文档实现继承操作: inherit @Style ThemeColor;
    
    auto custom = getCustomStyle(targetName);
    if (!custom) {
        errors_.push_back("自定义样式不存在: " + targetName);
        return false;
    }
    
    // 解析继承目标
    std::string inheritanceTarget = operation.target;
    if (inheritanceTarget.find("@Style") != std::string::npos) {
        std::string templateName = inheritanceTarget.substr(inheritanceTarget.find("@Style") + 6);
        templateName.erase(0, templateName.find_first_not_of(" \t"));
        
        custom->inheritedTemplates.push_back(templateName);
        return true;
    }
    
    errors_.push_back("无效的继承目标: " + inheritanceTarget);
    return false;
}

// === 无值样式组处理 ===

std::unordered_map<std::string, std::string> CustomManager::resolveNoValueStyle(
    const std::string& styleName, const std::unordered_map<std::string, std::string>& values) {
    
    auto custom = getCustomStyle(styleName);
    if (!custom) {
        errors_.push_back("自定义样式不存在: " + styleName);
        return {};
    }
    
    std::unordered_map<std::string, std::string> result;
    
    // 按语法文档：无值属性需要在使用时提供值
    for (const auto& noValueProp : custom->noValueProperties) {
        auto it = values.find(noValueProp);
        if (it != values.end()) {
            result[noValueProp] = it->second;
        } else {
            errors_.push_back("无值属性缺少值: " + noValueProp);
        }
    }
    
    // 添加有值属性
    for (const auto& [prop, value] : custom->properties) {
        result[prop] = value;
    }
    
    return result;
}

std::unordered_map<std::string, std::string> CustomManager::resolveNoValueVariable(
    const std::string& varName, const std::unordered_map<std::string, std::string>& values) {
    
    auto custom = getCustomVariable(varName);
    if (!custom) {
        errors_.push_back("自定义变量组不存在: " + varName);
        return {};
    }
    
    std::unordered_map<std::string, std::string> result;
    
    // 处理无值变量
    for (const auto& noValueVar : custom->noValueVariables) {
        auto it = values.find(noValueVar);
        if (it != values.end()) {
            result[noValueVar] = it->second;
        } else {
            errors_.push_back("无值变量缺少值: " + noValueVar);
        }
    }
    
    // 添加有值变量
    for (const auto& [var, value] : custom->variables) {
        result[var] = value;
    }
    
    return result;
}

// === 继承解析 ===

std::unordered_map<std::string, std::string> CustomManager::resolveCustomStyleInheritance(const std::string& customName) {
    std::unordered_map<std::string, std::string> result;
    
    auto custom = getCustomStyle(customName);
    if (!custom) {
        errors_.push_back("自定义样式不存在: " + customName);
        return result;
    }
    
    // 处理模板继承
    for (const auto& templateName : custom->inheritedTemplates) {
        if (templateManager_->hasStyleTemplate(templateName)) {
            auto templateProps = templateManager_->resolveStyleInheritance(templateName);
            for (const auto& [key, value] : templateProps) {
                result[key] = value;
            }
        }
    }
    
    // 处理自定义继承
    for (const auto& customName : custom->inheritedCustoms) {
        auto inheritedProps = resolveCustomStyleInheritance(customName);
        for (const auto& [key, value] : inheritedProps) {
            result[key] = value;
        }
    }
    
    // 应用当前自定义的属性
    for (const auto& [key, value] : custom->properties) {
        result[key] = value;
    }
    
    return result;
}

// === 删除操作实现 ===

bool CustomManager::deleteProperty(const std::string& styleName, const std::vector<std::string>& propertiesToDelete) {
    auto custom = getCustomStyle(styleName);
    if (!custom) {
        errors_.push_back("自定义样式不存在: " + styleName);
        return false;
    }
    
    // 按语法文档：删除指定的属性
    for (const auto& prop : propertiesToDelete) {
        custom->properties.erase(prop);
        custom->noValueProperties.erase(prop);
    }
    
    return true;
}

bool CustomManager::deleteElement(const std::string& elementName, const std::string& elementToDelete) {
    auto custom = getCustomElement(elementName);
    if (!custom) {
        errors_.push_back("自定义元素不存在: " + elementName);
        return false;
    }
    
    // TODO: 实现元素删除逻辑
    errors_.push_back("元素删除功能需要AST节点操作支持");
    return false;
}

bool CustomManager::deleteElementByIndex(const std::string& elementName, const std::string& indexExpression) {
    // 解析索引表达式: div[1]
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch match;
    
    if (!std::regex_match(indexExpression, match, indexRegex)) {
        errors_.push_back("无效的索引表达式: " + indexExpression);
        return false;
    }
    
    std::string elementType = match[1].str();
    int index = std::stoi(match[2].str());
    
    // TODO: 实现按索引删除元素
    errors_.push_back("按索引删除元素功能需要AST节点操作支持");
    return false;
}

bool CustomManager::deleteInheritance(const std::string& targetName, const std::string& inheritanceToDelete) {
    auto custom = getCustomStyle(targetName);
    if (!custom) {
        errors_.push_back("自定义样式不存在: " + targetName);
        return false;
    }
    
    // 删除继承关系
    auto& inherited = custom->inheritedTemplates;
    inherited.erase(std::remove_if(inherited.begin(), inherited.end(),
        [&inheritanceToDelete](const std::string& name) {
            return inheritanceToDelete.find(name) != std::string::npos;
        }), inherited.end());
    
    return true;
}

// === 插入操作实现 ===

bool CustomManager::insertElement(const std::string& elementName, const std::string& position, 
                                 const std::string& selector, std::vector<std::unique_ptr<ast::ASTNode>> newElements) {
    auto custom = getCustomElement(elementName);
    if (!custom) {
        errors_.push_back("自定义元素不存在: " + elementName);
        return false;
    }
    
    // TODO: 实现元素插入逻辑
    // 需要解析位置说明符：after, before, at top, at bottom
    // 需要解析选择器：div[0], span等
    
    errors_.push_back("元素插入功能需要AST节点操作支持");
    return false;
}

// === 索引访问 ===

ast::ASTNode* CustomManager::getElementByIndex(const std::string& elementName, const std::string& elementType, int index) {
    auto custom = getCustomElement(elementName);
    if (!custom) {
        errors_.push_back("自定义元素不存在: " + elementName);
        return nullptr;
    }
    
    // TODO: 实现按索引访问元素
    errors_.push_back("索引访问功能需要AST节点查询支持");
    return nullptr;
}

// === 验证 ===

bool CustomManager::validateCustom(const std::string& name, const std::string& type) {
    if (type == "Style") {
        return hasCustomStyle(name);
    } else if (type == "Element") {
        return hasCustomElement(name);
    } else if (type == "Var") {
        return hasCustomVariable(name);
    }
    
    return false;
}

// === 清理 ===

void CustomManager::clear() {
    customStyles_.clear();
    customElements_.clear();
    customVariables_.clear();
    clearErrors();
}

// === 继承处理 ===

void CustomManager::processCustomInheritance(CustomStyle& custom) {
    // 验证模板继承
    for (const auto& templateName : custom.inheritedTemplates) {
        if (!templateManager_->hasStyleTemplate(templateName)) {
            errors_.push_back("继承的样式模板不存在: " + templateName);
        }
    }
    
    // 验证自定义继承
    for (const auto& customName : custom.inheritedCustoms) {
        if (!hasCustomStyle(customName)) {
            errors_.push_back("继承的自定义样式不存在: " + customName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(custom.name, "Style", visited)) {
        errors_.push_back("检测到自定义样式循环依赖: " + custom.name);
    }
}

void CustomManager::processCustomInheritance(CustomElement& custom) {
    // 验证模板继承
    for (const auto& templateName : custom.inheritedTemplates) {
        if (!templateManager_->hasElementTemplate(templateName)) {
            errors_.push_back("继承的元素模板不存在: " + templateName);
        }
    }
    
    // 验证自定义继承
    for (const auto& customName : custom.inheritedCustoms) {
        if (!hasCustomElement(customName)) {
            errors_.push_back("继承的自定义元素不存在: " + customName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(custom.name, "Element", visited)) {
        errors_.push_back("检测到自定义元素循环依赖: " + custom.name);
    }
}

void CustomManager::processCustomInheritance(CustomVariable& custom) {
    // 验证模板继承
    for (const auto& templateName : custom.inheritedTemplates) {
        if (!templateManager_->hasVariableTemplate(templateName)) {
            errors_.push_back("继承的变量模板不存在: " + templateName);
        }
    }
    
    // 验证自定义继承
    for (const auto& customName : custom.inheritedCustoms) {
        if (!hasCustomVariable(customName)) {
            errors_.push_back("继承的自定义变量不存在: " + customName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(custom.name, "Var", visited)) {
        errors_.push_back("检测到自定义变量循环依赖: " + custom.name);
    }
}

bool CustomManager::checkCircularDependency(const std::string& customName, const std::string& type,
                                           std::unordered_set<std::string>& visited) {
    if (visited.find(customName) != visited.end()) {
        return true; // 发现循环依赖
    }
    
    visited.insert(customName);
    
    std::vector<std::string> dependencies;
    
    if (type == "Style") {
        auto custom = getCustomStyle(customName);
        if (custom) {
            dependencies.insert(dependencies.end(), custom->inheritedTemplates.begin(), custom->inheritedTemplates.end());
            dependencies.insert(dependencies.end(), custom->inheritedCustoms.begin(), custom->inheritedCustoms.end());
        }
    } else if (type == "Element") {
        auto custom = getCustomElement(customName);
        if (custom) {
            dependencies.insert(dependencies.end(), custom->inheritedTemplates.begin(), custom->inheritedTemplates.end());
            dependencies.insert(dependencies.end(), custom->inheritedCustoms.begin(), custom->inheritedCustoms.end());
        }
    } else if (type == "Var") {
        auto custom = getCustomVariable(customName);
        if (custom) {
            dependencies.insert(dependencies.end(), custom->inheritedTemplates.begin(), custom->inheritedTemplates.end());
            dependencies.insert(dependencies.end(), custom->inheritedCustoms.begin(), custom->inheritedCustoms.end());
        }
    }
    
    for (const auto& dep : dependencies) {
        if (checkCircularDependency(dep, type, visited)) {
            return true;
        }
    }
    
    visited.erase(customName);
    return false;
}

// === 工具方法 ===

std::vector<std::string> CustomManager::parseCommaSeparatedList(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // 去除前后空白
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

} // namespace custom_system
} // namespace chtl