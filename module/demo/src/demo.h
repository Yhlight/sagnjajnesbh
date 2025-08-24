#pragma once

#include "CJMOD/ProperCJMODApi.h"
#include <string>
#include <memory>
#include <functional>

/**
 * @brief Demo CJMOD模块 - 展示CHTL与CJMOD的完整集成
 * 
 * 核心理念：
 * 1. 使用CHTL优秀架构设计
 * 2. 与统一扫描器完美协作
 * 3. 展示正确的CJMOD实现方式
 */

namespace CHTL {
namespace CJMOD {
namespace Demo {

// ============ 全局注册表（语法文档1425行：允许全局变量管理状态） ============
extern std::unordered_map<std::string, std::string> g_demoFunctionRegistry;
extern std::unordered_map<std::string, std::string> g_animationRegistry;

/**
 * @brief printMylove扩展处理器
 * 演示如何正确实现CJMOD扩展
 */
class PrintMyloveProcessor : public CHTLJSFunction {
public:
    PrintMyloveProcessor();
    
    // 处理来自统一扫描器的CodeFragment
    std::string processFragment(const Scanner::CodeFragment& fragment) override;
    
    // 注册到统一扫描器
    void registerToScanner(Scanner::CHTLUnifiedScanner& scanner);
    
private:
    void setupParameterProcessors();
    std::string generateImageProcessingJS(const std::unordered_map<std::string, std::string>& params);
};

/**
 * @brief iNeverAway扩展处理器
 * 演示状态管理和标记函数的CJMOD实现
 */
class INeverAwayProcessor : public CHTLJSFunction {
public:
    INeverAwayProcessor();
    
    // 处理来自统一扫描器的CodeFragment
    std::string processFragment(const Scanner::CodeFragment& fragment) override;
    
    // 注册到统一扫描器
    void registerToScanner(Scanner::CHTLUnifiedScanner& scanner);
    
private:
    void setupStateProcessors();
    std::string generateStateManagementJS(const std::unordered_map<std::string, std::string>& stateMap);
};

/**
 * @brief createAnimation扩展处理器
 * 演示自定义CJMOD扩展的实现
 */
class CreateAnimationProcessor : public CHTLJSFunction {
public:
    CreateAnimationProcessor();
    
    // 处理来自统一扫描器的CodeFragment
    std::string processFragment(const Scanner::CodeFragment& fragment) override;
    
    // 注册到统一扫描器
    void registerToScanner(Scanner::CHTLUnifiedScanner& scanner);
    
private:
    void setupAnimationProcessors();
    std::string generateAnimationJS(const std::unordered_map<std::string, std::string>& animConfig);
};

/**
 * @brief Demo模块的统一处理器
 * 展示如何集成多个CJMOD扩展
 */
class DemoModuleProcessor {
public:
    DemoModuleProcessor();
    
    // 初始化所有扩展处理器
    void initialize();
    
    // 注册所有扩展到统一扫描器
    void registerAllExtensions(Scanner::CHTLUnifiedScanner& scanner);
    
    // 处理特定类型的CodeFragment
    std::string processCodeFragment(const Scanner::CodeFragment& fragment, const std::string& functionName);
    
    // 获取支持的扩展列表
    std::vector<std::string> getSupportedExtensions() const;
    
private:
    std::unique_ptr<PrintMyloveProcessor> printMyloveProcessor_;
    std::unique_ptr<INeverAwayProcessor> iNeverAwayProcessor_;
    std::unique_ptr<CreateAnimationProcessor> createAnimationProcessor_;
    
    std::unordered_map<std::string, std::function<std::string(const Scanner::CodeFragment&)>> extensionHandlers_;
    
    void setupExtensionHandlers();
};

// ============ 便利函数 - 简化CJMOD扩展的使用 ============

/**
 * @brief 创建Demo模块处理器
 */
std::unique_ptr<DemoModuleProcessor> createDemoProcessor();

/**
 * @brief 处理printMylove语法
 * @param fragment 来自统一扫描器的代码片段
 * @return 生成的JavaScript代码
 */
std::string processPrintMylove(const Scanner::CodeFragment& fragment);

/**
 * @brief 处理iNeverAway语法
 * @param fragment 来自统一扫描器的代码片段
 * @return 生成的JavaScript代码
 */
std::string processINeverAway(const Scanner::CodeFragment& fragment);

/**
 * @brief 处理createAnimation语法
 * @param fragment 来自统一扫描器的代码片段
 * @return 生成的JavaScript代码
 */
std::string processCreateAnimation(const Scanner::CodeFragment& fragment);

/**
 * @brief 注册Demo模块的所有扩展
 * @param scanner 统一扫描器实例
 */
void registerDemoExtensions(Scanner::CHTLUnifiedScanner& scanner);

/**
 * @brief 演示CHTL架构优势
 * 展示统一扫描器与CJMOD API的完美协作
 */
void demonstrateArchitectureAdvantages();

} // namespace Demo
} // namespace CJMOD
} // namespace CHTL

// ============ C接口 - 供CHTL编译器调用 ============
extern "C" {
    /**
     * @brief 初始化Demo CJMOD模块
     */
    void InitializeDemoCJMOD();
    
    /**
     * @brief 处理CJMOD函数调用
     * @param functionName 函数名
     * @param fragmentData 代码片段数据
     * @param fragmentSize 代码片段大小
     * @return 生成的JavaScript代码
     */
    const char* ProcessDemoCJMODFunction(const char* functionName, const char* fragmentData, size_t fragmentSize);
    
    /**
     * @brief 清理Demo CJMOD模块
     */
    void CleanupDemoCJMOD();
    
    /**
     * @brief 获取支持的扩展列表
     * @return 以逗号分隔的扩展名字符串
     */
    const char* GetDemoSupportedExtensions();
}