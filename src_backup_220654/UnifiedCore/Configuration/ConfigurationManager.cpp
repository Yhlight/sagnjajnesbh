#include "ConfigurationManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace configuration {

// 全局配置管理器实例
std::shared_ptr<ConfigurationManager> g_configManager = std::make_shared<ConfigurationManager>();

ConfigurationManager::ConfigurationManager() {
    resetToDefault();
}

ConfigurationManager::~ConfigurationManager() = default;

void ConfigurationManager::addConfigurationGroup(const std::string& name, const ConfigurationGroup& group) {
    auto groupPtr = std::make_shared<ConfigurationGroup>(group);
    groupPtr->name = name;
    groupPtr->isActive = name.empty(); // 无名配置组默认激活
    
    configGroups_[name] = groupPtr;
    
    // 如果是无名配置组，设置为活动配置组
    if (name.empty()) {
        activeGroupName_ = name;
    }
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getConfigurationGroup(const std::string& name) {
    auto it = configGroups_.find(name);
    return (it != configGroups_.end()) ? it->second : nullptr;
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getActiveConfigurationGroup() {
    // 首先尝试获取无名配置组
    auto unnamedGroup = getConfigurationGroup("");
    if (unnamedGroup) {
        return unnamedGroup;
    }
    
    // 如果没有无名配置组，返回第一个有名配置组
    if (!configGroups_.empty()) {
        return configGroups_.begin()->second;
    }
    
    // 如果没有任何配置组，返回默认配置
    return createDefaultConfiguration();
}

void ConfigurationManager::setActiveConfigurationGroup(const std::string& name) {
    auto group = getConfigurationGroup(name);
    if (group) {
        activeGroupName_ = name;
        group->isActive = true;
        
        // 取消其他配置组的激活状态
        for (auto& pair : configGroups_) {
            if (pair.first != name) {
                pair.second->isActive = false;
            }
        }
    }
}

bool ConfigurationManager::hasConflict() const {
    int unnamedGroupCount = 0;
    for (const auto& pair : configGroups_) {
        if (pair.first.empty()) {
            unnamedGroupCount++;
        }
    }
    return unnamedGroupCount > 1;
}

std::string ConfigurationManager::getConfigValue(const std::string& section, const std::string& key, const std::string& defaultValue) {
    auto activeGroup = getActiveConfigurationGroup();
    if (!activeGroup) {
        return defaultValue;
    }
    
    auto sectionIt = activeGroup->sections.find(section);
    if (sectionIt == activeGroup->sections.end()) {
        return defaultValue;
    }
    
    auto itemIt = sectionIt->second.items.find(key);
    if (itemIt == sectionIt->second.items.end()) {
        return defaultValue;
    }
    
    return itemIt->second.value;
}

std::vector<std::string> ConfigurationManager::getConfigValues(const std::string& section, const std::string& key) {
    auto activeGroup = getActiveConfigurationGroup();
    if (!activeGroup) {
        return {};
    }
    
    auto sectionIt = activeGroup->sections.find(section);
    if (sectionIt == activeGroup->sections.end()) {
        return {};
    }
    
    auto itemIt = sectionIt->second.items.find(key);
    if (itemIt == sectionIt->second.items.end()) {
        return {};
    }
    
    return itemIt->second.values;
}

bool ConfigurationManager::parseConfigurationGroup(const std::string& content, const std::string& groupName) {
    // 简化的配置解析逻辑
    ConfigurationGroup group;
    group.name = groupName;
    
    // 解析基础配置项
    std::regex configRegex(R"((\w+)\s*=\s*([^;]+);)");
    std::sregex_iterator iter(content.begin(), content.end(), configRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = iter->str(1);
        std::string value = iter->str(2);
        
        // 处理基础配置项
        if (key == "INDEX_INITIAL_COUNT") {
            group.indexInitialCount = std::stoi(value);
        } else if (key == "DISABLE_NAME_GROUP") {
            group.disableNameGroup = (value == "true");
        } else if (key == "DISABLE_CUSTOM_ORIGIN_TYPE") {
            group.disableCustomOriginType = (value == "true");
        } else if (key == "DEBUG_MODE") {
            group.debugMode = (value == "true");
        }
    }
    
    // 解析配置节
    std::regex sectionRegex(R"(\[(\w+)\]\s*\{([^}]+)\})");
    std::sregex_iterator sectionIter(content.begin(), content.end(), sectionRegex);
    
    for (; sectionIter != end; ++sectionIter) {
        std::string sectionName = sectionIter->str(1);
        std::string sectionContent = sectionIter->str(2);
        
        ConfigurationSection section;
        section.name = sectionName;
        
        if (parseConfigurationSection(sectionContent, section)) {
            group.sections[sectionName] = section;
        }
    }
    
    addConfigurationGroup(groupName, group);
    return true;
}

bool ConfigurationManager::parseConfigurationSection(const std::string& content, ConfigurationSection& section) {
    std::regex itemRegex(R"((\w+)\s*=\s*([^;]+);)");
    std::sregex_iterator iter(content.begin(), content.end(), itemRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = iter->str(1);
        std::string value = iter->str(2);
        
        ConfigurationItem item;
        item.name = key;
        
        // 检查是否为数组类型
        if (value.front() == '[' && value.back() == ']') {
            // 解析数组
            std::string arrayContent = value.substr(1, value.length() - 2);
            std::regex arrayItemRegex(R"(@?\w+)");
            std::sregex_iterator arrayIter(arrayContent.begin(), arrayContent.end(), arrayItemRegex);
            
            for (; arrayIter != end; ++arrayIter) {
                item.values.push_back(arrayIter->str());
            }
        } else {
            item.value = value;
        }
        
        section.items[key] = item;
    }
    
    return true;
}

bool ConfigurationManager::importConfigurationGroup(const std::string& filePath, const std::string& groupName) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件: " << filePath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parseConfigurationGroup(content, groupName);
}

std::vector<std::string> ConfigurationManager::getAllConfigurationGroupNames() const {
    std::vector<std::string> names;
    for (const auto& pair : configGroups_) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigurationManager::resetToDefault() {
    configGroups_.clear();
    
    // 创建默认的无名配置组
    auto defaultGroup = createDefaultConfiguration();
    addConfigurationGroup("", *defaultGroup);
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::createDefaultConfiguration() {
    return ConfigurationFactory::createBasicConfiguration();
}

bool ConfigurationManager::validateConfigurationGroup(const ConfigurationGroup& group) {
    // 验证配置组的完整性
    auto nameSection = group.sections.find("Name");
    if (nameSection == group.sections.end()) {
        return false;
    }
    
    // 检查必要的配置项
    const std::vector<std::string> requiredItems = {
        "CUSTOM_ELEMENT", "TEMPLATE_ELEMENT", "ORIGIN_HTML",
        "IMPORT_CHTL", "KEYWORD_CUSTOM", "KEYWORD_TEMPLATE"
    };
    
    for (const auto& item : requiredItems) {
        if (nameSection->second.items.find(item) == nameSection->second.items.end()) {
            return false;
        }
    }
    
    return true;
}

// ConfigurationFactory 实现
std::shared_ptr<ConfigurationGroup> ConfigurationFactory::createBasicConfiguration() {
    auto group = std::make_shared<ConfigurationGroup>();
    
    // 基础配置
    group->indexInitialCount = 0;
    group->disableNameGroup = true;
    group->disableCustomOriginType = false;
    group->debugMode = false;
    
    // Name 配置节
    ConfigurationSection nameSection;
    nameSection.name = "Name";
    
    // 添加所有配置项
    std::unordered_map<std::string, std::string> nameItems = {
        {"CUSTOM_ELEMENT", "@Element"},
        {"CUSTOM_VAR", "@Var"},
        {"TEMPLATE_STYLE", "@Style"},
        {"TEMPLATE_ELEMENT", "@Element"},
        {"TEMPLATE_VAR", "@Var"},
        {"ORIGIN_HTML", "@Html"},
        {"ORIGIN_STYLE", "@Style"},
        {"ORIGIN_JAVASCRIPT", "@JavaScript"},
        {"CONFIGURATION_CONFIG", "@Config"},
        {"IMPORT_HTML", "@Html"},
        {"IMPORT_STYLE", "@Style"},
        {"IMPORT_JAVASCRIPT", "@JavaScript"},
        {"IMPORT_CHTL", "@Chtl"},
        {"IMPORT_CRMOD", "@CJmod"},
        {"IMPORT_CONFIG", "@Config"},
        {"KEYWORD_INHERIT", "inherit"},
        {"KEYWORD_DELETE", "delete"},
        {"KEYWORD_INSERT", "insert"},
        {"KEYWORD_AFTER", "after"},
        {"KEYWORD_BEFORE", "before"},
        {"KEYWORD_REPLACE", "replace"},
        {"KEYWORD_ATTOP", "at top"},
        {"KEYWORD_ATBOTTOM", "at bottom"},
        {"KEYWORD_FROM", "from"},
        {"KEYWORD_AS", "as"},
        {"KEYWORD_EXCEPT", "except"},
        {"KEYWORD_TEXT", "text"},
        {"KEYWORD_STYLE", "style"},
        {"KEYWORD_SCRIPT", "script"},
        {"KEYWORD_CUSTOM", "[Custom]"},
        {"KEYWORD_TEMPLATE", "[Template]"},
        {"KEYWORD_ORIGIN", "[Origin]"},
        {"KEYWORD_IMPORT", "[Import]"},
        {"KEYWORD_NAMESPACE", "[Namespace]"},
        {"OPTION_COUNT", "3"}
    };
    
    // CUSTOM_STYLE 数组配置
    ConfigurationItem customStyleItem;
    customStyleItem.name = "CUSTOM_STYLE";
    customStyleItem.values = {"@Style", "@style", "@CSS", "@Css", "@css"};
    nameSection.items["CUSTOM_STYLE"] = customStyleItem;
    
    // 添加其他配置项
    for (const auto& pair : nameItems) {
        ConfigurationItem item;
        item.name = pair.first;
        item.value = pair.second;
        nameSection.items[pair.first] = item;
    }
    
    group->sections["Name"] = nameSection;
    
    // OriginType 配置节
    ConfigurationSection originTypeSection;
    originTypeSection.name = "OriginType";
    
    ConfigurationItem vueItem;
    vueItem.name = "ORIGINTYPE_VUE";
    vueItem.value = "@Vue";
    originTypeSection.items["ORIGINTYPE_VUE"] = vueItem;
    
    group->sections["OriginType"] = originTypeSection;
    
    return group;
}

std::shared_ptr<ConfigurationGroup> ConfigurationFactory::createStdConfiguration() {
    // Std配置与Basic配置相同
    return createBasicConfiguration();
}

std::shared_ptr<ConfigurationGroup> ConfigurationFactory::createCustomConfiguration(const std::string& name) {
    auto group = createBasicConfiguration();
    group->name = name;
    return group;
}

} // namespace configuration
} // namespace chtl