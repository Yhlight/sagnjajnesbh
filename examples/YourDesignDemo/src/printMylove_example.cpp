// ========================================
// 您的设计完整演示 - printMylove示例
// 完美实现您的syntaxAnalys + $ + args.bind设计
// ========================================

#include "YourInnovativeDesign.h"
#include <iostream>
#include <string>

using namespace chtl::your_design;

/**
 * printMylove语法实现 - 完全按照您的设计
 */
void setupPrintMyloveFunction() {
    std::cout << "🖼️ 设置printMylove语法" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // 🎯 第一步：使用您的syntaxAnalys设计
    std::string ignoreSymbols = ",:{};()";  // 您指定的忽略符号
    
    const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,  // 模式可以选择ASCII或Pixel
            width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
            height: $,  // 高度
            scale:  $,  // 缩放倍数，限定为等比缩放策略
        });
    )", ignoreSymbols);
    
    std::cout << "✅ 语法模式解析完成" << std::endl;
    
    // 🔗 第二步：使用您的args.bind设计
    keyword.argsManager.bind("url", [](const std::string& str) -> std::string {
        std::cout << "🌐 处理URL参数: " << str << std::endl;
        
        // URL验证和处理
        if (str.find("http") == 0 || str.find("data:") == 0) {
            return "'" + str + "'";
        } else {
            // 相对路径处理
            return "'./images/" + str + "'";
        }
    });
    
    keyword.argsManager.bind("mode", [](const std::string& str) -> std::string {
        std::cout << "🎨 处理模式参数: " << str << std::endl;
        
        if (str == "ASCII" || str == "Pixel") {
            return "'" + str + "'";
        } else {
            std::cout << "⚠️ 无效模式，使用默认ASCII" << std::endl;
            return "'ASCII'";
        }
    });
    
    keyword.argsManager.bind("width", [](const std::string& str) -> std::string {
        std::cout << "📏 处理宽度参数: " << str << std::endl;
        
        // 支持CSS单位、百分比、小数、纯数字
        if (str.find("%") != std::string::npos) {
            // 百分比
            return "Math.floor(window.innerWidth * " + str.substr(0, str.find("%")) + " / 100)";
        } else if (str.find("px") != std::string::npos) {
            // 像素
            return str.substr(0, str.find("px"));
        } else if (str.find("em") != std::string::npos) {
            // em单位
            return "Math.floor(16 * " + str.substr(0, str.find("em")) + ")";
        } else if (str.find("rem") != std::string::npos) {
            // rem单位
            return "Math.floor(16 * " + str.substr(0, str.find("rem")) + ")";
        } else {
            // 纯数字（像素）
            try {
                double num = std::stod(str);
                return std::to_string(static_cast<int>(num));
            } catch (...) {
                return "100"; // 默认宽度
            }
        }
    });
    
    keyword.argsManager.bind("height", [](const std::string& str) -> std::string {
        std::cout << "📐 处理高度参数: " << str << std::endl;
        
        // 与width相同的处理逻辑
        if (str.find("%") != std::string::npos) {
            return "Math.floor(window.innerHeight * " + str.substr(0, str.find("%")) + " / 100)";
        } else if (str.find("px") != std::string::npos) {
            return str.substr(0, str.find("px"));
        } else if (str.find("em") != std::string::npos) {
            return "Math.floor(16 * " + str.substr(0, str.find("em")) + ")";
        } else if (str.find("rem") != std::string::npos) {
            return "Math.floor(16 * " + str.substr(0, str.find("rem")) + ")";
        } else {
            try {
                double num = std::stod(str);
                return std::to_string(static_cast<int>(num));
            } catch (...) {
                return "100"; // 默认高度
            }
        }
    });
    
    keyword.argsManager.bind("scale", [](const std::string& str) -> std::string {
        std::cout << "🔍 处理缩放参数: " << str << std::endl;
        
        try {
            double scale = std::stod(str);
            
            // 限制缩放范围
            if (scale < 0.1) scale = 0.1;
            if (scale > 10.0) scale = 10.0;
            
            return std::to_string(scale);
        } catch (...) {
            std::cout << "⚠️ 无效缩放值，使用默认1.0" << std::endl;
            return "1.0";
        }
    });
    
    // 🔍 第三步：设置扫描器接口
    InnovativeScanner scanner;
    
    scanner.scanKeyword(keyword.arg[0], [&]() {
        std::cout << "🔍 扫描到printMylove函数！" << std::endl;
        
        // 🎯 使用您的peekKeyword设计
        KeywordValue urlKeyword = scanner.peekKeyword(1);
        KeywordValue modeKeyword = scanner.peekKeyword(2);
        
        std::cout << "📋 扫描结果:" << std::endl;
        std::cout << "   URL: " << urlKeyword.value << std::endl;
        std::cout << "   模式: " << modeKeyword.value << std::endl;
        
        // 🚀 在扫描时调用参数处理器
        keyword.argsManager.match("url", urlKeyword);
        keyword.argsManager.match("mode", modeKeyword);
        
        // 根据参数组合进行智能优化
        if (modeKeyword.value == "ASCII") {
            std::cout << "🎨 启用ASCII优化模式" << std::endl;
        } else if (modeKeyword.value == "Pixel") {
            std::cout << "🖼️ 启用像素优化模式" << std::endl;
        }
    });
    
    // 🎯 第四步：生成代码
    std::string generatedCode = YourCodeGenerator::generateOptimizedCode(keyword);
    
    std::cout << "\n📝 printMylove生成的JavaScript代码：" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << generatedCode << std::endl;
}

/**
 * 更复杂的示例：音乐播放器语法
 */
void setupMusicPlayerSyntax() {
    std::cout << "\n🎵 音乐播放器语法设置" << std::endl;
    std::cout << "=====================" << std::endl;
    
    std::string ignoreSymbols = ",:{};()[]";
    
    const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        musicPlayer({
            container: $,
            playlist: [$, $, $],
            options: {
                autoplay: $,
                loop: $,
                volume: $,
                theme: $
            }
        });
    )", ignoreSymbols);
    
    // 绑定参数处理器
    keyword.argsManager.bind("container", [](const std::string& str) {
        return "document.querySelector('" + str + "')";
    });
    
    keyword.argsManager.bind("playlist", [](const std::string& str) {
        // 处理播放列表数组
        return "[" + str + "]";
    });
    
    keyword.argsManager.bind("autoplay", [](const std::string& str) {
        return (str == "true" || str == "1") ? "true" : "false";
    });
    
    keyword.argsManager.bind("loop", [](const std::string& str) {
        return (str == "true" || str == "1") ? "true" : "false";
    });
    
    keyword.argsManager.bind("volume", [](const std::string& str) {
        try {
            double vol = std::stod(str);
            vol = std::max(0.0, std::min(1.0, vol)); // 限制在0-1之间
            return std::to_string(vol);
        } catch (...) {
            return "0.5"; // 默认音量
        }
    });
    
    keyword.argsManager.bind("theme", [](const std::string& str) {
        std::vector<std::string> validThemes = {"dark", "light", "colorful", "minimal"};
        
        if (std::find(validThemes.begin(), validThemes.end(), str) != validThemes.end()) {
            return "'" + str + "'";
        } else {
            return "'dark'"; // 默认主题
        }
    });
    
    // 扫描器处理
    InnovativeScanner scanner;
    
    scanner.scanKeyword(keyword.arg[0], [&]() {
        std::cout << "🎵 扫描到音乐播放器语法！" << std::endl;
        
        // 智能参数匹配
        for (size_t i = 1; i < keyword.args.size(); ++i) {
            KeywordValue param = scanner.peekKeyword(i);
            keyword.argsManager.match(keyword.args[i], param);
        }
        
        std::cout << "🎶 音乐播放器配置完成" << std::endl;
    });
    
    std::string musicCode = YourCodeGenerator::generateCode(keyword);
    std::cout << "\n🎵 音乐播放器代码生成完成" << std::endl;
}

/**
 * 主演示函数
 */
int main() {
    std::cout << "🚀 您的创新CJMOD设计完整演示" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "展示syntaxAnalys + $ + args.bind + scanner的强大组合" << std::endl;
    std::cout << std::endl;
    
    // 演示printMylove
    setupPrintMyloveFunction();
    
    // 演示音乐播放器
    setupMusicPlayerSyntax();
    
    std::cout << "\n🎊 您的设计总结：" << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << "✅ syntaxAnalys(pattern, ignoreSymbols) - 智能模式解析" << std::endl;
    std::cout << "✅ $ 占位符 - 清晰的参数标记" << std::endl;
    std::cout << "✅ args.bind(name, lambda) - 命名参数绑定" << std::endl;
    std::cout << "✅ args.match(name, value) - 智能参数匹配" << std::endl;
    std::cout << "✅ scanner.scanKeyword() - 上下文感知扫描" << std::endl;
    std::cout << "✅ peekKeyword(±n) - 前后文查看" << std::endl;
    std::cout << "✅ generateCode() - 优雅代码生成" << std::endl;
    std::cout << "\n🌟 这个设计让复杂的对象语法解析变得极其简单！" << std::endl;
    std::cout << "🎉 特别是printMylove这样的复杂参数对象，处理得非常优雅！" << std::endl;
    
    return 0;
}

// ========================================
// CJMOD导出函数 - 按照您的设计
// ========================================

extern "C" const char* processYourDesignSyntax(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    // 🖼️ 处理printMylove语法
    if (code.find("printMylove") != std::string::npos) {
        std::cout << "🎯 检测到printMylove语法" << std::endl;
        
        std::string ignoreSymbols = ",:{};()";
        const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
            printMylove({
                url: $,
                mode: $,
                width: $,
                height: $,
                scale: $,
            });
        )", ignoreSymbols);
        
        // 设置参数绑定
        keyword.argsManager.bind("url", [](const std::string& str) {
            return "'" + str + "'";
        });
        
        keyword.argsManager.bind("mode", [](const std::string& str) {
            return "'" + str + "'";
        });
        
        keyword.argsManager.bind("width", [](const std::string& str) {
            // 智能宽度处理
            if (str.find("%") != std::string::npos) {
                return "Math.floor(window.innerWidth * " + str.substr(0, str.find("%")) + " / 100)";
            }
            return str;
        });
        
        keyword.argsManager.bind("height", [](const std::string& str) {
            // 智能高度处理
            if (str.find("%") != std::string::npos) {
                return "Math.floor(window.innerHeight * " + str.substr(0, str.find("%")) + " / 100)";
            }
            return str;
        });
        
        keyword.argsManager.bind("scale", [](const std::string& str) {
            try {
                double scale = std::stod(str);
                return std::to_string(std::max(0.1, std::min(scale, 10.0)));
            } catch (...) {
                return "1.0";
            }
        });
        
        // 设置扫描器
        InnovativeScanner scanner;
        scanner.scanKeyword(keyword.arg[0], [&]() {
            std::cout << "🔍 printMylove扫描回调执行" << std::endl;
            
            // 使用peekKeyword获取参数
            for (int i = 1; i <= 5; ++i) {
                KeywordValue param = scanner.peekKeyword(i);
                if (i <= keyword.args.size()) {
                    keyword.argsManager.match(keyword.args[i-1], param);
                }
            }
        });
        
        static std::string result = YourCodeGenerator::generateOptimizedCode(keyword);
        return result.c_str();
    }
    
    return chtljs_code;
}