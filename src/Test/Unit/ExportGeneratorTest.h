#pragma once
#include "../Framework/TestBase.h"
#include "../../UnifiedCore/Export/ExportGenerator.h"

namespace chtl {
namespace test {

class ExportGeneratorTest : public TestBase {
public:
    ExportGeneratorTest();
    virtual ~ExportGeneratorTest();
    
    virtual void setUp() override;
    virtual void tearDown() override;
    
    // 基础功能测试
    void testParseCustomElements();
    void testParseCustomStyles();
    void testParseCustomVars();
    void testParseTemplateElements();
    void testParseTemplateStyles();
    void testParseTemplateVars();
    void testParseOriginItems();
    void testParseConfigurations();
    
    // 导出生成测试
    void testGenerateExportSection();
    void testGenerateInfoFile();
    void testUpdateInfoFileExports();
    
    // 工具函数测试
    void testExportStatistics();
    void testExportGrouping();
    void testFullNameGeneration();
    
    // 集成测试
    void testAnalyzeRealSourceFile();
    void testBatchProcessing();
    
    virtual void runAllTests() override;

private:
    std::shared_ptr<chtl::export_gen::ExportGenerator> exportGen_;
    
    void createTestSourceFiles();
    void cleanupTestFiles();
    
    // 测试用的源代码示例
    std::string getTestCustomElementSource();
    std::string getTestTemplateSource();
    std::string getTestOriginSource();
    std::string getTestConfigurationSource();
};

} // namespace test
} // namespace chtl