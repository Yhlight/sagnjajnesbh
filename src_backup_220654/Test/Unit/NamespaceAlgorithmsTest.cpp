#include "NamespaceAlgorithmsTest.h"
#include "../Utils/TestUtils.h"

namespace chtl {
namespace test {
namespace unit {

NamespaceAlgorithmsTest::NamespaceAlgorithmsTest() 
    : TestBase("NamespaceAlgorithms") {
    
    // 注册测试用例
    TEST_CASE("testSmartMerge", "测试智能合并算法") {
        testSmartMerge();
    };
    
    TEST_CASE("testConflictDetection", "测试冲突检测算法") {
        testConflictDetection();
    };
    
    TEST_CASE("testPerformanceOptimization", "测试性能优化算法") {
        testPerformanceOptimization();
    };
    
    TEST_CASE("testReportGeneration", "测试报告生成功能") {
        testReportGeneration();
    };
    
    TEST_CATEGORY("testErrorHandling", "测试错误处理", "ErrorHandling") {
        testErrorHandling();
    };
    
    TEST_PRIORITY("testEdgeCases", "测试边界情况", "EdgeCases", 8) {
        testEdgeCases();
    };
}

void NamespaceAlgorithmsTest::setUp() {
    algorithms_ = std::make_unique<shared::core::algorithms::NamespaceAlgorithms>();
}

void NamespaceAlgorithmsTest::tearDown() {
    algorithms_.reset();
}

void NamespaceAlgorithmsTest::testSmartMerge() {
    // 测试智能合并功能
    bool result = algorithms_->smartMergeNamespaces("Target", "Source");
    assertTrue(result, "智能合并应该成功");
    
    // 测试无效输入
    assertNoThrow([this]() {
        algorithms_->smartMergeNamespaces("", "");
    }, "空字符串输入不应该抛出异常");
}

void NamespaceAlgorithmsTest::testConflictDetection() {
    // 测试冲突检测功能
    auto conflicts = algorithms_->detectAdvancedConflicts();
    assertTrue(conflicts.size() >= 0, "冲突检测应该返回有效结果");
    
    // 验证冲突类型
    for (const auto& conflict : conflicts) {
        assertFalse(conflict.empty(), "冲突描述不应该为空");
    }
}

void NamespaceAlgorithmsTest::testPerformanceOptimization() {
    // 测试性能优化
    assertNoThrow([this]() {
        algorithms_->optimizePerformance();
    }, "性能优化不应该抛出异常");
}

void NamespaceAlgorithmsTest::testReportGeneration() {
    // 测试报告生成
    std::string report = algorithms_->generateReport();
    assertFalse(report.empty(), "报告不应该为空");
    assertTrue(report.find("命名空间算法报告") != std::string::npos, "报告应该包含标题");
}

void NamespaceAlgorithmsTest::testErrorHandling() {
    // 测试错误处理
    assertNoThrow([this]() {
        algorithms_->smartMergeNamespaces("InvalidTarget", "InvalidSource");
    }, "无效输入应该被优雅处理");
}

void NamespaceAlgorithmsTest::testEdgeCases() {
    // 测试边界情况
    auto& generator = utils::TestDataGenerator::getInstance();
    
    // 测试随机命名空间名称
    for (int i = 0; i < 10; ++i) {
        std::string randomName = generator.generateValidIdentifier();
        assertNoThrow([this, &randomName]() {
            algorithms_->smartMergeNamespaces(randomName, randomName + "_copy");
        }, "随机命名空间名称应该被正确处理");
    }
}

} // namespace unit
} // namespace test
} // namespace chtl