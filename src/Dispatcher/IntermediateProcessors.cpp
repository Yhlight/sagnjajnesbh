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
        // 1. 处理CHTL变量模板（如 ThemeColor(), Colors() 等）
        std::regex chtlVarRegex(R"(\b(ThemeColor|Colors|Spacing)\s*\(\s*([^)]*)\s*\))");
        result.processedCode = std::regex_replace(result.processedCode, chtlVarRegex, "CHTL.$1($2)");
        
        // 2. 处理CHTL选择器增强（局部script中可能的{{selector}}语法）
        std::regex localSelectorRegex(R"(\{\{([^}]+)\}\})");
        result.processedCode = std::regex_replace(result.processedCode, localSelectorRegex, "document.querySelector('$1')");
        
        // 3. 处理CHTL JS导入语法
        std::regex importRegex(R"(\[Import\]\s*@(\w+)\s+from\s+([^;]+);?)");
        std::string importTemp = result.processedCode;
        std::sregex_iterator iter(importTemp.begin(), importTemp.end(), importRegex);
        std::sregex_iterator end;
        
        while (iter != end) {
            std::smatch match = *iter;
            std::string importType = match[1].str();
            std::string moduleName = match[2].str();
            
            std::string replacement;
            if (importType == "CJmod") {
                result.warnings.push_back("CJMOD导入在JavaScript中需要特殊处理: " + moduleName);
                replacement = "// CJMOD导入: " + moduleName + " (需要动态加载)";
            } else {
                replacement = "// CHTL导入: " + moduleName;
            }
            
            result.processedCode = result.processedCode.replace(match.position(), match.length(), replacement);
            ++iter;
        }
        
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
    
    // 1. 处理箭头操作符转换
    result = ProcessArrowOperators(result);
    
    // 2. 处理增强选择器
    result = ProcessEnhancedSelectors(result);
    
    // 3. 处理虚对象语法
    result = ProcessVirtualObjects(result);
    
    // 4. 处理CHTL JS特定方法
    result = ProcessCHTLJSMethods(result);
    
    LogDebug("CHTL JS转换为JavaScript完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessArrowOperators(const std::string& code) {
    // 将箭头操作符 -> 转换为点操作符 .
    // 这是核心的等价转换
    std::regex arrowRegex(R"((\w+)\s*->\s*(\w+))");
    std::string result = std::regex_replace(code, arrowRegex, "$1.$2");
    
    LogDebug("箭头操作符转换完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessEnhancedSelectors(const std::string& code) {
    // 处理增强选择器 {{selector}}->method 转换为 document.querySelector('selector').method
    std::regex selectorRegex(R"(\{\{([^}]+)\}\}\s*\.\s*(\w+))");
    std::string result = std::regex_replace(code, selectorRegex, 
        "document.querySelector('$1').$2");
    
    LogDebug("增强选择器转换完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessVirtualObjects(const std::string& code) {
    // 处理虚对象声明：vir objectName = {...} 
    std::regex virRegex(R"(vir\s+(\w+)\s*=\s*(\{[^}]*\}))");
    std::string result = std::regex_replace(code, virRegex, 
        "const $1 = $2");
    
    LogDebug("虚对象语法转换完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessCHTLJSMethods(const std::string& code) {
    std::string result = code;
    
    // 处理CHTL JS特定方法调用
    // listen() -> addEventListener
    std::regex listenRegex(R"(\blisten\s*\(\s*([^,]+)\s*,\s*([^)]+)\s*\))");
    result = std::regex_replace(result, listenRegex, "addEventListener($1, $2)");
    
    // delegate() -> 自定义委托函数
    std::regex delegateRegex(R"(\bdelegate\s*\(\s*([^)]+)\s*\))");
    result = std::regex_replace(result, delegateRegex, "CHTL.delegate($1)");
    
    // animate() -> 自定义动画函数
    std::regex animateRegex(R"(\banimate\s*\(\s*([^)]+)\s*\))");
    result = std::regex_replace(result, animateRegex, "CHTL.animate($1)");
    
    LogDebug("CHTL JS方法转换完成");
    return result;
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
    std::string result = cssCode;
    
    // 处理CHTL变量模板
    // ThemeColor() -> var(--theme-color)
    std::regex themeColorRegex(R"(ThemeColor\s*\(\s*([^)]*)\s*\))");
    result = std::regex_replace(result, themeColorRegex, "var(--$1)");
    
    // Colors() -> var(--color-name)
    std::regex colorsRegex(R"(Colors\s*\(\s*([^)]*)\s*\))");
    result = std::regex_replace(result, colorsRegex, "var(--color-$1)");
    
    // Spacing() -> var(--spacing-size)
    std::regex spacingRegex(R"(Spacing\s*\(\s*([^)]*)\s*\))");
    result = std::regex_replace(result, spacingRegex, "var(--spacing-$1)");
    
    LogDebug("CSS CHTL变量模板处理完成");
    return result;
}

std::string CSSIntermediateProcessor::ProcessCHTLJSExpressions(const std::string& cssCode) {
    std::string result = cssCode;
    
    // 处理CSS中可能的CHTL JS表达式
    // 例如：{{selector}} 在CSS中转换为适当的选择器
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    result = std::regex_replace(result, selectorRegex, "$1");
    
    LogDebug("CSS CHTL JS表达式处理完成");
    return result;
}

void CSSIntermediateProcessor::LogDebug(const std::string& message) {
    if (debugMode_) {
        std::cout << "[CSSProcessor] " << message << std::endl;
        Utils::ErrorHandler::GetInstance().LogInfo("CSSProcessor: " + message);
    }
}

} // namespace Dispatcher
} // namespace CHTL