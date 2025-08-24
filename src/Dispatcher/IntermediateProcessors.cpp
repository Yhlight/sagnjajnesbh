#include "Dispatcher/IntermediateProcessors.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTLJS/Compiler/CHTLJSCompiler.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace CHTL {
namespace Dispatcher {

// ============ JavaScript中间处理器实现 ============

JavaScriptIntermediateProcessor::JavaScriptIntermediateProcessor() {
    LogDebug("JavaScript中间处理器初始化完成");
}

JavaScriptIntermediateProcessor::~JavaScriptIntermediateProcessor() {
    LogDebug("JavaScript中间处理器清理完成");
}

FragmentProcessingResult JavaScriptIntermediateProcessor::ProcessJavaScriptFragments(
    const std::vector<Scanner::CodeFragment>& chtlJSFragments,
    const std::vector<Scanner::CodeFragment>& jsFragments) {
    
    FragmentProcessingResult result;
    std::vector<std::string> allJavaScriptCode;
    
    LogDebug("开始处理JavaScript片段合并，CHTL JS片段: " + 
             std::to_string(chtlJSFragments.size()) + 
             "，纯JS片段: " + std::to_string(jsFragments.size()));
    
    try {
        // 1. 处理所有CHTL JS片段，转换为JavaScript
        for (const auto& fragment : chtlJSFragments) {
            std::string convertedJS = ConvertCHTLJSToJavaScript(fragment.content);
            if (!convertedJS.empty()) {
                allJavaScriptCode.push_back(convertedJS);
                LogDebug("CHTL JS片段转换完成，长度: " + std::to_string(convertedJS.length()));
            }
        }
        
        // 2. 处理所有纯JS片段
        for (const auto& fragment : jsFragments) {
            // 对纯JS片段也进行局部script块CHTL特征处理
            FragmentProcessingResult jsResult = ProcessLocalScriptCHTLFeatures(fragment.content);
            if (jsResult.success && !jsResult.processedCode.empty()) {
                allJavaScriptCode.push_back(jsResult.processedCode);
                LogDebug("纯JS片段处理完成，长度: " + std::to_string(jsResult.processedCode.length()));
            } else {
                // 如果处理失败，使用原始代码
                allJavaScriptCode.push_back(fragment.content);
                LogDebug("纯JS片段使用原始代码，长度: " + std::to_string(fragment.content.length()));
            }
        }
        
        // 3. 合并所有JavaScript代码
        result.processedCode = MergeJavaScriptFragments(allJavaScriptCode);
        
        // 4. 验证合并后的JavaScript代码
        if (ValidateMergedJavaScript(result.processedCode)) {
            result.success = true;
            LogDebug("JavaScript片段合并成功，最终代码长度: " + std::to_string(result.processedCode.length()));
        } else {
            result.warnings.push_back("合并后的JavaScript代码可能存在语法问题");
            result.success = true; // 仍然认为成功，只是警告
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("JavaScript片段处理异常: " + std::string(e.what()));
        LogDebug("JavaScript片段处理异常: " + std::string(e.what()));
    }
    
    return result;
}

FragmentProcessingResult JavaScriptIntermediateProcessor::ProcessLocalScriptCHTLFeatures(
    const std::string& scriptContent) {
    
    FragmentProcessingResult result;
    result.processedCode = scriptContent;
    
    try {
        // 中间处理器只负责代码合并，不进行任何语法转换
        // 所有的CHTL特征（变量模板、增强选择器、导入语法等）都由相应的编译器处理
        
        // 保持原始代码不变，只记录日志
        LogDebug("局部script特征处理：保持原始代码不变");
        
        result.success = true;
        
        if (result.processedCode != scriptContent) {
            LogDebug("局部script块CHTL特征处理完成，发现并转换了CHTL语法");
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("局部script块处理异常: " + std::string(e.what()));
    }
    
    return result;
}

std::string JavaScriptIntermediateProcessor::ConvertCHTLJSToJavaScript(const std::string& chtlJSCode) {
    std::string result = chtlJSCode;
    
    // 中间处理器不负责CHTL JS到JavaScript的转换
    // 这个转换工作由CHTL JS编译器自身负责
    // 调用各个处理方法，但它们现在都只是保持原始代码不变
    result = ProcessArrowOperators(result);
    result = ProcessEnhancedSelectors(result);
    result = ProcessVirtualObjects(result);
    result = ProcessCHTLJSMethods(result);
    
    LogDebug("CHTL JS代码处理完成（保持原始状态，等待CHTL JS编译器处理）");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessArrowOperators(const std::string& code) {
    // 中间处理器不负责转换，只负责代码合并
    // 箭头操作符的转换由CHTL JS编译器自身负责
    LogDebug("保持原始代码，不进行箭头操作符转换");
    return code;
}

std::string JavaScriptIntermediateProcessor::ProcessEnhancedSelectors(const std::string& code) {
    // 中间处理器不负责转换，只负责代码合并
    // 增强选择器的转换由CHTL JS编译器自身负责
    LogDebug("保持原始代码，不进行增强选择器转换");
    return code;
}

std::string JavaScriptIntermediateProcessor::ProcessVirtualObjects(const std::string& code) {
    // 中间处理器不负责转换，只负责代码合并
    // 虚对象的转换由CHTL JS编译器自身负责
    LogDebug("保持原始代码，不进行虚对象转换");
    return code;
}

std::string JavaScriptIntermediateProcessor::ProcessCHTLJSMethods(const std::string& code) {
    // 中间处理器不负责转换，只负责代码合并
    // CHTL JS方法的转换由CHTL JS编译器自身负责
    LogDebug("保持原始代码，不进行CHTL JS方法转换");
    return code;
}

std::string JavaScriptIntermediateProcessor::MergeJavaScriptFragments(
    const std::vector<std::string>& jsCodeFragments) {
    
    if (jsCodeFragments.empty()) {
        return "";
    }
    
    std::stringstream merged;
    
    // 添加CHTL运行时支持
    merged << "// === CHTL JavaScript Runtime ===" << std::endl;
    merged << "if (typeof CHTL === 'undefined') {" << std::endl;
    merged << "  window.CHTL = {" << std::endl;
    merged << "    delegate: function(config) { /* 委托实现 */ }," << std::endl;
    merged << "    animate: function(config) { /* 动画实现 */ }," << std::endl;
    merged << "    ThemeColor: function(name) { return 'var(--' + name + ')'; }," << std::endl;
    merged << "    Colors: function(name) { return 'var(--color-' + name + ')'; }," << std::endl;
    merged << "    Spacing: function(size) { return 'var(--spacing-' + size + ')'; }" << std::endl;
    merged << "  };" << std::endl;
    merged << "}" << std::endl << std::endl;
    
    // 合并所有代码片段
    for (size_t i = 0; i < jsCodeFragments.size(); ++i) {
        const std::string& fragment = jsCodeFragments[i];
        
        if (!fragment.empty()) {
            merged << "// === Fragment " << (i + 1) << " ===" << std::endl;
            merged << fragment;
            
            // 确保片段以分号或换行结束
            if (!fragment.empty() && fragment.back() != ';' && fragment.back() != '\n') {
                merged << std::endl;
            }
            merged << std::endl;
        }
    }
    
    LogDebug("JavaScript片段合并完成，共合并 " + std::to_string(jsCodeFragments.size()) + " 个片段");
    return merged.str();
}

bool JavaScriptIntermediateProcessor::ValidateMergedJavaScript(const std::string& jsCode) {
    // 基本的JavaScript语法验证
    // 检查括号是否配对
    int parenCount = 0, braceCount = 0, bracketCount = 0;
    
    for (char c : jsCode) {
        switch (c) {
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    bool isValid = (parenCount == 0 && braceCount == 0 && bracketCount == 0);
    
    if (!isValid) {
        LogDebug("JavaScript语法验证失败：括号不匹配");
    }
    
    return isValid;
}

void JavaScriptIntermediateProcessor::LogDebug(const std::string& message) {
    if (debugMode_) {
        std::cout << "[JSProcessor] " << message << std::endl;
        Utils::ErrorHandler::GetInstance().LogInfo("JSProcessor: " + message);
    }
}

// ============ CSS中间处理器实现 ============

CSSIntermediateProcessor::CSSIntermediateProcessor() {
    LogDebug("CSS中间处理器初始化完成");
}

CSSIntermediateProcessor::~CSSIntermediateProcessor() {
    LogDebug("CSS中间处理器清理完成");
}

FragmentProcessingResult CSSIntermediateProcessor::ProcessCSSFragments(
    const std::vector<Scanner::CodeFragment>& cssFragments) {
    
    FragmentProcessingResult result;
    std::stringstream mergedCSS;
    
    LogDebug("开始处理CSS片段，共 " + std::to_string(cssFragments.size()) + " 个片段");
    
    try {
        for (const auto& fragment : cssFragments) {
            std::string processedCSS = fragment.content;
            
            // 1. 处理CHTL变量模板
            processedCSS = ProcessCHTLVariableTemplates(processedCSS);
            
            // 2. 处理CHTL JS表达式
            processedCSS = ProcessCHTLJSExpressions(processedCSS);
            
            mergedCSS << processedCSS << std::endl;
        }
        
        result.processedCode = mergedCSS.str();
        result.success = true;
        
        LogDebug("CSS片段处理完成，最终长度: " + std::to_string(result.processedCode.length()));
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("CSS片段处理异常: " + std::string(e.what()));
    }
    
    return result;
}

std::string CSSIntermediateProcessor::ProcessCHTLVariableTemplates(const std::string& cssCode) {
    // 中间处理器不负责转换，只负责代码合并
    // CHTL变量模板的转换由CHTL编译器自身负责
    LogDebug("保持原始CSS代码，不进行变量模板转换");
    return cssCode;
}

std::string CSSIntermediateProcessor::ProcessCHTLJSExpressions(const std::string& cssCode) {
    // 中间处理器不负责转换，只负责代码合并
    // CHTL JS表达式的转换由相应的编译器自身负责
    LogDebug("保持原始CSS代码，不进行CHTL JS表达式转换");
    return cssCode;
}

void CSSIntermediateProcessor::LogDebug(const std::string& message) {
    if (debugMode_) {
        std::cout << "[CSSProcessor] " << message << std::endl;
        Utils::ErrorHandler::GetInstance().LogInfo("CSSProcessor: " + message);
    }
}

} // namespace Dispatcher
} // namespace CHTL