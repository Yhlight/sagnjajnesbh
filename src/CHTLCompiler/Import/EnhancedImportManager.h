#pragma once
#include "../../Common/StateManager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace chtl {
namespace import {

/**
 * 增强的导入管理器
 * 严格按照CHTL语法文档和您的详细要求实现
 */
class EnhancedImportManager {
public:
    enum class ImportType {
        HTML,           // @Html导入
        STYLE,          // @Style导入
        JAVASCRIPT,     // @JavaScript导入
        CHTL,           // @Chtl导入
        CJMOD           // @CJmod导入
    };
    
    struct ImportRule {
        ImportType type;
        std::string moduleName;
        std::string alias;              // as语法的别名
        std::string path;               // 导入路径
        bool hasAs = false;             // 是否有as语法
        bool isWildcard = false;        // 是否为通配符导入
        std::string wildcardPattern;    // 通配符模式
        bool isSubmodule = false;       // 是否为子模块导入
        std::string submodulePath;      // 子模块路径
    };
    
    struct SearchPath {
        std::string officialModuleDir;  // 官方模块目录
        std::string currentModuleDir;   // 当前目录module文件夹
        std::string currentDir;         // 当前目录
        std::string compilingFileDir;   // 编译文件所在目录
    };
    
    EnhancedImportManager();
    ~EnhancedImportManager();
    
    // === 设置和初始化 ===
    
    /**
     * 设置搜索路径
     */
    void setSearchPaths(const SearchPath& paths);
    
    /**
     * 设置当前编译文件路径
     */
    void setCurrentCompilingFile(const std::string& filePath);
    
    // === 导入处理 ===
    
    /**
     * 解析导入语句
     * 严格按照您的要求处理各类导入语法
     */
    bool parseImportStatement(const std::string& statement, ImportRule& rule);
    
    /**
     * 处理@Html、@Style、@JavaScript导入
     * - 无as语法时直接跳过
     * - 有as语法时，创建对应类型的带名原始嵌入节点
     */
    bool processResourceImport(const ImportRule& rule);
    
    /**
     * 处理@Chtl导入
     * - 优先搜索官方模块目录
     * - 其次搜索当前目录module文件夹
     * - 最后搜索当前目录
     * - 均优先匹配cmod文件
     */
    bool processChtlImport(const ImportRule& rule);
    
    /**
     * 处理@CJmod导入
     * - 采用与Cmod相同的路径搜索策略
     * - 仅匹配cjmod文件
     */
    bool processCJmodImport(const ImportRule& rule);
    
    // === 通配符支持 ===
    
    /**
     * 处理通配符导入
     * [Import] @Chtl from 具体路径.*
     * [Import] @Chtl from 具体路径/*
     */
    std::vector<std::string> resolveWildcardImport(const ImportRule& rule);
    
    /**
     * 处理子模块导入
     * [Import] @Chtl from Chtholly.*
     * [Import] @Chtl from Chtholly.Space
     * 支持使用'/'替代'.'作为路径分隔符
     */
    std::vector<std::string> resolveSubmoduleImport(const ImportRule& rule);
    
    // === 路径处理 ===
    
    /**
     * 解析路径类型
     * - 文件名（不带后缀）
     * - 具体文件名（带后缀）
     * - 具体路径（含文件信息）
     * - 具体路径（不含文件信息）- 触发报错
     */
    bool analyzePath(const std::string& path, std::string& pathType, std::string& fileName, std::string& extension);
    
    /**
     * 搜索文件
     * 按照您要求的优先级顺序搜索
     */
    std::string searchFile(const std::string& fileName, const std::string& extension, ImportType type);
    
    /**
     * 验证文件存在性
     */
    bool validateFilePath(const std::string& path);
    
    // === 循环依赖和重复导入处理 ===
    
    /**
     * 检测循环依赖
     * 修正同一路径多表达方式问题
     */
    bool detectCircularDependency(const std::string& moduleName);
    
    /**
     * 检测重复导入
     */
    bool detectDuplicateImport(const ImportRule& rule);
    
    /**
     * 规范化路径
     * 修正同一路径多表达方式问题
     */
    std::string normalizePath(const std::string& path);
    
    // === 官方模块支持 ===
    
    /**
     * 处理官方模块前缀"chtl::"
     * [Import] @Chtl from chtl::模块名
     */
    bool processOfficialModuleImport(const ImportRule& rule);
    
    /**
     * 检查是否为官方模块引用
     */
    bool isOfficialModuleReference(const std::string& moduleName);
    
    /**
     * 解析官方模块名称
     */
    std::string parseOfficialModuleName(const std::string& reference);
    
    // === 错误处理 ===
    
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    void clearErrors();
    void addError(const std::string& message);
    
    // === 状态管理集成 ===
    
    /**
     * 设置状态管理器
     */
    void setStateManager(std::shared_ptr<state::RAIIStateManager> stateManager);
    
    /**
     * 获取导入统计信息
     */
    std::unordered_map<std::string, int> getImportStatistics() const;

private:
    SearchPath searchPaths_;
    std::string currentCompilingFile_;
    std::vector<ImportRule> importRules_;
    std::unordered_set<std::string> importedModules_;
    std::unordered_set<std::string> normalizedPaths_;  // 用于检测重复导入
    std::vector<std::string> dependencyChain_;         // 用于检测循环依赖
    std::vector<std::string> errors_;
    
    std::shared_ptr<state::RAIIStateManager> stateManager_;
    
    // 私有方法
    bool isFileNameOnly(const std::string& path);
    bool isSpecificFileName(const std::string& path);
    bool isSpecificPathWithFile(const std::string& path);
    bool isSpecificPathWithoutFile(const std::string& path);
    
    std::string getFileExtension(const std::string& fileName);
    std::string getFileNameWithoutExtension(const std::string& fileName);
    std::string getDirectoryPath(const std::string& filePath);
    
    std::vector<std::string> searchInDirectory(const std::string& directory, const std::string& pattern);
    std::vector<std::string> expandWildcard(const std::string& pattern, const std::string& directory);
    
    void addToImportedModules(const std::string& moduleName);
    void addToDependencyChain(const std::string& moduleName);
    void removeFromDependencyChain(const std::string& moduleName);
};

} // namespace import
} // namespace chtl