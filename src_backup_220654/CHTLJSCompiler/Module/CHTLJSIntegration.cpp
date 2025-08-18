#include "CHTLJSIntegration.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <regex>

namespace chtl {
namespace cjmod {

std::unique_ptr<CHTLJSIntegration> CHTLJSIntegration::instance_ = nullptr;

CHTLJSIntegration& CHTLJSIntegration::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<CHTLJSIntegration>(new CHTLJSIntegration());
    }
    return *instance_;
}

bool CHTLJSIntegration::initialize() {
    if (initialized_) {
        return true;
    }
    
    try {
        // 初始化CJMOD注册表
        auto& registry = CJMODRegistry::getInstance();
        
        logDebug("CJMOD集成系统初始化中...");
        
        initialized_ = true;
        clearError();
        
        logDebug("CJMOD集成系统初始化成功");
        return true;
        
    } catch (const std::exception& e) {
        last_error_ = "初始化失败: " + std::string(e.what());
        return false;
    }
}

void CHTLJSIntegration::cleanup() {
    if (!initialized_) {
        return;
    }
    
    initialized_ = false;
    clearError();
    resetStats();
    
    logDebug("CJMOD集成系统已清理");
}

std::string CHTLJSIntegration::processCJMODCall(const std::string& module_name, 
                                               const std::string& function_name, 
                                               const std::string& args_json) {
    if (!initialized_) {
        return jsValueToJSON(helpers::makeError("CJMOD系统未初始化"));
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // 解析参数
        JSValue args_value = parseJSONToJSValue(args_json);
        std::vector<JSValue> args_array;
        
        if (args_value.isArray()) {
            args_array = args_value.toArray();
        } else if (!args_value.isUndefined()) {
            args_array.push_back(args_value);
        }
        
        // 调用函数
        auto& registry = CJMODRegistry::getInstance();
        JSValue result = registry.callFunction(module_name, function_name, args_array);
        
        // 更新统计
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        updateCallStats(true, duration.count() / 1000.0);
        
        return jsValueToJSON(result);
        
    } catch (const std::exception& e) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        updateCallStats(false, duration.count() / 1000.0);
        
        last_error_ = "调用失败: " + std::string(e.what());
        return jsValueToJSON(helpers::makeError(e.what()));
    }
}

std::string CHTLJSIntegration::generateCJMODPrelude() {
    std::ostringstream prelude;
    
    prelude << "// CHTL CJMOD Integration Prelude\n";
    prelude << "// 自动生成的CJMOD支持代码\n\n";
    
    prelude << "(function() {\n";
    prelude << "    'use strict';\n\n";
    
    // CJMOD运行时支持
    prelude << "    // CJMOD运行时系统\n";
    prelude << "    if (typeof window.__CJMOD === 'undefined') {\n";
    prelude << "        window.__CJMOD = {\n";
    prelude << "            call: function(moduleName, functionName, args) {\n";
    prelude << "                // 这个函数由CHTL JS编译器实现\n";
    prelude << "                console.log('[CJMOD] Calling:', moduleName + '.' + functionName, args);\n";
    prelude << "                return undefined;\n";
    prelude << "            },\n\n";
    
    prelude << "            callAsync: async function(moduleName, functionName, args) {\n";
    prelude << "                return new Promise((resolve, reject) => {\n";
    prelude << "                    try {\n";
    prelude << "                        const result = this.call(moduleName, functionName, args);\n";
    prelude << "                        resolve(result);\n";
    prelude << "                    } catch (error) {\n";
    prelude << "                        reject(error);\n";
    prelude << "                    }\n";
    prelude << "                });\n";
    prelude << "            },\n\n";
    
    prelude << "            handleError: function(error, context) {\n";
    prelude << "                console.error('[CJMOD Error]', context, error);\n";
    prelude << "                return { error: true, message: error.message || error };\n";
    prelude << "            }\n";
    prelude << "        };\n";
    prelude << "    }\n\n";
    
    // 创建CJMOD命名空间
    prelude << "    // 创建CJMOD模块命名空间\n";
    prelude << "    if (typeof window.CJMOD === 'undefined') {\n";
    prelude << "        window.CJMOD = {};\n";
    prelude << "    }\n\n";
    
    prelude << "})();\n\n";
    
    return prelude.str();
}

std::string CHTLJSIntegration::generateCJMODEpilogue() {
    std::ostringstream epilogue;
    
    epilogue << "// CHTL CJMOD Integration Epilogue\n";
    epilogue << "// CJMOD系统就绪\n\n";
    
    epilogue << "(function() {\n";
    epilogue << "    'use strict';\n\n";
    
    epilogue << "    // 初始化完成通知\n";
    epilogue << "    if (typeof window.__CJMOD !== 'undefined') {\n";
    epilogue << "        console.log('[CJMOD] 系统已就绪，已加载模块:', Object.keys(window.CJMOD));\n";
    epilogue << "        \n";
    epilogue << "        // 触发就绪事件\n";
    epilogue << "        if (typeof window.dispatchEvent === 'function') {\n";
    epilogue << "            window.dispatchEvent(new CustomEvent('cjmod:ready', {\n";
    epilogue << "                detail: { modules: Object.keys(window.CJMOD) }\n";
    epilogue << "            }));\n";
    epilogue << "        }\n";
    epilogue << "    }\n\n";
    
    epilogue << "})();\n";
    
    return epilogue.str();
}

std::string CHTLJSIntegration::generateModuleBindings() {
    auto& registry = CJMODRegistry::getInstance();
    
    std::ostringstream bindings;
    
    bindings << "// CJMOD模块绑定\n";
    bindings << "// 为每个已注册模块生成JavaScript绑定\n\n";
    
    auto module_names = registry.getModuleNames();
    for (const auto& module_name : module_names) {
        auto module = registry.getModule(module_name);
        if (!module) continue;
        
        bindings << "(function() {\n";
        bindings << "    // 模块: " << module_name << "\n";
        bindings << "    if (typeof window.CJMOD['" << module_name << "'] === 'undefined') {\n";
        bindings << "        window.CJMOD['" << module_name << "'] = {};\n";
        bindings << "    }\n\n";
        
        auto function_names = module->getFunctionNames();
        for (const auto& func_name : function_names) {
            bindings << "    // 函数: " << func_name << "\n";
            bindings << "    window.CJMOD['" << module_name << "']['" << func_name << "'] = function() {\n";
            bindings << "        const args = Array.from(arguments);\n";
            bindings << "        try {\n";
            bindings << "            return window.__CJMOD.call('" << module_name << "', '" << func_name << "', args);\n";
            bindings << "        } catch (error) {\n";
            bindings << "            return window.__CJMOD.handleError(error, '" << module_name << "." << func_name << "');\n";
            bindings << "        }\n";
            bindings << "    };\n\n";
            
            // 异步版本
            bindings << "    // 异步版本: " << func_name << "Async\n";
            bindings << "    window.CJMOD['" << module_name << "']['" << func_name << "Async'] = async function() {\n";
            bindings << "        const args = Array.from(arguments);\n";
            bindings << "        try {\n";
            bindings << "            return await window.__CJMOD.callAsync('" << module_name << "', '" << func_name << "', args);\n";
            bindings << "        } catch (error) {\n";
            bindings << "            return window.__CJMOD.handleError(error, '" << module_name << "." << func_name << "Async');\n";
            bindings << "        }\n";
            bindings << "    };\n\n";
        }
        
        bindings << "})();\n\n";
    }
    
    return bindings.str();
}

std::string CHTLJSIntegration::generateIntegratedJavaScript() {
    std::ostringstream integrated;
    
    integrated << generateCJMODPrelude();
    integrated << generateModuleBindings();
    integrated << generateCJMODEpilogue();
    
    return integrated.str();
}

std::vector<std::string> CHTLJSIntegration::getLoadedModules() const {
    auto& registry = CJMODRegistry::getInstance();
    return registry.getModuleNames();
}

void CHTLJSIntegration::printIntegrationInfo() const {
    std::cout << "=== CHTL JS CJMOD 集成信息 ===" << std::endl;
    std::cout << "初始化状态: " << (initialized_ ? "已初始化" : "未初始化") << std::endl;
    std::cout << "调试模式: " << (debug_mode_ ? "启用" : "禁用") << std::endl;
    
    if (!last_error_.empty()) {
        std::cout << "最后错误: " << last_error_ << std::endl;
    }
    
    std::cout << std::endl << "统计信息:" << std::endl;
    std::cout << "  已加载模块数: " << stats_.loaded_modules << std::endl;
    std::cout << "  总函数数: " << stats_.total_functions << std::endl;
    std::cout << "  成功调用数: " << stats_.successful_calls << std::endl;
    std::cout << "  失败调用数: " << stats_.failed_calls << std::endl;
    std::cout << "  平均调用时间: " << stats_.average_call_time_ms << "ms" << std::endl;
    
    // 显示已加载的模块
    auto modules = getLoadedModules();
    if (!modules.empty()) {
        std::cout << std::endl << "已加载的模块:" << std::endl;
        for (const auto& module_name : modules) {
            std::cout << "  - " << module_name << std::endl;
        }
    }
}

JSValue CHTLJSIntegration::parseJSONToJSValue(const std::string& json) {
    // 简化的JSON解析 - 在真实实现中应该使用专业的JSON库
    if (json.empty() || json == "undefined") {
        return JSValue();
    }
    
    if (json == "null") {
        return JSValue(); // null值
    }
    
    if (json == "true") {
        return JSValue(true);
    }
    
    if (json == "false") {
        return JSValue(false);
    }
    
    // 尝试解析为数字
    try {
        if (json.find('.') != std::string::npos) {
            return JSValue(std::stod(json));
        } else {
            return JSValue(std::stoi(json));
        }
    } catch (...) {
        // 不是数字，继续其他解析
    }
    
    // 字符串（去除引号）
    if (json.length() >= 2 && json.front() == '"' && json.back() == '"') {
        return JSValue(json.substr(1, json.length() - 2));
    }
    
    // 默认作为字符串处理
    return JSValue(json);
}

std::string CHTLJSIntegration::jsValueToJSON(const JSValue& value) {
    // 简化的JSON序列化
    switch (value.getType()) {
        case JSValue::UNDEFINED:
            return "undefined";
        case JSValue::NULL_VALUE:
            return "null";
        case JSValue::BOOLEAN:
            return value.toBool() ? "true" : "false";
        case JSValue::NUMBER:
            return std::to_string(value.toDouble());
        case JSValue::STRING:
            return "\"" + value.toString() + "\"";
        case JSValue::ARRAY: {
            std::ostringstream json;
            json << "[";
            auto arr = value.toArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) json << ",";
                json << jsValueToJSON(arr[i]);
            }
            json << "]";
            return json.str();
        }
        case JSValue::OBJECT: {
            std::ostringstream json;
            json << "{";
            auto obj = value.toObject();
            bool first = true;
            for (const auto& [key, val] : obj) {
                if (!first) json << ",";
                json << "\"" << key << "\":" << jsValueToJSON(val);
                first = false;
            }
            json << "}";
            return json.str();
        }
        default:
            return "null";
    }
}

void CHTLJSIntegration::updateCallStats(bool success, double time_ms) {
    if (success) {
        stats_.successful_calls++;
    } else {
        stats_.failed_calls++;
    }
    
    // 更新平均时间
    size_t total_calls = stats_.successful_calls + stats_.failed_calls;
    if (total_calls > 0) {
        stats_.average_call_time_ms = (stats_.average_call_time_ms * (total_calls - 1) + time_ms) / total_calls;
    }
}

void CHTLJSIntegration::logDebug(const std::string& message) const {
    if (debug_mode_) {
        std::cout << "[CJMOD Debug] " << message << std::endl;
    }
}

// 语法解析器实现
namespace syntax {

bool CJMODSyntaxParser::isCJMODCall(const std::string& code) {
    // 检查是否包含模块调用模式：ModuleName.functionName(
    std::regex pattern(R"(\b\w+\.\w+\s*\()");
    return std::regex_search(code, pattern);
}

CJMODSyntaxParser::ParsedCall CJMODSyntaxParser::parseCall(const std::string& code) {
    ParsedCall call;
    
    // 检查是否为异步调用
    call.is_async = code.find("await ") != std::string::npos;
    
    // 检查是否有错误处理
    call.has_error_handling = code.find("try") != std::string::npos || 
                             code.find("catch") != std::string::npos;
    
    // 提取模块名和函数名
    std::regex call_pattern(R"((\w+)\.(\w+)\s*\(([^)]*)\))");
    std::smatch match;
    
    if (std::regex_search(code, match, call_pattern)) {
        call.module_name = match[1].str();
        call.function_name = match[2].str();
        
        // 解析参数
        std::string args_str = match[3].str();
        if (!args_str.empty()) {
            // 简化的参数解析
            std::istringstream args_stream(args_str);
            std::string arg;
            while (std::getline(args_stream, arg, ',')) {
                // 去除空白
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                if (!arg.empty()) {
                    call.arguments.push_back(arg);
                }
            }
        }
    }
    
    return call;
}

std::string CJMODSyntaxParser::generateJavaScriptCall(const ParsedCall& call) {
    std::ostringstream js;
    
    if (call.is_async) {
        js << "await ";
    }
    
    js << "window.CJMOD['" << call.module_name << "']['" << call.function_name;
    
    if (call.is_async) {
        js << "Async";
    }
    
    js << "'](";
    
    for (size_t i = 0; i < call.arguments.size(); ++i) {
        if (i > 0) js << ", ";
        js << call.arguments[i];
    }
    
    js << ")";
    
    return js.str();
}

} // namespace syntax

} // namespace cjmod
} // namespace chtl

// C接口实现
extern "C" {

int cjmod_initialize() {
    return chtl::cjmod::CHTLJSIntegration::getInstance().initialize() ? 1 : 0;
}

void cjmod_cleanup() {
    chtl::cjmod::CHTLJSIntegration::getInstance().cleanup();
}

const char* cjmod_call_function(const char* module_name, const char* function_name, const char* args_json) {
    if (!module_name || !function_name || !args_json) {
        return nullptr;
    }
    
    static std::string result;
    result = chtl::cjmod::CHTLJSIntegration::getInstance().processCJMODCall(
        module_name, function_name, args_json);
    return result.c_str();
}

const char* cjmod_generate_prelude() {
    static std::string result;
    result = chtl::cjmod::CHTLJSIntegration::getInstance().generateCJMODPrelude();
    return result.c_str();
}

const char* cjmod_generate_epilogue() {
    static std::string result;
    result = chtl::cjmod::CHTLJSIntegration::getInstance().generateCJMODEpilogue();
    return result.c_str();
}

const char* cjmod_generate_bindings() {
    static std::string result;
    result = chtl::cjmod::CHTLJSIntegration::getInstance().generateModuleBindings();
    return result.c_str();
}

int cjmod_has_error() {
    return chtl::cjmod::CHTLJSIntegration::getInstance().hasError() ? 1 : 0;
}

const char* cjmod_get_error() {
    static std::string error;
    error = chtl::cjmod::CHTLJSIntegration::getInstance().getLastError();
    return error.c_str();
}

void cjmod_clear_error() {
    chtl::cjmod::CHTLJSIntegration::getInstance().clearError();
}

void cjmod_set_debug_mode(int enable) {
    chtl::cjmod::CHTLJSIntegration::getInstance().enableDebugMode(enable != 0);
}

void cjmod_print_info() {
    chtl::cjmod::CHTLJSIntegration::getInstance().printIntegrationInfo();
}

}