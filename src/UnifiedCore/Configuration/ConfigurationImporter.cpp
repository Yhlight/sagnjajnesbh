#include "ConfigurationImporter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <algorithm>

namespace chtl {
namespace configuration {

// 全局配置导入器实例
std::shared_ptr<ConfigurationImporter> g_configImporter = std::make_shared<ConfigurationImporter>();

ConfigurationImporter::ConfigurationImporter() 
    : configManager_(g_configManager) {
}

ConfigurationImporter::~ConfigurationImporter() = default;

bool ConfigurationImporter::parseImportStatement(const std::string& statement, ConfigurationImportStatement& importStmt) {
    // 解析各种导入语句格式
    
    // [Import] @Config 导入的配置组名称 from chtl文件路径 as 命名为
    std::regex configImportRegex(R"(\[Import\]\s*@Config\s*(\w*)\s*from\s+([\w\.\*/\\-]+)\s*(?:as\s+(\w+))?)", std::regex_constants::icase);
    
    // [Import] [Configuration] @Config 导入的配置组名称 from chtl文件路径 as 命名为
    std::regex fullConfigImportRegex(R"(\[Import\]\s*\[Configuration\]\s*@Config\s*(\w*)\s*from\s+([\w\.\*/\\-]+)\s*(?:as\s+(\w+))?)", std::regex_constants::icase);
    
    // [Import] [Template] from chtl文件路径
    std::regex allTemplateRegex(R"(\[Import\]\s*\[Template\]\s*from\s+([\w\.\*/\\-]+))", std::regex_constants::icase);
    
    // [Import] [Custom] from chtl文件路径
    std::regex allCustomRegex(R"(\[Import\]\s*\[Custom\]\s*from\s+([\w\.\*/\\-]+))", std::regex_constants::icase);
    
    // [Import] [Origin] from chtl文件路径
    std::regex allOriginRegex(R"(\[Import\]\s*\[Origin\]\s*from\s+([\w\.\*/\\-]+))", std::regex_constants::icase);
    
    std::smatch match;
    
    // 检查配置导入
    if (std::regex_search(statement, match, configImportRegex) || 
        std::regex_search(statement, match, fullConfigImportRegex)) {
        importStmt.configName = match[1].str();
        importStmt.filePath = match[2].str();
        importStmt.alias = match[3].str();
        importStmt.isFullName = (statement.find("[Configuration]") != std::string::npos);
        importStmt.isWildcard = isWildcardPattern(importStmt.filePath);
        if (importStmt.isWildcard) {
            importStmt.wildcardPattern = importStmt.filePath;
        }
        return true;
    }
    
    // 检查所有模板导入
    if (std::regex_search(statement, match, allTemplateRegex)) {
        importStmt.filePath = match[1].str();
        importStmt.isWildcard = isWildcardPattern(importStmt.filePath);
        if (importStmt.isWildcard) {
            importStmt.wildcardPattern = importStmt.filePath;
        }
        return true;
    }
    
    // 检查所有自定义导入
    if (std::regex_search(statement, match, allCustomRegex)) {
        importStmt.filePath = match[1].str();
        importStmt.isWildcard = isWildcardPattern(importStmt.filePath);
        if (importStmt.isWildcard) {
            importStmt.wildcardPattern = importStmt.filePath;
        }
        return true;
    }
    
    // 检查所有原始嵌入导入
    if (std::regex_search(statement, match, allOriginRegex)) {
        importStmt.filePath = match[1].str();
        importStmt.isWildcard = isWildcardPattern(importStmt.filePath);
        if (importStmt.isWildcard) {
            importStmt.wildcardPattern = importStmt.filePath;
        }
        return true;
    }
    
    lastError_ = "无法解析导入语句: " + statement;
    return false;
}

bool ConfigurationImporter::executeImport(const ConfigurationImportStatement& importStmt) {
    if (importStmt.isWildcard) {
        // 处理通配符导入
        auto matchedFiles = expandWildcard(importStmt.wildcardPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            ConfigurationImportStatement singleImport = importStmt;
            singleImport.filePath = filePath;
            singleImport.isWildcard = false;
            
            if (!executeImport(singleImport)) {
                success = false;
            }
        }
        
        return success;
    }
    
    // 处理单个文件导入
    if (!importStmt.configName.empty()) {
        return importConfiguration(importStmt.filePath, importStmt.configName);
    }
    
    // 根据语句类型执行相应的导入
    return importConfiguration(importStmt.filePath);
}

bool ConfigurationImporter::importConfiguration(const std::string& filePath, const std::string& configName) {
    if (!validateImportPath(filePath)) {
        lastError_ = "无效的导入路径: " + filePath;
        return false;
    }
    
    std::string content = readFileContent(filePath);
    if (content.empty()) {
        lastError_ = "无法读取文件内容: " + filePath;
        return false;
    }
    
    return configManager_->parseConfigurationGroup(content, configName);
}

bool ConfigurationImporter::importAllTemplates(const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importAllTemplates(filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::string content = readFileContent(filePathOrPattern);
    if (content.empty()) {
        return false;
    }
    
    // 解析文件中的所有模板
    std::regex templateRegex(R"(\[Template\]\s*@(\w+)\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), templateRegex);
    std::sregex_iterator end;
    
    bool success = true;
    for (; iter != end; ++iter) {
        std::string templateType = iter->str(1);
        std::string templateName = iter->str(2);
        
        // 这里应该实际导入模板，暂时只是记录
        std::cout << "导入模板: " << templateType << " " << templateName << " from " << filePathOrPattern << std::endl;
    }
    
    return success;
}

bool ConfigurationImporter::importAllCustom(const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importAllCustom(filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::string content = readFileContent(filePathOrPattern);
    if (content.empty()) {
        return false;
    }
    
    // 解析文件中的所有自定义
    std::regex customRegex(R"(\[Custom\]\s*@(\w+)\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), customRegex);
    std::sregex_iterator end;
    
    bool success = true;
    for (; iter != end; ++iter) {
        std::string customType = iter->str(1);
        std::string customName = iter->str(2);
        
        // 这里应该实际导入自定义，暂时只是记录
        std::cout << "导入自定义: " << customType << " " << customName << " from " << filePathOrPattern << std::endl;
    }
    
    return success;
}

bool ConfigurationImporter::importAllOrigin(const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importAllOrigin(filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::string content = readFileContent(filePathOrPattern);
    if (content.empty()) {
        return false;
    }
    
    // 解析文件中的所有命名原始嵌入
    std::regex originRegex(R"(\[Origin\]\s*@(\w+)\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), originRegex);
    std::sregex_iterator end;
    
    bool success = true;
    for (; iter != end; ++iter) {
        std::string originType = iter->str(1);
        std::string originName = iter->str(2);
        
        // 这里应该实际导入原始嵌入，暂时只是记录
        std::cout << "导入原始嵌入: " << originType << " " << originName << " from " << filePathOrPattern << std::endl;
    }
    
    return success;
}

bool ConfigurationImporter::importAllCustomElements(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllCustom(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importAllCustomStyles(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllCustom(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importAllCustomVars(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllCustom(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importAllTemplateElements(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllTemplates(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importAllTemplateStyles(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllTemplates(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importAllTemplateVars(const std::string& filePathOrPattern, const std::string& alias) {
    return importAllTemplates(filePathOrPattern, alias); // 简化实现
}

bool ConfigurationImporter::importOriginHtml(const std::string& name, const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importOriginHtml(name, filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::cout << "导入HTML原始嵌入: " << name << " from " << filePathOrPattern << std::endl;
    return true;
}

bool ConfigurationImporter::importOriginStyle(const std::string& name, const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importOriginStyle(name, filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::cout << "导入Style原始嵌入: " << name << " from " << filePathOrPattern << std::endl;
    return true;
}

bool ConfigurationImporter::importOriginJavaScript(const std::string& name, const std::string& filePathOrPattern, const std::string& alias) {
    if (isWildcardPattern(filePathOrPattern)) {
        auto matchedFiles = expandWildcard(filePathOrPattern);
        bool success = true;
        
        for (const auto& filePath : matchedFiles) {
            if (!importOriginJavaScript(name, filePath, alias)) {
                success = false;
            }
        }
        
        return success;
    }
    
    std::cout << "导入JavaScript原始嵌入: " << name << " from " << filePathOrPattern << std::endl;
    return true;
}

std::vector<std::string> ConfigurationImporter::expandWildcard(const std::string& pattern) {
    std::vector<std::string> matchedFiles;
    
    try {
        std::filesystem::path patternPath(pattern);
        std::filesystem::path directory = patternPath.parent_path();
        std::string filename = patternPath.filename().string();
        
        if (directory.empty()) {
            directory = std::filesystem::current_path();
        }
        
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string entryFilename = entry.path().filename().string();
                    if (matchesPattern(entryFilename, filename)) {
                        matchedFiles.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        lastError_ = "通配符展开失败: " + std::string(e.what());
    }
    
    return matchedFiles;
}

bool ConfigurationImporter::isWildcardPattern(const std::string& pattern) {
    return (pattern.find('*') != std::string::npos) || 
           (pattern.find('?') != std::string::npos);
}

bool ConfigurationImporter::matchesPattern(const std::string& filename, const std::string& pattern) {
    // 简化的通配符匹配实现
    // 将通配符模式转换为正则表达式
    std::string regexPattern = pattern;
    
    // 转义正则表达式特殊字符
    std::regex specialChars(R"([\.\[\]\(\)\+\^\$\|\\])");
    regexPattern = std::regex_replace(regexPattern, specialChars, R"(\$&)");
    
    // 将通配符转换为正则表达式
    std::regex wildcard(R"(\*)");
    regexPattern = std::regex_replace(regexPattern, wildcard, ".*");
    
    std::regex question(R"(\?)");
    regexPattern = std::regex_replace(regexPattern, question, ".");
    
    // 添加边界匹配
    regexPattern = "^" + regexPattern + "$";
    
    try {
        std::regex patternRegex(regexPattern, std::regex_constants::icase);
        return std::regex_match(filename, patternRegex);
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigurationImporter::validateImportPath(const std::string& filePath) {
    if (filePath.empty()) {
        return false;
    }
    
    // 检查是否为通配符模式
    if (isWildcardPattern(filePath)) {
        return true; // 通配符模式在展开时验证
    }
    
    // 检查文件是否存在
    return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
}

std::string ConfigurationImporter::resolveFilePath(const std::string& path) {
    // 处理相对路径和绝对路径
    std::filesystem::path filePath(path);
    
    if (filePath.is_relative()) {
        filePath = std::filesystem::current_path() / filePath;
    }
    
    return filePath.string();
}

std::string ConfigurationImporter::readFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        lastError_ = "无法打开文件: " + filePath;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace configuration
} // namespace chtl