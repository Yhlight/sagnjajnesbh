#pragma once

#include "CHTLCommon.h"
#include "CHTLASTNodes.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

namespace chtl {

// 导入路径类型
enum class ImportPathType {
    FILENAME_ONLY,      // 只有文件名（不带后缀）
    FILENAME_WITH_EXT,  // 文件名带后缀
    DIRECTORY_PATH,     // 目录路径
    FULL_FILE_PATH,     // 完整文件路径
    WILDCARD_ALL,       // .* 或 /*
    WILDCARD_CMOD,      // .*.cmod 或 /*.cmod
    WILDCARD_CHTL       // .*.chtl 或 /*.chtl
};

// 导入结果
struct ImportResult {
    bool success = false;
    String resolvedPath;
    std::vector<String> resolvedPaths; // 用于通配符导入
    String error;
    std::shared_ptr<CHTLASTNode> ast = nullptr;
    std::vector<std::shared_ptr<CHTLASTNode>> astNodes; // 用于通配符导入
};

// 路径规范化结果
struct PathNormalizationResult {
    String normalizedPath;
    bool isEquivalent = false;
    String canonicalForm;
};

// 循环依赖检测器
class CircularDependencyDetector {
public:
    bool hasCircularDependency(const String& currentFile, const String& targetFile);
    void addDependency(const String& from, const String& to);
    void removeDependency(const String& from, const String& to);
    std::vector<String> getDependencyChain(const String& file);
    void clear();

private:
    std::unordered_map<String, std::unordered_set<String>> dependencies_;
    bool dfsCheck(const String& current, const String& target, std::unordered_set<String>& visited, std::unordered_set<String>& recursionStack);
};

// 重复导入管理器
class DuplicateImportManager {
public:
    bool isAlreadyImported(const String& normalizedPath, ImportNode::ImportType type);
    void markAsImported(const String& normalizedPath, ImportNode::ImportType type, std::shared_ptr<CHTLASTNode> ast);
    std::shared_ptr<CHTLASTNode> getCachedImport(const String& normalizedPath, ImportNode::ImportType type);
    void clearCache();
    size_t getCacheSize() const;

private:
    struct ImportCacheEntry {
        ImportNode::ImportType type;
        std::shared_ptr<CHTLASTNode> ast;
        String normalizedPath;
    };
    
    std::unordered_map<String, ImportCacheEntry> importCache_;
};

// 路径解析器
class PathResolver {
public:
    PathResolver(const String& currentDirectory, const String& officialModulePath = "");
    
    // 路径规范化
    PathNormalizationResult normalizePath(const String& path);
    bool arePathsEquivalent(const String& path1, const String& path2);
    
    // 路径类型检测
    ImportPathType detectPathType(const String& path);
    
    // 路径搜索
    std::vector<String> searchPaths(const String& path, ImportNode::ImportType importType);
    
    // 特定类型的路径解析
    std::vector<String> resolveHtmlStyleJsPath(const String& path, ImportNode::ImportType type);
    std::vector<String> resolveChtlPath(const String& path);
    std::vector<String> resolveCJmodPath(const String& path);
    
    // 通配符路径解析
    std::vector<String> resolveWildcardPath(const String& path, ImportNode::ImportType type);
    
    // 官方模块前缀处理
    bool isOfficialModulePath(const String& path);
    String resolveOfficialModulePath(const String& path);

private:
    String currentDirectory_;
    String officialModulePath_;
    String currentModuleDirectory_;
    
    // 辅助方法
    std::vector<String> searchInDirectory(const String& directory, const String& filename, const std::vector<String>& extensions);
    std::vector<String> searchInDirectoryWithWildcard(const String& directory, const String& pattern);
    bool fileExists(const String& path);
    bool isDirectory(const String& path);
    String getFileExtension(const String& path);
    String getFileNameWithoutExtension(const String& path);
    String joinPath(const String& directory, const String& filename);
};

// 主导入管理器
class ImportManager {
public:
    ImportManager(const String& currentDirectory, const String& officialModulePath = "");
    ~ImportManager() = default;
    
    // 主要导入接口
    ImportResult processImport(std::shared_ptr<ImportNode> importNode, const String& currentFile);
    
    // 特定类型导入处理
    ImportResult processHtmlImport(const String& path, const String& alias, const String& currentFile);
    ImportResult processStyleImport(const String& path, const String& alias, const String& currentFile);
    ImportResult processJavaScriptImport(const String& path, const String& alias, const String& currentFile);
    ImportResult processChtlImport(const String& path, const String& currentFile);
    ImportResult processCJmodImport(const String& path, const String& currentFile);
    
    // 通配符导入处理
    ImportResult processWildcardImport(const String& path, ImportNode::ImportType type, const String& currentFile);
    
    // 带名原始嵌入节点创建
    std::shared_ptr<OriginNode> createNamedOriginNode(ImportNode::ImportType type, const String& content, const String& name);
    
    // 依赖管理
    void addDependency(const String& from, const String& to);
    bool checkCircularDependency(const String& from, const String& to);
    std::vector<String> getDependencyChain(const String& file);
    
    // 缓存管理
    void clearImportCache();
    size_t getImportCacheSize() const;
    
    // 配置
    void setOfficialModulePath(const String& path);
    void setCurrentDirectory(const String& directory);
    
    // 错误处理
    std::vector<String> getErrors() const { return errors_; }
    std::vector<String> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }

private:
    std::unique_ptr<PathResolver> pathResolver_;
    std::unique_ptr<CircularDependencyDetector> circularDetector_;
    std::unique_ptr<DuplicateImportManager> duplicateManager_;
    
    std::vector<String> errors_;
    std::vector<String> warnings_;
    
    // 辅助方法
    std::shared_ptr<CHTLASTNode> loadFileContent(const String& filePath, ImportNode::ImportType type);
    bool validateImportPath(const String& path, ImportNode::ImportType type);
    void addError(const String& error);
    void addWarning(const String& warning);
};

} // namespace chtl