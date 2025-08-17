#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include <unordered_map>
#include <memory>

namespace chtl {

// 前向声明
class CHTLASTNode;

// 模板类型
enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

// 自定义类型
enum class CustomType {
    STYLE,      // 自定义样式组
    ELEMENT,    // 自定义元素
    VAR         // 自定义变量组
};

// 模板项
struct TemplateItem {
    TemplateType type;
    String name;
    String namespace_;
    std::shared_ptr<CHTLASTNode> ast;
    StringMap properties;  // 模板属性
    StringList dependencies; // 依赖的其他模板
    
    TemplateItem(TemplateType t, const String& n, const String& ns = "")
        : type(t), name(n), namespace_(ns) {}
};

// 自定义项
struct CustomItem {
    CustomType type;
    String name;
    String namespace_;
    std::shared_ptr<CHTLASTNode> ast;
    StringMap properties;
    StringList dependencies;
    StringList inheritedTemplates; // 继承的模板
    
    CustomItem(CustomType t, const String& n, const String& ns = "")
        : type(t), name(n), namespace_(ns) {}
};

// 变量项
struct VariableItem {
    String name;
    String namespace_;
    String value;
    String type; // 变量类型(可选)
    
    VariableItem(const String& n, const String& v, const String& ns = "")
        : name(n), namespace_(ns), value(v) {}
};

// 原始嵌入项
struct OriginItem {
    String type; // @Html, @Style, @JavaScript, 或自定义类型
    String name;
    String namespace_;
    String content;
    
    OriginItem(const String& t, const String& n, const String& c, const String& ns = "")
        : type(t), name(n), namespace_(ns), content(c) {}
};

// 导入项
struct ImportItem {
    String type;        // 导入类型 (@Html, @Chtl, 等)
    String path;        // 导入路径
    String alias;       // 别名 (as语法)
    String namespace_;  // 命名空间
    StringList items;   // 具体导入的项目
    
    ImportItem(const String& t, const String& p, const String& ns = "")
        : type(t), path(p), namespace_(ns) {}
};

// 命名空间项
struct NamespaceItem {
    String name;
    String parentNamespace;
    StringSet templates;
    StringSet customs;
    StringSet variables;
    StringSet origins;
    StringSet imports;
    StringSet childNamespaces;
    
    NamespaceItem(const String& n, const String& parent = "")
        : name(n), parentNamespace(parent) {}
};

/**
 * 全局映射管理器
 * 管理所有模板、自定义、变量、原始嵌入、导入和命名空间的全局映射
 */
class GlobalMap {
public:
    static GlobalMap& getInstance();
    
    // 清空所有映射
    void clear();
    
    // 模板管理
    void addTemplate(const TemplateItem& item);
    TemplateItem* getTemplate(const String& name, const String& namespace_ = "");
    bool hasTemplate(const String& name, const String& namespace_ = "") const;
    void removeTemplate(const String& name, const String& namespace_ = "");
    std::vector<TemplateItem*> getTemplatesByType(TemplateType type, const String& namespace_ = "");
    
    // 自定义管理
    void addCustom(const CustomItem& item);
    CustomItem* getCustom(const String& name, const String& namespace_ = "");
    bool hasCustom(const String& name, const String& namespace_ = "") const;
    void removeCustom(const String& name, const String& namespace_ = "");
    std::vector<CustomItem*> getCustomsByType(CustomType type, const String& namespace_ = "");
    
    // 变量管理
    void addVariable(const VariableItem& item);
    VariableItem* getVariable(const String& name, const String& namespace_ = "");
    bool hasVariable(const String& name, const String& namespace_ = "") const;
    void removeVariable(const String& name, const String& namespace_ = "");
    String getVariableValue(const String& name, const String& namespace_ = "");
    
    // 原始嵌入管理
    void addOrigin(const OriginItem& item);
    OriginItem* getOrigin(const String& name, const String& namespace_ = "");
    bool hasOrigin(const String& name, const String& namespace_ = "") const;
    void removeOrigin(const String& name, const String& namespace_ = "");
    std::vector<OriginItem*> getOriginsByType(const String& type, const String& namespace_ = "");
    
    // 导入管理
    void addImport(const ImportItem& item);
    ImportItem* getImport(const String& path, const String& namespace_ = "");
    bool hasImport(const String& path, const String& namespace_ = "") const;
    void removeImport(const String& path, const String& namespace_ = "");
    std::vector<ImportItem*> getImportsByType(const String& type, const String& namespace_ = "");
    
    // 命名空间管理
    void addNamespace(const NamespaceItem& item);
    NamespaceItem* getNamespace(const String& name);
    bool hasNamespace(const String& name) const;
    void removeNamespace(const String& name);
    String resolveFullNamespace(const String& current, const String& target);
    std::vector<String> getNamespaceHierarchy(const String& namespace_);
    
    // 名称解析
    String resolveTemplateName(const String& name, const String& currentNamespace = "");
    String resolveCustomName(const String& name, const String& currentNamespace = "");
    String resolveVariableName(const String& name, const String& currentNamespace = "");
    
    // 依赖分析
    std::vector<String> getTemplateDependencies(const String& name, const String& namespace_ = "");
    std::vector<String> getCustomDependencies(const String& name, const String& namespace_ = "");
    bool hasCircularDependency(const String& name, const String& namespace_ = "");
    
    // 冲突检测
    bool hasNameConflict(const String& name, const String& namespace_ = "") const;
    std::vector<String> getConflictingNames(const String& name, const String& namespace_ = "") const;
    
    // 统计信息
    struct Statistics {
        size_t templateCount = 0;
        size_t customCount = 0;
        size_t variableCount = 0;
        size_t originCount = 0;
        size_t importCount = 0;
        size_t namespaceCount = 0;
    };
    
    Statistics getStatistics() const;
    
    // 调试和序列化
    String dumpToString() const;
    void dumpToFile(const String& filename) const;
    
private:
    GlobalMap() = default;
    
    // 存储映射
    std::unordered_map<String, std::unique_ptr<TemplateItem>> templates_;
    std::unordered_map<String, std::unique_ptr<CustomItem>> customs_;
    std::unordered_map<String, std::unique_ptr<VariableItem>> variables_;
    std::unordered_map<String, std::unique_ptr<OriginItem>> origins_;
    std::unordered_map<String, std::unique_ptr<ImportItem>> imports_;
    std::unordered_map<String, std::unique_ptr<NamespaceItem>> namespaces_;
    
    // 辅助方法
    String makeKey(const String& name, const String& namespace_) const;
    std::pair<String, String> parseKey(const String& key) const;
    String getCurrentNamespace() const;
    void setCurrentNamespace(const String& namespace_);
    
    // 依赖分析辅助
    void collectDependencies(const String& name, const String& namespace_, 
                           std::unordered_set<String>& visited, 
                           std::vector<String>& dependencies);
    
    String currentNamespace_;
};

} // namespace chtl