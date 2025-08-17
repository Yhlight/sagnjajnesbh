#pragma once

#include <string>

extern "C" {
    // CHTL JS接口函数
    bool chtl_js_init();
    void chtl_js_cleanup();
    void chtl_js_register_functions();
    
    // 示例扩展函数
    std::string test_function(const std::string& input);
    int calculate_hash(const std::string& data);
    void log_message(const std::string& message);
}

// CHTL JS扩展类
class CHTLJSExtension {
public:
    static void registerExtensions();
    static std::string processTemplate(const std::string& template_str);
    static bool validateSyntax(const std::string& code);
};