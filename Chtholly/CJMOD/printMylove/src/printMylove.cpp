#include "printMylove.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// ASCII字符集（从暗到亮）
const std::string ASCII_CHARS = " .:-=+*#%@";
const std::string UNICODE_CHARS = " ░▒▓█";
const std::string EMOJI_CHARS = "⚫⚪🔵🔴🟢🟡🟠🟣";

namespace Chtholly {
    namespace CJMOD {
        
        // 构造函数
        PrintMylove::PrintMylove() {
            this->mode = "ASCII";
            this->width = 80;
            this->height = 40;
            this->scale = 1.0;
            this->contrast = 1.0;
            this->brightness = 1.0;
            this->invertColors = false;
            this->enableDithering = false;
            this->enableEdgeDetection = false;
            this->colorMode = "GRAYSCALE";
            this->customCharSet = ASCII_CHARS;
        }
        
        // 析构函数
        PrintMylove::~PrintMylove() {
            // 清理资源
        }
        
        // 设置配置
        void PrintMylove::setConfig(const PrintMyloveConfig& config) {
            this->mode = config.mode;
            this->width = config.width;
            this->height = config.height;
            this->scale = config.scale;
            this->contrast = config.contrast;
            this->brightness = config.brightness;
            this->invertColors = config.invertColors;
            this->enableDithering = config.enableDithering;
            this->enableEdgeDetection = config.enableEdgeDetection;
            this->colorMode = config.colorMode;
            
            if (!config.customCharSet.empty()) {
                this->customCharSet = config.customCharSet;
            } else {
                // 根据模式选择字符集
                if (mode == "ASCII") {
                    this->customCharSet = ASCII_CHARS;
                } else if (mode == "UNICODE") {
                    this->customCharSet = UNICODE_CHARS;
                } else if (mode == "EMOJI") {
                    this->customCharSet = EMOJI_CHARS;
                }
            }
        }
        
        // 从URL/路径加载图片（模拟实现）
        bool PrintMylove::loadImage(const std::string& imagePath) {
            // 这里是模拟实现，实际应该使用图像处理库
            // 如OpenCV、STBI等来加载和处理图片
            this->imagePath = imagePath;
            
            // 模拟图片加载成功
            std::cout << "[printMylove] 正在加载图片: " << imagePath << std::endl;
            
            // 创建模拟的灰度图像数据
            imageData.clear();
            for (int y = 0; y < height; ++y) {
                std::vector<unsigned char> row;
                for (int x = 0; x < width; ++x) {
                    // 创建一个简单的渐变图案作为演示
                    unsigned char value = static_cast<unsigned char>(
                        255 * (std::sin(x * 0.1) * std::cos(y * 0.1) + 1) / 2
                    );
                    row.push_back(value);
                }
                imageData.push_back(row);
            }
            
            return true;
        }
        
        // 将像素值转换为字符
        char PrintMylove::pixelToChar(unsigned char pixel) {
            // 应用亮度和对比度调整
            double adjustedPixel = pixel * brightness;
            adjustedPixel = 128 + (adjustedPixel - 128) * contrast;
            adjustedPixel = std::max(0.0, std::min(255.0, adjustedPixel));
            
            // 反转颜色（如果启用）
            if (invertColors) {
                adjustedPixel = 255 - adjustedPixel;
            }
            
            // 映射到字符集
            int charIndex = static_cast<int>(
                (adjustedPixel / 255.0) * (customCharSet.length() - 1)
            );
            
            return customCharSet[charIndex];
        }
        
        // 应用抖动算法
        void PrintMylove::applyDithering() {
            if (!enableDithering || imageData.empty()) return;
            
            // Floyd-Steinberg抖动算法的简化实现
            for (int y = 0; y < height - 1; ++y) {
                for (int x = 1; x < width - 1; ++x) {
                    unsigned char oldPixel = imageData[y][x];
                    unsigned char newPixel = (oldPixel > 127) ? 255 : 0;
                    imageData[y][x] = newPixel;
                    
                    int error = oldPixel - newPixel;
                    
                    // 分发误差到相邻像素
                    imageData[y][x + 1] += error * 7 / 16;
                    imageData[y + 1][x - 1] += error * 3 / 16;
                    imageData[y + 1][x] += error * 5 / 16;
                    imageData[y + 1][x + 1] += error * 1 / 16;
                }
            }
        }
        
        // 应用边缘检测
        void PrintMylove::applyEdgeDetection() {
            if (!enableEdgeDetection || imageData.empty()) return;
            
            // Sobel边缘检测算子
            std::vector<std::vector<unsigned char>> result = imageData;
            
            for (int y = 1; y < height - 1; ++y) {
                for (int x = 1; x < width - 1; ++x) {
                    // Sobel X
                    int gx = imageData[y-1][x-1] * -1 + imageData[y-1][x+1] * 1 +
                             imageData[y][x-1] * -2 + imageData[y][x+1] * 2 +
                             imageData[y+1][x-1] * -1 + imageData[y+1][x+1] * 1;
                    
                    // Sobel Y
                    int gy = imageData[y-1][x-1] * -1 + imageData[y-1][x] * -2 + imageData[y-1][x+1] * -1 +
                             imageData[y+1][x-1] * 1 + imageData[y+1][x] * 2 + imageData[y+1][x+1] * 1;
                    
                    // 计算梯度幅值
                    int magnitude = static_cast<int>(std::sqrt(gx * gx + gy * gy));
                    result[y][x] = std::min(255, magnitude);
                }
            }
            
            imageData = result;
        }
        
        // 转换为ASCII艺术
        std::string PrintMylove::convert() {
            if (imageData.empty()) {
                return "[printMylove Error] 没有加载图片数据";
            }
            
            // 应用图像处理效果
            applyDithering();
            applyEdgeDetection();
            
            std::string result;
            result.reserve(height * (width + 1)); // 预分配内存
            
            // 添加头部信息注释
            result += "<!-- Generated by Chtholly::printMylove -->\n";
            result += "<!-- Mode: " + mode + ", Size: " + std::to_string(width) + "x" + std::to_string(height) + " -->\n";
            
            // 转换图像数据为字符
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    result += pixelToChar(imageData[y][x]);
                }
                result += '\n';
            }
            
            return result;
        }
        
        // 静态工厂方法
        std::string PrintMylove::createAsciiArt(const PrintMyloveConfig& config) {
            PrintMylove converter;
            converter.setConfig(config);
            
            if (!converter.loadImage(config.url)) {
                return "[printMylove Error] 无法加载图片: " + config.url;
            }
            
            return converter.convert();
        }
        
        // 批量转换
        std::vector<std::string> PrintMylove::batchConvert(const std::vector<PrintMyloveConfig>& configs) {
            std::vector<std::string> results;
            results.reserve(configs.size());
            
            for (const auto& config : configs) {
                results.push_back(createAsciiArt(config));
            }
            
            return results;
        }
        
        // 保存到文件
        bool PrintMylove::saveToFile(const std::string& asciiArt, const std::string& filePath) {
            std::ofstream file(filePath);
            if (!file.is_open()) {
                std::cerr << "[printMylove Error] 无法创建文件: " << filePath << std::endl;
                return false;
            }
            
            file << asciiArt;
            file.close();
            
            std::cout << "[printMylove] ASCII艺术已保存到: " << filePath << std::endl;
            return true;
        }
        
        // 获取支持的模式列表
        std::vector<std::string> PrintMylove::getSupportedModes() {
            return {"ASCII", "UNICODE", "EMOJI", "CUSTOM"};
        }
        
        // 获取预设字符集
        std::string PrintMylove::getPresetCharSet(const std::string& mode) {
            if (mode == "ASCII") return ASCII_CHARS;
            if (mode == "UNICODE") return UNICODE_CHARS;
            if (mode == "EMOJI") return EMOJI_CHARS;
            return ASCII_CHARS; // 默认
        }
        
    } // namespace CJMOD
} // namespace Chtholly

// C风格的导出函数（用于CHTL JS集成）
extern "C" {
    
    // 创建ASCII艺术的简化接口
    const char* chtl_printMylove(const char* url, const char* mode, int width, int height, double scale) {
        static std::string result; // 静态存储，避免返回悬空指针
        
        Chtholly::CJMOD::PrintMyloveConfig config;
        config.url = url ? url : "";
        config.mode = mode ? mode : "ASCII";
        config.width = width > 0 ? width : 80;
        config.height = height > 0 ? height : 40;
        config.scale = scale > 0 ? scale : 1.0;
        
        result = Chtholly::CJMOD::PrintMylove::createAsciiArt(config);
        return result.c_str();
    }
    
    // 获取支持的模式
    const char* chtl_printMylove_modes() {
        static std::string modes = "ASCII,UNICODE,EMOJI,CUSTOM";
        return modes.c_str();
    }
    
}