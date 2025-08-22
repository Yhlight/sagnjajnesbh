#include "CHTL/CHTLContext.hpp"

namespace CHTL {

CHTLContext::CHTLContext() : debugMode_(false) {
    // 初始化全局作用域
    EnterScope("global", "global");
}

void CHTLContext::EnterScope(const String& name, const String& type, const SourceLocation& location) {
    Scope scope(name, type, location);
    scopeStack_.push_back(scope);
}

void CHTLContext::ExitScope() {
    if (scopeStack_.size() > 1) { // 保持至少一个作用域
        scopeStack_.pop_back();
    }
}

const CHTLContext::Scope& CHTLContext::GetCurrentScope() const {
    return scopeStack_.back();
}

bool CHTLContext::IsInScope(const String& scopeName) const {
    for (const auto& scope : scopeStack_) {
        if (scope.name == scopeName) {
            return true;
        }
    }
    return false;
}

size_t CHTLContext::GetScopeDepth() const {
    return scopeStack_.size();
}

std::vector<CHTLContext::Scope> CHTLContext::GetScopeChain() const {
    return scopeStack_;
}

void CHTLContext::DefineVariable(const String& name, const String& value, const String& scope) {
    if (scope.empty()) {
        // 在当前作用域定义变量
        scopeStack_.back().localVariables[name] = value;
    } else {
        // 在指定作用域定义变量
        for (auto& s : scopeStack_) {
            if (s.name == scope) {
                s.localVariables[name] = value;
                return;
            }
        }
    }
}

Optional<String> CHTLContext::ResolveVariable(const String& name) const {
    return ResolveScopedVariable(name, scopeStack_.size());
}

bool CHTLContext::IsVariableDefined(const String& name) const {
    return ResolveVariable(name).has_value();
}

void CHTLContext::SetVariableInCurrentScope(const String& name, const String& value) {
    scopeStack_.back().localVariables[name] = value;
}

void CHTLContext::AddAllowedReference(const String& reference) {
    scopeStack_.back().allowedReferences.insert(reference);
}

void CHTLContext::AddForbiddenReference(const String& reference) {
    scopeStack_.back().forbiddenReferences.insert(reference);
    globalConstraints_.insert(reference);
}

bool CHTLContext::IsReferenceAllowed(const String& reference) const {
    const auto& currentScope = GetCurrentScope();
    
    // 检查当前作用域的允许引用
    if (!currentScope.allowedReferences.empty()) {
        return currentScope.allowedReferences.find(reference) != currentScope.allowedReferences.end();
    }
    
    return true; // 默认允许
}

bool CHTLContext::IsReferenceForbidden(const String& reference) const {
    // 检查全局约束
    if (globalConstraints_.find(reference) != globalConstraints_.end()) {
        return true;
    }
    
    // 检查当前作用域的禁止引用
    const auto& currentScope = GetCurrentScope();
    return currentScope.forbiddenReferences.find(reference) != currentScope.forbiddenReferences.end();
}

CHTLContext::InheritanceChain CHTLContext::ResolveInheritance(const String& name, const String& type) const {
    InheritanceChain chain;
    
    if (type.find("Template") != String::npos) {
        chain.resolvedProperties = ResolveTemplateInheritance(name, type);
    } else if (type.find("Custom") != String::npos) {
        chain.resolvedProperties = ResolveCustomInheritance(name, type);
    }
    
    return chain;
}

StringMap CHTLContext::ResolveTemplateInheritance(const String& templateName, const String& templateType) const {
    StringMap properties;
    
    // 通过全局映射表解析模板继承
    if (globalMap_) {
        auto templateInfo = globalMap_->GetTemplate(templateName, templateType);
        if (templateInfo.has_value()) {
            properties = templateInfo->properties;
            
            // 递归解析继承链
            for (const String& parent : templateInfo->inheritance) {
                StringMap parentProps = ResolveTemplateInheritance(parent, templateType);
                MergeInheritedProperties(properties, parentProps);
            }
        }
    }
    
    return properties;
}

StringMap CHTLContext::ResolveCustomInheritance(const String& customName, const String& customType) const {
    StringMap properties;
    
    // 通过全局映射表解析自定义继承
    if (globalMap_) {
        auto customInfo = globalMap_->GetCustom(customName, customType);
        if (customInfo.has_value()) {
            properties = customInfo->properties;
            
            // 处理特例化
            for (const String& specialization : customInfo->specializations) {
                // 处理特例化逻辑
                // 这里可以根据需要实现具体的特例化处理
            }
        }
    }
    
    return properties;
}

String CHTLContext::ResolveNamespacedName(const String& namespacePath, const String& itemName) const {
    if (globalMap_) {
        return globalMap_->ResolveNamespacedReference(namespacePath, itemName, "");
    }
    return namespacePath + "::" + itemName;
}

bool CHTLContext::IsInNamespace(const String& namespaceName) const {
    for (const auto& scope : scopeStack_) {
        if (scope.type == "namespace" && scope.name == namespaceName) {
            return true;
        }
    }
    return false;
}

String CHTLContext::GetCurrentNamespace() const {
    for (auto it = scopeStack_.rbegin(); it != scopeStack_.rend(); ++it) {
        if (it->type == "namespace") {
            return it->name;
        }
    }
    return "";
}

CHTLContext::ResolvedTemplate CHTLContext::ResolveTemplate(const String& name, const String& type) const {
    ResolvedTemplate resolved;
    resolved.name = name;
    resolved.type = type;
    
    if (globalMap_) {
        auto templateInfo = globalMap_->GetTemplate(name, type);
        if (templateInfo.has_value()) {
            resolved.properties = templateInfo->properties;
            resolved.inheritanceChain = templateInfo->inheritance;
            resolved.isResolved = true;
        }
    }
    
    return resolved;
}

CHTLContext::ResolvedCustom CHTLContext::ResolveCustom(const String& name, const String& type) const {
    ResolvedCustom resolved;
    resolved.name = name;
    resolved.type = type;
    
    if (globalMap_) {
        auto customInfo = globalMap_->GetCustom(name, type);
        if (customInfo.has_value()) {
            resolved.properties = customInfo->properties;
            resolved.specializationChain = customInfo->specializations;
            resolved.isResolved = true;
        }
    }
    
    return resolved;
}

String CHTLContext::ResolveVariableGroup(const String& groupName, const String& varName) const {
    if (globalMap_) {
        return globalMap_->ResolveVariableReference(groupName, varName);
    }
    return "";
}

String CHTLContext::ResolveVariableGroupWithSpecialization(const String& groupName, const String& varName, 
                                                            const StringMap& specializations) const {
    // 首先检查特例化
    auto specIt = specializations.find(varName);
    if (specIt != specializations.end()) {
        return specIt->second;
    }
    
    // 否则使用默认解析
    return ResolveVariableGroup(groupName, varName);
}

bool CHTLContext::ValidateConstraints(const String& itemName, const String& itemType) const {
    String fullName = itemType + "::" + itemName;
    
    // 检查是否被禁止
    if (IsReferenceForbidden(itemName) || IsReferenceForbidden(fullName) || IsReferenceForbidden(itemType)) {
        return false;
    }
    
    return true;
}

void CHTLContext::AddConstraint(const String& constraint) {
    AddForbiddenReference(constraint);
}

void CHTLContext::RemoveConstraint(const String& constraint) {
    globalConstraints_.erase(constraint);
    
    // 从当前作用域移除
    scopeStack_.back().forbiddenReferences.erase(constraint);
}

StringSet CHTLContext::GetActiveConstraints() const {
    return globalConstraints_;
}

void CHTLContext::SetGlobalMap(SharedPtr<CHTLGlobalMap> globalMap) {
    globalMap_ = globalMap;
}

SharedPtr<CHTLGlobalMap> CHTLContext::GetGlobalMap() const {
    return globalMap_;
}

void CHTLContext::SetStateManager(SharedPtr<CHTLStateManager> stateManager) {
    stateManager_ = stateManager;
}

SharedPtr<CHTLStateManager> CHTLContext::GetStateManager() const {
    return stateManager_;
}

void CHTLContext::AddError(const CompilerError& error) {
    errors_.push_back(error);
}

void CHTLContext::AddWarning(const String& message, const SourceLocation& location) {
    errors_.emplace_back(ErrorLevel::Warning, message, location.filename, location.line, location.column);
}

std::vector<CompilerError> CHTLContext::GetErrors() const {
    return errors_;
}

bool CHTLContext::HasErrors() const {
    for (const auto& error : errors_) {
        if (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal) {
            return true;
        }
    }
    return false;
}

void CHTLContext::ClearErrors() {
    errors_.clear();
}

void CHTLContext::EnableDebugMode(bool enable) {
    debugMode_ = enable;
}

bool CHTLContext::IsDebugMode() const {
    return debugMode_;
}

void CHTLContext::LogDebugInfo(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTLContext] " << message << std::endl;
    }
}

void CHTLContext::Reset() {
    scopeStack_.clear();
    globalConstraints_.clear();
    errors_.clear();
    
    // 重新初始化全局作用域
    EnterScope("global", "global");
}

void CHTLContext::Clear() {
    Reset();
}

CHTLContext::ScopeGuard CHTLContext::CreateScopeGuard(const String& name, const String& type, 
                                                       const SourceLocation& location) {
    return ScopeGuard(*this, name, type, location);
}

// 私有辅助方法实现
String CHTLContext::ResolveScopedVariable(const String& name, size_t maxDepth) const {
    // 从最内层作用域开始向外查找
    for (size_t i = 0; i < maxDepth && i < scopeStack_.size(); ++i) {
        size_t index = scopeStack_.size() - 1 - i;
        const auto& scope = scopeStack_[index];
        
        auto it = scope.localVariables.find(name);
        if (it != scope.localVariables.end()) {
            return it->second;
        }
    }
    
    return "";
}

bool CHTLContext::CheckConstraintInScope(const String& constraint, const Scope& scope) const {
    return scope.forbiddenReferences.find(constraint) != scope.forbiddenReferences.end();
}

void CHTLContext::MergeInheritedProperties(StringMap& target, const StringMap& source) const {
    for (const auto& pair : source) {
        // 如果目标中不存在该属性，则添加
        if (target.find(pair.first) == target.end()) {
            target[pair.first] = pair.second;
        }
        // 否则保持目标中的值（子类覆盖父类）
    }
}

// ScopeGuard实现
CHTLContext::ScopeGuard::ScopeGuard(CHTLContext& context, const String& name, const String& type, 
                                     const SourceLocation& location)
    : context_(&context), active_(true) {
    context_->EnterScope(name, type, location);
}

CHTLContext::ScopeGuard::~ScopeGuard() {
    if (active_ && context_) {
        context_->ExitScope();
    }
}

CHTLContext::ScopeGuard::ScopeGuard(ScopeGuard&& other) noexcept
    : context_(other.context_), active_(other.active_) {
    other.active_ = false;
}

CHTLContext::ScopeGuard& CHTLContext::ScopeGuard::operator=(ScopeGuard&& other) noexcept {
    if (this != &other) {
        if (active_ && context_) {
            context_->ExitScope();
        }
        context_ = other.context_;
        active_ = other.active_;
        other.active_ = false;
    }
    return *this;
}

} // namespace CHTL