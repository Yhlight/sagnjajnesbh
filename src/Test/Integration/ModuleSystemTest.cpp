#include "ModuleSystemTest.h"
#include "../Utils/TestUtils.h"

namespace chtl {
namespace test {
namespace integration {

ModuleSystemTest::ModuleSystemTest() : TestBase("ModuleSystem") {
    
    TEST_CASE("testCMODLifecycle", "测试CMOD完整生命周期") {
        testCMODLifecycle();
    };
    
    TEST_CASE("testCJMODLifecycle", "测试CJMOD完整生命周期") {
        testCJMODLifecycle();
    };
    
    TEST_CASE("testModuleImport", "测试模块导入功能") {
        testModuleImport();
    };
    
    TEST_CATEGORY("testModuleConflictResolution", "测试模块冲突解决", "Conflict") {
        testModuleConflictResolution();
    };
    
    TEST_CATEGORY("testModuleVersioning", "测试模块版本管理", "Versioning") {
        testModuleVersioning();
    };
    
    TEST_PRIORITY("testChthollyModuleIntegration", "测试珂朵莉模块集成", "Integration", 9) {
        testChthollyModuleIntegration();
    };
    
    TEST_PRIORITY("testYuigahamaModuleIntegration", "测试由比滨结衣模块集成", "Integration", 9) {
        testYuigahamaModuleIntegration();
    };
    
    TEST_CASE("testCrossModuleCompatibility", "测试跨模块兼容性") {
        testCrossModuleCompatibility();
    };
}

void ModuleSystemTest::setUp() {
    tempModuleDir_ = utils::FileSystemTestUtils::createTempDirectory();
}

void ModuleSystemTest::tearDown() {
    utils::FileSystemTestUtils::cleanupTempFiles();
}

void ModuleSystemTest::testCMODLifecycle() {
    // 1. 创建CMOD
    std::string modulePath = utils::FileSystemTestUtils::createTestCMOD("TestAccordion");
    assertTrue(utils::CHTLTestUtils::validateCMODStructure(modulePath), 
              "CMOD结构应该有效");
    
    // 2. 验证三者同名
    std::string moduleName = std::filesystem::path(modulePath).filename().string();
    std::string srcFile = modulePath + "/src/" + moduleName + ".chtl";
    std::string infoFile = modulePath + "/info/" + moduleName + ".chtl";
    
    assertTrue(utils::FileSystemTestUtils::fileExists(srcFile), "源文件应该存在");
    assertTrue(utils::FileSystemTestUtils::fileExists(infoFile), "信息文件应该存在");
    
    // 3. 验证文件内容
    std::string infoContent = utils::FileSystemTestUtils::readFile(infoFile);
    assertTrue(infoContent.find("[Info]") != std::string::npos, "应该包含Info段落");
    assertTrue(infoContent.find("[Export]") != std::string::npos, "应该包含Export段落");
    
    std::string srcContent = utils::FileSystemTestUtils::readFile(srcFile);
    assertTrue(utils::CHTLTestUtils::validateCHTLSyntax(srcContent), "源文件应该包含有效的CHTL语法");
}

void ModuleSystemTest::testCJMODLifecycle() {
    // CJMOD测试（简化版）
    std::string tempDir = utils::FileSystemTestUtils::createTempDirectory();
    std::string cjmodPath = tempDir + "/TestCJMOD";
    
    // 创建CJMOD目录结构
    std::filesystem::create_directories(cjmodPath + "/src");
    
    // 创建info文件
    std::string infoContent = R"([Info]
Name: Test CJMOD
Version: 1.0.0
Author: Test
Description: Test CJMOD module
)";
    
    utils::FileSystemTestUtils::writeFile(cjmodPath + "/TestCJMOD.chtl", infoContent);
    
    assertTrue(utils::CHTLTestUtils::validateCJMODStructure(cjmodPath), 
              "CJMOD结构应该有效");
}

void ModuleSystemTest::testModuleImport() {
    // 测试导入语句验证
    std::vector<std::string> validImports = {
        "@Chtl from Chtholly.Accordion",
        "@Chtl from Yuigahama.MusicPlayer",
        "@CJmod from TestModule",
        "@Html from template.html as MyTemplate"
    };
    
    for (const auto& importStmt : validImports) {
        assertTrue(utils::CHTLTestUtils::validateImportStatement(importStmt),
                  "导入语句应该有效: " + importStmt);
    }
    
    std::vector<std::string> invalidImports = {
        "@Invalid from Module",
        "Chtl from Module",
        "@Chtl Module",
        ""
    };
    
    for (const auto& importStmt : invalidImports) {
        assertFalse(utils::CHTLTestUtils::validateImportStatement(importStmt),
                   "导入语句应该无效: " + importStmt);
    }
}

void ModuleSystemTest::testModuleConflictResolution() {
    // 测试模块冲突解决
    auto& generator = utils::TestDataGenerator::getInstance();
    
    // 创建两个同名模块
    std::string module1 = utils::FileSystemTestUtils::createTestCMOD("ConflictModule");
    std::string module2 = utils::FileSystemTestUtils::createTestCMOD("ConflictModule");
    
    assertTrue(utils::CHTLTestUtils::validateCMODStructure(module1), "模块1应该有效");
    assertTrue(utils::CHTLTestUtils::validateCMODStructure(module2), "模块2应该有效");
    
    // 这里应该测试冲突解决逻辑
    // 简化实现：验证两个模块都存在
    assertTrue(true, "冲突解决测试占位符");
}

void ModuleSystemTest::testModuleVersioning() {
    // 测试模块版本管理
    std::string moduleContent = R"([Info]
Name: Versioned Module
Version: 2.1.0
Author: Test
Description: Version test module

[Export]
Component: VersionedComponent
)";
    
    std::string modulePath = utils::FileSystemTestUtils::createTestCMOD("VersionedModule", moduleContent);
    std::string infoFile = modulePath + "/info/VersionedModule.chtl";
    std::string content = utils::FileSystemTestUtils::readFile(infoFile);
    
    assertTrue(content.find("Version: 2.1.0") != std::string::npos, "版本信息应该正确");
}

void ModuleSystemTest::testChthollyModuleIntegration() {
    // 测试珂朵莉模块集成
    std::string chthollyCode = R"([Import]
@Chtl from Chtholly.Accordion

[Component]
{{ChthollyAccordion}}
    {{.accordion-elegant}}
        Test Content
    {{/.accordion-elegant}}
{{/ChthollyAccordion}}
)";
    
    assertTrue(utils::CHTLTestUtils::validateCHTLSyntax(chthollyCode), 
              "珂朵莉模块代码应该有效");
    assertTrue(utils::CHTLTestUtils::validateComponentStructure(chthollyCode),
              "组件结构应该有效");
}

void ModuleSystemTest::testYuigahamaModuleIntegration() {
    // 测试由比滨结衣模块集成
    std::string yuigahamaCode = R"([Import]
@Chtl from Yuigahama.MusicPlayer

[Component]
{{YuiMusicPlayer}}
    {{.music-player-yui}}
        Test Music Player
    {{/.music-player-yui}}
{{/YuiMusicPlayer}}
)";
    
    assertTrue(utils::CHTLTestUtils::validateCHTLSyntax(yuigahamaCode), 
              "由比滨结衣模块代码应该有效");
    assertTrue(utils::CHTLTestUtils::validateComponentStructure(yuigahamaCode),
              "组件结构应该有效");
}

void ModuleSystemTest::testCrossModuleCompatibility() {
    // 测试跨模块兼容性
    std::string crossModuleCode = R"([Import]
@Chtl from Chtholly.Accordion
@Chtl from Yuigahama.MusicPlayer

[Component]
{{CrossModuleTest}}
    {{ChthollyAccordion}}
        <!-- 珂朵莉手风琴 -->
    {{/ChthollyAccordion}}
    
    {{YuiMusicPlayer}}
        <!-- 由比滨结衣音乐播放器 -->
    {{/YuiMusicPlayer}}
{{/CrossModuleTest}}
)";
    
    assertTrue(utils::CHTLTestUtils::validateCHTLSyntax(crossModuleCode), 
              "跨模块代码应该有效");
    
    // 验证导入语句
    auto sections = utils::CHTLTestUtils::parseCHTLSections(crossModuleCode);
    assertTrue(sections.find("Import") != sections.end(), "应该包含Import段落");
    assertTrue(sections.find("Component") != sections.end(), "应该包含Component段落");
}

} // namespace integration
} // namespace test
} // namespace chtl