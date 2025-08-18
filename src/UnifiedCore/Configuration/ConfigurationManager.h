#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace chtl {
namespace configuration {

// 配置项结构
struct ConfigurationItem {
    std::string name;
    std::string value;
    std::vector<std::string> values; // 对于数组类型的配置
};

// 配置节结构
struct ConfigurationSection {
    std::string name;
    std::unordered_map<std::string, ConfigurationItem> items;
};

// 配置组结构
struct ConfigurationGroup {
    std::string name;           // 配置组名称，空字符串表示无名配置组
    bool isActive;              // 是否激活（无名配置组默认激活）
    std::unordered_map<std::string, ConfigurationSection> sections;
    
    // 基础配置项
    int indexInitialCount = 0;
    bool disableNameGroup = true;
    bool disableCustomOriginType = false;
    bool debugMode = false;
};

// 配置管理器
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // 添加配置组
    void addConfigurationGroup(const std::string& name, const ConfigurationGroup& group);
    
    // 获取配置组
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name = "");
    
    // 获取活动配置组（无名或第一个有名）
    std::shared_ptr<ConfigurationGroup> getActiveConfigurationGroup();
    
    // 设置活动配置组
    void setActiveConfigurationGroup(const std::string& name);
    
    // 检查配置组冲突
    bool hasConflict() const;
    
    // 获取配置项值
    std::string getConfigValue(const std::string& section, const std::string& key, const std::string& defaultValue = "");
    std::vector<std::string> getConfigValues(const std::string& section, const std::string& key);
    
    // 解析配置组
    bool parseConfigurationGroup(const std::string& content, const std::string& groupName = "");
    
    // 导入配置组
    bool importConfigurationGroup(const std::string& filePath, const std::string& groupName = "");
    
    // 获取所有配置组名称
    std::vector<std::string> getAllConfigurationGroupNames() const;
    
    // 重置为默认配置
    void resetToDefault();

private:
    std::unordered_map<std::string, std::shared_ptr<ConfigurationGroup>> configGroups_;
    std::string activeGroupName_;
    
    // 创建默认配置组
    std::shared_ptr<ConfigurationGroup> createDefaultConfiguration();
    
    // 解析配置节
    bool parseConfigurationSection(const std::string& content, ConfigurationSection& section);
    
    // 验证配置组
    bool validateConfigurationGroup(const ConfigurationGroup& group);
};

// 全局配置管理器实例
extern std::shared_ptr<ConfigurationManager> g_configManager;

// 配置组工厂
class ConfigurationFactory {
public:
    // 创建Basic配置组
    static std::shared_ptr<ConfigurationGroup> createBasicConfiguration();
    
    // 创建Std配置组
    static std::shared_ptr<ConfigurationGroup> createStdConfiguration();
    
    // 创建自定义配置组
    static std::shared_ptr<ConfigurationGroup> createCustomConfiguration(const std::string& name);
};

} // namespace configuration
} // namespace chtl