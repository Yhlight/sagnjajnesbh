#include "CJMOD/EnhancedCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateBasicChaining() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "基础链式调用：bind -> transform -> match 一气呵成" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 创建语法并进行链式操作
    auto syntax = syntaxAnalys("printMylove({ url: $, mode: $, width: $ });");
    
    // 一条链式调用完成所有配置
    std::string result = syntax
        ->bind<std::string>([](const std::string& url) -> std::string {
            std::cout << "链式处理URL: " << url << std::endl;
            return url.find("http") == 0 ? url : "assets/" + url;
        })
        .transform("ImageLoader.load(\"${value}\")")
        .match("avatar.jpg")
        
        .bind<std::string>([](const std::string& mode) -> std::string {
            std::cout << "链式处理模式: " << mode << std::endl;
            return mode == "fit" ? "contain" : mode;
        })
        .transform("DisplayMode.${value}")
        .match("fit")
        
        .bind<int>([](int width) -> std::string {
            std::cout << "链式处理宽度: " << width << std::endl;
            return std::to_string(width > 0 ? width : 800);
        })
        .transform("Math.max(${value}, 100)")
        .match("1200")
        
        .debug("基础链式调用完成")
        .result();
    
    std::cout << "\n🎯 链式调用结果:\n" << result << std::endl;
}

void demonstrateAdvancedChaining() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "高级链式调用：bindAndTransform 一步到位" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto syntax = syntaxAnalys("processImage({ input: $, filters: $, output: $, quality: $ });");
    
    // 使用bindAndTransform一步完成绑定和转换
    std::string result = syntax
        ->bindAndTransform<std::string>(
            [](const std::string& input) -> std::string {
                std::cout << "一步处理输入: " << input << std::endl;
                return input.find("/") == 0 ? input : "images/" + input;
            },
            "ImageLoader.loadFromPath(\"${value}\")"
        )
        .match("raw_photo.jpg")
        
        ->bindAndTransform<std::string>(
            [](const std::string& filters) -> std::string {
                std::cout << "一步处理滤镜: " << filters << std::endl;
                return filters == "auto" ? "blur,sharpen" : filters;
            },
            "FilterChain.parse(\"${value}\")"
        )
        .match("auto")
        
        ->bindAndTransform<std::string>(
            [](const std::string& output) -> std::string {
                std::cout << "一步处理输出: " << output << std::endl;
                return output.find("/") == 0 ? output : "processed/" + output;
            },
            "OutputManager.setPath(\"${value}\")"
        )
        .match("final.jpg")
        
        ->bindAndTransform<int>(
            [](int quality) -> std::string {
                std::cout << "一步处理质量: " << quality << std::endl;
                return std::to_string(std::max(1, std::min(100, quality)));
            },
            "QualitySettings.set(${value})"
        )
        .match("85")
        
        .debug("高级链式调用完成")
        .result();
    
    std::cout << "\n🎯 高级链式调用结果:\n" << result << std::endl;
}

void demonstrateConditionChaining() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "条件链式调用：when条件控制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto syntax = syntaxAnalys("renderImage({ src: $, size: $, lazy: $ });");
    
    bool enableOptimization = true;
    bool enableLazyLoading = false;
    
    std::string result = syntax
        ->bind<std::string>([](const std::string& src) -> std::string {
            return src.find("http") == 0 ? src : "cdn/" + src;
        })
        .when(enableOptimization, [](Syntax& s) -> Syntax& {
            std::cout << "应用优化策略" << std::endl;
            return s.transform("OptimizedImageLoader.load(\"${value}\")");
        })
        .when(!enableOptimization, [](Syntax& s) -> Syntax& {
            std::cout << "使用标准加载" << std::endl;
            return s.transform("StandardImageLoader.load(\"${value}\")");
        })
        .match("photo.jpg")
        
        ->bind<int>([](int size) -> std::string {
            return std::to_string(size > 0 ? size : 300);
        })
        .transform("ResponsiveSize.calculate(${value})")
        .match("500")
        
        ->when(enableLazyLoading, [](Syntax& s) -> Syntax& {
            std::cout << "启用懒加载" << std::endl;
            return s.bind<std::string>([](const std::string& lazy) -> std::string {
                return "true";
            }).transform("LazyLoader.enable(${value})").match("yes");
        })
        .when(!enableLazyLoading, [](Syntax& s) -> Syntax& {
            std::cout << "禁用懒加载" << std::endl;
            return s.bind<std::string>([](const std::string& lazy) -> std::string {
                return "false";
            }).transform("LazyLoader.disable()").match("no");
        })
        
        .debug("条件链式调用完成")
        .result();
    
    std::cout << "\n🎯 条件链式调用结果:\n" << result << std::endl;
}

void demonstrateScannerIntegration() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "扫描器集成：withScanner自动化peekKeyword" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& scanner = getCJMODScanner();
    
    // 设置源代码token流
    std::vector<std::string> tokens = {
        "const", "config", "=", "{", "theme:", "dark", "}", ";",
        "printMylove", "(", "{",
        "url", ":", "profile.jpg", ",",
        "mode", ":", "center", ",",
        "size", ":", "large",
        "}", ")", ";",
        "display", "(", "config", ")", ";"
    };
    
    scanner.setTokens(tokens, 8);  // 位置在 "printMylove"
    
    auto syntax = syntaxAnalys("printMylove({ url: $, mode: $, size: $ });");
    
    // 链式绑定处理器
    std::string result = syntax
        ->bind<std::string>([](const std::string& url) -> std::string {
            std::cout << "扫描器集成处理URL: " << url << std::endl;
            return "\"" + url + "\"";
        })
        .bind<std::string>([](const std::string& mode) -> std::string {
            std::cout << "扫描器集成处理模式: " << mode << std::endl;
            return "\"" + mode + "\"";
        })
        .bind<std::string>([](const std::string& size) -> std::string {
            std::cout << "扫描器集成处理大小: " << size << std::endl;
            return "\"" + size + "\"";
        })
        
        // 链式设置JS转换
        .transform("ImageCache.get(${value})")
        .transform("DisplayMode.${value}")
        .transform("SizePreset.${value}")
        
        // 与扫描器集成，自动使用peekKeyword
        .withScanner(scanner, {5, 9, 13})  // 自动peek这些偏移位置
        
        .debug("扫描器集成完成")
        .result();
    
    std::cout << "\n🎯 扫描器集成结果:\n" << result << std::endl;
}

void demonstrateBatchOperations() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "批量操作：transformSequence & matchSequence" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto syntax = syntaxAnalys("animateElement({ target: $, duration: $, easing: $, delay: $ });");
    
    // 批量绑定处理器
    syntax->args
        .bind<std::string>("$", [](const std::string& target) -> std::string {
            std::cout << "批量处理目标: " << target << std::endl;
            return target.find('.') == 0 ? target : "." + target;
        })
        .bind<int>("$", [](int duration) -> std::string {
            std::cout << "批量处理持续时间: " << duration << std::endl;
            return std::to_string(duration > 0 ? duration : 300);
        })
        .bind<std::string>("$", [](const std::string& easing) -> std::string {
            std::cout << "批量处理缓动: " << easing << std::endl;
            return easing.empty() ? "ease" : easing;
        })
        .bind<int>("$", [](int delay) -> std::string {
            std::cout << "批量处理延迟: " << delay << std::endl;
            return std::to_string(delay >= 0 ? delay : 0);
        });
    
    // 批量设置JS转换模板
    syntax->args.transformSequence({
        "document.querySelector('${value}')",
        "AnimationUtils.duration(${value})",
        "EasingFunctions.${value}",
        "DelayManager.set(${value})"
    });
    
    // 批量匹配值
    syntax->args.matchSequence({
        "header",
        "500",
        "ease-in-out",
        "100"
    });
    
    std::string result = syntax->debug("批量操作完成").result();
    
    std::cout << "\n🎯 批量操作结果:\n" << result << std::endl;
}

void demonstrateCHTLJSFunctionChaining() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "CHTLJSFunction链式调用：bindAndTransform高效配置" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 创建并进行链式配置
    auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height", "scale"});
    
    std::string result = func
        ->bindAndTransform<std::string>("url",
            [](const std::string& url) -> std::string {
                std::cout << "链式配置URL: " << url << std::endl;
                return url.find("http") == 0 ? url : "https://cdn.example.com/" + url;
            },
            "ImageService.loadOptimized(\"${value}\")"
        )
        
        ->bindAndTransform<std::string>("mode",
            [](const std::string& mode) -> std::string {
                std::cout << "链式配置模式: " << mode << std::endl;
                return mode == "auto" ? "responsive" : mode;
            },
            "DisplayEngine.setMode('${value}')"
        )
        
        ->bindAndTransform<int>("width",
            [](int width) -> std::string {
                std::cout << "链式配置宽度: " << width << std::endl;
                return std::to_string(width > 0 ? width : 400);
            },
            "ViewportCalculator.width(${value})"
        )
        
        ->bindAndTransform<int>("height",
            [](int height) -> std::string {
                std::cout << "链式配置高度: " << height << std::endl;
                return std::to_string(height > 0 ? height : 300);
            },
            "ViewportCalculator.height(${value})"
        )
        
        ->bindAndTransform<double>("scale",
            [](double scale) -> std::string {
                std::cout << "链式配置缩放: " << scale << std::endl;
                return std::to_string(scale > 0 ? scale : 1.0);
            },
            "ScaleEngine.optimize(${value})"
        )
        
        // 链式匹配参数
        ->matchKeyword("url", "avatar.png")
        ->matchKeyword("mode", "auto")
        ->matchKeyword("width", "800")
        ->matchKeyword("height", "600")
        ->matchKeyword("scale", "2.0")
        
        // 生成最终代码
        ->generateCode();
    
    std::cout << "\n🎯 CHTLJSFunction链式调用结果:\n" << result << std::endl;
}

void demonstrateUltraFluentAPI() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "终极流畅API：configure一步完成所有操作" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto syntax = syntaxAnalys("mediaPlayer({ src: $, autoplay: $, volume: $ });");
    
    // 使用configure一步完成bind、transform、match
    std::string result = syntax
        ->configure<std::string>(
            [](const std::string& src) -> std::string {
                std::cout << "终极配置源文件: " << src << std::endl;
                return src.find("http") == 0 ? src : "media/" + src;
            },
            "MediaLoader.loadSource(\"${value}\")",
            "video.mp4"
        )
        
        ->configure<std::string>(
            [](const std::string& autoplay) -> std::string {
                std::cout << "终极配置自动播放: " << autoplay << std::endl;
                return autoplay == "yes" ? "true" : "false";
            },
            "AutoplayManager.set(${value})",
            "yes"
        )
        
        ->configure<double>(
            [](double volume) -> std::string {
                std::cout << "终极配置音量: " << volume << std::endl;
                return std::to_string(std::max(0.0, std::min(1.0, volume)));
            },
            "VolumeControl.setLevel(${value})",
            "0.8"
        )
        
        .debug("终极流畅API完成")
        .result();
    
    std::cout << "\n🎯 终极流畅API结果:\n" << result << std::endl;
}

int main() {
    std::cout << "增强的CJMOD API链式调用演示程序" << std::endl;
    std::cout << "展示高效的链式bind和转换操作" << std::endl;
    
    // 演示各种链式调用功能
    demonstrateBasicChaining();
    demonstrateAdvancedChaining();
    demonstrateConditionChaining();
    demonstrateScannerIntegration();
    demonstrateBatchOperations();
    demonstrateCHTLJSFunctionChaining();
    demonstrateUltraFluentAPI();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "增强链式调用演示完成" << std::endl;
    std::cout << "\n核心增强特性总结:" << std::endl;
    std::cout << "✅ 基础链式：bind() -> transform() -> match()" << std::endl;
    std::cout << "✅ 高效链式：bindAndTransform() 一步到位" << std::endl;
    std::cout << "✅ 条件链式：when() 条件控制流程" << std::endl;
    std::cout << "✅ 扫描器集成：withScanner() 自动peekKeyword" << std::endl;
    std::cout << "✅ 批量操作：transformSequence() & matchSequence()" << std::endl;
    std::cout << "✅ 终极流畅：configure() 一步完成所有操作" << std::endl;
    std::cout << "✅ 调试支持：debug() 链式调试输出" << std::endl;
    std::cout << "\n链式调用让CJMOD API更加流畅高效！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}