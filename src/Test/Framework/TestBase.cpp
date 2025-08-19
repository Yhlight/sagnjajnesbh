#include "TestBase.h"
#include <algorithm>
#include <iomanip>
#include <exception>

namespace chtl {
namespace test {
namespace framework {

// 静态成员初始化
thread_local std::string TestBase::currentTestName_;
thread_local bool TestBase::testFailed_ = false;
thread_local std::string TestBase::failureMessage_;

TestBase::TestBase(const std::string& suiteName) 
    : suiteName_(suiteName), verbose_(false) {
}

void TestBase::addTest(const std::string& name, const std::string& description, 
                      std::function<void()> testFunction, const std::string& category, 
                      int priority) {
    testCases_.emplace_back(name, description, testFunction, category, priority);
}

TestStatistics TestBase::runAllTests() {
    TestStatistics stats;
    testResults_.clear();
    
    std::cout << "\n=== " << suiteName_ << " 测试套件 ===" << std::endl;
    std::cout << "运行 " << testCases_.size() << " 个测试用例\n" << std::endl;
    
    // 按优先级排序
    std::sort(testCases_.begin(), testCases_.end(), 
        [](const TestCase& a, const TestCase& b) {
            return a.priority > b.priority;
        });
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (const auto& testCase : testCases_) {
        if (!testCase.enabled) {
            TestResultInfo result;
            result.result = TestResult::SKIP;
            result.testName = testCase.name;
            result.details = "测试被禁用";
            recordResult(result);
            continue;
        }
        
        setUp();
        auto result = executeTest(testCase);
        tearDown();
        
        recordResult(result);
        printProgress(testCase.name, result.result);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // 计算统计信息
    stats.totalTests = testResults_.size();
    for (const auto& result : testResults_) {
        switch (result.result) {
            case TestResult::PASS: stats.passedTests++; break;
            case TestResult::FAIL: stats.failedTests++; break;
            case TestResult::SKIP: stats.skippedTests++; break;
            case TestResult::ERROR: stats.errorTests++; break;
        }
        stats.totalDuration += result.duration;
    }
    stats.calculate();
    
    // 输出测试结果
    std::cout << "\n" << generateReport(stats) << std::endl;
    
    return stats;
}

TestStatistics TestBase::runTestsByCategory(const std::string& category) {
    TestStatistics stats;
    testResults_.clear();
    
    std::cout << "\n=== " << suiteName_ << " - " << category << " 分类测试 ===" << std::endl;
    
    for (const auto& testCase : testCases_) {
        if (testCase.category == category && testCase.enabled) {
            setUp();
            auto result = executeTest(testCase);
            tearDown();
            
            recordResult(result);
            printProgress(testCase.name, result.result);
        }
    }
    
    // 计算统计信息
    stats.totalTests = testResults_.size();
    for (const auto& result : testResults_) {
        switch (result.result) {
            case TestResult::PASS: stats.passedTests++; break;
            case TestResult::FAIL: stats.failedTests++; break;
            case TestResult::SKIP: stats.skippedTests++; break;
            case TestResult::ERROR: stats.errorTests++; break;
        }
        stats.totalDuration += result.duration;
    }
    stats.calculate();
    
    std::cout << "\n" << generateReport(stats) << std::endl;
    
    return stats;
}

TestResultInfo TestBase::runSingleTest(const std::string& testName) {
    auto it = std::find_if(testCases_.begin(), testCases_.end(),
        [&testName](const TestCase& tc) { return tc.name == testName; });
    
    if (it == testCases_.end()) {
        TestResultInfo result;
        result.result = TestResult::ERROR;
        result.testName = testName;
        result.errorMessage = "测试用例不存在";
        return result;
    }
    
    setUp();
    auto result = executeTest(*it);
    tearDown();
    
    return result;
}

TestResultInfo TestBase::executeTest(const TestCase& testCase) {
    TestResultInfo result;
    result.testName = testCase.name;
    
    currentTestName_ = testCase.name;
    testFailed_ = false;
    failureMessage_.clear();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        testCase.testFunction();
        
        if (testFailed_) {
            result.result = TestResult::FAIL;
            result.errorMessage = failureMessage_;
        } else {
            result.result = TestResult::PASS;
        }
        
    } catch (const TestSkippedException& e) {
        result.result = TestResult::SKIP;
        result.errorMessage = e.what();
    } catch (const TestFailedException& e) {
        result.result = TestResult::FAIL;
        result.errorMessage = e.what();
    } catch (const std::exception& e) {
        result.result = TestResult::ERROR;
        result.errorMessage = std::string("异常: ") + e.what();
    } catch (...) {
        result.result = TestResult::ERROR;
        result.errorMessage = "未知异常";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.details = testCase.description;
    
    return result;
}

void TestBase::recordResult(const TestResultInfo& result) {
    testResults_.push_back(result);
}

void TestBase::printProgress(const std::string& testName, TestResult result) {
    std::string status;
    switch (result) {
        case TestResult::PASS:  status = "✅ PASS"; break;
        case TestResult::FAIL:  status = "❌ FAIL"; break;
        case TestResult::SKIP:  status = "⏭️ SKIP"; break;
        case TestResult::ERROR: status = "💥 ERROR"; break;
    }
    
    if (verbose_) {
        std::cout << "[" << status << "] " << testName << std::endl;
    } else {
        std::cout << (result == TestResult::PASS ? "." : 
                     result == TestResult::FAIL ? "F" :
                     result == TestResult::SKIP ? "S" : "E") << std::flush;
    }
}

// === 断言方法实现 ===

void TestBase::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::string fullMessage = "断言失败: 期望为真";
        if (!message.empty()) {
            fullMessage += " - " + message;
        }
        throw TestFailedException(fullMessage);
    }
}

void TestBase::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        std::string fullMessage = "断言失败: 期望为假";
        if (!message.empty()) {
            fullMessage += " - " + message;
        }
        throw TestFailedException(fullMessage);
    }
}

void TestBase::assertThrows(std::function<void()> func, const std::string& message) {
    bool threwException = false;
    try {
        func();
    } catch (...) {
        threwException = true;
    }
    
    if (!threwException) {
        std::string fullMessage = "断言失败: 期望抛出异常";
        if (!message.empty()) {
            fullMessage += " - " + message;
        }
        throw TestFailedException(fullMessage);
    }
}

void TestBase::assertNoThrow(std::function<void()> func, const std::string& message) {
    try {
        func();
    } catch (const std::exception& e) {
        std::string fullMessage = "断言失败: 不应该抛出异常: " + std::string(e.what());
        if (!message.empty()) {
            fullMessage += " - " + message;
        }
        throw TestFailedException(fullMessage);
    } catch (...) {
        std::string fullMessage = "断言失败: 不应该抛出未知异常";
        if (!message.empty()) {
            fullMessage += " - " + message;
        }
        throw TestFailedException(fullMessage);
    }
}

void TestBase::skipTest(const std::string& reason) {
    throw TestSkippedException(reason);
}

void TestBase::failTest(const std::string& message) {
    throw TestFailedException(message);
}

void TestBase::enableTest(const std::string& testName, bool enabled) {
    auto it = std::find_if(testCases_.begin(), testCases_.end(),
        [&testName](TestCase& tc) { return tc.name == testName; });
    
    if (it != testCases_.end()) {
        it->enabled = enabled;
    }
}

// === 报告生成实现 ===

std::string TestBase::generateReport(const TestStatistics& stats) {
    std::ostringstream oss;
    
    oss << "📊 测试结果统计:\n";
    oss << "  总测试数: " << stats.totalTests << "\n";
    oss << "  通过: " << stats.passedTests << " ✅\n";
    oss << "  失败: " << stats.failedTests << " ❌\n";
    oss << "  跳过: " << stats.skippedTests << " ⏭️\n";
    oss << "  错误: " << stats.errorTests << " 💥\n";
    oss << "  成功率: " << std::fixed << std::setprecision(1) << stats.successRate << "%\n";
    oss << "  总耗时: " << formatDuration(stats.totalDuration) << "\n";
    
    if (stats.failedTests > 0 || stats.errorTests > 0) {
        oss << "\n❌ 失败的测试:\n";
        for (const auto& result : testResults_) {
            if (result.result == TestResult::FAIL || result.result == TestResult::ERROR) {
                oss << "  - " << result.testName << ": " << result.errorMessage << "\n";
            }
        }
    }
    
    return oss.str();
}

std::string TestBase::generateDetailedReport(const TestStatistics& stats) {
    std::ostringstream oss;
    
    oss << "=== " << suiteName_ << " 详细测试报告 ===\n\n";
    
    // 按分类分组
    std::map<std::string, std::vector<TestResultInfo>> categorizedResults;
    for (const auto& result : testResults_) {
        std::string category = "General";
        auto it = std::find_if(testCases_.begin(), testCases_.end(),
            [&result](const TestCase& tc) { return tc.name == result.testName; });
        if (it != testCases_.end()) {
            category = it->category;
        }
        categorizedResults[category].push_back(result);
    }
    
    for (const auto& [category, results] : categorizedResults) {
        oss << "📂 " << category << " 分类:\n";
        for (const auto& result : results) {
            std::string status = (result.result == TestResult::PASS) ? "✅" :
                               (result.result == TestResult::FAIL) ? "❌" :
                               (result.result == TestResult::SKIP) ? "⏭️" : "💥";
            
            oss << "  " << status << " " << result.testName 
                << " (" << formatDuration(result.duration) << ")\n";
            
            if (!result.details.empty()) {
                oss << "      描述: " << result.details << "\n";
            }
            
            if (!result.errorMessage.empty()) {
                oss << "      错误: " << result.errorMessage << "\n";
            }
        }
        oss << "\n";
    }
    
    oss << generateReport(stats);
    
    return oss.str();
}

std::string TestBase::generateJsonReport(const TestStatistics& stats) {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"suiteName\": \"" << suiteName_ << "\",\n";
    oss << "  \"statistics\": {\n";
    oss << "    \"totalTests\": " << stats.totalTests << ",\n";
    oss << "    \"passedTests\": " << stats.passedTests << ",\n";
    oss << "    \"failedTests\": " << stats.failedTests << ",\n";
    oss << "    \"skippedTests\": " << stats.skippedTests << ",\n";
    oss << "    \"errorTests\": " << stats.errorTests << ",\n";
    oss << "    \"successRate\": " << stats.successRate << ",\n";
    oss << "    \"totalDuration\": " << stats.totalDuration.count() << "\n";
    oss << "  },\n";
    oss << "  \"results\": [\n";
    
    for (size_t i = 0; i < testResults_.size(); ++i) {
        const auto& result = testResults_[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << result.testName << "\",\n";
        oss << "      \"result\": \"" << (result.result == TestResult::PASS ? "PASS" :
                                         result.result == TestResult::FAIL ? "FAIL" :
                                         result.result == TestResult::SKIP ? "SKIP" : "ERROR") << "\",\n";
        oss << "      \"duration\": " << result.duration.count() << ",\n";
        oss << "      \"details\": \"" << result.details << "\",\n";
        oss << "      \"errorMessage\": \"" << result.errorMessage << "\"\n";
        oss << "    }";
        if (i < testResults_.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string TestBase::formatDuration(std::chrono::milliseconds duration) {
    auto ms = duration.count();
    if (ms < 1000) {
        return std::to_string(ms) + "ms";
    } else if (ms < 60000) {
        return std::to_string(ms / 1000.0) + "s";
    } else {
        auto minutes = ms / 60000;
        auto seconds = (ms % 60000) / 1000.0;
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }
}

} // namespace framework
} // namespace test
} // namespace chtl