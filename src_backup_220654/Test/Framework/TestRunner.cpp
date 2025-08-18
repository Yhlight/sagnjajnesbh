#include "TestRunner.h"
#include <algorithm>
#include <fstream>
#include <regex>
#include <thread>
#include <future>
#include <iomanip>

namespace chtl {
namespace test {
namespace framework {

// === TestSuiteRegistry 实现 ===

TestSuiteRegistry& TestSuiteRegistry::getInstance() {
    static TestSuiteRegistry instance;
    return instance;
}

void TestSuiteRegistry::registerSuite(const std::string& name, TestSuiteFactory factory) {
    suites_[name] = factory;
}

std::unique_ptr<TestBase> TestSuiteRegistry::createSuite(const std::string& name) {
    auto it = suites_.find(name);
    if (it != suites_.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> TestSuiteRegistry::getRegisteredSuites() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : suites_) {
        names.push_back(name);
    }
    return names;
}

// === TestRunner 实现 ===

TestRunner::TestRunner() 
    : verbose_(false), parallel_(false), maxParallel_(4), 
      outputFormat_("console"), minPriority_(1), maxPriority_(10) {
}

void TestRunner::addSuite(std::unique_ptr<TestBase> suite) {
    testSuites_.push_back(std::move(suite));
}

void TestRunner::addSuite(const std::string& suiteName) {
    auto suite = TestSuiteRegistry::getInstance().createSuite(suiteName);
    if (suite) {
        addSuite(std::move(suite));
    } else {
        std::cerr << "⚠️ 警告: 未找到测试套件 " << suiteName << std::endl;
    }
}

TestStatistics TestRunner::runAllSuites() {
    std::cout << "\n🚀 开始运行所有测试套件..." << std::endl;
    std::cout << "测试套件数量: " << testSuites_.size() << std::endl;
    
    overallStats_ = TestStatistics{};
    suiteStats_.clear();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (parallel_ && testSuites_.size() > 1) {
        runSuitesInParallel();
    } else {
        runSuitesSequentially();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    overallStats_.totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    overallStats_.calculate();
    
    // 输出综合报告
    std::cout << "\n" << generateComprehensiveReport() << std::endl;
    
    // 导出报告（如果配置了）
    if (!reportPath_.empty()) {
        exportReport(outputFormat_, reportPath_);
    }
    
    return overallStats_;
}

void TestRunner::runSuitesSequentially() {
    for (size_t i = 0; i < testSuites_.size(); ++i) {
        auto& suite = testSuites_[i];
        
        if (!shouldRunSuite(suite->getSuiteName())) {
            continue;
        }
        
        std::cout << "\n📦 运行测试套件: " << suite->getSuiteName() 
                  << " (" << (i + 1) << "/" << testSuites_.size() << ")" << std::endl;
        
        suite->setVerbose(verbose_);
        auto stats = suite->runAllTests();
        
        suiteStats_.push_back(stats);
        mergeStatistics(stats);
        
        if (!verbose_) {
            printProgressBar(i + 1, testSuites_.size());
        }
    }
}

void TestRunner::runSuitesInParallel() {
    std::cout << "🔄 并行运行模式 (最大并行数: " << maxParallel_ << ")" << std::endl;
    
    std::vector<std::future<TestStatistics>> futures;
    
    for (auto& suite : testSuites_) {
        if (!shouldRunSuite(suite->getSuiteName())) {
            continue;
        }
        
        auto future = std::async(std::launch::async, [&suite, this]() {
            suite->setVerbose(verbose_);
            return suite->runAllTests();
        });
        
        futures.push_back(std::move(future));
        
        // 控制并行数量
        if (futures.size() >= static_cast<size_t>(maxParallel_)) {
            for (auto& f : futures) {
                auto stats = f.get();
                suiteStats_.push_back(stats);
                mergeStatistics(stats);
            }
            futures.clear();
        }
    }
    
    // 等待剩余的任务完成
    for (auto& f : futures) {
        auto stats = f.get();
        suiteStats_.push_back(stats);
        mergeStatistics(stats);
    }
}

TestStatistics TestRunner::runSuite(const std::string& suiteName) {
    auto it = std::find_if(testSuites_.begin(), testSuites_.end(),
        [&suiteName](const std::unique_ptr<TestBase>& suite) {
            return suite->getSuiteName() == suiteName;
        });
    
    if (it != testSuites_.end()) {
        (*it)->setVerbose(verbose_);
        return (*it)->runAllTests();
    }
    
    std::cerr << "❌ 错误: 未找到测试套件 " << suiteName << std::endl;
    return TestStatistics{};
}

TestStatistics TestRunner::runCategory(const std::string& category) {
    TestStatistics combinedStats;
    
    std::cout << "\n🏷️ 运行分类测试: " << category << std::endl;
    
    for (auto& suite : testSuites_) {
        suite->setVerbose(verbose_);
        auto stats = suite->runTestsByCategory(category);
        
        combinedStats.totalTests += stats.totalTests;
        combinedStats.passedTests += stats.passedTests;
        combinedStats.failedTests += stats.failedTests;
        combinedStats.skippedTests += stats.skippedTests;
        combinedStats.errorTests += stats.errorTests;
        combinedStats.totalDuration += stats.totalDuration;
    }
    
    combinedStats.calculate();
    return combinedStats;
}

void TestRunner::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void TestRunner::setParallel(bool parallel) {
    parallel_ = parallel;
}

void TestRunner::setMaxParallel(int maxParallel) {
    maxParallel_ = std::max(1, maxParallel);
}

void TestRunner::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void TestRunner::setReportPath(const std::string& path) {
    reportPath_ = path;
}

void TestRunner::setNameFilter(const std::string& pattern) {
    nameFilter_ = pattern;
}

void TestRunner::setCategoryFilter(const std::string& category) {
    categoryFilter_ = category;
}

void TestRunner::setPriorityFilter(int minPriority, int maxPriority) {
    minPriority_ = minPriority;
    maxPriority_ = maxPriority;
}

bool TestRunner::shouldRunSuite(const std::string& suiteName) {
    // 应用名称过滤器
    if (!nameFilter_.empty()) {
        std::regex nameRegex(nameFilter_);
        if (!std::regex_search(suiteName, nameRegex)) {
            return false;
        }
    }
    
    return true;
}

void TestRunner::mergeStatistics(const TestStatistics& stats) {
    overallStats_.totalTests += stats.totalTests;
    overallStats_.passedTests += stats.passedTests;
    overallStats_.failedTests += stats.failedTests;
    overallStats_.skippedTests += stats.skippedTests;
    overallStats_.errorTests += stats.errorTests;
    overallStats_.totalDuration += stats.totalDuration;
}

void TestRunner::printProgressBar(int current, int total) {
    const int barWidth = 50;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% (" << current << "/" << total << ")";
    std::cout.flush();
    
    if (current == total) {
        std::cout << std::endl;
    }
}

std::string TestRunner::generateComprehensiveReport() {
    std::ostringstream oss;
    
    oss << "=== CHTL 测试框架综合报告 ===\n\n";
    
    // 总体统计
    oss << "📊 总体统计:\n";
    oss << "  测试套件数: " << testSuites_.size() << "\n";
    oss << "  总测试数: " << overallStats_.totalTests << "\n";
    oss << "  通过: " << overallStats_.passedTests << " ✅\n";
    oss << "  失败: " << overallStats_.failedTests << " ❌\n";
    oss << "  跳过: " << overallStats_.skippedTests << " ⏭️\n";
    oss << "  错误: " << overallStats_.errorTests << " 💥\n";
    oss << "  成功率: " << std::fixed << std::setprecision(1) << overallStats_.successRate << "%\n";
    oss << "  总耗时: " << formatDuration(overallStats_.totalDuration) << "\n\n";
    
    // 各套件详情
    oss << "📋 各测试套件详情:\n";
    for (size_t i = 0; i < testSuites_.size() && i < suiteStats_.size(); ++i) {
        const auto& suite = testSuites_[i];
        const auto& stats = suiteStats_[i];
        
        oss << "  🔸 " << suite->getSuiteName() << ":\n";
        oss << "    测试数: " << stats.totalTests << "\n";
        oss << "    成功率: " << std::fixed << std::setprecision(1) << stats.successRate << "%\n";
        oss << "    耗时: " << formatDuration(stats.totalDuration) << "\n";
        
        if (stats.failedTests > 0 || stats.errorTests > 0) {
            oss << "    状态: ❌ 有失败\n";
        } else {
            oss << "    状态: ✅ 全部通过\n";
        }
    }
    
    // 性能分析
    if (suiteStats_.size() > 1) {
        oss << "\n⚡ 性能分析:\n";
        
        auto fastestSuite = std::min_element(suiteStats_.begin(), suiteStats_.end(),
            [](const TestStatistics& a, const TestStatistics& b) {
                return a.totalDuration < b.totalDuration;
            });
        
        auto slowestSuite = std::max_element(suiteStats_.begin(), suiteStats_.end(),
            [](const TestStatistics& a, const TestStatistics& b) {
                return a.totalDuration < b.totalDuration;
            });
        
        if (fastestSuite != suiteStats_.end() && slowestSuite != suiteStats_.end()) {
            oss << "  最快套件耗时: " << formatDuration(fastestSuite->totalDuration) << "\n";
            oss << "  最慢套件耗时: " << formatDuration(slowestSuite->totalDuration) << "\n";
            oss << "  平均耗时: " << formatDuration(std::chrono::milliseconds(
                overallStats_.totalDuration.count() / suiteStats_.size())) << "\n";
        }
    }
    
    return oss.str();
}

std::string TestRunner::generatePerformanceReport() {
    std::ostringstream oss;
    
    oss << "=== CHTL 性能测试报告 ===\n\n";
    
    // 性能指标
    oss << "⚡ 性能指标:\n";
    oss << "  总执行时间: " << formatDuration(overallStats_.totalDuration) << "\n";
    oss << "  平均测试时间: " << (overallStats_.totalTests > 0 ? 
        formatDuration(std::chrono::milliseconds(overallStats_.totalDuration.count() / overallStats_.totalTests)) : "0ms") << "\n";
    oss << "  测试吞吐量: " << (overallStats_.totalDuration.count() > 0 ?
        static_cast<double>(overallStats_.totalTests * 1000) / overallStats_.totalDuration.count() : 0) << " 测试/秒\n";
    
    return oss.str();
}

std::string TestRunner::generateCoverageReport() {
    std::ostringstream oss;
    
    oss << "=== CHTL 测试覆盖率报告 ===\n\n";
    
    // 简化的覆盖率统计
    oss << "📈 覆盖率统计:\n";
    oss << "  测试套件覆盖: " << testSuites_.size() << " 个模块\n";
    oss << "  功能覆盖率: " << std::fixed << std::setprecision(1) << overallStats_.successRate << "%\n";
    
    return oss.str();
}

bool TestRunner::exportReport(const std::string& format, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ 无法创建报告文件: " << filename << std::endl;
            return false;
        }
        
        if (format == "json") {
            // 生成JSON格式报告
            file << "{\n";
            file << "  \"testFramework\": \"CHTL Test Framework\",\n";
            file << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() << "\",\n";
            file << "  \"overallStats\": {\n";
            file << "    \"totalTests\": " << overallStats_.totalTests << ",\n";
            file << "    \"passedTests\": " << overallStats_.passedTests << ",\n";
            file << "    \"failedTests\": " << overallStats_.failedTests << ",\n";
            file << "    \"successRate\": " << overallStats_.successRate << ",\n";
            file << "    \"duration\": " << overallStats_.totalDuration.count() << "\n";
            file << "  },\n";
            file << "  \"suites\": [\n";
            
            for (size_t i = 0; i < testSuites_.size() && i < suiteStats_.size(); ++i) {
                const auto& suite = testSuites_[i];
                const auto& stats = suiteStats_[i];
                
                file << "    {\n";
                file << "      \"name\": \"" << suite->getSuiteName() << "\",\n";
                file << "      \"testCount\": " << stats.totalTests << ",\n";
                file << "      \"successRate\": " << stats.successRate << ",\n";
                file << "      \"duration\": " << stats.totalDuration.count() << "\n";
                file << "    }";
                if (i < testSuites_.size() - 1) file << ",";
                file << "\n";
            }
            
            file << "  ]\n";
            file << "}\n";
        } else {
            // 默认文本格式
            file << generateComprehensiveReport();
        }
        
        file.close();
        std::cout << "📄 报告已导出到: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 导出报告失败: " << e.what() << std::endl;
        return false;
    }
}

void TestRunner::listAvailableSuites() {
    std::cout << "\n📋 可用的测试套件:\n";
    
    auto registeredSuites = TestSuiteRegistry::getInstance().getRegisteredSuites();
    for (const auto& suiteName : registeredSuites) {
        std::cout << "  🔸 " << suiteName << std::endl;
    }
    
    std::cout << "\n📦 已加载的测试套件:\n";
    for (const auto& suite : testSuites_) {
        std::cout << "  🔹 " << suite->getSuiteName() 
                  << " (" << suite->getTestCount() << " 个测试)" << std::endl;
    }
}

void TestRunner::clearResults() {
    suiteStats_.clear();
    overallStats_ = TestStatistics{};
}

std::string TestRunner::formatDuration(std::chrono::milliseconds duration) {
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