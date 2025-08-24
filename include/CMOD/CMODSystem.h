#pragma once

#include "CHTL/Core/CHTLToken.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {
namespace CMOD {

/**
 * @brief CMOD模块信息
 * 严格按照语法文档第1346-1357行的[Info]块格式
 */
struct CMODModuleInfo {
    std::string name;               // 模块名称 - 必须与文件夹同名
    std::string version;            // 版本号
    std::string description;        // 描述
    std::string author;             // 作者
    std::string license;            // 许可证
    std::string dependencies;       // 依赖（逗号分隔）
    std::string category;           // 分类
    std::string minCHTLVersion;     // 最小CHTL版本
    std::string maxCHTLVersion;     // 最大CHTL版本
    
    // 验证信息完整性
    bool IsValid() const;
    
    // 检查版本兼容性
    bool IsVersionCompatible(const std::string& currentCHTLVersion) const;
    
    // 解析依赖列表
    std::vector<std::string> GetDependencies() const;
};

/**
 * @brief CMOD导出信息
 * 严格按照语法文档第1361-1380行的[Export]块格式
 * 系统自动生成，优化性能用
 */
struct CMODExportInfo {
    // 自定义类型导出 - 语法文档第1363-1370行
    std::vector<std::string> customStyles;      // [Custom] @Style
    std::vector<std::string> customElements;    // [Custom] @Element
    std::vector<std::string> customVars;        // [Custom] @Var
    
    // 模板类型导出 - 语法文档第1372-1374行
    std::vector<std::string> templateStyles;    // [Template] @Style
    std::vector<std::string> templateElements;  // [Template] @Element
    std::vector<std::string> templateVars;      // [Template] @Var
    
    // 原始嵌入导出 - 语法文档第1375-1378行
    std::vector<std::string> originHtml;        // [Origin] @Html
    std::vector<std::string> originStyle;       // [Origin] @Style
    std::vector<std::string> originJavascript;  // [Origin] @Javascript
    std::vector<std::string> originCustomTypes; // [Origin] @CustomType (如@Vue)
    
    // 配置导出 - 语法文档第1379行
    std::vector<std::string> configurations;    // [Configuration] @Config
    
    // 自动生成导出信息
    void GenerateFromModule(const class CMODModule& module);
    
    // 序列化为CHTL格式
    std::string SerializeToCHTL() const;
};

/**
 * @brief CMOD模块结构类型
 * 基于语法文档第1323-1401行的模块结构定义
 */
enum class CMODModuleType {
    SIMPLE_MODULE,          // 简单模块（无子模块）
    SUBMODULE_CONTAINER,    // 子模块容器（有子模块）
    SUBMODULE,              // 子模块
    MIXED_MODULE            // 混合模块（CMOD + CJMOD）
};

/**
 * @brief CMOD文件结构
 * 严格按照语法文档第1323-1401行的标准格式
 */
struct CMODFileStructure {
    std::string moduleName;         // 模块名称（与文件夹同名）
    std::string modulePath;         // 模块路径
    CMODModuleType moduleType;      // 模块类型
    
    // 主要文件路径
    std::string srcPath;            // src文件夹路径
    std::string infoPath;           // info文件夹路径
    std::string mainModuleFile;     // 主模块.chtl文件路径（可选）
    std::string infoFile;           // 信息.chtl文件路径（必须）
    
    // 源文件列表
    std::vector<std::string> sourceFiles;      // src中的.chtl文件
    std::vector<std::string> subModules;       // 子模块列表
    
    // 验证结构完整性
    bool ValidateStructure() const;
    
    // 检查命名规则
    bool ValidateNamingRules() const;
    
    // 扫描模块结构
    static CMODFileStructure ScanModuleStructure(const std::string& modulePath);
};

/**
 * @brief CMOD模块
 * 完整的CMOD模块表示，包含所有信息和内容
 */
class CMODModule {
public:
    /**
     * @brief 构造函数
     * @param moduleStructure 模块结构
     */
    explicit CMODModule(const CMODFileStructure& moduleStructure);
    
    /**
     * @brief 加载模块
     * @return 是否加载成功
     */
    bool Load();
    
    /**
     * @brief 解析模块内容
     * @return 是否解析成功
     */
    bool Parse();
    
    /**
     * @brief 验证模块完整性
     * @return 是否验证通过
     */
    bool Validate();
    
    /**
     * @brief 获取模块信息
     */
    const CMODModuleInfo& GetModuleInfo() const { return moduleInfo_; }
    
    /**
     * @brief 获取导出信息
     */
    const CMODExportInfo& GetExportInfo() const { return exportInfo_; }
    
    /**
     * @brief 获取模块结构
     */
    const CMODFileStructure& GetModuleStructure() const { return moduleStructure_; }
    
    /**
     * @brief 获取子模块
     */
    const std::vector<std::shared_ptr<CMODModule>>& GetSubModules() const { return subModules_; }
    
    /**
     * @brief 获取模块AST
     */
    const std::vector<AST::ASTNodePtr>& GetModuleAST() const { return moduleAST_; }
    
    /**
     * @brief 检查是否有子模块
     */
    bool HasSubModules() const { return !subModules_.empty(); }
    
    /**
     * @brief 查找子模块
     * @param subModuleName 子模块名称
     * @return 子模块指针，未找到返回nullptr
     */
    std::shared_ptr<CMODModule> FindSubModule(const std::string& subModuleName) const;
    
    /**
     * @brief 获取所有导出的符号
     * @return 导出符号映射
     */
    std::unordered_map<std::string, AST::ASTNodePtr> GetAllExports() const;

private:
    CMODFileStructure moduleStructure_;     // 模块结构
    CMODModuleInfo moduleInfo_;             // 模块信息
    CMODExportInfo exportInfo_;             // 导出信息
    std::vector<std::shared_ptr<CMODModule>> subModules_;  // 子模块
    std::vector<AST::ASTNodePtr> moduleAST_;               // 模块AST
    
    // 加载辅助方法
    bool LoadModuleInfo();
    bool LoadSourceFiles();
    bool LoadSubModules();
    bool GenerateExportInfo();
    
    // 解析辅助方法
    bool ParseInfoFile();
    bool ParseSourceFiles();
    bool ParseSubModules();
    
    // 验证辅助方法
    bool ValidateNamingRules();
    bool ValidateFileStructure();
    bool ValidateVersionCompatibility();
    bool ValidateDependencies();
};

/**
 * @brief CMOD打包器
 * 严格按照语法文档第1317行：将符合CMOD格式的文件夹打包成.cmod文件
 */
class CMODPacker {
public:
    /**
     * @brief 打包模块文件夹为.cmod文件
     * @param moduleFolder 模块文件夹路径
     * @param outputPath 输出.cmod文件路径
     * @return 是否打包成功
     */
    static bool PackModule(const std::string& moduleFolder, const std::string& outputPath);
    
    /**
     * @brief 解包.cmod文件为模块文件夹
     * @param cmodFile .cmod文件路径
     * @param outputFolder 输出文件夹路径
     * @return 是否解包成功
     */
    static bool UnpackModule(const std::string& cmodFile, const std::string& outputFolder);
    
    /**
     * @brief 验证模块文件夹格式
     * @param moduleFolder 模块文件夹路径
     * @return 验证结果
     */
    static bool ValidateModuleFolder(const std::string& moduleFolder);

private:
    // 打包辅助方法
    static bool CompressFolder(const std::string& folderPath, const std::string& outputPath);
    static bool ExtractArchive(const std::string& archivePath, const std::string& outputPath);
    static bool ValidatePackedContent(const std::string& archivePath);
};

/**
 * @brief CMOD管理器
 * 完整的CMOD系统管理，支持所有语法文档要求
 */
class CompleteCMODManager {
public:
    /**
     * @brief 构造函数
     * @param compilerPath 编译器路径（用于确定官方模块目录）
     * @param currentFilePath 当前编译文件路径
     */
    CompleteCMODManager(const std::string& compilerPath, const std::string& currentFilePath);
    
    /**
     * @brief 析构函数
     */
    ~CompleteCMODManager() = default;
    
    /**
     * @brief 加载模块
     * @param moduleName 模块名称
     * @param subModulePath 子模块路径（可选，如"Chtholly.Space"）
     * @return 是否加载成功
     */
    bool LoadModule(const std::string& moduleName, const std::string& subModulePath = "");
    
    /**
     * @brief 获取模块
     * @param moduleName 模块名称
     * @return 模块指针，未找到返回nullptr
     */
    std::shared_ptr<CMODModule> GetModule(const std::string& moduleName) const;
    
    /**
     * @brief 获取子模块
     * @param moduleName 模块名称
     * @param subModulePath 子模块路径
     * @return 子模块指针，未找到返回nullptr
     */
    std::shared_ptr<CMODModule> GetSubModule(const std::string& moduleName, const std::string& subModulePath) const;
    
    /**
     * @brief 搜索模块
     * @param moduleName 模块名称
     * @return 模块路径，未找到返回空字符串
     */
    std::string SearchModule(const std::string& moduleName) const;
    
    /**
     * @brief 获取搜索路径
     * 基于用户要求的动态路径确定策略
     */
    std::vector<std::string> GetSearchPaths() const;
    
    /**
     * @brief 检查模块依赖
     * @param moduleName 模块名称
     * @return 依赖检查结果
     */
    bool CheckModuleDependencies(const std::string& moduleName) const;
    
    /**
     * @brief 获取所有已加载模块
     */
    std::vector<std::string> GetLoadedModules() const;
    
    /**
     * @brief 清理所有模块
     */
    void ClearAllModules();
    
    /**
     * @brief 重新加载模块
     * @param moduleName 模块名称
     * @return 是否重新加载成功
     */
    bool ReloadModule(const std::string& moduleName);
    
    /**
     * @brief 获取模块统计信息
     */
    std::string GetModuleStatistics() const;

private:
    std::string compilerPath_;          // 编译器路径
    std::string currentFilePath_;       // 当前编译文件路径
    std::string officialModulePath_;    // 官方模块目录路径
    std::string currentModulePath_;     // 当前模块目录路径
    std::string currentDirPath_;        // 当前目录路径
    
    // 已加载的模块
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> loadedModules_;
    
    // 模块依赖图
    std::unordered_map<std::string, std::vector<std::string>> dependencyGraph_;
    
    // 初始化方法
    void InitializeSearchPaths();
    void InitializeOfficialModulePath();
    void InitializeCurrentPaths();
    
    // 搜索辅助方法
    std::string SearchInOfficialModules(const std::string& moduleName) const;
    std::string SearchInCurrentModuleFolder(const std::string& moduleName) const;
    std::string SearchInCurrentDirectory(const std::string& moduleName) const;
    std::string SearchWithExtension(const std::string& basePath, const std::string& moduleName, const std::string& extension) const;
    
    // 模块加载辅助方法
    bool LoadModuleFromPath(const std::string& modulePath, const std::string& moduleName);
    bool LoadSubModulesRecursively(std::shared_ptr<CMODModule> parentModule);
    
    // 依赖管理
    bool ResolveDependencies(const std::string& moduleName);
    bool CheckCircularDependencies(const std::string& moduleName, std::vector<std::string>& visitStack) const;
    
    // 版本管理
    bool CheckVersionCompatibility(const CMODModuleInfo& moduleInfo) const;
    std::string GetCurrentCHTLVersion() const;
    
    // 文件系统辅助
    bool DirectoryExists(const std::string& path) const;
    bool FileExists(const std::string& path) const;
    std::vector<std::string> ListDirectory(const std::string& path) const;
    bool IsModuleDirectory(const std::string& path) const;
    bool IsCMODFile(const std::string& path) const;
};

/**
 * @brief CMOD模块目录结构管理器
 * 支持语法文档第1427-1453行的分类结构和混杂结构
 */
class CMODDirectoryManager {
public:
    /**
     * @brief 模块目录结构类型
     */
    enum class DirectoryStructureType {
        MIXED,              // 混杂结构（chtl、cmod、cjmod混合）
        CLASSIFIED          // 分类结构（CMOD/CJMOD文件夹分类）
    };
    
    /**
     * @brief 检测目录结构类型
     * @param modulePath 模块路径
     * @return 目录结构类型
     */
    static DirectoryStructureType DetectStructureType(const std::string& modulePath);
    
    /**
     * @brief 扫描分类结构的CMOD文件
     * @param modulePath 模块路径
     * @return CMOD文件列表
     */
    static std::vector<std::string> ScanClassifiedCMODFiles(const std::string& modulePath);
    
    /**
     * @brief 扫描混杂结构的CMOD文件
     * @param modulePath 模块路径
     * @return CMOD文件列表
     */
    static std::vector<std::string> ScanMixedCMODFiles(const std::string& modulePath);
    
    /**
     * @brief 创建标准CMOD目录结构
     * @param modulePath 模块路径
     * @param moduleName 模块名称
     * @param useClassified 是否使用分类结构
     * @return 是否创建成功
     */
    static bool CreateStandardStructure(const std::string& modulePath, const std::string& moduleName, bool useClassified = true);

public:
    // 目录操作辅助方法
    static bool HasCMODFolder(const std::string& path);
    static bool HasCJMODFolder(const std::string& path);
    static std::vector<std::string> GetCMODFolderVariants();  // CMOD/Cmod/cmod
    static std::vector<std::string> GetCJMODFolderVariants(); // CJMOD/CJmod/cjmod
};

/**
 * @brief CMOD导入解析器
 * 处理语法文档第1403-1407行的导入语法
 */
class CMODImportResolver {
public:
    /**
     * @brief 导入类型
     */
    enum class ImportType {
        FULL_MODULE,        // [Import] @Chtl from 模块名
        SUBMODULE,          // [Import] @Chtl from Chtholly.Space
        WILDCARD            // [Import] @Chtl from Chtholly.*
    };
    
    /**
     * @brief 导入信息
     */
    struct ImportInfo {
        ImportType type;                // 导入类型
        std::string moduleName;         // 模块名称
        std::string subModulePath;      // 子模块路径
        std::string asName;             // 别名（如果有）
        bool isWildcard;                // 是否通配符导入
    };
    
    /**
     * @brief 解析导入语句
     * @param importStatement 导入语句
     * @return 导入信息
     */
    static ImportInfo ParseImportStatement(const std::string& importStatement);
    
    /**
     * @brief 解析模块路径
     * @param modulePath 模块路径（如"Chtholly.Space"）
     * @return 解析结果：{模块名, 子模块路径}
     */
    static std::pair<std::string, std::string> ParseModulePath(const std::string& modulePath);
    
    /**
     * @brief 验证导入语法
     * @param importStatement 导入语句
     * @return 是否语法正确
     */
    static bool ValidateImportSyntax(const std::string& importStatement);

private:
    // 解析辅助方法
    static ImportType DetermineImportType(const std::string& fromClause);
    static std::string ExtractModuleName(const std::string& fromClause);
    static std::string ExtractSubModulePath(const std::string& fromClause);
    static std::string ExtractAsName(const std::string& importStatement);
};

/**
 * @brief CMOD版本管理器
 * 处理语法文档第1355-1356行的版本兼容性
 */
class CMODVersionManager {
public:
    /**
     * @brief 版本信息
     */
    struct VersionInfo {
        int major;
        int minor;
        int patch;
        
        // 从字符串解析版本
        static VersionInfo Parse(const std::string& versionStr);
        
        // 转换为字符串
        std::string ToString() const;
        
        // 版本比较
        bool operator<(const VersionInfo& other) const;
        bool operator<=(const VersionInfo& other) const;
        bool operator>(const VersionInfo& other) const;
        bool operator>=(const VersionInfo& other) const;
        bool operator==(const VersionInfo& other) const;
    };
    
    /**
     * @brief 检查版本兼容性
     * @param moduleMinVersion 模块最小版本要求
     * @param moduleMaxVersion 模块最大版本要求
     * @param currentVersion 当前CHTL版本
     * @return 是否兼容
     */
    static bool CheckCompatibility(const std::string& moduleMinVersion, 
                                  const std::string& moduleMaxVersion,
                                  const std::string& currentVersion);
    
    /**
     * @brief 获取当前CHTL版本
     * @return 当前版本字符串
     */
    static std::string GetCurrentCHTLVersion();

private:
    static bool IsVersionInRange(const VersionInfo& version, const VersionInfo& minVer, const VersionInfo& maxVer);
};

/**
 * @brief CMOD依赖管理器
 * 处理模块依赖关系和循环依赖检测
 */
class CMODDependencyManager {
public:
    /**
     * @brief 依赖信息
     */
    struct DependencyInfo {
        std::string moduleName;         // 依赖模块名称
        std::string versionRequirement; // 版本要求
        bool isOptional;                // 是否可选依赖
    };
    
    /**
     * @brief 解析依赖字符串
     * @param dependenciesStr 依赖字符串（逗号分隔）
     * @return 依赖信息列表
     */
    static std::vector<DependencyInfo> ParseDependencies(const std::string& dependenciesStr);
    
    /**
     * @brief 检查循环依赖
     * @param moduleName 模块名称
     * @param dependencyGraph 依赖图
     * @return 是否存在循环依赖
     */
    static bool CheckCircularDependencies(const std::string& moduleName, 
                                         const std::unordered_map<std::string, std::vector<std::string>>& dependencyGraph);
    
    /**
     * @brief 解析依赖顺序
     * @param modules 模块列表
     * @return 解析后的加载顺序
     */
    static std::vector<std::string> ResolveDependencyOrder(const std::vector<std::string>& modules);

private:
    static bool HasCircularDependency(const std::string& moduleName,
                                     const std::unordered_map<std::string, std::vector<std::string>>& graph,
                                     std::vector<std::string>& visitStack,
                                     std::unordered_set<std::string>& visited);
};

} // namespace CMOD
} // namespace CHTL