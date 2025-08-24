#include "CJMOD/PracticalCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateOriginalAPIStyle() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示原生API风格 - 真正实用的功能" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 1. 标准语法分析
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )");
    
    std::cout << "✅ 语法分析完成，识别到 " << keyword->args.length() << " 个参数" << std::endl;
    
    // 2. 绑定值处理器 - 不是统一功能，而是专门的值预处理
    keyword->args.bind<std::string>("$", [](const std::string& url) -> std::string {
        std::cout << "  处理URL值: " << url << std::endl;
        // 这里只处理值，不生成JS代码
        return url.find("http") == 0 ? url : "assets/" + url;
    });
    
    keyword->args.bind<std::string>("$", [](const std::string& mode) -> std::string {
        std::cout << "  处理模式值: " << mode << std::endl;
        return mode == "fit" ? "contain" : mode;
    });
    
    keyword->args.bind<int>("$", [](int width) -> std::string {
        std::cout << "  处理宽度值: " << width << std::endl;
        return std::to_string(width > 0 ? width : 800);
    });
    
    keyword->args.bind<int>("$", [](int height) -> std::string {
        std::cout << "  处理高度值: " << height << std::endl;
        return std::to_string(height > 0 ? height : 600);
    });
    
    keyword->args.bind<double>("$", [](double scale) -> std::string {
        std::cout << "  处理缩放值: " << scale << std::endl;
        return std::to_string(scale > 0 ? scale : 1.0);
    });
    
    // 3. JS代码转换 - 这是核心功能
    keyword->args.transform("$", "ImageLoader.load(\"${value}\")");
    keyword->args.transform("$", "DisplayMode.${value}");
    keyword->args.transform("$", "Math.max(${value}, 100)");
    keyword->args.transform("$", "Math.max(${value}, 100)");
    keyword->args.transform("$", "Math.min(Math.max(${value}, 0.1), 5.0)");
    
    // 4. 匹配原始值
    keyword->args.match("$", "profile.jpg");
    keyword->args.match("$", "fit");
    keyword->args.match("$", "1200");
    keyword->args.match("$", "800");
    keyword->args.match("$", "1.5");
    
    // 5. 生成最终的JS代码
    std::string result = keyword->result();
    std::cout << "\n🎯 生成的JS代码组合:\n" << result << std::endl;
}

void demonstratePeekKeywordFeature() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示peekKeyword功能 - 原生API的核心实用特性" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 创建扫描器并设置token流
    auto& scanner = getCJMODScanner();
    
    std::vector<std::string> tokens = {
        "const", "image", "=", "new", "Image", "(", ")", ";",
        "printMylove", "(", "{", "url", ":", "avatar.png", ",",
        "mode", ":", "center", "}", ")", ";",
        "console", ".", "log", "(", "image", ")", ";"
    };
    
    scanner.setTokens(tokens, 8);  // 设置位置到 "printMylove"
    
    // 创建语法
    auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });");
    
    // 绑定处理器，专门处理peekKeyword的结果
    keyword->args[0].bind<std::string>([](const std::string& value) -> std::string {
        return "\"" + value + "\"";
    });
    
    keyword->args[1].bind<std::string>([](const std::string& value) -> std::string {
        return "\"" + value + "\"";
    });
    
    // 设置JS转换模板
    keyword->args[0].transform("loadImage(${value})");
    keyword->args[1].transform("DisplayMode.${value}");
    
    // 演示peekKeyword的实用性
    std::cout << "\n=== peekKeyword相对位置访问 ===" << std::endl;
    
    // 查看相对位置的关键字
    std::string prevToken = scanner.peekKeyword(-1);  // 上一个token
    std::string nextToken = scanner.peekKeyword(1);   // 下一个token
    std::string farToken = scanner.peekKeyword(5);    // 向后5个位置
    
    // 使用peekKeyword结果
    std::cout << "\n使用peekKeyword结果处理参数:" << std::endl;
    std::string urlResult = keyword->args[0](scanner.peekKeyword(5));    // "avatar.png"
    std::string modeResult = keyword->args[1](scanner.peekKeyword(9));   // "center"
    
    std::cout << "\n🎯 peekKeyword处理结果:" << std::endl;
    std::cout << "URL参数: " << keyword->args[0].getJSCode() << std::endl;
    std::cout << "Mode参数: " << keyword->args[1].getJSCode() << std::endl;
}

void demonstratePolicyMode() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示Policy策略模式 - 扫描器行为控制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& scanner = getCJMODScanner();
    
    std::cout << "=== 收集策略演示 ===" << std::endl;
    
    // 开始收集策略
    scanner.policyChangeBegin(":", Policy::COLLECT);
    
    // 模拟收集过程
    scanner.processCollectPolicy("{ url: \"image.jpg\", mode: \"center\" }");
    
    // 结束收集策略并获取结果
    std::string collected = scanner.policyChangeEnd("}", Policy::NORMAL);
    
    std::cout << "\n收集到的内容: " << collected << std::endl;
    
    // 演示对象和函数判断
    std::cout << "\n=== 实用判断函数 ===" << std::endl;
    
    std::string objectTest = "{ name: 'test', value: 123 }";
    std::string functionTest = "function() { return true; }";
    std::string arrowTest = "() => console.log('hello')";
    
    std::cout << "isObject(\"" << objectTest << "\"): " << scanner.isObject(objectTest) << std::endl;
    std::cout << "isFunction(\"" << functionTest << "\"): " << scanner.isFunction(functionTest) << std::endl;
    std::cout << "isFunction(\"" << arrowTest << "\"): " << scanner.isFunction(arrowTest) << std::endl;
}

void demonstrateCHTLJSFunctionIntegration() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示CHTLJSFunction - 专门为CHTL JS设计的实用工具" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 创建专门的CHTL JS函数
    auto printMylove = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height"});
    
    // 绑定键处理器 - 专门处理CHTL JS函数的参数值
    printMylove->bindKeyProcessor<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  处理URL键: " << url << std::endl;
        return url.find("http") == 0 ? url : "assets/" + url;
    });
    
    printMylove->bindKeyProcessor<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  处理模式键: " << mode << std::endl;
        return mode == "fit" ? "contain" : mode;
    });
    
    printMylove->bindKeyProcessor<int>("width", [](int width) -> std::string {
        std::cout << "  处理宽度键: " << width << std::endl;
        return std::to_string(width > 0 ? width : 800);
    });
    
    printMylove->bindKeyProcessor<int>("height", [](int height) -> std::string {
        std::cout << "  处理高度键: " << height << std::endl;
        return std::to_string(height > 0 ? height : 600);
    });
    
    // 设置JS转换模板 - 专门为每个参数设置JS代码模板
    printMylove->transformKey("url", "ImageLoader.load(\"${value}\")");
    printMylove->transformKey("mode", "DisplayMode.${value}");
    printMylove->transformKey("width", "Math.max(${value}, 100)");
    printMylove->transformKey("height", "Math.max(${value}, 100)");
    
    // 与扫描器集成
    auto& scanner = getCJMODScanner();
    printMylove->scanKeyword(scanner);
    
    // 匹配关键字和值
    printMylove->matchKeyword("url", "profile.jpg");
    printMylove->matchKeyword("mode", "fit");
    printMylove->matchKeyword("width", "1200");
    printMylove->matchKeyword("height", "800");
    
    // 生成最终的JS代码
    std::string code = printMylove->generateCode();
    std::cout << "\n🎯 生成的CHTL JS代码:\n" << code << std::endl;
}

void demonstrateRealWorldUsage() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示真实世界使用场景 - 完整的CJMOD工作流程" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 场景：处理复杂的图像处理CHTL JS函数
    auto imageProcessor = createCHTLJSFunction("processImage", {"input", "filters", "output", "quality"});
    
    // 1. 绑定专门的值处理器
    imageProcessor->bindKeyProcessor<std::string>("input", [](const std::string& input) -> std::string {
        // 处理输入路径
        return input.find("/") == 0 ? input : "images/" + input;
    });
    
    imageProcessor->bindKeyProcessor<std::string>("filters", [](const std::string& filters) -> std::string {
        // 处理滤镜配置
        if (filters == "auto") return "blur,sharpen,enhance";
        return filters;
    });
    
    imageProcessor->bindKeyProcessor<std::string>("output", [](const std::string& output) -> std::string {
        // 处理输出路径
        return output.find("/") == 0 ? output : "processed/" + output;
    });
    
    imageProcessor->bindKeyProcessor<int>("quality", [](int quality) -> std::string {
        // 处理质量参数
        return std::to_string(std::max(1, std::min(100, quality)));
    });
    
    // 2. 设置专门的JS代码转换模板
    imageProcessor->transformKey("input", "ImageLoader.loadFromPath(\"${value}\")");
    imageProcessor->transformKey("filters", "FilterChain.parse(\"${value}\")");
    imageProcessor->transformKey("output", "OutputManager.setPath(\"${value}\")");
    imageProcessor->transformKey("quality", "QualitySettings.set(${value})");
    
    // 3. 模拟与统一扫描器的集成
    auto& scanner = getCJMODScanner();
    
    // 设置扫描上下文
    std::vector<std::string> sourceTokens = {
        "// 图像处理代码",
        "const", "canvas", "=", "setupCanvas", "(", ")", ";",
        "processImage", "(", "{",
        "input", ":", "raw_image.jpg", ",",
        "filters", ":", "auto", ",", 
        "output", ":", "final.jpg", ",",
        "quality", ":", "85",
        "}", ")", ";",
        "displayResult", "(", "canvas", ")", ";"
    };
    
    scanner.setTokens(sourceTokens, 8);  // 位置在 "processImage"
    
    // 4. 使用peekKeyword功能提取参数值
    std::cout << "\n=== 使用peekKeyword提取参数 ===" << std::endl;
    
    // 扫描并提取参数值（实际中会更复杂）
    imageProcessor->matchKeyword("input", scanner.peekKeyword(4));   // "raw_image.jpg"
    imageProcessor->matchKeyword("filters", scanner.peekKeyword(8)); // "auto"
    imageProcessor->matchKeyword("output", scanner.peekKeyword(12)); // "final.jpg"
    imageProcessor->matchKeyword("quality", scanner.peekKeyword(16)); // "85"
    
    // 5. 生成最终的优化JS代码
    std::string finalCode = imageProcessor->generateCode();
    
    std::cout << "\n🎯 完整的JS代码生成结果:" << std::endl;
    std::cout << finalCode << std::endl;
    
    // 6. 演示策略模式的实际应用
    std::cout << "\n=== 策略模式实际应用 ===" << std::endl;
    
    scanner.policyChangeBegin("{", Policy::COLLECT);
    std::string parametersBlock = scanner.policyChangeEnd("}", Policy::NORMAL);
    
    std::cout << "使用COLLECT策略收集到的参数块: " << parametersBlock << std::endl;
}

int main() {
    std::cout << "实用的CJMOD API演示程序" << std::endl;
    std::cout << "基于原生API设计理念，重点关注JS代码转换和实际使用场景" << std::endl;
    
    // 演示所有实用功能
    demonstrateOriginalAPIStyle();
    demonstratePeekKeywordFeature();
    demonstratePolicyMode();
    demonstrateCHTLJSFunctionIntegration();
    demonstrateRealWorldUsage();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示程序完成" << std::endl;
    std::cout << "核心特性总结:" << std::endl;
    std::cout << "1. ✅ 专门的JS代码转换 (transform方法)" << std::endl;
    std::cout << "2. ✅ peekKeyword相对位置访问" << std::endl;
    std::cout << "3. ✅ Policy策略模式 (COLLECT/SKIP/NORMAL)" << std::endl;
    std::cout << "4. ✅ 与统一扫描器深度集成" << std::endl;
    std::cout << "5. ✅ 不统一功能，各司其职" << std::endl;
    std::cout << "6. ✅ 真正实用的CJMOD API！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}