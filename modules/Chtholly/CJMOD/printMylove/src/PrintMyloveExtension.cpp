#include "PrintMyloveExtension.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace Chtholly {
namespace CJMOD {

// printMylove扩展实现 - 语法文档第1472-1483行
// 功能：将图片转换成字符像素块或ASCII形式，然后输出到控制台
// 通过CJMOD模块分发，不修改CHTL源码

std::vector<std::string> PrintMyloveExtension::GetSupportedSyntaxPatterns() const {
    return {
        "printMylove\\({[^}]+}\\)",  // printMylove({...}) 语法模式
        "const\\s+\\w+\\s*=\\s*printMylove\\({[^}]+}\\)"  // const str = printMylove({...})
    };
}

bool PrintMyloveExtension::MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const {
    // 检查是否匹配printMylove语法
    std::regex printMyloveRegex(R"(printMylove\s*\(\s*\{[^}]+\}\s*\))");
    return std::regex_search(syntaxPattern, printMyloveRegex);
}

std::string PrintMyloveExtension::ParseSyntax(const std::string& input, const std::any& context) const {
    // 解析printMylove语法并返回生成的JavaScript代码
    PrintMyloveParams params = ParseParameters(input);
    
    if (!params.IsValid()) {
        return "console.error('printMylove参数无效');";
    }
    
    return GeneratePrintMyloveJS(params);
}

std::string PrintMyloveExtension::GenerateJavaScript(const PrintMyloveParams& params) const {
    return GeneratePrintMyloveJS(params);
}

bool PrintMyloveExtension::Initialize() {
    std::cout << "printMylove扩展初始化完成" << std::endl;
    return true;
}

void PrintMyloveExtension::Cleanup() {
    std::cout << "printMylove扩展清理完成" << std::endl;
}

PrintMyloveExtension::PrintMyloveParams PrintMyloveExtension::ParseParameters(const std::string& paramString) {
    PrintMyloveParams params;
    
    // 提取参数块内容
    std::regex paramBlockRegex(R"(printMylove\s*\(\s*\{([^}]+)\}\s*\))");
    std::smatch blockMatch;
    
    if (!std::regex_search(paramString, blockMatch, paramBlockRegex)) {
        return params;
    }
    
    std::string paramContent = blockMatch[1].str();
    
    // 解析各个参数
    std::regex paramRegex(R"((\w+)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(paramContent.begin(), paramContent.end(), paramRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // 移除空白和引号
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            value = value.substr(1, value.length() - 2);
        }
        
        // 设置参数
        if (key == "url") {
            params.url = value;
        } else if (key == "mode") {
            params.mode = value;
        } else if (key == "width") {
            params.width = value;
        } else if (key == "height") {
            params.height = value;
        } else if (key == "scale") {
            params.scale = value;
        }
    }
    
    return params;
}

std::string PrintMyloveExtension::GeneratePrintMyloveJS(const PrintMyloveParams& params) {
    // 生成printMylove的JavaScript实现
    // 语法文档第1473-1474行：输出到控制台
    
    std::ostringstream js;
    
    js << "(function() {\n";
    js << "    // printMylove扩展 - 图片转ASCII/像素块\n";
    js << "    const printMyloveConfig = {\n";
    js << "        url: '" << params.url << "',\n";
    js << "        mode: '" << params.mode << "',\n";
    js << "        width: '" << params.width << "',\n";
    js << "        height: '" << params.height << "',\n";
    js << "        scale: '" << params.scale << "'\n";
    js << "    };\n\n";
    
    js << "    function convertImageToASCII(imageUrl, width, height) {\n";
    js << "        const canvas = document.createElement('canvas');\n";
    js << "        const ctx = canvas.getContext('2d');\n";
    js << "        const img = new Image();\n";
    js << "        \n";
    js << "        img.onload = function() {\n";
    js << "            canvas.width = width;\n";
    js << "            canvas.height = height;\n";
    js << "            ctx.drawImage(img, 0, 0, width, height);\n";
    js << "            \n";
    js << "            const imageData = ctx.getImageData(0, 0, width, height);\n";
    js << "            const pixels = imageData.data;\n";
    js << "            \n";
    js << "            let asciiArt = '';\n";
    js << "            const asciiChars = ' .:-=+*#%@';\n";
    js << "            \n";
    js << "            for (let y = 0; y < height; y++) {\n";
    js << "                for (let x = 0; x < width; x++) {\n";
    js << "                    const offset = (y * width + x) * 4;\n";
    js << "                    const r = pixels[offset];\n";
    js << "                    const g = pixels[offset + 1];\n";
    js << "                    const b = pixels[offset + 2];\n";
    js << "                    const brightness = (r + g + b) / 3;\n";
    js << "                    const charIndex = Math.floor(brightness / 255 * (asciiChars.length - 1));\n";
    js << "                    asciiArt += asciiChars[charIndex];\n";
    js << "                }\n";
    js << "                asciiArt += '\\n';\n";
    js << "            }\n";
    js << "            \n";
    js << "            console.log(asciiArt);\n";
    js << "        };\n";
    js << "        \n";
    js << "        img.src = imageUrl;\n";
    js << "    }\n\n";
    
    js << "    // 根据模式选择转换方式\n";
    js << "    if (printMyloveConfig.mode === 'ASCII') {\n";
    js << "        convertImageToASCII(printMyloveConfig.url, \n";
    js << "                           parseInt(printMyloveConfig.width) || 80, \n";
    js << "                           parseInt(printMyloveConfig.height) || 40);\n";
    js << "    } else if (printMyloveConfig.mode === 'Pixel') {\n";
    js << "        console.log('像素块模式待实现');\n";
    js << "    } else {\n";
    js << "        console.error('printMylove: 不支持的模式 ' + printMyloveConfig.mode);\n";
    js << "    }\n";
    js << "})();";
    
    return js.str();
}

bool PrintMyloveExtension::PrintMyloveParams::IsValid() const {
    return !url.empty() && !mode.empty() && 
           (mode == "ASCII" || mode == "Pixel") &&
           !width.empty() && !height.empty();
}

} // namespace CJMOD
} // namespace Chtholly