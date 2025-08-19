#include "TemplateManager.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>

namespace chtl {
namespace template_system {

// 全局模板管理器实例
std::shared_ptr<TemplateManager> g_templateManager = std::make_shared<TemplateManager>();

TemplateManager::TemplateManager() = default;
TemplateManager::~TemplateManager() = default;

// === 样式组模板管理 ===

void TemplateManager::addStyleTemplate(const std::string& name, const StyleTemplate& tmpl) {
    auto templatePtr = std::make_shared<StyleTemplate>(tmpl);
    styleTemplates_[name] = templatePtr;
    processInheritance(*templatePtr);
}

std::shared_ptr<StyleTemplate> TemplateManager::getStyleTemplate(const std::string& name) {
    auto it = styleTemplates_.find(name);
    return (it != styleTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasStyleTemplate(const std::string& name) const {
    return styleTemplates_.find(name) != styleTemplates_.end();
}

// === 元素模板管理 ===

void TemplateManager::addElementTemplate(const std::string& name, ElementTemplate&& tmpl) {
    tmpl.name = name;
    auto templatePtr = std::make_shared<ElementTemplate>(std::move(tmpl));
    elementTemplates_[name] = templatePtr;
    processInheritance(*templatePtr);
}

std::shared_ptr<ElementTemplate> TemplateManager::getElementTemplate(const std::string& name) {
    auto it = elementTemplates_.find(name);
    return (it != elementTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasElementTemplate(const std::string& name) const {
    return elementTemplates_.find(name) != elementTemplates_.end();
}

// === 变量组模板管理 ===

void TemplateManager::addVariableTemplate(const std::string& name, const VariableTemplate& tmpl) {
    auto templatePtr = std::make_shared<VariableTemplate>(tmpl);
    variableTemplates_[name] = templatePtr;
    processInheritance(*templatePtr);
}

std::shared_ptr<VariableTemplate> TemplateManager::getVariableTemplate(const std::string& name) {
    auto it = variableTemplates_.find(name);
    return (it != variableTemplates_.end()) ? it->second : nullptr;
}

bool TemplateManager::hasVariableTemplate(const std::string& name) const {
    return variableTemplates_.find(name) != variableTemplates_.end();
}

// === 模板继承处理 ===

std::unordered_map<std::string, std::string> TemplateManager::resolveStyleInheritance(const std::string& templateName) {
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::string> result;
    
    resolveStyleInheritanceRecursive(templateName, visited, result);
    return result;
}

void TemplateManager::resolveStyleInheritanceRecursive(const std::string& templateName, 
                                                      std::unordered_set<std::string>& visited,
                                                      std::unordered_map<std::string, std::string>& result) {
    // 检查循环依赖
    if (visited.find(templateName) != visited.end()) {
        errors_.push_back("检测到样式模板循环继承: " + templateName);
        return;
    }
    
    visited.insert(templateName);
    
    auto tmpl = getStyleTemplate(templateName);
    if (!tmpl) {
        errors_.push_back("样式模板不存在: " + templateName);
        visited.erase(templateName);
        return;
    }
    
    // 首先处理继承的模板（按语法文档：继承的属性先应用）
    for (const auto& inheritedName : tmpl->inheritedTemplates) {
        resolveStyleInheritanceRecursive(inheritedName, visited, result);
    }
    
    // 然后应用当前模板的属性（按语法文档：后定义的属性覆盖前面的）
    for (const auto& [key, value] : tmpl->properties) {
        result[key] = value;
    }
    
    visited.erase(templateName);
}

std::vector<std::unique_ptr<ast::ASTNode>> TemplateManager::resolveElementInheritance(const std::string& templateName) {
    std::vector<std::unique_ptr<ast::ASTNode>> result;
    
    auto tmpl = getElementTemplate(templateName);
    if (!tmpl) {
        errors_.push_back("元素模板不存在: " + templateName);
        return result;
    }
    
    // 按语法文档：元素模板继承需要深拷贝AST节点
    // TODO: 实现AST节点深拷贝机制
    errors_.push_back("元素模板继承需要AST节点深拷贝支持");
    
    return result;
}

std::unordered_map<std::string, std::string> TemplateManager::resolveVariableInheritance(const std::string& templateName) {
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::string> result;
    
    resolveVariableInheritanceRecursive(templateName, visited, result);
    return result;
}

void TemplateManager::resolveVariableInheritanceRecursive(const std::string& templateName,
                                                         std::unordered_set<std::string>& visited,
                                                         std::unordered_map<std::string, std::string>& result) {
    // 检查循环依赖
    if (visited.find(templateName) != visited.end()) {
        errors_.push_back("检测到变量模板循环继承: " + templateName);
        return;
    }
    
    visited.insert(templateName);
    
    auto tmpl = getVariableTemplate(templateName);
    if (!tmpl) {
        errors_.push_back("变量模板不存在: " + templateName);
        visited.erase(templateName);
        return;
    }
    
    // 首先处理继承的模板
    for (const auto& inheritedName : tmpl->inheritedTemplates) {
        resolveVariableInheritanceRecursive(inheritedName, visited, result);
    }
    
    // 然后应用当前模板的变量
    for (const auto& [key, value] : tmpl->variables) {
        result[key] = value;
    }
    
    visited.erase(templateName);
}

// === 模板使用 ===

bool TemplateManager::applyStyleTemplate(const std::string& templateName, 
                                        std::unordered_map<std::string, std::string>& targetProperties) {
    auto resolvedProps = resolveStyleInheritance(templateName);
    
    if (resolvedProps.empty() && hasStyleTemplate(templateName)) {
        // 模板存在但没有属性（空模板）
        return true;
    }
    
    // 应用解析后的属性
    for (const auto& [key, value] : resolvedProps) {
        targetProperties[key] = value;
    }
    
    return !resolvedProps.empty();
}

std::vector<std::unique_ptr<ast::ASTNode>> TemplateManager::instantiateElementTemplate(const std::string& templateName) {
    return resolveElementInheritance(templateName);
}

std::unordered_map<std::string, std::string> TemplateManager::getVariableValues(const std::string& templateName) {
    return resolveVariableInheritance(templateName);
}

// === 模板验证 ===

bool TemplateManager::validateTemplate(const std::string& name, TemplateType type) {
    switch (type) {
        case TemplateType::STYLE:
            return hasStyleTemplate(name);
        case TemplateType::ELEMENT:
            return hasElementTemplate(name);
        case TemplateType::VAR:
            return hasVariableTemplate(name);
        default:
            return false;
    }
}

// === 清理和统计 ===

void TemplateManager::clear() {
    styleTemplates_.clear();
    elementTemplates_.clear();
    variableTemplates_.clear();
    clearErrors();
}

std::unordered_map<TemplateType, int> TemplateManager::getTemplateStatistics() {
    std::unordered_map<TemplateType, int> stats;
    stats[TemplateType::STYLE] = static_cast<int>(styleTemplates_.size());
    stats[TemplateType::ELEMENT] = static_cast<int>(elementTemplates_.size());
    stats[TemplateType::VAR] = static_cast<int>(variableTemplates_.size());
    return stats;
}

// === 继承处理 ===

void TemplateManager::processInheritance(StyleTemplate& tmpl) {
    // 验证继承的模板是否存在
    for (const auto& inheritedName : tmpl.inheritedTemplates) {
        if (!hasStyleTemplate(inheritedName)) {
            errors_.push_back("继承的样式模板不存在: " + inheritedName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(tmpl.name, TemplateType::STYLE, visited)) {
        errors_.push_back("检测到样式模板循环依赖: " + tmpl.name);
    }
}

void TemplateManager::processInheritance(ElementTemplate& tmpl) {
    // 验证继承的模板是否存在
    for (const auto& inheritedName : tmpl.inheritedTemplates) {
        if (!hasElementTemplate(inheritedName)) {
            errors_.push_back("继承的元素模板不存在: " + inheritedName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(tmpl.name, TemplateType::ELEMENT, visited)) {
        errors_.push_back("检测到元素模板循环依赖: " + tmpl.name);
    }
}

void TemplateManager::processInheritance(VariableTemplate& tmpl) {
    // 验证继承的模板是否存在
    for (const auto& inheritedName : tmpl.inheritedTemplates) {
        if (!hasVariableTemplate(inheritedName)) {
            errors_.push_back("继承的变量模板不存在: " + inheritedName);
        }
    }
    
    // 检查循环依赖
    std::unordered_set<std::string> visited;
    if (checkCircularDependency(tmpl.name, TemplateType::VAR, visited)) {
        errors_.push_back("检测到变量模板循环依赖: " + tmpl.name);
    }
}

bool TemplateManager::checkCircularDependency(const std::string& templateName, TemplateType type, 
                                             std::unordered_set<std::string>& visited) {
    if (visited.find(templateName) != visited.end()) {
        return true; // 发现循环依赖
    }
    
    visited.insert(templateName);
    
    std::vector<std::string> dependencies;
    
    switch (type) {
        case TemplateType::STYLE: {
            auto tmpl = getStyleTemplate(templateName);
            if (tmpl) dependencies = tmpl->inheritedTemplates;
            break;
        }
        case TemplateType::ELEMENT: {
            auto tmpl = getElementTemplate(templateName);
            if (tmpl) dependencies = tmpl->inheritedTemplates;
            break;
        }
        case TemplateType::VAR: {
            auto tmpl = getVariableTemplate(templateName);
            if (tmpl) dependencies = tmpl->inheritedTemplates;
            break;
        }
    }
    
    for (const auto& dep : dependencies) {
        if (checkCircularDependency(dep, type, visited)) {
            return true;
        }
    }
    
    visited.erase(templateName);
    return false;
}

} // namespace template_system
} // namespace chtl