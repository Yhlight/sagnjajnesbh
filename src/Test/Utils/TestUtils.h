#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <random>
#include <chrono>
#include <functional>
#include <filesystem>
#include <thread>

namespace chtl {
namespace test {
namespace utils {

/**
 * 测试数据生成器
 */
class TestDataGenerator {
public:
    static TestDataGenerator& getInstance();
    
    // 字符串生成
    std::string generateRandomString(size_t length);
    std::string generateValidIdentifier();
    std::string generateInvalidIdentifier();
    
    // 数值生成
    int generateRandomInt(int min = 0, int max = 100);
    double generateRandomDouble(double min = 0.0, double max = 100.0);
    
    // CHTL特定数据生成
    std::string generateCHTLComponent();
    std::string generateCSSSelector();
    std::string generateCHTLJSFunction();
    std::vector<std::string> generateNamespaceHierarchy();
    
    // 文件路径生成
    std::string generateModulePath();
    std::string generateFilePath();
    
private:
    std::mt19937 rng_;
    std::vector<std::string> componentNames_;
    std::vector<std::string> selectorTypes_;
    std::vector<std::string> functionNames_;
    
    TestDataGenerator();
};

/**
 * 文件系统测试工具
 */
class FileSystemTestUtils {
public:
    /**
     * 创建临时目录
     */
    static std::string createTempDirectory();
    
    /**
     * 创建临时文件
     */
    static std::string createTempFile(const std::string& content = "");
    
    /**
     * 清理临时文件
     */
    static void cleanupTempFiles();
    
    /**
     * 创建测试用CMOD结构
     */
    static std::string createTestCMOD(const std::string& moduleName, 
                                     const std::string& content = "");
    
    /**
     * 创建测试用CJMOD结构
     */
    static std::string createTestCJMOD(const std::string& moduleName,
                                      const std::string& content = "");
    
    /**
     * 验证文件存在
     */
    static bool fileExists(const std::string& path);
    
    /**
     * 读取文件内容
     */
    static std::string readFile(const std::string& path);
    
    /**
     * 写入文件内容
     */
    static bool writeFile(const std::string& path, const std::string& content);

private:
    static std::vector<std::string> tempFiles_;
    static std::vector<std::string> tempDirectories_;
};

/**
 * 性能测试工具
 */
class PerformanceTestUtils {
public:
    /**
     * 性能计时器
     */
    class Timer {
    public:
        Timer();
        void start();
        void stop();
        void reset();
        std::chrono::milliseconds elapsed() const;
        
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
        std::chrono::high_resolution_clock::time_point endTime_;
        bool running_;
    };
    
    /**
     * 基准测试
     */
    static std::chrono::milliseconds benchmark(std::function<void()> func, int iterations = 1000);
    
    /**
     * 内存使用测试
     */
    static size_t measureMemoryUsage(std::function<void()> func);
    
    /**
     * 性能比较
     */
    static double comparePerformance(std::function<void()> func1, std::function<void()> func2, 
                                   int iterations = 1000);

private:
    static size_t getCurrentMemoryUsage();
};

/**
 * 字符串测试工具
 */
class StringTestUtils {
public:
    /**
     * 比较字符串（忽略空白）
     */
    static bool compareIgnoreWhitespace(const std::string& str1, const std::string& str2);
    
    /**
     * 比较多行字符串
     */
    static bool compareMultiline(const std::string& str1, const std::string& str2);
    
    /**
     * 查找字符串差异
     */
    static std::vector<std::string> findDifferences(const std::string& expected, 
                                                   const std::string& actual);
    
    /**
     * 格式化字符串用于显示
     */
    static std::string formatForDisplay(const std::string& str, size_t maxLength = 100);
    
    /**
     * 转义特殊字符
     */
    static std::string escapeString(const std::string& str);
};

/**
 * CHTL特定测试工具
 */
class CHTLTestUtils {
public:
    /**
     * 验证CHTL语法
     */
    static bool validateCHTLSyntax(const std::string& code);
    
    /**
     * 验证组件结构
     */
    static bool validateComponentStructure(const std::string& componentCode);
    
    /**
     * 验证CHTL JS语法
     */
    static bool validateCHTLJSSyntax(const std::string& chtlJSCode);
    
    /**
     * 验证导入语句
     */
    static bool validateImportStatement(const std::string& importStatement);
    
    /**
     * 验证命名空间
     */
    static bool validateNamespace(const std::string& namespaceName);
    
    /**
     * 创建测试用CHTL代码
     */
    static std::string createTestCHTLCode(const std::string& componentName);
    
    /**
     * 解析CHTL段落
     */
    static std::map<std::string, std::string> parseCHTLSections(const std::string& code);
    
    /**
     * 验证CMOD结构
     */
    static bool validateCMODStructure(const std::string& cmodPath);
    
    /**
     * 验证CJMOD结构
     */
    static bool validateCJMODStructure(const std::string& cjmodPath);
};

/**
 * 模拟对象工具
 */
class MockUtils {
public:
    /**
     * 创建模拟文件系统
     */
    static std::shared_ptr<class MockFileSystem> createMockFileSystem();
    
    /**
     * 创建模拟编译器
     */
    static std::shared_ptr<class MockCompiler> createMockCompiler();
    
    /**
     * 创建模拟模块管理器
     */
    static std::shared_ptr<class MockModuleManager> createMockModuleManager();
};

} // namespace utils
} // namespace test
} // namespace chtl