#include "../../include/GlobalMap.h"
#include <fstream>

namespace chtl {

GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

void GlobalMap::clear() {
    templates_.clear();
    customs_.clear();
    variables_.clear();
    origins_.clear();
    imports_.clear();
    namespaces_.clear();
    currentNamespace_.clear();
}

void GlobalMap::addTemplate(const TemplateItem& item) {
    String key = makeKey(item.name, item.namespace_);
    templates_[key] = std::unique_ptr<TemplateItem>(new TemplateItem(item));
}

TemplateItem* GlobalMap::getTemplate(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    auto it = templates_.find(key);
    return it != templates_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasTemplate(const String& name, const String& namespace_) const {
    String key = makeKey(name, namespace_);
    return templates_.find(key) != templates_.end();
}

void GlobalMap::removeTemplate(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    templates_.erase(key);
}

std::vector<TemplateItem*> GlobalMap::getTemplatesByType(TemplateType type, const String& namespace_) {
    std::vector<TemplateItem*> result;
    for (auto& [key, template_] : templates_) {
        if (template_->type == type && (namespace_.empty() || template_->namespace_ == namespace_)) {
            result.push_back(template_.get());
        }
    }
    return result;
}

void GlobalMap::addCustom(const CustomItem& item) {
    String key = makeKey(item.name, item.namespace_);
    customs_[key] = std::unique_ptr<CustomItem>(new CustomItem(item));
}

CustomItem* GlobalMap::getCustom(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    auto it = customs_.find(key);
    return it != customs_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasCustom(const String& name, const String& namespace_) const {
    String key = makeKey(name, namespace_);
    return customs_.find(key) != customs_.end();
}

void GlobalMap::removeCustom(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    customs_.erase(key);
}

std::vector<CustomItem*> GlobalMap::getCustomsByType(CustomType type, const String& namespace_) {
    std::vector<CustomItem*> result;
    for (auto& [key, custom] : customs_) {
        if (custom->type == type && (namespace_.empty() || custom->namespace_ == namespace_)) {
            result.push_back(custom.get());
        }
    }
    return result;
}

void GlobalMap::addVariable(const VariableItem& item) {
    String key = makeKey(item.name, item.namespace_);
    variables_[key] = std::unique_ptr<VariableItem>(new VariableItem(item));
}

VariableItem* GlobalMap::getVariable(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    auto it = variables_.find(key);
    return it != variables_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasVariable(const String& name, const String& namespace_) const {
    String key = makeKey(name, namespace_);
    return variables_.find(key) != variables_.end();
}

void GlobalMap::removeVariable(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    variables_.erase(key);
}

String GlobalMap::getVariableValue(const String& name, const String& namespace_) {
    VariableItem* var = getVariable(name, namespace_);
    return var ? var->value : "";
}

void GlobalMap::addOrigin(const OriginItem& item) {
    String key = makeKey(item.name, item.namespace_);
    origins_[key] = std::unique_ptr<OriginItem>(new OriginItem(item));
}

OriginItem* GlobalMap::getOrigin(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    auto it = origins_.find(key);
    return it != origins_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasOrigin(const String& name, const String& namespace_) const {
    String key = makeKey(name, namespace_);
    return origins_.find(key) != origins_.end();
}

void GlobalMap::removeOrigin(const String& name, const String& namespace_) {
    String key = makeKey(name, namespace_);
    origins_.erase(key);
}

std::vector<OriginItem*> GlobalMap::getOriginsByType(const String& type, const String& namespace_) {
    std::vector<OriginItem*> result;
    for (auto& [key, origin] : origins_) {
        if (origin->type == type && (namespace_.empty() || origin->namespace_ == namespace_)) {
            result.push_back(origin.get());
        }
    }
    return result;
}

void GlobalMap::addImport(const ImportItem& item) {
    String key = makeKey(item.path, item.namespace_);
    imports_[key] = std::unique_ptr<ImportItem>(new ImportItem(item));
}

ImportItem* GlobalMap::getImport(const String& path, const String& namespace_) {
    String key = makeKey(path, namespace_);
    auto it = imports_.find(key);
    return it != imports_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasImport(const String& path, const String& namespace_) const {
    String key = makeKey(path, namespace_);
    return imports_.find(key) != imports_.end();
}

void GlobalMap::removeImport(const String& path, const String& namespace_) {
    String key = makeKey(path, namespace_);
    imports_.erase(key);
}

std::vector<ImportItem*> GlobalMap::getImportsByType(const String& type, const String& namespace_) {
    std::vector<ImportItem*> result;
    for (auto& [key, import] : imports_) {
        if (import->type == type && (namespace_.empty() || import->namespace_ == namespace_)) {
            result.push_back(import.get());
        }
    }
    return result;
}

void GlobalMap::addNamespace(const NamespaceItem& item) {
    namespaces_[item.name] = std::unique_ptr<NamespaceItem>(new NamespaceItem(item));
}

NamespaceItem* GlobalMap::getNamespace(const String& name) {
    auto it = namespaces_.find(name);
    return it != namespaces_.end() ? it->second.get() : nullptr;
}

bool GlobalMap::hasNamespace(const String& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

void GlobalMap::removeNamespace(const String& name) {
    namespaces_.erase(name);
}

String GlobalMap::resolveFullNamespace(const String& current, const String& target) {
    if (target.find('.') != String::npos) {
        return target; // 已经是完整命名空间
    }
    
    if (current.empty()) {
        return target;
    }
    
    return current + "." + target;
}

std::vector<String> GlobalMap::getNamespaceHierarchy(const String& namespace_) {
    std::vector<String> hierarchy;
    String current = namespace_;
    
    while (!current.empty()) {
        hierarchy.push_back(current);
        size_t pos = current.rfind('.');
        if (pos == String::npos) {
            break;
        }
        current = current.substr(0, pos);
    }
    
    return hierarchy;
}

String GlobalMap::resolveTemplateName(const String& name, const String& currentNamespace) {
    // 首先在当前命名空间查找
    if (hasTemplate(name, currentNamespace)) {
        return makeKey(name, currentNamespace);
    }
    
    // 在父命名空间查找
    auto hierarchy = getNamespaceHierarchy(currentNamespace);
    for (const auto& ns : hierarchy) {
        if (hasTemplate(name, ns)) {
            return makeKey(name, ns);
        }
    }
    
    // 在全局命名空间查找
    if (hasTemplate(name, "")) {
        return makeKey(name, "");
    }
    
    return ""; // 未找到
}

String GlobalMap::resolveCustomName(const String& name, const String& currentNamespace) {
    // 类似于resolveTemplateName的逻辑
    if (hasCustom(name, currentNamespace)) {
        return makeKey(name, currentNamespace);
    }
    
    auto hierarchy = getNamespaceHierarchy(currentNamespace);
    for (const auto& ns : hierarchy) {
        if (hasCustom(name, ns)) {
            return makeKey(name, ns);
        }
    }
    
    if (hasCustom(name, "")) {
        return makeKey(name, "");
    }
    
    return "";
}

String GlobalMap::resolveVariableName(const String& name, const String& currentNamespace) {
    // 类似于resolveTemplateName的逻辑
    if (hasVariable(name, currentNamespace)) {
        return makeKey(name, currentNamespace);
    }
    
    auto hierarchy = getNamespaceHierarchy(currentNamespace);
    for (const auto& ns : hierarchy) {
        if (hasVariable(name, ns)) {
            return makeKey(name, ns);
        }
    }
    
    if (hasVariable(name, "")) {
        return makeKey(name, "");
    }
    
    return "";
}

std::vector<String> GlobalMap::getTemplateDependencies(const String& name, const String& namespace_) {
    TemplateItem* template_ = getTemplate(name, namespace_);
    return template_ ? template_->dependencies : std::vector<String>();
}

std::vector<String> GlobalMap::getCustomDependencies(const String& name, const String& namespace_) {
    CustomItem* custom = getCustom(name, namespace_);
    return custom ? custom->dependencies : std::vector<String>();
}

bool GlobalMap::hasCircularDependency(const String& name, const String& namespace_) {
    std::unordered_set<String> visited;
    std::vector<String> dependencies;
    collectDependencies(name, namespace_, visited, dependencies);
    
    String key = makeKey(name, namespace_);
    return visited.find(key) != visited.end();
}

bool GlobalMap::hasNameConflict(const String& name, const String& namespace_) const {
    int count = 0;
    if (hasTemplate(name, namespace_)) count++;
    if (hasCustom(name, namespace_)) count++;
    if (hasVariable(name, namespace_)) count++;
    if (hasOrigin(name, namespace_)) count++;
    
    return count > 1;
}

std::vector<String> GlobalMap::getConflictingNames(const String& name, const String& namespace_) const {
    std::vector<String> conflicts;
    
    if (hasTemplate(name, namespace_)) conflicts.push_back("Template");
    if (hasCustom(name, namespace_)) conflicts.push_back("Custom");
    if (hasVariable(name, namespace_)) conflicts.push_back("Variable");
    if (hasOrigin(name, namespace_)) conflicts.push_back("Origin");
    
    return conflicts;
}

GlobalMap::Statistics GlobalMap::getStatistics() const {
    Statistics stats;
    stats.templateCount = templates_.size();
    stats.customCount = customs_.size();
    stats.variableCount = variables_.size();
    stats.originCount = origins_.size();
    stats.importCount = imports_.size();
    stats.namespaceCount = namespaces_.size();
    
    return stats;
}

String GlobalMap::dumpToString() const {
    String result = "=== GlobalMap Dump ===\n";
    
    result += "Templates: " + std::to_string(templates_.size()) + "\n";
    for (const auto& [key, template_] : templates_) {
        result += "  " + key + " (" + std::to_string(static_cast<int>(template_->type)) + ")\n";
    }
    
    result += "Customs: " + std::to_string(customs_.size()) + "\n";
    for (const auto& [key, custom] : customs_) {
        result += "  " + key + " (" + std::to_string(static_cast<int>(custom->type)) + ")\n";
    }
    
    result += "Variables: " + std::to_string(variables_.size()) + "\n";
    for (const auto& [key, variable] : variables_) {
        result += "  " + key + " = " + variable->value + "\n";
    }
    
    result += "Origins: " + std::to_string(origins_.size()) + "\n";
    for (const auto& [key, origin] : origins_) {
        result += "  " + key + " (" + origin->type + ")\n";
    }
    
    result += "Imports: " + std::to_string(imports_.size()) + "\n";
    for (const auto& [key, import] : imports_) {
        result += "  " + key + " (" + import->type + ")\n";
    }
    
    result += "Namespaces: " + std::to_string(namespaces_.size()) + "\n";
    for (const auto& [name, namespace_] : namespaces_) {
        result += "  " + name + "\n";
    }
    
    return result;
}

void GlobalMap::dumpToFile(const String& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << dumpToString();
        file.close();
    }
}

String GlobalMap::makeKey(const String& name, const String& namespace_) const {
    if (namespace_.empty()) {
        return name;
    }
    return namespace_ + "::" + name;
}

std::pair<String, String> GlobalMap::parseKey(const String& key) const {
    size_t pos = key.find("::");
    if (pos == String::npos) {
        return {key, ""};
    }
    return {key.substr(pos + 2), key.substr(0, pos)};
}

String GlobalMap::getCurrentNamespace() const {
    return currentNamespace_;
}

void GlobalMap::setCurrentNamespace(const String& namespace_) {
    currentNamespace_ = namespace_;
}

void GlobalMap::collectDependencies(const String& name, const String& namespace_, 
                                  std::unordered_set<String>& visited, 
                                  std::vector<String>& dependencies) {
    String key = makeKey(name, namespace_);
    if (visited.find(key) != visited.end()) {
        return; // 已访问过，可能有循环依赖
    }
    
    visited.insert(key);
    
    // 收集模板依赖
    TemplateItem* template_ = getTemplate(name, namespace_);
    if (template_) {
        for (const auto& dep : template_->dependencies) {
            dependencies.push_back(dep);
            auto [depName, depNs] = parseKey(dep);
            collectDependencies(depName, depNs, visited, dependencies);
        }
    }
    
    // 收集自定义依赖
    CustomItem* custom = getCustom(name, namespace_);
    if (custom) {
        for (const auto& dep : custom->dependencies) {
            dependencies.push_back(dep);
            auto [depName, depNs] = parseKey(dep);
            collectDependencies(depName, depNs, visited, dependencies);
        }
    }
}

} // namespace chtl