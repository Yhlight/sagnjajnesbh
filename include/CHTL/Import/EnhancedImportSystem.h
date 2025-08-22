#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {
namespace AST {
    class ASTNode;
    class ImportNode;
    class OriginNode;
}

namespace Import {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    // 基础Web资源导入
    HTML,                   // @Html
    STYLE,                  // @Style  
    JAVASCRIPT,             // @JavaScript
    
    // 模块导入
    CHTL,                   // @Chtl
    CJMOD,                  // @CJmod
    CONFIG,                 // @Config
    
    // 模板导入
    TEMPLATE_STYLE,         // [Template] @Style
    TEMPLATE_ELEMENT,       // [Template] @Element
    TEMPLATE_VAR,           // [Template] @Var
    
    // 自定义导入
    CUSTOM_STYLE,           // [Custom] @Style
    CUSTOM_ELEMENT,         // [Custom] @Element
    CUSTOM_VAR,             // [Custom] @Var
    
    // 原始嵌入导入
    ORIGIN_HTML,            // [Origin] @Html
    ORIGIN_STYLE,           // [Origin] @Style
    ORIGIN_JAVASCRIPT,      // [Origin] @JavaScript
    ORIGIN_CUSTOM,          // [Origin] @CustomType
    
    // 配置导入
    CONFIGURATION_CONFIG,   // [Configuration] @Config
    
    UNKNOWN                 // 未知类型
};

/**
 * @brief 路径类型枚举
 */
enum class PathType {
    NAME_ONLY,              // 名称（不带后缀）
    SPECIFIC_FILE,          // 具体文件名（带后缀）
    SPECIFIC_PATH,          // 具体路径（含文件信息）
    DIRECTORY_PATH,         // 目录路径（不含文件信息）
    
    // 通配符：支持所有文件类型
    WILDCARD_ALL,           // 通配符：.*或/*（所有文件）
    WILDCARD_CHTL,          // 通配符：.*.chtl或/*.chtl
    WILDCARD_CMOD,          // 通配符：.*.cmod或/*.cmod
    WILDCARD_CJMOD,         // 通配符：.*.cjmod或/*.cjmod
    WILDCARD_HTML,          // 通配符：.*.html或/*.html
    WILDCARD_CSS,           // 通配符：.*.css或/*.css
    WILDCARD_JS,            // 通配符：.*.js或/*.js
    
    // 子模块
    SUBMODULE_ALL,          // 子模块：Module.*
    SUBMODULE_SPECIFIC      // 子模块：Module.SubModule
};

/**
 * @brief 导入信息结构
 */
struct ImportInfo {
    ImportType type;                // 导入类型
    std::string path;               // 导入路径
    std::string asName;             // as别名（可选）
    PathType pathType;              // 路径类型
    std::vector<std::string> importList; // 导入列表（对于@Chtl）
    bool hasAsClause;               // 是否有as子句
    bool useOfficialPrefix;         // 是否使用官方模块前缀
    std::string originalPath;       // 原始路径字符串
};

/**
 * @brief 路径解析器
 */
class PathResolver {
public:
    /**
     * @brief 构造函数
     * @param currentFilePath 当前编译文件路径
     * @param officialModulePath 官方模块路径
     */
    PathResolver(const std::string& currentFilePath, const std::string& officialModulePath);
    
    /**
     * @brief 解析导入路径
     * @param importInfo 导入信息
     * @return 解析后的文件路径列表
     */
    std::vector<std::string> ResolvePath(const ImportInfo& importInfo);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool FileExists(const std::string& filePath) const;
    
    /**
     * @brief 获取目录下的所有文件
     * @param directory 目录路径
     * @param extension 文件扩展名（可选）
     * @return 文件路径列表
     */
    std::vector<std::string> GetDirectoryFiles(const std::string& directory, 
                                              const std::string& extension = "") const;

private:
    /**
     * @brief 解析@Html/@Style/@JavaScript类型路径
     * @param importInfo 导入信息
     * @return 文件路径列表
     */
    std::vector<std::string> ResolveWebAssetPath(const ImportInfo& importInfo);
    
    /**
     * @brief 解析@Chtl类型路径
     * @param importInfo 导入信息
     * @return 文件路径列表
     */
    std::vector<std::string> ResolveChtlPath(const ImportInfo& importInfo);
    
    /**
     * @brief 解析@CJmod类型路径
     * @param importInfo 导入信息
     * @return 文件路径列表
     */
    std::vector<std::string> ResolveCJmodPath(const ImportInfo& importInfo);
    
    /**
     * @brief 解析通配符路径
     * @param importInfo 导入信息
     * @return 文件路径列表
     */
    std::vector<std::string> ResolveWildcardPath(const ImportInfo& importInfo);
    
    /**
     * @brief 解析子模块路径
     * @param importInfo 导入信息
     * @return 文件路径列表
     */
    std::vector<std::string> ResolveSubmodulePath(const ImportInfo& importInfo);
    
    /**
     * @brief 搜索官方模块目录
     * @param fileName 文件名
     * @param extensions 支持的扩展名列表
     * @return 找到的文件路径
     */
    std::string SearchOfficialModules(const std::string& fileName, 
                                     const std::vector<std::string>& extensions);
    
    /**
     * @brief 搜索当前目录的module文件夹
     * @param fileName 文件名
     * @param extensions 支持的扩展名列表
     * @return 找到的文件路径
     */
    std::string SearchCurrentModules(const std::string& fileName, 
                                    const std::vector<std::string>& extensions);
    
    /**
     * @brief 搜索当前目录
     * @param fileName 文件名
     * @param extensions 支持的扩展名列表
     * @return 找到的文件路径
     */
    std::string SearchCurrentDirectory(const std::string& fileName, 
                                      const std::vector<std::string>& extensions);

private:
    std::string currentFilePath_;       // 当前编译文件路径
    std::string currentDirectory_;      // 当前目录
    std::string officialModulePath_;    // 官方模块路径
};

/**
 * @brief 导入冲突检测器
 */
class ImportConflictDetector {
public:
    /**
     * @brief 构造函数
     */
    ImportConflictDetector();
    
    /**
     * @brief 检查循环依赖
     * @param currentFile 当前文件
     * @param importPath 导入路径
     * @return 是否存在循环依赖
     */
    bool CheckCircularDependency(const std::string& currentFile, const std::string& importPath);
    
    /**
     * @brief 检查重复导入
     * @param importPath 导入路径
     * @return 是否为重复导入
     */
    bool CheckDuplicateImport(const std::string& importPath);
    
    /**
     * @brief 规范化路径（解决同一路径多表达方式问题）
     * @param path 原始路径
     * @return 规范化后的路径
     */
    std::string NormalizePath(const std::string& path);
    
    /**
     * @brief 添加导入记录
     * @param currentFile 当前文件
     * @param importPath 导入路径
     */
    void AddImportRecord(const std::string& currentFile, const std::string& importPath);
    
    /**
     * @brief 重置检测器状态
     */
    void Reset();

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph_; // 依赖图
    std::unordered_set<std::string> importedPaths_;                                    // 已导入路径
    std::unordered_map<std::string, std::string> pathNormalizationCache_;             // 路径规范化缓存
    
    /**
     * @brief 深度优先搜索检查循环依赖
     * @param current 当前节点
     * @param target 目标节点
     * @param visited 已访问节点
     * @param recursionStack 递归栈
     * @return 是否存在循环
     */
    bool DFSCheckCycle(const std::string& current, const std::string& target,
                      std::unordered_set<std::string>& visited,
                      std::unordered_set<std::string>& recursionStack);
};

/**
 * @brief 导入解析器
 */
class ImportParser {
public:
    /**
     * @brief 构造函数
     */
    ImportParser();
    
    /**
     * @brief 解析导入语句
     * @param importStatement 导入语句字符串
     * @return 导入信息
     */
    ImportInfo ParseImportStatement(const std::string& importStatement);
    
    /**
     * @brief 检查是否为支持的导入类型
     * @param typeStr 类型字符串
     * @return 导入类型
     */
    ImportType GetImportType(const std::string& typeStr);
    
    /**
     * @brief 解析路径类型
     * @param path 路径字符串
     * @return 路径类型
     */
    PathType GetPathType(const std::string& path);

private:
    /**
     * @brief 解析from子句
     * @param fromClause from子句字符串
     * @return 路径信息
     */
    std::string ParseFromClause(const std::string& fromClause);
    
    /**
     * @brief 解析as子句
     * @param asClause as子句字符串
     * @return 别名
     */
    std::string ParseAsClause(const std::string& asClause);
    
    /**
     * @brief 解析import列表
     * @param importClause import子句字符串
     * @return 导入项列表
     */
    std::vector<std::string> ParseImportList(const std::string& importClause);
};

/**
 * @brief 增强导入系统
 */
class EnhancedImportSystem {
public:
    /**
     * @brief 构造函数
     * @param currentFilePath 当前编译文件路径
     * @param officialModulePath 官方模块路径
     */
    EnhancedImportSystem(const std::string& currentFilePath, 
                        const std::string& officialModulePath);
    
    /**
     * @brief 处理导入语句
     * @param importStatement 导入语句
     * @return 处理结果（AST节点列表）
     */
    std::vector<std::shared_ptr<AST::ASTNode>> ProcessImport(const std::string& importStatement);
    
    /**
     * @brief 设置配置
     * @param disableDefaultNamespace 是否禁用默认命名空间
     */
    void SetConfig(bool disableDefaultNamespace);
    
    /**
     * @brief 获取导入统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;
    
    /**
     * @brief 重置导入系统状态
     */
    void Reset();

private:
    /**
     * @brief 处理@Html/@Style/@JavaScript导入
     * @param importInfo 导入信息
     * @return AST节点列表
     */
    std::vector<std::shared_ptr<AST::ASTNode>> ProcessWebAssetImport(const ImportInfo& importInfo);
    
    /**
     * @brief 处理@Chtl导入
     * @param importInfo 导入信息
     * @return AST节点列表
     */
    std::vector<std::shared_ptr<AST::ASTNode>> ProcessChtlImport(const ImportInfo& importInfo);
    
    /**
     * @brief 处理@CJmod导入
     * @param importInfo 导入信息
     * @return AST节点列表
     */
    std::vector<std::shared_ptr<AST::ASTNode>> ProcessCJmodImport(const ImportInfo& importInfo);
    
    /**
     * @brief 处理[Origin]自定义类型导入
     * @param importInfo 导入信息
     * @return AST节点列表
     */
    std::vector<std::shared_ptr<AST::ASTNode>> ProcessOriginImport(const ImportInfo& importInfo);
    
    /**
     * @brief 创建带名原始嵌入节点
     * @param type 原始嵌入类型
     * @param content 内容
     * @param name 名称
     * @return 原始嵌入节点
     */
    std::shared_ptr<AST::OriginNode> CreateNamedOriginNode(ImportType type,
                                                          const std::string& content,
                                                          const std::string& name);
    
    /**
     * @brief 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string ReadFileContent(const std::string& filePath);
    
    /**
     * @brief 创建默认命名空间
     * @param fileName 文件名
     * @return 命名空间名称
     */
    std::string CreateDefaultNamespace(const std::string& fileName);

private:
    PathResolver pathResolver_;                     // 路径解析器
    ImportConflictDetector conflictDetector_;       // 冲突检测器
    ImportParser parser_;                           // 导入解析器
    
    // 配置
    std::string currentFilePath_;                   // 当前文件路径
    bool disableDefaultNamespace_;                  // 是否禁用默认命名空间
    
    // 统计信息
    mutable int htmlImportCount_;                   // HTML导入计数
    mutable int styleImportCount_;                  // 样式导入计数
    mutable int jsImportCount_;                     // JavaScript导入计数
    mutable int chtlImportCount_;                   // CHTL导入计数
    mutable int cjmodImportCount_;                  // CJmod导入计数
    mutable int originImportCount_;                 // 原始嵌入导入计数
    mutable int duplicateImportCount_;              // 重复导入计数
    mutable int circularDependencyCount_;           // 循环依赖计数
};

/**
 * @brief 模块目录结构管理器
 */
class ModuleDirectoryManager {
public:
    /**
     * @brief 构造函数
     * @param officialModulePath 官方模块路径
     */
    ModuleDirectoryManager(const std::string& officialModulePath);
    
    /**
     * @brief 检查目录是否使用分类结构
     * @param directory 目录路径
     * @return 是否使用CMOD/CJMOD分类结构
     */
    bool IsClassifiedStructure(const std::string& directory) const;
    
    /**
     * @brief 在分类结构中搜索文件
     * @param directory 目录路径
     * @param fileName 文件名
     * @param extensions 支持的扩展名
     * @return 找到的文件路径
     */
    std::string SearchInClassifiedStructure(const std::string& directory,
                                           const std::string& fileName,
                                           const std::vector<std::string>& extensions);
    
    /**
     * @brief 在混杂结构中搜索文件
     * @param directory 目录路径
     * @param fileName 文件名
     * @param extensions 支持的扩展名
     * @return 找到的文件路径
     */
    std::string SearchInMixedStructure(const std::string& directory,
                                      const std::string& fileName,
                                      const std::vector<std::string>& extensions);

private:
    std::string officialModulePath_;    // 官方模块路径
};

} // namespace Import
} // namespace CHTL