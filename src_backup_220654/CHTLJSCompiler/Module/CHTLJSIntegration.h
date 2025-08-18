#pragma once
#include "CJMODFramework.h"
#include <string>
#include <memory>

namespace chtl {
namespace cjmod {

/**
 * CHTL JS 集成接口
 * 让CJMOD模块与CHTL JS编译器无缝集成
 */
class CHTLJSIntegration {
public:
    static CHTLJSIntegration& getInstance();
    
    // 初始化集成
    bool initialize();
    void cleanup();
    bool isInitialized() const { return initialized_; }
    
    // CHTL JS编译器接口
    std::string processCJMODCall(const std::string& module_name, 
                                const std::string& function_name, 
                                const std::string& args_json);
    
    std::string generateCJMODPrelude();  // 生成前导JavaScript代码
    std::string generateCJMODEpilogue(); // 生成结尾JavaScript代码
    
    // 模块管理
    bool loadModule(const std::string& module_path);
    bool unloadModule(const std::string& module_name);
    std::vector<std::string> getLoadedModules() const;
    
    // 代码生成
    std::string generateIntegratedJavaScript();
    std::string generateModuleBindings();
    
    // 调试和诊断
    void enableDebugMode(bool enable) { debug_mode_ = enable; }
    bool isDebugMode() const { return debug_mode_; }
    void printIntegrationInfo() const;
    
    // 错误处理
    bool hasError() const { return !last_error_.empty(); }
    std::string getLastError() const { return last_error_; }
    void clearError() { last_error_.clear(); }
    
    // 统计信息
    struct IntegrationStats {
        size_t loaded_modules;
        size_t total_functions;
        size_t successful_calls;
        size_t failed_calls;
        double average_call_time_ms;
        
        IntegrationStats() : loaded_modules(0), total_functions(0), 
                           successful_calls(0), failed_calls(0), average_call_time_ms(0.0) {}
    };
    
    IntegrationStats getStats() const { return stats_; }
    void resetStats() { stats_ = IntegrationStats(); }
    
private:
    CHTLJSIntegration() : initialized_(false), debug_mode_(false) {}
    
    bool initialized_;
    bool debug_mode_;
    std::string last_error_;
    IntegrationStats stats_;
    
    static std::unique_ptr<CHTLJSIntegration> instance_;
    
    // 内部方法
    JSValue parseJSONToJSValue(const std::string& json);
    std::string jsValueToJSON(const JSValue& value);
    void updateCallStats(bool success, double time_ms);
    void logDebug(const std::string& message) const;
};

/**
 * C接口 - 供CHTL JS编译器直接调用
 */
extern "C" {
    // 初始化
    int cjmod_initialize();
    void cjmod_cleanup();
    
    // 函数调用
    const char* cjmod_call_function(const char* module_name, 
                                   const char* function_name, 
                                   const char* args_json);
    
    // 代码生成
    const char* cjmod_generate_prelude();
    const char* cjmod_generate_epilogue();
    const char* cjmod_generate_bindings();
    
    // 模块管理
    int cjmod_load_module(const char* module_path);
    int cjmod_unload_module(const char* module_name);
    const char* cjmod_list_modules();
    
    // 错误处理
    int cjmod_has_error();
    const char* cjmod_get_error();
    void cjmod_clear_error();
    
    // 调试
    void cjmod_set_debug_mode(int enable);
    void cjmod_print_info();
}

/**
 * CHTL JS 语法扩展 - 让CJMOD调用更自然
 */
namespace syntax {
    
    /**
     * 解析CHTL JS中的CJMOD调用语法
     * 
     * 支持的语法：
     * 1. 直接调用：ModuleName.functionName(args)
     * 2. 异步调用：await ModuleName.functionName(args)
     * 3. 错误处理：try { ModuleName.func() } catch(e) { }
     */
    class CJMODSyntaxParser {
    public:
        struct ParsedCall {
            std::string module_name;
            std::string function_name;
            std::vector<std::string> arguments;
            bool is_async;
            bool has_error_handling;
            
            ParsedCall() : is_async(false), has_error_handling(false) {}
        };
        
        static bool isCJMODCall(const std::string& code);
        static ParsedCall parseCall(const std::string& code);
        static std::string generateJavaScriptCall(const ParsedCall& call);
        
    private:
        static std::string extractModuleName(const std::string& code);
        static std::string extractFunctionName(const std::string& code);
        static std::vector<std::string> extractArguments(const std::string& code);
    };
    
} // namespace syntax

/**
 * 开发者友好的辅助宏
 */

// 快速创建CJMOD模块
#define CHTL_CJMOD_MODULE(name) \
    static void register_##name##_module() { \
        auto module = std::make_unique<chtl::cjmod::CJMODModule>(#name);

// 快速注册函数
#define CHTL_FUNCTION(name, description) \
    module->addFunction(#name, [](const chtl::cjmod::JSContext& ctx) -> chtl::cjmod::JSValue

// 结束模块定义
#define CHTL_CJMOD_END(name) \
        chtl::cjmod::CJMODRegistry::getInstance().registerModule(std::move(module)); \
    } \
    static bool name##_auto_registered = []() { \
        register_##name##_module(); \
        return true; \
    }();

// 参数验证宏
#define CHTL_REQUIRE_ARGS(count) \
    if (!chtl::cjmod::helpers::validateArgCount(ctx, count)) { \
        return chtl::cjmod::helpers::makeError("需要 " #count " 个参数"); \
    }

#define CHTL_REQUIRE_ARGS_RANGE(min, max) \
    if (!chtl::cjmod::helpers::validateArgCount(ctx, min, max)) { \
        return chtl::cjmod::helpers::makeError("需要 " #min " 到 " #max " 个参数"); \
    }

// 类型验证宏
#define CHTL_REQUIRE_STRING(index) \
    if (!chtl::cjmod::helpers::validateArgType(ctx, index, chtl::cjmod::JSValue::STRING)) { \
        return chtl::cjmod::helpers::makeError("参数 " #index " 必须是字符串"); \
    }

#define CHTL_REQUIRE_NUMBER(index) \
    if (!chtl::cjmod::helpers::validateArgType(ctx, index, chtl::cjmod::JSValue::NUMBER)) { \
        return chtl::cjmod::helpers::makeError("参数 " #index " 必须是数字"); \
    }

} // namespace cjmod
} // namespace chtl