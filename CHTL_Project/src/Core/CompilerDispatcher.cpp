#include "Core/CompilerDispatcher.hpp"
#include "Core/CHTLUnifiedScanner.hpp"
#include <chrono>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode_(false), optimizationEnabled_(true) {
    
    scanner_ = std::make_unique<CHTLUnifiedScanner>();
    statistics_ = {};
    
    // 暂时不初始化其他编译器，直到我们实现它们
    // chtlCompiler_ = std::make_unique<CHTLCompiler>();
    // chtljsCompiler_ = std::make_unique<CHTLJSCompiler>();
    // cssCompiler_ = std::make_unique<CSSCompiler>();
    // jsCompiler_ = std::make_unique<JavaScriptCompiler>();
}

CompilerDispatcher::~CompilerDispatcher() = default;

CompilationResult CompilerDispatcher::Compile(const String& source, const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilationContext context;
    context.sourceFilename = filename;
    
    CompilationResult result;
    
    try {
        // 第一阶段：扫描源代码
        if (!ScanSource(context, source)) {
            result.success = false;
            result.errors = context.errors;
            return result;
        }
        
        // 第二阶段：编译CHTL片段
        if (!CompileCHTLFragments(context)) {
            result.success = false;
            result.errors = context.errors;
            return result;
        }
        
        // 第三阶段：编译CSS片段（如果有）
        if (!CompileCSSFragments(context)) {
            result.success = false;
            result.errors = context.errors;
            return result;
        }
        
        // 第四阶段：编译JavaScript片段（如果有）
        if (!CompileJavaScriptFragments(context)) {
            result.success = false;
            result.errors = context.errors;
            return result;
        }
        
        // 第五阶段：合并结果
        result.output = MergeResults(context);
        result.success = true;
        
    } catch (const std::exception& e) {
        context.errors.emplace_back(ErrorLevel::Fatal, e.what(), filename);
        result.success = false;
        result.errors = context.errors;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    UpdateStatistics(result.success, static_cast<double>(duration.count()));
    
    return result;
}

void CompilerDispatcher::SetCompilationOptions(bool enableDebug, bool enableOptimization) {
    debugMode_ = enableDebug;
    optimizationEnabled_ = enableOptimization;
    
    if (scanner_) {
        scanner_->SetOptions(enableDebug);
    }
}

bool CompilerDispatcher::ScanSource(CompilationContext& context, const String& source) {
    context.currentPhase = CompilationPhase::Scanning;
    
    try {
        context.fragments = scanner_->ScanSource(source, context.sourceFilename);
        
        if (debugMode_) {
            auto stats = scanner_->GetStatistics();
            std::cout << "[CompilerDispatcher] 扫描统计: 总片段=" << stats.totalFragments 
                     << ", CHTL=" << stats.chtlFragments 
                     << ", CSS=" << stats.cssFragments 
                     << ", JS=" << stats.jsFragments << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        context.errors.emplace_back(ErrorLevel::Error, "扫描失败: " + String(e.what()), context.sourceFilename);
        return false;
    }
}

bool CompilerDispatcher::CompileCHTLFragments(CompilationContext& context) {
    context.currentPhase = CompilationPhase::CHTLCompilation;
    
    // 暂时的占位实现
    for (const auto& fragment : context.fragments) {
        if (fragment.type == FragmentType::CHTL) {
            // 目前直接将CHTL片段作为HTML输出（占位实现）
            context.compilationResults["chtl"] += fragment.content;
        }
    }
    
    return true;
}

bool CompilerDispatcher::CompileCHTLJSFragments(CompilationContext& context) {
    context.currentPhase = CompilationPhase::CHTLJSCompilation;
    
    // 暂时跳过CHTL JS编译
    return true;
}

bool CompilerDispatcher::CompileCSSFragments(CompilationContext& context) {
    context.currentPhase = CompilationPhase::CSSCompilation;
    
    // 暂时的占位实现
    for (const auto& fragment : context.fragments) {
        if (fragment.type == FragmentType::CSS) {
            context.compilationResults["css"] += fragment.content;
        }
    }
    
    return true;
}

bool CompilerDispatcher::CompileJavaScriptFragments(CompilationContext& context) {
    context.currentPhase = CompilationPhase::JavaScriptCompilation;
    
    // 暂时的占位实现
    for (const auto& fragment : context.fragments) {
        if (fragment.type == FragmentType::JavaScript) {
            context.compilationResults["js"] += fragment.content;
        }
    }
    
    return true;
}

String CompilerDispatcher::MergeResults(const CompilationContext& context) {
    String output = "<!DOCTYPE html>\n<html>\n<head>\n";
    
    // 添加CSS
    auto cssIt = context.compilationResults.find("css");
    if (cssIt != context.compilationResults.end() && !cssIt->second.empty()) {
        output += "<style>\n" + cssIt->second + "\n</style>\n";
    }
    
    output += "</head>\n<body>\n";
    
    // 添加HTML内容
    auto chtlIt = context.compilationResults.find("chtl");
    if (chtlIt != context.compilationResults.end() && !chtlIt->second.empty()) {
        output += chtlIt->second;
    }
    
    // 添加JavaScript
    auto jsIt = context.compilationResults.find("js");
    if (jsIt != context.compilationResults.end() && !jsIt->second.empty()) {
        output += "<script>\n" + jsIt->second + "\n</script>\n";
    }
    
    output += "</body>\n</html>";
    
    return output;
}

void CompilerDispatcher::UpdateStatistics(bool success, double compileTime) {
    statistics_.totalFiles++;
    
    if (success) {
        statistics_.successfulCompilations++;
    } else {
        statistics_.failedCompilations++;
    }
    
    // 更新平均编译时间
    statistics_.averageCompileTime = 
        (statistics_.averageCompileTime * (statistics_.totalFiles - 1) + compileTime) / statistics_.totalFiles;
}

} // namespace CHTL