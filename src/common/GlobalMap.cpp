#include "GlobalMap.h"
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace chtl {

// Scope类实现
bool Scope::addSymbol(std::shared_ptr<Symbol> symbol) {
    if (!symbol) return false;
    
    // 检查符号是否已存在
    auto it = symbols.find(symbol->name);
    if (it != symbols.end()) {
        return false; // 符号已存在
    }
    
    symbols[symbol->name] = symbol;
    return true;
}

std::shared_ptr<Symbol> Scope::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return it != symbols.end() ? it->second : nullptr;
}

std::shared_ptr<Symbol> Scope::findSymbolRecursive(const std::string& name) {
    // 先在当前作用域查找
    auto symbol = findSymbol(name);
    if (symbol) return symbol;
    
    // 递归在父作用域查找
    if (parent) {
        return parent->findSymbolRecursive(name);
    }
    
    return nullptr;
}

Scope* Scope::createChildScope(const std::string& scope_name) {
    auto child = std::make_unique<Scope>(scope_name, this);
    Scope* child_ptr = child.get();
    children.push_back(std::move(child));
    return child_ptr;
}

Scope* Scope::findChildScope(const std::string& scope_name) {
    for (const auto& child : children) {
        if (child->name == scope_name) {
            return child.get();
        }
    }
    return nullptr;
}

Scope* Scope::findOrCreateChildScope(const std::string& scope_name) {
    // 首先尝试查找已存在的子作用域
    Scope* existing_scope = findChildScope(scope_name);
    if (existing_scope) {
        return existing_scope; // 返回已存在的作用域，实现合并
    }
    
    // 如果不存在，则创建新的子作用域
    return createChildScope(scope_name);
}

std::string Scope::getFullPath() const {
    if (!parent) return name;
    
    std::string parent_path = parent->getFullPath();
    if (parent_path.empty()) return name;
    return parent_path + "::" + name;
}

// GlobalMap类实现
GlobalMap::GlobalMap() {
    root_scope_ = std::make_unique<Scope>("global");
    current_scope_ = root_scope_.get();
}

GlobalMap::~GlobalMap() = default;

void GlobalMap::enterScope(const std::string& scope_name) {
    current_scope_ = current_scope_->findOrCreateChildScope(scope_name);
}

void GlobalMap::exitScope() {
    if (current_scope_->parent) {
        current_scope_ = current_scope_->parent;
    }
}

Scope* GlobalMap::getCurrentScope() {
    return current_scope_;
}

const Scope* GlobalMap::getCurrentScope() const {
    return current_scope_;
}

Scope* GlobalMap::getRootScope() {
    return root_scope_.get();
}

const Scope* GlobalMap::getRootScope() const {
    return root_scope_.get();
}

bool GlobalMap::addSymbol(const std::string& name, SymbolType type, 
                         const std::string& value, size_t line, size_t column) {
    std::string current_namespace = getCurrentNamespace();
    auto symbol = std::make_shared<Symbol>(name, type, value, current_namespace, line, column);
    
    // 检测冲突
    auto conflicts = detectConflicts(name, type, value);
    if (!conflicts.empty()) {
        // 如果是命名空间类型且存在同名命名空间，允许合并
        if (type == SymbolType::NAMESPACE) {
            auto existing = current_scope_->findSymbol(name);
            if (existing && existing->type == SymbolType::NAMESPACE) {
                // 命名空间合并，不添加重复符号，返回成功
                return true;
            }
        }
        
        // 对于其他类型的冲突，可以选择警告但仍然添加，或者拒绝添加
        // 这里选择警告但允许添加（可根据需要调整策略）
        std::cout << "警告: 检测到符号冲突 - " << conflicts[0].conflict_type << ": " << conflicts[0].suggestion << std::endl;
    }
    
    return current_scope_->addSymbol(symbol);
}

std::shared_ptr<Symbol> GlobalMap::findSymbol(const std::string& name) {
    return current_scope_->findSymbolRecursive(name);
}

std::shared_ptr<Symbol> GlobalMap::findSymbolInNamespace(const std::string& namespace_path, 
                                                        const std::string& name) {
    // 简化实现：在根作用域查找带命名空间前缀的符号
    std::string full_name = namespace_path.empty() ? name : namespace_path + "::" + name;
    return root_scope_->findSymbolRecursive(full_name);
}

void GlobalMap::enterNamespace(const std::string& namespace_name) {
    namespace_stack_.push(namespace_name);
    enterScope(namespace_name);
}

bool GlobalMap::mergeNamespace(const std::string& namespace_name) {
    // 检查当前作用域是否已有同名子命名空间
    Scope* existing_namespace = current_scope_->findChildScope(namespace_name);
    if (existing_namespace) {
        // 命名空间已存在，进入该命名空间进行合并
        namespace_stack_.push(namespace_name);
        current_scope_ = existing_namespace;
        return true; // 表示合并了现有命名空间
    } else {
        // 命名空间不存在，创建新的
        enterNamespace(namespace_name);
        return false; // 表示创建了新的命名空间
    }
}

std::vector<std::string> GlobalMap::getNamespaceHierarchy() const {
    std::vector<std::string> hierarchy;
    std::stack<std::string> temp_stack = namespace_stack_;
    
    while (!temp_stack.empty()) {
        hierarchy.insert(hierarchy.begin(), temp_stack.top());
        temp_stack.pop();
    }
    
    return hierarchy;
}

void GlobalMap::exitNamespace() {
    if (!namespace_stack_.empty()) {
        namespace_stack_.pop();
        exitScope();
    }
}

std::string GlobalMap::getCurrentNamespace() const {
    if (namespace_stack_.empty()) return "";
    
    std::ostringstream oss;
    std::stack<std::string> temp_stack = namespace_stack_;
    std::vector<std::string> namespaces;
    
    while (!temp_stack.empty()) {
        namespaces.insert(namespaces.begin(), temp_stack.top());
        temp_stack.pop();
    }
    
    for (size_t i = 0; i < namespaces.size(); ++i) {
        if (i > 0) oss << "::";
        oss << namespaces[i];
    }
    
    return oss.str();
}

bool GlobalMap::isNamespaceExists(const std::string& namespace_path) {
    // 简化实现：检查是否存在该命名空间的作用域
    return findSymbolInNamespace(namespace_path, "") != nullptr;
}

void GlobalMap::addImport(const ImportInfo& import_info) {
    imports_.push_back(import_info);
    
    // 添加依赖关系
    std::string current_path = getCurrentNamespace();
    if (current_path.empty()) current_path = "global";
    addDependency(current_path, import_info.source_path);
}

std::vector<GlobalMap::ImportInfo> GlobalMap::getImports() const {
    return imports_;
}

void GlobalMap::registerTemplate(const std::string& name, SymbolType type, 
                                const std::string& content) {
    addSymbol(name, type, content);
}

void GlobalMap::registerCustom(const std::string& name, SymbolType type, 
                              const std::string& content) {
    addSymbol(name, type, content);
}

bool GlobalMap::hasConflict(const std::string& name, SymbolType type) {
    auto existing = findSymbol(name);
    return existing != nullptr && existing->type != type;
}

std::vector<std::shared_ptr<Symbol>> GlobalMap::getConflicts(const std::string& name) {
    std::vector<std::shared_ptr<Symbol>> conflicts;
    
    // 递归查找所有作用域中的同名符号
    std::function<void(Scope*)> findConflictsInScope = [&](Scope* scope) {
        auto it = scope->symbols.find(name);
        if (it != scope->symbols.end()) {
            conflicts.push_back(it->second);
        }
        
        // 递归查找子作用域
        for (const auto& child : scope->children) {
            findConflictsInScope(child.get());
        }
    };
    
    findConflictsInScope(root_scope_.get());
    return conflicts;
}

std::vector<GlobalMap::ConflictInfo> GlobalMap::detectConflicts(const std::string& name, 
                                                               SymbolType type, 
                                                               const std::string& value) {
    std::vector<ConflictInfo> conflicts;
    
    // 查找当前作用域中的符号
    auto existing_symbol = current_scope_->findSymbol(name);
    if (existing_symbol) {
        ConflictInfo conflict;
        conflict.existing_symbol = existing_symbol;
        
        // 创建新符号用于比较
        std::string current_namespace = getCurrentNamespace();
        conflict.new_symbol = std::make_shared<Symbol>(name, type, value, current_namespace);
        
        if (existing_symbol->type != type) {
            conflict.conflict_type = "类型冲突";
            conflict.suggestion = "符号 '" + name + "' 已存在不同类型定义，请使用不同的名称或确认类型一致性";
        } else if (existing_symbol->value != value && !value.empty()) {
            conflict.conflict_type = "值冲突";
            conflict.suggestion = "符号 '" + name + "' 已存在不同值定义，可能需要合并或重命名";
        } else {
            conflict.conflict_type = "重复定义";
            conflict.suggestion = "符号 '" + name + "' 重复定义，如果是命名空间合并则正常，否则请检查";
        }
        
        conflicts.push_back(conflict);
    }
    
    // 检查父作用域中的潜在冲突
    if (current_scope_->parent) {
        auto parent_symbol = current_scope_->parent->findSymbolRecursive(name);
        if (parent_symbol && parent_symbol != existing_symbol) {
            ConflictInfo conflict;
            conflict.existing_symbol = parent_symbol;
            
            std::string current_namespace = getCurrentNamespace();
            conflict.new_symbol = std::make_shared<Symbol>(name, type, value, current_namespace);
            
            conflict.conflict_type = "作用域遮蔽";
            conflict.suggestion = "符号 '" + name + "' 将遮蔽外层作用域的同名符号";
            conflicts.push_back(conflict);
        }
    }
    
    return conflicts;
}

bool GlobalMap::hasNamespaceConflict(const std::string& namespace_path) {
    // 检查命名空间路径是否与现有符号冲突
    auto parts = getNamespaceHierarchy();
    std::string current_path = "";
    
    for (const std::string& part : parts) {
        if (!current_path.empty()) current_path += "::";
        current_path += part;
        
        // 检查是否存在同名但不同类型的符号
        auto symbol = findSymbolInNamespace("", current_path);
        if (symbol && symbol->type != SymbolType::NAMESPACE) {
            return true;
        }
    }
    
    return false;
}

std::vector<GlobalMap::ConflictInfo> GlobalMap::getNamespaceConflicts(const std::string& namespace_path) {
    std::vector<ConflictInfo> conflicts;
    
    // 分解命名空间路径并检查每个部分
    std::vector<std::string> parts;
    std::stringstream ss(namespace_path);
    std::string part;
    
    while (std::getline(ss, part, ':')) {
        if (!part.empty() && part != ":") {
            parts.push_back(part);
        }
    }
    
    std::string current_path = "";
    for (const std::string& part : parts) {
        if (!current_path.empty()) current_path += "::";
        current_path += part;
        
        auto existing_symbol = findSymbolInNamespace("", current_path);
        if (existing_symbol && existing_symbol->type != SymbolType::NAMESPACE) {
            ConflictInfo conflict;
            conflict.existing_symbol = existing_symbol;
            conflict.new_symbol = std::make_shared<Symbol>(part, SymbolType::NAMESPACE, "", current_path);
            conflict.conflict_type = "命名空间与符号冲突";
            conflict.suggestion = "命名空间路径 '" + current_path + "' 与现有符号冲突";
            conflicts.push_back(conflict);
        }
    }
    
    return conflicts;
}

bool GlobalMap::hasCyclicDependency(const std::string& target_path) {
    std::unordered_set<std::string> visited;
    std::string current_path = getCurrentNamespace();
    if (current_path.empty()) current_path = "global";
    
    return detectCyclicDependencyHelper(current_path, target_path, visited);
}

void GlobalMap::addDependency(const std::string& from_path, const std::string& to_path) {
    dependencies_[from_path].push_back(to_path);
}

void GlobalMap::exportSymbol(const std::string& name) {
    auto symbol = findSymbol(name);
    if (symbol) {
        symbol->is_exported = true;
    }
}

std::vector<std::shared_ptr<Symbol>> GlobalMap::getExportedSymbols() {
    std::vector<std::shared_ptr<Symbol>> exported;
    
    // 递归收集所有导出的符号
    std::function<void(Scope*)> collectExported = [&](Scope* scope) {
        for (const auto& pair : scope->symbols) {
            if (pair.second->is_exported) {
                exported.push_back(pair.second);
            }
        }
        for (const auto& child : scope->children) {
            collectExported(child.get());
        }
    };
    
    collectExported(root_scope_.get());
    return exported;
}

void GlobalMap::printSymbolTable() const {
    std::cout << "=== 符号表 ===" << std::endl;
    printScope(root_scope_.get());
}

void GlobalMap::printScopeHierarchy() const {
    std::cout << "=== 作用域层次结构 ===" << std::endl;
    printScope(root_scope_.get());
}

std::string GlobalMap::getStatistics() const {
    std::ostringstream oss;
    
    // 统计符号数量
    int symbol_count = 0;
    int scope_count = 0;
    
    std::function<void(Scope*)> countSymbols = [&](Scope* scope) {
        scope_count++;
        symbol_count += scope->symbols.size();
        for (const auto& child : scope->children) {
            countSymbols(child.get());
        }
    };
    
    countSymbols(root_scope_.get());
    
    oss << "符号总数: " << symbol_count << std::endl;
    oss << "作用域总数: " << scope_count << std::endl;
    oss << "导入总数: " << imports_.size() << std::endl;
    oss << "依赖关系总数: " << dependencies_.size() << std::endl;
    
    return oss.str();
}

void GlobalMap::clear() {
    root_scope_ = std::make_unique<Scope>("global");
    current_scope_ = root_scope_.get();
    while (!namespace_stack_.empty()) {
        namespace_stack_.pop();
    }
    imports_.clear();
    dependencies_.clear();
}

void GlobalMap::printScope(const Scope* scope, int indent) const {
    std::string prefix(indent * 2, ' ');
    std::cout << prefix << "作用域: " << scope->name << std::endl;
    
    // 打印符号
    for (const auto& pair : scope->symbols) {
        const auto& symbol = pair.second;
        std::cout << prefix << "  符号: " << symbol->name 
                  << " (类型: " << static_cast<int>(symbol->type) << ")" << std::endl;
    }
    
    // 递归打印子作用域
    for (const auto& child : scope->children) {
        printScope(child.get(), indent + 1);
    }
}

bool GlobalMap::detectCyclicDependencyHelper(const std::string& current, 
                                            const std::string& target,
                                            std::unordered_set<std::string>& visited) const {
    if (current == target) return true;
    if (visited.count(current)) return false;
    
    visited.insert(current);
    
    auto it = dependencies_.find(current);
    if (it != dependencies_.end()) {
        for (const std::string& dependency : it->second) {
            if (detectCyclicDependencyHelper(dependency, target, visited)) {
                return true;
            }
        }
    }
    
    return false;
}

} // namespace chtl