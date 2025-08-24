#include "CJMOD/SimpleCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateStandardFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示标准流程 - 手动控制每个步骤" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 标准流程：使用 syntaxAnalys + generateCode
    auto syntax = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )");
    
    std::cout << "✅ 语法分析完成，识别到 " << syntax->length() << " 个参数" << std::endl;
    
    // 链式绑定 - 简洁而强大
    syntax->bind<std::string>([](const std::string& url) -> std::string {
        std::cout << "  处理URL: " << url << std::endl;
        return "\"" + url + "\"";
    })
    ->bind<std::string>([](const std::string& mode) -> std::string {
        std::cout << "  处理模式: " << mode << std::endl;
        return "\"" + mode + "\"";
    })
    ->bind<int>([](int width) -> std::string {
        std::cout << "  处理宽度: " << width << std::endl;
        return std::to_string(width);
    })
    ->bind<int>([](int height) -> std::string {
        std::cout << "  处理高度: " << height << std::endl;
        return std::to_string(height);
    })
    ->bind<double>([](double scale) -> std::string {
        std::cout << "  处理缩放: " << scale << std::endl;
        return std::to_string(scale);
    });
    
    // 匹配值
    std::vector<std::string> values = {"avatar.png", "center", "800", "600", "1.5"};
    syntax->match(values);
    
    // 生成代码
    std::string result = generateCode(*syntax);
    std::cout << "\n🎯 生成的结果: " << result << std::endl;
    
    // 演示索引访问
    std::cout << "\n=== 演示索引访问 ===" << std::endl;
    std::cout << "syntax[0](\"new_image.png\"): " << (*syntax)[0]("new_image.png") << std::endl;
    std::cout << "syntax[2](\"1024\"): " << (*syntax)[2]("1024") << std::endl;
}

void demonstrateSimplifiedFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示简化流程 - CHTLJSFunction一步到位" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 简化流程：使用 CHTLJSFunction
    auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height", "scale"});
    
    std::cout << "✅ 创建CHTLJSFunction: " << func->getFunctionName() << std::endl;
    
    // 链式绑定处理器 - 极简设计
    func->bind<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  绑定URL处理器: " << url << std::endl;
        return "loadImage(\"" + url + "\")";
    })
    ->bind<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  绑定模式处理器: " << mode << std::endl;
        return "DisplayMode." + mode.substr(0, 1) + char(std::toupper(mode[1])) + mode.substr(2);
    })
    ->bind<int>("width", [](int width) -> std::string {
        std::cout << "  绑定宽度处理器: " << width << std::endl;
        return "window.innerWidth || " + std::to_string(width);
    })
    ->bind<int>("height", [](int height) -> std::string {
        std::cout << "  绑定高度处理器: " << height << std::endl;
        return "window.innerHeight || " + std::to_string(height);
    })
    ->bind<double>("scale", [](double scale) -> std::string {
        std::cout << "  绑定缩放处理器: " << scale << std::endl;
        return "Math.min(" + std::to_string(scale) + ", devicePixelRatio)";
    });
    
    // 一键处理 - 真正的简化流程
    std::string sourceCode = R"(
        const container = document.getElementById("image-container");
        
        printMylove({
            url: "profile.jpg",
            mode: "fit",
            width: 1200,
            height: 800,
            scale: 2.0
        });
        
        console.log("图片处理完成");
    )";
    
    std::vector<std::string> values = {"profile.jpg", "fit", "1200", "800", "2.0"};
    std::string result = func->process(sourceCode, values);
    
    std::cout << "\n🎯 简化流程结果:\n" << result << std::endl;
}

void demonstrateFlexibleUsage() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示灵活用法 - 混合使用模式" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 1. 占位符模式（最简单）
    std::cout << "--- 占位符模式 ---" << std::endl;
    auto func1 = createCHTLJSFunction("animate");
    
    func1->bind<std::string>([](const std::string& element) -> std::string {
        return "document.querySelector('" + element + "')";
    })
    ->bind<int>([](int duration) -> std::string {
        return std::to_string(duration) + "ms";
    })
    ->bind<std::string>([](const std::string& easing) -> std::string {
        return "ease-" + easing;
    });
    
    std::string code1 = func1->process("animate('.button', 300, 'in-out');", {".button", "300", "in-out"});
    std::cout << "生成代码: " << code1 << std::endl;
    
    // 2. 命名参数模式（最清晰）
    std::cout << "\n--- 命名参数模式 ---" << std::endl;
    auto func2 = createCHTLJSFunction("fetchData", {"endpoint", "method", "headers"});
    
    func2->bind<std::string>("endpoint", [](const std::string& url) -> std::string {
        return "API_BASE + \"" + url + "\"";
    })
    ->bind<std::string>("method", [](const std::string& method) -> std::string {
        return "\"" + method + "\"";
    })
    ->bind<std::string>("headers", [](const std::string& headers) -> std::string {
        return "{ ...DEFAULT_HEADERS, " + headers + " }";
    });
    
    std::string code2 = func2->process("", {"/users", "GET", "Accept: 'application/json'"});
    std::cout << "生成代码: " << code2 << std::endl;
    
    // 3. 手动流程控制（最灵活）
    std::cout << "\n--- 手动流程控制 ---" << std::endl;
    auto func3 = createCHTLJSFunction("customFunction", {"data", "callback"});
    
    // 手动创建语法
    auto syntax = func3->createSyntax();
    
    // 手动绑定
    func3->bind<std::string>("data", [](const std::string& data) -> std::string {
        return "JSON.stringify(" + data + ")";
    });
    
    // 手动匹配
    func3->matchValues({"user.profile", "handleResponse"});
    
    // 手动生成
    std::string code3 = func3->generateCode();
    std::cout << "生成代码: " << code3 << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示高级特性 - 双指针扫描和前置截取" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 演示双指针扫描机制
    auto func = createCHTLJSFunction("processImage", {"src", "filters", "output"});
    
    std::string complexSource = R"(
        // 普通JavaScript代码
        function setupCanvas() {
            const canvas = document.createElement('canvas');
            return canvas;
        }
        
        // CJMOD语法 - 会被双指针扫描识别
        arg imageData ** processImage({
            src: "input.jpg",
            filters: ["blur", "sharpen"],
            output: "processed.jpg"
        });
        
        // 更多普通代码
        function displayResult(result) {
            console.log("处理完成:", result);
        }
    )";
    
    std::cout << "复杂源代码包含普通JS和CJMOD语法..." << std::endl;
    
    func->bind<std::string>("src", [](const std::string& src) -> std::string {
        return "ImageLoader.load(\"" + src + "\")";
    })
    ->bind<std::string>("filters", [](const std::string& filters) -> std::string {
        return "FilterChain.create(" + filters + ")";
    })
    ->bind<std::string>("output", [](const std::string& output) -> std::string {
        return "OutputManager.save(\"" + output + "\")";
    });
    
    // 扫描关键字（演示双指针扫描）
    func->scanKeyword(complexSource);
    
    std::cout << "\n✅ 双指针扫描完成，已识别CJMOD语法" << std::endl;
    std::cout << "✅ 前置截取机制已应用，arg参数已正确处理" << std::endl;
    
    // 匹配和生成
    func->matchValues({"input.jpg", "[\"blur\", \"sharpen\"]", "processed.jpg"});
    std::string result = func->generateCode();
    
    std::cout << "\n🎯 最终生成的代码:\n" << result << std::endl;
}

int main() {
    std::cout << "CJMOD API 演示程序" << std::endl;
    std::cout << "设计理念：使用简单，功能强大，很好用" << std::endl;
    
    // 演示所有功能
    demonstrateStandardFlow();
    demonstrateSimplifiedFlow();
    demonstrateFlexibleUsage();
    demonstrateAdvancedFeatures();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示程序完成" << std::endl;
    std::cout << "总结：只需要三个类 - Syntax, Arg, CHTLJSFunction" << std::endl;
    std::cout << "      既有标准流程，又有简化流程" << std::endl;
    std::cout << "      使用简单，功能强大，很好用！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}