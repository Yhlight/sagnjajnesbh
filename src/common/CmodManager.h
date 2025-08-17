#pragma once
#include "SimpleZip.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace chtl {

// Cmod信息结构
struct CmodInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string min_chtl_version;
    std::string max_chtl_version;
    
    // 导出符号表
    std::vector<std::string> exported_styles;
    std::vector<std::string> exported_elements;
    std::vector<std::string> exported_vars;
};

// Cmod文件结构
struct CmodStructure {
    std::string main_module_name;
    std::string src_dir;
    std::string info_dir;
    std::vector<std::string> source_files;
    std::vector<std::string> submodules;
    bool has_main_file;
};

// Cmod管理器
class CmodManager {
public:
    CmodManager();
    ~CmodManager();
    
    // Cmod打包功能
    bool packCmod(const std::string& source_directory, const std::string& output_cmod_path);
    bool packCmodFromStructure(const CmodStructure& structure, const std::string& base_path, 
                              const std::string& output_cmod_path);
    
    // Cmod解包功能
    bool unpackCmod(const std::string& cmod_path, const std::string& output_directory);
    
    // Cmod验证功能
    bool validateCmodStructure(const std::string& directory_path);
    bool validateCmodFile(const std::string& cmod_path);
    
    // Cmod信息解析
    bool parseCmodInfo(const std::string& info_file_content, CmodInfo& info);
    bool extractCmodInfo(const std::string& cmod_path, CmodInfo& info);
    
    // Cmod结构分析
    bool analyzeCmodStructure(const std::string& directory_path, CmodStructure& structure);
    
    // 子模块管理
    std::vector<std::string> getSubmodules(const std::string& cmod_path);
    bool hasSubmodule(const std::string& cmod_path, const std::string& submodule_name);
    
    // 文件操作
    std::vector<std::string> listCmodFiles(const std::string& cmod_path);
    bool extractCmodFile(const std::string& cmod_path, const std::string& file_path, std::string& content);
    
    // 路径解析
    std::string resolveCmodPath(const std::string& module_name, const std::vector<std::string>& search_paths);
    std::string resolveSubmodulePath(const std::string& main_module, const std::string& submodule);
    
    // 依赖管理
    std::vector<std::string> parseDependencies(const std::string& dependencies_str);
    bool checkDependencies(const CmodInfo& info, const std::vector<std::string>& available_modules);
    
    // 版本兼容性检查
    bool isVersionCompatible(const std::string& required_version, const std::string& available_version);
    bool isCHTLVersionCompatible(const std::string& min_version, const std::string& max_version, 
                                const std::string& current_version);
    
    // 错误处理
    std::string getLastError() const { return last_error_; }
    
    // 搜索路径管理
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const { return search_paths_; }
    
    // Cmod缓存管理
    void clearCache();
    bool isCached(const std::string& cmod_path);
    
private:
    std::vector<std::string> search_paths_;
    std::unordered_map<std::string, std::unique_ptr<CmodInfo>> info_cache_;
    std::string last_error_;
    
    // 内部辅助方法
    bool validateCmodName(const std::string& name);
    bool validateCmodVersion(const std::string& version);
    bool validateFileStructure(const std::string& base_path, const CmodStructure& structure);
    
    // 文件系统操作
    bool directoryExists(const std::string& path);
    bool fileExists(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& path);
    bool readFileContent(const std::string& file_path, std::string& content);
    
    // 路径处理
    std::string joinPath(const std::string& base, const std::string& relative);
    std::string getFileName(const std::string& path);
    std::string getFileExtension(const std::string& path);
    std::string getDirectoryName(const std::string& path);
    
    // 解析辅助
    std::string extractInfoValue(const std::string& content, const std::string& key);
    std::vector<std::string> parseExportList(const std::string& content, const std::string& type);
    
    // 错误处理
    void setError(const std::string& error);
    
    // 常量
    static const std::string CMOD_EXTENSION;
    static const std::string CHTL_EXTENSION;
    static const std::string SRC_DIR;
    static const std::string INFO_DIR;
    static const std::string INFO_SECTION;
    static const std::string EXPORT_SECTION;
};

// Cmod相关的工具函数
namespace CmodUtils {
    // 标准搜索路径
    std::vector<std::string> getStandardSearchPaths();
    
    // 官方模块路径
    std::string getOfficialModulePath();
    
    // 本地模块路径
    std::string getLocalModulePath(const std::string& base_path = ".");
    
    // 模块名解析
    std::pair<std::string, std::string> parseModuleName(const std::string& full_name);
    
    // 版本比较
    int compareVersions(const std::string& version1, const std::string& version2);
    
    // 路径通配符匹配
    bool matchesPattern(const std::string& path, const std::string& pattern);
}

} // namespace chtl