#ifndef CHTHOLLY_CJMOD_PRINTMYLOVE_H
#define CHTHOLLY_CJMOD_PRINTMYLOVE_H

#include <string>
#include <vector>

namespace Chtholly {
    namespace CJMOD {
        
        // 配置结构体
        struct PrintMyloveConfig {
            std::string url;                    // 图片URL或路径
            std::string mode;                   // 模式: ASCII, UNICODE, EMOJI, CUSTOM
            int width;                          // 输出宽度
            int height;                         // 输出高度
            double scale;                       // 缩放比例
            double contrast;                    // 对比度 (0.0 - 2.0)
            double brightness;                  // 亮度 (0.0 - 2.0)
            bool invertColors;                  // 是否反转颜色
            bool enableDithering;               // 是否启用抖动
            bool enableEdgeDetection;           // 是否启用边缘检测
            std::string colorMode;              // 颜色模式: GRAYSCALE, RGB, ANSI
            std::string customCharSet;          // 自定义字符集
            
            // 默认构造函数
            PrintMyloveConfig() :
                url(""),
                mode("ASCII"),
                width(80),
                height(40),
                scale(1.0),
                contrast(1.0),
                brightness(1.0),
                invertColors(false),
                enableDithering(false),
                enableEdgeDetection(false),
                colorMode("GRAYSCALE"),
                customCharSet("")
            {}
        };
        
        // 主要的printMylove类
        class PrintMylove {
        private:
            std::string mode;
            std::string imagePath;
            int width, height;
            double scale, contrast, brightness;
            bool invertColors, enableDithering, enableEdgeDetection;
            std::string colorMode;
            std::string customCharSet;
            
            // 图像数据存储（灰度值）
            std::vector<std::vector<unsigned char>> imageData;
            
            // 私有方法
            char pixelToChar(unsigned char pixel);
            void applyDithering();
            void applyEdgeDetection();
            
        public:
            // 构造函数和析构函数
            PrintMylove();
            ~PrintMylove();
            
            // 配置方法
            void setConfig(const PrintMyloveConfig& config);
            
            // 图像加载和处理
            bool loadImage(const std::string& imagePath);
            std::string convert();
            
            // 静态方法 - 便捷接口
            static std::string createAsciiArt(const PrintMyloveConfig& config);
            static std::vector<std::string> batchConvert(const std::vector<PrintMyloveConfig>& configs);
            static bool saveToFile(const std::string& asciiArt, const std::string& filePath);
            
            // 工具方法
            static std::vector<std::string> getSupportedModes();
            static std::string getPresetCharSet(const std::string& mode);
        };
        
    } // namespace CJMOD
} // namespace Chtholly

// C风格的导出函数声明（用于CHTL JS集成）
extern "C" {
    const char* chtl_printMylove(const char* url, const char* mode, int width, int height, double scale);
    const char* chtl_printMylove_modes();
}

#endif // CHTHOLLY_CJMOD_PRINTMYLOVE_H