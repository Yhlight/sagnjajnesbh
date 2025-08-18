#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace chtl {
namespace test {
namespace mocks {

/**
 * 模拟文件系统
 * 用于测试文件操作而不实际操作文件系统
 */
class MockFileSystem {
public:
    MockFileSystem();
    ~MockFileSystem() = default;
    
    // 文件操作
    bool exists(const std::string& path) const;
    bool createFile(const std::string& path, const std::string& content = "");
    bool createDirectory(const std::string& path);
    bool deleteFile(const std::string& path);
    bool deleteDirectory(const std::string& path);
    
    // 内容操作
    std::string readFile(const std::string& path) const;
    bool writeFile(const std::string& path, const std::string& content);
    bool appendFile(const std::string& path, const std::string& content);
    
    // 目录操作
    std::vector<std::string> listDirectory(const std::string& path) const;
    std::vector<std::string> findFiles(const std::string& pattern) const;
    
    // 路径操作
    std::string getParentPath(const std::string& path) const;
    std::string getFileName(const std::string& path) const;
    std::string getExtension(const std::string& path) const;
    
    // 测试辅助
    void clear();
    void addPredefinedStructure();
    size_t getFileCount() const;
    size_t getDirectoryCount() const;
    
    // 统计信息
    int getReadCount() const { return readCount_; }
    int getWriteCount() const { return writeCount_; }
    void resetCounters();

private:
    struct FileInfo {
        std::string content;
        bool isDirectory;
        std::chrono::system_clock::time_point lastModified;
        
        FileInfo(const std::string& c = "", bool isDir = false) 
            : content(c), isDirectory(isDir), lastModified(std::chrono::system_clock::now()) {}
    };
    
    std::map<std::string, FileInfo> files_;
    mutable int readCount_;
    mutable int writeCount_;
    
    std::string normalizePath(const std::string& path) const;
    bool isValidPath(const std::string& path) const;
};

/**
 * 模拟编译器
 */
class MockCompiler {
public:
    MockCompiler();
    
    // 编译操作
    bool compileCSS(const std::string& css);
    bool compileJavaScript(const std::string& js);
    bool compileCHTL(const std::string& chtl);
    
    // 验证操作
    std::vector<std::string> validateCSS(const std::string& css);
    std::vector<std::string> validateJavaScript(const std::string& js);
    std::vector<std::string> validateCHTL(const std::string& chtl);
    
    // 优化操作
    std::string optimizeCSS(const std::string& css);
    std::string optimizeJavaScript(const std::string& js);
    
    // 配置
    void setStrictMode(bool strict);
    void setOptimizationLevel(int level);
    
    // 统计
    int getCompilationCount() const { return compilationCount_; }
    int getValidationCount() const { return validationCount_; }
    void resetCounters();

private:
    bool strictMode_;
    int optimizationLevel_;
    mutable int compilationCount_;
    mutable int validationCount_;
    
    bool simulateCompilation(const std::string& code);
    std::vector<std::string> simulateValidation(const std::string& code);
};

/**
 * 模拟模块管理器
 */
class MockModuleManager {
public:
    MockModuleManager();
    
    // 模块操作
    bool loadModule(const std::string& moduleName);
    bool unloadModule(const std::string& moduleName);
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // 模块信息
    std::vector<std::string> getLoadedModules() const;
    std::vector<std::string> getAvailableModules() const;
    std::string getModuleInfo(const std::string& moduleName) const;
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& query) const;
    std::string resolveModulePath(const std::string& moduleName) const;
    
    // 依赖管理
    std::vector<std::string> getModuleDependencies(const std::string& moduleName) const;
    bool checkDependencies(const std::string& moduleName) const;
    
    // 测试辅助
    void addMockModule(const std::string& name, const std::string& info);
    void removeMockModule(const std::string& name);
    void clear();

private:
    std::map<std::string, std::string> availableModules_;
    std::vector<std::string> loadedModules_;
    std::map<std::string, std::vector<std::string>> moduleDependencies_;
};

} // namespace mocks
} // namespace test
} // namespace chtl