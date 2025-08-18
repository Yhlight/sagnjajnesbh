#pragma once
#include <string>
#include <vector>

namespace chtl {
namespace cjmod {
namespace printMylove {

// printMylove 图像转字符功能
struct PrintMyloveOptions {
    std::string url;           // 图片URL
    std::string mode;          // ASCII 或 Pixel 模式
    std::string width;         // 宽度（支持CSS单位、百分比、小数、纯数字）
    std::string height;        // 高度
    double scale = 1.0;        // 缩放倍数（等比缩放）
};

class PrintMylove {
public:
    static std::string convert(const PrintMyloveOptions& options);
    
private:
    static std::string convertToASCII(const std::string& imagePath, int width, int height);
    static std::string convertToPixel(const std::string& imagePath, int width, int height);
    static std::pair<int, int> parseSize(const std::string& width, const std::string& height);
};

// CHTL JS 接口函数
extern "C" {
    const char* chtl_printMylove(const char* url, const char* mode, 
                                const char* width, const char* height, double scale);
}

} // namespace printMylove
} // namespace cjmod
} // namespace chtl