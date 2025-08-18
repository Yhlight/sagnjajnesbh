#pragma once
#include "../../common/Context.h"
#include "../../common/CmodManager.h"
#include "../../common/CJmodManager.h"
#include "../ast/ASTNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>
#include <regex>

namespace chtl {

// 导入类型枚举
enum class ImportType {
    HTML,           // @Html
    STYLE,          // @Style  
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    TEMPLATE_STYLE, // [Template] @Style
    TEMPLATE_ELEMENT, // [Template] @Element
    TEMPLATE_VAR,   // [Template] @Var
    CUSTOM_STYLE,   // [Custom] @Style
    CUSTOM_ELEMENT, // [Custom] @Element
    CUSTOM_VAR      // [Custom] @Var
};

// 路径类型
enum class PathType {
    NAME_ONLY,          // 仅名称，不带后缀
    NAME_WITH_EXTENSION, // 名称带后缀
    SPECIFIC_PATH,      // 具体路径含文件信息
    DIRECTORY_PATH,     // 目录路径不含文件信息
    WILDCARD_PATH       // 通配符路径 (*, *.ext)
};

// 导入信息
struct ImportInfo {
    ImportType type;
    std::string original_path;      // 原始路径
    std::string resolved_path;      // 解析后的绝对路径
    std::string alias_name;         // as 后的别名
    PathType path_type;
    bool has_alias;
    std::vector<std::string> resolved_files; // 对于通配符，存储所有匹配的文件
    
    ImportInfo(ImportType t, const std::string& path, const std::string& alias = "")
        : type(t), original_path(path), alias_name(alias), 
          has_alias(!alias.empty()), path_type(PathType::NAME_ONLY) {}
};

// 导入依赖节点
struct ImportDependencyNode {
    std::string file_path;
    std::vector<ImportInfo> imports;
    std::unordered_set<std::string> dependencies; // 依赖的其他文件
    bool is_processing;  // 用于循环依赖检测
    bool is_processed;   // 是否已处理完成
    
    ImportDependencyNode(const std::string& path) 
        : file_path(path), is_processing(false), is_processed(false) {}
};

// 导入搜索配置
struct ImportSearchConfig {
    std::string official_module_dir;    // 官方模块目录
    std::string current_module_dir;     // 当前目录的module文件夹
    std::string current_dir;            // 当前目录
    std::vector<std::string> search_extensions; // 搜索的文件扩展名
    bool recursive_search;              // 是否递归搜索
    
    ImportSearchConfig() : recursive_search(false) {
        search_extensions = {".cmod", ".cjmod", ".chtl", ".html", ".css", ".js"};
    }
};

// 导入管理器
class ImportManager {
public:
    explicit ImportManager(CompilerContext& context);
    ~ImportManager() = default;
    
    // 设置搜索配置
    void setSearchConfig(const ImportSearchConfig& config);
    ImportSearchConfig getSearchConfig() const;
    
    // 解析导入语句
    ImportInfo parseImportStatement(const std::string& import_type, 
                                   const std::string& path, 
                                   const std::string& alias = "");
    
    // 处理各类导入类型
    std::vector<ImportInfo> processHtmlImport(const std::string& path, const std::string& alias = "");
    std::vector<ImportInfo> processStyleImport(const std::string& path, const std::string& alias = "");
    std::vector<ImportInfo> processJavaScriptImport(const std::string& path, const std::string& alias = "");
    std::vector<ImportInfo> processChtlImport(const std::string& path, const std::string& alias = "");
    std::vector<ImportInfo> processCJmodImport(const std::string& path, const std::string& alias = "");
    
    // 处理模板和自定义导入
    std::vector<ImportInfo> processTemplateImport(ImportType template_type, 
                                                  const std::string& name,
                                                  const std::string& path,
                                                  const std::string& alias = "");
    std::vector<ImportInfo> processCustomImport(ImportType custom_type,
                                               const std::string& name, 
                                               const std::string& path,
                                               const std::string& alias = "");
    
    // 路径解析和搜索
    PathType analyzePathType(const std::string& path) const;
    std::vector<std::string> resolvePath(const std::string& path, ImportType type) const;
    std::vector<std::string> searchFiles(const std::string& base_path, 
                                        const std::string& pattern,
                                        const std::vector<std::string>& extensions) const;
    
    // 通配符支持
    std::vector<std::string> resolveWildcardPath(const std::string& path, ImportType type) const;
    bool isWildcardPath(const std::string& path) const;
    std::string convertDotSlashPath(const std::string& path) const; // 转换 '.' 为 '/'
    
    // 循环依赖检测
    bool detectCircularDependency(const std::string& current_file, 
                                 const std::string& target_file);
    bool hasCircularDependency(const std::string& file_path) const;
    std::vector<std::string> getCircularDependencyChain(const std::string& file_path) const;
    
    // 重复导入检测
    bool isAlreadyImported(const std::string& file_path, ImportType type) const;
    bool isDuplicateImport(const ImportInfo& import_info) const;
    void markAsImported(const ImportInfo& import_info);
    
    // 导入规范化
    std::string normalizeImportPath(const std::string& path) const;
    std::string getCanonicalPath(const std::string& path) const;
    bool isSameImport(const ImportInfo& a, const ImportInfo& b) const;
    
    // 依赖图管理
    void addDependency(const std::string& from_file, const std::string& to_file);
    void removeDependency(const std::string& from_file, const std::string& to_file);
    std::vector<std::string> getDependencies(const std::string& file_path) const;
    std::vector<std::string> getTopologicalOrder() const;
    
    // 导入缓存管理
    void clearImportCache();
    void clearDependencyGraph();
    size_t getImportCacheSize() const;
    
    // 错误处理
    void reportImportError(const std::string& message, const ImportInfo& import_info);
    void reportImportWarning(const std::string& message, const ImportInfo& import_info);
    std::vector<std::string> getImportErrors() const;
    std::vector<std::string> getImportWarnings() const;
    
    // 验证和诊断
    bool validateImportPath(const std::string& path, ImportType type) const;
    bool validateImportSyntax(const std::string& import_statement) const;
    std::string getImportStatistics() const;
    void printDependencyGraph() const;
    
    // 创建AST节点
    std::unique_ptr<ast::ASTNode> createOriginBlockNode(ImportType type, 
                                                       const std::string& name,
                                                       const std::string& content) const;
    std::unique_ptr<ast::ImportStatementNode> createImportStatementNode(const ImportInfo& import_info) const;
    
private:
    CompilerContext& context_;
    ImportSearchConfig search_config_;
    
    // 导入缓存和依赖图
    std::unordered_map<std::string, std::vector<ImportInfo>> import_cache_;
    std::unordered_map<std::string, ImportDependencyNode> dependency_graph_;
    std::unordered_set<std::string> imported_files_;
    
    // 错误和警告
    std::vector<std::string> import_errors_;
    std::vector<std::string> import_warnings_;
    
    // 内部辅助方法
    std::string getCurrentDirectory() const;
    std::string getOfficialModuleDirectory() const;
    std::string getCurrentModuleDirectory() const;
    
    // 文件搜索策略
    std::vector<std::string> searchInOfficialModules(const std::string& name, 
                                                    const std::vector<std::string>& extensions) const;
    std::vector<std::string> searchInCurrentModules(const std::string& name,
                                                   const std::vector<std::string>& extensions) const;
    std::vector<std::string> searchInCurrentDirectory(const std::string& name,
                                                     const std::vector<std::string>& extensions) const;
    
    // HTML/CSS/JS特定搜索
    std::vector<std::string> searchResourceFiles(const std::string& name, 
                                                const std::string& extension) const;
    
    // CHTL/CJmod特定搜索
    std::vector<std::string> searchModuleFiles(const std::string& name, 
                                              const std::vector<std::string>& extensions) const;
    
    // Cmod管理
    std::unique_ptr<CmodManager> cmod_manager_;
    
    // CJmod管理
    std::unique_ptr<CJmodManager> cjmod_manager_;
    
    // Cmod特定方法
    std::vector<ImportInfo> processCmodImport(const std::string& module_name, const std::string& alias = "");
    std::vector<ImportInfo> processSubmoduleImport(const std::string& main_module, 
                                                  const std::string& submodule, 
                                                  const std::string& alias = "");
    bool loadCmodContent(const std::string& cmod_path, const std::string& file_path, std::string& content);
    std::vector<std::string> extractCmodSymbols(const std::string& cmod_path, const std::string& symbol_type);
    
    // CJmod特定方法
    std::vector<ImportInfo> processCJmodImport(const std::string& module_name, const std::string& alias = "");
    std::vector<ImportInfo> processCJmodSubmoduleImport(const std::string& main_module, 
                                                       const std::string& submodule, 
                                                       const std::string& alias = "");
    bool loadCJmodContent(const std::string& cjmod_path, const std::string& file_path, std::string& content);
    bool compileCJmodModule(const std::string& cjmod_path, const std::string& output_path);
    
    // 通配符处理
    std::vector<std::string> expandWildcard(const std::string& base_path, 
                                          const std::string& pattern) const;
    bool matchesPattern(const std::string& filename, const std::string& pattern) const;
    
    // 循环依赖检测辅助
    bool dfsDetectCycle(const std::string& current, 
                       const std::string& target,
                       std::unordered_set<std::string>& visited,
                       std::unordered_set<std::string>& recursion_stack,
                       std::vector<std::string>& path) const;
    
    // 路径处理辅助
    bool isAbsolutePath(const std::string& path) const;
    bool isRelativePath(const std::string& path) const;
    std::string joinPath(const std::string& base, const std::string& relative) const;
    std::string getFileExtension(const std::string& path) const;
    std::string getBaseName(const std::string& path) const;
    std::string getDirectoryPath(const std::string& path) const;
    
    // 导入类型转换
    ImportType stringToImportType(const std::string& type_str) const;
    std::string importTypeToString(ImportType type) const;
    std::vector<std::string> getExtensionsForType(ImportType type) const;
    
    // 验证辅助
    bool isValidHtmlFile(const std::string& path) const;
    bool isValidStyleFile(const std::string& path) const;
    bool isValidJavaScriptFile(const std::string& path) const;
    bool isValidChtlFile(const std::string& path) const;
    bool isValidCJmodFile(const std::string& path) const;
    
    // 内容读取
    std::string readFileContent(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
};

// 导入语句解析器
class ImportStatementParser {
public:
    struct ParsedImport {
        ImportType type;
        std::string element_name;  // 对于模板/自定义导入
        std::string path;
        std::string alias;
        bool is_template;
        bool is_custom;
        
        ParsedImport() : type(ImportType::CHTL), is_template(false), is_custom(false) {}
    };
    
    static ParsedImport parseImportStatement(const std::string& statement);
    static bool isValidImportStatement(const std::string& statement);
    static std::string normalizeImportStatement(const std::string& statement);
    
private:
    static std::regex getImportRegex();
    static ImportType parseImportType(const std::string& type_part);
    static std::pair<bool, bool> parseModifiers(const std::string& modifiers); // [Template]/[Custom]
};

// 便利宏定义
#define CHTL_IMPORT_HTML(manager, path, alias) \
    (manager).processHtmlImport(path, alias)

#define CHTL_IMPORT_STYLE(manager, path, alias) \
    (manager).processStyleImport(path, alias)

#define CHTL_IMPORT_JAVASCRIPT(manager, path, alias) \
    (manager).processJavaScriptImport(path, alias)

#define CHTL_IMPORT_CHTL(manager, path, alias) \
    (manager).processChtlImport(path, alias)

#define CHTL_IMPORT_CJMOD(manager, path, alias) \
    (manager).processCJmodImport(path, alias)

#define CHTL_CHECK_CIRCULAR_DEPENDENCY(manager, current, target) \
    (manager).detectCircularDependency(current, target)

#define CHTL_VALIDATE_IMPORT_PATH(manager, path, type) \
    (manager).validateImportPath(path, type)

} // namespace chtl