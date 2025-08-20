#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace chtl {
namespace compiler {

// 前向声明
class CHTLTemplateStyle;
class CHTLTemplateElement;
class CHTLTemplateVar;
class CHTLCustomStyle;
class CHTLCustomElement;
class CHTLCustomVar;
class CHTLOriginBlock;
class CHTLConfiguration;
class CHTLNamespace;

/**
 * CHTL 全局映射表
 * 管理所有的模板、自定义、配置等全局定义
 * 注意：这是CHTL专用的，不与CHTL JS共享
 */
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap();
    
    // 模板管理
    void RegisterTemplateStyle(const std::string& name, 
                              std::shared_ptr<CHTLTemplateStyle> style);
    void RegisterTemplateElement(const std::string& name,
                                std::shared_ptr<CHTLTemplateElement> element);
    void RegisterTemplateVar(const std::string& name,
                            std::shared_ptr<CHTLTemplateVar> var);
    
    std::shared_ptr<CHTLTemplateStyle> GetTemplateStyle(const std::string& name) const;
    std::shared_ptr<CHTLTemplateElement> GetTemplateElement(const std::string& name) const;
    std::shared_ptr<CHTLTemplateVar> GetTemplateVar(const std::string& name) const;
    
    // 自定义管理
    void RegisterCustomStyle(const std::string& name,
                            std::shared_ptr<CHTLCustomStyle> style);
    void RegisterCustomElement(const std::string& name,
                              std::shared_ptr<CHTLCustomElement> element);
    void RegisterCustomVar(const std::string& name,
                          std::shared_ptr<CHTLCustomVar> var);
    
    std::shared_ptr<CHTLCustomStyle> GetCustomStyle(const std::string& name) const;
    std::shared_ptr<CHTLCustomElement> GetCustomElement(const std::string& name) const;
    std::shared_ptr<CHTLCustomVar> GetCustomVar(const std::string& name) const;
    
    // 原始嵌入管理
    void RegisterOriginBlock(const std::string& type, const std::string& name,
                            std::shared_ptr<CHTLOriginBlock> block);
    std::shared_ptr<CHTLOriginBlock> GetOriginBlock(const std::string& type,
                                                   const std::string& name) const;
    std::vector<std::string> GetOriginBlockNames(const std::string& type) const;
    
    // 配置管理
    void SetConfiguration(std::shared_ptr<CHTLConfiguration> config);
    std::shared_ptr<CHTLConfiguration> GetConfiguration() const;
    void RegisterNamedConfiguration(const std::string& name,
                                   std::shared_ptr<CHTLConfiguration> config);
    std::shared_ptr<CHTLConfiguration> GetNamedConfiguration(const std::string& name) const;
    
    // 命名空间管理
    void EnterNamespace(const std::string& name);
    void ExitNamespace();
    std::string GetCurrentNamespace() const;
    std::vector<std::string> GetNamespaceStack() const;
    
    // 导入管理
    void RegisterImport(const std::string& path, const std::string& type);
    bool IsImported(const std::string& path) const;
    void AddImportAlias(const std::string& original, const std::string& alias);
    std::optional<std::string> ResolveAlias(const std::string& alias) const;
    
    // 官方模块前缀
    bool IsOfficialModule(const std::string& name) const;
    std::string ResolveModulePath(const std::string& name) const;
    
    // 清理
    void Clear();
    
    // 调试信息
    void DumpTemplates() const;
    void DumpCustoms() const;
    void DumpImports() const;
    
private:
    // 模板存储
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplateStyle>> m_TemplateStyles;
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplateElement>> m_TemplateElements;
    std::unordered_map<std::string, std::shared_ptr<CHTLTemplateVar>> m_TemplateVars;
    
    // 自定义存储
    std::unordered_map<std::string, std::shared_ptr<CHTLCustomStyle>> m_CustomStyles;
    std::unordered_map<std::string, std::shared_ptr<CHTLCustomElement>> m_CustomElements;
    std::unordered_map<std::string, std::shared_ptr<CHTLCustomVar>> m_CustomVars;
    
    // 原始嵌入存储 (type -> (name -> block))
    std::unordered_map<std::string, 
        std::unordered_map<std::string, std::shared_ptr<CHTLOriginBlock>>> m_OriginBlocks;
    
    // 配置存储
    std::shared_ptr<CHTLConfiguration> m_Configuration;
    std::unordered_map<std::string, std::shared_ptr<CHTLConfiguration>> m_NamedConfigurations;
    
    // 命名空间栈
    std::vector<std::string> m_NamespaceStack;
    
    // 导入记录
    std::unordered_map<std::string, std::string> m_Imports;  // path -> type
    std::unordered_map<std::string, std::string> m_Aliases;  // alias -> original
    
    // 辅助方法
    std::string GetFullyQualifiedName(const std::string& name) const;
};

} // namespace compiler
} // namespace chtl