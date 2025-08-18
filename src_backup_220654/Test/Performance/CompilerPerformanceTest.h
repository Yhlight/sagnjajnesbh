#pragma once
#include "../Framework/TestBase.h"
#include "../Utils/TestUtils.h"

namespace chtl {
namespace test {
namespace performance {

/**
 * 编译器性能测试
 */
class CompilerPerformanceTest : public framework::TestBase {
public:
    CompilerPerformanceTest();
    
    void setUp() override;
    void tearDown() override;

private:
    utils::PerformanceTestUtils::Timer timer_;
    
    // 性能测试用例
    void testCSSCompilationSpeed();
    void testJavaScriptCompilationSpeed();
    void testLargeFileCompilation();
    void testMemoryUsage();
    void testConcurrentCompilation();
    void testNamespacePerformance();
    void testModuleLoadingPerformance();
    
    // 基准测试
    void benchmarkCSSParser();
    void benchmarkJSParser();
    void benchmarkCHTLParser();
    
    // 性能回归测试
    void testPerformanceRegression();
};

} // namespace performance
} // namespace test
} // namespace chtl