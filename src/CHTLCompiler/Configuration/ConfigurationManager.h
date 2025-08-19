#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace chtl {
namespace configuration {

// 配置项
struct ConfigItem {
    std::string key;
    std::string value;
    std::string type; // INT, STRING, BOOL, ARRAY
};

// 命名配置组
struct NamedConfiguration {
    std::string name;
    std::unordered_map<std::string, ConfigItem> basicConfig;
    std::unordered_map<std::string, std::string> nameMapping; // [Name]节
    std::unordered_map<std::string, std::string> originTypes; // [OriginType]节
    bool isActive; // 是否为当前激活的配置
};

// 配置管理器
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // ========================================
    // 命名配置组管理
    // ========================================
    
    /**
     * 添加命名配置组
     */
    void addNamedConfiguration(const std::string& name, const NamedConfiguration& config);
    
    /**
     * 获取命名配置组
     */
    NamedConfiguration* getNamedConfiguration(const std::string& name);
    
    /**
     * 设置活动配置组（无名配置组或第一个有名配置组）
     */
    void setActiveConfiguration(const std::string& name = "");
    
    /**
     * 获取当前活动配置
     */
    NamedConfiguration* getActiveConfiguration();
    
    // ========================================
    // [OriginType]节管理
    // ========================================
    
    /**
     * 注册自定义原始嵌入类型
     */
    void registerOriginType(const std::string& configName, const std::string& typeName);
    
    /**
     * 检查原始类型是否已注册
     */
    bool isOriginTypeRegistered(const std::string& typeName) const;
    
    /**
     * 获取所有注册的原始类型
     */
    std::vector<std::string> getAllOriginTypes() const;
    
    // ========================================
    // [Name]节管理
    // ========================================
    
    /**
     * 设置关键字映射
     */
    void setKeywordMapping(const std::string& configName, const std::string& key, const std::string& value);
    
    /**
     * 获取关键字映射
     */
    std::string getKeywordMapping(const std::string& key) const;
    
    /**
     * 检查关键字是否有自定义映射
     */
    bool hasKeywordMapping(const std::string& key) const;
    
    // ========================================
    // 配置解析
    // ========================================
    
    /**
     * 解析配置文件
     */
    bool parseConfigurationFile(const std::string& filePath);
    
    /**
     * 解析配置块
     */
    bool parseConfigurationBlock(const std::string& content, const std::string& configName = "");
    
    /**
     * 解析[Name]节
     */
    bool parseNameSection(const std::string& content, NamedConfiguration& config);
    
    /**
     * 解析[OriginType]节
     */
    bool parseOriginTypeSection(const std::string& content, NamedConfiguration& config);
    
    // ========================================
    // 配置验证
    // ========================================
    
    /**
     * 验证配置完整性
     */
    bool validateConfiguration(const NamedConfiguration& config);
    
    /**
     * 检查配置冲突
     */
    bool checkConfigurationConflict() const;
    
    // ========================================
    // 配置导入支持
    // ========================================
    
    /**
     * 处理配置导入
     * [Import] @Config ConfigName from filePath
     */
    bool importConfiguration(const std::string& configName, const std::string& filePath, const std::string& alias = "");
    
    /**
     * 处理全缀名配置导入
     * [Import] [Configuration] @Config ConfigName from filePath
     */
    bool importConfigurationFullName(const std::string& configName, const std::string& filePath, const std::string& alias = "");
    
    // ========================================
    // 配置查询
    // ========================================
    
    /**
     * 获取配置值
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * 设置配置值
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * 获取所有配置组
     */
    std::vector<std::string> getAllConfigurationNames() const;
    
    /**
     * 清理
     */
    void clear();

private:
    std::unordered_map<std::string, NamedConfiguration> namedConfigurations_;
    NamedConfiguration* activeConfiguration_;
    std::vector<std::string> errors_;
    
    // 解析辅助函数
    std::unordered_map<std::string, std::string> parseKeyValuePairs(const std::string& content);
    std::string extractSectionContent(const std::string& content, const std::string& sectionName);
    void reportError(const std::string& message);
};

} // namespace configuration
} // namespace chtl