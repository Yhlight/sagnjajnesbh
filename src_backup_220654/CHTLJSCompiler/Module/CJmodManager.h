#pragma once
#include "../../Shared/SimpleZip/SimpleZip.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace chtl {

// CJmod信息结构（不包含Export节）
struct CJmodInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string min_chtl_version;
    std::string max_chtl_version;
    
    // CJmod特有字段
    std::string api_version;        // CHTL JS编译器API版本
    std::string entry_point;        // 入口函数名
    std::vector<std::string> required_headers; // 需要的头文件
    std::vector<std::string> exported_functions; // 导出的C++函数
};

// CJmod文件结构
struct CJmodStructure {
    std::string main_module_name;
    std::string src_dir;
    std::string info_dir;
    std::vector<std::string> cpp_files;
    std::vector<std::string> header_files;
    std::vector<std::string> submodules;
    bool has_main_cpp;
    bool has_main_header;
};

// C++扩展接口定义
struct CJmodInterface {
    std::string function_name;
    std::string return_type;
    std::vector<std::pair<std::string, std::string>> parameters; // type, name
    std::string implementation_hint;
};

// CJmod管理器
class CJmodManager {
public:
    CJmodManager();
    ~CJmodManager();
    
    // CJmod打包功能
    bool packCJmod(const std::string& source_directory, const std::string& output_cjmod_path);
    bool packCJmodFromStructure(const CJmodStructure& structure, const std::string& base_path, 
                               const std::string& output_cjmod_path);
    
    // CJmod解包功能
    bool unpackCJmod(const std::string& cjmod_path, const std::string& output_directory);
    
    // CJmod验证功能
    bool validateCJmodStructure(const std::string& directory_path);
    bool validateCJmodFile(const std::string& cjmod_path);
    
    // CJmod信息解析
    bool parseCJmodInfo(const std::string& info_file_content, CJmodInfo& info);
    bool extractCJmodInfo(const std::string& cjmod_path, CJmodInfo& info);
    
    // CJmod结构分析
    bool analyzeCJmodStructure(const std::string& directory_path, CJmodStructure& structure);
    
    // 子模块管理
    std::vector<std::string> getCJmodSubmodules(const std::string& cjmod_path);
    bool hasCJmodSubmodule(const std::string& cjmod_path, const std::string& submodule_name);
    
    // 文件操作
    std::vector<std::string> listCJmodFiles(const std::string& cjmod_path);
    bool extractCJmodFile(const std::string& cjmod_path, const std::string& file_path, std::string& content);
    bool extractCJmodBinary(const std::string& cjmod_path, const std::string& file_path, std::vector<uint8_t>& data);
    
    // 路径解析（支持chtl::前缀）
    std::string resolveCJmodPath(const std::string& module_name, const std::vector<std::string>& search_paths);
    std::string resolveOfficialCJmodPath(const std::string& module_name);
    std::string resolveCJmodSubmodulePath(const std::string& main_module, const std::string& submodule);
    
    // C++源码分析
    std::vector<CJmodInterface> analyzeCppInterfaces(const std::string& cpp_content);
    std::vector<std::string> extractIncludeHeaders(const std::string& cpp_content);
    std::vector<std::string> extractExportedFunctions(const std::string& cpp_content);
    
    // 依赖管理
    std::vector<std::string> parseCJmodDependencies(const std::string& dependencies_str);
    bool checkCJmodDependencies(const CJmodInfo& info, const std::vector<std::string>& available_modules);
    
    // 版本兼容性检查
    bool isCJmodVersionCompatible(const std::string& required_version, const std::string& available_version);
    bool isAPIVersionCompatible(const std::string& cjmod_api_version, const std::string& compiler_api_version);
    
    // 编译支持
    bool generateCJmodWrapper(const CJmodInfo& info, const std::string& output_path);
    bool compileCJmodSources(const std::string& cjmod_path, const std::string& output_path);
    
    // 错误处理
    std::string getLastError() const { return last_error_; }
    
    // 搜索路径管理
    void addCJmodSearchPath(const std::string& path);
    void removeCJmodSearchPath(const std::string& path);
    std::vector<std::string> getCJmodSearchPaths() const { return search_paths_; }
    
    // 官方模块前缀支持
    bool isOfficialModule(const std::string& module_name);
    std::string stripOfficialPrefix(const std::string& module_name);
    std::string addOfficialPrefix(const std::string& module_name);
    
    // CJmod缓存管理
    void clearCJmodCache();
    bool isCJmodCached(const std::string& cjmod_path);
    
private:
    std::vector<std::string> search_paths_;
    std::unordered_map<std::string, std::unique_ptr<CJmodInfo>> info_cache_;
    std::string last_error_;
    
    // 内部辅助方法
    bool validateCJmodName(const std::string& name);
    bool validateCJmodVersion(const std::string& version);
    bool validateCJmodFileStructure(const std::string& base_path, const CJmodStructure& structure);
    
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
    
    // C++文件分析
    bool isCppFile(const std::string& filename);
    bool isHeaderFile(const std::string& filename);
    
    // 解析辅助
    std::string extractCJmodInfoValue(const std::string& content, const std::string& key);
    
    // 错误处理
    void setError(const std::string& error);
    
    // 常量
    static const std::string CJMOD_EXTENSION;
    static const std::string CPP_EXTENSION;
    static const std::string HEADER_EXTENSION;
    static const std::string CHTL_EXTENSION;
    static const std::string SRC_DIR;
    static const std::string INFO_DIR;
    static const std::string INFO_SECTION;
    static const std::string OFFICIAL_PREFIX;
};

// CJmod相关的工具函数
namespace CJmodUtils {
    // 标准搜索路径
    std::vector<std::string> getCJmodStandardSearchPaths();
    
    // 官方CJmod模块路径
    std::string getOfficialCJmodPath();
    
    // 本地CJmod模块路径
    std::string getLocalCJmodPath(const std::string& base_path = ".");
    
    // 模块名解析（支持chtl::前缀）
    std::pair<std::string, std::string> parseCJmodModuleName(const std::string& full_name);
    bool isOfficialModuleName(const std::string& module_name);
    
    // C++代码分析工具
    std::vector<std::string> extractFunctionDeclarations(const std::string& cpp_content);
    std::vector<std::string> extractIncludeDirectives(const std::string& cpp_content);
    std::string extractNamespace(const std::string& cpp_content);
    
    // 编译工具
    std::string generateCJmodWrapperCode(const CJmodInfo& info);
    std::vector<std::string> generateCompilerFlags(const CJmodInfo& info);
}

} // namespace chtl