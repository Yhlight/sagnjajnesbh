#pragma once
#include "../AST/CHTLNodes.h"
#include "../Template/TemplateManager.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace chtl {
namespace custom_system {

// 自定义样式组
struct CustomStyle {
    std::string name;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_set<std::string> noValueProperties; // 无值属性
    std::vector<std::string> inheritedTemplates;
    std::vector<std::string> inheritedCustoms;
};

// 自定义元素
struct CustomElement {
    std::string name;
    std::vector<std::unique_ptr<ast::ASTNode>> elements;
    std::vector<std::string> inheritedTemplates;
    std::vector<std::string> inheritedCustoms;
    
    // 移动构造函数和赋值操作符
    CustomElement() = default;
    CustomElement(const CustomElement&) = delete;
    CustomElement& operator=(const CustomElement&) = delete;
    CustomElement(CustomElement&&) = default;
    CustomElement& operator=(CustomElement&&) = default;
};

// 自定义变量组
struct CustomVariable {
    std::string name;
    std::unordered_map<std::string, std::string> variables;
    std::unordered_set<std::string> noValueVariables; // 无值变量
    std::vector<std::string> inheritedTemplates;
    std::vector<std::string> inheritedCustoms;
};

// 特例化操作
struct SpecializationOperation {
    std::string operation; // inherit, delete, insert, replace
    std::string target;
    std::string position; // after, before, at top, at bottom
    std::vector<std::unique_ptr<ast::ASTNode>> content; // 插入的内容
    
    // 移动构造函数和赋值操作符
    SpecializationOperation() = default;
    SpecializationOperation(const SpecializationOperation&) = delete;
    SpecializationOperation& operator=(const SpecializationOperation&) = delete;
    SpecializationOperation(SpecializationOperation&&) = default;
    SpecializationOperation& operator=(SpecializationOperation&&) = default;
};

// 自定义管理器
class CustomManager {
public:
    CustomManager();
    ~CustomManager();
    
    // 自定义样式组管理
    void addCustomStyle(const std::string& name, const CustomStyle& custom);
    std::shared_ptr<CustomStyle> getCustomStyle(const std::string& name);
    bool hasCustomStyle(const std::string& name) const;
    
    // 自定义元素管理
    void addCustomElement(const std::string& name, CustomElement&& custom);
    std::shared_ptr<CustomElement> getCustomElement(const std::string& name);
    bool hasCustomElement(const std::string& name) const;
    
    // 自定义变量组管理
    void addCustomVariable(const std::string& name, const CustomVariable& custom);
    std::shared_ptr<CustomVariable> getCustomVariable(const std::string& name);
    bool hasCustomVariable(const std::string& name) const;
    
    // 特例化操作处理
    bool applySpecialization(const std::string& targetName, const SpecializationOperation& operation);
    
    // 无值样式组处理
    std::unordered_map<std::string, std::string> resolveNoValueStyle(const std::string& styleName, 
                                                                     const std::unordered_map<std::string, std::string>& values);
    
    // 无值变量组处理
    std::unordered_map<std::string, std::string> resolveNoValueVariable(const std::string& varName,
                                                                        const std::unordered_map<std::string, std::string>& values);
    
    // 继承解析
    std::unordered_map<std::string, std::string> resolveCustomStyleInheritance(const std::string& customName);
    std::vector<std::unique_ptr<ast::ASTNode>> resolveCustomElementInheritance(const std::string& customName);
    std::unordered_map<std::string, std::string> resolveCustomVariableInheritance(const std::string& customName);
    
    // 删除操作
    bool deleteProperty(const std::string& styleName, const std::vector<std::string>& propertiesToDelete);
    bool deleteElement(const std::string& elementName, const std::string& elementToDelete);
    bool deleteInheritance(const std::string& targetName, const std::string& inheritanceToDelete);
    
    // 插入操作
    bool insertElement(const std::string& elementName, const std::string& position, 
                      const std::string& selector, std::vector<std::unique_ptr<ast::ASTNode>> newElements);
    
    // 索引访问
    ast::ASTNode* getElementByIndex(const std::string& elementName, const std::string& elementType, int index);
    
    // 验证
    bool validateCustom(const std::string& name, const std::string& type);
    std::vector<std::string> getCustomErrors() const { return errors_; }
    
    // 清理
    void clear();
    void clearErrors() { errors_.clear(); }

private:
    std::unordered_map<std::string, std::shared_ptr<CustomStyle>> customStyles_;
    std::unordered_map<std::string, std::shared_ptr<CustomElement>> customElements_;
    std::unordered_map<std::string, std::shared_ptr<CustomVariable>> customVariables_;
    
    std::vector<std::string> errors_;
    
    // 模板管理器引用
    std::shared_ptr<template_system::TemplateManager> templateManager_;
    
    // 内部方法
    void processCustomInheritance(CustomStyle& custom);
    void processCustomInheritance(CustomElement& custom);
    void processCustomInheritance(CustomVariable& custom);
    
    bool checkCircularDependency(const std::string& customName, const std::string& type,
                                std::unordered_set<std::string>& visited);
    
    // 特例化操作实现
    bool applyDeleteOperation(const std::string& targetName, const SpecializationOperation& operation);
    bool applyInsertOperation(const std::string& targetName, const SpecializationOperation& operation);
    bool applyReplaceOperation(const std::string& targetName, const SpecializationOperation& operation);
    bool applyInheritOperation(const std::string& targetName, const SpecializationOperation& operation);
    
    bool deleteElementByIndex(const std::string& elementName, const std::string& indexExpression);
    
    // 工具方法
    std::vector<std::string> parseCommaSeparatedList(const std::string& input);
};

// 全局自定义管理器实例
extern std::shared_ptr<CustomManager> g_customManager;

} // namespace custom_system
} // namespace chtl