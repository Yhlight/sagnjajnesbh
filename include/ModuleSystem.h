#pragma once

#include "CHTLCommon.h"
#include "CHTLASTNodes.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace chtl {

// 模块文件信息
struct ModuleFileInfo {
    String relativePath;    // 相对路径
    String content;         // 文件内容
    size_t size;           // 文件大小
    bool isDirectory;      // 是否为目录
    
    ModuleFileInfo(const String& path, const String& data = "", bool isDir = false)
        : relativePath(path), content(data), size(data.size()), isDirectory(isDir) {}
};

// 模块信息
struct ModuleInfo {
    String name;
    String version;
    String description;
    String author;
    String license;
    String dependencies;
    String category;
    String minCHTLVersion;
    String maxCHTLVersion;
    
    // 导出信息
    std::vector<String> exportedStyles;
    std::vector<String> exportedElements;
    std::vector<String> exportedVars;
    
    ModuleInfo() = default;
    
    // 从字符串解析模块信息
    bool parseFromString(const String& infoContent);
    
    // 转换为字符串
    String toString() const;
    
    // 验证模块信息
    bool validate() const;
};

// 简单的Zip压缩/解压缩实现
class SimpleZip {
public:
    SimpleZip() = default;
    ~SimpleZip() = default;
    
    // 压缩文件到内存
    static bool compressToMemory(const std::vector<ModuleFileInfo>& files, std::vector<uint8_t>& output);
    
    // 从内存解压缩文件
    static bool decompressFromMemory(const std::vector<uint8_t>& data, std::vector<ModuleFileInfo>& files);
    
    // 压缩文件到文件
    static bool compressToFile(const std::vector<ModuleFileInfo>& files, const String& outputPath);
    
    // 从文件解压缩
    static bool decompressFromFile(const String& inputPath, std::vector<ModuleFileInfo>& files);

private:
    // 简单的压缩算法（基于deflate的简化版本）
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data);
    
    // 计算CRC32
    static uint32_t crc32(const std::vector<uint8_t>& data);
    
    // 写入/读取基本数据类型
    static void writeUint32(std::vector<uint8_t>& buffer, uint32_t value);
    static void writeUint16(std::vector<uint8_t>& buffer, uint16_t value);
    static void writeString(std::vector<uint8_t>& buffer, const String& str);
    static uint32_t readUint32(const std::vector<uint8_t>& buffer, size_t& offset);
    static uint16_t readUint16(const std::vector<uint8_t>& buffer, size_t& offset);
    static String readString(const std::vector<uint8_t>& buffer, size_t& offset, size_t length);
};

// Cmod管理器
class CmodManager {
public:
    CmodManager() = default;
    ~CmodManager() = default;
    
    // 打包Cmod
    bool packCmod(const String& sourceDirectory, const String& outputPath);
    
    // 解包Cmod
    bool unpackCmod(const String& cmodPath, const String& outputDirectory);
    
    // 验证Cmod格式
    bool validateCmodStructure(const String& directory);
    
    // 加载Cmod信息
    ModuleInfo loadCmodInfo(const String& cmodPath);
    
    // 列出Cmod内容
    std::vector<String> listCmodContents(const String& cmodPath);
    
    // 从目录创建模块信息
    ModuleInfo createModuleInfoFromDirectory(const String& directory);
    
    // 生成导出信息
    bool generateExportInfo(const String& sourceDirectory, ModuleInfo& moduleInfo);
    
    // 错误处理
    std::vector<String> getErrors() const { return errors_; }
    std::vector<String> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

private:
    std::vector<String> errors_;
    std::vector<String> warnings_;
    
    // 辅助方法
    void addError(const String& error);
    void addWarning(const String& warning);
    
    // 收集目录中的文件
    std::vector<ModuleFileInfo> collectFiles(const String& directory, const String& basePath = "");
    
    // 创建目录结构
    bool createDirectoryStructure(const String& basePath, const std::vector<ModuleFileInfo>& files);
    
    // 验证必需文件
    bool validateRequiredFiles(const std::vector<ModuleFileInfo>& files);
    
    // 分析CHTL文件以提取导出信息
    void analyzeChtlFile(const String& content, ModuleInfo& moduleInfo);
    
    // 路径处理
    String normalizePath(const String& path);
    String joinPath(const String& base, const String& relative);
    bool isValidModuleName(const String& name);
};

// CJmod管理器
class CJmodManager {
public:
    CJmodManager() = default;
    ~CJmodManager() = default;
    
    // 打包CJmod
    bool packCJmod(const String& sourceDirectory, const String& outputPath);
    
    // 解包CJmod
    bool unpackCJmod(const String& cjmodPath, const String& outputDirectory);
    
    // 验证CJmod格式
    bool validateCJmodStructure(const String& directory);
    
    // 加载CJmod信息
    ModuleInfo loadCJmodInfo(const String& cjmodPath);
    
    // 列出CJmod内容
    std::vector<String> listCJmodContents(const String& cjmodPath);
    
    // 编译C++源码（需要外部编译器）
    bool compileCppSources(const String& sourceDirectory, const String& outputDirectory);
    
    // 验证C++接口
    bool validateCppInterface(const String& headerPath);
    
    // 生成接口文档
    bool generateInterfaceDoc(const String& sourceDirectory, const String& outputPath);
    
    // 错误处理
    std::vector<String> getErrors() const { return errors_; }
    std::vector<String> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

private:
    std::vector<String> errors_;
    std::vector<String> warnings_;
    
    // 辅助方法
    void addError(const String& error);
    void addWarning(const String& warning);
    
    // 收集C++源文件
    std::vector<ModuleFileInfo> collectCppFiles(const String& directory, const String& basePath = "");
    
    // 验证C++文件结构
    bool validateCppStructure(const std::vector<ModuleFileInfo>& files);
    
    // 分析C++头文件
    void analyzeCppHeader(const String& content, ModuleInfo& moduleInfo);
    
    // 检查必需的C++接口函数
    bool checkRequiredInterface(const String& headerContent);
};

// 统一模块管理器
class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager() = default;
    
    // 初始化模块系统
    bool initialize(const String& officialModulePath = "");
    
    // 安装模块
    bool installModule(const String& modulePath, const String& installDirectory = "");
    
    // 卸载模块
    bool uninstallModule(const String& moduleName);
    
    // 列出已安装的模块
    std::vector<ModuleInfo> listInstalledModules();
    
    // 查找模块
    String findModule(const String& moduleName);
    
    // 加载模块
    std::shared_ptr<CHTLASTNode> loadModule(const String& moduleName);
    
    // 验证模块依赖
    bool validateDependencies(const ModuleInfo& moduleInfo);
    
    // 解析模块依赖关系
    std::vector<String> resolveDependencies(const String& moduleName);
    
    // 检查模块兼容性
    bool checkCompatibility(const ModuleInfo& moduleInfo, const String& chtlVersion);
    
    // 创建官方模块前缀处理
    bool isOfficialModule(const String& moduleName);
    String resolveOfficialModulePath(const String& moduleName);
    
    // 模块缓存管理
    void clearModuleCache();
    size_t getModuleCacheSize() const;
    
    // 配置管理
    void setOfficialModulePath(const String& path);
    void setInstallDirectory(const String& directory);
    
    // 错误处理
    std::vector<String> getErrors() const { return errors_; }
    std::vector<String> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

private:
    std::unique_ptr<CmodManager> cmodManager_;
    std::unique_ptr<CJmodManager> cjmodManager_;
    
    String officialModulePath_;
    String installDirectory_;
    
    std::unordered_map<String, ModuleInfo> installedModules_;
    std::unordered_map<String, std::shared_ptr<CHTLASTNode>> moduleCache_;
    
    std::vector<String> errors_;
    std::vector<String> warnings_;
    
    // 辅助方法
    void addError(const String& error);
    void addWarning(const String& warning);
    
    // 模块类型检测
    enum class ModuleType {
        CMOD,
        CJMOD,
        UNKNOWN
    };
    
    ModuleType detectModuleType(const String& modulePath);
    
    // 模块信息持久化
    bool saveModuleRegistry();
    bool loadModuleRegistry();
    String getRegistryPath();
    
    // 依赖解析
    std::vector<String> parseDependencyString(const String& dependencies);
    bool isDependencySatisfied(const String& dependency);
    
    // 版本比较
    int compareVersions(const String& version1, const String& version2);
    bool isVersionInRange(const String& version, const String& minVersion, const String& maxVersion);
};

} // namespace chtl