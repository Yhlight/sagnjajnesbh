#include "printMylove.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <cmath>
#include <algorithm>

namespace chtl {
namespace cjmod {
namespace printMylove {

// printMylove 实现
std::string PrintMylove::convert(const PrintMyloveOptions& options) {
    auto [width, height] = parseSize(options.width, options.height);
    
    // 应用缩放
    width = static_cast<int>(width * options.scale);
    height = static_cast<int>(height * options.scale);
    
    if (options.mode == "ASCII") {
        return convertToASCII(options.url, width, height);
    } else if (options.mode == "Pixel") {
        return convertToPixel(options.url, width, height);
    } else {
        return "Error: Invalid mode. Use 'ASCII' or 'Pixel'";
    }
}

std::string PrintMylove::convertToASCII(const std::string& imagePath, int width, int height) {
    // ASCII字符集（从暗到亮）
    const std::string asciiChars = "@%#*+=-:. ";
    
    std::ostringstream result;
    result << "// printMylove ASCII Art - " << imagePath << "\n";
    result << "// Size: " << width << "x" << height << "\n";
    result << "// 珂朵莉的ASCII艺术 - 世界上最幸福的女孩\n";
    
    // 模拟图像处理（实际实现需要图像库）
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // 模拟亮度计算 - 创建珂朵莉的轮廓
            double brightness = std::sin(x * 0.1) * std::cos(y * 0.1) * 0.5 + 0.5;
            int charIndex = static_cast<int>(brightness * (asciiChars.length() - 1));
            result << asciiChars[charIndex];
        }
        result << "\n";
    }
    
    result << "// 🌸 珂朵莉永远是世界上最幸福的女孩 🌸\n";
    return result.str();
}

std::string PrintMylove::convertToPixel(const std::string& imagePath, int width, int height) {
    std::ostringstream result;
    result << "// printMylove Pixel Art - " << imagePath << "\n";
    result << "// Size: " << width << "x" << height << "\n";
    result << "// 珂朵莉的像素艺术 - 永远的回忆\n";
    
    // 像素字符
    const std::string pixelChars = "██▓▒░  ";
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // 模拟像素密度计算
            double density = std::sin(x * 0.15) * std::cos(y * 0.15) * 0.5 + 0.5;
            int pixelIndex = static_cast<int>(density * (pixelChars.length() / 2 - 1)) * 2;
            result << pixelChars.substr(pixelIndex, 2);
        }
        result << "\n";
    }
    
    result << "// 💕 珂朵莉的像素艺术 - 永恒的约定 💕\n";
    return result.str();
}

std::pair<int, int> PrintMylove::parseSize(const std::string& width, const std::string& height) {
    auto parseUnit = [](const std::string& value) -> int {
        std::regex numberRegex(R"(^(\d*\.?\d+)(.*)$)");
        std::smatch match;
        
        if (std::regex_match(value, match, numberRegex)) {
            double num = std::stod(match[1].str());
            std::string unit = match[2].str();
            
            if (unit.empty() || unit == "px") {
                return static_cast<int>(num);
            } else if (unit == "%") {
                return static_cast<int>(num * 8); // 假设基础尺寸为800px
            } else {
                return static_cast<int>(num); // 默认处理
            }
        }
        return 80; // 默认值
    };
    
    return {parseUnit(width), parseUnit(height)};
}

} // namespace printMylove
} // namespace cjmod
} // namespace chtl

// C接口实现
extern "C" {
    using namespace chtl::cjmod::printMylove;
    
    const char* chtl_printMylove(const char* url, const char* mode, 
                                const char* width, const char* height, double scale) {
        PrintMyloveOptions options;
        options.url = url ? url : "";
        options.mode = mode ? mode : "ASCII";
        options.width = width ? width : "80";
        options.height = height ? height : "24";
        options.scale = scale;
        
        static std::string result = PrintMylove::convert(options);
        return result.c_str();
    }
}