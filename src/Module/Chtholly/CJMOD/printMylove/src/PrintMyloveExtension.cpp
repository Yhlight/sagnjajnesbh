#include "PrintMyloveExtension.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Chtholly {
namespace CJMOD {

// 全局配置注册表定义
std::unordered_map<std::string, std::string> g_printMyloveConfig;

void PrintMyloveExtension::Initialize() {
    InitializeDefaultConfig();
    std::cout << "[printMylove] CJMOD扩展已初始化" << std::endl;
}

void PrintMyloveExtension::InitializeDefaultConfig() {
    g_printMyloveConfig["default_mode"] = "ASCII";
    g_printMyloveConfig["default_width"] = "80";
    g_printMyloveConfig["default_height"] = "40";
    g_printMyloveConfig["default_scale"] = "1.0";
    g_printMyloveConfig["ascii_chars"] = " .:-=+*#%@";
}

std::string PrintMyloveExtension::ProcessPrintMylove(const std::string& chtlJsCode) {
    // 使用TrueCJMODApi标准流程
    return ExecuteStandardFlow("printMylove($url, $mode, $width, $height, $scale)", chtlJsCode);
}

std::string PrintMyloveExtension::CreatePrintMyloveFunction(const std::string& chtlJsCode) {
    // 使用TrueCJMODApi简化流程
    return CHTL::CJMOD::createCHTLJSFunction(chtlJsCode);
}

std::string PrintMyloveExtension::ExecuteStandardFlow(const std::string& pattern, const std::string& chtlJsCode) {
    // 1. syntaxAnalys - 语法分析
    auto syntax = CHTL::CJMOD::syntaxAnalys(pattern);
    
    // 2. bind - 绑定处理器
    syntax->bind("url", [](const std::string& url) -> std::string {
        return "\"" + url + "\"";  // 确保URL被正确引用
    });
    
    syntax->bind("mode", [](const std::string& mode) -> std::string {
        std::string m = mode.empty() ? g_printMyloveConfig["default_mode"] : mode;
        return "\"" + m + "\"";
    });
    
    syntax->bind("width", [](const std::string& width) -> std::string {
        return width.empty() ? g_printMyloveConfig["default_width"] : width;
    });
    
    syntax->bind("height", [](const std::string& height) -> std::string {
        return height.empty() ? g_printMyloveConfig["default_height"] : height;
    });
    
    syntax->bind("scale", [](const std::string& scale) -> std::string {
        return scale.empty() ? g_printMyloveConfig["default_scale"] : scale;
    });
    
    // 3. transform - 转换为JS模板
    syntax->transform("url", "imageUrl: $");
    syntax->transform("mode", "mode: $");
    syntax->transform("width", "width: $");
    syntax->transform("height", "height: $");
    syntax->transform("scale", "scale: $");
    
    // 4. scanKeyword - 扫描关键字（这里模拟从chtlJsCode中提取值）
    // 实际实现中会使用统一扫描器
    
    // 5. match - 匹配参数值（模拟从代码片段中提取）
    syntax->match("url", "image.jpg");
    syntax->match("mode", "ASCII");
    syntax->match("width", "80");
    syntax->match("height", "40");
    syntax->match("scale", "1.0");
    
    // 6. result - 组合结果
    std::string paramStr = syntax->result();
    
    // 7. generateCode - 生成最终JS代码
    std::string finalCode = R"(
function printMylove(params) {
    const { )" + paramStr + R"( } = params;
    
    // 图像处理逻辑
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    return new Promise((resolve, reject) => {
        img.onload = function() {
            canvas.width = width;
            canvas.height = height;
            ctx.drawImage(img, 0, 0, width, height);
            
            const imageData = ctx.getImageData(0, 0, width, height);
            const pixels = imageData.data;
            
            let result = '';
            if (mode === 'ASCII') {
                const chars = ')" + g_printMyloveConfig["ascii_chars"] + R"(';
                for (let y = 0; y < height; y++) {
                    for (let x = 0; x < width; x++) {
                        const i = (y * width + x) * 4;
                        const brightness = (pixels[i] + pixels[i + 1] + pixels[i + 2]) / 3;
                        const charIndex = Math.floor((brightness / 255) * (chars.length - 1));
                        result += chars[charIndex];
                    }
                    result += '\n';
                }
            } else {
                // 像素块模式
                for (let y = 0; y < height; y++) {
                    for (let x = 0; x < width; x++) {
                        const i = (y * width + x) * 4;
                        const r = pixels[i];
                        const g = pixels[i + 1];
                        const b = pixels[i + 2];
                        result += `rgb(${r},${g},${b}) `;
                    }
                    result += '\n';
                }
            }
            
            resolve(result);
        };
        
        img.onerror = reject;
        img.src = imageUrl;
    });
}
)";
    
    return OptimizeOutputCode(finalCode);
}

std::string PrintMyloveExtension::ProcessImageToAscii(const std::string& imageUrl, int width, int height) {
    // 实际的图像处理逻辑会在这里实现
    // 这里返回模拟的ASCII艺术
    return "ASCII艺术输出占位符";
}

std::string PrintMyloveExtension::ProcessImageToPixel(const std::string& imageUrl, double scale) {
    // 实际的像素块处理逻辑会在这里实现
    return "像素块输出占位符";
}

std::string PrintMyloveExtension::OptimizeOutputCode(const std::string& jsCode) {
    // 代码优化：移除多余空行，压缩等
    std::string optimized = jsCode;
    
    // 移除多余的空行
    std::string result;
    std::istringstream stream(optimized);
    std::string line;
    bool lastWasEmpty = false;
    
    while (std::getline(stream, line)) {
        bool isEmpty = line.find_first_not_of(" \t") == std::string::npos;
        if (!isEmpty || !lastWasEmpty) {
            result += line + "\n";
        }
        lastWasEmpty = isEmpty;
    }
    
    return result;
}

} // namespace CJMOD
} // namespace Chtholly

// C接口实现
extern "C" {
    void RegisterPrintMyloveExtension() {
        Chtholly::CJMOD::PrintMyloveExtension::Initialize();
    }
    
    const char* ProcessPrintMyloveCall(const char* chtlJsCode) {
        static std::string result;
        try {
            result = Chtholly::CJMOD::PrintMyloveExtension::ProcessPrintMylove(chtlJsCode);
            return result.c_str();
        } catch (const std::exception& e) {
            result = "// Error: " + std::string(e.what());
            return result.c_str();
        }
    }
    
    void CleanupPrintMyloveExtension() {
        Chtholly::CJMOD::g_printMyloveConfig.clear();
    }
}