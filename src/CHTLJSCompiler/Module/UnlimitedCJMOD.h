#pragma once
#include "../../Common/SimpleZip/SimpleZip.h"
#include "../../Common/Core/ErrorHandler.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace chtl {
namespace unlimited_cjmod {

/**
 * 无限制CJMOD框架
 * 设计理念：高自由度 + 简单易用 + 任意扩展
 * JS的上限决定了CHTL JS的上限，CJMOD要做到比JS更自由
 */

/**
 * 自由C++代码块
 * 允许开发者写任意C++代码，包括全局变量、任意头文件、任意语法
 */
struct FreeCppBlock {
    std::string blockType;          // "headers", "globals", "functions", "classes", "custom"
    std::string code;               // 完全自由的C++代码
    int priority = 0;               // 代码块优先级（影响生成顺序）
    std::string description;        // 代码块描述
    
    FreeCppBlock() = default;
    FreeCppBlock(const std::string& type, const std::string& c, int p = 0, const std::string& desc = "")
        : blockType(type), code(c), priority(p), description(desc) {}
};

/**
 * CHTL JS语法扩展定义
 * 允许创造任意新语法，上限是JavaScript能做到的一切
 */
struct CHTLJSSyntaxExtension {
    std::string syntaxName;         // 新语法名称，如 "myCustomSyntax"
    std::string syntaxPattern;      // 语法模式，如 "myCustomSyntax({...})"
    std::string cppHandler;         // 对应的C++处理函数名
    std::string jsImplementation;   // 最终生成的JavaScript实现
    std::string documentation;      // 语法文档
    bool isGlobalFunction = false;  // 是否是全局函数
    bool extendsExisting = false;   // 是否扩展现有语法
    
    CHTLJSSyntaxExtension() = default;
};

/**
 * 全局状态管理器
 * 允许CJmod维护任意全局状态，实现复杂的状态管理
 */
class GlobalStateManager {
public:
    GlobalStateManager() = default;
    ~GlobalStateManager() = default;
    
    // 设置全局状态
    void setState(const std::string& key, const std::string& value);
    std::string getState(const std::string& key) const;
    
    // 注册状态变化监听器
    void onStateChange(const std::string& key, std::function<void(const std::string&, const std::string&)> callback);
    
    // 清理状态
    void clearState();
    void clearState(const std::string& key);
    
private:
    std::unordered_map<std::string, std::string> globalState_;
    std::unordered_map<std::string, std::vector<std::function<void(const std::string&, const std::string&)>>> stateListeners_;
};

/**
 * 动态库管理器
 * 支持动态加载任意C++库，实现真正的无限扩展
 */
class DynamicLibraryManager {
public:
    DynamicLibraryManager() = default;
    ~DynamicLibraryManager();
    
    // 动态加载库
    bool loadLibrary(const std::string& libraryPath, const std::string& alias = "");
    bool unloadLibrary(const std::string& alias);
    
    // 获取函数指针
    void* getFunction(const std::string& libraryAlias, const std::string& functionName);
    
    // 调用C++函数（支持任意参数类型）
    template<typename ReturnType, typename... Args>
    ReturnType callFunction(const std::string& libraryAlias, const std::string& functionName, Args... args);
    
    // 获取已加载的库
    std::vector<std::string> getLoadedLibraries() const;
    
private:
    std::unordered_map<std::string, void*> loadedLibraries_;
    
    void* loadDynamicLibrary(const std::string& path);
    bool unloadDynamicLibrary(void* handle);
    void* getSymbolFromLibrary(void* handle, const std::string& symbolName);
};

/**
 * 无限制CJMOD模块信息
 * 支持任意自定义字段和配置
 */
struct UnlimitedCJmodInfo {
    // 基本信息
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    
    // 自由配置
    std::unordered_map<std::string, std::string> customFields;  // 任意自定义字段
    
    // C++编译配置 - 完全自由
    std::string cppStandard = "17";             // C++标准（默认17）
    std::vector<std::string> compilerFlags;     // 任意编译器标志
    std::vector<std::string> linkerFlags;       // 任意链接器标志
    std::vector<std::string> includePaths;      // 任意包含路径
    std::vector<std::string> libraryPaths;      // 任意库路径
    std::vector<std::string> libraries;         // 任意链接库
    
    // 依赖管理
    std::vector<std::string> systemDependencies;  // 系统依赖
    std::vector<std::string> cjmodDependencies;   // 其他CJmod依赖
    
    // 运行时配置
    bool enableGlobalState = true;              // 启用全局状态
    bool enableDynamicLoading = true;           // 启用动态加载
    int maxMemoryUsage = -1;                    // 最大内存使用（-1表示无限制）
    
    UnlimitedCJmodInfo() = default;
};

/**
 * 无限制CJMOD模块
 * 真正的高自由度、无限制扩展框架
 */
struct UnlimitedCJmodModule {
    UnlimitedCJmodInfo info;
    
    // 自由C++代码块 - 按类型和优先级组织
    std::vector<FreeCppBlock> cppBlocks;
    
    // CHTL JS语法扩展
    std::vector<CHTLJSSyntaxExtension> syntaxExtensions;
    
    // 全局状态管理
    std::shared_ptr<GlobalStateManager> stateManager;
    
    // 动态库管理
    std::shared_ptr<DynamicLibraryManager> libraryManager;
    
    // 模块路径
    std::string rootPath;
    std::string infoPath;
    std::string srcPath;
    
    // 验证二同名规则
    bool isValidNaming() const;
    
    // 生成完整的C++源代码
    std::string generateCompleteCppCode() const;
    
    // 生成JavaScript接口代码
    std::string generateJavaScriptInterface() const;
    
    UnlimitedCJmodModule() : stateManager(std::make_shared<GlobalStateManager>()),
                            libraryManager(std::make_shared<DynamicLibraryManager>()) {}
};

/**
 * 无限制CJMOD管理器
 * 实现真正简单、合理、容易上手的C++扩展框架
 */
class UnlimitedCJmodManager {
public:
    UnlimitedCJmodManager();
    ~UnlimitedCJmodManager();
    
    /**
     * 设置错误处理器
     */
    void setErrorHandler(std::shared_ptr<common::ErrorHandler> errorHandler);
    
    // === 简单易用的API设计 ===
    
    /**
     * 创建新的CJMOD项目
     * 一行代码创建完整的项目结构
     */
    bool createCJmodProject(const std::string& projectName, const std::string& outputDir);
    
    /**
     * 添加自由C++代码
     * 支持任意C++代码，包括头文件、全局变量、函数、类
     */
    bool addFreeCppCode(UnlimitedCJmodModule& module, const std::string& blockType, 
                       const std::string& code, int priority = 0, const std::string& description = "");
    
    /**
     * 添加CHTL JS语法扩展
     * 创造全新的CHTL JS语法
     */
    bool addCHTLJSSyntax(UnlimitedCJmodModule& module, const std::string& syntaxName,
                        const std::string& pattern, const std::string& cppHandler,
                        const std::string& jsImplementation);
    
    /**
     * 快速添加全局函数
     * 一行代码添加C++函数到CHTL JS
     */
    bool addGlobalFunction(UnlimitedCJmodModule& module, const std::string& functionName,
                          const std::string& cppImplementation);
    
    /**
     * 添加全局状态变量
     * 维护跨调用的状态
     */
    bool addGlobalState(UnlimitedCJmodModule& module, const std::string& stateName,
                       const std::string& cppType, const std::string& initialValue = "");
    
    // === 高级功能 ===
    
    /**
     * 导入任意C++库
     * 支持系统库、第三方库、自定义库
     */
    bool importCppLibrary(UnlimitedCJmodModule& module, const std::string& libraryName,
                         const std::vector<std::string>& headers = {},
                         const std::vector<std::string>& linkFlags = {});
    
    /**
     * 扩展现有CHTL JS语法
     * 在现有语法基础上添加功能
     */
    bool extendExistingSyntax(UnlimitedCJmodModule& module, const std::string& existingSyntax,
                             const std::string& extensionCode);
    
    /**
     * 创建自定义对象类型
     * 在CHTL JS中创建全新的对象类型
     */
    bool createCustomObjectType(UnlimitedCJmodModule& module, const std::string& typeName,
                               const std::vector<std::string>& methods,
                               const std::vector<std::string>& properties);
    
    // === 模块管理 ===
    
    /**
     * 解析CJMOD模块
     */
    bool parseUnlimitedCJmod(const std::string& modulePath, UnlimitedCJmodModule& module);
    
    /**
     * 编译CJMOD模块
     * 自动处理所有C++编译细节
     */
    bool compileModule(const UnlimitedCJmodModule& module, const std::string& outputPath);
    
    /**
     * 打包CJMOD
     */
    bool packUnlimitedCJmod(const std::string& modulePath, const std::string& outputPath);
    
    /**
     * 安装CJMOD
     */
    bool installUnlimitedCJmod(const std::string& cjmodPath, const std::string& targetDir);
    
    // === 运行时支持 ===
    
    /**
     * 加载CJMOD到CHTL JS运行时
     */
    bool loadCJmodToRuntime(const std::string& modulePath);
    
    /**
     * 卸载CJMOD
     */
    bool unloadCJmodFromRuntime(const std::string& moduleName);
    
    /**
     * 调用CJMOD函数
     */
    std::string callCJmodFunction(const std::string& moduleName, const std::string& functionName,
                                 const std::vector<std::string>& parameters);
    
    // === 开发辅助工具 ===
    
    /**
     * 生成模板代码
     * 为常见使用场景生成模板
     */
    std::string generateTemplate(const std::string& templateType);
    
    /**
     * 验证C++代码语法
     */
    bool validateCppSyntax(const std::string& cppCode);
    
    /**
     * 生成文档
     */
    std::string generateDocumentation(const UnlimitedCJmodModule& module);
    
    /**
     * 获取使用示例
     */
    std::vector<std::string> getUsageExamples();
    
    // === 实用工具 ===
    
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }

private:
    std::shared_ptr<common::ErrorHandler> errorHandler_;
    std::vector<std::string> errors_;
    std::unordered_map<std::string, UnlimitedCJmodModule> loadedModules_;
    
    // 代码生成辅助
    std::string generateCppHeader(const UnlimitedCJmodModule& module) const;
    std::string generateCppSource(const UnlimitedCJmodModule& module) const;
    std::string generateJSInterface(const UnlimitedCJmodModule& module) const;
    std::string generateCMakeFile(const UnlimitedCJmodModule& module) const;
    
    // 编译辅助
    bool compileWithGCC(const std::string& sourceFile, const std::string& outputFile,
                       const std::vector<std::string>& flags);
    bool compileWithMSVC(const std::string& sourceFile, const std::string& outputFile,
                        const std::vector<std::string>& flags);
    bool compileWithClang(const std::string& sourceFile, const std::string& outputFile,
                         const std::vector<std::string>& flags);
    
    // 模板生成辅助
    std::string getBasicTemplate() const;
    std::string getAdvancedTemplate() const;
    std::string getGameEngineTemplate() const;
    std::string getNetworkingTemplate() const;
    std::string getImageProcessingTemplate() const;
    
    // 错误报告
    void reportError(const std::string& message, const std::string& context = "");
    void reportWarning(const std::string& message, const std::string& context = "");
    
    // 文件操作
    bool readFileContent(const std::string& filePath, std::string& content);
    bool writeFileContent(const std::string& filePath, const std::string& content);
    std::string joinPath(const std::string& base, const std::string& relative);
    bool createDirectory(const std::string& dirPath);
};

// === 模板函数实现 ===

template<typename ReturnType, typename... Args>
ReturnType DynamicLibraryManager::callFunction(const std::string& libraryAlias, 
                                              const std::string& functionName, Args... args) {
    void* funcPtr = getFunction(libraryAlias, functionName);
    if (!funcPtr) {
        throw std::runtime_error("函数未找到: " + functionName);
    }
    
    typedef ReturnType (*FuncType)(Args...);
    FuncType func = reinterpret_cast<FuncType>(funcPtr);
    
    return func(args...);
}

} // namespace unlimited_cjmod
} // namespace chtl