#include "CHTL/CHTLGlobalMap.hpp"

namespace CHTL {

CHTLGlobalMap::CHTLGlobalMap() {
    // 初始化默认配置
}

// 模板管理实现
bool CHTLGlobalMap::RegisterTemplate(const String& name, const String& type, const TemplateInfo& info) {
    templates_[type][name] = info;
    return true;
}

Optional<CHTLGlobalMap::TemplateInfo> CHTLGlobalMap::GetTemplate(const String& name, const String& type) const {
    auto typeIt = templates_.find(type);
    if (typeIt != templates_.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return {};
}

StringList CHTLGlobalMap::GetTemplatesByType(const String& type) const {
    StringList result;
    auto typeIt = templates_.find(type);
    if (typeIt != templates_.end()) {
        for (const auto& pair : typeIt->second) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool CHTLGlobalMap::RemoveTemplate(const String& name, const String& type) {
    auto typeIt = templates_.find(type);
    if (typeIt != templates_.end()) {
        return typeIt->second.erase(name) > 0;
    }
    return false;
}

// 自定义管理实现
bool CHTLGlobalMap::RegisterCustom(const String& name, const String& type, const CustomInfo& info) {
    customs_[type][name] = info;
    return true;
}

Optional<CHTLGlobalMap::CustomInfo> CHTLGlobalMap::GetCustom(const String& name, const String& type) const {
    auto typeIt = customs_.find(type);
    if (typeIt != customs_.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return {};
}

StringList CHTLGlobalMap::GetCustomsByType(const String& type) const {
    StringList result;
    auto typeIt = customs_.find(type);
    if (typeIt != customs_.end()) {
        for (const auto& pair : typeIt->second) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool CHTLGlobalMap::RemoveCustom(const String& name, const String& type) {
    auto typeIt = customs_.find(type);
    if (typeIt != customs_.end()) {
        return typeIt->second.erase(name) > 0;
    }
    return false;
}

// 变量组管理实现
bool CHTLGlobalMap::RegisterVariableGroup(const String& name, const VariableGroup& group) {
    variableGroups_[name] = group;
    return true;
}

Optional<CHTLGlobalMap::VariableGroup> CHTLGlobalMap::GetVariableGroup(const String& name) const {
    auto it = variableGroups_.find(name);
    if (it != variableGroups_.end()) {
        return it->second;
    }
    return {};
}

StringList CHTLGlobalMap::GetAllVariableGroups() const {
    StringList result;
    for (const auto& pair : variableGroups_) {
        result.push_back(pair.first);
    }
    return result;
}

bool CHTLGlobalMap::RemoveVariableGroup(const String& name) {
    return variableGroups_.erase(name) > 0;
}

// 命名空间管理实现
bool CHTLGlobalMap::RegisterNamespace(const String& name, const NamespaceInfo& info) {
    namespaces_[name] = info;
    return true;
}

Optional<CHTLGlobalMap::NamespaceInfo> CHTLGlobalMap::GetNamespace(const String& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return {};
}

StringList CHTLGlobalMap::GetAllNamespaces() const {
    StringList result;
    for (const auto& pair : namespaces_) {
        result.push_back(pair.first);
    }
    return result;
}

bool CHTLGlobalMap::RemoveNamespace(const String& name) {
    return namespaces_.erase(name) > 0;
}

// 原始嵌入管理实现
bool CHTLGlobalMap::RegisterOrigin(const String& name, const String& type, const OriginInfo& info) {
    origins_[type][name] = info;
    return true;
}

Optional<CHTLGlobalMap::OriginInfo> CHTLGlobalMap::GetOrigin(const String& name, const String& type) const {
    auto typeIt = origins_.find(type);
    if (typeIt != origins_.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return {};
}

StringList CHTLGlobalMap::GetOriginsByType(const String& type) const {
    StringList result;
    auto typeIt = origins_.find(type);
    if (typeIt != origins_.end()) {
        for (const auto& pair : typeIt->second) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool CHTLGlobalMap::RemoveOrigin(const String& name, const String& type) {
    auto typeIt = origins_.find(type);
    if (typeIt != origins_.end()) {
        return typeIt->second.erase(name) > 0;
    }
    return false;
}

// 导入管理实现
bool CHTLGlobalMap::RegisterImport(const String& path, const ImportInfo& info) {
    imports_[path] = info;
    return true;
}

Optional<CHTLGlobalMap::ImportInfo> CHTLGlobalMap::GetImport(const String& path) const {
    auto it = imports_.find(path);
    if (it != imports_.end()) {
        return it->second;
    }
    return {};
}

StringList CHTLGlobalMap::GetAllImports() const {
    StringList result;
    for (const auto& pair : imports_) {
        result.push_back(pair.first);
    }
    return result;
}

bool CHTLGlobalMap::RemoveImport(const String& path) {
    return imports_.erase(path) > 0;
}

// 配置管理实现
bool CHTLGlobalMap::RegisterConfiguration(const String& name, const ConfigurationInfo& info) {
    configurations_[name] = info;
    return true;
}

Optional<CHTLGlobalMap::ConfigurationInfo> CHTLGlobalMap::GetConfiguration(const String& name) const {
    auto it = configurations_.find(name);
    if (it != configurations_.end()) {
        return it->second;
    }
    return {};
}

CHTLGlobalMap::ConfigurationInfo CHTLGlobalMap::GetActiveConfiguration() const {
    if (!activeConfiguration_.empty()) {
        auto config = GetConfiguration(activeConfiguration_);
        if (config.has_value()) {
            return config.value();
        }
    }
    
    // 返回默认配置
    if (!configurations_.empty()) {
        return configurations_.begin()->second;
    }
    
    // 返回空配置
    return ConfigurationInfo{};
}

bool CHTLGlobalMap::SetActiveConfiguration(const String& name) {
    if (configurations_.find(name) != configurations_.end()) {
        activeConfiguration_ = name;
        return true;
    }
    return false;
}

// 实用方法实现
bool CHTLGlobalMap::IsTemplateExists(const String& name, const String& type) const {
    return GetTemplate(name, type).has_value();
}

bool CHTLGlobalMap::IsCustomExists(const String& name, const String& type) const {
    return GetCustom(name, type).has_value();
}

bool CHTLGlobalMap::IsVariableGroupExists(const String& name) const {
    return GetVariableGroup(name).has_value();
}

bool CHTLGlobalMap::IsNamespaceExists(const String& name) const {
    return GetNamespace(name).has_value();
}

bool CHTLGlobalMap::IsOriginExists(const String& name, const String& type) const {
    return GetOrigin(name, type).has_value();
}

bool CHTLGlobalMap::IsImportExists(const String& path) const {
    return GetImport(path).has_value();
}

// 解析辅助方法实现
String CHTLGlobalMap::ResolveVariableReference(const String& groupName, const String& varName) const {
    auto group = GetVariableGroup(groupName);
    if (group.has_value()) {
        auto it = group->variables.find(varName);
        if (it != group->variables.end()) {
            return it->second;
        }
    }
    return "";
}

String CHTLGlobalMap::ResolveNamespacedReference(const String& namespacePath, const String& itemName, const String& itemType) const {
    // 简单实现：直接在指定命名空间中查找
    auto ns = GetNamespace(namespacePath);
    if (ns.has_value()) {
        if (itemType == "template") {
            if (ns->templates.find(itemName) != ns->templates.end()) {
                return namespacePath + "::" + itemName;
            }
        } else if (itemType == "custom") {
            if (ns->customs.find(itemName) != ns->customs.end()) {
                return namespacePath + "::" + itemName;
            }
        } else if (itemType == "variable") {
            if (ns->variables.find(itemName) != ns->variables.end()) {
                return namespacePath + "::" + itemName;
            }
        }
    }
    return "";
}

// 清理方法实现
void CHTLGlobalMap::Clear() {
    ClearTemplates();
    ClearCustoms();
    ClearVariableGroups();
    ClearNamespaces();
    ClearOrigins();
    ClearImports();
    ClearConfigurations();
}

void CHTLGlobalMap::ClearTemplates() {
    templates_.clear();
}

void CHTLGlobalMap::ClearCustoms() {
    customs_.clear();
}

void CHTLGlobalMap::ClearVariableGroups() {
    variableGroups_.clear();
}

void CHTLGlobalMap::ClearNamespaces() {
    namespaces_.clear();
}

void CHTLGlobalMap::ClearOrigins() {
    origins_.clear();
}

void CHTLGlobalMap::ClearImports() {
    imports_.clear();
}

void CHTLGlobalMap::ClearConfigurations() {
    configurations_.clear();
    activeConfiguration_.clear();
}

} // namespace CHTL