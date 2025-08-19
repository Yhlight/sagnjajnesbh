// ========================================
// 您的CJMOD设计完整演示
// 完美实现您描述的所有功能
// ========================================

#include "YourCJMODDesign.h"
#include <iostream>

using namespace chtl::your_cjmod_design;

/**
 * 完整演示您的设计流程
 */
int main() {
    std::cout << "🚀 您的CJMOD设计完整演示" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "展示syntaxAnalys + bind + scanner + match + result的完整流程" << std::endl;
    std::cout << std::endl;
    
    // 🎯 第一步：使用syntaxAnalys函数
    std::cout << "📝 第一步：语法分析" << std::endl;
    std::cout << "=================" << std::endl;
    
    std::string ignoreSymbols = ",:{};()";   // 您指定的忽略符号
    
    const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,  // 模式可以选择ASCII或Pixel
            width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
            height: $,  // 高度
            scale:  $,  // 缩放倍数，限定为等比缩放策略
        });
    )", ignoreSymbols);
    
    std::cout << "✅ syntaxAnalys完成，提取到的关键字：" << std::endl;
    for (const auto& name : keyword.args.getAllKeywordNames()) {
        std::cout << "   - " << name << std::endl;
    }
    std::cout << std::endl;
    
    // 🔗 第二步：使用bind方法绑定采集函数
    std::cout << "📝 第二步：绑定参数处理函数" << std::endl;
    std::cout << "=========================" << std::endl;
    
    keyword.args.bind("url", [](std::string str) -> std::string {
        std::cout << "🌐 处理URL: " << str << std::endl;
        
        // URL智能处理
        if (str.find("http") == 0) {
            return "'" + str + "'";  // 网络URL
        } else if (str.find("data:") == 0) {
            return "'" + str + "'";  // Base64 URL
        } else {
            return "'./images/" + str + "'";  // 相对路径
        }
    });
    
    keyword.args.bind("mode", [](std::string str) -> std::string {
        std::cout << "🎨 处理模式: " << str << std::endl;
        
        if (str == "ASCII" || str == "Pixel") {
            return "'" + str + "'";
        } else {
            std::cout << "⚠️ 无效模式，使用ASCII" << std::endl;
            return "'ASCII'";
        }
    });
    
    keyword.args.bind("width", [](std::string str) -> std::string {
        std::cout << "📏 处理宽度: " << str << std::endl;
        
        // 支持CSS单位、百分比、小数、纯数字
        if (str.find("%") != std::string::npos) {
            std::string percent = str.substr(0, str.find("%"));
            return "Math.floor(window.innerWidth * " + percent + " / 100)";
        } else if (str.find("px") != std::string::npos) {
            return str.substr(0, str.find("px"));
        } else if (str.find("em") != std::string::npos) {
            std::string em = str.substr(0, str.find("em"));
            return "Math.floor(16 * " + em + ")";
        } else {
            // 纯数字
            try {
                double num = std::stod(str);
                return std::to_string(static_cast<int>(num));
            } catch (...) {
                return "100";
            }
        }
    });
    
    keyword.args.bind("height", [](std::string str) -> std::string {
        std::cout << "📐 处理高度: " << str << std::endl;
        // 与width相同的处理逻辑
        if (str.find("%") != std::string::npos) {
            std::string percent = str.substr(0, str.find("%"));
            return "Math.floor(window.innerHeight * " + percent + " / 100)";
        } else if (str.find("px") != std::string::npos) {
            return str.substr(0, str.find("px"));
        } else {
            try {
                double num = std::stod(str);
                return std::to_string(static_cast<int>(num));
            } catch (...) {
                return "100";
            }
        }
    });
    
    keyword.args.bind("scale", [](std::string str) -> std::string {
        std::cout << "🔍 处理缩放: " << str << std::endl;
        
        try {
            double scale = std::stod(str);
            scale = std::max(0.1, std::min(scale, 10.0));  // 限制范围
            return std::to_string(scale);
        } catch (...) {
            return "1.0";
        }
    });
    
    std::cout << "✅ 所有参数处理函数绑定完成" << std::endl;
    std::cout << std::endl;
    
    // 🔍 第三步：设置扫描器
    std::cout << "📝 第三步：设置扫描器" << std::endl;
    std::cout << "==================" << std::endl;
    
    CJMODScanner scanner;
    
    scanner.scanKeyword(keyword.args[0], [&]() {
        std::cout << "🎯 扫描到printMylove函数，开始参数匹配..." << std::endl;
        
        // 🚀 使用您的peekKeyword和match设计
        keyword.args.match("url", scanner.peekKeyword(1));
        keyword.args.match("mode", scanner.peekKeyword(2));
        keyword.args.match("width", scanner.peekKeyword(3));
        keyword.args.match("height", scanner.peekKeyword(4));
        keyword.args.match("scale", scanner.peekKeyword(5));
        
        std::cout << "✅ 所有参数匹配完成" << std::endl;
    });
    
    // 🎯 第四步：模拟扫描过程
    std::cout << "📝 第四步：执行扫描" << std::endl;
    std::cout << "=================" << std::endl;
    
    // 模拟的CHTL JS代码
    std::string testCHTLJS = R"(
        printMylove({
            url: 'avatar.jpg',
            mode: 'ASCII',  // 使用ASCII模式
            width: '80%',   // 80%宽度
            height: 60,     // 60像素高度
            scale: 1.5,     // 1.5倍缩放
        });
    )";
    
    std::cout << "🔍 扫描测试代码：" << std::endl;
    std::cout << testCHTLJS << std::endl;
    
    scanner.executeScan(testCHTLJS, keyword);
    
    // 🎯 第五步：生成最终代码
    std::cout << "📝 第五步：生成最终代码" << std::endl;
    std::cout << "=====================" << std::endl;
    
    std::string finalCode = YourCodeGenerator::generateCode(keyword);
    
    std::cout << "🎉 最终生成的JavaScript代码：" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << finalCode << std::endl;
    
    // 🎊 总结
    std::cout << "\n🎊 您的设计流程总结：" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "1️⃣ syntaxAnalys(字符串, 忽略符号) → 分析出关键字" << std::endl;
    std::cout << "2️⃣ args.bind(名称, lambda) → 绑定处理函数" << std::endl;
    std::cout << "3️⃣ scanner.scanKeyword(关键字, 回调) → 设置扫描器" << std::endl;
    std::cout << "4️⃣ args.match(名称, peekKeyword(n)) → 匹配参数" << std::endl;
    std::cout << "5️⃣ generateCode(keyword) → 生成最终代码" << std::endl;
    std::cout << "\n🌟 这个设计让复杂的语法处理变得极其简单和直观！" << std::endl;
    std::cout << "🎉 特别适合处理像printMylove这样的复杂对象参数！" << std::endl;
    
    return 0;
}

// ========================================
// 实际的CJMOD导出函数
// ========================================

extern "C" const char* processYourInnovativeDesign(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    if (code.find("printMylove") != std::string::npos) {
        std::cout << "🎯 检测到printMylove语法，使用您的设计处理" << std::endl;
        
        // 🚀 使用您的完整设计
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
        
        // 绑定所有参数处理器
        keyword.args.bind("url", [](std::string str) {
            return "'" + str + "'";
        });
        
        keyword.args.bind("mode", [](std::string str) {
            return "'" + str + "'";
        });
        
        keyword.args.bind("width", [](std::string str) {
            if (str.find("%") != std::string::npos) {
                return "Math.floor(window.innerWidth * " + str.substr(0, str.find("%")) + " / 100)";
            }
            return str;
        });
        
        keyword.args.bind("height", [](std::string str) {
            if (str.find("%") != std::string::npos) {
                return "Math.floor(window.innerHeight * " + str.substr(0, str.find("%")) + " / 100)";
            }
            return str;
        });
        
        keyword.args.bind("scale", [](std::string str) {
            try {
                double scale = std::stod(str);
                return std::to_string(std::max(0.1, std::min(scale, 10.0)));
            } catch (...) {
                return "1.0";
            }
        });
        
        // 设置扫描器
        CJMODScanner scanner;
        scanner.scanKeyword(keyword.args[0], [&]() {
            // 使用您的match设计
            keyword.args.match("url", scanner.peekKeyword(1));
            keyword.args.match("mode", scanner.peekKeyword(2));
            keyword.args.match("width", scanner.peekKeyword(3));
            keyword.args.match("height", scanner.peekKeyword(4));
            keyword.args.match("scale", scanner.peekKeyword(5));
        });
        
        // 执行扫描
        scanner.executeScan(code, keyword);
        
        // 生成最终代码
        static std::string result = YourCodeGenerator::generateCode(keyword);
        return result.c_str();
    }
    
    return chtljs_code;
}