#include "ConfigurationManager.h"
#include <fstream>
#include <regex>
#include <iostream>

namespace chtl {
namespace configuration {

ConfigurationManager::ConfigurationManager() : activeConfiguration_(nullptr) {}

ConfigurationManager::~ConfigurationManager() = default;

void ConfigurationManager::addNamedConfiguration(const std::string& name, const NamedConfiguration& config) {
    namedConfigurations_[name] = config;
    
    // 如果没有活动配置，设置为活动配置
    if (!activeConfiguration_) {
        activeConfiguration_ = &namedConfigurations_[name];
    }
}

NamedConfiguration* ConfigurationManager::getNamedConfiguration(const std::string& name) {
    auto it = namedConfigurations_.find(name);
    return (it != namedConfigurations_.end()) ? &it->second : nullptr;
}

void ConfigurationManager::setActiveConfiguration(const std::string& name) {
    if (name.empty()) {
        // 查找无名配置组或第一个有名配置组
        for (auto& [configName, config] : namedConfigurations_) {
            if (configName.empty() || activeConfiguration_ == nullptr) {
                activeConfiguration_ = &config;
                break;
            }
        }
    } else {
        auto* config = getNamedConfiguration(name);
        if (config) {
            activeConfiguration_ = config;
        }
    }
}

NamedConfiguration* ConfigurationManager::getActiveConfiguration() {
    return activeConfiguration_;
}

void ConfigurationManager::registerOriginType(const std::string& configName, const std::string& typeName) {
    auto* config = getNamedConfiguration(configName);
    if (config) {
        config->originTypes[typeName] = typeName;
    } else if (activeConfiguration_) {
        activeConfiguration_->originTypes[typeName] = typeName;
    }
}

bool ConfigurationManager::isOriginTypeRegistered(const std::string& typeName) const {
    if (activeConfiguration_) {
        return activeConfiguration_->originTypes.find(typeName) != activeConfiguration_->originTypes.end();
    }
    return false;
}

std::vector<std::string> ConfigurationManager::getAllOriginTypes() const {
    std::vector<std::string> types;
    if (activeConfiguration_) {
        for (const auto& [key, value] : activeConfiguration_->originTypes) {
            types.push_back(key);
        }
    }
    return types;
}

void ConfigurationManager::setKeywordMapping(const std::string& configName, const std::string& key, const std::string& value) {
    auto* config = getNamedConfiguration(configName);
    if (config) {
        config->nameMapping[key] = value;
    } else if (activeConfiguration_) {
        activeConfiguration_->nameMapping[key] = value;
    }
}

std::string ConfigurationManager::getKeywordMapping(const std::string& key) const {
    if (activeConfiguration_) {
        auto it = activeConfiguration_->nameMapping.find(key);
        if (it != activeConfiguration_->nameMapping.end()) {
            return it->second;
        }
    }
    return key; // 返回原始关键字
}

bool ConfigurationManager::hasKeywordMapping(const std::string& key) const {
    if (activeConfiguration_) {
        return activeConfiguration_->nameMapping.find(key) != activeConfiguration_->nameMapping.end();
    }
    return false;
}

bool ConfigurationManager::parseConfigurationFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        reportError("Cannot open configuration file: " + filePath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 查找所有配置块
    std::regex configRegex(R"(\[Configuration\]\s*@Config\s+(\w+)\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\})");
    std::sregex_iterator iter(content.begin(), content.end(), configRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string configName = (*iter)[1].str();
        std::string configContent = (*iter)[2].str();
        
        parseConfigurationBlock(configContent, configName);
    }
    
    return true;
}

bool ConfigurationManager::parseConfigurationBlock(const std::string& content, const std::string& configName) {
    NamedConfiguration config;
    config.name = configName;
    config.isActive = configName.empty(); // 无名配置组默认激活
    
    // 解析基础配置项
    auto keyValuePairs = parseKeyValuePairs(content);
    for (const auto& [key, value] : keyValuePairs) {
        ConfigItem item;
        item.key = key;
        item.value = value;
        item.type = "STRING"; // 简化类型检测
        config.basicConfig[key] = item;
    }
    
    // 解析[Name]节
    parseNameSection(content, config);
    
    // 解析[OriginType]节
    parseOriginTypeSection(content, config);
    
    // 添加配置
    addNamedConfiguration(configName, config);
    
    return true;
}

bool ConfigurationManager::parseNameSection(const std::string& content, NamedConfiguration& config) {
    std::string nameContent = extractSectionContent(content, "Name");
    if (nameContent.empty()) {
        return true; // [Name]节是可选的
    }
    
    auto keyValuePairs = parseKeyValuePairs(nameContent);
    for (const auto& [key, value] : keyValuePairs) {
        config.nameMapping[key] = value;
    }
    
    return true;
}

bool ConfigurationManager::parseOriginTypeSection(const std::string& content, NamedConfiguration& config) {
    std::string originContent = extractSectionContent(content, "OriginType");
    if (originContent.empty()) {
        return true; // [OriginType]节是可选的
    }
    
    auto keyValuePairs = parseKeyValuePairs(originContent);
    for (const auto& [key, value] : keyValuePairs) {
        config.originTypes[key] = value;
    }
    
    return true;
}

bool ConfigurationManager::validateConfiguration(const NamedConfiguration& config) {
    // 验证必需的配置项
    const std::vector<std::string> requiredKeys = {
        "INDEX_INITIAL_COUNT",
        "DISABLE_NAME_GROUP", 
        "DISABLE_CUSTOM_ORIGIN_TYPE",
        "DEBUG_MODE"
    };
    
    for (const auto& key : requiredKeys) {
        if (config.basicConfig.find(key) == config.basicConfig.end()) {
            reportError("Missing required configuration: " + key);
            return false;
        }
    }
    
    return true;
}

bool ConfigurationManager::checkConfigurationConflict() const {
    int unnamedConfigs = 0;
    for (const auto& [name, config] : namedConfigurations_) {
        if (name.empty()) {
            unnamedConfigs++;
        }
    }
    
    if (unnamedConfigs > 1) {
        reportError("Multiple unnamed configurations found, conflict detected");
        return true;
    }
    
    return false;
}

bool ConfigurationManager::importConfiguration(const std::string& configName, const std::string& filePath, const std::string& alias) {
    // 解析并导入指定的配置组
    if (!parseConfigurationFile(filePath)) {
        return false;
    }
    
    auto* config = getNamedConfiguration(configName);
    if (!config) {
        reportError("Configuration not found: " + configName);
        return false;
    }
    
    // 如果有别名，重新注册
    if (!alias.empty()) {
        addNamedConfiguration(alias, *config);
    }
    
    return true;
}

bool ConfigurationManager::importConfigurationFullName(const std::string& configName, const std::string& filePath, const std::string& alias) {
    // 与importConfiguration相同，但使用全缀名语法
    return importConfiguration(configName, filePath, alias);
}

std::string ConfigurationManager::getConfigValue(const std::string& key) const {
    if (activeConfiguration_) {
        auto it = activeConfiguration_->basicConfig.find(key);
        if (it != activeConfiguration_->basicConfig.end()) {
            return it->second.value;
        }
    }
    return "";
}

void ConfigurationManager::setConfigValue(const std::string& key, const std::string& value) {
    if (activeConfiguration_) {
        ConfigItem item;
        item.key = key;
        item.value = value;
        item.type = "STRING";
        activeConfiguration_->basicConfig[key] = item;
    }
}

std::vector<std::string> ConfigurationManager::getAllConfigurationNames() const {
    std::vector<std::string> names;
    for (const auto& [name, config] : namedConfigurations_) {
        names.push_back(name);
    }
    return names;
}

void ConfigurationManager::clear() {
    namedConfigurations_.clear();
    activeConfiguration_ = nullptr;
    errors_.clear();
}

std::unordered_map<std::string, std::string> ConfigurationManager::parseKeyValuePairs(const std::string& content) {
    std::unordered_map<std::string, std::string> pairs;
    
    // 解析 KEY = VALUE; 格式
    std::regex pairRegex(R"((\w+)\s*=\s*([^;]+);)");
    std::sregex_iterator iter(content.begin(), content.end(), pairRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 去除引号
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        pairs[key] = value;
    }
    
    return pairs;
}

std::string ConfigurationManager::extractSectionContent(const std::string& content, const std::string& sectionName) {
    std::regex sectionRegex("\\[" + sectionName + "\\]\\s*\\{([^}]*)\\}");
    std::smatch match;
    
    if (std::regex_search(content, match, sectionRegex)) {
        return match[1].str();
    }
    
    return "";
}

void ConfigurationManager::reportError(const std::string& message) {
    errors_.push_back(message);
    std::cerr << "ConfigurationManager Error: " << message << std::endl;
}

} // namespace configuration
} // namespace chtl