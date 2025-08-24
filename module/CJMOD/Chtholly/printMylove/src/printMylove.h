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
 * 使用TrueCJMODApi严格实现
 */
class PrintMyloveExtension {
public:
    // 使用TrueCJMODApi的标准流程
    static std::string ProcessPrintMylove(const std::string& chtlJsCode);
    
    // 使用简化流程
    static std::string CreatePrintMyloveFunction(const std::string& chtlJsCode);
    
private:
    // 内部处理方法
    static void InitializeDefaultConfig();
    static std::string GenerateImageProcessingJS(const CHTL::CJMOD::Syntax& syntax);
    static std::string OptimizeOutputCode(const std::string& jsCode);
};

// CJMOD扩展注册函数
extern "C" {
    void RegisterPrintMyloveExtension();
    std::string ProcessCHTLJSCall(const char* functionName, const char* parameters);
}

} // namespace CJMOD
} // namespace Chtholly