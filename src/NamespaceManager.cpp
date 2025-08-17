#include "NamespaceManager.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace chtl {

// ==================== Namespace ====================

Namespace::Namespace(const String& name, const String& sourceFile)
    : name_(name), sourceFile_(sourceFile) {
}

bool Namespace::addItem(const NamespaceItem& item) {
    // 检查是否已存在相同的项目
    auto& itemList = items_[item.name];
    
    // 检查兼容性
    for (const auto& existingItem : itemList) {
        if (existingItem.type == item.type) {
            // 相同类型的项目冲突
            return false;
        }
    }
    
    itemList.push_back(item);
    return true;
}

bool Namespace::hasItem(const String& name, NamespaceItemType type) const {
    auto it = items_.find(name);
    if (it == items_.end()) {
        return false;
    }
    
    for (const auto& item : it->second) {
        if (item.type == type) {
            return true;
        }
    }
    
    return false;
}

std::shared_ptr<CHTLASTNode> Namespace::getItem(const String& name, NamespaceItemType type) const {
    auto it = items_.find(name);
    if (it == items_.end()) {
        return nullptr;
    }
    
    for (const auto& item : it->second) {
        if (item.type == type) {
            return item.node;
        }
    }
    
    return nullptr;
}

std::vector<NamespaceItem> Namespace::getItemsByName(const String& name) const {
    auto it = items_.find(name);
    if (it != items_.end()) {
        return it->second;
    }
    return {};
}

std::vector<NamespaceItem> Namespace::getAllItems() const {
    std::vector<NamespaceItem> allItems;
    
    for (const auto& pair : items_) {
        for (const auto& item : pair.second) {
            allItems.push_back(item);
        }
    }
    
    return allItems;
}

bool Namespace::addNestedNamespace(std::shared_ptr<Namespace> nestedNs) {
    if (!nestedNs || nestedNs->getName().empty()) {
        return false;
    }
    
    const String& nsName = nestedNs->getName();
    
    // 检查是否已存在同名的嵌套命名空间
    auto it = nestedNamespaces_.find(nsName);
    if (it != nestedNamespaces_.end()) {
        // 尝试合并
        std::vector<NamespaceConflict> conflicts;
        return it->second->mergeWith(*nestedNs, conflicts);
    } else {
        nestedNamespaces_[nsName] = nestedNs;
        return true;
    }
}

std::shared_ptr<Namespace> Namespace::getNestedNamespace(const String& name) const {
    auto it = nestedNamespaces_.find(name);
    return (it != nestedNamespaces_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Namespace>> Namespace::getNestedNamespaces() const {
    std::vector<std::shared_ptr<Namespace>> result;
    
    for (const auto& pair : nestedNamespaces_) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool Namespace::mergeWith(const Namespace& other, std::vector<NamespaceConflict>& conflicts) {
    bool success = true;
    
    // 合并项目
    for (const auto& pair : other.items_) {
        const String& itemName = pair.first;
        const auto& otherItems = pair.second;
        
        for (const auto& otherItem : otherItems) {
            // 检查冲突
            if (hasItem(itemName, otherItem.type)) {
                // 发现冲突
                auto existingItem = getItem(itemName, otherItem.type);
                
                NamespaceConflict conflict;
                conflict.itemName = itemName;
                conflict.type = otherItem.type;
                conflict.existingSource = sourceFile_;
                conflict.newSource = other.sourceFile_;
                conflict.existingNode = existingItem;
                conflict.newNode = otherItem.node;
                
                conflicts.push_back(conflict);
                success = false;
            } else {
                // 无冲突，直接添加
                NamespaceItem newItem = otherItem;
                items_[itemName].push_back(newItem);
            }
        }
    }
    
    // 合并嵌套命名空间
    for (const auto& pair : other.nestedNamespaces_) {
        const String& nsName = pair.first;
        const auto& otherNs = pair.second;
        
        auto it = nestedNamespaces_.find(nsName);
        if (it != nestedNamespaces_.end()) {
            // 递归合并嵌套命名空间
            std::vector<NamespaceConflict> nestedConflicts;
            bool mergeSuccess = it->second->mergeWith(*otherNs, nestedConflicts);
            
            if (!mergeSuccess) {
                conflicts.insert(conflicts.end(), nestedConflicts.begin(), nestedConflicts.end());
                success = false;
            }
        } else {
            // 直接添加新的嵌套命名空间
            nestedNamespaces_[nsName] = std::make_shared<Namespace>(*otherNs);
        }
    }
    
    return success;
}

bool Namespace::resolveConflict(const NamespaceConflict& conflict, ConflictResolutionStrategy strategy) {
    switch (strategy) {
        case ConflictResolutionStrategy::KEEP_EXISTING:
            // 保持现有项目，不做任何操作
            return true;
            
        case ConflictResolutionStrategy::REPLACE_WITH_NEW:
            // 用新项目替换现有项目
            {
                auto& itemList = items_[conflict.itemName];
                for (auto it = itemList.begin(); it != itemList.end(); ++it) {
                    if (it->type == conflict.type) {
                        *it = NamespaceItem{
                            conflict.itemName,
                            conflict.type,
                            conflict.newNode,
                            conflict.newSource
                        };
                        return true;
                    }
                }
            }
            break;
            
        case ConflictResolutionStrategy::CREATE_ALIAS:
            // 为新项目创建别名
            {
                String aliasName = conflict.itemName + "_" + generateUniqueId();
                NamespaceItem aliasItem{
                    aliasName,
                    conflict.type,
                    conflict.newNode,
                    conflict.newSource
                };
                items_[aliasName].push_back(aliasItem);
                return true;
            }
            break;
            
        case ConflictResolutionStrategy::FAIL:
        default:
            return false;
    }
    
    return false;
}

String Namespace::generateUniqueId() {
    static int counter = 0;
    return std::to_string(++counter);
}

NamespaceResolutionResult Namespace::resolve(const String& path) const {
    NamespaceResolutionResult result;
    
    // 解析路径（支持 . 和 / 分隔符）
    std::vector<String> parts;
    std::stringstream ss(path);
    String part;
    
    // 替换 / 为 .
    String normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '/', '.');
    
    ss.str(normalizedPath);
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    if (parts.empty()) {
        result.error = "Empty resolution path";
        return result;
    }
    
    if (parts.size() == 1) {
        // 直接在当前命名空间查找
        const String& itemName = parts[0];
        auto items = getItemsByName(itemName);
        
        if (!items.empty()) {
            result.success = true;
            result.resolvedNode = items[0].node; // 返回第一个匹配项
            result.fullyQualifiedName = name_ + "." + itemName;
        } else {
            result.error = "Item not found: " + itemName;
        }
    } else {
        // 递归查找嵌套命名空间
        const String& firstPart = parts[0];
        auto nestedNs = getNestedNamespace(firstPart);
        
        if (nestedNs) {
            // 构建剩余路径
            String remainingPath;
            for (size_t i = 1; i < parts.size(); ++i) {
                if (i > 1) remainingPath += ".";
                remainingPath += parts[i];
            }
            
            result = nestedNs->resolve(remainingPath);
        } else {
            result.error = "Namespace not found: " + firstPart;
        }
    }
    
    return result;
}

NamespaceResolutionResult Namespace::resolveRecursive(const String& path) const {
    // 首先尝试直接解析
    auto result = resolve(path);
    if (result.success) {
        return result;
    }
    
    // 如果失败，在所有嵌套命名空间中递归查找
    for (const auto& pair : nestedNamespaces_) {
        auto nestedResult = pair.second->resolveRecursive(path);
        if (nestedResult.success) {
            return nestedResult;
        }
    }
    
    return result; // 返回原始错误
}

std::vector<NamespaceConflict> Namespace::detectConflicts() const {
    std::vector<NamespaceConflict> conflicts;
    
    // 检查项目冲突
    for (const auto& pair : items_) {
        const String& itemName = pair.first;
        const auto& items = pair.second;
        
        if (items.size() > 1) {
            // 检查是否有相同类型的项目
            std::unordered_map<NamespaceItemType, std::vector<NamespaceItem>> typeGroups;
            
            for (const auto& item : items) {
                typeGroups[item.type].push_back(item);
            }
            
            for (const auto& typeGroup : typeGroups) {
                if (typeGroup.second.size() > 1) {
                    NamespaceConflict conflict(name_, itemName, typeGroup.first);
                    conflict.conflictingItems = typeGroup.second;
                    conflicts.push_back(conflict);
                }
            }
        }
    }
    
    // 递归检查嵌套命名空间的冲突
    for (const auto& pair : nestedNamespaces_) {
        auto nestedConflicts = pair.second->detectConflicts();
        conflicts.insert(conflicts.end(), nestedConflicts.begin(), nestedConflicts.end());
    }
    
    return conflicts;
}

bool Namespace::hasConflicts() const {
    return !detectConflicts().empty();
}

size_t Namespace::getItemCount() const {
    size_t count = 0;
    for (const auto& pair : items_) {
        count += pair.second.size();
    }
    return count;
}

size_t Namespace::getNestedNamespaceCount() const {
    return nestedNamespaces_.size();
}

String Namespace::getItemTypeString(NamespaceItemType type) const {
    switch (type) {
        case NamespaceItemType::TEMPLATE_STYLE: return "Template Style";
        case NamespaceItemType::TEMPLATE_ELEMENT: return "Template Element";
        case NamespaceItemType::TEMPLATE_VAR: return "Template Variable";
        case NamespaceItemType::CUSTOM_STYLE: return "Custom Style";
        case NamespaceItemType::CUSTOM_ELEMENT: return "Custom Element";
        case NamespaceItemType::CUSTOM_VAR: return "Custom Variable";
        case NamespaceItemType::ORIGIN_NODE: return "Origin Node";
        case NamespaceItemType::NESTED_NAMESPACE: return "Nested Namespace";
        default: return "Unknown";
    }
}

bool Namespace::areItemsCompatible(const NamespaceItem& item1, const NamespaceItem& item2) const {
    // 相同类型的项目不兼容（冲突）
    return item1.type != item2.type;
}

// ==================== NamespaceManager ====================

NamespaceManager::NamespaceManager() {
}

bool NamespaceManager::registerNamespace(std::shared_ptr<Namespace> ns) {
    if (!ns || ns->getName().empty()) {
        addError("Invalid namespace");
        return false;
    }
    
    const String& name = ns->getName();
    
    if (!validateNamespaceName(name)) {
        addError("Invalid namespace name: " + name);
        return false;
    }
    
    // 添加到同名命名空间列表中
    namespaces_[name].push_back(ns);
    
    return true;
}

std::shared_ptr<Namespace> NamespaceManager::getNamespace(const String& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end() && !it->second.empty()) {
        // 如果有多个同名命名空间，返回合并后的结果
        if (it->second.size() == 1) {
            return it->second[0];
        } else {
            // 创建临时合并的命名空间
            auto merged = std::make_shared<Namespace>(name);
            std::vector<NamespaceConflict> conflicts;
            
            for (const auto& ns : it->second) {
                merged->mergeWith(*ns, conflicts);
            }
            
            return merged;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Namespace>> NamespaceManager::getAllNamespaces() const {
    std::vector<std::shared_ptr<Namespace>> result;
    
    for (const auto& pair : namespaces_) {
        for (const auto& ns : pair.second) {
            result.push_back(ns);
        }
    }
    
    return result;
}

bool NamespaceManager::hasNamespace(const String& name) const {
    auto it = namespaces_.find(name);
    return it != namespaces_.end() && !it->second.empty();
}

bool NamespaceManager::mergeNamespaces(const String& name) {
    auto it = namespaces_.find(name);
    if (it == namespaces_.end() || it->second.size() <= 1) {
        return true; // 没有需要合并的命名空间
    }
    
    auto& namespaceList = it->second;
    auto mergedNs = namespaceList[0];
    
    std::vector<NamespaceConflict> allConflicts;
    bool success = true;
    
    for (size_t i = 1; i < namespaceList.size(); ++i) {
        std::vector<NamespaceConflict> conflicts;
        if (!mergedNs->mergeWith(*namespaceList[i], conflicts)) {
            allConflicts.insert(allConflicts.end(), conflicts.begin(), conflicts.end());
            success = false;
        }
    }
    
    if (success) {
        // 合并成功，替换命名空间列表
        namespaceList.clear();
        namespaceList.push_back(mergedNs);
    } else {
        // 记录冲突信息
        for (const auto& conflict : allConflicts) {
            addWarning("Namespace merge conflict in " + conflict.namespaceName + 
                      ": " + conflict.itemName + " (" + 
                      mergedNs->getItemTypeString(conflict.itemType) + ")");
        }
    }
    
    return success;
}

std::vector<NamespaceConflict> NamespaceManager::getMergeConflicts(const String& name) const {
    auto it = namespaces_.find(name);
    if (it == namespaces_.end() || it->second.size() <= 1) {
        return {};
    }
    
    const auto& namespaceList = it->second;
    std::vector<NamespaceConflict> allConflicts;
    
    // 模拟合并过程以检测冲突
    auto tempMerged = std::make_shared<Namespace>(name);
    
    for (const auto& ns : namespaceList) {
        std::vector<NamespaceConflict> conflicts;
        tempMerged->mergeWith(*ns, conflicts);
        allConflicts.insert(allConflicts.end(), conflicts.begin(), conflicts.end());
    }
    
    return allConflicts;
}

std::vector<NamespaceConflict> NamespaceManager::detectAllConflicts() const {
    std::vector<NamespaceConflict> allConflicts;
    
    for (const auto& pair : namespaces_) {
        const String& nsName = pair.first;
        const auto& namespaceList = pair.second;
        
        // 检查同名命名空间的合并冲突
        auto mergeConflicts = getMergeConflicts(nsName);
        allConflicts.insert(allConflicts.end(), mergeConflicts.begin(), mergeConflicts.end());
        
        // 检查每个命名空间内部的冲突
        for (const auto& ns : namespaceList) {
            auto internalConflicts = ns->detectConflicts();
            allConflicts.insert(allConflicts.end(), internalConflicts.begin(), internalConflicts.end());
        }
    }
    
    return allConflicts;
}

bool NamespaceManager::hasGlobalConflicts() const {
    return !detectAllConflicts().empty();
}

NamespaceResolutionResult NamespaceManager::resolve(const String& fullyQualifiedName) const {
    NamespaceResolutionResult result;
    
    auto parts = parseQualifiedName(fullyQualifiedName);
    if (parts.empty()) {
        result.error = "Invalid qualified name: " + fullyQualifiedName;
        return result;
    }
    
    const String& namespaceName = parts[0];
    auto ns = getNamespace(namespaceName);
    
    if (!ns) {
        result.error = "Namespace not found: " + namespaceName;
        return result;
    }
    
    if (parts.size() == 1) {
        result.error = "No item specified in qualified name";
        return result;
    }
    
    // 构建项目路径
    String itemPath;
    for (size_t i = 1; i < parts.size(); ++i) {
        if (i > 1) itemPath += ".";
        itemPath += parts[i];
    }
    
    return ns->resolve(itemPath);
}

NamespaceResolutionResult NamespaceManager::resolveWithFallback(const String& name, const String& currentNamespace) const {
    NamespaceResolutionResult result;
    
    // 首先尝试在当前命名空间中查找
    if (!currentNamespace.empty()) {
        auto ns = getNamespace(currentNamespace);
        if (ns) {
            result = ns->resolve(name);
            if (result.success) {
                return result;
            }
        }
    }
    
    // 然后尝试作为全限定名查找
    result = resolve(name);
    if (result.success) {
        return result;
    }
    
    // 最后在所有命名空间中查找
    for (const auto& pair : namespaces_) {
        auto ns = getNamespace(pair.first);
        if (ns) {
            auto tempResult = ns->resolveRecursive(name);
            if (tempResult.success) {
                return tempResult;
            }
        }
    }
    
    result.error = "Item not found: " + name;
    return result;
}

bool NamespaceManager::buildFromAST(std::shared_ptr<CHTLASTNode> ast, const String& sourceFile) {
    if (!ast) {
        addError("Invalid AST node");
        return false;
    }
    
    // 查找命名空间声明
    std::function<void(std::shared_ptr<CHTLASTNode>, std::shared_ptr<Namespace>)> traverseNode = 
        [&](std::shared_ptr<CHTLASTNode> node, std::shared_ptr<Namespace> currentNs) {
            
        if (node->getType() == ASTNodeType::NAMESPACE_DECLARATION) {
            auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(node);
            if (nsNode) {
                auto newNs = std::make_shared<Namespace>(nsNode->getName(), sourceFile);
                
                if (currentNs) {
                    currentNs->addNestedNamespace(newNs);
                } else {
                    registerNamespace(newNs);
                }
                
                // 递归处理子节点
                for (const auto& child : node->getChildren()) {
                    traverseNode(child, newNs);
                }
                
                return;
            }
        }
        
        // 收集命名空间项目
        if (currentNs) {
            collectNamespaceItems(node, currentNs, sourceFile);
        }
        
        // 递归处理子节点
        for (const auto& child : node->getChildren()) {
            traverseNode(child, currentNs);
        }
    };
    
    traverseNode(ast, nullptr);
    return true;
}

bool NamespaceManager::processImportedNamespace(std::shared_ptr<Namespace> importedNs, const String& alias) {
    if (!importedNs) {
        addError("Invalid imported namespace");
        return false;
    }
    
    String targetName = alias.empty() ? importedNs->getName() : alias;
    
    // 创建新的命名空间或合并到现有命名空间
    auto existingNs = getNamespace(targetName);
    if (existingNs) {
        std::vector<NamespaceConflict> conflicts;
        return existingNs->mergeWith(*importedNs, conflicts);
    } else {
        auto newNs = std::make_shared<Namespace>(targetName, importedNs->getSourceFile());
        std::vector<NamespaceConflict> conflicts;
        newNs->mergeWith(*importedNs, conflicts);
        return registerNamespace(newNs);
    }
}

std::vector<String> NamespaceManager::getAvailableItems(const String& namespaceName) const {
    std::vector<String> items;
    
    auto ns = getNamespace(namespaceName);
    if (ns) {
        auto allItems = ns->getAllItems();
        for (const auto& item : allItems) {
            items.push_back(item.name);
        }
    }
    
    // 去重
    std::sort(items.begin(), items.end());
    items.erase(std::unique(items.begin(), items.end()), items.end());
    
    return items;
}

std::vector<String> NamespaceManager::getAvailableNamespaces() const {
    std::vector<String> names;
    
    for (const auto& pair : namespaces_) {
        names.push_back(pair.first);
    }
    
    std::sort(names.begin(), names.end());
    return names;
}

bool NamespaceManager::validateNamespaceName(const String& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查是否包含无效字符
    return isValidNamespaceIdentifier(name);
}

bool NamespaceManager::validateItemName(const String& name, const String& namespaceName) const {
    if (name.empty()) {
        return false;
    }
    
    return isValidNamespaceIdentifier(name);
}

void NamespaceManager::clearNamespace(const String& name) {
    namespaces_.erase(name);
}

void NamespaceManager::clearAll() {
    namespaces_.clear();
    errors_.clear();
    warnings_.clear();
}

// 辅助方法实现

void NamespaceManager::addError(const String& error) {
    errors_.push_back(error);
}

void NamespaceManager::addWarning(const String& warning) {
    warnings_.push_back(warning);
}

std::vector<String> NamespaceManager::parseQualifiedName(const String& qualifiedName) const {
    std::vector<String> parts;
    std::stringstream ss(qualifiedName);
    String part;
    
    // 支持 . 和 / 分隔符
    String normalizedName = qualifiedName;
    std::replace(normalizedName.begin(), normalizedName.end(), '/', '.');
    
    ss.str(normalizedName);
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

String NamespaceManager::joinNamespacePath(const std::vector<String>& parts) const {
    String result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += ".";
        result += parts[i];
    }
    return result;
}

NamespaceItemType NamespaceManager::getItemTypeFromNode(std::shared_ptr<CHTLASTNode> node) const {
    switch (node->getType()) {
        case ASTNodeType::TEMPLATE_STYLE:
            return NamespaceItemType::TEMPLATE_STYLE;
        case ASTNodeType::TEMPLATE_ELEMENT:
            return NamespaceItemType::TEMPLATE_ELEMENT;
        case ASTNodeType::TEMPLATE_VAR:
            return NamespaceItemType::TEMPLATE_VAR;
        case ASTNodeType::CUSTOM_STYLE:
            return NamespaceItemType::CUSTOM_STYLE;
        case ASTNodeType::CUSTOM_ELEMENT:
            return NamespaceItemType::CUSTOM_ELEMENT;
        case ASTNodeType::CUSTOM_VAR:
            return NamespaceItemType::CUSTOM_VAR;
        case ASTNodeType::NAMESPACE_DECLARATION:
            return NamespaceItemType::NESTED_NAMESPACE;
        default:
            return NamespaceItemType::ORIGIN_NODE;
    }
}

void NamespaceManager::collectNamespaceItems(std::shared_ptr<CHTLASTNode> node, std::shared_ptr<Namespace> ns, const String& sourceFile) {
    NamespaceItemType itemType = getItemTypeFromNode(node);
    String itemName;
    
    // 根据节点类型提取名称
    switch (node->getType()) {
        case ASTNodeType::TEMPLATE_STYLE: {
            auto templateNode = std::dynamic_pointer_cast<TemplateStyleNode>(node);
            if (templateNode) {
                itemName = templateNode->getName();
            }
            break;
        }
        case ASTNodeType::TEMPLATE_ELEMENT: {
            auto templateNode = std::dynamic_pointer_cast<TemplateElementNode>(node);
            if (templateNode) {
                itemName = templateNode->getName();
            }
            break;
        }
        case ASTNodeType::TEMPLATE_VAR: {
            auto templateNode = std::dynamic_pointer_cast<TemplateVarNode>(node);
            if (templateNode) {
                itemName = templateNode->getName();
            }
            break;
        }
        case ASTNodeType::CUSTOM_STYLE: {
            auto customNode = std::dynamic_pointer_cast<CustomStyleNode>(node);
            if (customNode) {
                itemName = customNode->getName();
            }
            break;
        }
        case ASTNodeType::CUSTOM_ELEMENT: {
            auto customNode = std::dynamic_pointer_cast<CustomElementNode>(node);
            if (customNode) {
                itemName = customNode->getName();
            }
            break;
        }
        case ASTNodeType::CUSTOM_VAR: {
            auto customNode = std::dynamic_pointer_cast<CustomVarNode>(node);
            if (customNode) {
                itemName = customNode->getName();
            }
            break;
        }
        default:
            return; // 不是命名空间项目
    }
    
    if (!itemName.empty()) {
        NamespaceItem item(itemName, itemType, node, sourceFile, node->getLine(), node->getColumn());
        ns->addItem(item);
    }
}

bool NamespaceManager::isValidNamespaceIdentifier(const String& identifier) const {
    if (identifier.empty()) {
        return false;
    }
    
    // 简单的标识符验证：只允许字母、数字、下划线
    std::regex identifierRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(identifier, identifierRegex);
}

// ==================== NamespaceAccessor ====================

NamespaceAccessor::NamespaceAccessor(NamespaceManager& manager, const String& currentNamespace)
    : manager_(manager), currentNamespace_(currentNamespace) {
}

void NamespaceAccessor::setCurrentNamespace(const String& namespaceName) {
    currentNamespace_ = namespaceName;
}

NamespaceResolutionResult NamespaceAccessor::resolve(const String& name) const {
    return manager_.resolveWithFallback(name, currentNamespace_);
}

bool NamespaceAccessor::exists(const String& name) const {
    auto result = resolve(name);
    return result.success;
}

std::shared_ptr<CHTLASTNode> NamespaceAccessor::get(const String& name) const {
    auto result = resolve(name);
    return result.success ? result.resolvedNode : nullptr;
}

std::vector<String> NamespaceAccessor::listAvailableItems() const {
    if (currentNamespace_.empty()) {
        // 列出所有命名空间的所有项目
        std::vector<String> allItems;
        auto namespaces = manager_.getAvailableNamespaces();
        
        for (const auto& nsName : namespaces) {
            auto items = manager_.getAvailableItems(nsName);
            for (const auto& item : items) {
                allItems.push_back(nsName + "." + item);
            }
        }
        
        return allItems;
    } else {
        return manager_.getAvailableItems(currentNamespace_);
    }
}

std::vector<String> NamespaceAccessor::listAvailableNamespaces() const {
    return manager_.getAvailableNamespaces();
}

} // namespace chtl