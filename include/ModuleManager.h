#pragma once

#include "CHTLCommon.h"
#include "ImportManager.h"
#include "NamespaceManager.h"
#include "SimpleZip.h"
#include <memory>

namespace chtl {

// 模块类型
enum class ModuleType {
    CHTL_MODULE,    // CHTL模块
    CMOD_MODULE,    // CMOD模块
    CJMOD_MODULE,   // CJMOD模块
    HTML_FILE,      // HTML文件
    CSS_FILE,       // CSS文件
    JS_FILE         // JavaScript文件
};

// 模块加载结果
struct ModuleLoadResult {
    bool success = false;
    String error;
    ModuleType type;
    String modulePath;
    String moduleAlias;
    std::shared_ptr<CHTLASTNode> ast = nullptr;
    std::vector<std::shared_ptr<CHTLASTNode>> astNodes; // 用于多文件模块
};

// 模块打包结果
struct ModulePackResult {
    bool success = false;
    String error;
    String outputPath;
    size_t fileCount = 0;
};

// 综合模块管理器
class ModuleManager {
public:
    ModuleManager(const String& currentDirectory, const String& officialModulePath = "./modules");
    ~ModuleManager() = default;
    
    // 导入功能
    ModuleLoadResult loadModule(const ImportNode& importNode);
    std::vector<ModuleLoadResult> loadMultipleModules(const std::vector<ImportNode>& importNodes);
    
    // CMOD功能
    ModulePackResult packCmod(const String& sourceDir, const String& outputPath = "");
    bool unpackCmod(const String& cmodPath, const String& outputDir = "");
    
    // CJMOD功能  
    ModulePackResult packCJmod(const String& sourceDir, const String& outputPath = "");
    bool unpackCJmod(const String& cjmodPath, const String& outputDir = "");
    
    // 命名空间管理
    bool registerNamespace(const String& name, std::shared_ptr<Namespace> ns);
    std::shared_ptr<Namespace> getNamespace(const String& name);
    bool mergeNamespaces(const String& targetName, const String& sourceName);
    std::vector<NamespaceConflict> detectNamespaceConflicts(const String& name);
    
    // 官方模块前缀支持
    bool isOfficialModule(const String& moduleName);
    String resolveOfficialModulePath(const String& moduleName);
    
    // 模块搜索和发现
    std::vector<String> discoverModules(const String& searchPath = "");
    std::vector<String> discoverCJmodules(const String& searchPath = "");
    
    // 依赖管理
    bool checkCircularDependency(const String& fromModule, const String& toModule);
    std::vector<String> getDependencyChain(const String& module);
    void clearDependencyCache();
    
    // 配置
    void setOfficialModulePath(const String& path);
    void setCurrentDirectory(const String& directory);
    String getCurrentDirectory() const { return currentDirectory_; }
    String getOfficialModulePath() const { return officialModulePath_; }
    
    // 统计信息
    struct ModuleStats {
        size_t loadedModules = 0;
        size_t cachedImports = 0;
        size_t namespacesRegistered = 0;
        size_t circularDependenciesDetected = 0;
    };
    
    ModuleStats getStats() const;
    void resetStats();

private:
    String currentDirectory_;
    String officialModulePath_;
    
    // 核心组件
    std::unique_ptr<ImportManager> importManager_;
    std::unique_ptr<NamespaceManager> namespaceManager_;
    
    // 模块缓存
    std::unordered_map<String, std::shared_ptr<CHTLASTNode>> moduleCache_;
    std::unordered_map<String, std::shared_ptr<Namespace>> namespaceCache_;
    
    // 统计信息
    mutable ModuleStats stats_;
    
    // 内部方法
    ModuleType detectModuleType(const String& path);
    String generateOutputPath(const String& sourceDir, ModuleType type);
    bool validateModuleStructure(const String& sourceDir, ModuleType type);
    
    // CMOD/CJMOD处理
    ModuleLoadResult loadCmodModule(const String& cmodPath, const String& alias);
    ModuleLoadResult loadCJmodModule(const String& cjmodPath, const String& alias);
    
    // 子模块处理
    std::vector<String> parseSubmodulePath(const String& path);
    ModuleLoadResult loadSubmodule(const String& moduleName, const String& submoduleName);
    
    // 错误处理
    void reportError(const String& error);
    void reportWarning(const String& warning);
    
    std::vector<String> errors_;
    std::vector<String> warnings_;
};

// 模块管理器工厂
class ModuleManagerFactory {
public:
    static std::unique_ptr<ModuleManager> createManager(const String& currentDirectory, const String& officialModulePath = "");
    static std::unique_ptr<ModuleManager> createWithDefaults();
};

} // namespace chtl