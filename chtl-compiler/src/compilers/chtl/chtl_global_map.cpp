#include "compilers/chtl/chtl_global_map.h"
#include <sstream>
#include <algorithm>

namespace chtl {

CHTLGlobalMap::CHTLGlobalMap() {
    // 创建默认命名空间
    createNamespace("", "");  // 全局命名空间
}

CHTLGlobalMap::~CHTLGlobalMap() = default;

bool CHTLGlobalMap::registerSymbol(const std::string& name, SymbolType type,
                                  std::shared_ptr<CHTLASTNode> node,
                                  const std::string& namespace_) {
    std::string ns = namespace_.empty() ? currentNamespace_ : namespace_;
    std::string qualifiedName = makeQualifiedName(name, ns);
    
    // 检查是否已存在
    if (globalSymbols_.find(qualifiedName) != globalSymbols_.end()) {
        return false;  // 符号已存在
    }
    
    auto symbol = std::make_shared<Symbol>(name, type, node);
    symbol->namespace_ = ns;
    
    globalSymbols_[qualifiedName] = symbol;
    
    // 添加到命名空间
    if (auto nsInfo = getNamespace(ns)) {
        nsInfo->symbols[name] = symbol;
    }
    
    return true;
}

std::shared_ptr<Symbol> CHTLGlobalMap::lookupSymbol(const std::string& name,
                                                   const std::string& namespace_,
                                                   bool searchParent) const {
    std::string ns = namespace_.empty() ? currentNamespace_ : namespace_;
    
    // 首先在指定命名空间查找
    std::string qualifiedName = makeQualifiedName(name, ns);
    auto it = globalSymbols_.find(qualifiedName);
    if (it != globalSymbols_.end()) {
        return it->second;
    }
    
    // 如果需要搜索父命名空间
    if (searchParent && !ns.empty()) {
        auto nsPath = parseNamespacePath(ns);
        while (!nsPath.empty()) {
            nsPath.pop_back();
            std::string parentNs;
            for (const auto& part : nsPath) {
                if (!parentNs.empty()) parentNs += ".";
                parentNs += part;
            }
            
            qualifiedName = makeQualifiedName(name, parentNs);
            it = globalSymbols_.find(qualifiedName);
            if (it != globalSymbols_.end()) {
                return it->second;
            }
        }
        
        // 最后在全局命名空间查找
        it = globalSymbols_.find(name);
        if (it != globalSymbols_.end()) {
            return it->second;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Symbol>> CHTLGlobalMap::lookupSymbolsByType(
    SymbolType type, const std::string& namespace_) const {
    
    std::vector<std::shared_ptr<Symbol>> result;
    
    for (const auto& [name, symbol] : globalSymbols_) {
        if (symbol->type == type) {
            if (namespace_.empty() || symbol->namespace_ == namespace_) {
                result.push_back(symbol);
            }
        }
    }
    
    return result;
}

bool CHTLGlobalMap::createNamespace(const std::string& name, const std::string& parent) {
    if (namespaces_.find(name) != namespaces_.end()) {
        return false;  // 命名空间已存在
    }
    
    auto nsInfo = std::make_shared<NamespaceInfo>(name, parent);
    namespaces_[name] = nsInfo;
    
    // 更新父命名空间的子命名空间列表
    if (!parent.empty()) {
        if (auto parentNs = getNamespace(parent)) {
            parentNs->childNamespaces.push_back(name);
        }
    }
    
    return true;
}

std::shared_ptr<NamespaceInfo> CHTLGlobalMap::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGlobalMap::enterNamespace(const std::string& name) {
    namespaceStack_.push_back(currentNamespace_);
    currentNamespace_ = name;
}

void CHTLGlobalMap::exitNamespace() {
    if (!namespaceStack_.empty()) {
        currentNamespace_ = namespaceStack_.back();
        namespaceStack_.pop_back();
    }
}

bool CHTLGlobalMap::symbolExists(const std::string& name, const std::string& namespace_) const {
    return lookupSymbol(name, namespace_, false) != nullptr;
}

std::string CHTLGlobalMap::getFullyQualifiedName(const std::string& name, 
                                                const std::string& namespace_) const {
    return makeQualifiedName(name, namespace_);
}

std::shared_ptr<Symbol> CHTLGlobalMap::resolveSymbolReference(
    const std::string& name, const std::string& fromNamespace) const {
    
    // 解析from语法，如：space.room2
    return lookupSymbol(name, fromNamespace, true);
}

bool CHTLGlobalMap::importSymbol(const std::string& name, const std::string& fromNamespace,
                                const std::string& asName, const std::string& toNamespace) {
    // 查找源符号
    auto symbol = resolveSymbolReference(name, fromNamespace);
    if (!symbol) {
        return false;
    }
    
    // 创建导入的符号
    std::string importName = asName.empty() ? name : asName;
    std::string targetNs = toNamespace.empty() ? currentNamespace_ : toNamespace;
    
    // 创建新符号（引用原符号）
    auto importedSymbol = std::make_shared<Symbol>(*symbol);
    importedSymbol->name = importName;
    importedSymbol->namespace_ = targetNs;
    importedSymbol->metadata["imported_from"] = fromNamespace;
    importedSymbol->metadata["original_name"] = name;
    
    std::string qualifiedName = makeQualifiedName(importName, targetNs);
    globalSymbols_[qualifiedName] = importedSymbol;
    
    return true;
}

void CHTLGlobalMap::clear() {
    globalSymbols_.clear();
    namespaces_.clear();
    namespaceStack_.clear();
    currentNamespace_.clear();
    configuration_.clear();
    
    // 重新创建默认命名空间
    createNamespace("", "");
}

std::vector<std::shared_ptr<Symbol>> CHTLGlobalMap::getAllSymbols() const {
    std::vector<std::shared_ptr<Symbol>> result;
    for (const auto& [name, symbol] : globalSymbols_) {
        result.push_back(symbol);
    }
    return result;
}

std::unordered_map<std::string, std::string> CHTLGlobalMap::getConfiguration() const {
    return configuration_;
}

void CHTLGlobalMap::setConfiguration(const std::string& key, const std::string& value) {
    configuration_[key] = value;
}

std::string CHTLGlobalMap::makeQualifiedName(const std::string& name, 
                                            const std::string& namespace_) const {
    if (namespace_.empty()) {
        return name;
    }
    return namespace_ + "." + name;
}

std::vector<std::string> CHTLGlobalMap::parseNamespacePath(const std::string& path) const {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            result.push_back(part);
        }
    }
    
    return result;
}

} // namespace chtl