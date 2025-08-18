#include "ChthollyInterface.h"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace chtl {
namespace chtholly {

std::unique_ptr<ChthollyModuleInterface> ChthollyModuleInterface::instance_ = nullptr;

ChthollyModuleInterface& ChthollyModuleInterface::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<ChthollyModuleInterface>(new ChthollyModuleInterface());
    }
    return *instance_;
}

bool ChthollyModuleInterface::initialize() {
    if (initialized_) {
        return true;
    }
    
    try {
        // 获取单例引用
        printmylove_interface_ = &PrintMyloveInterface::getInstance();
        ineveraway_interface_ = &INeverAwayInterface::getInstance();
        
        // 设置调试模式
        if (debug_mode_) {
            ineveraway_interface_->enableDebugMode(true);
        }
        
        initialized_ = true;
        
        logDebugMessage("Chtholly模块初始化成功");
        return true;
        
    } catch (const std::exception& e) {
        logDebugMessage("Chtholly模块初始化失败: " + std::string(e.what()));
        return false;
    }
}

void ChthollyModuleInterface::cleanup() {
    if (!initialized_) {
        return;
    }
    
    // 单例不需要手动清理，只重置指针
    printmylove_interface_ = nullptr;
    ineveraway_interface_ = nullptr;
    
    initialized_ = false;
    logDebugMessage("Chtholly模块已清理");
}

std::string ChthollyModuleInterface::printMylove(const std::string& config_json) {
    if (!initialized_) {
        logDebugMessage("错误：模块未初始化");
        return "";
    }
    
    std::string result = printmylove_interface_->printMylove(config_json);
    
    if (!result.empty()) {
        stats_.images_processed++;
        
        // 根据配置判断转换类型
        if (config_json.find("\"ASCII\"") != std::string::npos || 
            config_json.find("\"ascii\"") != std::string::npos) {
            stats_.ascii_conversions++;
        } else {
            stats_.pixel_conversions++;
        }
    }
    
    logDebugMessage("printMylove处理完成，输出长度: " + std::to_string(result.length()));
    return result;
}

bool ChthollyModuleInterface::hasPrintMyloveError() const {
    return initialized_ && printmylove_interface_->hasError();
}

std::string ChthollyModuleInterface::getPrintMyloveError() const {
    if (!initialized_) return "模块未初始化";
    return printmylove_interface_->getLastError();
}

std::string ChthollyModuleInterface::processVirDeclaration(const std::string& vir_code) {
    if (!initialized_) {
        logDebugMessage("错误：模块未初始化");
        return "";
    }
    
    std::string result = ineveraway_interface_->processVirDeclaration(vir_code);
    
    if (!result.empty()) {
        stats_.virtual_objects_created++;
    }
    
    logDebugMessage("vir声明处理完成");
    return result;
}

std::string ChthollyModuleInterface::processVirCall(const std::string& call_code) {
    if (!initialized_) {
        logDebugMessage("错误：模块未初始化");
        return "";
    }
    
    std::string result = ineveraway_interface_->processVirCall(call_code);
    
    if (!result.empty()) {
        stats_.function_calls_processed++;
    }
    
    logDebugMessage("vir调用处理完成");
    return result;
}

std::string ChthollyModuleInterface::generateINeverAwayCode() {
    if (!initialized_) {
        logDebugMessage("错误：模块未初始化");
        return "";
    }
    
    std::string prelude = ineveraway_interface_->generatePreludeCode();
    std::string epilogue = ineveraway_interface_->generateEpilogueCode();
    
    std::string result = prelude + "\n" + epilogue;
    
    if (ineveraway_interface_->hasGeneratedCode()) {
        // 统计生成的JS函数数量（简化计算）
        size_t function_count = std::count(result.begin(), result.end(), '{');
        stats_.js_functions_generated += function_count;
    }
    
    logDebugMessage("iNeverAway代码生成完成，长度: " + std::to_string(result.length()));
    return result;
}

bool ChthollyModuleInterface::hasINeverAwayError() const {
    return initialized_ && ineveraway_interface_->hasError();
}

std::string ChthollyModuleInterface::getINeverAwayError() const {
    if (!initialized_) return "模块未初始化";
    return ineveraway_interface_->getLastError();
}

void ChthollyModuleInterface::setDebugMode(bool enable) {
    debug_mode_ = enable;
    
    if (initialized_ && ineveraway_interface_) {
        ineveraway_interface_->enableDebugMode(enable);
    }
    
    logDebugMessage(enable ? "调试模式已启用" : "调试模式已禁用");
}

ChthollyModuleInterface::ModuleStatistics ChthollyModuleInterface::getStatistics() const {
    return stats_;
}

void ChthollyModuleInterface::printModuleInfo() const {
    std::cout << "=== Chtholly 珂朵莉模块信息 ===" << std::endl;
    std::cout << "模块状态: " << (initialized_ ? "已初始化" : "未初始化") << std::endl;
    std::cout << "调试模式: " << (debug_mode_ ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== printMylove统计 ===" << std::endl;
    std::cout << "处理图片数: " << stats_.images_processed << std::endl;
    std::cout << "ASCII转换数: " << stats_.ascii_conversions << std::endl;
    std::cout << "像素转换数: " << stats_.pixel_conversions << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== iNeverAway统计 ===" << std::endl;
    std::cout << "创建虚对象数: " << stats_.virtual_objects_created << std::endl;
    std::cout << "处理函数调用数: " << stats_.function_calls_processed << std::endl;
    std::cout << "生成JS函数数: " << stats_.js_functions_generated << std::endl;
    std::cout << std::endl;
    
    if (initialized_) {
        if (hasPrintMyloveError()) {
            std::cout << "printMylove错误: " << getPrintMyloveError() << std::endl;
        }
        
        if (hasINeverAwayError()) {
            std::cout << "iNeverAway错误: " << getINeverAwayError() << std::endl;
        }
    }
    
    std::cout << "珂朵莉永远在你身边 ♥" << std::endl;
}

bool ChthollyModuleInterface::hasAnyError() const {
    return hasPrintMyloveError() || hasINeverAwayError();
}

std::vector<std::string> ChthollyModuleInterface::getAllErrors() const {
    std::vector<std::string> errors;
    
    if (hasPrintMyloveError()) {
        errors.push_back("printMylove: " + getPrintMyloveError());
    }
    
    if (hasINeverAwayError()) {
        errors.push_back("iNeverAway: " + getINeverAwayError());
    }
    
    return errors;
}

void ChthollyModuleInterface::clearAllErrors() {
    if (initialized_) {
        if (printmylove_interface_) {
            printmylove_interface_->clearError();
        }
        
        if (ineveraway_interface_) {
            ineveraway_interface_->clearError();
        }
    }
}

void ChthollyModuleInterface::updateStatistics() {
    // 统计信息在各个方法中实时更新
}

void ChthollyModuleInterface::logDebugMessage(const std::string& message) const {
    if (debug_mode_) {
        std::cout << "[Chtholly Debug] " << message << std::endl;
    }
}

} // namespace chtholly
} // namespace chtl

// C接口实现

extern "C" {
    
const char* chtholly_printMylove(const char* config_json) {
    if (!config_json) return nullptr;
    
    static std::string result;
    result = chtl::chtholly::ChthollyModuleInterface::getInstance().printMylove(config_json);
    return result.c_str();
}

const char* chtholly_processVirDeclaration(const char* vir_code) {
    if (!vir_code) return nullptr;
    
    static std::string result;
    result = chtl::chtholly::ChthollyModuleInterface::getInstance().processVirDeclaration(vir_code);
    return result.c_str();
}

const char* chtholly_processVirCall(const char* call_code) {
    if (!call_code) return nullptr;
    
    static std::string result;
    result = chtl::chtholly::ChthollyModuleInterface::getInstance().processVirCall(call_code);
    return result.c_str();
}

const char* chtholly_generateINeverAwayCode() {
    static std::string result;
    result = chtl::chtholly::ChthollyModuleInterface::getInstance().generateINeverAwayCode();
    return result.c_str();
}

int chtholly_initialize() {
    return chtl::chtholly::ChthollyModuleInterface::getInstance().initialize() ? 1 : 0;
}

void chtholly_cleanup() {
    chtl::chtholly::ChthollyModuleInterface::getInstance().cleanup();
}

int chtholly_hasError() {
    return chtl::chtholly::ChthollyModuleInterface::getInstance().hasAnyError() ? 1 : 0;
}

const char* chtholly_getLastError() {
    static std::string error_msg;
    auto errors = chtl::chtholly::ChthollyModuleInterface::getInstance().getAllErrors();
    
    if (errors.empty()) {
        error_msg = "";
    } else {
        error_msg = errors[0]; // 返回第一个错误
    }
    
    return error_msg.c_str();
}

void chtholly_clearErrors() {
    chtl::chtholly::ChthollyModuleInterface::getInstance().clearAllErrors();
}

void chtholly_setDebugMode(int enable) {
    chtl::chtholly::ChthollyModuleInterface::getInstance().setDebugMode(enable != 0);
}

void chtholly_printModuleInfo() {
    chtl::chtholly::ChthollyModuleInterface::getInstance().printModuleInfo();
}

}