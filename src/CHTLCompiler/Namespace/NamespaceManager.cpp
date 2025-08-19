#include "NamespaceManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace ns {

// === NamespaceNode实现 ===

NamespaceNode::NamespaceNode(const std::string& name, NamespaceNodeType type, NamespaceNode* parent)
    : name_(name), type_(type), parent_(parent) {}

std::string NamespaceNode::getFullQualifiedName() const {
    if (parent_ == nullptr || parent_->getType() == NamespaceNodeType::ROOT) {
        return name_;
    }
    return parent_->getFullQualifiedName() + "." + name_;
}

NamespaceNode* NamespaceNode::addChild(const std::string& childName, NamespaceNodeType childType) {
    // 检查是否已存在同名子命名空间
    auto existing = findChild(childName);
    if (existing != nullptr) {
        return existing; // 返回现有的，将在上层处理合并
    }
    
    auto child = std::make_unique<NamespaceNode>(childName, childType, this);
    NamespaceNode* childPtr = child.get();
    children_.push_back(std::move(child));
    return childPtr;
}

NamespaceNode* NamespaceNode::findChild(const std::string& childName) const {
    for (const auto& child : children_) {
        if (child->getName() == childName) {
            return child.get();
        }
    }
    return nullptr;
}

NamespaceNode* NamespaceNode::findDescendant(const std::string& path) const {
    if (path.empty()) {
        return const_cast<NamespaceNode*>(this);
    }
    
    size_t dotPos = path.find('.');
    if (dotPos == std::string::npos) {
        return findChild(path);
    }
    
    std::string firstPart = path.substr(0, dotPos);
    std::string remainingPath = path.substr(dotPos + 1);
    
    auto child = findChild(firstPart);
    if (child == nullptr) {
        return nullptr;
    }
    
    return child->findDescendant(remainingPath);
}

bool NamespaceNode::addSymbol(const NamespaceSymbol& symbol) {
    // 检查约束违反
    if (isConstraintViolated(symbol.name, symbol.type)) {
        return false;
    }
    
    symbols_[symbol.name].push_back(symbol);
    return true;
}

bool NamespaceNode::hasSymbol(const std::string& symbolName, SymbolType type) const {
    auto it = symbols_.find(symbolName);
    if (it == symbols_.end()) {
        return false;
    }
    
    for (const auto& symbol : it->second) {
        if (symbol.type == type) {
            return true;
        }
    }
    return false;
}

NamespaceSymbol* NamespaceNode::findSymbol(const std::string& symbolName, SymbolType type) {
    auto it = symbols_.find(symbolName);
    if (it == symbols_.end()) {
        return nullptr;
    }
    
    for (auto& symbol : it->second) {
        if (symbol.type == type) {
            return &symbol;
        }
    }
    return nullptr;
}

const NamespaceSymbol* NamespaceNode::findSymbol(const std::string& symbolName, SymbolType type) const {
    auto it = symbols_.find(symbolName);
    if (it == symbols_.end()) {
        return nullptr;
    }
    
    for (const auto& symbol : it->second) {
        if (symbol.type == type) {
            return &symbol;
        }
    }
    return nullptr;
}

std::vector<NamespaceSymbol*> NamespaceNode::findSymbolsByName(const std::string& symbolName) {
    std::vector<NamespaceSymbol*> result;
    auto it = symbols_.find(symbolName);
    if (it != symbols_.end()) {
        for (auto& symbol : it->second) {
            result.push_back(&symbol);
        }
    }
    return result;
}

std::vector<NamespaceSymbol*> NamespaceNode::findSymbolsByType(SymbolType type) {
    std::vector<NamespaceSymbol*> result;
    for (auto& pair : symbols_) {
        for (auto& symbol : pair.second) {
            if (symbol.type == type) {
                result.push_back(&symbol);
            }
        }
    }
    return result;
}

void NamespaceNode::addConstraint(const std::string& constraint) {
    if (isValidConstraint(constraint)) {
        constraints_.push_back(constraint);
    }
}

bool NamespaceNode::isConstraintViolated(const std::string& symbolName, SymbolType type) const {
    for (const auto& constraint : constraints_) {
        if (checkConstraintConflict(symbolName, type, constraint)) {
            return true;
        }
    }
    return false;
}

bool NamespaceNode::canMergeWith(const NamespaceNode& other) const {
    // 检查基本条件
    if (name_ != other.name_ || type_ != other.type_) {
        return false;
    }
    
    // 检查符号冲突
    for (const auto& otherSymbolPair : other.symbols_) {
        const std::string& symbolName = otherSymbolPair.first;
        const auto& otherSymbols = otherSymbolPair.second;
        
        auto it = symbols_.find(symbolName);
        if (it != symbols_.end()) {
            const auto& mySymbols = it->second;
            
            // 检查是否有相同类型的符号冲突
            for (const auto& otherSymbol : otherSymbols) {
                for (const auto& mySymbol : mySymbols) {
                    if (otherSymbol.type == mySymbol.type) {
                        // 同名同类型冲突
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

bool NamespaceNode::mergeWith(const NamespaceNode& other) {
    if (!canMergeWith(other)) {
        return false;
    }
    
    // 合并符号
    for (const auto& otherSymbolPair : other.symbols_) {
        const std::string& symbolName = otherSymbolPair.first;
        const auto& otherSymbols = otherSymbolPair.second;
        
        for (const auto& symbol : otherSymbols) {
            symbols_[symbolName].push_back(symbol);
        }
    }
    
    // 合并约束
    for (const auto& constraint : other.constraints_) {
        if (std::find(constraints_.begin(), constraints_.end(), constraint) == constraints_.end()) {
            constraints_.push_back(constraint);
        }
    }
    
    // 合并子命名空间
    for (const auto& otherChild : other.children_) {
        auto myChild = findChild(otherChild->getName());
        if (myChild != nullptr) {
            // 递归合并子命名空间
            myChild->mergeWith(*otherChild);
        } else {
            // 创建新的子命名空间
            auto newChild = addChild(otherChild->getName(), otherChild->getType());
            newChild->mergeWith(*otherChild);
        }
    }
    
    return true;
}

std::vector<NamespaceConflict> NamespaceNode::detectConflicts() const {
    std::vector<NamespaceConflict> conflicts;
    
    // 检查符号冲突
    for (const auto& symbolPair : symbols_) {
        const std::string& symbolName = symbolPair.first;
        const auto& symbols = symbolPair.second;
        
        if (symbols.size() > 1) {
            // 检查是否有相同类型的符号
            std::unordered_map<SymbolType, std::vector<const NamespaceSymbol*>> typeGroups;
            
            for (const auto& symbol : symbols) {
                typeGroups[symbol.type].push_back(&symbol);
            }
            
            for (const auto& typeGroup : typeGroups) {
                if (typeGroup.second.size() > 1) {
                    // 同名同类型冲突
                    NamespaceConflict conflict;
                    conflict.namespaceName = getFullQualifiedName();
                    conflict.symbolName = symbolName;
                    conflict.conflictType = typeGroup.first;
                    conflict.severity = NamespaceConflict::ConflictSeverity::ERROR;
                    conflict.conflictDescription = "同名同类型符号冲突";
                    
                    for (const auto* symbol : typeGroup.second) {
                        conflict.conflictingSymbols.push_back(*symbol);
                    }
                    
                    conflicts.push_back(conflict);
                }
            }
        }
    }
    
    return conflicts;
}

bool NamespaceNode::isValidConstraint(const std::string& constraint) const {
    // 严格按照CHTL语法文档验证约束
    return !constraint.empty() && 
           (constraint.find("except") != std::string::npos ||
            constraint.find("[Template]") != std::string::npos ||
            constraint.find("[Custom]") != std::string::npos ||
            constraint.find("@Html") != std::string::npos);
}

bool NamespaceNode::checkConstraintConflict(const std::string& symbolName, SymbolType type, 
                                           const std::string& constraint) const {
    // 严格按照CHTL语法文档检查约束冲突
    
    // 类型约束
    if (constraint.find("except [Template]") != std::string::npos) {
        return type == SymbolType::TEMPLATE_ELEMENT || 
               type == SymbolType::TEMPLATE_STYLE || 
               type == SymbolType::TEMPLATE_VAR;
    }
    
    if (constraint.find("except [Custom]") != std::string::npos) {
        return type == SymbolType::CUSTOM_ELEMENT || 
               type == SymbolType::CUSTOM_STYLE || 
               type == SymbolType::CUSTOM_VAR;
    }
    
    if (constraint.find("except @Html") != std::string::npos) {
        return type == SymbolType::ORIGIN_HTML;
    }
    
    // 精确约束：except span, [Custom] @Element Box
    if (constraint.find("except " + symbolName) != std::string::npos) {
        return true;
    }
    
    return false;
}

// === NamespaceManager实现 ===

NamespaceManager::NamespaceManager() {
    rootNamespace_ = std::make_unique<NamespaceNode>("", NamespaceNodeType::ROOT);
}

NamespaceManager::~NamespaceManager() = default;

void NamespaceManager::setErrorHandler(std::shared_ptr<chtl::common::ErrorHandler> errorHandler) {
    errorHandler_ = errorHandler;
}

void NamespaceManager::setMergeStrategy(MergeStrategy strategy) {
    mergeStrategy_ = strategy;
}

void NamespaceManager::setConflictDetectionStrategy(ConflictDetectionStrategy strategy) {
    conflictStrategy_ = strategy;
}

// === 命名空间管理 ===

NamespaceNode* NamespaceManager::createNamespace(const std::string& name, const std::string& sourceFile) {
    return createNamespaceInternal(name, NamespaceNodeType::NAMED, rootNamespace_.get(), sourceFile);
}

NamespaceNode* NamespaceManager::createNestedNamespace(const std::string& parentPath, 
                                                      const std::string& childName, 
                                                      const std::string& sourceFile) {
    auto parent = findNamespace(parentPath);
    if (parent == nullptr) {
        reportError("找不到父命名空间: " + parentPath);
        return nullptr;
    }
    
    return createNamespaceInternal(childName, NamespaceNodeType::NESTED, parent, sourceFile);
}

NamespaceNode* NamespaceManager::createFileDefaultNamespace(const std::string& fileName, 
                                                           const std::string& sourceFile) {
    // 严格按照CHTL语法文档：如果文件没有命名空间，则默认使用文件名称来作为命名空间
    std::string namespaceName = fileName;
    size_t dotPos = namespaceName.find_last_of('.');
    if (dotPos != std::string::npos) {
        namespaceName = namespaceName.substr(0, dotPos);
    }
    
    size_t slashPos = namespaceName.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        namespaceName = namespaceName.substr(slashPos + 1);
    }
    
    return createNamespaceInternal(namespaceName, NamespaceNodeType::FILE_DEFAULT, 
                                  rootNamespace_.get(), sourceFile);
}

NamespaceNode* NamespaceManager::findNamespace(const std::string& path) {
    if (path.empty()) {
        return rootNamespace_.get();
    }
    return rootNamespace_->findDescendant(path);
}

const NamespaceNode* NamespaceManager::findNamespace(const std::string& path) const {
    if (path.empty()) {
        return rootNamespace_.get();
    }
    return rootNamespace_->findDescendant(path);
}

// === 同名命名空间自动合并 ===

bool NamespaceManager::hasSameNameNamespace(const std::string& name) const {
    auto sameNamespaces = const_cast<NamespaceManager*>(this)->getSameNameNamespaces(name);
    return sameNamespaces.size() > 1;
}

std::vector<NamespaceNode*> NamespaceManager::getSameNameNamespaces(const std::string& name) {
    std::vector<NamespaceNode*> result;
    
    // 检查根级别的同名命名空间
    for (const auto& child : rootNamespace_->getChildren()) {
        if (child->getName() == name) {
            result.push_back(child.get());
        }
    }
    
    return result;
}

bool NamespaceManager::autoMergeSameNameNamespaces(const std::string& name) {
    auto sameNamespaces = getSameNameNamespaces(name);
    if (sameNamespaces.size() <= 1) {
        return true; // 没有需要合并的
    }
    
    if (mergeStrategy_ == MergeStrategy::REJECT) {
        reportError("拒绝合并同名命名空间: " + name);
        return false;
    }
    
    if (mergeStrategy_ == MergeStrategy::MANUAL) {
        reportWarning("需要手动确认合并同名命名空间: " + name);
        return false;
    }
    
    // 自动合并或覆盖
    auto target = sameNamespaces[0];
    
    for (size_t i = 1; i < sameNamespaces.size(); ++i) {
        auto source = sameNamespaces[i];
        
        if (mergeStrategy_ == MergeStrategy::OVERWRITE) {
            // 覆盖模式：清空目标，然后合并
            // 注意：这里需要小心处理，避免破坏内部状态
        }
        
        if (!performMerge(target, source)) {
            reportError("无法合并命名空间: " + name);
            return false;
        }
    }
    
    statistics_.mergedNamespaces++;
    return true;
}

bool NamespaceManager::mergeNamespaces(const std::string& targetPath, const std::string& sourcePath) {
    auto target = findNamespace(targetPath);
    auto source = findNamespace(sourcePath);
    
    if (target == nullptr || source == nullptr) {
        reportError("找不到要合并的命名空间");
        return false;
    }
    
    return performMerge(target, source);
}

bool NamespaceManager::canMergeNamespaces(const std::string& targetPath, const std::string& sourcePath) const {
    auto target = findNamespace(targetPath);
    auto source = findNamespace(sourcePath);
    
    if (target == nullptr || source == nullptr) {
        return false;
    }
    
    return target->canMergeWith(*source);
}

// === 冲突检测策略 ===

std::vector<NamespaceConflict> NamespaceManager::detectAllConflicts() {
    return detectConflictsInternal(rootNamespace_.get());
}

std::vector<NamespaceConflict> NamespaceManager::detectNamespaceConflicts(const std::string& namespacePath) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        return {};
    }
    
    return ns->detectConflicts();
}

std::vector<NamespaceConflict> NamespaceManager::detectSymbolConflicts(const std::string& namespacePath, 
                                                                      const std::string& symbolName) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        return {};
    }
    
    std::vector<NamespaceConflict> conflicts;
    auto symbols = ns->findSymbolsByName(symbolName);
    
    if (symbols.size() > 1) {
        // 按类型分组检查冲突
        std::unordered_map<SymbolType, std::vector<NamespaceSymbol*>> typeGroups;
        
        for (auto* symbol : symbols) {
            typeGroups[symbol->type].push_back(symbol);
        }
        
        for (const auto& typeGroup : typeGroups) {
            if (typeGroup.second.size() > 1) {
                NamespaceConflict conflict;
                conflict.namespaceName = namespacePath;
                conflict.symbolName = symbolName;
                conflict.conflictType = typeGroup.first;
                conflict.severity = NamespaceConflict::ConflictSeverity::ERROR;
                conflict.conflictDescription = "同名同类型符号冲突";
                
                for (const auto* symbol : typeGroup.second) {
                    conflict.conflictingSymbols.push_back(*symbol);
                }
                
                conflicts.push_back(conflict);
            }
        }
    }
    
    return conflicts;
}

bool NamespaceManager::resolveConflict(const NamespaceConflict& conflict) {
    // 简化的冲突解决
    if (conflict.severity == NamespaceConflict::ConflictSeverity::WARNING) {
        // 警告级别的冲突可以自动解决
        statistics_.resolvedConflicts++;
        return true;
    }
    
    // 其他冲突需要手动处理
    reportError("无法自动解决冲突: " + conflict.conflictDescription);
    return false;
}

int NamespaceManager::autoResolveConflicts() {
    auto conflicts = detectAllConflicts();
    int resolvedCount = 0;
    
    for (const auto& conflict : conflicts) {
        if (isConflictResolvable(conflict) && resolveConflict(conflict)) {
            resolvedCount++;
        }
    }
    
    return resolvedCount;
}

// === 符号管理 ===

bool NamespaceManager::addSymbolToNamespace(const std::string& namespacePath, const NamespaceSymbol& symbol) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        reportError("找不到命名空间: " + namespacePath);
        return false;
    }
    
    return ns->addSymbol(symbol);
}

NamespaceSymbol* NamespaceManager::findSymbol(const std::string& namespacePath, 
                                             const std::string& symbolName, SymbolType type) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        return nullptr;
    }
    
    return ns->findSymbol(symbolName, type);
}

NamespaceSymbol* NamespaceManager::findSymbolRecursive(const std::string& startNamespace, 
                                                      const std::string& symbolName, SymbolType type) {
    auto ns = findNamespace(startNamespace);
    if (ns == nullptr) {
        return nullptr;
    }
    
    // 在当前命名空间中查找
    auto symbol = ns->findSymbol(symbolName, type);
    if (symbol != nullptr) {
        return symbol;
    }
    
    // 在父命名空间中查找
    auto parent = ns->getParent();
    if (parent != nullptr && parent->getType() != NamespaceNodeType::ROOT) {
        return findSymbolRecursive(parent->getFullQualifiedName(), symbolName, type);
    }
    
    return nullptr;
}

NamespaceSymbol* NamespaceManager::resolveSymbolReference(const std::string& symbolName, SymbolType type, 
                                                         const std::string& fromNamespace) {
    // 严格按照CHTL语法文档解析：@Element Box from space.room2
    
    auto symbol = findSymbol(fromNamespace, symbolName, type);
    if (symbol != nullptr) {
        return symbol;
    }
    
    // 如果没找到，尝试递归查找
    return findSymbolRecursive(fromNamespace, symbolName, type);
}

// === 约束管理 ===

bool NamespaceManager::addConstraintToNamespace(const std::string& namespacePath, 
                                               const std::string& constraint) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        reportError("找不到命名空间: " + namespacePath);
        return false;
    }
    
    ns->addConstraint(constraint);
    return true;
}

bool NamespaceManager::checkConstraintViolation(const std::string& namespacePath, 
                                               const std::string& symbolName, SymbolType type) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        return false;
    }
    
    return ns->isConstraintViolated(symbolName, type);
}

bool NamespaceManager::validateGlobalConstraints(const std::string& namespacePath) {
    auto ns = findNamespace(namespacePath);
    if (ns == nullptr) {
        return false;
    }
    
    // 严格按照CHTL语法文档：在命名空间内使用全局约束，全局约束只支持前面列出来的类型
    for (const auto& constraint : ns->getConstraints()) {
        if (constraint.find("except [Template]") == std::string::npos &&
            constraint.find("except [Custom]") == std::string::npos &&
            constraint.find("except @Html") == std::string::npos) {
            reportError("无效的全局约束: " + constraint);
            return false;
        }
    }
    
    return true;
}

// === 实用工具 ===

std::string NamespaceManager::getNamespaceFullPath(const NamespaceNode* node) const {
    if (node == nullptr) {
        return "";
    }
    return node->getFullQualifiedName();
}

std::vector<std::string> NamespaceManager::parseNamespacePath(const std::string& path) const {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

std::string NamespaceManager::formatNamespaceTree() const {
    std::stringstream ss;
    
    std::function<void(const NamespaceNode*, int)> formatNode = 
        [&](const NamespaceNode* node, int depth) {
            std::string indent(depth * 2, ' ');
            
            if (node->getType() != NamespaceNodeType::ROOT) {
                ss << indent << node->getName() << " (" 
                   << node->getAllSymbols().size() << " symbols)" << std::endl;
            }
            
            for (const auto& child : node->getChildren()) {
                formatNode(child.get(), depth + 1);
            }
        };
    
    formatNode(rootNamespace_.get(), 0);
    return ss.str();
}

NamespaceManager::Statistics NamespaceManager::getStatistics() const {
    updateStatistics();
    return statistics_;
}

void NamespaceManager::clear() {
    rootNamespace_ = std::make_unique<NamespaceNode>("", NamespaceNodeType::ROOT);
    statistics_ = Statistics{};
}

// === 私有辅助方法 ===

NamespaceNode* NamespaceManager::createNamespaceInternal(const std::string& name, 
                                                        NamespaceNodeType type, 
                                                        NamespaceNode* parent, 
                                                        const std::string& sourceFile) {
    // 检查是否已存在同名命名空间
    auto existing = parent->findChild(name);
    if (existing != nullptr) {
        // 根据合并策略处理
        if (mergeStrategy_ == MergeStrategy::AUTOMATIC) {
            return existing; // 返回现有的，后续会自动合并
        } else if (mergeStrategy_ == MergeStrategy::REJECT) {
            reportError("命名空间已存在: " + name);
            return nullptr;
        }
    }
    
    return parent->addChild(name, type);
}

bool NamespaceManager::performMerge(NamespaceNode* target, NamespaceNode* source) {
    if (!target->canMergeWith(*source)) {
        return false;
    }
    
    return target->mergeWith(*source);
}

std::vector<NamespaceConflict> NamespaceManager::detectConflictsInternal(NamespaceNode* node) const {
    std::vector<NamespaceConflict> conflicts;
    
    // 检查当前节点的冲突
    auto nodeConflicts = node->detectConflicts();
    conflicts.insert(conflicts.end(), nodeConflicts.begin(), nodeConflicts.end());
    
    // 递归检查子节点
    for (const auto& child : node->getChildren()) {
        auto childConflicts = detectConflictsInternal(child.get());
        conflicts.insert(conflicts.end(), childConflicts.begin(), childConflicts.end());
    }
    
    return conflicts;
}

bool NamespaceManager::isConflictResolvable(const NamespaceConflict& conflict) const {
    return conflict.severity == NamespaceConflict::ConflictSeverity::WARNING;
}

void NamespaceManager::reportError(const std::string& message, const std::string& context) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::ERROR, chtl::common::ErrorType::NAMESPACE_ERROR, 
                                 message, context);
    }
}

void NamespaceManager::reportWarning(const std::string& message, const std::string& context) {
    if (errorHandler_) {
        errorHandler_->reportError(chtl::common::ErrorLevel::WARNING, chtl::common::ErrorType::NAMESPACE_ERROR, 
                                 message, context);
    }
}

void NamespaceManager::updateStatistics() const {
    statistics_.totalNamespaces = 0;
    statistics_.totalSymbols = 0;
    
    std::function<void(const NamespaceNode*)> countNode = [&](const NamespaceNode* node) {
        if (node->getType() != NamespaceNodeType::ROOT) {
            statistics_.totalNamespaces++;
        }
        
        for (const auto& symbolPair : node->getAllSymbols()) {
            statistics_.totalSymbols += symbolPair.second.size();
        }
        
        for (const auto& child : node->getChildren()) {
            countNode(child.get());
        }
    };
    
    countNode(rootNamespace_.get());
}

std::string NamespaceManager::symbolTypeToString(SymbolType type) const {
    switch (type) {
        case SymbolType::CUSTOM_ELEMENT: return "[Custom] @Element";
        case SymbolType::CUSTOM_STYLE: return "[Custom] @Style";
        case SymbolType::CUSTOM_VAR: return "[Custom] @Var";
        case SymbolType::TEMPLATE_ELEMENT: return "[Template] @Element";
        case SymbolType::TEMPLATE_STYLE: return "[Template] @Style";
        case SymbolType::TEMPLATE_VAR: return "[Template] @Var";
        case SymbolType::ORIGIN_HTML: return "[Origin] @Html";
        case SymbolType::ORIGIN_STYLE: return "[Origin] @Style";
        case SymbolType::ORIGIN_JAVASCRIPT: return "[Origin] @Javascript";
        case SymbolType::CONFIGURATION: return "[Configuration] @Config";
        default: return "Unknown";
    }
}

SymbolType NamespaceManager::stringToSymbolType(const std::string& typeStr) const {
    if (typeStr == "[Custom] @Element") return SymbolType::CUSTOM_ELEMENT;
    if (typeStr == "[Custom] @Style") return SymbolType::CUSTOM_STYLE;
    if (typeStr == "[Custom] @Var") return SymbolType::CUSTOM_VAR;
    if (typeStr == "[Template] @Element") return SymbolType::TEMPLATE_ELEMENT;
    if (typeStr == "[Template] @Style") return SymbolType::TEMPLATE_STYLE;
    if (typeStr == "[Template] @Var") return SymbolType::TEMPLATE_VAR;
    if (typeStr == "[Origin] @Html") return SymbolType::ORIGIN_HTML;
    if (typeStr == "[Origin] @Style") return SymbolType::ORIGIN_STYLE;
    if (typeStr == "[Origin] @Javascript") return SymbolType::ORIGIN_JAVASCRIPT;
    if (typeStr == "[Configuration] @Config") return SymbolType::CONFIGURATION;
    
    return SymbolType::CUSTOM_ELEMENT; // 默认值
}

} // namespace ns
} // namespace chtl