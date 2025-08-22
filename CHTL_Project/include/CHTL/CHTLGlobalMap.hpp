#pragma once

#include "../Core/CHTLTypes.hpp"
#include <unordered_map>
#include <map>

namespace CHTL {

// 前向声明
struct CHTLToken;

/**
 * @brief CHTL全局映射表
 * 
 * 管理CHTL编译过程中的全局状态和映射关系
 * 包括模板、自定义、变量、命名空间等的映射
 */
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap() = default;

    // 模板相关
    struct TemplateInfo {
        String name;
        String type;  // @Style, @Element, @Var
        StringMap properties;
        StringList inheritance;
        SourceLocation location;
    };

    // 自定义相关
    struct CustomInfo {
        String name;
        String type;  // @Style, @Element, @Var
        StringMap properties;
        StringList specializations;
        SourceLocation location;
    };

    // 变量组相关
    struct VariableGroup {
        String name;
        StringMap variables;
        StringMap specializations;
        SourceLocation location;
    };

    // 命名空间相关
    struct NamespaceInfo {
        String name;
        StringSet templates;
        StringSet customs;
        StringSet variables;
        std::map<String, NamespaceInfo> nestedNamespaces;
        SourceLocation location;
    };

    // 原始嵌入相关
    struct OriginInfo {
        String name;
        String type;  // @Html, @Style, @JavaScript, 自定义类型
        String content;
        SourceLocation location;
    };

    // 导入相关
    struct ImportInfo {
        String path;
        String type;  // @Chtl, @CJmod, @Html, @Style, @JavaScript
        String alias;
        StringSet importedItems;
        SourceLocation location;
    };

    // 配置相关
    struct ConfigurationInfo {
        String name;  // 配置组名称，空表示默认配置
        StringMap settings;
        StringMap nameSettings;
        StringMap originTypes;
        SourceLocation location;
    };

    // 模板管理
    bool RegisterTemplate(const String& name, const String& type, const TemplateInfo& info);
    Optional<TemplateInfo> GetTemplate(const String& name, const String& type) const;
    StringList GetTemplatesByType(const String& type) const;
    bool RemoveTemplate(const String& name, const String& type);

    // 自定义管理
    bool RegisterCustom(const String& name, const String& type, const CustomInfo& info);
    Optional<CustomInfo> GetCustom(const String& name, const String& type) const;
    StringList GetCustomsByType(const String& type) const;
    bool RemoveCustom(const String& name, const String& type);

    // 变量组管理
    bool RegisterVariableGroup(const String& name, const VariableGroup& group);
    Optional<VariableGroup> GetVariableGroup(const String& name) const;
    StringList GetAllVariableGroups() const;
    bool RemoveVariableGroup(const String& name);

    // 命名空间管理
    bool RegisterNamespace(const String& name, const NamespaceInfo& info);
    Optional<NamespaceInfo> GetNamespace(const String& name) const;
    StringList GetAllNamespaces() const;
    bool RemoveNamespace(const String& name);

    // 原始嵌入管理
    bool RegisterOrigin(const String& name, const String& type, const OriginInfo& info);
    Optional<OriginInfo> GetOrigin(const String& name, const String& type) const;
    StringList GetOriginsByType(const String& type) const;
    bool RemoveOrigin(const String& name, const String& type);

    // 导入管理
    bool RegisterImport(const String& path, const ImportInfo& info);
    Optional<ImportInfo> GetImport(const String& path) const;
    StringList GetAllImports() const;
    bool RemoveImport(const String& path);

    // 配置管理
    bool RegisterConfiguration(const String& name, const ConfigurationInfo& info);
    Optional<ConfigurationInfo> GetConfiguration(const String& name = "") const;
    ConfigurationInfo GetActiveConfiguration() const;
    bool SetActiveConfiguration(const String& name);

    // 实用方法
    bool IsTemplateExists(const String& name, const String& type) const;
    bool IsCustomExists(const String& name, const String& type) const;
    bool IsVariableGroupExists(const String& name) const;
    bool IsNamespaceExists(const String& name) const;
    bool IsOriginExists(const String& name, const String& type) const;
    bool IsImportExists(const String& path) const;

    // 解析辅助方法
    String ResolveVariableReference(const String& groupName, const String& varName) const;
    String ResolveNamespacedReference(const String& namespacePath, const String& itemName, const String& itemType) const;
    
    // 清理方法
    void Clear();
    void ClearTemplates();
    void ClearCustoms();
    void ClearVariableGroups();
    void ClearNamespaces();
    void ClearOrigins();
    void ClearImports();
    void ClearConfigurations();

private:
    // 内部存储
    std::map<String, std::map<String, TemplateInfo>> templates_;      // [type][name] -> TemplateInfo
    std::map<String, std::map<String, CustomInfo>> customs_;         // [type][name] -> CustomInfo
    std::map<String, VariableGroup> variableGroups_;                 // [name] -> VariableGroup
    std::map<String, NamespaceInfo> namespaces_;                     // [name] -> NamespaceInfo
    std::map<String, std::map<String, OriginInfo>> origins_;         // [type][name] -> OriginInfo
    std::map<String, ImportInfo> imports_;                           // [path] -> ImportInfo
    std::map<String, ConfigurationInfo> configurations_;             // [name] -> ConfigurationInfo
    
    String activeConfiguration_;  // 当前活动的配置名称
};

} // namespace CHTL