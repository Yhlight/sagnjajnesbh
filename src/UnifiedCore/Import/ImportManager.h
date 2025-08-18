#pragma once
#include "../../common/GlobalMap.h"
#include "../../common/ModulePathManager.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace chtl {
namespace import {

/**
 * 导入语句信息
 */
struct ImportStatement {
    std::string moduleName;
    std::string alias;
    std::vector<std::string> specificImports;  // 特定导入项
    bool isWildcard = false;  // 是否为通配符导入
    int lineNumber = 0;
    int columnNumber = 0;
};

/**
 * 模块依赖信息
 */
struct ModuleDependency {
    std::string name;
    std::string version;
    std::string path;
    common::ModulePathManager::ModuleType type;
    bool isResolved = false;
    std::vector<std::string> exports;
};

/**
 * 导入管理器
 * 负责处理CHTL中的模块导入语句，支持CMOD和CJMOD
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
     * 解析导入语句
     */
    bool parseImportStatement(const std::string& statement, ImportStatement& result);
    
    /**
     * 添加导入语句
     */
    bool addImport(const ImportStatement& import);
    
    /**
     * 解析所有导入依赖
     */
    bool resolveDependencies();
    
    /**
     * 获取已解析的模块依赖
     */
    const std::vector<ModuleDependency>& getDependencies() const { return dependencies_; }
    
    /**
     * 获取特定模块的导出项
     */
    std::vector<std::string> getModuleExports(const std::string& moduleName) const;
    
    /**
     * 检查符号是否可用（已导入）
     */
    bool isSymbolAvailable(const std::string& symbol) const;
    
    /**
     * 获取符号来源模块
     */
    std::string getSymbolSource(const std::string& symbol) const;
    
    /**
     * 验证所有导入
     */
    std::vector<std::string> validateImports();
    
    /**
     * 获取导入的命名空间列表
     */
    std::vector<std::string> getImportedNamespaces() const;
    
    /**
     * 检查循环依赖
     */
    bool hasCircularDependency() const;
    
    /**
     * 获取依赖图
     */
    std::unordered_map<std::string, std::vector<std::string>> getDependencyGraph() const;
    
    /**
     * 清空所有导入
     */
    void clear();
    
    /**
     * 获取导入统计信息
     */
    struct ImportStatistics {
        int totalImports = 0;
        int cmodImports = 0;
        int cjmodImports = 0;
        int resolvedImports = 0;
        int unresolvedImports = 0;
    };
    ImportStatistics getStatistics() const;
    
private:
    std::vector<ImportStatement> imports_;
    std::vector<ModuleDependency> dependencies_;
    std::unordered_map<std::string, std::string> symbolToModule_;  // symbol -> module name
    std::unordered_set<std::string> availableSymbols_;
    std::shared_ptr<common::ModulePathManager> pathManager_;
    
    /**
     * 解析单个模块依赖
     */
    bool resolveModule(const std::string& moduleName, ModuleDependency& dependency);
    
    /**
     * 加载CMOD模块
     */
    bool loadCMODModule(const std::string& path, ModuleDependency& dependency);
    
    /**
     * 加载CJMOD模块
     */
    bool loadCJMODModule(const std::string& path, ModuleDependency& dependency);
    
    /**
     * 解析模块info文件
     */
    bool parseModuleInfo(const std::string& infoPath, ModuleDependency& dependency);
    
    /**
     * 构建符号映射
     */
    void buildSymbolMapping();
    
    /**
     * 检测循环依赖的递归函数
     */
    bool detectCircularDependency(
        const std::string& module, 
        std::unordered_set<std::string>& visiting,
        std::unordered_set<std::string>& visited
    ) const;
    
    /**
     * 规范化模块名称
     */
    std::string normalizeModuleName(const std::string& name) const;
    
    /**
     * 解析导出字符串
     */
    std::vector<std::string> parseExports(const std::string& exportString) const;
};

} // namespace import
} // namespace chtl