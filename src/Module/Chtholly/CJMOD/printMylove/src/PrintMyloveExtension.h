#pragma once

#include "CJMOD/TrueCJMODApi.h"
#include <string>
#include <unordered_map>

// 全局变量注册表，用于状态管理（语法文档第1425行）
namespace Chtholly {
namespace CJMOD {

// printMylove全局配置注册表
extern std::unordered_map<std::string, std::string> g_printMyloveConfig;

// iNeverAway全局事件注册表
extern std::unordered_map<std::string, std::string> g_iNeverAwayRegistry;

/**
 * @brief printMylove CJMOD扩展实现
 * 严格按照语法文档第1472-1483行实现
 * 使用TrueCJMODApi完整工作流程，支持无序、可选、无修饰字面量
 */
class PrintMyloveExtension {
public:
    // 标准流程：支持无序、可选、无修饰字面量
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

/**
 * @brief iNeverAway CJMOD扩展实现
 * 严格按照语法文档实现，支持：
 * - 自定义键名（任意合法命名）
 * - 状态可选（Void<Init>、Handle<UserAction>等）
 * - 无序参数
 * - 可选参数
 * - 无修饰字面量
 * - 可变参数（...）处理动态键值对
 */
class INeverAwayExtension {
public:
    // 标准流程：使用可变参数处理动态键值对
    static std::string ProcessINeverAway(const std::string& chtlJsCode);
    
    // 简化流程：自动化处理
    static std::string CreateINeverAwayFunction(const std::string& chtlJsCode);
    
    // 初始化扩展
    static void Initialize();
    
    // 获取扩展信息
    static std::string GetName() { return "iNeverAway"; }
    static std::string GetVersion() { return "1.0.0"; }
    
private:
    // 内部处理方法
    static void InitializeEventRegistry();
    static std::string ProcessCustomKeys(const std::vector<std::string>& keyValuePairs);
    static std::string GenerateEventHandlerJS(const std::string& keyName, const std::string& stateInfo, const std::string& functionBody);
    static std::string ParseStateInfo(const std::string& keyName); // 解析Void<Init>、Handle<UserAction>等
    
    // 使用TrueCJMODApi的标准工作流程，支持可变参数
    static std::string ExecuteVariadicFlow(const std::string& pattern, const std::string& chtlJsCode);
};

// CJMOD扩展注册函数（C接口，供CHTL编译器调用）
extern "C" {
    void RegisterPrintMyloveExtension();
    const char* ProcessPrintMyloveCall(const char* chtlJsCode);
    void CleanupPrintMyloveExtension();
    
    void RegisterINeverAwayExtension();
    const char* ProcessINeverAwayCall(const char* chtlJsCode);
    void CleanupINeverAwayExtension();
}

} // namespace CJMOD
} // namespace Chtholly