#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace chtl {
namespace common {

/**
 * 模块路径管理器
 * 支持新的CMOD/CJMOD文件夹分类结构
 */
class ModulePathManager {
public:
    enum class ModuleType {
        CMOD,
        CJMOD,
        UNKNOWN
    };
    
    struct ModuleLocation {
        std::string name;
        ModuleType type;
        std::string path;
        bool isPacked;  // true: .cmod/.cjmod文件, false: 源码目录
        std::string category;  // official, user, etc.
    };
    
    ModulePathManager();
    
    /**
     * 添加模块搜索路径
     */
    void addSearchPath(const std::string& path, const std::string& category = "user");
    
    /**
     * 扫描所有搜索路径中的模块
     */
    void scanModules();
    
    /**
     * 查找指定名称的模块
     */
    ModuleLocation* findModule(const std::string& name, ModuleType type = ModuleType::UNKNOWN);
    
    /**
     * 获取所有模块
     */
    const std::vector<ModuleLocation>& getAllModules() const { return modules_; }
    
    /**
     * 按类型获取模块
     */
    std::vector<ModuleLocation> getModulesByType(ModuleType type) const;
    
    /**
     * 按分类获取模块
     */
    std::vector<ModuleLocation> getModulesByCategory(const std::string& category) const;
    
    /**
     * 获取模块的完整路径
     */
    std::string getModuleFullPath(const ModuleLocation& location) const;
    
    /**
     * 检查模块是否存在
     */
    bool moduleExists(const std::string& name, ModuleType type = ModuleType::UNKNOWN) const;
    
    /**
     * 获取支持的文件夹名称变体
     */
    static std::vector<std::string> getFolderVariants(ModuleType type);
    
    /**
     * 从文件扩展名推断模块类型
     */
    static ModuleType getTypeFromExtension(const std::string& extension);
    
    /**
     * 从文件夹名称推断模块类型
     */
    static ModuleType getTypeFromFolderName(const std::string& folderName);
    
    /**
     * 获取模块类型的字符串表示
     */
    static std::string typeToString(ModuleType type);
    
private:
    std::vector<std::string> searchPaths_;
    std::map<std::string, std::string> pathCategories_;  // path -> category
    std::vector<ModuleLocation> modules_;
    
    /**
     * 扫描指定路径中的模块
     */
    void scanPath(const std::string& basePath, const std::string& category);
    
    /**
     * 扫描指定类型的模块文件夹
     */
    void scanModuleTypeFolder(const std::string& basePath, ModuleType type, const std::string& category);
    
    /**
     * 分析模块目录或文件
     */
    ModuleLocation analyzeModuleItem(const std::string& itemPath, ModuleType type, const std::string& category);
    
    /**
     * 检查路径是否为打包的模块文件
     */
    bool isPackedModule(const std::string& path, ModuleType type) const;
    
    /**
     * 检查路径是否为模块源码目录
     */
    bool isSourceModule(const std::string& path, ModuleType type) const;
    
    /**
     * 规范化模块名称
     */
    std::string normalizeModuleName(const std::string& name) const;
};

} // namespace common
} // namespace chtl