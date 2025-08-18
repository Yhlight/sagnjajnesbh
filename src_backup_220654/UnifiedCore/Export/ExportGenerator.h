#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace chtl {
namespace export_gen {

// 导出项类型
enum class ExportType {
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM,  // 自定义原始类型如@Vue, @React等
    CONFIGURATION
};

// 导出项结构
struct ExportItem {
    ExportType type;
    std::string name;
    std::string fullName;      // 全缀名
    std::string description;
    int lineNumber = 0;
    int columnNumber = 0;
};

// 导出分组
struct ExportGroup {
    ExportType type;
    std::vector<ExportItem> items;
};

// 导出生成器
class ExportGenerator {
public:
    ExportGenerator();
    ~ExportGenerator();
    
    // 分析源文件并提取导出项
    bool analyzeSourceFile(const std::string& filePath);
    
    // 分析源代码内容
    bool analyzeSourceContent(const std::string& content, const std::string& filePath = "");
    
    // 添加导出项
    void addExportItem(const ExportItem& item);
    
    // 生成[Export]节内容
    std::string generateExportSection();
    
    // 生成完整的Info文件
    std::string generateInfoFile(const std::string& name, const std::string& version,
                                const std::string& description, const std::string& author,
                                const std::string& license = "MIT",
                                const std::string& category = "module");
    
    // 更新现有Info文件的[Export]节
    bool updateInfoFileExports(const std::string& infoFilePath);
    
    // 清除所有导出项
    void clearExports();
    
    // 获取导出项统计
    std::unordered_map<ExportType, int> getExportStatistics();
    
    // 获取所有导出项
    std::vector<ExportItem> getAllExportItems() const;
    
    // 按类型获取导出项
    std::vector<ExportItem> getExportItemsByType(ExportType type) const;

private:
    std::vector<ExportItem> exportItems_;
    std::unordered_set<std::string> processedNames_; // 防重复
    
    // 解析自定义元素
    void parseCustomElements(const std::string& content);
    
    // 解析自定义样式组
    void parseCustomStyles(const std::string& content);
    
    // 解析自定义变量组
    void parseCustomVars(const std::string& content);
    
    // 解析模板元素
    void parseTemplateElements(const std::string& content);
    
    // 解析模板样式组
    void parseTemplateStyles(const std::string& content);
    
    // 解析模板变量组
    void parseTemplateVars(const std::string& content);
    
    // 解析原始嵌入
    void parseOriginItems(const std::string& content);
    
    // 解析配置组
    void parseConfigurations(const std::string& content);
    
    // 格式化导出类型为字符串
    std::string formatExportType(ExportType type);
    
    // 格式化导出项为字符串
    std::string formatExportItem(const ExportItem& item);
    
    // 按类型分组导出项
    std::vector<ExportGroup> groupExportsByType();
    
    // 验证导出项名称
    bool isValidExportName(const std::string& name);
    
    // 生成导出项的全缀名
    std::string generateFullName(ExportType type, const std::string& name);
};

// 全局导出生成器实例
extern std::shared_ptr<ExportGenerator> g_exportGenerator;

// 导出工具函数
namespace utils {
    // 从文件路径提取模块名
    std::string extractModuleName(const std::string& filePath);
    
    // 检查是否为Info文件
    bool isInfoFile(const std::string& filePath);
    
    // 检查是否为源文件
    bool isSourceFile(const std::string& filePath);
    
    // 批量处理模块文件
    bool processModuleDirectory(const std::string& moduleDir);
    
    // 更新所有Info文件的导出
    bool updateAllInfoFiles(const std::string& rootDir);
}

} // namespace export_gen
} // namespace chtl