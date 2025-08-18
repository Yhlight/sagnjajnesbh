#include "ConfigurationSystemTest.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace chtl {
namespace test {

ConfigurationSystemTest::ConfigurationSystemTest() 
    : TestBase("ConfigurationSystem") {
    configManager_ = std::make_shared<chtl::configuration::ConfigurationManager>();
    configImporter_ = std::make_shared<chtl::configuration::ConfigurationImporter>();
}

ConfigurationSystemTest::~ConfigurationSystemTest() = default;

void ConfigurationSystemTest::setUp() {
    createTestConfigurationFiles();
}

void ConfigurationSystemTest::tearDown() {
    cleanupTestFiles();
}

void ConfigurationSystemTest::testCreateConfigurationGroup() {
    startTest("创建配置组");
    
    auto basicConfig = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("TestBasic", *basicConfig);
    
    auto retrievedConfig = configManager_->getConfigurationGroup("TestBasic");
    
    assertTrue(retrievedConfig != nullptr, "配置组应该被成功创建");
    assertTrue(retrievedConfig->name == "TestBasic", "配置组名称应该正确");
    assertTrue(!retrievedConfig->sections.empty(), "配置组应该包含配置节");
    
    endTest();
}

void ConfigurationSystemTest::testNamedConfigurationGroup() {
    startTest("命名配置组");
    
    auto config = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("Named", *config);
    
    auto namedConfig = configManager_->getConfigurationGroup("Named");
    assertTrue(namedConfig != nullptr, "应该能获取命名配置组");
    assertTrue(namedConfig->name == "Named", "命名配置组名称应该正确");
    
    endTest();
}

void ConfigurationSystemTest::testUnnamedConfigurationGroup() {
    startTest("无名配置组");
    
    auto config = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("", *config);
    
    auto unnamedConfig = configManager_->getConfigurationGroup("");
    assertTrue(unnamedConfig != nullptr, "应该能获取无名配置组");
    assertTrue(unnamedConfig->name.empty(), "无名配置组名称应该为空");
    assertTrue(unnamedConfig->isActive, "无名配置组应该默认激活");
    
    endTest();
}

void ConfigurationSystemTest::testConfigurationGroupConflict() {
    startTest("配置组冲突检测");
    
    auto config1 = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    auto config2 = chtl::configuration::ConfigurationFactory::createStdConfiguration();
    
    configManager_->addConfigurationGroup("", *config1);
    configManager_->addConfigurationGroup("", *config2);
    
    assertTrue(configManager_->hasConflict(), "应该检测到多个无名配置组的冲突");
    
    endTest();
}

void ConfigurationSystemTest::testActiveConfigurationGroup() {
    startTest("活动配置组");
    
    auto config1 = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    auto config2 = chtl::configuration::ConfigurationFactory::createStdConfiguration();
    
    configManager_->addConfigurationGroup("Config1", *config1);
    configManager_->addConfigurationGroup("Config2", *config2);
    
    configManager_->setActiveConfigurationGroup("Config2");
    
    auto activeConfig = configManager_->getActiveConfigurationGroup();
    assertTrue(activeConfig != nullptr, "应该有活动配置组");
    assertTrue(activeConfig->name == "Config2", "活动配置组应该是Config2");
    
    endTest();
}

void ConfigurationSystemTest::testGetConfigValue() {
    startTest("获取配置值");
    
    auto config = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("", *config);
    
    std::string customElement = configManager_->getConfigValue("Name", "CUSTOM_ELEMENT", "");
    assertTrue(customElement == "@Element", "应该能获取正确的配置值");
    
    std::string nonExistent = configManager_->getConfigValue("Name", "NON_EXISTENT", "default");
    assertTrue(nonExistent == "default", "不存在的配置项应该返回默认值");
    
    endTest();
}

void ConfigurationSystemTest::testGetConfigValues() {
    startTest("获取配置数组值");
    
    auto config = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("", *config);
    
    auto customStyles = configManager_->getConfigValues("Name", "CUSTOM_STYLE");
    assertTrue(!customStyles.empty(), "应该能获取配置数组值");
    assertTrue(customStyles.size() >= 3, "CUSTOM_STYLE应该包含多个值");
    
    endTest();
}

void ConfigurationSystemTest::testConfigurationSections() {
    startTest("配置节");
    
    auto config = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    configManager_->addConfigurationGroup("", *config);
    
    auto activeConfig = configManager_->getActiveConfigurationGroup();
    assertTrue(activeConfig->sections.find("Name") != activeConfig->sections.end(), 
              "应该包含Name配置节");
    assertTrue(activeConfig->sections.find("OriginType") != activeConfig->sections.end(), 
              "应该包含OriginType配置节");
    
    endTest();
}

void ConfigurationSystemTest::testImportConfiguration() {
    startTest("导入配置");
    
    bool success = configImporter_->importConfiguration("config/basic.chtl", "ImportedBasic");
    assertTrue(success, "应该能成功导入配置文件");
    
    auto importedConfig = configManager_->getConfigurationGroup("ImportedBasic");
    assertTrue(importedConfig != nullptr, "导入的配置组应该存在");
    
    endTest();
}

void ConfigurationSystemTest::testImportWithWildcard() {
    startTest("通配符导入");
    
    bool isWildcard = configImporter_->isWildcardPattern("config/*.chtl");
    assertTrue(isWildcard, "应该识别通配符模式");
    
    bool notWildcard = configImporter_->isWildcardPattern("config/basic.chtl");
    assertTrue(!notWildcard, "普通路径不应该被识别为通配符");
    
    endTest();
}

void ConfigurationSystemTest::testImportAllTemplates() {
    startTest("导入所有模板");
    
    bool success = configImporter_->importAllTemplates("components/*.chtl");
    assertTrue(success, "应该能使用通配符导入所有模板");
    
    endTest();
}

void ConfigurationSystemTest::testImportAllCustom() {
    startTest("导入所有自定义");
    
    bool success = configImporter_->importAllCustom("ui/*.chtl");
    assertTrue(success, "应该能使用通配符导入所有自定义");
    
    endTest();
}

void ConfigurationSystemTest::testImportAllOrigin() {
    startTest("导入所有原始嵌入");
    
    bool success = configImporter_->importAllOrigin("origins/*.chtl");
    assertTrue(success, "应该能使用通配符导入所有原始嵌入");
    
    endTest();
}

void ConfigurationSystemTest::testWildcardExpansion() {
    startTest("通配符展开");
    
    auto matchedFiles = configImporter_->expandWildcard("components/*.chtl");
    assertTrue(!matchedFiles.empty(), "通配符展开应该找到匹配的文件");
    
    endTest();
}

void ConfigurationSystemTest::testPatternMatching() {
    startTest("模式匹配");
    
    bool matches1 = configImporter_->matchesPattern("Button.chtl", "*.chtl");
    assertTrue(matches1, "Button.chtl应该匹配*.chtl模式");
    
    bool matches2 = configImporter_->matchesPattern("Card.chtl", "C*.chtl");
    assertTrue(matches2, "Card.chtl应该匹配C*.chtl模式");
    
    bool notMatches = configImporter_->matchesPattern("Button.js", "*.chtl");
    assertTrue(!notMatches, "Button.js不应该匹配*.chtl模式");
    
    endTest();
}

void ConfigurationSystemTest::testWildcardImportMultipleFiles() {
    startTest("通配符导入多个文件");
    
    bool success = configImporter_->importAllTemplates("components/*");
    assertTrue(success, "应该能通过通配符导入多个文件");
    
    endTest();
}

void ConfigurationSystemTest::testConfigurationValidation() {
    startTest("配置验证");
    
    auto validConfig = chtl::configuration::ConfigurationFactory::createBasicConfiguration();
    assertTrue(validConfig != nullptr, "基础配置应该是有效的");
    assertTrue(!validConfig->sections.empty(), "有效配置应该包含配置节");
    
    endTest();
}

void ConfigurationSystemTest::testInvalidConfiguration() {
    startTest("无效配置处理");
    
    bool success = configImporter_->importConfiguration("nonexistent.chtl");
    assertTrue(!success, "导入不存在的文件应该失败");
    
    std::string error = configImporter_->getLastError();
    assertTrue(!error.empty(), "应该有错误信息");
    
    endTest();
}

void ConfigurationSystemTest::runAllTests() {
    std::cout << "🔧 运行配置系统测试...\n" << std::endl;
    
    testCreateConfigurationGroup();
    testNamedConfigurationGroup();
    testUnnamedConfigurationGroup();
    testConfigurationGroupConflict();
    testActiveConfigurationGroup();
    testGetConfigValue();
    testGetConfigValues();
    testConfigurationSections();
    testImportConfiguration();
    testImportWithWildcard();
    testImportAllTemplates();
    testImportAllCustom();
    testImportAllOrigin();
    testWildcardExpansion();
    testPatternMatching();
    testWildcardImportMultipleFiles();
    testConfigurationValidation();
    testInvalidConfiguration();
    
    printSummary();
}

void ConfigurationSystemTest::createTestConfigurationFiles() {
    // 测试文件已经在之前创建了
    std::cout << "📁 使用现有的测试配置文件" << std::endl;
}

void ConfigurationSystemTest::cleanupTestFiles() {
    // 清理操作（如果需要）
    std::cout << "🧹 配置系统测试清理完成" << std::endl;
}

} // namespace test
} // namespace chtl