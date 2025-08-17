#pragma once

#include "CHTLCommon.h"
#include "StateMachine.h"
#include "GlobalMap.h"
#include <stack>

namespace CHTL {

// 上下文类型
enum class ContextType {
    GLOBAL,                 // 全局上下文
    ELEMENT,                // 元素上下文
    ATTRIBUTE,              // 属性上下文
    TEXT,                   // 文本上下文
    STYLE_BLOCK,            // 样式块上下文
    SCRIPT_BLOCK,           // 脚本块上下文
    TEMPLATE,               // 模板上下文
    CUSTOM,                 // 自定义上下文
    NAMESPACE,              // 命名空间上下文
    IMPORT,                 // 导入上下文
    CONFIGURATION           // 配置上下文
};

// 约束类型(except关键字)
enum class ConstraintType {
    ELEMENT_CONSTRAINT,     // 元素约束
    TYPE_CONSTRAINT,        // 类型约束
    GLOBAL_CONSTRAINT       // 全局约束
};

// 约束信息
struct ConstraintInfo {
    ConstraintType type;
    StringSet bannedElements;   // 禁用的元素
    StringSet bannedTypes;      // 禁用的类型
    String scope;               // 约束作用域
    
    ConstraintInfo(ConstraintType t = ConstraintType::ELEMENT_CONSTRAINT)
        : type(t) {}
};

// 上下文信息
struct ContextInfo {
    ContextType type;
    String name;                // 上下文名称
    String namespace_;          // 命名空间
    StringMap attributes;       // 当前属性
    StringMap variables;        // 局部变量
    StringList classNames;      // 自动生成的类名
    StringList idNames;         // 自动生成的ID
    ConstraintInfo constraints; // 约束信息
    bool allowCHTLJS;           // 是否允许CHTL JS语法
    bool allowTemplates;        // 是否允许模板
    bool allowCustoms;          // 是否允许自定义
    
    ContextInfo(ContextType t = ContextType::GLOBAL, const String& n = "")
        : type(t), name(n), allowCHTLJS(true), allowTemplates(true), allowCustoms(true) {}
};

/**
 * 上下文管理器
 * 管理编译过程中的上下文状态，为解析器和生成器提供精确的状态信息
 */
class ContextManager {
public:
    ContextManager(StateMachine& stateMachine, GlobalMap& globalMap);
    ~ContextManager() = default;
    
    // 上下文管理
    void pushContext(const ContextInfo& context);
    void popContext();
    const ContextInfo& getCurrentContext() const;
    const ContextInfo& getParentContext() const;
    
    // 上下文查询
    bool isInContext(ContextType type) const;
    bool isInNamespace(const String& namespace_) const;
    String getCurrentNamespace() const;
    String getCurrentElementName() const;
    
    // 属性管理
    void addAttribute(const String& name, const String& value);
    void removeAttribute(const String& name);
    bool hasAttribute(const String& name) const;
    String getAttributeValue(const String& name) const;
    StringMap getAllAttributes() const;
    
    // 变量管理
    void addVariable(const String& name, const String& value);
    void removeVariable(const String& name);
    bool hasVariable(const String& name) const;
    String getVariableValue(const String& name) const;
    String resolveVariable(const String& name) const; // 支持命名空间解析
    
    // 类名和ID管理
    void addAutoClassName(const String& className);
    void addAutoIdName(const String& idName);
    StringList getAutoClassNames() const;
    StringList getAutoIdNames() const;
    void clearAutoNames();
    
    // 约束管理
    void addConstraint(const ConstraintInfo& constraint);
    void removeConstraint();
    bool isElementAllowed(const String& element) const;
    bool isTypeAllowed(const String& type) const;
    bool checkConstraints(const String& element, const String& type = "") const;
    
    // 语法约束检查
    bool isCHTLJSAllowed() const;
    bool areTemplatesAllowed() const;
    bool areCustomsAllowed() const;
    void setCHTLJSAllowed(bool allowed);
    void setTemplatesAllowed(bool allowed);
    void setCustomsAllowed(bool allowed);
    
    // 作用域特定检查
    bool isInGlobalStyleBlock() const;
    bool isInLocalStyleBlock() const;
    bool isInLocalScriptBlock() const;
    bool isInTemplateScope() const;
    bool isInCustomScope() const;
    
    // 语法验证
    bool validateElementUsage(const String& element) const;
    bool validateTypeUsage(const String& type) const;
    bool validateCHTLJSUsage() const;
    bool validateTemplateUsage(const String& templateName) const;
    bool validateCustomUsage(const String& customName) const;
    
    // 名称解析
    String resolveTemplateName(const String& name) const;
    String resolveCustomName(const String& name) const;
    String resolveOriginName(const String& name) const;
    String resolveFullName(const String& name, const String& type) const;
    
    // 上下文路径
    std::vector<String> getContextPath() const;
    String getContextPathString() const;
    size_t getContextDepth() const;
    
    // 继承和特例化支持
    void enterInheritanceContext(const String& parentName);
    void exitInheritanceContext();
    bool isInInheritanceContext() const;
    String getCurrentInheritanceParent() const;
    
    // 错误上下文
    void reportContextError(const String& message);
    void reportConstraintViolation(const String& element, const String& constraint);
    std::vector<String> getContextErrors() const;
    void clearContextErrors();
    
    // 调试和诊断
    String dumpCurrentContext() const;
    String dumpContextStack() const;
    void printContextHierarchy() const;
    
    // 统计信息
    struct Statistics {
        size_t contextPushes = 0;
        size_t contextPops = 0;
        size_t attributeAccesses = 0;
        size_t variableAccesses = 0;
        size_t constraintChecks = 0;
        size_t constraintViolations = 0;
    };
    
    Statistics getStatistics() const;
    void resetStatistics();
    
private:
    StateMachine& stateMachine_;
    GlobalMap& globalMap_;
    std::stack<ContextInfo> contextStack_;
    std::stack<String> inheritanceStack_;
    std::vector<String> contextErrors_;
    Statistics statistics_;
    
    // 辅助方法
    String contextTypeToString(ContextType type) const;
    bool isValidContextTransition(ContextType from, ContextType to) const;
    void updateStatistics(const String& operation);
    
    // 约束检查辅助
    bool checkElementConstraint(const String& element) const;
    bool checkTypeConstraint(const String& type) const;
    bool checkGlobalConstraint() const;
    
    // 名称解析辅助
    String resolveInCurrentNamespace(const String& name, const String& type) const;
    String resolveInParentNamespaces(const String& name, const String& type) const;
    String resolveInGlobalScope(const String& name, const String& type) const;
};

// RAII上下文守卫
class ContextGuard {
public:
    ContextGuard(ContextManager& cm, const ContextInfo& context);
    ~ContextGuard();
    
    // 禁止拷贝和移动
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    ContextGuard(ContextGuard&&) = delete;
    ContextGuard& operator=(ContextGuard&&) = delete;
    
private:
    ContextManager& contextManager_;
};

// 便利宏
#define CHTL_CONTEXT_GUARD(cm, type, name) \
    ContextGuard __ctx_guard(cm, ContextInfo(type, name))

#define CHTL_ELEMENT_CONTEXT(cm, element) \
    ContextGuard __elem_guard(cm, ContextInfo(ContextType::ELEMENT, element))

#define CHTL_STYLE_CONTEXT(cm) \
    ContextGuard __style_guard(cm, ContextInfo(ContextType::STYLE_BLOCK, "style"))

#define CHTL_SCRIPT_CONTEXT(cm) \
    ContextGuard __script_guard(cm, ContextInfo(ContextType::SCRIPT_BLOCK, "script"))

} // namespace CHTL