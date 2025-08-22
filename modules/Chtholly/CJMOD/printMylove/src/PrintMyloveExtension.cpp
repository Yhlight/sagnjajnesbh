#include "PrintMyloveExtension.h"
#include <iostream>
#include <sstream>

namespace Chtholly {
namespace CJMOD {

// printMylove扩展实现 - 使用您设计的CJMOD API
// 语法文档第1472-1483行：将图片转换成字符像素块或ASCII形式，然后输出到控制台

bool PrintMyloveExtension::Initialize(CHTL::CJMOD::CJMODScanner& scanner) {
    // 使用您的CJMOD API注册printMylove语法扫描
    
    scanner.ScanKeyword("printMylove", [this, &scanner]() {
        // 当扫描到printMylove关键字时的处理
        
        // 设置收集策略来收集参数块
        scanner.PolicyChangeBegin("{", CHTL::CJMOD::ScanPolicy::COLLECT);
        std::string paramBlock = scanner.PolicyChangeEnd("}", CHTL::CJMOD::ScanPolicy::NORMAL);
        
        // 解析参数
        ProcessPrintMyloveCall(paramBlock);
    });
    
    std::cout << "printMylove扩展初始化完成（使用CJMOD API）" << std::endl;
    return true;
}

std::string PrintMyloveExtension::GetName() const {
    return "printMylove";
}

std::string PrintMyloveExtension::GetVersion() const {
    return "1.0.0";
}

std::vector<std::string> PrintMyloveExtension::GetSupportedSyntax() const {
    return {
        "printMylove({...})",           // 基本语法
        "const $ = printMylove({...})"  // 赋值语法
    };
}

void PrintMyloveExtension::ProcessPrintMyloveCall(const std::string& paramBlock) {
    // 使用您的CJMOD API处理printMylove调用
    
    // 创建语法分析 - 使用您的SyntaxAnalys函数
    auto syntax = CHTL::CJMOD::SyntaxAnalys("url: $, mode: $, width: $, height: $, scale: $", ",:{};()");
    
    if (!syntax) {
        std::cerr << "printMylove语法分析失败" << std::endl;
        return;
    }
    
    // 绑定参数处理函数 - 使用您的Arg绑定机制
    syntax->GetArgs().Bind("url", [](const CHTL::CJMOD::ArgValue& value) -> std::string {
        return "'" + std::get<std::string>(value) + "'";
    });
    
    syntax->GetArgs().Bind("mode", [](const CHTL::CJMOD::ArgValue& value) -> std::string {
        return "'" + std::get<std::string>(value) + "'";
    });
    
    syntax->GetArgs().Bind("width", [](const CHTL::CJMOD::ArgValue& value) -> std::string {
        return std::get<std::string>(value);
    });
    
    syntax->GetArgs().Bind("height", [](const CHTL::CJMOD::ArgValue& value) -> std::string {
        return std::get<std::string>(value);
    });
    
    syntax->GetArgs().Bind("scale", [](const CHTL::CJMOD::ArgValue& value) -> std::string {
        return std::get<std::string>(value);
    });
    
    // 从参数块中提取实际值并匹配
    ExtractAndMatchParams(paramBlock, *syntax);
    
    // 生成JavaScript代码
    std::string jsCode = GeneratePrintMyloveJS(*syntax);
    
    // 输出生成的代码（实际应该返回给编译器）
    std::cout << "生成的printMylove JavaScript代码:\n" << jsCode << std::endl;
}

void PrintMyloveExtension::ExtractAndMatchParams(const std::string& paramBlock, CHTL::CJMOD::Syntax& syntax) {
    // 从参数块中提取参数值
    // 简化实现：使用正则表达式提取键值对
    
    std::string url, mode, width, height, scale;
    
    // 提取url
    size_t urlPos = paramBlock.find("url:");
    if (urlPos != std::string::npos) {
        size_t start = paramBlock.find_first_of("\"'", urlPos);
        size_t end = paramBlock.find_first_of("\"'", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            url = paramBlock.substr(start + 1, end - start - 1);
        }
    }
    
    // 提取mode
    size_t modePos = paramBlock.find("mode:");
    if (modePos != std::string::npos) {
        size_t start = paramBlock.find_first_of("\"'", modePos);
        size_t end = paramBlock.find_first_of("\"'", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            mode = paramBlock.substr(start + 1, end - start - 1);
        }
    }
    
    // 简化处理其他参数
    width = "80";
    height = "40";
    scale = "1.0";
    
    // 使用您的API匹配参数
    syntax.GetArgs().Match("url", url);
    syntax.GetArgs().Match("mode", mode);
    syntax.GetArgs().Match("width", width);
    syntax.GetArgs().Match("height", height);
    syntax.GetArgs().Match("scale", scale);
}

std::string PrintMyloveExtension::GeneratePrintMyloveJS(const CHTL::CJMOD::Syntax& syntax) {
    // 使用您的CJMOD API生成JavaScript代码
    
    std::ostringstream js;
    
    js << "(function() {\n";
    js << "    // printMylove扩展 - 使用CJMOD API生成\n";
    js << "    const config = {\n";
    js << "        url: " << syntax.GetArgs()[0].GetCurrentValue() << ",\n";
    js << "        mode: " << syntax.GetArgs()[1].GetCurrentValue() << ",\n";
    js << "        width: " << syntax.GetArgs()[2].GetCurrentValue() << ",\n";
    js << "        height: " << syntax.GetArgs()[3].GetCurrentValue() << ",\n";
    js << "        scale: " << syntax.GetArgs()[4].GetCurrentValue() << "\n";
    js << "    };\n\n";
    
    js << "    function convertToASCII() {\n";
    js << "        const canvas = document.createElement('canvas');\n";
    js << "        const ctx = canvas.getContext('2d');\n";
    js << "        const img = new Image();\n";
    js << "        \n";
    js << "        img.onload = function() {\n";
    js << "            canvas.width = parseInt(config.width);\n";
    js << "            canvas.height = parseInt(config.height);\n";
    js << "            ctx.drawImage(img, 0, 0, canvas.width, canvas.height);\n";
    js << "            \n";
    js << "            const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);\n";
    js << "            const pixels = imageData.data;\n";
    js << "            \n";
    js << "            let asciiArt = '';\n";
    js << "            const asciiChars = ' .:-=+*#%@';\n";
    js << "            \n";
    js << "            for (let y = 0; y < canvas.height; y++) {\n";
    js << "                for (let x = 0; x < canvas.width; x++) {\n";
    js << "                    const offset = (y * canvas.width + x) * 4;\n";
    js << "                    const brightness = (pixels[offset] + pixels[offset + 1] + pixels[offset + 2]) / 3;\n";
    js << "                    const charIndex = Math.floor(brightness / 255 * (asciiChars.length - 1));\n";
    js << "                    asciiArt += asciiChars[charIndex];\n";
    js << "                }\n";
    js << "                asciiArt += '\\n';\n";
    js << "            }\n";
    js << "            \n";
    js << "            console.log(asciiArt);\n";
    js << "        };\n";
    js << "        \n";
    js << "        img.src = config.url;\n";
    js << "    }\n\n";
    
    js << "    if (config.mode === 'ASCII') {\n";
    js << "        convertToASCII();\n";
    js << "    } else {\n";
    js << "        console.log('像素块模式待实现');\n";
    js << "    }\n";
    js << "})();";
    
    return js.str();
}

} // namespace CJMOD
} // namespace Chtholly