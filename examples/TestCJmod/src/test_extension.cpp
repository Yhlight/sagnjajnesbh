#include "test_extension.h"
#include <iostream>
#include <algorithm>
#include <functional>

// CHTL JS接口实现
extern "C" {
    bool chtl_js_init() {
        std::cout << "TestCJmod: Initializing CHTL JS extension..." << std::endl;
        CHTLJSExtension::registerExtensions();
        return true;
    }
    
    void chtl_js_cleanup() {
        std::cout << "TestCJmod: Cleaning up CHTL JS extension..." << std::endl;
    }
    
    void chtl_js_register_functions() {
        std::cout << "TestCJmod: Registering extension functions..." << std::endl;
        // 这里应该注册函数到CHTL JS运行时
    }
    
    std::string test_function(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return "PROCESSED: " + result;
    }
    
    int calculate_hash(const std::string& data) {
        std::hash<std::string> hasher;
        return static_cast<int>(hasher(data) % 10000);
    }
    
    void log_message(const std::string& message) {
        std::cout << "[TestCJmod] " << message << std::endl;
    }
}

// CHTLJSExtension类实现
void CHTLJSExtension::registerExtensions() {
    // 注册扩展函数到CHTL JS运行时
    // 这里应该调用CHTL JS编译器的API
}

std::string CHTLJSExtension::processTemplate(const std::string& template_str) {
    // 处理模板字符串
    return "Processed: " + template_str;
}

bool CHTLJSExtension::validateSyntax(const std::string& code) {
    // 简单的语法验证
    return !code.empty() && code.find("error") == std::string::npos;
}