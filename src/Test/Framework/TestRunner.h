#pragma once
#include "TestBase.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace chtl {
namespace test {
namespace framework {

/**
 * 测试套件注册器
 */
class TestSuiteRegistry {
public:
    using TestSuiteFactory = std::function<std::unique_ptr<TestBase>()>;
    
    static TestSuiteRegistry& getInstance();
    
    void registerSuite(const std::string& name, TestSuiteFactory factory);
    std::unique_ptr<TestBase> createSuite(const std::string& name);
    std::vector<std::string> getRegisteredSuites() const;
    
private:
    std::map<std::string, TestSuiteFactory> suites_;
};

/**
 * 测试运行器
 * 管理和执行多个测试套件
 */
class TestRunner {
public:
    TestRunner();
    ~TestRunner() = default;
    
    // === 测试套件管理 ===
    
    /**
     * 添加测试套件
     */
    void addSuite(std::unique_ptr<TestBase> suite);
    
    /**
     * 通过名称添加测试套件
     */
    void addSuite(const std::string& suiteName);
    
    /**
     * 运行所有测试套件
     */
    TestStatistics runAllSuites();
    
    /**
     * 运行指定的测试套件
     */
    TestStatistics runSuite(const std::string& suiteName);
    
    /**
     * 运行指定分类的测试
     */
    TestStatistics runCategory(const std::string& category);
    
    // === 配置选项 ===
    
    /**
     * 设置详细输出
     */
    void setVerbose(bool verbose);
    
    /**
     * 设置并行执行
     */
    void setParallel(bool parallel);
    
    /**
     * 设置最大并行数
     */
    void setMaxParallel(int maxParallel);
    
    /**
     * 设置输出格式
     */
    void setOutputFormat(const std::string& format); // "console", "json", "xml"
    
    /**
     * 设置报告输出路径
     */
    void setReportPath(const std::string& path);
    
    // === 过滤和选择 ===
    
    /**
     * 设置测试名称过滤器
     */
    void setNameFilter(const std::string& pattern);
    
    /**
     * 设置分类过滤器
     */
    void setCategoryFilter(const std::string& category);
    
    /**
     * 设置优先级过滤器
     */
    void setPriorityFilter(int minPriority, int maxPriority = 10);
    
    // === 报告和统计 ===
    
    /**
     * 生成综合报告
     */
    std::string generateComprehensiveReport();
    
    /**
     * 生成性能报告
     */
    std::string generatePerformanceReport();
    
    /**
     * 生成覆盖率报告
     */
    std::string generateCoverageReport();
    
    /**
     * 导出报告到文件
     */
    bool exportReport(const std::string& format, const std::string& filename);
    
    // === 实用工具 ===
    
    /**
     * 列出所有可用的测试套件
     */
    void listAvailableSuites();
    
    /**
     * 获取测试统计信息
     */
    const TestStatistics& getStatistics() const { return overallStats_; }
    
    /**
     * 清理所有结果
     */
    void clearResults();

private:
    std::vector<std::unique_ptr<TestBase>> testSuites_;
    std::vector<TestStatistics> suiteStats_;
    TestStatistics overallStats_;
    
    // 配置选项
    bool verbose_;
    bool parallel_;
    int maxParallel_;
    std::string outputFormat_;
    std::string reportPath_;
    std::string nameFilter_;
    std::string categoryFilter_;
    int minPriority_;
    int maxPriority_;
    
    /**
     * 应用过滤器
     */
    bool shouldRunSuite(const std::string& suiteName);
    
    /**
     * 合并统计信息
     */
    void mergeStatistics(const TestStatistics& stats);
    
    /**
     * 输出进度条
     */
    void printProgressBar(int current, int total);
    
    /**
     * 顺序运行测试套件
     */
    void runSuitesSequentially();
    
    /**
     * 并行运行测试套件
     */
    void runSuitesInParallel();
    
    /**
     * 格式化持续时间
     */
    std::string formatDuration(std::chrono::milliseconds duration);
};

/**
 * 测试套件自动注册宏
 */
#define REGISTER_TEST_SUITE(ClassName) \
    namespace { \
        struct ClassName##Registrar { \
            ClassName##Registrar() { \
                TestSuiteRegistry::getInstance().registerSuite( \
                    #ClassName, \
                    []() -> std::unique_ptr<TestBase> { \
                        return std::make_unique<ClassName>(); \
                    } \
                ); \
            } \
        }; \
        static ClassName##Registrar ClassName##_registrar; \
    }

/**
 * 便捷的测试宏
 */
#define TEST_CASE(testName, description) \
    addTest(testName, description, [this]()

#define TEST_CATEGORY(testName, description, category) \
    addTest(testName, description, [this](), category)

#define TEST_PRIORITY(testName, description, category, priority) \
    addTest(testName, description, [this](), category, priority)

} // namespace framework
} // namespace test
} // namespace chtl