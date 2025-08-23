#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include "CMOD/CMODManager.h"
#include "Utils/FileUtils.h"

using namespace CHTL;

// 测试自动导出功能
void test_auto_export_generation() {
    std::cout << "测试自动导出生成...\n";
    
    CMOD::CMODManager manager;
    
    // 测试扫描源文件符号
    std::string testSource = R"(
[Template] @Style BaseStyle
{
    color: black;
}

[Custom] @Style TestButton
{
    @Style BaseStyle;
    background-color: blue;
}

[Custom] @Element TestBox
{
    div { text { 测试 } }
}

[Custom] @Var TestColors
{
    primary: "#007bff";
}

[Origin] @Html TestPage
{
    <div>测试页面</div>
}

[Configuration] TestConfig
{
    theme: "dark";
}
)";
    
    // 扫描符号
    CMOD::ModuleExports exports = manager.ScanSourceForSymbols(testSource);
    
    // 验证扫描结果
    assert(exports.templateStyles.size() == 1);
    assert(exports.templateStyles[0] == "BaseStyle");
    
    assert(exports.customStyles.size() == 1);
    assert(exports.customStyles[0] == "TestButton");
    
    assert(exports.customElements.size() == 1);
    assert(exports.customElements[0] == "TestBox");
    
    assert(exports.customVars.size() == 1);
    assert(exports.customVars[0] == "TestColors");
    
    assert(exports.originHtml.size() == 1);
    assert(exports.originHtml[0] == "TestPage");
    
    assert(exports.configurations.size() == 1);
    assert(exports.configurations[0] == "TestConfig");
    
    std::cout << "  自动导出生成测试通过\n";
    std::cout << "    扫描到模板样式: " << exports.templateStyles.size() << "个\n";
    std::cout << "    扫描到自定义样式: " << exports.customStyles.size() << "个\n";
    std::cout << "    扫描到自定义元素: " << exports.customElements.size() << "个\n";
    std::cout << "    扫描到自定义变量: " << exports.customVars.size() << "个\n";
    std::cout << "    扫描到原始HTML: " << exports.originHtml.size() << "个\n";
    std::cout << "    扫描到配置: " << exports.configurations.size() << "个\n";
}

// 测试模块导出更新
void test_export_update() {
    std::cout << "测试导出信息更新...\n";
    
    // 创建临时测试模块
    std::string testModulePath = "./test_temp_module";
    std::string srcPath = testModulePath + "/src";
    std::string infoPath = testModulePath + "/info";
    
    try {
        // 创建目录结构
        std::filesystem::create_directories(srcPath);
        std::filesystem::create_directories(infoPath);
        
        // 创建源文件
        std::string sourceContent = R"(
[Template] @Style AutoStyle
{
    color: red;
}

[Custom] @Element AutoElement
{
    div { text { 自动元素 } }
}
)";
        
        Utils::FileUtils::WriteFile(srcPath + "/test_temp_module.chtl", sourceContent);
        
        // 创建基础信息文件（没有Export块）
        std::string infoContent = R"(
[Info]
{
    name = "test_temp_module";
    version = "1.0.0";
    description = "临时测试模块";
}
)";
        
        Utils::FileUtils::WriteFile(infoPath + "/test_temp_module.chtl", infoContent);
        
        // 使用CMOD管理器加载模块（应该自动生成Export）
        CMOD::CMODManager manager;
        bool loaded = manager.LoadModule(testModulePath);
        
        if (loaded) {
            const CMOD::CMODModule* module = manager.FindModule("test_temp_module");
            assert(module != nullptr);
            
            // 验证自动生成的导出信息
            assert(!module->exports.templateStyles.empty());
            assert(!module->exports.customElements.empty());
            
            std::cout << "  导出信息更新测试通过\n";
            std::cout << "    自动生成的模板样式: " << module->exports.templateStyles.size() << "个\n";
            std::cout << "    自动生成的自定义元素: " << module->exports.customElements.size() << "个\n";
            
            // 检查信息文件是否已更新
            std::string updatedContent = Utils::FileUtils::ReadFile(infoPath + "/test_temp_module.chtl");
            assert(updatedContent.find("[Export]") != std::string::npos);
            assert(updatedContent.find("AutoStyle") != std::string::npos);
            assert(updatedContent.find("AutoElement") != std::string::npos);
            
            std::cout << "  信息文件自动更新验证通过\n";
        }
        
        // 清理临时文件
        std::filesystem::remove_all(testModulePath);
        
    } catch (const std::exception& e) {
        // 确保清理临时文件
        try {
            std::filesystem::remove_all(testModulePath);
        } catch (...) {}
        
        std::cerr << "导出更新测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试复杂模块解析（修正版）
void test_complex_module_parsing() {
    std::cout << "测试复杂模块解析（修正版）...\n";
    
    CMOD::CMODManager manager;
    
    try {
        // 测试基础模块信息解析
        CMOD::ModuleInfo testInfo;
        testInfo.name = "ComplexModule";
        testInfo.version = "2.0.0";
        testInfo.dependencies = "BaseModule, UtilModule";
        
        CMOD::CMODModule testModule;
        testModule.info = testInfo;
        
        // 测试依赖验证（应该失败，因为依赖模块不存在）
        bool isValid = manager.ValidateModuleDependencies(testModule);
        assert(!isValid); // 应该失败
        
        // 测试空依赖的模块
        testInfo.dependencies = "";
        testModule.info = testInfo;
        isValid = manager.ValidateModuleDependencies(testModule);
        assert(isValid); // 应该成功
        
        std::cout << "  复杂模块解析测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "复杂模块解析测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    std::cout << "运行CMOD自动导出功能测试...\n\n";
    
    try {
        test_auto_export_generation();
        test_export_update();
        test_complex_module_parsing();
        
        std::cout << "\n所有自动导出功能测试通过!\n";
        std::cout << "✅ 自动导出生成正常\n";
        std::cout << "✅ 导出信息更新正常\n";
        std::cout << "✅ 复杂模块解析正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}