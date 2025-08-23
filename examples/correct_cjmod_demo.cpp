#include "CJMOD/CorrectCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateCorrectDualPointerScanning() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示正确的双指针扫描机制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 模拟源代码，包含普通JavaScript和CJMOD语法
    std::string sourceCode = R"(
        function normalFunction() {
            console.log("这是普通代码");
            var x = 10;
        }
        
        // 这里是CJMOD语法
        printMylove({
            url: "image.jpg",
            mode: "center",
            width: 800,
            height: 600
        });
        
        function anotherFunction() {
            return "更多普通代码";
        }
        
        // 另一个CJMOD语法，演示arg ** arg情况
        arg myArg ** processArg({
            value: "test",
            type: "string"
        });
    )";
    
    // 设置CJMOD扫描关键字
    std::vector<std::string> keywords = {"printMylove", "**", "processArg"};
    
    // 创建双指针扫描器
    DualPointerScanner scanner(sourceCode, keywords);
    
    // 执行扫描
    auto result = scanner.scan();
    
    std::cout << "\n=== 扫描结果分析 ===" << std::endl;
    std::cout << "普通代码片段数量: " << result.normalCodeSegments.size() << std::endl;
    std::cout << "CJMOD代码片段数量: " << result.cjmodCodeSegments.size() << std::endl;
    
    for (size_t i = 0; i < result.normalCodeSegments.size(); ++i) {
        std::cout << "\n普通代码片段 " << (i+1) << ":" << std::endl;
        std::cout << result.normalCodeSegments[i].substr(0, 100) << "..." << std::endl;
    }
    
    for (size_t i = 0; i < result.cjmodCodeSegments.size(); ++i) {
        std::cout << "\nCJMOD代码片段 " << (i+1) << " (位置 " << result.cjmodPositions[i] << "):" << std::endl;
        std::cout << result.cjmodCodeSegments[i] << std::endl;
    }
}

void demonstrateFrontExtractMechanism() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示前置截取机制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 模拟统一扫描器收集的代码片段，包含需要前置截取的情况
    std::string codeSegment = "arg myParameter ** processFunction({ value: test, mode: auto });";
    
    std::cout << "原始代码片段: " << codeSegment << std::endl;
    
    // 创建前置截取机制
    // 触发关键字是"**"，需要截取的模式是"arg"
    FrontExtractMechanism extractor("**", "arg");
    
    // 执行前置截取
    auto extractResult = extractor.extract(codeSegment);
    
    std::cout << "\n=== 前置截取结果 ===" << std::endl;
    
    if (extractResult.hasExtraction) {
        std::cout << "✅ 成功进行前置截取" << std::endl;
        std::cout << "前置部分（不发送给编译器）: \"" << extractResult.frontPart << "\"" << std::endl;
        std::cout << "剩余部分（发送给编译器）: \"" << extractResult.remainingPart << "\"" << std::endl;
        
        std::cout << "\n解释：" << std::endl;
        std::cout << "- 统一扫描器扫描到'**'时才启动CJMOD收集" << std::endl;
        std::cout << "- 但'arg myParameter'已经在统一扫描器的片段中" << std::endl;
        std::cout << "- 前置截取机制将'arg myParameter **'部分截取出来" << std::endl;
        std::cout << "- 避免将参数定义部分发送给编译器" << std::endl;
    } else {
        std::cout << "❌ 未进行前置截取" << std::endl;
        std::cout << "完整片段: \"" << extractResult.remainingPart << "\"" << std::endl;
    }
}

void demonstrateCorrectAPI() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示重构后的CJMOD API使用" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 1. 创建CHTLJSFunction - 基于正确理解
    CHTLJSFunction function("printMylove", {"url", "mode", "width", "height", "scale"}, true);
    
    // 2. 设置前置截取规则
    function.setFrontExtractRule("**", "arg");
    
    // 3. 绑定键处理器
    function.bindKeyProcessor<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  处理URL参数: " << url << std::endl;
        return "\"" + url + "\"";
    });
    
    function.bindKeyProcessor<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  处理模式参数: " << mode << std::endl;
        return "\"" + mode + "\"";
    });
    
    function.bindKeyProcessor<int>("width", [](int width) -> std::string {
        std::cout << "  处理宽度参数: " << width << std::endl;
        return std::to_string(width);
    });
    
    function.bindKeyProcessor<int>("height", [](int height) -> std::string {
        std::cout << "  处理高度参数: " << height << std::endl;
        return std::to_string(height);
    });
    
    // 4. 创建语法对象
    auto syntax = function.createSyntax();
    
    // 5. 模拟参数匹配（在实际使用中，这些值会从代码扫描中获得）
    syntax->args.match("url", "avatar.png");
    syntax->args.match("mode", "center");
    syntax->args.match("width", "800");
    syntax->args.match("height", "600");
    syntax->args.match("scale", "1.0");
    
    // 6. 生成最终的JavaScript代码
    std::string generatedCode = function.generateCode(*syntax);
    
    std::cout << "\n=== 生成的JavaScript代码 ===" << std::endl;
    std::cout << generatedCode << std::endl;
    
    // 7. 演示双指针扫描处理
    std::string sourceWithCJMOD = R"(
        var element = document.getElementById("container");
        
        arg imageUrl ** printMylove({
            url: "avatar.png",
            mode: "center",
            width: 800,
            height: 600,
            scale: 1.0
        });
        
        console.log("处理完成");
    )";
    
    std::cout << "\n=== 双指针扫描处理源代码 ===" << std::endl;
    std::string processedCode = function.processWithDualPointer(sourceWithCJMOD);
    
    std::cout << "\n处理后的代码:" << std::endl;
    std::cout << processedCode << std::endl;
}

void demonstrateOriginalAPICompatibility() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示与原生API的完全兼容性" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 完全按照原生API的使用方式
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ",:{};()");
    
    std::cout << "语法分析完成，参数数量: " << keyword->args.length() << std::endl;
    
    // 绑定处理器 - 完全兼容原生API语法
    keyword->args.bind<std::string>("$", [](const std::string& url) -> std::string {
        std::cout << "  绑定处理第1个参数(URL): " << url << std::endl;
        return "loadImage(\"" + url + "\")";
    });
    
    keyword->args.bind<std::string>("$", [](const std::string& mode) -> std::string {
        std::cout << "  绑定处理第2个参数(模式): " << mode << std::endl;
        return "\"" + mode + "\"";
    });
    
    keyword->args.bind<int>("$", [](int width) -> std::string {
        std::cout << "  绑定处理第3个参数(宽度): " << width << std::endl;
        return std::to_string(width);
    });
    
    keyword->args.bind<int>("$", [](int height) -> std::string {
        std::cout << "  绑定处理第4个参数(高度): " << height << std::endl;
        return std::to_string(height);
    });
    
    keyword->args.bind<double>("$", [](double scale) -> std::string {
        std::cout << "  绑定处理第5个参数(缩放): " << scale << std::endl;
        return std::to_string(scale);
    });
    
    // 模拟参数匹配
    keyword->args.match("$", "profile.jpg");
    keyword->args.match("$", "fit");
    keyword->args.match("$", "1024");
    keyword->args.match("$", "768");
    keyword->args.match("$", "1.5");
    
    // 生成结果
    std::string result = keyword->result();
    std::cout << "\n生成的结果: " << result << std::endl;
    
    // 演示operator()重载使用
    std::cout << "\n=== 演示operator()重载 ===" << std::endl;
    std::cout << "keyword->args[0](\"new_image.png\"): " << keyword->args[0]("new_image.png") << std::endl;
    std::cout << "keyword->args[2](\"2048\"): " << keyword->args[2]("2048") << std::endl;
}

int main() {
    std::cout << "CJMOD API 正确实现演示程序" << std::endl;
    std::cout << "基于正确的双指针扫描机制和前置截取机制\n" << std::endl;
    
    // 演示各个功能
    demonstrateCorrectDualPointerScanning();
    demonstrateFrontExtractMechanism();
    demonstrateCorrectAPI();
    demonstrateOriginalAPICompatibility();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示程序完成" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}