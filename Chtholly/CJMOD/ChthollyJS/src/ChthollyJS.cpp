#include "CJMOD/UnifiedCJMODApi.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace CHTL::CJMOD;

// 全局Syntax管理器
static Syntax globalSyntax;

// printMylove图像转字符实现
class ImageToCharConverter {
private:
    std::vector<std::string> asciiChars = {" ", ".", ":", "-", "=", "+", "*", "#", "%", "@"};
    std::vector<std::string> pixelChars = {"░", "▒", "▓", "█"};

public:
    std::string convertToASCII(const std::string& imagePath, int width, int height, double scale) {
        // 简化实现：模拟图像转ASCII
        std::string result = "// ASCII Art generated from: " + imagePath + "\n";
        
        int actualWidth = static_cast<int>(width * scale);
        int actualHeight = static_cast<int>(height * scale);
        
        for (int y = 0; y < actualHeight; ++y) {
            for (int x = 0; x < actualWidth; ++x) {
                // 模拟像素强度
                double intensity = (double)(x + y) / (actualWidth + actualHeight);
                int charIndex = static_cast<int>(intensity * (asciiChars.size() - 1));
                result += asciiChars[charIndex];
            }
            result += "\n";
        }
        
        return result;
    }
    
    std::string convertToPixel(const std::string& imagePath, int width, int height, double scale) {
        // 简化实现：模拟图像转像素块
        std::string result = "// Pixel Art generated from: " + imagePath + "\n";
        
        int actualWidth = static_cast<int>(width * scale);
        int actualHeight = static_cast<int>(height * scale);
        
        for (int y = 0; y < actualHeight; ++y) {
            for (int x = 0; x < actualWidth; ++x) {
                // 模拟像素强度
                double intensity = (double)(x + y) / (actualWidth + actualHeight);
                int charIndex = static_cast<int>(intensity * (pixelChars.size() - 1));
                result += pixelChars[charIndex];
            }
            result += "\n";
        }
        
        return result;
    }
};

// printMylove实现
extern "C" {
    // 标准流程实现
    void registerPrintMylove() {
        // 使用Syntax类统一管理
        
        // 创建printMylove函数
        CHTLJSFunction printMyloveFunc("printMylove");
        printMyloveFunc.AddParam("url", "\"\"");
        printMyloveFunc.AddParam("mode", "\"ASCII\"");
        printMyloveFunc.AddParam("width", "80");
        printMyloveFunc.AddParam("height", "24");
        printMyloveFunc.AddParam("scale", "1.0");
        
        // 设置函数体模板
        printMyloveFunc.SetBodyTemplate(R"(
            const converter = new ImageToCharConverter();
            const imagePath = arguments[0].url || "";
            const mode = arguments[0].mode || "ASCII";
            const width = parseInt(arguments[0].width || "80");
            const height = parseInt(arguments[0].height || "24");
            const scale = parseFloat(arguments[0].scale || "1.0");
            
            if (mode === "ASCII") {
                return converter.convertToASCII(imagePath, width, height, scale);
            } else if (mode === "Pixel") {
                return converter.convertToPixel(imagePath, width, height, scale);
            } else {
                return "// 不支持的模式: " + mode;
            }
        )");
        
        // 启用vir支持
        printMyloveFunc.EnableVirSupport(true);
        
        // 注册到全局语法管理器
        globalSyntax.RegisterFunction(printMyloveFunc);
        
        // 绑定vir支持
        globalSyntax.BindVirFunction("printMylove");
        
        std::cout << "[ChthollyJS] printMylove函数已注册，支持vir虚对象" << std::endl;
    }
    
    // 简化流程实现（使用createCHTLJSFunction）
    void registerPrintMyloveSimplified() {
        // 使用便捷函数创建
        auto func = CreateCHTLJSFunction("printMylove", {"url", "mode", "width", "height", "scale"});
        
        // 自定义转换规则
        globalSyntax.SetTransformRule([](const std::unordered_map<std::string, Arg>& args) -> std::string {
            std::string url = "\"\"";
            std::string mode = "\"ASCII\"";
            std::string width = "80";
            std::string height = "24";
            std::string scale = "1.0";
            
            // 提取参数值
            if (args.find("url") != args.end()) {
                url = "\"" + args.at("url").ToString() + "\"";
            }
            if (args.find("mode") != args.end()) {
                mode = "\"" + args.at("mode").ToString() + "\"";
            }
            if (args.find("width") != args.end()) {
                width = args.at("width").ToString();
            }
            if (args.find("height") != args.end()) {
                height = args.at("height").ToString();
            }
            if (args.find("scale") != args.end()) {
                scale = args.at("scale").ToString();
            }
            
            return "printMylove({url: " + url + ", mode: " + mode + 
                   ", width: " + width + ", height: " + height + ", scale: " + scale + "})";
        });
        
        globalSyntax.RegisterFunction(func);
        
        std::cout << "[ChthollyJS] printMylove函数已注册（简化流程）" << std::endl;
    }
    
    // iNeverAway实现 - 支持自定义键的标记函数组
    void registerINeverAway() {
        // iNeverAway与其他CHTL JS功能不同，它允许开发者定义键
        CHTLJSFunction iNeverAwayFunc("iNeverAway");
        
        // iNeverAway不预定义键名，而是支持任意键名
        // 这里我们只设置基础结构
        iNeverAwayFunc.SetBodyTemplate(R"(
            // iNeverAway: 创建一组标记函数，支持自定义键
            const functionGroup = {};
            const args = arguments[0] || {};
            
            // 遍历所有提供的键
            for (const [key, value] of Object.entries(args)) {
                if (typeof value === 'function') {
                    // 如果是函数，直接注册
                    functionGroup[key] = value;
                } else if (typeof value === 'object' && value !== null) {
                    // 如果是对象，创建对应的函数引用
                    functionGroup[key] = function() { 
                        return value; 
                    };
                }
                
                // 注册到全局函数表，用于vir对象引用
                window['__chtl_' + key] = functionGroup[key];
            }
            
            return functionGroup;
        )");
        
        // iNeverAway必须支持vir
        iNeverAwayFunc.EnableVirSupport(true);
        
        globalSyntax.RegisterFunction(iNeverAwayFunc);
        globalSyntax.BindVirFunction("iNeverAway");
        
        std::cout << "[ChthollyJS] iNeverAway函数已注册，支持自定义键和vir虚对象" << std::endl;
    }
    
    // 处理CHTL JS语法的主要入口
    const char* processCHTLJS(const char* source) {
        static std::string result;
        
        if (!source) {
            return "";
        }
        
        std::string sourceStr(source);
        
        // 使用Syntax类的扫描和处理功能
        result = globalSyntax.ScanAndProcess(sourceStr);
        
        return result.c_str();
    }
    
    // 设置扫描策略
    void setScanStrategy(int strategy) {
        ScanStrategy scanStrategy = (strategy == 1) ? 
            ScanStrategy::SLIDING_WINDOW : ScanStrategy::FRONT_EXTRACT;
        
        globalSyntax.SetScanStrategy(scanStrategy);
        
        std::cout << "[ChthollyJS] 扫描策略已设置为: " << 
            (strategy == 1 ? "SLIDING_WINDOW" : "FRONT_EXTRACT") << std::endl;
    }
    
    // virFunctionBind实现
    void virFunctionBind(const char* functionName) {
        if (!functionName) return;
        
        std::string funcName(functionName);
        globalSyntax.BindVirFunction(funcName);
        
        std::cout << "[ChthollyJS] 函数已绑定到vir支持: " << funcName << std::endl;
    }
    
    // 模块初始化
    void initializeChthollyJS() {
        std::cout << "[ChthollyJS] 珂朵莉模块初始化..." << std::endl;
        
        // 设置默认扫描策略
        globalSyntax.SetScanStrategy(ScanStrategy::SLIDING_WINDOW);
        
        // 注册所有函数
        registerPrintMylove();
        registerINeverAway();
        
        std::cout << "[ChthollyJS] 珂朵莉模块初始化完成" << std::endl;
        std::cout << "[ChthollyJS] 已注册函数: ";
        
        auto functions = globalSyntax.GetRegisteredFunctions();
        for (size_t i = 0; i < functions.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << functions[i];
        }
        std::cout << std::endl;
    }
    
    // 获取支持vir的函数列表
    const char* getVirSupportedFunctions() {
        static std::string result;
        result.clear();
        
        auto functions = globalSyntax.GetRegisteredFunctions();
        for (const auto& func : functions) {
            if (globalSyntax.IsVirSupported(func)) {
                if (!result.empty()) result += ",";
                result += func;
            }
        }
        
        return result.c_str();
    }
}