#pragma once

#include "CJMOD/TrueCJMODApi.h"
#include <string>
#include <unordered_map>

// 全局变量注册表，用于状态管理（语法文档第1425行）
namespace Chtholly {
namespace CJMOD {

// printMylove全局配置注册表
extern std::unordered_map<std::string, std::string> g_printMyloveConfig;

/**
 * @brief printMylove CJMOD扩展实现
 * 严格按照语法文档第1472-1483行实现
 * 使用TrueCJMODApi完整工作流程
 */
class PrintMyloveExtension {
public:
    // 标准流程：使用TrueCJMODApi的完整工作流程
    static std::string ProcessPrintMylove(const std::string& chtlJsCode);
    
    // 简化流程：使用createCHTLJSFunction自动化处理
    static std::string CreatePrintMyloveFunction(const std::string& chtlJsCode);
    
    // 初始化扩展
    static void Initialize();
    
    // 获取扩展信息
    static std::string GetName() { return "printMylove"; }
    static std::string GetVersion() { return "1.0.0"; }
    
private:
    // 内部处理方法
    static void InitializeDefaultConfig();
    static std::string ProcessImageToAscii(const std::string& imageUrl, int width, int height);
    static std::string ProcessImageToPixel(const std::string& imageUrl, double scale);
    static std::string OptimizeOutputCode(const std::string& jsCode);
    
    // 使用TrueCJMODApi的标准工作流程
    static std::string ExecuteStandardFlow(const std::string& pattern, const std::string& chtlJsCode);
};

// CJMOD扩展注册函数（C接口，供CHTL编译器调用）
extern "C" {
    void RegisterPrintMyloveExtension();
    const char* ProcessPrintMyloveCall(const char* chtlJsCode);
    void CleanupPrintMyloveExtension();
}

} // namespace CJMOD
} // namespace Chtholly