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
        // 1. 根据官方语法文档，局部script中不应该有CHTL变量模板
        // 这些变量模板应该在CHTL编译阶段处理，而不是在JavaScript中
        // 因此这里不进行变量模板转换
        
        // 2. 处理增强选择器{{selector}}（这是CHTL JS特征，应该转换）
        // 根据官方语法文档，增强选择器会创建DOM对象
        // 这部分应该由CHTL JS编译器处理，但如果在局部script中出现，则进行基础转换
        std::regex localSelectorRegex(R"(\{\{([^}]+)\}\})");
        // 在局部script中，直接转换为基础的querySelector
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
    // 根据官方CHTL语法文档第1162-1164行：
    // "使用到CHTL JS语法时，我们推荐使用->代替. 以便明确使用了CHTL JS语法"
    // "->与.是完全等价的，因此你可以直接使用->进行链式访问"
    
    std::string result = code;
    
    // 将箭头操作符 -> 转换为点操作符 .（它们完全等价）
    // 支持链式访问，处理复杂的选择器和方法调用
    std::regex arrowRegex(R"((\w+|\)\s*|\]\s*)\s*->\s*(\w+))");
    result = std::regex_replace(result, arrowRegex, "$1.$2");
    
    // 处理增强选择器中的箭头操作符
    std::regex selectorArrowRegex(R"(\}\}\s*->\s*(\w+))");
    result = std::regex_replace(result, selectorArrowRegex, "}}.$1");
    
    LogDebug("箭头操作符转换完成（-> 等价于 .）");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessEnhancedSelectors(const std::string& code) {
    // 根据官方CHTL语法文档，处理增强选择器 {{selector}} 转换为 DOM对象
    // 支持的形式：{{box}}, {{.box}}, {{#box}}, {{button}}, {{.box button}}, {{button[0]}}
    std::string result = code;
    
    // 使用迭代器方式处理复杂的转换逻辑
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(result.begin(), result.end(), selectorRegex);
    std::sregex_iterator end;
    
    // 从后往前替换，避免位置偏移问题
    std::vector<std::pair<size_t, std::pair<size_t, std::string>>> replacements;
    
    while (iter != end) {
        std::smatch match = *iter;
        std::string selector = match[1].str();
        std::string replacement;
        
        // 处理特殊选择器形式
        if (selector.find('[') != std::string::npos) {
            // 处理索引访问 {{button[0]}} -> document.querySelectorAll('button')[0]
            size_t bracketPos = selector.find('[');
            std::string baseSelector = selector.substr(0, bracketPos);
            std::string indexPart = selector.substr(bracketPos);
            replacement = "document.querySelectorAll('" + baseSelector + "')" + indexPart;
        } else if (selector.find(' ') != std::string::npos) {
            // 处理复合选择器 {{.box button}} -> document.querySelectorAll('.box button')
            replacement = "document.querySelectorAll('" + selector + "')";
        } else if (selector.length() > 0 && (selector[0] == '.' || selector[0] == '#')) {
            // 处理类选择器和ID选择器 {{.box}}, {{#box}} -> document.querySelector
            replacement = "document.querySelector('" + selector + "')";
        } else {
            // 处理普通选择器 {{box}} -> 智能查找 (先判断tag，然后查找类名/id)
            replacement = "(document.querySelector('#" + selector + "') || document.querySelector('." + selector + "') || document.querySelector('" + selector + "'))";
        }
        
        replacements.push_back({static_cast<size_t>(match.position()), {static_cast<size_t>(match.length()), replacement}});
        ++iter;
    }
    
    // 从后往前替换
    for (auto rit = replacements.rbegin(); rit != replacements.rend(); ++rit) {
        result.replace(rit->first, rit->second.first, rit->second.second);
    }
    
    LogDebug("增强选择器转换完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessVirtualObjects(const std::string& code) {
    // 根据官方语法文档，虚对象: vir 对象名 = CHTL JS函数
    // vir是编译期语法糖，不涉及JS，应该由CHTL JS编译器处理
    // 这里只是作为容错机制，如果CHTL JS编译器没有处理，则进行基础转换
    std::string result = code;
    
    // 检查是否有虚对象语法
    std::regex virRegex(R"(\bvir\s+\w+\s*=)");
    if (std::regex_search(result, virRegex)) {
        LogDebug("检测到虚对象语法，这应该由CHTL JS编译器处理");
        // 作为容错，进行基础转换
        std::regex virDeclRegex(R"(vir\s+(\w+)\s*=\s*([^;]+;?))");
        result = std::regex_replace(result, virDeclRegex, "var $1 = $2");
    }
    
    LogDebug("虚对象语法转换完成");
    return result;
}

std::string JavaScriptIntermediateProcessor::ProcessCHTLJSMethods(const std::string& code) {
    std::string result = code;
    
    // 根据官方语法文档，CHTL JS特定方法应该由CHTL JS编译器处理
    // 这里只检查是否有这些方法，如果有则记录警告
    
    // 检查是否有CHTL JS特定方法
    std::regex chtlJSMethodRegex(R"(\b(listen|delegate|animate)\s*\()");
    if (std::regex_search(result, chtlJSMethodRegex)) {
        LogDebug("检测到CHTL JS特定方法，这应该由CHTL JS编译器处理");
    }
    
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
    
    // 根据官方语法文档，CSS中的CHTL变量模板应该在CHTL编译阶段处理
    // 这里只是作为容错机制，理论上不应该在这个阶段出现CHTL变量模板
    // 如果出现，可能是编译流程有问题，记录警告但不进行转换
    
    // 检查是否有CHTL变量模板语法
    std::regex chtlTemplateRegex(R"(\b(ThemeColor|Colors|Spacing)\s*\()");
    if (std::regex_search(result, chtlTemplateRegex)) {
        LogDebug("警告：CSS中检测到CHTL变量模板语法，这应该在CHTL编译阶段处理");
    }
    
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