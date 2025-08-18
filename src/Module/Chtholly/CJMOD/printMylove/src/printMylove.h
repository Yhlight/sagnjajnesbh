#pragma once
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace cjmod {
namespace chtholly {

/**
 * printMylove - 珂朵莉图片转字符像素块函数
 * 严格按照CHTL语法文档实现
 * 
 * 功能：将图片转换成字符像素块或ASCII形式，然后输出到控制台
 * 
 * 使用方式：
 * const str = printMylove({
 *     url: "图片URL",
 *     mode: "ASCII" | "Pixel",
 *     width: "宽度",  // CSS单位、百分比、小数、纯数字(像素)
 *     height: "高度",
 *     scale: 缩放倍数  // 等比缩放策略
 * });
 */

struct PrintMyloveConfig {
    std::string url;           // 图片URL
    std::string mode;          // 模式：ASCII 或 Pixel
    std::string width;         // 宽度（支持CSS单位、百分比、小数、纯数字）
    std::string height;        // 高度
    double scale = 1.0;        // 缩放倍数（等比缩放）
    
    // 内部计算字段
    int pixelWidth = 0;        // 计算后的像素宽度
    int pixelHeight = 0;       // 计算后的像素高度
};

struct ImageData {
    std::vector<unsigned char> pixels;  // 图像像素数据
    int width;                         // 图像宽度
    int height;                        // 图像高度
    int channels;                      // 颜色通道数
};

class PrintMyloveProcessor {
public:
    PrintMyloveProcessor();
    ~PrintMyloveProcessor();
    
    /**
     * 按语法文档：主处理函数
     * 将图片转换为字符像素块或ASCII形式
     */
    std::string processImage(const PrintMyloveConfig& config);
    
    /**
     * 加载图片数据
     * 支持URL和本地文件路径
     */
    ImageData loadImage(const std::string& url);
    
    /**
     * 解析尺寸配置
     * 按语法文档：支持CSS单位、百分比、小数、纯数字(像素)
     */
    void parseDimensions(PrintMyloveConfig& config);
    
    /**
     * 应用等比缩放
     * 按语法文档：限定为等比缩放策略
     */
    void applyProportionalScaling(PrintMyloveConfig& config, const ImageData& imageData);
    
    /**
     * 转换为ASCII模式
     * 使用字符表示不同的灰度级别
     */
    std::string convertToASCII(const ImageData& imageData, const PrintMyloveConfig& config);
    
    /**
     * 转换为Pixel模式
     * 使用像素字符块表示
     */
    std::string convertToPixel(const ImageData& imageData, const PrintMyloveConfig& config);
    
    /**
     * 输出到控制台
     * 按语法文档：输出到控制台
     */
    void outputToConsole(const std::string& result);

private:
    // ASCII字符映射表（按亮度排序）
    static const std::string ASCII_CHARS;
    
    // 像素字符映射表
    static const std::vector<std::string> PIXEL_CHARS;
    
    /**
     * 计算像素亮度
     */
    int calculateBrightness(unsigned char r, unsigned char g, unsigned char b);
    
    /**
     * 映射亮度到ASCII字符
     */
    char mapBrightnessToASCII(int brightness);
    
    /**
     * 映射亮度到像素字符
     */
    std::string mapBrightnessToPixel(int brightness);
    
    /**
     * 调整图像大小
     */
    ImageData resizeImage(const ImageData& original, int newWidth, int newHeight);
    
    /**
     * 解析CSS单位
     */
    int parseCSSUnit(const std::string& value, int referenceSize = 100);
    
    /**
     * 验证配置参数
     */
    bool validateConfig(const PrintMyloveConfig& config);
};

// === CHTL JS接口函数 ===

/**
 * 按语法文档：CHTL JS接口函数
 * 在CHTL JS中调用：const str = printMylove({ ... });
 */
extern "C" {
    const char* chtl_printMylove(const char* configJson);
    void chtl_printMylove_cleanup(const char* result);
}

} // namespace chtholly
} // namespace cjmod
} // namespace chtl