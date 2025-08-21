#include "CHTL/GlobalMap.h"
#include "Utils/Logger.h"
#include <stack>
#include <algorithm>
#include <iostream>

namespace CHTL {

// GlobalMap内部实现类
class GlobalMap::Impl {
public:
    // 符号表
    std::unordered_map<std::string, Symbol> symbols_;
    
    // 命名空间表
    std::unordered_map<std::string, NamespaceInfo> namespaces_;
    
    // 当前命名空间栈
    std::stack<std::string> namespaceStack_;
    
    // 作用域栈（每个作用域保存该作用域内定义的符号）
    std::stack<std::vector<std::string>> scopeStack_;
    
    Impl() {
        // 创建全局命名空间
        NamespaceInfo global;
        global.name = "";
        namespaces_[""] = global;
        namespaceStack_.push("");
    }
};

// GlobalMap实现
GlobalMap::GlobalMap() : impl_(std::make_unique<Impl>()) {}
GlobalMap::~GlobalMap() = default;

bool GlobalMap::AddSymbol(const Symbol& symbol) {
    std::string fullName = BuildFullName(symbol.name);
    
    // 检查名称冲突
    if (impl_->symbols_.find(fullName) != impl_->symbols_.end()) {
        LOG_WARNING("符号已存在: " + fullName);
        return false;
    }
    
    // 添加符号
    Symbol newSymbol = symbol;
    newSymbol.fullName = fullName;
    newSymbol.parentNamespace = GetCurrentNamespace();
    impl_->symbols_[fullName] = newSymbol;
    
    // 添加到当前命名空间
    auto& currentNs = impl_->namespaces_[GetCurrentNamespace()];
    currentNs.symbols.push_back(fullName);
    
    // 添加到当前作用域（如果有）
    if (!impl_->scopeStack_.empty()) {
        impl_->scopeStack_.top().push_back(fullName);
    }
    
    LOG_DEBUG("添加符号: " + fullName);
    return true;
}

bool GlobalMap::RemoveSymbol(const std::string& fullName) {
    auto it = impl_->symbols_.find(fullName);
    if (it == impl_->symbols_.end()) {
        return false;
    }
    
    // 从命名空间中移除
    auto& ns = impl_->namespaces_[it->second.parentNamespace];
    ns.symbols.erase(
        std::remove(ns.symbols.begin(), ns.symbols.end(), fullName),
        ns.symbols.end()
    );
    
    // 从符号表中移除
    impl_->symbols_.erase(it);
    return true;
}

std::optional<Symbol> GlobalMap::FindSymbol(const std::string& name) const {
    // 首先尝试在当前命名空间查找
    std::string fullName = BuildFullName(name);
    auto it = impl_->symbols_.find(fullName);
    if (it != impl_->symbols_.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    std::vector<std::string> hierarchy = GetNamespaceHierarchy();
    for (auto nsIt = hierarchy.rbegin(); nsIt != hierarchy.rend(); ++nsIt) {
        std::string searchName = *nsIt + (nsIt->empty() ? "" : "::") + name;
        auto symIt = impl_->symbols_.find(searchName);
        if (symIt != impl_->symbols_.end()) {
            return symIt->second;
        }
    }
    
    // 检查导入的符号
    const auto& currentNs = impl_->namespaces_.at(GetCurrentNamespace());
    auto aliasIt = currentNs.aliases.find(name);
    if (aliasIt != currentNs.aliases.end()) {
        auto symIt = impl_->symbols_.find(aliasIt->second);
        if (symIt != impl_->symbols_.end()) {
            return symIt->second;
        }
    }
    
    return std::nullopt;
}

std::optional<Symbol> GlobalMap::FindSymbolInNamespace(const std::string& name, 
                                                       const std::string& namespaceName) const {
    std::string fullName = namespaceName + (namespaceName.empty() ? "" : "::") + name;
    auto it = impl_->symbols_.find(fullName);
    if (it != impl_->symbols_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool GlobalMap::CreateNamespace(const std::string& name, const std::string& parent) {
    std::string fullName = parent + (parent.empty() ? "" : "::") + name;
    
    if (impl_->namespaces_.find(fullName) != impl_->namespaces_.end()) {
        LOG_WARNING("命名空间已存在: " + fullName);
        return false;
    }
    
    NamespaceInfo ns;
    ns.name = fullName;
    ns.parentNamespace = parent;
    impl_->namespaces_[fullName] = ns;
    
    // 添加到父命名空间的子命名空间列表
    if (impl_->namespaces_.find(parent) != impl_->namespaces_.end()) {
        impl_->namespaces_[parent].childNamespaces.push_back(fullName);
    }
    
    LOG_DEBUG("创建命名空间: " + fullName);
    return true;
}

bool GlobalMap::EnterNamespace(const std::string& name) {
    std::string fullName = BuildFullName(name);
    
    if (impl_->namespaces_.find(fullName) == impl_->namespaces_.end()) {
        LOG_ERROR("命名空间不存在: " + fullName);
        return false;
    }
    
    impl_->namespaceStack_.push(fullName);
    LOG_DEBUG("进入命名空间: " + fullName);
    return true;
}

bool GlobalMap::ExitNamespace() {
    if (impl_->namespaceStack_.size() <= 1) {
        LOG_WARNING("无法退出全局命名空间");
        return false;
    }
    
    std::string exitingNs = impl_->namespaceStack_.top();
    impl_->namespaceStack_.pop();
    LOG_DEBUG("退出命名空间: " + exitingNs);
    return true;
}

std::string GlobalMap::GetCurrentNamespace() const {
    return impl_->namespaceStack_.top();
}

std::vector<std::string> GlobalMap::GetNamespaceHierarchy() const {
    std::vector<std::string> hierarchy;
    std::stack<std::string> temp = impl_->namespaceStack_;
    
    while (!temp.empty()) {
        hierarchy.push_back(temp.top());
        temp.pop();
    }
    
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

bool GlobalMap::ImportSymbol(const std::string& symbolName, 
                            const std::string& fromNamespace,
                            const std::string& asName) {
    std::string fullSymbolName = fromNamespace + (fromNamespace.empty() ? "" : "::") + symbolName;
    
    // 检查符号是否存在
    if (impl_->symbols_.find(fullSymbolName) == impl_->symbols_.end()) {
        LOG_ERROR("导入的符号不存在: " + fullSymbolName);
        return false;
    }
    
    // 添加别名
    std::string alias = asName.empty() ? symbolName : asName;
    auto& currentNs = impl_->namespaces_[GetCurrentNamespace()];
    currentNs.aliases[alias] = fullSymbolName;
    
    LOG_DEBUG("导入符号: " + fullSymbolName + " as " + alias);
    return true;
}

bool GlobalMap::ImportAll(const std::string& fromNamespace) {
    auto nsIt = impl_->namespaces_.find(fromNamespace);
    if (nsIt == impl_->namespaces_.end()) {
        LOG_ERROR("命名空间不存在: " + fromNamespace);
        return false;
    }
    
    // 导入所有符号
    for (const auto& symbolName : nsIt->second.symbols) {
        auto symIt = impl_->symbols_.find(symbolName);
        if (symIt != impl_->symbols_.end()) {
            std::string shortName = symIt->second.name;
            auto& currentNs = impl_->namespaces_[GetCurrentNamespace()];
            currentNs.aliases[shortName] = symbolName;
        }
    }
    
    LOG_DEBUG("导入所有符号从: " + fromNamespace);
    return true;
}

std::vector<Symbol> GlobalMap::GetSymbolsByType(SymbolType type) const {
    std::vector<Symbol> result;
    for (const auto& [name, symbol] : impl_->symbols_) {
        if (symbol.type == type) {
            result.push_back(symbol);
        }
    }
    return result;
}

std::vector<Symbol> GlobalMap::GetSymbolsInNamespace(const std::string& namespaceName) const {
    std::vector<Symbol> result;
    auto nsIt = impl_->namespaces_.find(namespaceName);
    if (nsIt != impl_->namespaces_.end()) {
        for (const auto& symbolName : nsIt->second.symbols) {
            auto symIt = impl_->symbols_.find(symbolName);
            if (symIt != impl_->symbols_.end()) {
                result.push_back(symIt->second);
            }
        }
    }
    return result;
}

std::vector<std::string> GlobalMap::GetAllNamespaces() const {
    std::vector<std::string> result;
    for (const auto& [name, ns] : impl_->namespaces_) {
        result.push_back(name);
    }
    return result;
}

bool GlobalMap::HasSymbol(const std::string& name) const {
    return FindSymbol(name).has_value();
}

bool GlobalMap::HasNamespace(const std::string& name) const {
    std::string fullName = BuildFullName(name);
    return impl_->namespaces_.find(fullName) != impl_->namespaces_.end();
}

bool GlobalMap::CheckNameConflict(const std::string& name) const {
    return HasSymbol(name);
}

std::vector<std::string> GlobalMap::GetConflictingSymbols(const std::string& name) const {
    std::vector<std::string> conflicts;
    
    // 检查所有可能的全限定名
    for (const auto& [fullName, symbol] : impl_->symbols_) {
        if (symbol.name == name) {
            conflicts.push_back(fullName);
        }
    }
    
    return conflicts;
}

void GlobalMap::PushScope() {
    impl_->scopeStack_.push(std::vector<std::string>());
    LOG_DEBUG("进入新作用域");
}

void GlobalMap::PopScope() {
    if (impl_->scopeStack_.empty()) {
        LOG_WARNING("作用域栈为空");
        return;
    }
    
    // 移除当前作用域的所有符号
    auto& currentScope = impl_->scopeStack_.top();
    for (const auto& symbolName : currentScope) {
        RemoveSymbol(symbolName);
    }
    
    impl_->scopeStack_.pop();
    LOG_DEBUG("退出作用域");
}

void GlobalMap::Clear() {
    impl_->symbols_.clear();
    impl_->namespaces_.clear();
    
    // 重新创建全局命名空间
    NamespaceInfo global;
    global.name = "";
    impl_->namespaces_[""] = global;
    
    // 清空栈
    while (!impl_->namespaceStack_.empty()) {
        impl_->namespaceStack_.pop();
    }
    impl_->namespaceStack_.push("");
    
    while (!impl_->scopeStack_.empty()) {
        impl_->scopeStack_.pop();
    }
}

void GlobalMap::ClearNamespace(const std::string& namespaceName) {
    auto nsIt = impl_->namespaces_.find(namespaceName);
    if (nsIt == impl_->namespaces_.end()) {
        return;
    }
    
    // 移除命名空间中的所有符号
    std::vector<std::string> symbolsToRemove = nsIt->second.symbols;
    for (const auto& symbolName : symbolsToRemove) {
        RemoveSymbol(symbolName);
    }
    
    // 递归清理子命名空间
    for (const auto& childNs : nsIt->second.childNamespaces) {
        ClearNamespace(childNs);
    }
    
    // 从父命名空间中移除
    if (!nsIt->second.parentNamespace.empty()) {
        auto& parentNs = impl_->namespaces_[nsIt->second.parentNamespace];
        parentNs.childNamespaces.erase(
            std::remove(parentNs.childNamespaces.begin(), 
                       parentNs.childNamespaces.end(), namespaceName),
            parentNs.childNamespaces.end()
        );
    }
    
    // 移除命名空间
    impl_->namespaces_.erase(nsIt);
}

void GlobalMap::DumpSymbols() const {
    std::cout << "=== 符号表 ===" << std::endl;
    for (const auto& [name, symbol] : impl_->symbols_) {
        std::cout << "  " << name << " (" 
                  << SymbolTableUtils::GetSymbolTypeName(symbol.type) 
                  << ") at " << symbol.sourceFile << ":" 
                  << symbol.line << ":" << symbol.column << std::endl;
    }
}

void GlobalMap::DumpNamespaces() const {
    std::cout << "=== 命名空间 ===" << std::endl;
    for (const auto& [name, ns] : impl_->namespaces_) {
        std::cout << "  " << (name.empty() ? "<global>" : name) 
                  << " (符号数: " << ns.symbols.size() 
                  << ", 子命名空间数: " << ns.childNamespaces.size() << ")" 
                  << std::endl;
    }
}

std::string GlobalMap::BuildFullName(const std::string& name) const {
    std::string currentNs = GetCurrentNamespace();
    if (currentNs.empty()) {
        return name;
    }
    return currentNs + "::" + name;
}

std::string GlobalMap::ResolveSymbolName(const std::string& name) const {
    // 如果已经是全限定名，直接返回
    if (name.find("::") != std::string::npos) {
        return name;
    }
    return BuildFullName(name);
}

// SymbolTableUtils实现
std::string SymbolTableUtils::GetSymbolTypeName(SymbolType type) {
    switch (type) {
        case SymbolType::Element: return "Element";
        case SymbolType::TemplateStyle: return "TemplateStyle";
        case SymbolType::TemplateElement: return "TemplateElement";
        case SymbolType::TemplateVar: return "TemplateVar";
        case SymbolType::CustomStyle: return "CustomStyle";
        case SymbolType::CustomElement: return "CustomElement";
        case SymbolType::CustomVar: return "CustomVar";
        case SymbolType::OriginBlock: return "OriginBlock";
        case SymbolType::Namespace: return "Namespace";
        case SymbolType::Configuration: return "Configuration";
        case SymbolType::ImportedSymbol: return "ImportedSymbol";
        case SymbolType::LocalVariable: return "LocalVariable";
        case SymbolType::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

bool SymbolTableUtils::IsTemplateType(SymbolType type) {
    return type == SymbolType::TemplateStyle ||
           type == SymbolType::TemplateElement ||
           type == SymbolType::TemplateVar;
}

bool SymbolTableUtils::IsCustomType(SymbolType type) {
    return type == SymbolType::CustomStyle ||
           type == SymbolType::CustomElement ||
           type == SymbolType::CustomVar;
}

bool SymbolTableUtils::CanBeImported(SymbolType type) {
    return IsTemplateType(type) || IsCustomType(type) || 
           type == SymbolType::OriginBlock ||
           type == SymbolType::Configuration;
}

bool SymbolTableUtils::CanBeExported(SymbolType type) {
    return IsTemplateType(type) || IsCustomType(type) || 
           type == SymbolType::OriginBlock;
}

} // namespace CHTL