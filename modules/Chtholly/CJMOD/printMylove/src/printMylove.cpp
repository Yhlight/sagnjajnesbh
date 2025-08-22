#include "printMylove.h"
#include "Utils/ErrorHandler.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace CJMOD {

// printMylove扩展实现 - 语法文档第1472-1483行
// 功能：将图片转换成字符像素块或ASCII形式，然后输出到控制台

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

CHTLJS::AST::ASTNodePtr PrintMyloveExtension::ParseSyntax(const std::string& input, const std::any& context) const {
    // 解析printMylove语法
    // 语法文档第1476-1482行：
    // const str = printMylove({
    //     url: ,
    //     mode: ,  // 模式可以选择ASCII或Pixel
    //     width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    //     height: ,  // 高度
    //     scale:  ,  // 缩放倍数，限定为等比缩放策略
    // });
    
    PrintMyloveParams params = ParseParameters(input);
    
    if (!params.IsValid()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "printMylove参数无效: " + input
        );
        return nullptr;
    }
    
    // 创建特殊的AST节点表示printMylove调用
    // 这里返回一个标识符节点，实际的代码生成在GenerateJavaScript中进行
    auto node = std::make_shared<CHTLJS::AST::IdentifierNode>("printMylove_call", 
                                                              CHTLJS::Core::CHTLJSToken());
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "printMylove语法解析完成，模式: " + params.mode + "，URL: " + params.url
    );
    
    return node;
}

std::string PrintMyloveExtension::GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const {
    // 生成printMylove的JavaScript实现
    // 语法文档第1473-1474行：将图片转换成字符像素块或ASCII形式，然后输出到控制台
    
    // 从context中获取原始输入来解析参数
    std::string input;
    try {
        input = std::any_cast<std::string>(context);
    } catch (const std::bad_any_cast&) {
        Utils::ErrorHandler::GetInstance().LogError(
            "printMylove代码生成失败：无法获取输入上下文"
        );
        return "";
    }
    
    PrintMyloveParams params = ParseParameters(input);
    return GeneratePrintMyloveJS(params);
}

bool PrintMyloveExtension::Initialize() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "printMylove扩展初始化完成"
    );
    return true;
}

void PrintMyloveExtension::Cleanup() {
    Utils::ErrorHandler::GetInstance().LogInfo(
        "printMylove扩展清理完成"
    );
}

PrintMyloveExtension::PrintMyloveParams PrintMyloveExtension::ParseParameters(const std::string& paramString) {
    PrintMyloveParams params;
    
    // 提取参数块内容
    std::regex paramBlockRegex(R"(printMylove\s*\(\s*\{([^}]+)\}\s*\))");
    std::smatch blockMatch;
    
    if (!std::regex_search(paramString, blockMatch, paramBlockRegex)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "printMylove参数块解析失败: " + paramString
        );
        return params;
    }
    
    std::string paramContent = blockMatch[1].str();
    
    // 解析各个参数
    std::regex paramRegex(R"((\w+)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(paramContent.begin(), paramContent.end(), paramRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string key = Utils::StringUtils::Trim(match[1].str());
        std::string value = Utils::StringUtils::Trim(match[2].str());
        
        // 移除引号
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
    
    js << "    // 图片处理函数\n";
    js << "    function convertImageToASCII(imageUrl, width, height) {\n";
    js << "        // ASCII转换实现\n";
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
    
    js << "    function convertImageToPixel(imageUrl, width, height) {\n";
    js << "        // 像素块转换实现\n";
    js << "        console.log('像素块模式暂未实现');\n";
    js << "    }\n\n";
    
    js << "    // 根据模式选择转换方式\n";
    js << "    if (printMyloveConfig.mode === 'ASCII') {\n";
    js << "        convertImageToASCII(printMyloveConfig.url, \n";
    js << "                           parseInt(printMyloveConfig.width) || 80, \n";
    js << "                           parseInt(printMyloveConfig.height) || 40);\n";
    js << "    } else if (printMyloveConfig.mode === 'Pixel') {\n";
    js << "        convertImageToPixel(printMyloveConfig.url, \n";
    js << "                           parseInt(printMyloveConfig.width) || 80, \n";
    js << "                           parseInt(printMyloveConfig.height) || 40);\n";
    js << "    } else {\n";
    js << "        console.error('printMylove: 不支持的模式 ' + printMyloveConfig.mode);\n";
    js << "    }\n";
    js << "})()";
    
    return js.str();
}

bool PrintMyloveExtension::PrintMyloveParams::IsValid() const {
    return !url.empty() && !mode.empty() && 
           (mode == "ASCII" || mode == "Pixel") &&
           !width.empty() && !height.empty();
}

std::pair<int, int> PrintMyloveExtension::ParseDimensions(const std::string& width, const std::string& height) {
    // 解析宽度和高度
    // 语法文档第1479-1480行：支持CSS单位、百分比、小数、纯数字(像素)
    
    int w = 80, h = 40;  // 默认值
    
    try {
        // 简化实现：只处理纯数字
        if (!width.empty() && std::isdigit(width[0])) {
            w = std::stoi(width);
        }
        if (!height.empty() && std::isdigit(height[0])) {
            h = std::stoi(height);
        }
    } catch (const std::exception&) {
        // 使用默认值
    }
    
    return {w, h};
}

double PrintMyloveExtension::ParseScale(const std::string& scale) {
    // 解析缩放倍数 - 语法文档第1481行：等比缩放策略
    try {
        return std::stod(scale);
    } catch (const std::exception&) {
        return 1.0;  // 默认不缩放
    }
}

} // namespace CJMOD
} // namespace CHTL