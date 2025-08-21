#include "chtl/core/CHTLGlobalMap.h"
#include "utils/Logger.h"
#include <iostream>

namespace chtl {
namespace compiler {

CHTLGlobalMap::CHTLGlobalMap() {
    // 初始化根命名空间
    m_NamespaceStack.push_back("");
}

CHTLGlobalMap::~CHTLGlobalMap() = default;

void CHTLGlobalMap::RegisterTemplateStyle(const std::string& name,
                                         std::shared_ptr<CHTLTemplateStyle> style) {
    std::string fullName = GetFullyQualifiedName(name);
    m_TemplateStyles[fullName] = style;
    utils::Logger::GetInstance().Debug("注册模板样式: " + fullName);
}

void CHTLGlobalMap::RegisterTemplateElement(const std::string& name,
                                           std::shared_ptr<CHTLTemplateElement> element) {
    std::string fullName = GetFullyQualifiedName(name);
    m_TemplateElements[fullName] = element;
    utils::Logger::GetInstance().Debug("注册模板元素: " + fullName);
}

void CHTLGlobalMap::RegisterTemplateVar(const std::string& name,
                                       std::shared_ptr<CHTLTemplateVar> var) {
    std::string fullName = GetFullyQualifiedName(name);
    m_TemplateVars[fullName] = var;
    utils::Logger::GetInstance().Debug("注册模板变量组: " + fullName);
}

std::shared_ptr<CHTLTemplateStyle> CHTLGlobalMap::GetTemplateStyle(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_TemplateStyles.find(fullName);
    if (it != m_TemplateStyles.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_TemplateStyles.find(parentName);
        if (it != m_TemplateStyles.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_TemplateStyles.find(name);
    if (it != m_TemplateStyles.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLTemplateElement> CHTLGlobalMap::GetTemplateElement(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_TemplateElements.find(fullName);
    if (it != m_TemplateElements.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_TemplateElements.find(parentName);
        if (it != m_TemplateElements.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_TemplateElements.find(name);
    if (it != m_TemplateElements.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLTemplateVar> CHTLGlobalMap::GetTemplateVar(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_TemplateVars.find(fullName);
    if (it != m_TemplateVars.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_TemplateVars.find(parentName);
        if (it != m_TemplateVars.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_TemplateVars.find(name);
    if (it != m_TemplateVars.end()) {
        return it->second;
    }
    
    return nullptr;
}

void CHTLGlobalMap::RegisterCustomStyle(const std::string& name,
                                       std::shared_ptr<CHTLCustomStyle> style) {
    std::string fullName = GetFullyQualifiedName(name);
    m_CustomStyles[fullName] = style;
    utils::Logger::GetInstance().Debug("注册自定义样式: " + fullName);
}

void CHTLGlobalMap::RegisterCustomElement(const std::string& name,
                                         std::shared_ptr<CHTLCustomElement> element) {
    std::string fullName = GetFullyQualifiedName(name);
    m_CustomElements[fullName] = element;
    utils::Logger::GetInstance().Debug("注册自定义元素: " + fullName);
}

void CHTLGlobalMap::RegisterCustomVar(const std::string& name,
                                     std::shared_ptr<CHTLCustomVar> var) {
    std::string fullName = GetFullyQualifiedName(name);
    m_CustomVars[fullName] = var;
    utils::Logger::GetInstance().Debug("注册自定义变量组: " + fullName);
}

std::shared_ptr<CHTLCustomStyle> CHTLGlobalMap::GetCustomStyle(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_CustomStyles.find(fullName);
    if (it != m_CustomStyles.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_CustomStyles.find(parentName);
        if (it != m_CustomStyles.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_CustomStyles.find(name);
    if (it != m_CustomStyles.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLCustomElement> CHTLGlobalMap::GetCustomElement(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_CustomElements.find(fullName);
    if (it != m_CustomElements.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_CustomElements.find(parentName);
        if (it != m_CustomElements.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_CustomElements.find(name);
    if (it != m_CustomElements.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLCustomVar> CHTLGlobalMap::GetCustomVar(const std::string& name) const {
    std::string fullName = GetFullyQualifiedName(name);
    auto it = m_CustomVars.find(fullName);
    if (it != m_CustomVars.end()) {
        return it->second;
    }
    
    // 尝试在父命名空间中查找
    size_t lastDot = fullName.rfind('.');
    if (lastDot != std::string::npos) {
        std::string parentName = fullName.substr(0, lastDot) + "." + name;
        it = m_CustomVars.find(parentName);
        if (it != m_CustomVars.end()) {
            return it->second;
        }
    }
    
    // 尝试在全局命名空间中查找
    it = m_CustomVars.find(name);
    if (it != m_CustomVars.end()) {
        return it->second;
    }
    
    return nullptr;
}

void CHTLGlobalMap::RegisterOriginBlock(const std::string& type, const std::string& name,
                                       std::shared_ptr<CHTLOriginBlock> block) {
    m_OriginBlocks[type][name] = block;
    utils::Logger::GetInstance().Debug("注册原始嵌入块: " + type + " " + name);
}

std::shared_ptr<CHTLOriginBlock> CHTLGlobalMap::GetOriginBlock(const std::string& type,
                                                              const std::string& name) const {
    auto typeIt = m_OriginBlocks.find(type);
    if (typeIt != m_OriginBlocks.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return nullptr;
}

std::vector<std::string> CHTLGlobalMap::GetOriginBlockNames(const std::string& type) const {
    std::vector<std::string> names;
    auto typeIt = m_OriginBlocks.find(type);
    if (typeIt != m_OriginBlocks.end()) {
        for (const auto& pair : typeIt->second) {
            names.push_back(pair.first);
        }
    }
    return names;
}

void CHTLGlobalMap::SetConfiguration(std::shared_ptr<CHTLConfiguration> config) {
    m_Configuration = config;
    utils::Logger::GetInstance().Debug("设置默认配置");
}

std::shared_ptr<CHTLConfiguration> CHTLGlobalMap::GetConfiguration() const {
    return m_Configuration;
}

void CHTLGlobalMap::RegisterNamedConfiguration(const std::string& name,
                                              std::shared_ptr<CHTLConfiguration> config) {
    m_NamedConfigurations[name] = config;
    utils::Logger::GetInstance().Debug("注册命名配置: " + name);
}

std::shared_ptr<CHTLConfiguration> CHTLGlobalMap::GetNamedConfiguration(const std::string& name) const {
    auto it = m_NamedConfigurations.find(name);
    if (it != m_NamedConfigurations.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGlobalMap::EnterNamespace(const std::string& name) {
    std::string currentNamespace = GetCurrentNamespace();
    std::string newNamespace = currentNamespace.empty() ? name : currentNamespace + "." + name;
    m_NamespaceStack.push_back(newNamespace);
    utils::Logger::GetInstance().Debug("进入命名空间: " + newNamespace);
}

void CHTLGlobalMap::ExitNamespace() {
    if (m_NamespaceStack.size() > 1) {
        std::string exitingNamespace = m_NamespaceStack.back();
        m_NamespaceStack.pop_back();
        utils::Logger::GetInstance().Debug("退出命名空间: " + exitingNamespace);
    }
}

std::string CHTLGlobalMap::GetCurrentNamespace() const {
    return m_NamespaceStack.empty() ? "" : m_NamespaceStack.back();
}

std::vector<std::string> CHTLGlobalMap::GetNamespaceStack() const {
    return m_NamespaceStack;
}

void CHTLGlobalMap::RegisterImport(const std::string& path, const std::string& type) {
    m_Imports[path] = type;
    utils::Logger::GetInstance().Debug("注册导入: " + path + " (" + type + ")");
}

bool CHTLGlobalMap::IsImported(const std::string& path) const {
    return m_Imports.find(path) != m_Imports.end();
}

void CHTLGlobalMap::AddImportAlias(const std::string& original, const std::string& alias) {
    m_Aliases[alias] = original;
    utils::Logger::GetInstance().Debug("添加导入别名: " + alias + " -> " + original);
}

std::optional<std::string> CHTLGlobalMap::ResolveAlias(const std::string& alias) const {
    auto it = m_Aliases.find(alias);
    if (it != m_Aliases.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool CHTLGlobalMap::IsOfficialModule(const std::string& name) const {
    return name.find("chtl::") == 0;
}

std::string CHTLGlobalMap::ResolveModulePath(const std::string& name) const {
    if (IsOfficialModule(name)) {
        // 官方模块路径解析
        std::string moduleName = name.substr(6);  // 去掉 "chtl::"
        return "module/" + moduleName;
    }
    return name;
}

void CHTLGlobalMap::Clear() {
    m_TemplateStyles.clear();
    m_TemplateElements.clear();
    m_TemplateVars.clear();
    m_CustomStyles.clear();
    m_CustomElements.clear();
    m_CustomVars.clear();
    m_OriginBlocks.clear();
    m_Configuration.reset();
    m_NamedConfigurations.clear();
    m_NamespaceStack.clear();
    m_NamespaceStack.push_back("");  // 重新初始化根命名空间
    m_Imports.clear();
    m_Aliases.clear();
}

void CHTLGlobalMap::DumpTemplates() const {
    std::cout << "=== 模板 ===" << std::endl;
    std::cout << "模板样式:" << std::endl;
    for (const auto& pair : m_TemplateStyles) {
        std::cout << "  " << pair.first << std::endl;
    }
    std::cout << "模板元素:" << std::endl;
    for (const auto& pair : m_TemplateElements) {
        std::cout << "  " << pair.first << std::endl;
    }
    std::cout << "模板变量组:" << std::endl;
    for (const auto& pair : m_TemplateVars) {
        std::cout << "  " << pair.first << std::endl;
    }
}

void CHTLGlobalMap::DumpCustoms() const {
    std::cout << "=== 自定义 ===" << std::endl;
    std::cout << "自定义样式:" << std::endl;
    for (const auto& pair : m_CustomStyles) {
        std::cout << "  " << pair.first << std::endl;
    }
    std::cout << "自定义元素:" << std::endl;
    for (const auto& pair : m_CustomElements) {
        std::cout << "  " << pair.first << std::endl;
    }
    std::cout << "自定义变量组:" << std::endl;
    for (const auto& pair : m_CustomVars) {
        std::cout << "  " << pair.first << std::endl;
    }
}

void CHTLGlobalMap::DumpImports() const {
    std::cout << "=== 导入 ===" << std::endl;
    for (const auto& pair : m_Imports) {
        std::cout << "  " << pair.first << " (" << pair.second << ")" << std::endl;
    }
    std::cout << "=== 别名 ===" << std::endl;
    for (const auto& pair : m_Aliases) {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
    }
}

std::string CHTLGlobalMap::GetFullyQualifiedName(const std::string& name) const {
    std::string currentNamespace = GetCurrentNamespace();
    if (currentNamespace.empty() || name.find('.') != std::string::npos) {
        // 已经是完全限定名或在全局命名空间
        return name;
    }
    return currentNamespace + "." + name;
}

} // namespace compiler
} // namespace chtl