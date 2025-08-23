#include "CJMOD/AutoFillCJMODApi.h"
#include <iostream>
#include <vector>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateBasicAutoFill() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "基础自动填充：从源代码自动提取参数" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 模拟源代码token流
    std::vector<std::string> sourceTokens = {
        "const", "config", "=", "{",
        "imageUrl:", "avatar.jpg", ",",
        "displayMode:", "center", ",",
        "width:", "800", ",",
        "height:", "600",
        "}", ";",
        "printMylove", "(", "config", ")", ";"
    };
    
    std::cout << "源代码token流:" << std::endl;
    for (size_t i = 0; i < sourceTokens.size(); ++i) {
        std::cout << "[" << i << "] " << sourceTokens[i] << " ";
        if ((i + 1) % 8 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 创建语法并设置期望类型
    auto syntax = syntaxAnalys("printMylove({ url: $, mode: $, width: $, height: $ });");
    
    std::string result = syntax
        ->expectTypes({ParamType::URL, ParamType::STRING, ParamType::SIZE, ParamType::SIZE})
        .autoFill(sourceTokens, FillStrategy::SEQUENTIAL)
        .acceptAutoFill(0.6)
        .debug("基础自动填充完成")
        .result();
    
    std::cout << "\n🎯 基础自动填充结果:\n" << result << std::endl;
}

void demonstrateTypeAwareAutoFill() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "类型感知自动填充：智能类型匹配" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "loadImage", "(", "{",
        "src:", "https://example.com/photo.jpg", ",",
        "width:", "1200px", ",",
        "height:", "800px", ",",
        "quality:", "0.85", ",",
        "lazy:", "true", ",",
        "alt:", "Beautiful photo"
        "}", ")", ";"
    };
    
    std::cout << "类型感知源代码:" << std::endl;
    for (const auto& token : sourceTokens) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    auto syntax = syntaxAnalys("processImage({ url: $, width: $, height: $, quality: $, lazy: $ });");
    
    std::string result = syntax
        ->expectTypes({
            ParamType::URL,      // url参数期望URL类型
            ParamType::SIZE,     // width参数期望尺寸类型
            ParamType::SIZE,     // height参数期望尺寸类型
            ParamType::NUMBER,   // quality参数期望数字类型
            ParamType::BOOLEAN   // lazy参数期望布尔类型
        })
        .autoFill(sourceTokens, FillStrategy::TYPE_AWARE)
        .acceptAutoFill(0.7)
        .debug("类型感知自动填充完成")
        .result();
    
    std::cout << "\n🎯 类型感知自动填充结果:\n" << result << std::endl;
}

void demonstrateDualPointerScanning() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "双指针扫描机制：精确代码片段提取" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "function", "init", "(", ")", "{",
        "const", "url", "=", "profile.jpg", ";",
        "const", "mode", "=", "fit", ";",
        "let", "width", "=", "800", ";",
        "printMylove", "(", "{",    // 关键字出现位置
        "url", ":", "url", ",",
        "mode", ":", "mode", ",",
        "width", ":", "width",
        "}", ")", ";",
        "}"
    };
    
    std::cout << "双指针扫描源代码:" << std::endl;
    for (size_t i = 0; i < sourceTokens.size(); ++i) {
        std::cout << "[" << std::setw(2) << i << "] " << sourceTokens[i] << " ";
        if ((i + 1) % 6 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 创建自动填充扫描器，启用双指针扫描
    auto scanner = createAutoFillScanner();
    scanner->setTokens(sourceTokens);
    scanner->setKeywords({"printMylove"});
    scanner->enableDualPointerScanning(true);
    scanner->setFillStrategy(FillStrategy::CONTEXT_AWARE);
    
    auto syntax = syntaxAnalys("printMylove({ url: $, mode: $, width: $ });");
    
    std::string result = syntax
        ->expectTypes({ParamType::URL, ParamType::STRING, ParamType::SIZE})
        .autoFillWithScanner(*scanner, "printMylove")
        .acceptAutoFill(0.7)
        .debug("双指针扫描自动填充完成")
        .result();
    
    std::cout << "\n🎯 双指针扫描自动填充结果:\n" << result << std::endl;
}

void demonstrateFrontExtraction() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "前置截取机制：arg ** arg 模式处理" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "let", "result", "=", "0", ";",
        "for", "(", "let", "i", "=", "0", ";", "i", "<", "10", ";", "i", "++", ")", "{",
        "let", "value", "=", "input", "**", "2", ";",  // arg ** arg 模式
        "result", "+=", "value", ";",
        "power", "(", "{",
        "base", ":", "input", ",",
        "exponent", ":", "2",
        "}", ")", ";",
        "}"
    };
    
    std::cout << "前置截取源代码 (包含 arg ** arg 模式):" << std::endl;
    for (size_t i = 0; i < sourceTokens.size(); ++i) {
        std::cout << "[" << std::setw(2) << i << "] " << sourceTokens[i] << " ";
        if ((i + 1) % 8 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 创建自动填充扫描器，启用前置截取
    auto scanner = createAutoFillScanner();
    scanner->setTokens(sourceTokens);
    scanner->setKeywords({"**", "power"});
    scanner->enableDualPointerScanning(true);
    scanner->enableFrontExtraction(true);
    scanner->setFillStrategy(FillStrategy::AUTO);
    
    auto syntax = syntaxAnalys("power({ base: $, exponent: $ });");
    
    std::string result = syntax
        ->expectTypes({ParamType::STRING, ParamType::NUMBER})
        .autoFillWithScanner(*scanner, "power")
        .acceptAutoFill(0.6)
        .debug("前置截取自动填充完成")
        .result();
    
    std::cout << "\n🎯 前置截取自动填充结果:\n" << result << std::endl;
}

void demonstratePatternMatching() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "模式匹配自动填充：正则表达式精确匹配" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "const", "styles", "=", "{",
        "backgroundColor:", "#FF5733", ",",
        "fontSize:", "16px", ",",
        "margin:", "10px", ",",
        "borderRadius:", "5px", ",",
        "color:", "white",
        "}", ";",
        "setStyle", "(", "styles", ")", ";"
    };
    
    std::cout << "模式匹配源代码:" << std::endl;
    for (const auto& token : sourceTokens) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    auto syntax = syntaxAnalys("setStyle({ color: $, fontSize: $, margin: $ });");
    
    std::string result = syntax
        ->expectTypes({ParamType::COLOR, ParamType::SIZE, ParamType::SIZE})
        .setPatterns({
            R"(^#[0-9A-Fa-f]{6}$)",      // 十六进制颜色
            R"(^\d+px$)",                // px尺寸
            R"(^\d+px$)"                 // px尺寸
        })
        .autoFill(sourceTokens, FillStrategy::PATTERN_MATCH)
        .acceptAutoFill(0.8)
        .debug("模式匹配自动填充完成")
        .result();
    
    std::cout << "\n🎯 模式匹配自动填充结果:\n" << result << std::endl;
}

void demonstrateContextAwareAutoFill() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "上下文感知自动填充：函数名智能推断" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "const", "media", "=", "{",
        "videoSrc:", "movie.mp4", ",",
        "posterUrl:", "poster.jpg", ",",
        "volume:", "0.8", ",",
        "autoplay:", "false", ",",
        "controls:", "true",
        "}", ";",
        "playVideo", "(", "media", ")", ";"
    };
    
    std::cout << "上下文感知源代码:" << std::endl;
    for (const auto& token : sourceTokens) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    // 创建智能CHTLJSFunction，利用函数名推断参数类型
    auto func = createSmartCHTLJSFunction("playVideo", 
        {"src", "poster", "volume", "autoplay"}, 
        {ParamType::URL, ParamType::URL, ParamType::NUMBER, ParamType::BOOLEAN},
        FillStrategy::CONTEXT_AWARE);
    
    std::string result = func
        ->performAutoFill(sourceTokens)
        ->generateCode();
    
    std::cout << "\n🎯 上下文感知自动填充结果:\n" << result << std::endl;
}

void demonstrateAdvancedAutoFill() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "高级自动填充：多策略组合应用" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "import", "React", "from", "react", ";",
        "const", "config", "=", "{",
        "apiUrl:", "https://api.example.com/data", ",",
        "timeout:", "5000", ",",
        "retries:", "3", ",",
        "useCache:", "true", ",",
        "format:", "json"
        "}", ";",
        "arg", "**", "2", ";",  // 前置截取模式
        "fetchData", "(", "config", ")", ";"
    };
    
    std::cout << "高级自动填充源代码 (包含复杂模式):" << std::endl;
    for (size_t i = 0; i < sourceTokens.size(); ++i) {
        std::cout << "[" << std::setw(2) << i << "] " << sourceTokens[i] << " ";
        if ((i + 1) % 6 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 创建高级自动填充扫描器
    auto scanner = createAutoFillScanner();
    scanner->setTokens(sourceTokens);
    scanner->setKeywords({"fetchData", "**"});
    scanner->enableDualPointerScanning(true);
    scanner->enableFrontExtraction(true);
    scanner->setFillStrategy(FillStrategy::AUTO);
    scanner->setMinConfidence(0.6);
    
    auto syntax = syntaxAnalys("fetchData({ url: $, timeout: $, retries: $, cache: $, format: $ });");
    
    std::string result = syntax
        ->expectTypes({
            ParamType::URL,      // url
            ParamType::NUMBER,   // timeout
            ParamType::NUMBER,   // retries
            ParamType::BOOLEAN,  // cache
            ParamType::STRING    // format
        })
        .setPatterns({
            R"(https?://[^\s]+)",  // URL模式
            R"(\d+)",              // 数字模式
            R"(\d+)",              // 数字模式
            R"(true|false)",       // 布尔模式
            R"(\w+)"               // 字符串模式
        })
        .autoFillWithScanner(*scanner, "fetchData")
        .acceptAutoFill(0.6)
        .debug("高级自动填充完成")
        .result();
    
    std::cout << "\n🎯 高级自动填充结果:\n" << result << std::endl;
}

void demonstrateChainedAutoFill() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "链式自动填充：流畅的一体化操作" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "const", "imageConfig", "=", "{",
        "source:", "beautiful-sunset.jpg", ",",
        "width:", "1920", ",",
        "height:", "1080", ",",
        "quality:", "95", ",",
        "format:", "webp", ",",
        "lazy:", "true"
        "}", ";",
        "printMylove", "(", "imageConfig", ")", ";"
    };
    
    std::cout << "链式自动填充源代码:" << std::endl;
    for (const auto& token : sourceTokens) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    // 一条链式调用完成复杂的自动填充配置
    auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height", "scale"});
    
    std::string result = func
        ->enableAutoFill(
            {ParamType::URL, ParamType::STRING, ParamType::SIZE, ParamType::SIZE, ParamType::NUMBER},
            {R"([^/]+\.(jpg|png|webp))", R"(\w+)", R"(\d+)", R"(\d+)", R"(\d+(?:\.\d+)?)"},
            FillStrategy::AUTO
        )
        ->performAutoFill(sourceTokens)
        ->generateCode();
    
    std::cout << "\n🎯 链式自动填充结果:\n" << result << std::endl;
}

void demonstrateRealWorldExample() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "真实应用场景：复杂的前端组件配置" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<std::string> sourceTokens = {
        "// React组件配置",
        "const", "galleryConfig", "=", "{",
        "images:", "[",
        "{", "url:", "photo1.jpg", ",", "alt:", "First photo", "}",
        "{", "url:", "photo2.png", ",", "alt:", "Second photo", "}",
        "]", ",",
        "layout:", "grid", ",",
        "columns:", "3", ",",
        "spacing:", "20px", ",",
        "lightbox:", "true", ",",
        "lazy:", "true", ",",
        "animation:", "fadeIn"
        "}", ";",
        
        "// 处理函数调用",
        "value", "**", "power", ";",  // 前置截取测试
        "renderGallery", "(", "galleryConfig", ")", ";"
    };
    
    std::cout << "真实应用场景源代码:" << std::endl;
    for (size_t i = 0; i < sourceTokens.size(); ++i) {
        if (sourceTokens[i].find("//") == 0) {
            std::cout << "\n" << sourceTokens[i] << std::endl;
        } else {
            std::cout << sourceTokens[i] << " ";
        }
    }
    std::cout << std::endl;
    
    // 创建高级智能扫描器
    auto scanner = createAutoFillScanner();
    scanner->setTokens(sourceTokens);
    scanner->setKeywords({"renderGallery", "**"});
    scanner->enableDualPointerScanning(true);
    scanner->enableFrontExtraction(true);
    scanner->setFillStrategy(FillStrategy::AUTO);
    scanner->setMinConfidence(0.5);  // 降低阈值以处理复杂场景
    
    auto syntax = syntaxAnalys("renderGallery({ layout: $, columns: $, spacing: $, lightbox: $, animation: $ });");
    
    std::string result = syntax
        ->expectTypes({
            ParamType::STRING,   // layout
            ParamType::NUMBER,   // columns  
            ParamType::SIZE,     // spacing
            ParamType::BOOLEAN,  // lightbox
            ParamType::STRING    // animation
        })
        .setPatterns({
            R"(grid|list|masonry)",     // 布局模式
            R"(\d+)",                   // 列数
            R"(\d+px)",                 // 间距
            R"(true|false)",            // 灯箱
            R"(fadeIn|slideIn|none)"    // 动画
        })
        .autoFillWithScanner(*scanner, "renderGallery")
        .acceptAutoFill(0.5)
        .debug("真实应用场景自动填充完成")
        .result();
    
    std::cout << "\n🎯 真实应用场景自动填充结果:\n" << result << std::endl;
}

int main() {
    std::cout << "CJMOD 智能自动参数填充演示程序" << std::endl;
    std::cout << "深度集成双指针扫描和前置截取机制" << std::endl;
    
    // 演示各种自动填充功能
    demonstrateBasicAutoFill();
    demonstrateTypeAwareAutoFill();
    demonstrateDualPointerScanning();
    demonstrateFrontExtraction();
    demonstratePatternMatching();
    demonstrateContextAwareAutoFill();
    demonstrateAdvancedAutoFill();
    demonstrateChainedAutoFill();
    demonstrateRealWorldExample();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "智能自动参数填充演示完成" << std::endl;
    std::cout << "\n🚀 核心特性总结:" << std::endl;
    std::cout << "✅ 基础自动填充：顺序参数匹配" << std::endl;
    std::cout << "✅ 类型感知填充：智能类型检测和匹配" << std::endl;
    std::cout << "✅ 双指针扫描：精确代码片段提取" << std::endl;
    std::cout << "  • 前后指针从0开始，初始范围扫描" << std::endl;
    std::cout << "  • 同步移动，关键字检测和收集" << std::endl;
    std::cout << "✅ 前置截取机制：arg ** arg 模式处理" << std::endl;
    std::cout << "  • 统一扫描器片段前置参数提取" << std::endl;
    std::cout << "  • 避免发送给编译器的冲突" << std::endl;
    std::cout << "✅ 模式匹配填充：正则表达式精确匹配" << std::endl;
    std::cout << "✅ 上下文感知填充：函数名智能推断" << std::endl;
    std::cout << "✅ 链式自动填充：流畅的一体化操作" << std::endl;
    std::cout << "✅ 置信度系统：智能筛选最佳候选项" << std::endl;
    std::cout << "✅ 多策略组合：自动选择最优策略" << std::endl;
    std::cout << "\n🎯 自动参数填充让CJMOD API真正实现智能化！" << std::endl;
    std::cout << "无需手动匹配参数，扫描机制自动完成一切！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}