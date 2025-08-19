#pragma once
#include "../State/CHTLGlobalMap.h"
#include "../../Common/ModulePathManager.h"
#include "../../Common/Core/UnifiedErrorHandler.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <regex>

namespace chtl {
namespace import {

/**
 * 导入类型枚举 - 严格按照CHTL语法文档
 */
enum class ImportType {
    HTML_FILE,              // [Import] @Html from html文件路径 as 命名为
    CSS_FILE,               // [Import] @Style from css文件路径 as 命名为  
    JAVASCRIPT_FILE,        // [Import] @JavaScript from js文件路径 as 命名为
    CHTL_FILE,              // [Import] @Chtl from chtl文件路径
    CJMOD_FILE,             // [Import] @CJmod from cjmod文件路径
    CONFIGURATION,          // [Import] @Config / [Import] [Configuration] @Config
    ORIGIN_HTML,            // [Import] [Origin] @Html 名称 from chtl文件路径
    ORIGIN_STYLE,           // [Import] [Origin] @Style 名称 from chtl文件路径
    ORIGIN_JAVASCRIPT,      // [Import] [Origin] @Javascript 名称 from chtl文件路径
    CUSTOM_ELEMENT,         // [Import] [Custom] @Element 名称 from chtl文件路径
    CUSTOM_STYLE,           // [Import] [Custom] @Style 名称 from chtl文件路径
    CUSTOM_VAR,             // [Import] [Custom] @Var 名称 from chtl文件路径
    TEMPLATE_ELEMENT,       // [Import] [Template] @Element 名称 from chtl文件路径
    TEMPLATE_STYLE,         // [Import] [Template] @Style 名称 from chtl文件路径
    TEMPLATE_VAR,           // [Import] [Template] @Var 名称 from chtl文件路径
    ALL_TEMPLATES,          // [Import] [Template] from chtl文件路径
    ALL_CUSTOMS,            // [Import] [Custom] from chtl文件路径
    ALL_ORIGINS,            // [Import] [Origin] from chtl文件路径
    ALL_CUSTOM_ELEMENTS,    // [Import] [Custom] @Element from chtl文件路径
    ALL_CUSTOM_STYLES,      // [Import] [Custom] @Style from chtl文件路径
    ALL_CUSTOM_VARS,        // [Import] [Custom] @Var from chtl文件路径
    ALL_TEMPLATE_ELEMENTS,  // [Import] [Template] @Element from chtl文件路径
    ALL_TEMPLATE_STYLES,    // [Import] [Template] @Style from chtl文件路径
    ALL_TEMPLATE_VARS       // [Import] [Template] @Var from chtl文件路径
};

/**
 * 路径解析规则 - 严格按照您的要求
 */
struct PathResolutionRule {
    std::string originalPath;
    std::string resolvedPath;
    bool hasFileExtension;
    bool isDirectory;
    bool isWildcard;
    bool isSubmodule;
    std::string searchPattern;
};

/**
 * 增强的导入语句信息 - 完全符合CHTL语法文档
 */
struct EnhancedImportStatement {
    ImportType type;
    std::string targetName;         // 要导入的具体名称（如果有）
    std::string sourcePath;         // from 后的路径
    std::string alias;              // as 后的别名（如果有）
    bool hasAlias;
    bool isWildcard;
    std::string configName;         // 配置组名称（仅用于配置导入）
    int lineNumber = 0;
    int columnNumber = 0;
    
    // 路径解析结果
    PathResolutionRule pathRule;
};

/**
 * 循环依赖检测器
 */
class CircularDependencyDetector {
public:
    bool addDependency(const std::string& from, const std::string& to);
    bool hasCircularDependency() const;
    std::vector<std::string> getCircularPath() const;
    void clear();

private:
    std::unordered_map<std::string, std::vector<std::string>> dependencies_;
    bool dfs(const std::string& node, std::unordered_set<std::string>& visited, 
             std::unordered_set<std::string>& recStack, std::vector<std::string>& path) const;
};

/**
 * 重复导入检测器
 */
class DuplicateImportDetector {
public:
    bool addImport(const EnhancedImportStatement& import);
    bool isDuplicate(const EnhancedImportStatement& import) const;
    std::vector<EnhancedImportStatement> getDuplicates() const;
    void clear();

private:
    std::vector<EnhancedImportStatement> imports_;
    std::string getImportKey(const EnhancedImportStatement& import) const;
};

/**
 * 增强的导入管理器
 * 严格按照CHTL语法文档实现所有导入功能
 */
class ImportManager {
public:
    ImportManager();
    ~ImportManager();
    
    /**
     * 设置模块路径管理器
     */
    void setModulePathManager(std::shared_ptr<common::ModulePathManager> pathManager);
    
    /**
     * 设置错误处理器
     */
    void setErrorHandler(std::shared_ptr<chtl::common::ErrorHandler> errorHandler);
    
    // === 导入语句解析 - 严格按照CHTL语法文档 ===
    
    /**
     * 解析HTML文件导入
     * [Import] @Html from html文件路径 as(可选) 命名为
     */
    bool parseHtmlImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析CSS文件导入
     * [Import] @Style from css文件路径 as(可选) 命名为
     */
    bool parseCssImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析JavaScript文件导入
     * [Import] @JavaScript from js文件路径 as(可选) 命名为
     */
    bool parseJavaScriptImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析CHTL文件导入
     * [Import] @Chtl from chtl文件路径
     * 支持通配符：[Import] @Chtl from 具体路径.*
     */
    bool parseChtlImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析CJmod导入
     * [Import] @CJmod from cjmod文件路径
     */
    bool parseCJmodImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析配置导入
     * [Import] @Config / [Import] [Configuration] @Config
     */
    bool parseConfigImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析原始嵌入导入
     * [Import] [Origin] @Html/Style/Javascript 名称 from chtl文件路径
     */
    bool parseOriginImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析自定义导入
     * [Import] [Custom] @Element/Style/Var 名称 from chtl文件路径
     */
    bool parseCustomImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析模板导入
     * [Import] [Template] @Element/Style/Var 名称 from chtl文件路径
     */
    bool parseTemplateImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 解析批量导入
     * [Import] [Template/Custom/Origin] from chtl文件路径
     */
    bool parseBatchImport(const std::string& statement, EnhancedImportStatement& result);
    
    /**
     * 主解析函数 - 自动识别导入类型
     */
    bool parseImportStatement(const std::string& statement, EnhancedImportStatement& result);
    
    // === 路径处理规则 - 严格按照您的要求 ===
    
    /**
     * @Html、@Style、@JavaScript导入路径处理
     * - 无as语法时直接跳过
     * - 有as语法时，创建对应类型的带名原始嵌入节点
     * - 文件名（不带后缀）：在编译文件所在目录按类型搜索
     * - 具体文件名（带后缀）：直接搜索该文件
     * - 路径为文件夹时触发报错
     */
    PathResolutionRule resolveStaticFileImportPath(const EnhancedImportStatement& import);
    
    /**
     * @Chtl导入路径处理
     * - 名称（不带后缀）：优先官方模块目录→当前目录module文件夹→当前目录，优先匹配cmod
     * - 具体名称（带后缀）：按顺序搜索指定文件
     * - 具体路径（含文件信息）：直接按路径查找
     * - 具体路径（不含文件信息）：触发报错
     */
    PathResolutionRule resolveChtlImportPath(const EnhancedImportStatement& import);
    
    /**
     * @CJmod导入路径处理
     * - 名称（不带后缀）：优先官方模块目录→当前目录module文件夹→当前目录，仅匹配cjmod
     * - 具体名称（带后缀）：按顺序搜索指定文件
     * - 具体路径（含文件信息）：直接按路径查找
     * - 具体路径（不含文件信息）：触发报错
     */
    PathResolutionRule resolveCJmodImportPath(const EnhancedImportStatement& import);
    
    /**
     * 通配符导入支持
     * [Import] @Chtl from 具体路径.* / 具体路径/*
     * [Import] @Chtl from 具体路径.*.cmod / 具体路径/*.cmod
     * [Import] @Chtl from 具体路径.*.chtl / 具体路径/*.chtl
     */
    std::vector<PathResolutionRule> resolveWildcardImport(const EnhancedImportStatement& import);
    
    /**
     * 子模块导入支持
     * [Import] @Chtl from Chtholly.* / Chtholly/*
     * [Import] @Chtl from Chtholly.Space / Chtholly/Space
     */
    PathResolutionRule resolveSubmoduleImport(const EnhancedImportStatement& import);
    
    // === 依赖管理 ===
    
    /**
     * 添加导入语句
     */
    bool addImport(const EnhancedImportStatement& import);
    
    /**
     * 检查循环依赖
     */
    bool checkCircularDependencies();
    
    /**
     * 检查重复导入
     */
    bool checkDuplicateImports();
    
    /**
     * 解析所有依赖
     */
    bool resolveAllDependencies();
    
    /**
     * 获取解析结果
     */
    const std::vector<EnhancedImportStatement>& getResolvedImports() const;
    
    /**
     * 获取错误信息
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * 清理状态
     */
    void clear();

private:
    std::shared_ptr<common::ModulePathManager> pathManager_;
    std::shared_ptr<chtl::common::ErrorHandler> errorHandler_;
    
    std::vector<EnhancedImportStatement> imports_;
    std::vector<EnhancedImportStatement> resolvedImports_;
    
    CircularDependencyDetector circularDetector_;
    DuplicateImportDetector duplicateDetector_;
    
    std::vector<std::string> errors_;
    
    // 私有辅助方法
    bool extractFromClause(const std::string& statement, std::string& path);
    bool extractAsClause(const std::string& statement, std::string& alias);
    bool isValidPath(const std::string& path);
    std::string normalizePathSeparators(const std::string& path);
    std::vector<std::string> expandWildcardPath(const std::string& wildcardPath);
    bool fileExists(const std::string& path);
    std::string getCurrentDirectory();
    std::string getOfficialModuleDirectory();
    std::string getCurrentModuleDirectory();
    
    // 错误报告
    void reportError(const std::string& message, int line = 0, int column = 0);
    void reportPathError(const std::string& path, const std::string& context);
};

} // namespace import
} // namespace chtl