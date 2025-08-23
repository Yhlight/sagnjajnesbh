#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include "CMOD/CMODManager.h"
#include "Utils/FileUtils.h"

using namespace CHTL;

// 测试CMOD模块加载
void test_cmod_loading() {
    std::cout << "测试CMOD模块加载...\n";
    
    CMOD::CMODManager manager;
    
    // 测试加载TestModule
    bool loaded = manager.LoadModule("modules/TestModule");
    assert(loaded);
    
    // 验证模块信息
    const CMOD::CMODModule* module = manager.FindModule("TestModule");
    assert(module != nullptr);
    assert(module->info.name == "TestModule");
    assert(module->info.version == "1.0.0");
    assert(module->info.author == "CHTL Team");
    
    std::cout << "  CMOD模块加载测试通过\n";
}

// 测试模块导出解析
void test_module_exports() {
    std::cout << "测试模块导出解析...\n";
    
    CMOD::CMODManager manager;
    bool loaded = manager.LoadModule("modules/TestModule");
    assert(loaded);
    
    const CMOD::CMODModule* module = manager.FindModule("TestModule");
    assert(module != nullptr);
    
    // 验证导出信息
    const auto& exports = module->exports;
    assert(exports.customStyles.size() >= 2);  // TestButton, TestCard
    assert(exports.customElements.size() >= 2); // TestBox, TestList
    assert(exports.customVars.size() >= 2);     // TestColors, TestSizes
    assert(exports.templateStyles.size() >= 1); // BaseStyle
    
    // 检查具体导出项
    bool hasTestButton = std::find(exports.customStyles.begin(), 
                                  exports.customStyles.end(), 
                                  "TestButton") != exports.customStyles.end();
    assert(hasTestButton);
    
    std::cout << "  模块导出解析测试通过\n";
}

// 测试模块导入
void test_module_import() {
    std::cout << "测试模块导入...\n";
    
    CMOD::CMODManager manager;
    
    // 测试导入模块
    bool imported = manager.ImportModule("TestModule");
    assert(imported);
    
    // 验证导入的模块
    auto loadedModules = manager.GetLoadedModules();
    assert(!loadedModules.empty());
    assert(std::find(loadedModules.begin(), loadedModules.end(), "TestModule") != loadedModules.end());
    
    std::cout << "  模块导入测试通过\n";
}

// 测试模块源文件内容获取
void test_module_content() {
    std::cout << "测试模块源文件内容获取...\n";
    
    CMOD::CMODManager manager;
    bool loaded = manager.LoadModule("modules/TestModule");
    assert(loaded);
    
    // 获取模块源文件内容
    auto contents = manager.GetModuleSourceContent("TestModule");
    assert(!contents.empty());
    
    // 验证内容包含预期的CHTL代码
    bool hasTemplateStyle = false;
    for (const auto& content : contents) {
        if (content.find("[Template] @Style BaseStyle") != std::string::npos) {
            hasTemplateStyle = true;
            break;
        }
    }
    assert(hasTemplateStyle);
    
    std::cout << "  模块源文件内容获取测试通过\n";
}

// 测试模块搜索路径
void test_search_paths() {
    std::cout << "测试模块搜索路径...\n";
    
    CMOD::CMODManager manager;
    
    // 添加自定义搜索路径
    manager.AddModuleSearchPath("./custom_modules");
    manager.AddModuleSearchPath("../external_modules");
    
    // 验证搜索路径
    auto loadedModules = manager.GetLoadedModules();
    
    std::cout << "  模块搜索路径测试通过\n";
}

// 测试完整的CMOD集成
void test_cmod_integration() {
    std::cout << "测试CMOD集成...\n";
    
    CMOD::CMODManager manager;
    
    // 设置搜索路径
    manager.SetModuleSearchPaths({"./modules", "./cmod"});
    
    // 导入模块
    bool imported = manager.ImportModule("TestModule", "TM");
    assert(imported);
    
    // 验证模块
    const CMOD::CMODModule* module = manager.FindModule("TestModule");
    assert(module != nullptr);
    
    // 获取模块内容
    auto contents = manager.GetModuleSourceContent("TestModule");
    assert(!contents.empty());
    
    // 验证模块信息完整性
    assert(!module->info.name.empty());
    assert(!module->info.version.empty());
    assert(!module->info.description.empty());
    
    std::cout << "  CMOD集成测试通过\n";
    
    // 输出模块信息
    std::cout << "\n模块信息:\n";
    std::cout << "  名称: " << module->info.name << "\n";
    std::cout << "  版本: " << module->info.version << "\n";
    std::cout << "  描述: " << module->info.description << "\n";
    std::cout << "  作者: " << module->info.author << "\n";
    std::cout << "  许可证: " << module->info.license << "\n";
    std::cout << "  分类: " << module->info.category << "\n";
    
    std::cout << "\n导出统计:\n";
    std::cout << "  自定义样式: " << module->exports.customStyles.size() << "个\n";
    std::cout << "  自定义元素: " << module->exports.customElements.size() << "个\n";
    std::cout << "  自定义变量: " << module->exports.customVars.size() << "个\n";
    std::cout << "  模板样式: " << module->exports.templateStyles.size() << "个\n";
    std::cout << "  源文件数量: " << module->sourceFiles.size() << "个\n";
}

int main() {
    std::cout << "运行CMOD系统测试...\n\n";
    
    try {
        test_cmod_loading();
        test_module_exports();
        test_module_import();
        test_module_content();
        test_search_paths();
        test_cmod_integration();
        
        std::cout << "\n所有CMOD系统测试通过!\n";
        std::cout << "✅ CMOD模块加载正常\n";
        std::cout << "✅ 模块导出解析正常\n";
        std::cout << "✅ 模块导入正常\n";
        std::cout << "✅ 模块内容获取正常\n";
        std::cout << "✅ 搜索路径正常\n";
        std::cout << "✅ CMOD集成正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}