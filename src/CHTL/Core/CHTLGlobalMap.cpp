#include "CHTL/Core/CHTLGlobalMap.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include "Utils/FileUtils.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Core {

CHTLGlobalMap::CHTLGlobalMap() {
    // 初始化默认命名空间
    currentNamespace_ = "";
    currentConfiguration_ = "";
}

bool CHTLGlobalMap::AddSymbol(const SymbolInfo& symbol) {
    if (!IsValidSymbolName(symbol.name)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的符号名称: " + symbol.name,
            symbol.fileName, symbol.line, symbol.column
        );
        return false;
    }
    
    // 生成完整键名
    std::string key = GenerateKey(symbol.name, symbol.namespaceName);
    
    // 检查是否存在冲突
    if (HasConflict(symbol.name, symbol.type, symbol.namespaceName)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "符号名称冲突: " + symbol.name + " (类型: " + GetSymbolTypeName(symbol.type) + ")",
            symbol.fileName, symbol.line, symbol.column
        );
        return false;
    }
    
    // 创建符号副本并设置完整信息
    SymbolInfo newSymbol = symbol;
    newSymbol.fullName = key;
    if (newSymbol.namespaceName.empty()) {
        newSymbol.namespaceName = currentNamespace_;
    }
    
    // 添加符号
    symbols_[key] = newSymbol;
    
    // 将符号添加到命名空间
    if (!newSymbol.namespaceName.empty()) {
        auto nsIt = namespaces_.find(newSymbol.namespaceName);
        if (nsIt != namespaces_.end()) {
            nsIt->second.symbols.insert(key);
        }
    }
    
    return true;
}

const SymbolInfo* CHTLGlobalMap::FindSymbol(const std::string& name, 
                                            const std::string& namespaceName) const {
    std::string key = GenerateKey(name, namespaceName);
    auto it = symbols_.find(key);
    if (it != symbols_.end()) {
        return &it->second;
    }
    
    // 如果未指定命名空间，尝试在当前命名空间中查找
    if (namespaceName.empty() && !currentNamespace_.empty()) {
        key = GenerateKey(name, currentNamespace_);
        it = symbols_.find(key);
        if (it != symbols_.end()) {
            return &it->second;
        }
    }
    
    // 最后尝试在全局命名空间查找
    if (!namespaceName.empty() || !currentNamespace_.empty()) {
        key = GenerateKey(name, "");
        it = symbols_.find(key);
        if (it != symbols_.end()) {
            return &it->second;
        }
    }
    
    return nullptr;
}

const SymbolInfo* CHTLGlobalMap::FindSymbolByType(const std::string& name, 
                                                  SymbolType type,
                                                  const std::string& namespaceName) const {
    const SymbolInfo* symbol = FindSymbol(name, namespaceName);
    if (symbol && (symbol->type == type || IsCompatibleType(symbol->type, type))) {
        return symbol;
    }
    return nullptr;
}

bool CHTLGlobalMap::AddNamespace(const NamespaceInfo& namespaceInfo) {
    if (!IsValidNamespaceName(namespaceInfo.name)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的命名空间名称: " + namespaceInfo.name,
            namespaceInfo.fileName, namespaceInfo.line
        );
        return false;
    }
    
    // 检查命名空间是否已存在
    if (namespaces_.find(namespaceInfo.name) != namespaces_.end()) {
        // 命名空间合并：CHTL支持同名命名空间合并
        auto& existingNs = namespaces_[namespaceInfo.name];
        // 合并子命名空间
        for (const auto& child : namespaceInfo.children) {
            if (std::find(existingNs.children.begin(), existingNs.children.end(), child) 
                == existingNs.children.end()) {
                existingNs.children.push_back(child);
            }
        }
        return true;
    }
    
    namespaces_[namespaceInfo.name] = namespaceInfo;
    return true;
}

const NamespaceInfo* CHTLGlobalMap::FindNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? &it->second : nullptr;
}

void CHTLGlobalMap::SetCurrentNamespace(const std::string& namespaceName) {
    currentNamespace_ = namespaceName;
}

void CHTLGlobalMap::EnterNamespace(const std::string& namespaceName) {
    namespaceStack_.push_back(currentNamespace_);
    currentNamespace_ = namespaceName;
}

void CHTLGlobalMap::ExitNamespace() {
    if (!namespaceStack_.empty()) {
        currentNamespace_ = namespaceStack_.back();
        namespaceStack_.pop_back();
    } else {
        currentNamespace_ = "";
    }
}

bool CHTLGlobalMap::AddConfiguration(const ConfigurationInfo& config) {
    if (config.name.empty()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "配置名称不能为空",
            config.fileName, config.line
        );
        return false;
    }
    
    configurations_[config.name] = config;
    return true;
}

const ConfigurationInfo* CHTLGlobalMap::FindConfiguration(const std::string& name) const {
    auto it = configurations_.find(name);
    return (it != configurations_.end()) ? &it->second : nullptr;
}

const ConfigurationInfo* CHTLGlobalMap::GetCurrentConfiguration() const {
    if (currentConfiguration_.empty()) {
        return nullptr;
    }
    return FindConfiguration(currentConfiguration_);
}

void CHTLGlobalMap::SetCurrentConfiguration(const std::string& configName) {
    currentConfiguration_ = configName;
}

bool CHTLGlobalMap::HasSymbol(const std::string& name, const std::string& namespaceName) const {
    return FindSymbol(name, namespaceName) != nullptr;
}

bool CHTLGlobalMap::HasConflict(const std::string& name, SymbolType type, 
                               const std::string& namespaceName) const {
    const SymbolInfo* existing = FindSymbol(name, namespaceName);
    if (!existing) {
        return false;
    }
    
    // 检查类型兼容性
    if (IsCompatibleType(existing->type, type)) {
        return false;  // 兼容类型不算冲突
    }
    
    return true;  // 存在不兼容的符号
}

bool CHTLGlobalMap::ParseFullName(const std::string& fullName, 
                                 std::string& outNamespace, 
                                 std::string& outSymbolName) {
    size_t lastDot = fullName.find_last_of('.');
    if (lastDot == std::string::npos) {
        outNamespace = "";
        outSymbolName = fullName;
        return true;
    }
    
    outNamespace = fullName.substr(0, lastDot);
    outSymbolName = fullName.substr(lastDot + 1);
    return true;
}

std::string CHTLGlobalMap::BuildFullName(const std::string& namespaceName, 
                                        const std::string& symbolName) {
    if (namespaceName.empty()) {
        return symbolName;
    }
    return namespaceName + "." + symbolName;
}

std::string CHTLGlobalMap::GetSymbolTypeName(SymbolType type) {
    switch (type) {
        case SymbolType::TEMPLATE_STYLE: return "Template Style";
        case SymbolType::TEMPLATE_ELEMENT: return "Template Element";
        case SymbolType::TEMPLATE_VAR: return "Template Variable";
        case SymbolType::CUSTOM_STYLE: return "Custom Style";
        case SymbolType::CUSTOM_ELEMENT: return "Custom Element";
        case SymbolType::CUSTOM_VAR: return "Custom Variable";
        case SymbolType::ORIGIN_HTML: return "Origin HTML";
        case SymbolType::ORIGIN_STYLE: return "Origin Style";
        case SymbolType::ORIGIN_JAVASCRIPT: return "Origin JavaScript";
        case SymbolType::ORIGIN_CUSTOM: return "Origin Custom";
        case SymbolType::NAMESPACE: return "Namespace";
        case SymbolType::CONFIGURATION: return "Configuration";
        case SymbolType::IMPORT: return "Import";
        case SymbolType::HTML_ELEMENT: return "HTML Element";
        case SymbolType::VARIABLE: return "Variable";
        case SymbolType::FUNCTION: return "Function";
        case SymbolType::UNKNOWN: return "Unknown";
        default: return "Unknown Type";
    }
}

bool CHTLGlobalMap::IsCompatibleType(SymbolType type1, SymbolType type2) {
    // 相同类型总是兼容的
    if (type1 == type2) {
        return true;
    }
    
    // 模板和自定义类型之间可以相互继承
    if ((type1 == SymbolType::TEMPLATE_STYLE && type2 == SymbolType::CUSTOM_STYLE) ||
        (type1 == SymbolType::CUSTOM_STYLE && type2 == SymbolType::TEMPLATE_STYLE) ||
        (type1 == SymbolType::TEMPLATE_ELEMENT && type2 == SymbolType::CUSTOM_ELEMENT) ||
        (type1 == SymbolType::CUSTOM_ELEMENT && type2 == SymbolType::TEMPLATE_ELEMENT) ||
        (type1 == SymbolType::TEMPLATE_VAR && type2 == SymbolType::CUSTOM_VAR) ||
        (type1 == SymbolType::CUSTOM_VAR && type2 == SymbolType::TEMPLATE_VAR)) {
        return true;
    }
    
    return false;
}

void CHTLGlobalMap::Clear() {
    symbols_.clear();
    namespaces_.clear();
    configurations_.clear();
    namespaceStack_.clear();
    currentNamespace_ = "";
    currentConfiguration_ = "";
}

bool CHTLGlobalMap::ExportToFile(const std::string& fileName) const {
    std::ostringstream oss;
    
    // 导出配置
    oss << "# CHTL符号导出文件\n";
    oss << "# 配置信息\n";
    for (const auto& pair : configurations_) {
        const auto& config = pair.second;
        oss << "[Configuration] " << config.name << "\n";
        for (const auto& setting : config.settings) {
            oss << "  " << setting.first << " = " << setting.second << "\n";
        }
        oss << "\n";
    }
    
    // 导出命名空间
    oss << "# 命名空间信息\n";
    for (const auto& pair : namespaces_) {
        const auto& ns = pair.second;
        oss << "[Namespace] " << ns.name << "\n";
        if (!ns.parentNamespace.empty()) {
            oss << "  parent = " << ns.parentNamespace << "\n";
        }
        for (const auto& child : ns.children) {
            oss << "  child = " << child << "\n";
        }
        oss << "\n";
    }
    
    // 导出符号
    oss << "# 符号信息\n";
    for (const auto& pair : symbols_) {
        const auto& symbol = pair.second;
        oss << "[Symbol] " << symbol.name << "\n";
        oss << "  type = " << GetSymbolTypeName(symbol.type) << "\n";
        oss << "  namespace = " << symbol.namespaceName << "\n";
        oss << "  file = " << symbol.fileName << "\n";
        oss << "  line = " << symbol.line << "\n";
        oss << "  exported = " << (symbol.isExported ? "true" : "false") << "\n";
        
        for (const auto& inherit : symbol.inherits) {
            oss << "  inherit = " << inherit << "\n";
        }
        
        for (const auto& prop : symbol.properties) {
            oss << "  property = " << prop.first << " : " << prop.second << "\n";
        }
        
        oss << "\n";
    }
    
    return Utils::FileUtils::WriteFile(fileName, oss.str());
}

bool CHTLGlobalMap::ImportFromFile(const std::string& fileName) {
    std::string content = Utils::FileUtils::ReadFile(fileName);
    if (content.empty()) {
        return false;
    }
    
    // 简单的解析实现（可以根据需要扩展）
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = Utils::StringUtils::Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;  // 跳过空行和注释
        }
        
        // 解析符号定义（简化实现）
        if (Utils::StringUtils::StartsWith(line, "[Symbol]")) {
            // 实际项目中需要更完整的解析逻辑
            // 这里只是示例
        }
    }
    
    return true;
}

std::string CHTLGlobalMap::GetStatistics() const {
    std::ostringstream oss;
    
    // 统计各类型符号数量
    std::unordered_map<SymbolType, int> typeCounts;
    for (const auto& pair : symbols_) {
        typeCounts[pair.second.type]++;
    }
    
    oss << "CHTL全局符号统计:\n";
    oss << "  总符号数: " << symbols_.size() << "\n";
    oss << "  命名空间数: " << namespaces_.size() << "\n";
    oss << "  配置数: " << configurations_.size() << "\n";
    oss << "\n符号类型分布:\n";
    
    for (const auto& pair : typeCounts) {
        oss << "  " << GetSymbolTypeName(pair.first) << ": " << pair.second << "\n";
    }
    
    if (!currentNamespace_.empty()) {
        oss << "\n当前命名空间: " << currentNamespace_ << "\n";
    }
    
    if (!currentConfiguration_.empty()) {
        oss << "当前配置: " << currentConfiguration_ << "\n";
    }
    
    return oss.str();
}

std::string CHTLGlobalMap::GenerateKey(const std::string& name, 
                                      const std::string& namespaceName) const {
    return BuildFullName(namespaceName, name);
}

bool CHTLGlobalMap::IsValidSymbolName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // CHTL符号名称规则：
    // 1. 不能以数字开头
    // 2. 只能包含字母、数字、下划线
    // 3. 长度不能超过256字符
    
    if (name.length() > 256) {
        return false;
    }
    
    if (std::isdigit(name[0])) {
        return false;
    }
    
    for (char ch : name) {
        if (!std::isalnum(ch) && ch != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLGlobalMap::IsValidNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return true;  // 空命名空间名称是有效的（表示全局命名空间）
    }
    
    // 命名空间可以包含点号分隔符
    std::vector<std::string> parts = Utils::StringUtils::Split(name, ".");
    for (const auto& part : parts) {
        if (!IsValidSymbolName(part)) {
            return false;
        }
    }
    
    return true;
}

} // namespace Core
} // namespace CHTL
