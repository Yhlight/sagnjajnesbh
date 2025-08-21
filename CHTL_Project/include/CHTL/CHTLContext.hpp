#pragma once

#include "../Core/CHTLTypes.hpp"
#include "CHTLState.hpp"
#include "CHTLGlobalMap.hpp"

namespace CHTL {

/**
 * @brief CHTL编译上下文
 * 
 * 管理编译过程中的上下文信息，包括作用域、变量解析、继承关系等
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;

    // 作用域管理
    struct Scope {
        String name;
        String type;  // element, template, custom, namespace等
        StringMap localVariables;
        StringSet allowedReferences;
        StringSet forbiddenReferences;  // except约束
        SourceLocation location;
        
        Scope(const String& n = "", const String& t = "", const SourceLocation& loc = {})
            : name(n), type(t), location(loc) {}
    };

    // 进入新作用域
    void EnterScope(const String& name, const String& type, const SourceLocation& location = {});
    
    // 退出当前作用域
    void ExitScope();
    
    // 获取当前作用域
    const Scope& GetCurrentScope() const;
    
    // 作用域查询
    bool IsInScope(const String& scopeName) const;
    size_t GetScopeDepth() const;
    std::vector<Scope> GetScopeChain() const;

    // 变量管理
    void DefineVariable(const String& name, const String& value, const String& scope = "");
    Optional<String> ResolveVariable(const String& name) const;
    bool IsVariableDefined(const String& name) const;
    void SetVariableInCurrentScope(const String& name, const String& value);

    // 引用管理
    void AddAllowedReference(const String& reference);
    void AddForbiddenReference(const String& reference);  // except约束
    bool IsReferenceAllowed(const String& reference) const;
    bool IsReferenceForbidden(const String& reference) const;

    // 继承解析
    struct InheritanceChain {
        StringList templateChain;
        StringList customChain;
        StringMap resolvedProperties;
    };

    InheritanceChain ResolveInheritance(const String& name, const String& type) const;
    StringMap ResolveTemplateInheritance(const String& templateName, const String& templateType) const;
    StringMap ResolveCustomInheritance(const String& customName, const String& customType) const;

    // 命名空间解析
    String ResolveNamespacedName(const String& namespacePath, const String& itemName) const;
    bool IsInNamespace(const String& namespaceName) const;
    String GetCurrentNamespace() const;

    // 模板和自定义解析
    struct ResolvedTemplate {
        String name;
        String type;
        StringMap properties;
        StringList inheritanceChain;
        bool isResolved;
        
        ResolvedTemplate() : isResolved(false) {}
    };

    struct ResolvedCustom {
        String name;
        String type;
        StringMap properties;
        StringList specializationChain;
        bool isResolved;
        
        ResolvedCustom() : isResolved(false) {}
    };

    ResolvedTemplate ResolveTemplate(const String& name, const String& type) const;
    ResolvedCustom ResolveCustom(const String& name, const String& type) const;

    // 变量组解析
    String ResolveVariableGroup(const String& groupName, const String& varName) const;
    String ResolveVariableGroupWithSpecialization(const String& groupName, const String& varName, 
                                                   const StringMap& specializations) const;

    // 约束验证
    bool ValidateConstraints(const String& itemName, const String& itemType) const;
    void AddConstraint(const String& constraint);  // except约束
    void RemoveConstraint(const String& constraint);
    StringSet GetActiveConstraints() const;

    // 上下文状态
    void SetGlobalMap(SharedPtr<CHTLGlobalMap> globalMap);
    SharedPtr<CHTLGlobalMap> GetGlobalMap() const;
    
    void SetStateManager(SharedPtr<CHTLStateManager> stateManager);
    SharedPtr<CHTLStateManager> GetStateManager() const;

    // 错误处理
    void AddError(const CompilerError& error);
    void AddWarning(const String& message, const SourceLocation& location = {});
    std::vector<CompilerError> GetErrors() const;
    bool HasErrors() const;
    void ClearErrors();

    // 调试信息
    void EnableDebugMode(bool enable = true);
    bool IsDebugMode() const;
    void LogDebugInfo(const String& message) const;

    // 重置和清理
    void Reset();
    void Clear();

    // RAII作用域管理助手
    class ScopeGuard {
    public:
        ScopeGuard(CHTLContext& context, const String& name, const String& type, 
                   const SourceLocation& location = {});
        ~ScopeGuard();
        
        // 禁用拷贝
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
        
        // 支持移动
        ScopeGuard(ScopeGuard&& other) noexcept;
        ScopeGuard& operator=(ScopeGuard&& other) noexcept;
        
    private:
        CHTLContext* context_;
        bool active_;
    };

    // 创建作用域守护
    ScopeGuard CreateScopeGuard(const String& name, const String& type, 
                                 const SourceLocation& location = {});

private:
    // 内部数据
    std::vector<Scope> scopeStack_;
    StringSet globalConstraints_;  // except约束
    std::vector<CompilerError> errors_;
    bool debugMode_;
    
    // 外部引用
    SharedPtr<CHTLGlobalMap> globalMap_;
    SharedPtr<CHTLStateManager> stateManager_;
    
    // 辅助方法
    String ResolveScopedVariable(const String& name, size_t maxDepth) const;
    bool CheckConstraintInScope(const String& constraint, const Scope& scope) const;
    void MergeInheritedProperties(StringMap& target, const StringMap& source) const;
};

} // namespace CHTL