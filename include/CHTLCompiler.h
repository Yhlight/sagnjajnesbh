#pragma once

#include "CHTLCommon.h"
#include "CHTLParser.h"
#include "CHTLGenerator.h"
#include "ImportManager.h"
#include "NamespaceManager.h"
#include "ModuleSystem.h"
#include <memory>
#include <vector>
#include <functional>

namespace chtl {

// 编译选项
struct CompilerConfig {
    String inputFile;
    String outputFile;
    String officialModulePath = "module"; // 官方模块路径
    String currentDirectory = ".";        // 当前工作目录
    bool enableImportSystem = true;       // 启用导入系统
    bool enableNamespaceSystem = true;    // 启用命名空间系统
    bool enableModuleSystem = true;       // 启用模块系统
    bool strictMode = false;              // 严格模式
    bool debugMode = false;               // 调试模式
    bool generateSourceMap = false;       // 生成源码映射
    
    CompilerConfig() = default;
};

// 编译结果
struct CompilationResult {
    bool success = false;
    String generatedCode;
    std::vector<String> errors;
    std::vector<String> warnings;
    std::vector<String> importedFiles;    // 导入的文件列表
    std::vector<String> usedNamespaces;   // 使用的命名空间列表
    std::vector<String> loadedModules;    // 加载的模块列表
    
    void addError(const String& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const String& warning) {
        warnings.push_back(warning);
    }
    
    void mergeFrom(const CompilationResult& other) {
        errors.insert(errors.end(), other.errors.begin(), other.errors.end());
        warnings.insert(warnings.end(), other.warnings.begin(), other.warnings.end());
        importedFiles.insert(importedFiles.end(), other.importedFiles.begin(), other.importedFiles.end());
        usedNamespaces.insert(usedNamespaces.end(), other.usedNamespaces.begin(), other.usedNamespaces.end());
        loadedModules.insert(loadedModules.end(), other.loadedModules.begin(), other.loadedModules.end());
        
        if (!other.success) {
            success = false;
        }
    }
};

// 统一的CHTL编译器
class CHTLCompiler {
public:
    CHTLCompiler();
    explicit CHTLCompiler(const CompilerConfig& config);
    ~CHTLCompiler() = default;
    
    // 主要编译接口
    CompilationResult compile(const String& sourceCode, const String& filename = "");
    CompilationResult compileFile(const String& inputFile, const String& outputFile = "");
    CompilationResult compileBatch(const std::vector<String>& inputFiles, const String& outputDirectory = "");
    
    // 配置管理
    void setConfig(const CompilerConfig& config);
    const CompilerConfig& getConfig() const { return config_; }
    
    // 系统配置
    void setOfficialModulePath(const String& path);
    void setCurrentDirectory(const String& directory);
    
    // 模块操作
    bool installModule(const String& modulePath, const String& installDirectory = "");
    bool uninstallModule(const String& moduleName);
    std::vector<ModuleInfo> listInstalledModules();
    
    // Cmod操作
    bool packCmod(const String& sourceDirectory, const String& outputPath);
    bool unpackCmod(const String& cmodPath, const String& outputDirectory);
    
    // CJmod操作
    bool packCJmod(const String& sourceDirectory, const String& outputPath);
    bool unpackCJmod(const String& cjmodPath, const String& outputDirectory);
    
    // 命名空间操作
    std::vector<String> listAvailableNamespaces() const;
    std::vector<String> listNamespaceItems(const String& namespaceName) const;
    
    // 导入缓存管理
    void clearImportCache();
    size_t getImportCacheSize() const;
    
    // 模块缓存管理
    void clearModuleCache();
    size_t getModuleCacheSize() const;
    
    // 错误和警告
    std::vector<String> getLastErrors() const { return lastErrors_; }
    std::vector<String> getLastWarnings() const { return lastWarnings_; }
    void clearErrors() { lastErrors_.clear(); lastWarnings_.clear(); }
    
    // 统计信息
    struct Statistics {
        size_t compiledFiles = 0;
        size_t importedFiles = 0;
        size_t loadedModules = 0;
        size_t namespacesUsed = 0;
        size_t errorsCount = 0;
        size_t warningsCount = 0;
        double totalCompileTime = 0.0; // 秒
    };
    
    Statistics getStatistics() const { return statistics_; }
    void resetStatistics() { statistics_ = Statistics{}; }

private:
    CompilerConfig config_;
    
    // 核心组件
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLGenerator> generator_;
    std::unique_ptr<ImportManager> importManager_;
    std::unique_ptr<NamespaceManager> namespaceManager_;
    std::unique_ptr<ModuleManager> moduleManager_;
    
    // 状态管理
    std::vector<String> lastErrors_;
    std::vector<String> lastWarnings_;
    Statistics statistics_;
    
    // 内部方法
    bool initializeSystems();
    CompilationResult processAST(std::shared_ptr<CHTLASTNode> ast, const String& filename);
    void processImports(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result, const String& currentFile);
    void processNamespaces(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result, const String& currentFile);
    void collectErrors();
    void updateStatistics(const CompilationResult& result);
    
    // AST遍历和处理
    void traverseAST(std::shared_ptr<CHTLASTNode> node, 
                     std::function<void(std::shared_ptr<CHTLASTNode>)> callback);
    void replaceImportNodes(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result, const String& currentFile);
    void resolveNamespaceReferences(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result);
    
    // 辅助方法
    String resolveRelativePath(const String& basePath, const String& relativePath);
    bool isAbsolutePath(const String& path);
    String getCurrentWorkingDirectory();
};

// 便利函数
namespace compiler_utils {
    // 快速编译单个文件
    CompilationResult compileFile(const String& inputFile, const String& outputFile = "", 
                                 const CompilerConfig& config = CompilerConfig{});
    
    // 快速编译字符串
    CompilationResult compileString(const String& sourceCode, const String& filename = "",
                                   const CompilerConfig& config = CompilerConfig{});
    
    // 批量编译
    std::vector<CompilationResult> compileBatch(const std::vector<String>& inputFiles,
                                               const String& outputDirectory = "",
                                               const CompilerConfig& config = CompilerConfig{});
    
    // 验证CHTL语法
    bool validateSyntax(const String& sourceCode, std::vector<String>& errors);
    
    // 格式化CHTL代码
    String formatCode(const String& sourceCode, const CompilerConfig& config = CompilerConfig{});
    
    // 获取编译器版本信息
    String getCompilerVersion();
    String getCompilerInfo();
}

} // namespace chtl