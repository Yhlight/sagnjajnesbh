#pragma once
#include "../../Common/SimpleZip/SimpleZip.h"
#include "../../Common/Core/UnifiedErrorHandler.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace module {

/**
 * CMOD模块信息
 * 严格按照CHTL语法文档中[Info]部分的规范
 */
struct CmodInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    
    // 文件路径信息
    std::string modulePath;
    std::string infoPath;
    std::string srcPath;
    
    CmodInfo() = default;
};

/**
 * CMOD导出符号信息
 * 严格按照CHTL语法文档中[Export]部分的规范
 */
struct CmodExport {
    std::string type;           // [Custom], [Template], [Origin], [Configuration]
    std::string subType;        // @Element, @Style, @Var, @Html, @Javascript, @Config
    std::vector<std::string> symbols;
    
    CmodExport() = default;
    CmodExport(const std::string& t, const std::string& st, const std::vector<std::string>& syms)
        : type(t), subType(st), symbols(syms) {}
};

/**
 * 子模块信息
 */
struct SubmoduleInfo {
    std::string name;
    std::string srcPath;
    std::string infoPath;
    CmodInfo info;
    std::vector<CmodExport> exports;
    
    SubmoduleInfo() = default;
    SubmoduleInfo(const std::string& n) : name(n) {}
};

/**
 * CMOD模块结构
 * 严格按照CHTL语法文档的标准格式
 */
struct CmodModule {
    CmodInfo info;
    std::vector<CmodExport> exports;
    std::vector<SubmoduleInfo> submodules;
    
    // 文件路径
    std::string rootPath;
    std::string mainChtlPath;      // src/ModuleName.chtl (可选)
    
    // 验证三同名规则
    bool isValidNaming() const;
    
    // 检查是否有子模块
    bool hasSubmodules() const { return !submodules.empty(); }
    
    CmodModule() = default;
};

/**
 * CMOD管理器
 * 严格按照CHTL语法文档实现CMOD的打包、解包功能
 */
class CmodManager {
public:
    CmodManager();
    ~CmodManager();
    
    /**
     * 设置错误处理器
     */
    void setErrorHandler(std::shared_ptr<chtl::common::ErrorHandler> errorHandler);
    
    // === CMOD验证 ===
    
    /**
     * 验证CMOD文件夹结构
     * 检查三同名规则：模块文件夹，主模块chtl文件，模块信息chtl文件必须同名
     */
    bool validateCmodStructure(const std::string& modulePath);
    
    /**
     * 验证子模块结构
     * 子模块必须在src内部
     */
    bool validateSubmoduleStructure(const std::string& submodulePath);
    
    /**
     * 验证info.chtl文件格式
     * 必须包含[Info]部分，[Export]部分由系统生成
     */
    bool validateInfoFile(const std::string& infoFilePath);
    
    // === CMOD解析 ===
    
    /**
     * 解析CMOD模块
     */
    bool parseCmodModule(const std::string& modulePath, CmodModule& module);
    
    /**
     * 解析模块信息文件
     * 解析[Info]部分
     */
    bool parseInfoFile(const std::string& infoFilePath, CmodInfo& info);
    
    /**
     * 解析导出文件
     * 解析[Export]部分
     */
    bool parseExportFile(const std::string& infoFilePath, std::vector<CmodExport>& exports);
    
    /**
     * 解析子模块
     */
    bool parseSubmodules(const std::string& srcPath, std::vector<SubmoduleInfo>& submodules);
    
    /**
     * 自动生成[Export]部分
     * 通过扫描src目录中的.chtl文件自动生成导出表
     */
    bool generateExports(const std::string& srcPath, std::vector<CmodExport>& exports);
    
    // === CMOD打包 ===
    
    /**
     * 打包CMOD文件夹为.cmod文件
     * 严格按照CHTL语法文档的标准格式
     */
    bool packCmod(const std::string& modulePath, const std::string& outputPath);
    
    /**
     * 打包带子模块的CMOD
     */
    bool packCmodWithSubmodules(const std::string& modulePath, const std::string& outputPath);
    
    // === CMOD解包 ===
    
    /**
     * 解包.cmod文件到指定目录
     */
    bool unpackCmod(const std::string& cmodFilePath, const std::string& outputDir);
    
    /**
     * 验证.cmod文件完整性
     */
    bool validateCmodFile(const std::string& cmodFilePath);
    
    // === CMOD安装和管理 ===
    
    /**
     * 安装CMOD到模块目录
     * 支持官方模块目录和当前目录module文件夹
     */
    bool installCmod(const std::string& cmodFilePath, const std::string& targetDir);
    
    /**
     * 卸载CMOD
     */
    bool uninstallCmod(const std::string& moduleName, const std::string& moduleDir);
    
    /**
     * 列出已安装的CMOD
     */
    std::vector<CmodInfo> listInstalledCmods(const std::string& moduleDir);
    
    /**
     * 查找CMOD模块
     * 按照Import规则的搜索顺序：官方模块目录→当前目录module文件夹→当前目录
     */
    std::string findCmodModule(const std::string& moduleName);
    
    // === 版本兼容性 ===
    
    /**
     * 检查CHTL版本兼容性
     */
    bool checkVersionCompatibility(const CmodInfo& info, const std::string& currentVersion);
    
    /**
     * 解析版本字符串
     */
    struct Version {
        int major = 0;
        int minor = 0;
        int patch = 0;
        
        Version() = default;
        Version(const std::string& versionStr);
        bool operator<(const Version& other) const;
        bool operator<=(const Version& other) const;
        bool operator>(const Version& other) const;
        bool operator>=(const Version& other) const;
        bool operator==(const Version& other) const;
    };
    
    Version parseVersion(const std::string& versionStr);
    
    // === 依赖管理 ===
    
    /**
     * 解析依赖关系
     */
    std::vector<std::string> parseDependencies(const std::string& dependencies);
    
    /**
     * 检查依赖是否满足
     */
    bool checkDependencies(const CmodInfo& info, const std::string& moduleDir);
    
    /**
     * 自动安装依赖
     */
    bool installDependencies(const CmodInfo& info, const std::string& moduleDir);
    
    // === 实用工具 ===
    
    /**
     * 获取模块的完整路径
     */
    std::string getModuleFullPath(const std::string& moduleDir, const std::string& moduleName);
    
    /**
     * 检查文件是否存在
     */
    bool fileExists(const std::string& filePath);
    
    /**
     * 检查目录是否存在
     */
    bool directoryExists(const std::string& dirPath);
    
    /**
     * 创建目录
     */
    bool createDirectory(const std::string& dirPath);
    
    /**
     * 获取当前CHTL版本
     */
    std::string getCurrentCHTLVersion();
    
    /**
     * 获取错误信息
     */
    std::vector<std::string> getErrors() const { return errors_; }
    
    /**
     * 清理错误信息
     */
    void clearErrors() { errors_.clear(); }

private:
    std::shared_ptr<chtl::common::ErrorHandler> errorHandler_;
    std::vector<std::string> errors_;
    
    // 私有辅助方法
    void reportError(const std::string& message, const std::string& context = "");
    void reportWarning(const std::string& message, const std::string& context = "");
    
    // 文件操作辅助
    bool readFileContent(const std::string& filePath, std::string& content);
    bool writeFileContent(const std::string& filePath, const std::string& content);
    
    // 路径处理辅助
    std::string normalizePath(const std::string& path);
    std::string joinPath(const std::string& base, const std::string& relative);
    std::string getFileName(const std::string& filePath);
    std::string getFileBaseName(const std::string& filePath);
    std::string getDirectoryName(const std::string& filePath);
    
    // CHTL文件解析辅助
    bool parseInfoSection(const std::string& content, CmodInfo& info);
    bool parseExportSection(const std::string& content, std::vector<CmodExport>& exports);
    
    // 导出生成辅助
    bool scanChtlFile(const std::string& filePath, std::vector<CmodExport>& exports);
    bool extractSymbolsFromChtl(const std::string& content, std::vector<CmodExport>& exports);
    
    // ZIP操作辅助
    bool addDirectoryToZip(SimpleZip& zip, const std::string& dirPath, const std::string& zipPrefix = "");
    bool extractZipToDirectory(const std::string& zipPath, const std::string& outputDir);
    
    // 验证辅助
    bool isValidModuleName(const std::string& name);
    bool isValidVersion(const std::string& version);
    bool hasRequiredInfoFields(const CmodInfo& info);
};

} // namespace module
} // namespace chtl