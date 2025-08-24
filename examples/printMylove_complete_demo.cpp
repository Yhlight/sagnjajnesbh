#include "CJMOD/PracticalCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateStandardFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "标准流程：printMylove 手动控制每个步骤" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 步骤1：语法分析
    std::cout << "=== 步骤1：语法分析 ===" << std::endl;
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ",:{};()");
    
    std::cout << "✅ 语法分析完成，识别到 " << keyword->args.length() << " 个参数" << std::endl;
    
    // 步骤2：绑定值处理器
    std::cout << "\n=== 步骤2：绑定值处理器 ===" << std::endl;
    
    // URL参数处理器
    keyword->args.bind<std::string>("$", [](const std::string& url) -> std::string {
        std::cout << "  处理URL: " << url << std::endl;
        // 处理URL路径，添加前缀或验证
        if (url.find("http") == 0) {
            return url;  // 已经是完整URL
        } else if (url.find("/") == 0) {
            return url;  // 已经是绝对路径
        } else {
            return "assets/images/" + url;  // 添加默认路径
        }
    });
    
    // 模式参数处理器
    keyword->args.bind<std::string>("$", [](const std::string& mode) -> std::string {
        std::cout << "  处理模式: " << mode << std::endl;
        // 标准化显示模式
        if (mode == "fit") return "contain";
        if (mode == "fill") return "cover";
        if (mode == "stretch") return "fill";
        return mode;  // center, contain, cover等标准值
    });
    
    // 宽度参数处理器
    keyword->args.bind<int>("$", [](int width) -> std::string {
        std::cout << "  处理宽度: " << width << std::endl;
        // 验证和限制宽度范围
        int validWidth = std::max(50, std::min(4096, width));
        return std::to_string(validWidth);
    });
    
    // 高度参数处理器
    keyword->args.bind<int>("$", [](int height) -> std::string {
        std::cout << "  处理高度: " << height << std::endl;
        // 验证和限制高度范围
        int validHeight = std::max(50, std::min(4096, height));
        return std::to_string(validHeight);
    });
    
    // 缩放参数处理器
    keyword->args.bind<double>("$", [](double scale) -> std::string {
        std::cout << "  处理缩放: " << scale << std::endl;
        // 验证和限制缩放范围
        double validScale = std::max(0.1, std::min(5.0, scale));
        return std::to_string(validScale);
    });
    
    // 步骤3：JS代码转换
    std::cout << "\n=== 步骤3：JS代码转换 ===" << std::endl;
    
    keyword->args.transform("$", "ImageLoader.loadImage(\"${value}\")");
    keyword->args.transform("$", "DisplayMode.${value}");
    keyword->args.transform("$", "Math.max(${value}, window.innerWidth * 0.1)");
    keyword->args.transform("$", "Math.max(${value}, window.innerHeight * 0.1)");
    keyword->args.transform("$", "Math.min(${value}, window.devicePixelRatio || 1)");
    
    // 步骤4：与扫描器集成
    std::cout << "\n=== 步骤4：与扫描器集成 ===" << std::endl;
    
    auto& scanner = getCJMODScanner();
    
    // 模拟源代码token流
    std::vector<std::string> sourceTokens = {
        "const", "container", "=", "document", ".", "getElementById", "(", "\"main\"", ")", ";",
        "printMylove", "(", "{",
        "url", ":", "\"profile.jpg\"", ",",
        "mode", ":", "\"center\"", ",",
        "width", ":", "800", ",",
        "height", ":", "600", ",",
        "scale", ":", "1.5",
        "}", ")", ";",
        "container", ".", "appendChild", "(", "result", ")", ";"
    };
    
    scanner.setTokens(sourceTokens, 10);  // 位置在 "printMylove"
    
    // 使用peekKeyword提取参数值
    std::cout << "使用peekKeyword提取参数值:" << std::endl;
    std::string url = scanner.peekKeyword(5);      // "profile.jpg"
    std::string mode = scanner.peekKeyword(9);     // "center"
    std::string width = scanner.peekKeyword(13);   // "800"
    std::string height = scanner.peekKeyword(17);  // "600"
    std::string scale = scanner.peekKeyword(21);   // "1.5"
    
    // 步骤5：匹配值并处理
    std::cout << "\n=== 步骤5：匹配值并处理 ===" << std::endl;
    
    keyword->args.match("$", url);
    keyword->args.match("$", mode);
    keyword->args.match("$", width);
    keyword->args.match("$", height);
    keyword->args.match("$", scale);
    
    // 步骤6：生成最终JS代码
    std::cout << "\n=== 步骤6：生成最终JS代码 ===" << std::endl;
    
    std::string result = generateCode(*keyword);
    
    std::cout << "\n🎯 标准流程生成的printMylove JS代码:" << std::endl;
    std::cout << "printMylove({" << std::endl;
    std::cout << "  url: " << keyword->args[0].getJSCode() << "," << std::endl;
    std::cout << "  mode: " << keyword->args[1].getJSCode() << "," << std::endl;
    std::cout << "  width: " << keyword->args[2].getJSCode() << "," << std::endl;
    std::cout << "  height: " << keyword->args[3].getJSCode() << "," << std::endl;
    std::cout << "  scale: " << keyword->args[4].getJSCode() << std::endl;
    std::cout << "});" << std::endl;
    
    // 演示operator()重载的使用
    std::cout << "\n=== 演示operator()重载使用 ===" << std::endl;
    std::cout << "使用peekKeyword结果:" << std::endl;
    std::cout << "keyword.args[0](peekKeyword(5)): " << keyword->args[0](scanner.peekKeyword(5)) << std::endl;
    std::cout << "keyword.args[1](peekKeyword(9)): " << keyword->args[1](scanner.peekKeyword(9)) << std::endl;
}

void demonstrateSimplifiedFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "简化流程：printMylove CHTLJSFunction自动化" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 步骤1：创建CHTLJSFunction
    std::cout << "=== 步骤1：创建CHTLJSFunction ===" << std::endl;
    
    auto printMylove = createCHTLJSFunction("printMylove", {
        "url", "mode", "width", "height", "scale"
    });
    
    std::cout << "✅ 创建完成：" << printMylove->getFunctionName() << std::endl;
    
    // 步骤2：绑定键处理器
    std::cout << "\n=== 步骤2：绑定键处理器 ===" << std::endl;
    
    printMylove->bindKeyProcessor<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  绑定URL处理器: " << url << std::endl;
        // 智能URL处理
        if (url.find("http") == 0) return url;
        if (url.find("data:") == 0) return url;  // base64图片
        return url.find("/") == 0 ? url : "assets/images/" + url;
    });
    
    printMylove->bindKeyProcessor<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  绑定模式处理器: " << mode << std::endl;
        // 模式映射和验证
        std::unordered_map<std::string, std::string> modeMap = {
            {"fit", "contain"}, {"fill", "cover"}, {"stretch", "fill"},
            {"center", "center"}, {"contain", "contain"}, {"cover", "cover"}
        };
        auto it = modeMap.find(mode);
        return it != modeMap.end() ? it->second : "contain";
    });
    
    printMylove->bindKeyProcessor<int>("width", [](int width) -> std::string {
        std::cout << "  绑定宽度处理器: " << width << std::endl;
        // 响应式宽度处理
        if (width <= 0) return "window.innerWidth * 0.8";
        if (width < 100) return "100";
        if (width > 3840) return "3840";
        return std::to_string(width);
    });
    
    printMylove->bindKeyProcessor<int>("height", [](int height) -> std::string {
        std::cout << "  绑定高度处理器: " << height << std::endl;
        // 响应式高度处理
        if (height <= 0) return "window.innerHeight * 0.8";
        if (height < 100) return "100";
        if (height > 2160) return "2160";
        return std::to_string(height);
    });
    
    printMylove->bindKeyProcessor<double>("scale", [](double scale) -> std::string {
        std::cout << "  绑定缩放处理器: " << scale << std::endl;
        // 智能缩放处理
        if (scale <= 0) return "window.devicePixelRatio || 1";
        double validScale = std::max(0.1, std::min(4.0, scale));
        return std::to_string(validScale);
    });
    
    // 步骤3：设置JS转换模板
    std::cout << "\n=== 步骤3：设置JS转换模板 ===" << std::endl;
    
    printMylove->transformKey("url", "ImageManager.loadWithCache(\"${value}\")");
    printMylove->transformKey("mode", "ObjectFit.${value}");
    printMylove->transformKey("width", "ResponsiveSize.width(${value})");
    printMylove->transformKey("height", "ResponsiveSize.height(${value})");
    printMylove->transformKey("scale", "ScaleManager.optimize(${value})");
    
    // 步骤4：与扫描器集成（自动化）
    std::cout << "\n=== 步骤4：与扫描器集成（自动化） ===" << std::endl;
    
    auto& scanner = getCJMODScanner();
    printMylove->scanKeyword(scanner);
    
    // 步骤5：处理实际参数
    std::cout << "\n=== 步骤5：处理实际参数 ===" << std::endl;
    
    // 模拟从实际代码中提取的参数
    printMylove->matchKeyword("url", "my_avatar.png");
    printMylove->matchKeyword("mode", "fit");
    printMylove->matchKeyword("width", "1200");
    printMylove->matchKeyword("height", "800");
    printMylove->matchKeyword("scale", "2.0");
    
    // 步骤6：生成代码（一键完成）
    std::cout << "\n=== 步骤6：生成代码（一键完成） ===" << std::endl;
    
    std::string generatedCode = printMylove->generateCode();
    
    std::cout << "\n🎯 简化流程生成的printMylove JS代码:" << std::endl;
    std::cout << generatedCode << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "高级特性：printMylove 复杂场景处理" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 场景1：策略模式收集参数
    std::cout << "=== 场景1：策略模式收集参数 ===" << std::endl;
    
    auto& scanner = getCJMODScanner();
    
    // 开始收集策略
    scanner.policyChangeBegin("{", Policy::COLLECT);
    
    // 模拟收集参数对象
    scanner.processCollectPolicy(R"(
        url: "https://example.com/image.jpg",
        mode: "center",
        width: 1920,
        height: 1080,
        scale: window.devicePixelRatio,
        lazy: true,
        placeholder: "loading.gif"
    )");
    
    std::string collectedParams = scanner.policyChangeEnd("}", Policy::NORMAL);
    std::cout << "收集到的参数: " << collectedParams << std::endl;
    
    // 场景2：对象和函数判断
    std::cout << "\n=== 场景2：对象和函数判断 ===" << std::endl;
    
    std::string objectParam = "{ url: 'test.jpg', mode: 'center' }";
    std::string functionParam = "() => loadImage('dynamic.jpg')";
    
    if (scanner.isObject(objectParam)) {
        std::cout << "✅ 检测到对象参数，进行对象解析" << std::endl;
    }
    
    if (scanner.isFunction(functionParam)) {
        std::cout << "✅ 检测到函数参数，延迟执行" << std::endl;
    }
    
    // 场景3：复杂的peekKeyword应用
    std::cout << "\n=== 场景3：复杂的peekKeyword应用 ===" << std::endl;
    
    std::vector<std::string> complexTokens = {
        "if", "(", "condition", ")", "{",
        "printMylove", "(", "{",
        "url", ":", "conditional_image.jpg", ",",
        "mode", ":", "responsive", ",",
        "width", ":", "calc(100vw - 40px)", ",",
        "height", ":", "auto"
        "}", ")", ";",
        "}", "else", "{",
        "fallbackDisplay", "(", ")", ";",
        "}"
    };
    
    scanner.setTokens(complexTokens, 5);  // 位置在 "printMylove"
    
    auto complexPrintMylove = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height"});
    
    // 使用peekKeyword处理条件上下文
    std::string condition = scanner.peekKeyword(-4);  // "condition"
    std::string elseKeyword = scanner.peekKeyword(10); // "else"
    
    std::cout << "上下文分析 - 条件: " << condition << ", 否则分支: " << elseKeyword << std::endl;
    
    // 根据上下文调整处理逻辑
    complexPrintMylove->bindKeyProcessor<std::string>("url", [condition](const std::string& url) -> std::string {
        if (condition == "condition") {
            return "ConditionalLoader.load(\"" + url + "\")";
        }
        return "\"" + url + "\"";
    });
    
    complexPrintMylove->transformKey("url", "ImageResolver.resolve(${value})");
    complexPrintMylove->transformKey("mode", "ResponsiveMode.${value}");
    complexPrintMylove->transformKey("width", "CSSCalc.parse(\"${value}\")");
    complexPrintMylove->transformKey("height", "AutoHeight.calculate()");
    
    // 匹配复杂参数
    complexPrintMylove->matchKeyword("url", scanner.peekKeyword(4));   // "conditional_image.jpg"
    complexPrintMylove->matchKeyword("mode", scanner.peekKeyword(8));  // "responsive"
    complexPrintMylove->matchKeyword("width", scanner.peekKeyword(12)); // "calc(100vw - 40px)"
    complexPrintMylove->matchKeyword("height", scanner.peekKeyword(16)); // "auto"
    
    std::string complexResult = complexPrintMylove->generateCode();
    std::cout << "\n🎯 复杂场景生成的代码:\n" << complexResult << std::endl;
}

void demonstrateRealWorldExample() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "真实世界示例：完整的printMylove应用" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::cout << "=== CHTL源代码示例 ===" << std::endl;
    std::string chtlSource = R"(
<div class="image-gallery">
    <h2>我的照片</h2>
    
    printMylove({
        url: "portrait.jpg",
        mode: "cover",
        width: 400,
        height: 300,
        scale: 2.0
    });
    
    <p>这是我最喜欢的照片</p>
</div>
)";
    
    std::cout << chtlSource << std::endl;
    
    std::cout << "\n=== 处理过程 ===" << std::endl;
    
    // 使用简化流程处理
    auto realWorldPrintMylove = createCHTLJSFunction("printMylove", {
        "url", "mode", "width", "height", "scale"
    });
    
    // 实际项目中的处理器
    realWorldPrintMylove->bindKeyProcessor<std::string>("url", [](const std::string& url) -> std::string {
        // 生产环境的URL处理
        if (url.find("http") == 0) return url;
        return "/static/images/" + url;
    });
    
    realWorldPrintMylove->bindKeyProcessor<std::string>("mode", [](const std::string& mode) -> std::string {
        // CSS object-fit值映射
        if (mode == "cover") return "cover";
        if (mode == "contain") return "contain";
        if (mode == "fill") return "fill";
        return "cover";  // 默认值
    });
    
    realWorldPrintMylove->bindKeyProcessor<int>("width", [](int width) -> std::string {
        // 响应式设计考虑
        return "Math.min(" + std::to_string(width) + ", window.innerWidth * 0.9)";
    });
    
    realWorldPrintMylove->bindKeyProcessor<int>("height", [](int height) -> std::string {
        // 保持宽高比
        return "Math.min(" + std::to_string(height) + ", window.innerHeight * 0.7)";
    });
    
    realWorldPrintMylove->bindKeyProcessor<double>("scale", [](double scale) -> std::string {
        // 高DPI显示支持
        return "Math.min(" + std::to_string(scale) + ", window.devicePixelRatio || 1)";
    });
    
    // 生产级JS模板
    realWorldPrintMylove->transformKey("url", "ImageUtils.createOptimizedSrc(\"${value}\")");
    realWorldPrintMylove->transformKey("mode", "ObjectFitPolyfill.apply(\"${value}\")");
    realWorldPrintMylove->transformKey("width", "ResponsiveUtils.calculateWidth(${value})");
    realWorldPrintMylove->transformKey("height", "ResponsiveUtils.calculateHeight(${value})");
    realWorldPrintMylove->transformKey("scale", "DPIUtils.adjustScale(${value})");
    
    // 处理参数
    realWorldPrintMylove->matchKeyword("url", "portrait.jpg");
    realWorldPrintMylove->matchKeyword("mode", "cover");
    realWorldPrintMylove->matchKeyword("width", "400");
    realWorldPrintMylove->matchKeyword("height", "300");
    realWorldPrintMylove->matchKeyword("scale", "2.0");
    
    std::string productionCode = realWorldPrintMylove->generateCode();
    
    std::cout << "\n🎯 生产环境的printMylove代码:" << std::endl;
    std::cout << productionCode << std::endl;
    
    std::cout << "\n=== 最终生成的HTML + JS ===" << std::endl;
    std::cout << R"(
<div class="image-gallery">
    <h2>我的照片</h2>
    <script>
        )" << productionCode << R"(
    </script>
    <p>这是我最喜欢的照片</p>
</div>
)" << std::endl;
}

int main() {
    std::cout << "printMylove 完整演示程序" << std::endl;
    std::cout << "展示标准流程和简化流程的设计和使用" << std::endl;
    
    // 演示所有功能
    demonstrateStandardFlow();
    demonstrateSimplifiedFlow();
    demonstrateAdvancedFeatures();
    demonstrateRealWorldExample();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "printMylove 演示完成" << std::endl;
    std::cout << "\n总结对比:" << std::endl;
    std::cout << "标准流程：" << std::endl;
    std::cout << "  ✅ 手动控制每个步骤" << std::endl;
    std::cout << "  ✅ 灵活的参数处理" << std::endl;
    std::cout << "  ✅ 完整的扫描器集成" << std::endl;
    std::cout << "  ✅ peekKeyword相对位置访问" << std::endl;
    std::cout << "\n简化流程：" << std::endl;
    std::cout << "  ✅ CHTLJSFunction自动化" << std::endl;
    std::cout << "  ✅ 键处理器绑定" << std::endl;
    std::cout << "  ✅ JS模板转换" << std::endl;
    std::cout << "  ✅ 一键代码生成" << std::endl;
    std::cout << "\n两种流程各有优势，可根据具体需求选择！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}