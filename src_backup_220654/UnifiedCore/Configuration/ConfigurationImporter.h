#pragma once
#include "ConfigurationManager.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace configuration {

// 配置导入语句结构
struct ConfigurationImportStatement {
    std::string configName;        // 配置组名称
    std::string filePath;          // 文件路径
    std::string alias;             // 别名（对于配置导入无效）
    bool isFullName = false;       // 是否使用全缀名
    bool isWildcard = false;       // 是否使用通配符
    std::string wildcardPattern;   // 通配符模式
    int lineNumber = 0;
    int columnNumber = 0;
};

// 配置导入器
class ConfigurationImporter {
public:
    ConfigurationImporter();
    ~ConfigurationImporter();
    
    // 解析配置导入语句
    bool parseImportStatement(const std::string& statement, ConfigurationImportStatement& importStmt);
    
    // 执行配置导入
    bool executeImport(const ConfigurationImportStatement& importStmt);
    
    // 导入配置组
    bool importConfiguration(const std::string& filePath, const std::string& configName = "");
    
    // 导入所有模板（支持通配符）
    bool importAllTemplates(const std::string& filePathOrPattern, const std::string& alias = "");
    
    // 导入所有自定义（支持通配符）
    bool importAllCustom(const std::string& filePathOrPattern, const std::string& alias = "");
    
    // 导入所有原始嵌入（支持通配符）
    bool importAllOrigin(const std::string& filePathOrPattern, const std::string& alias = "");
    
    // 导入特定类型的所有项（支持通配符）
    bool importAllCustomElements(const std::string& filePathOrPattern, const std::string& alias = "");
    bool importAllCustomStyles(const std::string& filePathOrPattern, const std::string& alias = "");
    bool importAllCustomVars(const std::string& filePathOrPattern, const std::string& alias = "");
    bool importAllTemplateElements(const std::string& filePathOrPattern, const std::string& alias = "");
    bool importAllTemplateStyles(const std::string& filePathOrPattern, const std::string& alias = "");
    bool importAllTemplateVars(const std::string& filePathOrPattern, const std::string& alias = "");
    
    // 导入原始嵌入项（支持通配符）
    bool importOriginHtml(const std::string& name, const std::string& filePathOrPattern, const std::string& alias = "");
    bool importOriginStyle(const std::string& name, const std::string& filePathOrPattern, const std::string& alias = "");
    bool importOriginJavaScript(const std::string& name, const std::string& filePathOrPattern, const std::string& alias = "");
    
    // 通配符处理
    std::vector<std::string> expandWildcard(const std::string& pattern);
    bool isWildcardPattern(const std::string& pattern);
    bool matchesPattern(const std::string& filename, const std::string& pattern);
    
    // 验证导入路径
    bool validateImportPath(const std::string& filePath);
    
    // 获取导入错误信息
    std::string getLastError() const { return lastError_; }

private:
    std::string lastError_;
    std::shared_ptr<ConfigurationManager> configManager_;
    
    // 解析文件路径
    std::string resolveFilePath(const std::string& path);
    
    // 读取文件内容
    std::string readFileContent(const std::string& filePath);
    
    // 解析导入语句的正则表达式匹配
    bool matchImportPattern(const std::string& statement, const std::string& pattern, 
                           std::vector<std::string>& matches);
};

// 全局配置导入器实例
extern std::shared_ptr<ConfigurationImporter> g_configImporter;

} // namespace configuration
} // namespace chtl