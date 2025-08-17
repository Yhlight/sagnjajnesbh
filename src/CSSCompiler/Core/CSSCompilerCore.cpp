#include "CSSCompilerCore.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace chtl {

CSSCompilerCore::CSSCompilerCore() : debug_mode_(false) {
    initialize();
}

CSSCompilerCore::~CSSCompilerCore() {
    cleanup();
}

bool CSSCompilerCore::initialize() {
    try {
        // TODO: 初始化ANTLR CSS解析器
        if (!initializeANTLR()) {
            addError("ANTLR CSS解析器初始化失败");
            return false;
        }
        
        if (debug_mode_) {
            std::cout << "CSS编译器核心初始化成功" << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        addError("初始化失败: " + std::string(e.what()));
        return false;
    }
}

void CSSCompilerCore::cleanup() {
    cleanupANTLR();
    
    if (debug_mode_) {
        std::cout << "CSS编译器核心已清理" << std::endl;
    }
}

void CSSCompilerCore::reset() {
    clearErrors();
    cleanup();
    initialize();
}

std::string CSSCompilerCore::compile(const std::string& css_code) {
    try {
        // TODO: 使用ANTLR解析和编译CSS
        // 目前返回原始CSS代码
        if (debug_mode_) {
            std::cout << "CSS编译完成" << std::endl;
        }
        
        return css_code;
        
    } catch (const std::exception& e) {
        addError("编译异常: " + std::string(e.what()));
        return "";
    }
}

std::string CSSCompilerCore::compileFile(const std::string& file_path) {
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

bool CSSCompilerCore::initializeANTLR() {
    // TODO: 实现ANTLR初始化
    return true; // 暂时返回true
}

void CSSCompilerCore::cleanupANTLR() {
    // TODO: 实现ANTLR清理
}

void CSSCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "CSS编译器错误: " << error << std::endl;
    }
}

void CSSCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    if (debug_mode_) {
        std::cout << "CSS编译器警告: " << warning << std::endl;
    }
}

} // namespace chtl