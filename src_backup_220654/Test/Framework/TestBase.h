#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <sstream>
#include <memory>
#include <map>

namespace chtl {
namespace test {
namespace framework {

/**
 * 测试结果枚举
 */
enum class TestResult {
    PASS,
    FAIL,
    SKIP,
    ERROR
};

/**
 * 测试用例信息
 */
struct TestCase {
    std::string name;
    std::string description;
    std::function<void()> testFunction;
    std::string category;
    int priority;
    bool enabled;
    
    TestCase(const std::string& n, const std::string& desc, std::function<void()> func, 
             const std::string& cat = "General", int prio = 5, bool en = true)
        : name(n), description(desc), testFunction(func), category(cat), priority(prio), enabled(en) {}
};

/**
 * 测试结果详情
 */
struct TestResultInfo {
    TestResult result;
    std::string testName;
    std::string errorMessage;
    std::chrono::milliseconds duration;
    std::string details;
    
    TestResultInfo() : result(TestResult::PASS), duration(0) {}
};

/**
 * 测试统计信息
 */
struct TestStatistics {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    int skippedTests = 0;
    int errorTests = 0;
    std::chrono::milliseconds totalDuration{0};
    double successRate = 0.0;
    
    void calculate() {
        if (totalTests > 0) {
            successRate = static_cast<double>(passedTests) / totalTests * 100.0;
        }
    }
};

/**
 * 测试基础类
 * 提供测试框架的核心功能
 */
class TestBase {
public:
    TestBase(const std::string& suiteName);
    virtual ~TestBase() = default;
    
    // === 测试用例管理 ===
    
    /**
     * 添加测试用例
     */
    void addTest(const std::string& name, const std::string& description, 
                 std::function<void()> testFunction, const std::string& category = "General", 
                 int priority = 5);
    
    /**
     * 运行所有测试
     */
    virtual TestStatistics runAllTests();
    
    /**
     * 运行指定分类的测试
     */
    virtual TestStatistics runTestsByCategory(const std::string& category);
    
    /**
     * 运行单个测试
     */
    virtual TestResultInfo runSingleTest(const std::string& testName);
    
    // === 断言方法 ===
    
    /**
     * 断言为真
     */
    static void assertTrue(bool condition, const std::string& message = "");
    
    /**
     * 断言为假
     */
    static void assertFalse(bool condition, const std::string& message = "");
    
    /**
     * 断言相等
     */
    template<typename T>
    static void assertEqual(const T& expected, const T& actual, const std::string& message = "");
    
    /**
     * 断言不相等
     */
    template<typename T>
    static void assertNotEqual(const T& expected, const T& actual, const std::string& message = "");
    
    /**
     * 断言抛出异常
     */
    static void assertThrows(std::function<void()> func, const std::string& message = "");
    
    /**
     * 断言不抛出异常
     */
    static void assertNoThrow(std::function<void()> func, const std::string& message = "");
    
    // === 测试工具方法 ===
    
    /**
     * 设置测试前的准备工作
     */
    virtual void setUp() {}
    
    /**
     * 清理测试后的工作
     */
    virtual void tearDown() {}
    
    /**
     * 跳过测试
     */
    static void skipTest(const std::string& reason = "");
    
    /**
     * 测试失败
     */
    static void failTest(const std::string& message = "");
    
    // === 报告生成 ===
    
    /**
     * 生成测试报告
     */
    std::string generateReport(const TestStatistics& stats);
    
    /**
     * 生成详细报告
     */
    std::string generateDetailedReport(const TestStatistics& stats);
    
    /**
     * 生成JSON报告
     */
    std::string generateJsonReport(const TestStatistics& stats);
    
    // === 配置和状态 ===
    
    /**
     * 设置详细输出
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    /**
     * 获取测试套件名称
     */
    const std::string& getSuiteName() const { return suiteName_; }
    
    /**
     * 获取测试用例数量
     */
    size_t getTestCount() const { return testCases_.size(); }
    
    /**
     * 启用/禁用测试用例
     */
    void enableTest(const std::string& testName, bool enabled = true);
    
protected:
    std::string suiteName_;
    std::vector<TestCase> testCases_;
    std::vector<TestResultInfo> testResults_;
    bool verbose_;
    
    // 当前测试上下文
    static thread_local std::string currentTestName_;
    static thread_local bool testFailed_;
    static thread_local std::string failureMessage_;
    
    /**
     * 执行单个测试用例
     */
    TestResultInfo executeTest(const TestCase& testCase);
    
    /**
     * 记录测试结果
     */
    void recordResult(const TestResultInfo& result);
    
    /**
     * 输出测试进度
     */
    void printProgress(const std::string& testName, TestResult result);
    
    /**
     * 格式化持续时间
     */
    std::string formatDuration(std::chrono::milliseconds duration);
};

/**
 * 测试异常类
 */
class TestException : public std::exception {
public:
    explicit TestException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
    
private:
    std::string message_;
};

/**
 * 测试跳过异常
 */
class TestSkippedException : public TestException {
public:
    explicit TestSkippedException(const std::string& reason) 
        : TestException("Test skipped: " + reason) {}
};

/**
 * 测试失败异常
 */
class TestFailedException : public TestException {
public:
    explicit TestFailedException(const std::string& message) 
        : TestException("Test failed: " + message) {}
};

// === 模板实现 ===

template<typename T>
void TestBase::assertEqual(const T& expected, const T& actual, const std::string& message) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << "断言失败: 期望 [" << expected << "] 但得到 [" << actual << "]";
        if (!message.empty()) {
            oss << " - " << message;
        }
        throw TestFailedException(oss.str());
    }
}

template<typename T>
void TestBase::assertNotEqual(const T& expected, const T& actual, const std::string& message) {
    if (expected == actual) {
        std::ostringstream oss;
        oss << "断言失败: 不应该相等 [" << expected << "] == [" << actual << "]";
        if (!message.empty()) {
            oss << " - " << message;
        }
        throw TestFailedException(oss.str());
    }
}

} // namespace framework
} // namespace test
} // namespace chtl