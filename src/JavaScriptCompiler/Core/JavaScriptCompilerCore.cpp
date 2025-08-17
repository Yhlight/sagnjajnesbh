#include "JavaScriptCompilerCore.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace chtl {

JavaScriptCompilerCore::JavaScriptCompilerCore() : debug_mode_(false) {
    initialize();
}

JavaScriptCompilerCore::~JavaScriptCompilerCore() {
    cleanup();
}

bool JavaScriptCompilerCore::initialize() {
    try {
        // TODO: 初始化ANTLR JavaScript解析器
        if (!initializeANTLR()) {
            addError("ANTLR JavaScript解析器初始化失败");
            return false;
        }
        
        if (debug_mode_) {
            std::cout << "JavaScript编译器核心初始化成功" << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        addError("初始化失败: " + std::string(e.what()));
        return false;
    }
}

void JavaScriptCompilerCore::cleanup() {
    cleanupANTLR();
    
    if (debug_mode_) {
        std::cout << "JavaScript编译器核心已清理" << std::endl;
    }
}

void JavaScriptCompilerCore::reset() {
    clearErrors();
    cleanup();
    initialize();
}

std::string JavaScriptCompilerCore::compile(const std::string& js_code) {
    try {
        // TODO: 使用ANTLR解析和编译JavaScript
        // 目前返回原始JavaScript代码
        if (debug_mode_) {
            std::cout << "JavaScript编译完成" << std::endl;
        }
        
        return js_code;
        
    } catch (const std::exception& e) {
        addError("编译异常: " + std::string(e.what()));
        return "";
    }
}

std::string JavaScriptCompilerCore::compileFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        addError("无法打开文件: " + file_path);
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    return compile(content.str());
}

bool JavaScriptCompilerCore::initializeANTLR() {
    // TODO: 实现ANTLR初始化
    return true; // 暂时返回true
}

void JavaScriptCompilerCore::cleanupANTLR() {
    // TODO: 实现ANTLR清理
}

void JavaScriptCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "JavaScript编译器错误: " << error << std::endl;
    }
}

void JavaScriptCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    if (debug_mode_) {
        std::cout << "JavaScript编译器警告: " << warning << std::endl;
    }
}

} // namespace chtl