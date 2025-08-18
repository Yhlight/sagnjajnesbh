#pragma once
#include "../Framework/TestBase.h"
#include "../../UnifiedCore/Configuration/ConfigurationManager.h"
#include "../../UnifiedCore/Configuration/ConfigurationImporter.h"

namespace chtl {
namespace test {

class ConfigurationSystemTest : public TestBase {
public:
    ConfigurationSystemTest();
    virtual ~ConfigurationSystemTest();
    
    virtual void setUp() override;
    virtual void tearDown() override;
    
    // 配置组管理测试
    void testCreateConfigurationGroup();
    void testNamedConfigurationGroup();
    void testUnnamedConfigurationGroup();
    void testConfigurationGroupConflict();
    void testActiveConfigurationGroup();
    
    // 配置项访问测试
    void testGetConfigValue();
    void testGetConfigValues();
    void testConfigurationSections();
    
    // 配置导入测试
    void testImportConfiguration();
    void testImportWithWildcard();
    void testImportAllTemplates();
    void testImportAllCustom();
    void testImportAllOrigin();
    
    // 通配符功能测试
    void testWildcardExpansion();
    void testPatternMatching();
    void testWildcardImportMultipleFiles();
    
    // 配置验证测试
    void testConfigurationValidation();
    void testInvalidConfiguration();
    
    virtual void runAllTests() override;

private:
    std::shared_ptr<chtl::configuration::ConfigurationManager> configManager_;
    std::shared_ptr<chtl::configuration::ConfigurationImporter> configImporter_;
    
    void createTestConfigurationFiles();
    void cleanupTestFiles();
};

} // namespace test
} // namespace chtl