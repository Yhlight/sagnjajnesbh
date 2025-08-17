#include "../common/ImportManager.h"
#include "../parser/EnhancedImportParser.h"
#include "../common/CompilerCore.h"
#include <iostream>
#include <string>

using namespace chtl;
using namespace chtl::parser;

// 示例：演示各种导入语法的处理
void demonstrateImportSyntax() {
    std::cout << "=== 增强Import功能示例 ===\n\n";
    
    // 创建编译器核心和上下文管理器
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    
    // 创建ImportManager
    ImportManager import_manager(context_manager.getCompilerContext());
    
    std::cout << "1. 测试HTML导入\n";
    
    // HTML导入测试
    {
        // 无as语法 - 应该被跳过
        auto html_imports1 = import_manager.processHtmlImport("header", "");
        std::cout << "   无as语法的HTML导入结果: " << html_imports1.size() << " 个文件\n";
        
        // 有as语法 - 应该创建原始嵌入节点
        auto html_imports2 = import_manager.processHtmlImport("header", "HeaderContent");
        std::cout << "   有as语法的HTML导入结果: " << html_imports2.size() << " 个文件\n";
        
        // 具体文件名导入
        auto html_imports3 = import_manager.processHtmlImport("header.html", "HeaderHtml");
        std::cout << "   具体文件名HTML导入结果: " << html_imports3.size() << " 个文件\n";
    }
    
    std::cout << "\n2. 测试Style导入\n";
    
    // Style导入测试
    {
        auto style_imports1 = import_manager.processStyleImport("main", "MainStyles");
        std::cout << "   Style导入结果: " << style_imports1.size() << " 个文件\n";
        
        auto style_imports2 = import_manager.processStyleImport("components.css", "ComponentStyles");
        std::cout << "   具体CSS文件导入结果: " << style_imports2.size() << " 个文件\n";
    }
    
    std::cout << "\n3. 测试JavaScript导入\n";
    
    // JavaScript导入测试
    {
        auto js_imports1 = import_manager.processJavaScriptImport("utils", "UtilsScript");
        std::cout << "   JavaScript导入结果: " << js_imports1.size() << " 个文件\n";
        
        auto js_imports2 = import_manager.processJavaScriptImport("app.js", "AppScript");
        std::cout << "   具体JS文件导入结果: " << js_imports2.size() << " 个文件\n";
    }
    
    std::cout << "\n4. 测试CHTL导入\n";
    
    // CHTL导入测试
    {
        // 名称（不带后缀）- 优先搜索cmod文件
        auto chtl_imports1 = import_manager.processChtlImport("UIComponents", "");
        std::cout << "   CHTL模块导入结果: " << chtl_imports1.size() << " 个文件\n";
        
        // 具体名称（带后缀）
        auto chtl_imports2 = import_manager.processChtlImport("UIComponents.cmod", "");
        std::cout << "   具体CMOD文件导入结果: " << chtl_imports2.size() << " 个文件\n";
        
        // 通配符导入
        auto chtl_imports3 = import_manager.processChtlImport("components/*", "");
        std::cout << "   通配符CHTL导入结果: " << chtl_imports3.size() << " 个文件\n";
        
        // 点号路径转换
        auto chtl_imports4 = import_manager.processChtlImport("Chtholly.Space", "");
        std::cout << "   点号路径CHTL导入结果: " << chtl_imports4.size() << " 个文件\n";
    }
    
    std::cout << "\n5. 测试CJmod导入\n";
    
    // CJmod导入测试
    {
        auto cjmod_imports1 = import_manager.processCJmodImport("ThirdPartyLib", "");
        std::cout << "   CJmod模块导入结果: " << cjmod_imports1.size() << " 个文件\n";
        
        auto cjmod_imports2 = import_manager.processCJmodImport("ThirdPartyLib.cjmod", "");
        std::cout << "   具体CJmod文件导入结果: " << cjmod_imports2.size() << " 个文件\n";
    }
    
    // 显示导入统计
    std::cout << "\n6. 导入统计信息\n";
    std::cout << import_manager.getImportStatistics() << "\n";
    
    // 显示错误和警告
    auto errors = import_manager.getImportErrors();
    auto warnings = import_manager.getImportWarnings();
    
    std::cout << "\n7. 导入错误和警告\n";
    std::cout << "错误数量: " << errors.size() << "\n";
    for (const auto& error : errors) {
        std::cout << "   - " << error << "\n";
    }
    
    std::cout << "警告数量: " << warnings.size() << "\n";
    for (const auto& warning : warnings) {
        std::cout << "   - " << warning << "\n";
    }
}

// 示例：演示循环依赖检测
void demonstrateCircularDependencyDetection() {
    std::cout << "\n=== 循环依赖检测示例 ===\n\n";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    ImportManager import_manager(context_manager.getCompilerContext());
    
    // 模拟文件依赖关系
    import_manager.addDependency("A.chtl", "B.chtl");
    import_manager.addDependency("B.chtl", "C.chtl");
    import_manager.addDependency("C.chtl", "A.chtl"); // 创建循环依赖
    
    std::cout << "1. 检测循环依赖\n";
    bool has_cycle = import_manager.detectCircularDependency("A.chtl", "C.chtl");
    std::cout << "   A.chtl -> C.chtl 循环依赖: " << (has_cycle ? "是" : "否") << "\n";
    
    if (has_cycle) {
        auto cycle_chain = import_manager.getCircularDependencyChain("A.chtl");
        std::cout << "   循环依赖链: ";
        for (size_t i = 0; i < cycle_chain.size(); ++i) {
            std::cout << cycle_chain[i];
            if (i < cycle_chain.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
    }
    
    std::cout << "2. 依赖图信息\n";
    import_manager.printDependencyGraph();
}

// 示例：演示重复导入处理
void demonstrateDuplicateImportHandling() {
    std::cout << "\n=== 重复导入处理示例 ===\n\n";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    ImportManager import_manager(context_manager.getCompilerContext());
    
    std::cout << "1. 测试重复导入检测\n";
    
    // 第一次导入
    auto imports1 = import_manager.processHtmlImport("header.html", "HeaderContent");
    std::cout << "   第一次导入 header.html: " << imports1.size() << " 个文件\n";
    
    // 第二次导入相同文件
    auto imports2 = import_manager.processHtmlImport("header.html", "HeaderContent");
    std::cout << "   第二次导入 header.html: " << imports2.size() << " 个文件\n";
    
    // 用不同别名导入相同文件
    auto imports3 = import_manager.processHtmlImport("header.html", "AnotherHeaderContent");
    std::cout << "   用不同别名导入 header.html: " << imports3.size() << " 个文件\n";
    
    std::cout << "2. 导入缓存信息\n";
    std::cout << "   缓存大小: " << import_manager.getImportCacheSize() << "\n";
}

// 示例：演示通配符导入
void demonstrateWildcardImport() {
    std::cout << "\n=== 通配符导入示例 ===\n\n";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    ImportManager import_manager(context_manager.getCompilerContext());
    
    std::cout << "1. 测试通配符导入语法\n";
    
    // 测试各种通配符语法
    std::vector<std::string> wildcard_paths = {
        "components/*",        // 导入components目录下所有文件
        "components/*.cmod",   // 导入components目录下所有.cmod文件
        "components/*.chtl",   // 导入components目录下所有.chtl文件
        "Chtholly.*",          // 导入Chtholly模块的所有子模块
        "UI.Components.*"      // 导入UI.Components下的所有子模块
    };
    
    for (const std::string& path : wildcard_paths) {
        auto imports = import_manager.processChtlImport(path, "");
        std::cout << "   通配符路径 '" << path << "': " << imports.size() << " 个文件\n";
    }
}

// 示例：演示完整的CHTL导入场景
void demonstrateCompleteImportScenario() {
    std::cout << "\n=== 完整CHTL导入场景示例 ===\n\n";
    
    // 创建一个完整的CHTL文件，包含各种导入语句
    std::string chtl_code = R"(
        // HTML资源导入
        [Import] @Html from header as HeaderContent
        [Import] @Html from footer.html as FooterContent
        
        // CSS样式导入
        [Import] @Style from main as MainStyles
        [Import] @Style from components.css as ComponentStyles
        
        // JavaScript脚本导入
        [Import] @JavaScript from utils as UtilsScript
        [Import] @JavaScript from app.js as AppScript
        
        // CHTL模块导入
        [Import] @Chtl from UIComponents
        [Import] @Chtl from UIComponents.cmod
        [Import] @Chtl from components/*
        [Import] @Chtl from Chtholly.Space
        
        // CJmod模块导入
        [Import] @CJmod from ThirdPartyLib
        [Import] @CJmod from ThirdPartyLib.cjmod
        
        // 模板导入
        [Import] [Template] @Style ButtonStyles from ui.templates
        [Import] [Template] @Element Button from ui.templates as CustomButton
        
        // 自定义导入
        [Import] [Custom] @Element AdvancedButton from ui.custom as MyButton
        
        html
        {
            head
            {
                [Origin] @Style MainStyles;
                [Origin] @Style ComponentStyles;
            }
            
            body
            {
                [Origin] @Html HeaderContent;
                
                div
                {
                    class: content;
                    @Element CustomButton;
                    @Element MyButton;
                }
                
                [Origin] @Html FooterContent;
                
                script
                {
                    [Origin] @JavaScript UtilsScript;
                    [Origin] @JavaScript AppScript;
                }
            }
        }
    )";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    
    std::cout << "1. 编译包含各种导入的CHTL代码\n";
    auto result = compiler->compileFromString(chtl_code);
    
    std::cout << "2. 编译结果\n";
    std::cout << "   成功: " << (result.success ? "是" : "否") << "\n";
    std::cout << "   错误数量: " << result.errors.size() << "\n";
    std::cout << "   警告数量: " << result.warnings.size() << "\n";
    
    if (!result.success) {
        std::cout << "3. 编译错误详情\n";
        for (const auto& error : result.errors) {
            std::cout << "   - " << error << "\n";
        }
    }
    
    if (!result.warnings.empty()) {
        std::cout << "4. 编译警告详情\n";
        for (const auto& warning : result.warnings) {
            std::cout << "   - " << warning << "\n";
        }
    }
    
    std::cout << "5. 编译统计\n";
    std::cout << compiler->getCompilationStatistics() << "\n";
}

int main() {
    try {
        // 运行所有示例
        demonstrateImportSyntax();
        demonstrateCircularDependencyDetection();
        demonstrateDuplicateImportHandling();
        demonstrateWildcardImport();
        demonstrateCompleteImportScenario();
        
        std::cout << "\n=== 所有导入示例完成 ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "示例执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}