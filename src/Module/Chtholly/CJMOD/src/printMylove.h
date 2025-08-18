#pragma once
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace chtholly {

/**
 * 图片转换模式
 */
enum class ConversionMode {
    ASCII,      // ASCII字符模式
    PIXEL       // 像素字符模式
};

/**
 * 尺寸单位类型
 */
enum class SizeUnit {
    PIXEL,      // 像素 (纯数字)
    PERCENT,    // 百分比
    CSS_UNIT    // CSS单位 (px, em, rem, vw, vh等)
};

/**
 * 尺寸信息
 */
struct SizeInfo {
    double value;
    SizeUnit unit;
    std::string unit_string;  // CSS单位的字符串表示
    
    SizeInfo(double v = 0, SizeUnit u = SizeUnit::PIXEL, const std::string& us = "")
        : value(v), unit(u), unit_string(us) {}
};

/**
 * 图片像素数据
 */
struct PixelData {
    int width;
    int height;
    std::vector<std::vector<int>> grayscale; // 灰度值矩阵
    
    PixelData(int w = 0, int h = 0) : width(w), height(h) {
        if (w > 0 && h > 0) {
            grayscale.resize(h, std::vector<int>(w, 0));
        }
    }
};

/**
 * printMylove配置参数
 */
struct PrintMyloveConfig {
    std::string url;           // 图片URL或路径
    ConversionMode mode;       // 转换模式
    SizeInfo width;           // 宽度
    SizeInfo height;          // 高度
    double scale;             // 缩放倍数
    
    // 高级配置
    bool maintain_aspect_ratio;  // 保持宽高比
    int max_width;              // 最大宽度限制
    int max_height;             // 最大高度限制
    bool use_color;             // 是否使用彩色（未来扩展）
    
    PrintMyloveConfig() 
        : mode(ConversionMode::ASCII), scale(1.0), 
          maintain_aspect_ratio(true), max_width(120), max_height(40), 
          use_color(false) {}
};

/**
 * printMylove核心实现类
 * 将图片转换为字符像素块或ASCII形式
 */
class PrintMyloveProcessor {
public:
    PrintMyloveProcessor();
    ~PrintMyloveProcessor() = default;
    
    // 主处理接口
    std::string processImage(const PrintMyloveConfig& config);
    
    // 图片加载和处理
    std::unique_ptr<PixelData> loadImage(const std::string& url);
    std::unique_ptr<PixelData> resizeImage(const PixelData& original, 
                                          int target_width, int target_height);
    
    // 转换功能
    std::string convertToASCII(const PixelData& pixel_data);
    std::string convertToPixel(const PixelData& pixel_data);
    
    // 尺寸计算
    SizeInfo parseSizeString(const std::string& size_str);
    std::pair<int, int> calculateTargetSize(const PrintMyloveConfig& config, 
                                           int original_width, int original_height);
    
    // 字符映射
    char getASCIIChar(int grayscale_value);
    std::string getPixelChar(int grayscale_value);
    
    // 配置和验证
    bool validateConfig(const PrintMyloveConfig& config);
    std::string getErrorMessage() const { return last_error_; }
    
    // 统计信息
    struct ProcessingStats {
        int original_width;
        int original_height;
        int output_width;
        int output_height;
        ConversionMode mode;
        double processing_time_ms;
        size_t output_length;
        
        ProcessingStats() : original_width(0), original_height(0), 
                          output_width(0), output_height(0),
                          mode(ConversionMode::ASCII), processing_time_ms(0.0), 
                          output_length(0) {}
    };
    
    ProcessingStats getLastStats() const { return last_stats_; }
    
private:
    std::string last_error_;
    ProcessingStats last_stats_;
    
    // ASCII字符集 (从暗到亮)
    static const std::string ASCII_CHARS;
    
    // 像素字符集 (Unicode块字符)
    static const std::vector<std::string> PIXEL_CHARS;
    
    // 内部处理方法
    std::vector<std::vector<int>> extractGrayscaleFromURL(const std::string& url);
    std::vector<std::vector<int>> simulateImageData(int width, int height); // 用于测试
    
    // 尺寸处理
    double parseSizeValue(const std::string& size_str, SizeUnit& unit, std::string& unit_str);
    int convertToPixels(const SizeInfo& size, int reference_size);
    
    // 图片处理算法
    int bilinearInterpolation(const std::vector<std::vector<int>>& data, 
                             double x, double y);
    std::vector<std::vector<int>> applyScale(const std::vector<std::vector<int>>& data, 
                                           double scale_x, double scale_y);
    
    // 输出格式化
    std::string formatOutput(const std::string& char_data, int width);
    void updateStats(const PrintMyloveConfig& config, const PixelData& input, 
                    const std::string& output, double processing_time);
};

/**
 * printMylove CHTL JS接口
 * 这是暴露给CHTL JS的C++接口
 */
class PrintMyloveInterface {
public:
    static PrintMyloveInterface& getInstance();
    
    // CHTL JS调用接口
    std::string printMylove(const std::string& config_json);
    
    // 配置解析
    PrintMyloveConfig parseConfigFromJSON(const std::string& json);
    std::string configToJSON(const PrintMyloveConfig& config);
    
    // 错误处理
    bool hasError() const { return !last_error_.empty(); }
    std::string getLastError() const { return last_error_; }
    void clearError() { last_error_.clear(); }
    
private:
    PrintMyloveInterface() = default;
    std::unique_ptr<PrintMyloveProcessor> processor_;
    std::string last_error_;
    
    // 单例实现
    static std::unique_ptr<PrintMyloveInterface> instance_;
    
    // JSON处理辅助
    std::string extractJSONValue(const std::string& json, const std::string& key);
    ConversionMode parseMode(const std::string& mode_str);
    std::string modeToString(ConversionMode mode);
};

} // namespace chtholly
} // namespace chtl