#pragma once
#include "../Framework/TestBase.h"
#include "../../Shared/Core/NamespaceAlgorithms.h"

namespace chtl {
namespace test {
namespace unit {

/**
 * 命名空间算法单元测试
 */
class NamespaceAlgorithmsTest : public framework::TestBase {
public:
    NamespaceAlgorithmsTest();
    
    void setUp() override;
    void tearDown() override;

private:
    std::unique_ptr<shared::core::algorithms::NamespaceAlgorithms> algorithms_;
    
    // 测试用例
    void testSmartMerge();
    void testConflictDetection();
    void testPerformanceOptimization();
    void testReportGeneration();
    void testErrorHandling();
    void testEdgeCases();
};

} // namespace unit
} // namespace test
} // namespace chtl