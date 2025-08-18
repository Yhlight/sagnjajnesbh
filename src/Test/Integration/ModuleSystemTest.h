#pragma once
#include "../Framework/TestBase.h"
#include <memory>

namespace chtl {
namespace test {
namespace integration {

/**
 * 模块系统集成测试
 * 测试CMOD和CJMOD的完整工作流程
 */
class ModuleSystemTest : public framework::TestBase {
public:
    ModuleSystemTest();
    
    void setUp() override;
    void tearDown() override;

private:
    std::string tempModuleDir_;
    
    // 测试用例
    void testCMODLifecycle();
    void testCJMODLifecycle();
    void testModuleImport();
    void testModuleConflictResolution();
    void testModuleVersioning();
    void testModuleValidation();
    void testChthollyModuleIntegration();
    void testYuigahamaModuleIntegration();
    void testCrossModuleCompatibility();
};

} // namespace integration
} // namespace test
} // namespace chtl