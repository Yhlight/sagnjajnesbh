#include "CJMOD/Extensions/PrintMyloveExtension.h"
#include "Utils/ErrorHandler.h"
#include <sstream>

namespace CHTL {
namespace CJMOD {
namespace Extensions {

PrintMyloveExtension::PrintMyloveExtension() : scanner_(nullptr) {}

bool PrintMyloveExtension::Initialize(CJMODScanner& scanner) {
    scanner_ = &scanner;
    
    // 定义printMylove语法模式（语法文档示例）
    std::string ignoreChars = ",:{};()";
    printMyloveSyntax_ = SyntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ignoreChars);
    
    // 绑定参数处理函数
    printMyloveSyntax_->GetArgs().Bind("url", [](const ArgValue& value) -> std::string {
        if (std::holds_alternative<std::string>(value)) {
            std::string url = std::get<std::string>(value);
            // 移除引号
            if (url.length() >= 2 && url.front() == '"' && url.back() == '"') {
                url = url.substr(1, url.length() - 2);
            }
            return "\"" + url + "\"";
        }
        return "\"\"";
    });
    
    printMyloveSyntax_->GetArgs().Bind("mode", [](const ArgValue& value) -> std::string {
        if (std::holds_alternative<std::string>(value)) {
            std::string mode = std::get<std::string>(value);
            // 验证模式值
            if (mode == "ASCII" || mode == "Pixel" || mode == "\"ASCII\"" || mode == "\"Pixel\"") {
                return mode;
            }
        }
        return "\"ASCII\""; // 默认模式
    });
    
    printMyloveSyntax_->GetArgs().Bind("width", [](const ArgValue& value) -> std::string {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        }
        return "100";
    });
    
    printMyloveSyntax_->GetArgs().Bind("height", [](const ArgValue& value) -> std::string {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        }
        return "100";
    });
    
    printMyloveSyntax_->GetArgs().Bind("scale", [](const ArgValue& value) -> std::string {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        }
        return "1.0";
    });
    
    // 注册关键字扫描
    scanner.ScanKeyword("printMylove", [this]() {
        HandlePrintMylove();
    });
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "PrintMylove扩展初始化成功"
    );
    
    return true;
}

std::vector<std::string> PrintMyloveExtension::GetSupportedSyntax() const {
    return {
        "printMylove({url: $, mode: $, width: $, height: $, scale: $});"
    };
}

void PrintMyloveExtension::HandlePrintMylove() {
    if (!scanner_) {
        return;
    }
    
    try {
        // 使用策略收集参数值
        std::vector<std::string> paramValues;
        
        // 收集URL参数
        scanner_->PolicyChangeBegin(":", ScanPolicy::COLLECT);
        std::string url = scanner_->PolicyChangeEnd(",", ScanPolicy::NORMAL);
        paramValues.push_back(url);
        
        // 收集mode参数
        scanner_->PolicyChangeBegin(":", ScanPolicy::COLLECT);
        std::string mode = scanner_->PolicyChangeEnd(",", ScanPolicy::NORMAL);
        paramValues.push_back(mode);
        
        // 收集width参数
        scanner_->PolicyChangeBegin(":", ScanPolicy::COLLECT);
        std::string width = scanner_->PolicyChangeEnd(",", ScanPolicy::NORMAL);
        paramValues.push_back(width);
        
        // 收集height参数
        scanner_->PolicyChangeBegin(":", ScanPolicy::COLLECT);
        std::string height = scanner_->PolicyChangeEnd(",", ScanPolicy::NORMAL);
        paramValues.push_back(height);
        
        // 收集scale参数
        scanner_->PolicyChangeBegin(":", ScanPolicy::COLLECT);
        std::string scale = scanner_->PolicyChangeEnd("}", ScanPolicy::NORMAL);
        paramValues.push_back(scale);
        
        // 匹配参数
        if (paramValues.size() >= 5) {
            printMyloveSyntax_->GetArgs().Match("url", paramValues[0]);
            printMyloveSyntax_->GetArgs().Match("mode", paramValues[1]);
            printMyloveSyntax_->GetArgs().Match("width", paramValues[2]);
            printMyloveSyntax_->GetArgs().Match("height", paramValues[3]);
            printMyloveSyntax_->GetArgs().Match("scale", paramValues[4]);
        }
        
        // 生成JavaScript代码
        std::string jsCode = GeneratePrintMyloveCode(
            paramValues.size() > 0 ? paramValues[0] : "",
            paramValues.size() > 1 ? paramValues[1] : "ASCII",
            paramValues.size() > 2 ? paramValues[2] : "100",
            paramValues.size() > 3 ? paramValues[3] : "100",
            paramValues.size() > 4 ? paramValues[4] : "1.0"
        );
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "PrintMylove处理完成，生成JavaScript代码长度: " + std::to_string(jsCode.length())
        );
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "PrintMylove处理异常: " + std::string(e.what())
        );
    }
}

std::string PrintMyloveExtension::GeneratePrintMyloveCode(const std::string& url, 
                                                         const std::string& mode,
                                                         const std::string& width,
                                                         const std::string& height,
                                                         const std::string& scale) {
    std::ostringstream jsCode;
    
    // 生成printMylove的JavaScript实现
    jsCode << "// printMylove功能实现 - 珂朵莉模块\n";
    jsCode << "(function() {\n";
    jsCode << "  function printMylove(config) {\n";
    jsCode << "    const canvas = document.createElement('canvas');\n";
    jsCode << "    const ctx = canvas.getContext('2d');\n";
    jsCode << "    const img = new Image();\n";
    jsCode << "    \n";
    jsCode << "    img.onload = function() {\n";
    jsCode << "      const targetWidth = " << width << ";\n";
    jsCode << "      const targetHeight = " << height << ";\n";
    jsCode << "      const scaleValue = " << scale << ";\n";
    jsCode << "      \n";
    jsCode << "      canvas.width = targetWidth * scaleValue;\n";
    jsCode << "      canvas.height = targetHeight * scaleValue;\n";
    jsCode << "      \n";
    jsCode << "      ctx.drawImage(img, 0, 0, canvas.width, canvas.height);\n";
    jsCode << "      \n";
    jsCode << "      const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);\n";
    jsCode << "      const pixels = imageData.data;\n";
    jsCode << "      \n";
    jsCode << "      let output = '';\n";
    jsCode << "      \n";
    jsCode << "      if (" << mode << " === 'ASCII') {\n";
    jsCode << "        // ASCII模式实现\n";
    jsCode << "        const asciiChars = ' .:-=+*#%@';\n";
    jsCode << "        for (let y = 0; y < canvas.height; y += 2) {\n";
    jsCode << "          for (let x = 0; x < canvas.width; x++) {\n";
    jsCode << "            const i = (y * canvas.width + x) * 4;\n";
    jsCode << "            const r = pixels[i];\n";
    jsCode << "            const g = pixels[i + 1];\n";
    jsCode << "            const b = pixels[i + 2];\n";
    jsCode << "            const brightness = (r + g + b) / 3;\n";
    jsCode << "            const charIndex = Math.floor(brightness / 255 * (asciiChars.length - 1));\n";
    jsCode << "            output += asciiChars[charIndex];\n";
    jsCode << "          }\n";
    jsCode << "          output += '\\n';\n";
    jsCode << "        }\n";
    jsCode << "      } else {\n";
    jsCode << "        // Pixel模式实现\n";
    jsCode << "        const pixelChars = '█▉▊▋▌▍▎▏ ';\n";
    jsCode << "        for (let y = 0; y < canvas.height; y += 2) {\n";
    jsCode << "          for (let x = 0; x < canvas.width; x++) {\n";
    jsCode << "            const i = (y * canvas.width + x) * 4;\n";
    jsCode << "            const r = pixels[i];\n";
    jsCode << "            const g = pixels[i + 1];\n";
    jsCode << "            const b = pixels[i + 2];\n";
    jsCode << "            const brightness = (r + g + b) / 3;\n";
    jsCode << "            const charIndex = Math.floor(brightness / 255 * (pixelChars.length - 1));\n";
    jsCode << "            output += pixelChars[charIndex];\n";
    jsCode << "          }\n";
    jsCode << "          output += '\\n';\n";
    jsCode << "        }\n";
    jsCode << "      }\n";
    jsCode << "      \n";
    jsCode << "      console.log(output);\n";
    jsCode << "      return output;\n";
    jsCode << "    };\n";
    jsCode << "    \n";
    jsCode << "    img.src = " << url << ";\n";
    jsCode << "  }\n";
    jsCode << "  \n";
    jsCode << "  // 调用printMylove\n";
    jsCode << "  return printMylove({\n";
    jsCode << "    url: " << url << ",\n";
    jsCode << "    mode: " << mode << ",\n";
    jsCode << "    width: " << width << ",\n";
    jsCode << "    height: " << height << ",\n";
    jsCode << "    scale: " << scale << "\n";
    jsCode << "  });\n";
    jsCode << "})();\n";
    
    return jsCode.str();
}

} // namespace Extensions
} // namespace CJMOD
} // namespace CHTL