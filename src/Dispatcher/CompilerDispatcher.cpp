#include "CompilerDispatcher.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <future>

namespace chtl {

CompilerDispatcher::CompilerDispatcher() {
    scanner_ = std::make_unique<scanner::CHTLUnifiedScanner>();
}

CompilerDispatcher::~CompilerDispatcher() = default;

// === 初始化 ===

bool CompilerDispatcher::initialize() {
    if (initialized_) {
        return true;
    }
    
    clearErrors();
    
    try {
        // 初始化所有编译器
        chtlCompiler_ = std::make_unique<CHTLCompilerCore>();
        chtljsCompiler_ = std::make_unique<CHTLJSCompilerCore>();
        cssCompiler_ = std::make_unique<CSSCompilerCore>();
        jsCompiler_ = std::make_unique<JavaScriptCompilerCore>();
        
        // 初始化新的CHTL JS处理器
        // chtljs::CHTLJSProcessor::initialize(); // TODO: 实现CHTLJSProcessor
        
        // 初始化CJMOD系统
        // your_cjmod_design::CJMODSystemInitializer::initializeSystem(); // TODO: 实现CJMOD系统初始化
        
        // 初始化编译器
        if (!chtlCompiler_->initialize()) {
            addError("CHTL编译器初始化失败");
            return false;
        }
        
        if (!chtljsCompiler_->initialize()) {
            addError("CHTL JS编译器初始化失败");
            return false;
        }
        
        if (!cssCompiler_->initialize()) {
            addError("CSS编译器初始化失败");
            return false;
        }
        
        if (!jsCompiler_->initialize()) {
            addError("JavaScript编译器初始化失败");
            return false;
        }
        
        // 初始化上下文映射
        initializeContextMapping();
        
        initialized_ = true;
        
        if (debugMode_) {
            std::cout << "编译器调度器初始化成功" << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("编译器调度器初始化异常: " + std::string(e.what()));
        return false;
    }
}

void CompilerDispatcher::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    
    if (scanner_) {
        scanner_->setDebugMode(enabled);
    }
    
    if (chtlCompiler_) {
        chtlCompiler_->setDebugMode(enabled);
    }
    
    if (chtljsCompiler_) {
        chtljsCompiler_->setDebugMode(enabled);
    }
    
    if (cssCompiler_) {
        cssCompiler_->setDebugMode(enabled);
    }
    
    if (jsCompiler_) {
        jsCompiler_->setDebugMode(enabled);
    }
}

// === 主编译接口 ===

CompilerDispatcher::CompilationResult CompilerDispatcher::compileCode(const std::string& code) {
    CompilationResult result;
    clearErrors();
    
    if (!initialized_ && !initialize()) {
        result.errors = getErrors();
        return result;
    }
    
    try {
        // 1. 使用统一扫描器分析代码
        if (debugMode_) {
            std::cout << "开始扫描代码..." << std::endl;
        }
        
        auto scanResult = scanner_->scanCode(code);
        
        if (scanResult.hasErrors) {
            result.errors = scanResult.errors;
            result.warnings = scanResult.warnings;
            return result;
        }
        
        // 2. 编译每个代码片段
        std::vector<CompilationResult> fragmentResults;
        
        for (const auto& fragment : scanResult.fragments) {
            if (debugMode_) {
                std::cout << "编译片段: " << static_cast<int>(fragment.context) << std::endl;
            }
            
            CompilationResult fragmentResult = compileFragment(fragment);
            fragmentResults.push_back(fragmentResult);
        }
        
        // 3. 合并编译结果
        result = mergeCompilationResults(fragmentResults);
        
        if (debugMode_) {
            std::cout << "编译完成，成功: " << (result.success ? "是" : "否") << std::endl;
        }
        
    } catch (const std::exception& e) {
        addError("编译异常: " + std::string(e.what()));
        result.errors = getErrors();
    }
    
    return result;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::compileFragment(const scanner::CHTLUnifiedScanner::CodeFragment& fragment) {
    CompilationResult result;
    
    // 选择合适的编译器
    std::string compilerName = selectCompilerForFragment(fragment);
    
    if (debugMode_) {
        std::cout << "为片段选择编译器: " << compilerName << std::endl;
    }
    
    // 根据编译器类型编译片段
    if (compilerName == "chtl") {
        // CHTL编译器处理
        std::string compiledHTML = chtlCompiler_->compile(fragment.content);
        result.compiledCode = compiledHTML;
        result.success = !compiledHTML.empty();
        // result.errors = chtlResult.errors; // TODO: 实现错误收集
        // result.warnings = chtlResult.warnings; // TODO: 实现警告收集
        
    } else if (compilerName == "chtljs") {
        // CHTL JS编译器处理 - 集成新的处理器
        
        // TODO: 实现CHTL JS处理器
        std::string processedCode = fragment.content; // 暂时直接使用原始代码
        
        // TODO: 实现CJMOD管理器处理
        // auto& cjmodManager = your_cjmod_design::CJMODManager::getInstance();
        // processedCode = cjmodManager.processCHTLJS(processedCode);
        
        // 使用CHTL JS编译器编译
        std::string compiledJS = chtljsCompiler_->compile(processedCode);
        result.compiledCode = compiledJS;
        result.success = !compiledJS.empty();
        // result.errors = {}; // TODO: 实现错误收集
        // result.warnings = {}; // TODO: 实现警告收集
        
    } else if (compilerName == "css") {
        // CSS编译器处理
        auto cssResult = cssCompiler_->compileCode(fragment.content);
        result.compiledCode = cssResult.compiledCode;
        result.success = cssResult.success;
        result.errors = cssResult.errors;
        result.warnings = cssResult.warnings;
        
    } else if (compilerName == "javascript") {
        // JavaScript编译器处理
        auto jsResult = jsCompiler_->compileCode(fragment.content);
        result.compiledCode = jsResult.compiledCode;
        result.success = jsResult.success;
        result.errors = jsResult.errors;
        result.warnings = jsResult.warnings;
        
    } else {
        // 未知编译器
        result.success = false;
        result.errors.push_back("未知的编译器类型: " + compilerName);
    }
    
    return result;
}

// === 编译器管理 ===

CHTLCompilerCore* CompilerDispatcher::getCHTLCompiler() {
    return chtlCompiler_.get();
}

CHTLJSCompilerCore* CompilerDispatcher::getCHTLJSCompiler() {
    return chtljsCompiler_.get();
}

CSSCompilerCore* CompilerDispatcher::getCSSCompiler() {
    return cssCompiler_.get();
}

JavaScriptCompilerCore* CompilerDispatcher::getJavaScriptCompiler() {
    return jsCompiler_.get();
}

// === 统一扫描器接口 ===

scanner::CHTLUnifiedScanner* CompilerDispatcher::getScanner() {
    return scanner_.get();
}

std::vector<scanner::CHTLUnifiedScanner::CodeFragment> CompilerDispatcher::scanCode(const std::string& code) {
    if (!scanner_) {
        return {};
    }
    
    auto scanResult = scanner_->scanCode(code);
    return scanResult.fragments;
}

// === 智能调度 ===

std::string CompilerDispatcher::selectCompilerForFragment(const scanner::CHTLUnifiedScanner::CodeFragment& fragment) {
    // 根据代码片段上下文选择合适的编译器
    
    auto it = contextToCompilerMap_.find(fragment.context);
    if (it != contextToCompilerMap_.end()) {
        return it->second;
    }
    
    // 特殊处理CHTL JS
    if (fragment.context == scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT) {
        if (fragment.containsCHTLJSFeatures) {
            return "chtljs";  // 使用CHTL JS编译器
        } else {
            return "javascript";  // 使用普通JavaScript编译器
        }
    }
    
    // 默认使用CHTL编译器
    return "chtl";
}

bool CompilerDispatcher::isCompilerCompatible(const std::string& compilerName, const scanner::CHTLUnifiedScanner::CodeFragment& fragment) {
    // 验证编译器与片段的兼容性
    
    if (compilerName == "chtljs") {
        // CHTL JS编译器只能处理script上下文且包含CHTL JS特征的片段
        return fragment.context == scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT && 
               fragment.containsCHTLJSFeatures;
    }
    
    if (compilerName == "css") {
        // CSS编译器只能处理style上下文
        return fragment.context == scanner::CHTLUnifiedScanner::CodeContext::CHTL_STYLE;
    }
    
    if (compilerName == "javascript") {
        // JavaScript编译器可以处理script上下文但不包含CHTL JS特征的片段
        return fragment.context == scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT && 
               !fragment.containsCHTLJSFeatures;
    }
    
    if (compilerName == "chtl") {
        // CHTL编译器可以处理大多数上下文
        return fragment.context != scanner::CHTLUnifiedScanner::CodeContext::CSS_RAW &&
               fragment.context != scanner::CHTLUnifiedScanner::CodeContext::JS_RAW;
    }
    
    return false;
}

// === 编译优化 ===

std::vector<CompilerDispatcher::CompilationResult> CompilerDispatcher::compileFragmentsParallel(const std::vector<scanner::CHTLUnifiedScanner::CodeFragment>& fragments) {
    // 并行编译代码片段
    
    std::vector<std::future<CompilationResult>> futures;
    std::vector<CompilationResult> results;
    
    // 启动并行编译任务
    for (const auto& fragment : fragments) {
        futures.push_back(std::async(std::launch::async, [this, fragment]() {
            return compileFragment(fragment);
        }));
    }
    
    // 收集结果
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    return results;
}

CompilerDispatcher::CompilationResult CompilerDispatcher::mergeCompilationResults(const std::vector<CompilationResult>& results) {
    // 合并编译结果
    
    CompilationResult merged;
    merged.success = true;
    
    for (const auto& result : results) {
        // 合并代码
        merged.compiledCode += result.compiledCode + "\n";
        
        // 合并错误和警告
        merged.errors.insert(merged.errors.end(), result.errors.begin(), result.errors.end());
        merged.warnings.insert(merged.warnings.end(), result.warnings.begin(), result.warnings.end());
        
        // 如果任何一个片段编译失败，整体就失败
        if (!result.success) {
            merged.success = false;
        }
    }
    
    return merged;
}

// === 私有方法 ===

void CompilerDispatcher::initializeContextMapping() {
    // 初始化上下文到编译器的映射
    
    using Context = scanner::CHTLUnifiedScanner::CodeContext;
    
    contextToCompilerMap_[Context::CHTL_GLOBAL] = "chtl";
    contextToCompilerMap_[Context::CHTL_ELEMENT] = "chtl";
    contextToCompilerMap_[Context::CHTL_STYLE] = "css";
    contextToCompilerMap_[Context::CHTL_SCRIPT] = "chtljs";  // 默认，但会根据特征动态调整
    contextToCompilerMap_[Context::CHTL_TEXT] = "chtl";
    contextToCompilerMap_[Context::CHTL_TEMPLATE] = "chtl";
    contextToCompilerMap_[Context::CHTL_CUSTOM] = "chtl";
    contextToCompilerMap_[Context::CHTL_ORIGIN] = "chtl";
    contextToCompilerMap_[Context::CHTL_CONFIGURATION] = "chtl";
    contextToCompilerMap_[Context::CHTL_IMPORT] = "chtl";
    contextToCompilerMap_[Context::CHTL_NAMESPACE] = "chtl";
    contextToCompilerMap_[Context::HTML_RAW] = "chtl";
    contextToCompilerMap_[Context::CSS_RAW] = "css";
    contextToCompilerMap_[Context::JS_RAW] = "javascript";
}

// === 错误处理 ===

void CompilerDispatcher::addError(const std::string& message) {
    errors_.push_back("CompilerDispatcher: " + message);
}

void CompilerDispatcher::addWarning(const std::string& message) {
    warnings_.push_back("CompilerDispatcher: " + message);
}

bool CompilerDispatcher::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> CompilerDispatcher::getErrors() const {
    return errors_;
}

std::vector<std::string> CompilerDispatcher::getWarnings() const {
    return warnings_;
}

void CompilerDispatcher::clearErrors() {
    errors_.clear();
    warnings_.clear();
}

} // namespace chtl