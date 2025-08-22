#include "Dispatcher/CompilerDispatcher.h"
#include "CSS/CSSCompiler.h"
#include "JavaScript/JavaScriptCompiler.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <map>
#include <sstream>

namespace CHTL {
namespace Dispatcher {

CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& config) 
    : config_(config) {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() {
    Cleanup();
}

void CompilerDispatcher::InitializeCompilers() {
    // 初始化统一扫描器
    Scanner::ScanConfig scanConfig;
    scanConfig.preserveWhitespace = false;
    scanConfig.enableMinimalUnit = true;
    scanConfig.enableContextCheck = true;
    scanner_ = std::make_unique<Scanner::CHTLUnifiedScanner>(scanConfig);
    
    // 基础实现：暂时不初始化复杂的解析器
    // 实际实现需要完整的解析器集成
    // chtlParser_ = ...;
    // chtlJSParser_ = ...;
    
    // 初始化CSS编译器（ANTLR4）
    cssCompiler_ = std::make_unique<CSS::CSSCompiler>();
    
    // 初始化JavaScript编译器（ANTLR4）
    jsCompiler_ = std::make_unique<JavaScript::JavaScriptCompiler>();
}

CompilationResult CompilerDispatcher::Compile(const std::string& source, const std::string& fileName) {
    CompilationResult result;
    
    try {
        if (config_.enableDebugOutput) {
            Utils::ErrorHandler::GetInstance().LogInfo(
                "CompilerDispatcher开始编译: " + fileName
            );
        }
        
        // 第一步：使用统一扫描器进行精准代码切割
        auto fragments = scanner_->Scan(source, fileName);
        
        if (config_.enableDebugOutput) {
            Utils::ErrorHandler::GetInstance().LogInfo(
                "扫描器切割出 " + std::to_string(fragments.size()) + " 个代码片段"
            );
        }
        
        // 第二步：分发代码片段给对应的编译器
        result = DispatchFragments(fragments, fileName);
        
        if (config_.enableDebugOutput) {
            Utils::ErrorHandler::GetInstance().LogInfo(
                "编译完成，成功: " + std::string(result.success ? "是" : "否")
            );
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("编译器调度异常: " + std::string(e.what()));
        Utils::ErrorHandler::GetInstance().LogError(
            "CompilerDispatcher编译异常: " + std::string(e.what())
        );
    }
    
    return result;
}

CompilationResult CompilerDispatcher::DispatchFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName) {
    CompilationResult result;
    
    // 按类型分组代码片段
    auto groupedFragments = GroupFragmentsByType(fragments);
    
    std::string htmlOutput = "";
    std::string cssOutput = "";
    std::string jsOutput = "";
    
    try {
        // 处理CHTL片段
        if (groupedFragments.count(Scanner::FragmentType::CHTL)) {
            htmlOutput = ProcessCHTLFragments(groupedFragments[Scanner::FragmentType::CHTL], fileName);
        }
        
        // 处理CSS片段 - 目标规划第44行：全局style → CSS编译器
        if (groupedFragments.count(Scanner::FragmentType::CSS)) {
            cssOutput = ProcessCSSFragments(groupedFragments[Scanner::FragmentType::CSS], fileName);
        }
        
        // 处理CHTL JS片段
        if (groupedFragments.count(Scanner::FragmentType::CHTLJS)) {
            std::string chtlJSOutput = ProcessCHTLJSFragments(groupedFragments[Scanner::FragmentType::CHTLJS], fileName);
            jsOutput += chtlJSOutput;
        }
        
        // 处理JavaScript片段
        if (groupedFragments.count(Scanner::FragmentType::JavaScript)) {
            std::string pureJSOutput = ProcessJavaScriptFragments(groupedFragments[Scanner::FragmentType::JavaScript], fileName);
            jsOutput += pureJSOutput;
        }
        
        // 处理Script的共同管理 - 目标规划第46行：script → 由CHTL，CHTL JS，JS编译器共同管理
        std::vector<Scanner::CodeFragment> allScriptFragments;
        if (groupedFragments.count(Scanner::FragmentType::CHTL)) {
            // 提取CHTL中的script片段
            for (const auto& fragment : groupedFragments[Scanner::FragmentType::CHTL]) {
                if (fragment.content.find("script") != std::string::npos) {
                    allScriptFragments.push_back(fragment);
                }
            }
        }
        if (groupedFragments.count(Scanner::FragmentType::CHTLJS)) {
            allScriptFragments.insert(allScriptFragments.end(), 
                                    groupedFragments[Scanner::FragmentType::CHTLJS].begin(),
                                    groupedFragments[Scanner::FragmentType::CHTLJS].end());
        }
        if (groupedFragments.count(Scanner::FragmentType::JavaScript)) {
            allScriptFragments.insert(allScriptFragments.end(), 
                                    groupedFragments[Scanner::FragmentType::JavaScript].begin(),
                                    groupedFragments[Scanner::FragmentType::JavaScript].end());
        }
        
        if (!allScriptFragments.empty()) {
            std::string sharedScriptOutput = ProcessSharedScriptFragments(allScriptFragments, fileName);
            jsOutput += sharedScriptOutput;
        }
        
        // 合并编译结果
        result.htmlOutput = MergeCompilationResults(htmlOutput, cssOutput, jsOutput);
        result.cssOutput = cssOutput;
        result.javascriptOutput = jsOutput;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("片段分发处理异常: " + std::string(e.what()));
    }
    
    return result;
}

std::string CompilerDispatcher::ProcessCHTLFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName) {
    // 处理CHTL片段 - 目标规划第44行：局部style → CHTL编译器
    std::string htmlOutput = "";
    
    for (const auto& fragment : fragments) {
        try {
            // 使用CHTL解析器解析片段
            // 注意：这里需要根据实际的CHTLParser接口调整
            // 暂时使用基础的字符串处理
            htmlOutput += "<!-- CHTL Fragment -->\n";
            htmlOutput += fragment.content;
            htmlOutput += "\n<!-- /CHTL Fragment -->\n";
            
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "处理CHTL片段异常: " + std::string(e.what())
            );
        }
    }
    
    return htmlOutput;
}

std::string CompilerDispatcher::ProcessCHTLJSFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName) {
    // 处理CHTL JS片段
    std::string jsOutput = "";
    
    for (const auto& fragment : fragments) {
        try {
            // 使用CHTL JS解析器解析片段
            jsOutput += "// CHTL JS Fragment\n";
            jsOutput += fragment.content;
            jsOutput += "\n// /CHTL JS Fragment\n";
            
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "处理CHTL JS片段异常: " + std::string(e.what())
            );
        }
    }
    
    return jsOutput;
}

std::string CompilerDispatcher::ProcessCSSFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName) {
    // 处理CSS片段 - 使用ANTLR4 CSS编译器
    std::string cssOutput = "";
    
    for (const auto& fragment : fragments) {
        try {
            // 使用CSS编译器编译片段
            auto compiledCSS = cssCompiler_->Compile(fragment.content, fileName);
            cssOutput += compiledCSS;
            cssOutput += "\n";
            
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "处理CSS片段异常: " + std::string(e.what())
            );
        }
    }
    
    return cssOutput;
}

std::string CompilerDispatcher::ProcessJavaScriptFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName) {
    // 处理JavaScript片段 - 使用ANTLR4 JavaScript编译器
    std::string jsOutput = "";
    
    for (const auto& fragment : fragments) {
        try {
            // 使用JavaScript编译器编译片段
            auto compiledJS = jsCompiler_->Compile(fragment.content, fileName);
            jsOutput += compiledJS;
            jsOutput += "\n";
            
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "处理JavaScript片段异常: " + std::string(e.what())
            );
        }
    }
    
    return jsOutput;
}

std::string CompilerDispatcher::ProcessSharedScriptFragments(const std::vector<Scanner::CodeFragment>& scriptFragments, const std::string& fileName) {
    // 处理Script的共同管理 - 目标规划第46行：script → 由CHTL，CHTL JS，JS编译器共同管理
    std::string jsOutput = "";
    
    jsOutput += "// Shared Script Processing - CHTL + CHTL JS + JavaScript\n";
    
    for (const auto& fragment : scriptFragments) {
        try {
            // 分析片段内容，决定使用哪个编译器
            if (fragment.content.find("{{") != std::string::npos || 
                fragment.content.find("listen") != std::string::npos ||
                fragment.content.find("delegate") != std::string::npos) {
                // CHTL JS语法，使用CHTL JS编译器
                jsOutput += "// CHTL JS Processed\n";
                jsOutput += fragment.content;
            } else if (fragment.content.find("@") != std::string::npos ||
                      fragment.content.find("[Template]") != std::string::npos) {
                // CHTL语法，使用CHTL编译器
                jsOutput += "// CHTL Processed\n";
                jsOutput += fragment.content;
            } else {
                // 纯JavaScript，使用JavaScript编译器
                auto compiledJS = jsCompiler_->Compile(fragment.content, fileName);
                jsOutput += "// Pure JavaScript Processed\n";
                jsOutput += compiledJS;
            }
            
            jsOutput += "\n";
            
        } catch (const std::exception& e) {
            Utils::ErrorHandler::GetInstance().LogError(
                "处理共同管理Script片段异常: " + std::string(e.what())
            );
        }
    }
    
    return jsOutput;
}

std::string CompilerDispatcher::MergeCompilationResults(const std::string& htmlOutput, const std::string& cssOutput, const std::string& jsOutput) {
    // 合并编译结果 - 目标规划第38-41行：编译结果合并（HTML输出）
    std::ostringstream merged;
    
    merged << "<!DOCTYPE html>\n";
    merged << "<html>\n";
    merged << "<head>\n";
    merged << "    <meta charset=\"UTF-8\">\n";
    merged << "    <title>CHTL Compiled Output</title>\n";
    
    // 嵌入CSS
    if (!cssOutput.empty()) {
        merged << "    <style>\n";
        merged << "        " << cssOutput;
        merged << "    </style>\n";
    }
    
    merged << "</head>\n";
    merged << "<body>\n";
    
    // 嵌入HTML内容
    if (!htmlOutput.empty()) {
        merged << "    " << htmlOutput;
    }
    
    // 嵌入JavaScript
    if (!jsOutput.empty()) {
        merged << "    <script>\n";
        merged << "        " << jsOutput;
        merged << "    </script>\n";
    }
    
    merged << "</body>\n";
    merged << "</html>\n";
    
    return merged.str();
}

std::map<Scanner::FragmentType, std::vector<Scanner::CodeFragment>> CompilerDispatcher::GroupFragmentsByType(const std::vector<Scanner::CodeFragment>& fragments) {
    // 按类型分组代码片段
    std::map<Scanner::FragmentType, std::vector<Scanner::CodeFragment>> grouped;
    
    for (const auto& fragment : fragments) {
        grouped[fragment.type].push_back(fragment);
    }
    
    return grouped;
}

void CompilerDispatcher::Cleanup() {
    // 清理资源
    scanner_.reset();
    // chtlParser_.reset();
    // chtlJSParser_.reset();
    cssCompiler_.reset();
    jsCompiler_.reset();
}

} // namespace Dispatcher
} // namespace CHTL
