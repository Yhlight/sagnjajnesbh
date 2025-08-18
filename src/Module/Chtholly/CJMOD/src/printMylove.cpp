#include "printMylove.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <regex>

namespace chtl {
namespace chtholly {

// ASCII字符集定义 (从暗到亮)
const std::string PrintMyloveProcessor::ASCII_CHARS = " .:-=+*#%@";

// 像素字符集定义 (Unicode块字符，从透明到实心)
const std::vector<std::string> PrintMyloveProcessor::PIXEL_CHARS = {
    " ",     // 空白
    "░",     // 轻阴影
    "▒",     // 中阴影  
    "▓",     // 重阴影
    "█"      // 实心块
};

std::unique_ptr<PrintMyloveInterface> PrintMyloveInterface::instance_ = nullptr;

PrintMyloveProcessor::PrintMyloveProcessor() {
    // 初始化处理器
}

std::string PrintMyloveProcessor::processImage(const PrintMyloveConfig& config) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 验证配置
    if (!validateConfig(config)) {
        return "";
    }
    
    try {
        // 1. 加载图片
        auto pixel_data = loadImage(config.url);
        if (!pixel_data) {
            last_error_ = "无法加载图片: " + config.url;
            return "";
        }
        
        // 2. 计算目标尺寸
        auto [target_width, target_height] = calculateTargetSize(config, 
                                                                pixel_data->width, 
                                                                pixel_data->height);
        
        // 3. 调整图片尺寸
        auto resized_data = resizeImage(*pixel_data, target_width, target_height);
        if (!resized_data) {
            last_error_ = "图片尺寸调整失败";
            return "";
        }
        
        // 4. 转换为字符
        std::string result;
        if (config.mode == ConversionMode::ASCII) {
            result = convertToASCII(*resized_data);
        } else {
            result = convertToPixel(*resized_data);
        }
        
        // 5. 更新统计信息
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        updateStats(config, *pixel_data, result, duration.count() / 1000.0);
        
        return result;
        
    } catch (const std::exception& e) {
        last_error_ = "处理过程中发生错误: " + std::string(e.what());
        return "";
    }
}

std::unique_ptr<PixelData> PrintMyloveProcessor::loadImage(const std::string& url) {
    // 在实际实现中，这里会加载真实的图片
    // 现在我们模拟一个图片数据用于演示
    
    if (url.empty()) {
        return nullptr;
    }
    
    // 模拟加载Chtholly珂朵莉的图片数据
    // 创建一个简单的心形图案作为演示
    int width = 60;
    int height = 30;
    
    auto pixel_data = std::make_unique<PixelData>(width, height);
    
    // 生成心形图案 (为珂朵莉特制♥)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double nx = (x - width/2.0) / (width/4.0);
            double ny = (y - height/2.0) / (height/4.0);
            
            // 心形方程: (x²+y²-1)³ - x²y³ ≤ 0
            double heart_eq = std::pow(nx*nx + ny*ny - 1, 3) - nx*nx*ny*ny*ny;
            
            if (heart_eq <= 0) {
                // 心形内部，根据距离中心的远近设置灰度
                double dist = std::sqrt(nx*nx + ny*ny);
                pixel_data->grayscale[y][x] = static_cast<int>(255 * (1 - std::min(1.0, dist)));
            } else {
                pixel_data->grayscale[y][x] = 0; // 背景
            }
        }
    }
    
    return pixel_data;
}

std::unique_ptr<PixelData> PrintMyloveProcessor::resizeImage(const PixelData& original, 
                                                           int target_width, int target_height) {
    if (target_width <= 0 || target_height <= 0) {
        return nullptr;
    }
    
    auto resized = std::make_unique<PixelData>(target_width, target_height);
    
    double scale_x = static_cast<double>(original.width) / target_width;
    double scale_y = static_cast<double>(original.height) / target_height;
    
    for (int y = 0; y < target_height; y++) {
        for (int x = 0; x < target_width; x++) {
            double src_x = x * scale_x;
            double src_y = y * scale_y;
            
            // 双线性插值
            int gray_value = bilinearInterpolation(original.grayscale, src_x, src_y);
            resized->grayscale[y][x] = gray_value;
        }
    }
    
    return resized;
}

std::string PrintMyloveProcessor::convertToASCII(const PixelData& pixel_data) {
    std::ostringstream result;
    
    for (int y = 0; y < pixel_data.height; y++) {
        for (int x = 0; x < pixel_data.width; x++) {
            int gray_value = pixel_data.grayscale[y][x];
            char ascii_char = getASCIIChar(gray_value);
            result << ascii_char;
        }
        if (y < pixel_data.height - 1) {
            result << '\n';
        }
    }
    
    return result.str();
}

std::string PrintMyloveProcessor::convertToPixel(const PixelData& pixel_data) {
    std::ostringstream result;
    
    for (int y = 0; y < pixel_data.height; y++) {
        for (int x = 0; x < pixel_data.width; x++) {
            int gray_value = pixel_data.grayscale[y][x];
            std::string pixel_char = getPixelChar(gray_value);
            result << pixel_char;
        }
        if (y < pixel_data.height - 1) {
            result << '\n';
        }
    }
    
    return result.str();
}

char PrintMyloveProcessor::getASCIIChar(int grayscale_value) {
    // 将0-255的灰度值映射到ASCII字符
    int char_index = (grayscale_value * (ASCII_CHARS.length() - 1)) / 255;
    char_index = std::max(0, std::min(static_cast<int>(ASCII_CHARS.length() - 1), char_index));
    return ASCII_CHARS[char_index];
}

std::string PrintMyloveProcessor::getPixelChar(int grayscale_value) {
    // 将0-255的灰度值映射到像素字符
    int char_index = (grayscale_value * (PIXEL_CHARS.size() - 1)) / 255;
    char_index = std::max(0, std::min(static_cast<int>(PIXEL_CHARS.size() - 1), char_index));
    return PIXEL_CHARS[char_index];
}

std::pair<int, int> PrintMyloveProcessor::calculateTargetSize(const PrintMyloveConfig& config, 
                                                            int original_width, int original_height) {
    int target_width = original_width;
    int target_height = original_height;
    
    // 应用缩放
    if (config.scale != 1.0) {
        target_width = static_cast<int>(target_width * config.scale);
        target_height = static_cast<int>(target_height * config.scale);
    }
    
    // 处理指定的宽度
    if (config.width.value > 0) {
        target_width = convertToPixels(config.width, original_width);
    }
    
    // 处理指定的高度
    if (config.height.value > 0) {
        target_height = convertToPixels(config.height, original_height);
    }
    
    // 保持宽高比
    if (config.maintain_aspect_ratio && config.width.value > 0 && config.height.value == 0) {
        double aspect_ratio = static_cast<double>(original_height) / original_width;
        target_height = static_cast<int>(target_width * aspect_ratio);
    } else if (config.maintain_aspect_ratio && config.height.value > 0 && config.width.value == 0) {
        double aspect_ratio = static_cast<double>(original_width) / original_height;
        target_width = static_cast<int>(target_height * aspect_ratio);
    }
    
    // 应用最大尺寸限制
    target_width = std::min(target_width, config.max_width);
    target_height = std::min(target_height, config.max_height);
    
    // 确保最小尺寸
    target_width = std::max(1, target_width);
    target_height = std::max(1, target_height);
    
    return {target_width, target_height};
}

int PrintMyloveProcessor::convertToPixels(const SizeInfo& size, int reference_size) {
    switch (size.unit) {
        case SizeUnit::PIXEL:
            return static_cast<int>(size.value);
        case SizeUnit::PERCENT:
            return static_cast<int>(reference_size * size.value / 100.0);
        case SizeUnit::CSS_UNIT:
            // 简化处理CSS单位，实际实现中需要更复杂的转换
            if (size.unit_string == "em" || size.unit_string == "rem") {
                return static_cast<int>(size.value * 16); // 假设1em = 16px
            } else if (size.unit_string == "vw") {
                return static_cast<int>(size.value * 12); // 假设100vw = 1200px
            } else if (size.unit_string == "vh") {
                return static_cast<int>(size.value * 8);  // 假设100vh = 800px
            }
            return static_cast<int>(size.value); // 默认当作像素处理
        default:
            return static_cast<int>(size.value);
    }
}

bool PrintMyloveProcessor::validateConfig(const PrintMyloveConfig& config) {
    last_error_.clear();
    
    if (config.url.empty()) {
        last_error_ = "图片URL不能为空";
        return false;
    }
    
    if (config.scale <= 0) {
        last_error_ = "缩放倍数必须大于0";
        return false;
    }
    
    if (config.width.value < 0 || config.height.value < 0) {
        last_error_ = "宽度和高度不能为负数";
        return false;
    }
    
    return true;
}

int PrintMyloveProcessor::bilinearInterpolation(const std::vector<std::vector<int>>& data, 
                                              double x, double y) {
    int x1 = static_cast<int>(std::floor(x));
    int y1 = static_cast<int>(std::floor(y));
    int x2 = std::min(x1 + 1, static_cast<int>(data[0].size() - 1));
    int y2 = std::min(y1 + 1, static_cast<int>(data.size() - 1));
    
    x1 = std::max(0, x1);
    y1 = std::max(0, y1);
    
    double dx = x - x1;
    double dy = y - y1;
    
    int v11 = data[y1][x1];
    int v12 = data[y2][x1];
    int v21 = data[y1][x2];
    int v22 = data[y2][x2];
    
    double v1 = v11 * (1 - dx) + v21 * dx;
    double v2 = v12 * (1 - dx) + v22 * dx;
    double result = v1 * (1 - dy) + v2 * dy;
    
    return static_cast<int>(std::round(result));
}

void PrintMyloveProcessor::updateStats(const PrintMyloveConfig& config, const PixelData& input, 
                                      const std::string& output, double processing_time) {
    last_stats_.original_width = input.width;
    last_stats_.original_height = input.height;
    last_stats_.mode = config.mode;
    last_stats_.processing_time_ms = processing_time;
    last_stats_.output_length = output.length();
    
    // 计算输出尺寸
    size_t line_count = std::count(output.begin(), output.end(), '\n') + 1;
    last_stats_.output_height = static_cast<int>(line_count);
    
    if (line_count > 0) {
        size_t first_line_end = output.find('\n');
        if (first_line_end != std::string::npos) {
            last_stats_.output_width = static_cast<int>(first_line_end);
        } else {
            last_stats_.output_width = static_cast<int>(output.length());
        }
    }
}

SizeInfo PrintMyloveProcessor::parseSizeString(const std::string& size_str) {
    if (size_str.empty()) {
        return SizeInfo();
    }
    
    SizeUnit unit;
    std::string unit_str;
    double value = parseSizeValue(size_str, unit, unit_str);
    
    return SizeInfo(value, unit, unit_str);
}

double PrintMyloveProcessor::parseSizeValue(const std::string& size_str, SizeUnit& unit, std::string& unit_str) {
    std::regex number_regex(R"(([0-9]*\.?[0-9]+)(.*)?)");
    std::smatch match;
    
    if (std::regex_match(size_str, match, number_regex)) {
        double value = std::stod(match[1].str());
        std::string unit_part = match[2].str();
        
        // 去除空白
        unit_part.erase(0, unit_part.find_first_not_of(" \t"));
        unit_part.erase(unit_part.find_last_not_of(" \t") + 1);
        
        if (unit_part.empty()) {
            unit = SizeUnit::PIXEL;
        } else if (unit_part == "%") {
            unit = SizeUnit::PERCENT;
        } else {
            unit = SizeUnit::CSS_UNIT;
            unit_str = unit_part;
        }
        
        return value;
    }
    
    unit = SizeUnit::PIXEL;
    return 0.0;
}

// PrintMyloveInterface实现

PrintMyloveInterface& PrintMyloveInterface::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<PrintMyloveInterface>(new PrintMyloveInterface());
        instance_->processor_ = std::make_unique<PrintMyloveProcessor>();
    }
    return *instance_;
}

std::string PrintMyloveInterface::printMylove(const std::string& config_json) {
    clearError();
    
    try {
        PrintMyloveConfig config = parseConfigFromJSON(config_json);
        std::string result = processor_->processImage(config);
        
        if (result.empty() && !processor_->getErrorMessage().empty()) {
            last_error_ = processor_->getErrorMessage();
        }
        
        return result;
    } catch (const std::exception& e) {
        last_error_ = "JSON解析错误: " + std::string(e.what());
        return "";
    }
}

PrintMyloveConfig PrintMyloveInterface::parseConfigFromJSON(const std::string& json) {
    PrintMyloveConfig config;
    
    // 简化的JSON解析 (实际实现中应该使用专业的JSON库)
    config.url = extractJSONValue(json, "url");
    
    std::string mode_str = extractJSONValue(json, "mode");
    config.mode = parseMode(mode_str);
    
    std::string width_str = extractJSONValue(json, "width");
    if (!width_str.empty()) {
        config.width = processor_->parseSizeString(width_str);
    }
    
    std::string height_str = extractJSONValue(json, "height");
    if (!height_str.empty()) {
        config.height = processor_->parseSizeString(height_str);
    }
    
    std::string scale_str = extractJSONValue(json, "scale");
    if (!scale_str.empty()) {
        config.scale = std::stod(scale_str);
    }
    
    return config;
}

std::string PrintMyloveInterface::extractJSONValue(const std::string& json, const std::string& key) {
    // 简化的JSON值提取
    std::string pattern = "\"" + key + "\"\\s*:\\s*\"([^\"]*)\"|\"" + key + "\"\\s*:\\s*([^,}\\s]+)";
    std::regex json_regex(pattern);
    std::smatch match;
    
    if (std::regex_search(json, match, json_regex)) {
        return match[1].matched ? match[1].str() : match[2].str();
    }
    
    return "";
}

ConversionMode PrintMyloveInterface::parseMode(const std::string& mode_str) {
    if (mode_str == "ASCII" || mode_str == "ascii") {
        return ConversionMode::ASCII;
    } else if (mode_str == "PIXEL" || mode_str == "pixel" || mode_str == "Pixel") {
        return ConversionMode::PIXEL;
    }
    return ConversionMode::ASCII; // 默认
}

std::string PrintMyloveInterface::modeToString(ConversionMode mode) {
    switch (mode) {
        case ConversionMode::ASCII: return "ASCII";
        case ConversionMode::PIXEL: return "PIXEL";
        default: return "ASCII";
    }
}

} // namespace chtholly
} // namespace chtl