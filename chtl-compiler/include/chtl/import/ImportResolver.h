#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

namespace chtl {
namespace import {

/**
 * 导入类型
 */
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CUSTOM_ELEMENT, // [Custom] @Element
    CUSTOM_STYLE,   // [Custom] @Style
    CUSTOM_VAR,     // [Custom] @Var
    TEMPLATE_ELEMENT, // [Template] @Element
    TEMPLATE_STYLE,   // [Template] @Style
    TEMPLATE_VAR,     // [Template] @Var
    CONFIG,           // @Config
    ORIGIN_HTML,      // [Origin] @Html
    ORIGIN_STYLE,     // [Origin] @Style
    ORIGIN_JAVASCRIPT // [Origin] @JavaScript
};

/**
 * 导入信息
 */
struct ImportInfo {
    ImportType type;
    std::string path;           // 导入路径
    std::string targetName;     // 要导入的具体名称（如元素名、样式组名）
    std::string asName;         // as语法重命名
    bool hasAs;                 // 是否有as语法
    bool importAll;             // 是否导入所有（如导入所有模板）
    
    // 位置信息
    std::string sourceFile;
    size_t line;
    size_t column;
    
    // 解析后的信息
    std::string resolvedPath;   // 解析后的完整路径
    bool isResolved = false;
};

/**
 * 导入路径解析器
 * 负责解析各种导入路径，查找文件
 */
class ImportResolver {
public:
    ImportResolver();
    ~ImportResolver();
    
    /**
     * 设置编译器模块路径（官方模块）
     */
    void SetCompilerModulePath(const std::string& path);
    
    /**
     * 设置当前编译文件路径
     */
    void SetCurrentFile(const std::string& filePath);
    
    /**
     * 解析导入路径
     * @param import 导入信息
     * @return 是否成功解析
     */
    bool ResolveImport(ImportInfo& import);
    
    /**
     * 批量解析导入
     */
    bool ResolveImports(std::vector<ImportInfo>& imports);
    
    /**
     * 获取解析错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
private:
    std::string m_CompilerModulePath;  // 编译器官方模块路径
    std::filesystem::path m_CurrentFilePath;  // 当前编译文件路径
    std::filesystem::path m_CurrentDirectory;  // 当前文件所在目录
    
    std::vector<std::string> m_Errors;
    
    // 不同类型的解析方法
    bool ResolveHtmlStyleJS(ImportInfo& import);
    bool ResolveChtl(ImportInfo& import);
    bool ResolveCJmod(ImportInfo& import);
    bool ResolveCustomOrTemplate(ImportInfo& import);
    bool ResolveOrigin(ImportInfo& import);
    bool ResolveConfig(ImportInfo& import);
    
    // 路径搜索辅助方法
    std::string FindFileWithExtensions(const std::filesystem::path& dir,
                                      const std::string& basename,
                                      const std::vector<std::string>& extensions);
    std::string FindFile(const std::filesystem::path& dir,
                        const std::string& filename);
    std::vector<std::string> FindAllFiles(const std::filesystem::path& dir,
                                         const std::string& pattern);
    
    // 错误报告
    void ReportError(const ImportInfo& import, const std::string& message);
};

/**
 * 导入依赖图
 * 用于检测循环依赖和管理导入关系
 */
class ImportDependencyGraph {
public:
    ImportDependencyGraph();
    ~ImportDependencyGraph();
    
    /**
     * 添加依赖关系
     * @param from 源文件
     * @param to 目标文件
     * @return 是否添加成功（false表示会产生循环依赖）
     */
    bool AddDependency(const std::string& from, const std::string& to);
    
    /**
     * 检查是否存在循环依赖
     */
    bool HasCycle() const;
    
    /**
     * 获取拓扑排序结果
     * @return 排序后的文件列表（如果有循环依赖则返回空）
     */
    std::vector<std::string> GetTopologicalOrder() const;
    
    /**
     * 检查文件是否已被导入
     */
    bool IsImported(const std::string& file) const;
    
    /**
     * 标记文件为已导入
     */
    void MarkAsImported(const std::string& file);
    
    /**
     * 获取循环依赖路径（如果存在）
     */
    std::vector<std::string> GetCyclePath() const;
    
private:
    // 邻接表表示的依赖图
    std::unordered_map<std::string, std::unordered_set<std::string>> m_Graph;
    
    // 已导入的文件集合
    std::unordered_set<std::string> m_ImportedFiles;
    
    // 循环依赖检测
    mutable std::vector<std::string> m_CyclePath;
    
    // DFS辅助方法
    bool DFSCycleDetection(const std::string& node,
                          std::unordered_set<std::string>& visited,
                          std::unordered_set<std::string>& recStack,
                          std::vector<std::string>& path) const;
};

/**
 * 导入管理器
 * 整合路径解析和依赖管理
 */
class ImportManager {
public:
    ImportManager();
    ~ImportManager();
    
    /**
     * 设置编译器模块路径
     */
    void SetCompilerModulePath(const std::string& path);
    
    /**
     * 处理导入声明
     * @param imports 导入信息列表
     * @param currentFile 当前文件路径
     * @return 是否成功处理所有导入
     */
    bool ProcessImports(std::vector<ImportInfo>& imports,
                       const std::string& currentFile);
    
    /**
     * 检查是否有循环依赖
     */
    bool HasCircularDependency() const;
    
    /**
     * 获取循环依赖信息
     */
    std::string GetCircularDependencyInfo() const;
    
    /**
     * 获取已导入的文件列表（按依赖顺序）
     */
    std::vector<std::string> GetImportedFiles() const;
    
    /**
     * 路径规范化
     * 处理同一路径的不同表达方式
     */
    static std::string NormalizePath(const std::string& path);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    void ClearErrors() { m_Errors.clear(); }
    
private:
    std::unique_ptr<ImportResolver> m_Resolver;
    std::unique_ptr<ImportDependencyGraph> m_DependencyGraph;
    
    std::vector<std::string> m_Errors;
    
    // 已处理的导入（避免重复导入）
    std::unordered_map<std::string, std::vector<ImportInfo>> m_ProcessedImports;
    
    // 错误报告
    void ReportError(const std::string& message);
};

} // namespace import
} // namespace chtl