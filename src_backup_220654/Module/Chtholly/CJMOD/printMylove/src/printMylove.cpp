#include "printMylove.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <regex>
#include <sstream>

// 注意：实际项目中需要链接stb_image库
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

namespace chtl {
namespace cjmod {
namespace chtholly {

// 静态成员定义
const std::string PrintMyloveProcessor::ASCII_CHARS = " .:-=+*#%@";

const std::vector<std::string> PrintMyloveProcessor::PIXEL_CHARS = {
    " ", "░", "▒", "▓", "█"
};

PrintMyloveProcessor::PrintMyloveProcessor() = default;
PrintMyloveProcessor::~PrintMyloveProcessor() = default;

// === 按语法文档：主处理函数 ===

std::string PrintMyloveProcessor::processImage(const PrintMyloveConfig& config) {
    // 按语法文档：将图片转换为字符像素块或ASCII形式
    
    if (!validateConfig(config)) {
        return "// printMylove错误：配置参数无效";
    }
    
    // 加载图片
    ImageData imageData = loadImage(config.url);
    if (imageData.pixels.empty()) {
        return "// printMylove错误：无法加载图片 " + config.url;
    }
    
    // 解析和应用配置
    PrintMyloveConfig processedConfig = config;
    parseDimensions(processedConfig);
    applyProportionalScaling(processedConfig, imageData);
    
    // 调整图像大小
    ImageData resizedImage = resizeImage(imageData, processedConfig.pixelWidth, processedConfig.pixelHeight);
    
    // 根据模式转换
    std::string result;
    if (processedConfig.mode == "ASCII") {
        result = convertToASCII(resizedImage, processedConfig);
    } else if (processedConfig.mode == "Pixel") {
        result = convertToPixel(resizedImage, processedConfig);
    } else {
        return "// printMylove错误：不支持的模式 " + processedConfig.mode;
    }
    
    // 按语法文档：输出到控制台
    outputToConsole(result);
    
    return result;
}

ImageData PrintMyloveProcessor::loadImage(const std::string& url) {
    ImageData data;
    
    // 简化实现：在实际项目中，这里应该使用stb_image或其他图像库
    // 这里创建一个模拟的图像数据用于演示
    
    if (url.empty()) {
        return data;
    }
    
    // 模拟加载珂朵莉主题的图像
    data.width = 64;
    data.height = 64;
    data.channels = 3;
    data.pixels.resize(data.width * data.height * data.channels);
    
    // 创建一个简单的渐变图案（粉色主题）
    for (int y = 0; y < data.height; ++y) {
        for (int x = 0; x < data.width; ++x) {
            int index = (y * data.width + x) * data.channels;
            
            // 创建珂朵莉主题的粉色渐变
            float centerX = data.width / 2.0f;
            float centerY = data.height / 2.0f;
            float distance = std::sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
            float normalized = std::min(1.0f, distance / (data.width / 2.0f));
            
            // 粉色主题 (255, 182, 193) 到白色渐变
            data.pixels[index] = static_cast<unsigned char>(255 - normalized * 73);     // R
            data.pixels[index + 1] = static_cast<unsigned char>(182 + normalized * 73); // G
            data.pixels[index + 2] = static_cast<unsigned char>(193 + normalized * 62); // B
        }
    }
    
    return data;
}

void PrintMyloveProcessor::parseDimensions(PrintMyloveConfig& config) {
    // 按语法文档：支持CSS单位、百分比、小数、纯数字(像素)
    
    if (!config.width.empty()) {
        config.pixelWidth = parseCSSUnit(config.width, 800); // 默认参考宽度800px
    } else {
        config.pixelWidth = 80; // 默认宽度
    }
    
    if (!config.height.empty()) {
        config.pixelHeight = parseCSSUnit(config.height, 600); // 默认参考高度600px
    } else {
        config.pixelHeight = 40; // 默认高度
    }
}

void PrintMyloveProcessor::applyProportionalScaling(PrintMyloveConfig& config, const ImageData& imageData) {
    // 按语法文档：限定为等比缩放策略
    
    if (config.scale != 1.0 && config.scale > 0) {
        config.pixelWidth = static_cast<int>(config.pixelWidth * config.scale);
        config.pixelHeight = static_cast<int>(config.pixelHeight * config.scale);
    }
    
    // 保持宽高比
    if (imageData.width > 0 && imageData.height > 0) {
        float imageRatio = static_cast<float>(imageData.width) / imageData.height;
        float targetRatio = static_cast<float>(config.pixelWidth) / config.pixelHeight;
        
        if (imageRatio > targetRatio) {
            // 图像更宽，以宽度为准
            config.pixelHeight = static_cast<int>(config.pixelWidth / imageRatio);
        } else {
            // 图像更高，以高度为准
            config.pixelWidth = static_cast<int>(config.pixelHeight * imageRatio);
        }
    }
}

std::string PrintMyloveProcessor::convertToASCII(const ImageData& imageData, const PrintMyloveConfig& config) {
    // 按语法文档：转换为ASCII形式
    
    std::stringstream result;
    result << "// printMylove ASCII模式输出\n";
    result << "// 图片: " << config.url << "\n";
    result << "// 尺寸: " << config.pixelWidth << "x" << config.pixelHeight << "\n\n";
    
    for (int y = 0; y < config.pixelHeight; ++y) {
        for (int x = 0; x < config.pixelWidth; ++x) {
            // 映射到原图像坐标
            int srcX = (x * imageData.width) / config.pixelWidth;
            int srcY = (y * imageData.height) / config.pixelHeight;
            int srcIndex = (srcY * imageData.width + srcX) * imageData.channels;
            
            if (srcIndex + 2 < static_cast<int>(imageData.pixels.size())) {
                unsigned char r = imageData.pixels[srcIndex];
                unsigned char g = imageData.pixels[srcIndex + 1];
                unsigned char b = imageData.pixels[srcIndex + 2];
                
                int brightness = calculateBrightness(r, g, b);
                char asciiChar = mapBrightnessToASCII(brightness);
                result << asciiChar;
            } else {
                result << ' ';
            }
        }
        result << '\n';
    }
    
    return result.str();
}

std::string PrintMyloveProcessor::convertToPixel(const ImageData& imageData, const PrintMyloveConfig& config) {
    // 按语法文档：转换为像素字符块形式
    
    std::stringstream result;
    result << "// printMylove Pixel模式输出\n";
    result << "// 图片: " << config.url << "\n";
    result << "// 尺寸: " << config.pixelWidth << "x" << config.pixelHeight << "\n\n";
    
    for (int y = 0; y < config.pixelHeight; ++y) {
        for (int x = 0; x < config.pixelWidth; ++x) {
            // 映射到原图像坐标
            int srcX = (x * imageData.width) / config.pixelWidth;
            int srcY = (y * imageData.height) / config.pixelHeight;
            int srcIndex = (srcY * imageData.width + srcX) * imageData.channels;
            
            if (srcIndex + 2 < static_cast<int>(imageData.pixels.size())) {
                unsigned char r = imageData.pixels[srcIndex];
                unsigned char g = imageData.pixels[srcIndex + 1];
                unsigned char b = imageData.pixels[srcIndex + 2];
                
                int brightness = calculateBrightness(r, g, b);
                std::string pixelChar = mapBrightnessToPixel(brightness);
                result << pixelChar;
            } else {
                result << ' ';
            }
        }
        result << '\n';
    }
    
    return result.str();
}

void PrintMyloveProcessor::outputToConsole(const std::string& result) {
    // 按语法文档：输出到控制台
    std::cout << "\n=== printMylove 输出 ===\n";
    std::cout << result;
    std::cout << "=== 输出完成 ===\n" << std::endl;
}

// === 辅助方法实现 ===

int PrintMyloveProcessor::calculateBrightness(unsigned char r, unsigned char g, unsigned char b) {
    // 使用加权平均计算亮度
    return static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
}

char PrintMyloveProcessor::mapBrightnessToASCII(int brightness) {
    int index = (brightness * (ASCII_CHARS.length() - 1)) / 255;
    index = std::max(0, std::min(static_cast<int>(ASCII_CHARS.length() - 1), index));
    return ASCII_CHARS[index];
}

std::string PrintMyloveProcessor::mapBrightnessToPixel(int brightness) {
    int index = (brightness * (PIXEL_CHARS.size() - 1)) / 255;
    index = std::max(0, std::min(static_cast<int>(PIXEL_CHARS.size() - 1), index));
    return PIXEL_CHARS[index];
}

ImageData PrintMyloveProcessor::resizeImage(const ImageData& original, int newWidth, int newHeight) {
    ImageData resized;
    resized.width = newWidth;
    resized.height = newHeight;
    resized.channels = original.channels;
    resized.pixels.resize(newWidth * newHeight * resized.channels);
    
    // 简单的最近邻插值
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = (x * original.width) / newWidth;
            int srcY = (y * original.height) / newHeight;
            
            int srcIndex = (srcY * original.width + srcX) * original.channels;
            int dstIndex = (y * newWidth + x) * resized.channels;
            
            for (int c = 0; c < resized.channels; ++c) {
                if (srcIndex + c < static_cast<int>(original.pixels.size()) && 
                    dstIndex + c < static_cast<int>(resized.pixels.size())) {
                    resized.pixels[dstIndex + c] = original.pixels[srcIndex + c];
                }
            }
        }
    }
    
    return resized;
}

int PrintMyloveProcessor::parseCSSUnit(const std::string& value, int referenceSize) {
    // 按语法文档：解析CSS单位、百分比、小数、纯数字
    
    if (value.empty()) return 0;
    
    std::regex numberRegex(R"(^(\d+(?:\.\d+)?)(.*)$)");
    std::smatch match;
    
    if (std::regex_match(value, match, numberRegex)) {
        double number = std::stod(match[1].str());
        std::string unit = match[2].str();
        
        if (unit.empty() || unit == "px") {
            // 纯数字或像素
            return static_cast<int>(number);
        } else if (unit == "%") {
            // 百分比
            return static_cast<int>(number * referenceSize / 100.0);
        } else if (unit == "em") {
            // em单位（假设1em = 16px）
            return static_cast<int>(number * 16);
        } else if (unit == "rem") {
            // rem单位（假设1rem = 16px）
            return static_cast<int>(number * 16);
        } else {
            // 其他单位，返回数值部分
            return static_cast<int>(number);
        }
    }
    
    return 0;
}

bool PrintMyloveProcessor::validateConfig(const PrintMyloveConfig& config) {
    if (config.url.empty()) {
        std::cerr << "printMylove错误：缺少图片URL" << std::endl;
        return false;
    }
    
    if (config.mode != "ASCII" && config.mode != "Pixel") {
        std::cerr << "printMylove错误：模式必须是ASCII或Pixel" << std::endl;
        return false;
    }
    
    if (config.scale <= 0) {
        std::cerr << "printMylove错误：缩放倍数必须大于0" << std::endl;
        return false;
    }
    
    return true;
}

// === CHTL JS接口函数实现 ===

extern "C" {
    const char* chtl_printMylove(const char* configJson) {
        try {
            // 解析JSON配置（简化实现）
            PrintMyloveConfig config;
            
            // 这里应该解析JSON，简化实现直接设置默认值
            config.url = "chtholly_demo.png";
            config.mode = "ASCII";
            config.width = "60";
            config.height = "30";
            config.scale = 1.0;
            
            PrintMyloveProcessor processor;
            std::string result = processor.processImage(config);
            
            // 分配内存返回结果
            char* resultCopy = new char[result.length() + 1];
            std::strcpy(resultCopy, result.c_str());
            
            return resultCopy;
            
        } catch (const std::exception& e) {
            std::string error = "printMylove异常: " + std::string(e.what());
            char* errorCopy = new char[error.length() + 1];
            std::strcpy(errorCopy, error.c_str());
            return errorCopy;
        }
    }
    
    void chtl_printMylove_cleanup(const char* result) {
        delete[] result;
    }
}

} // namespace chtholly
} // namespace cjmod
} // namespace chtl