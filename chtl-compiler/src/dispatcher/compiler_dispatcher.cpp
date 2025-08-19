#include "dispatcher/compiler_dispatcher.h"
#include "merger/result_merger.h"
#include <sstream>
#include <algorithm>

namespace chtl {

CompilerDispatcher::CompilerDispatcher() 
    : chtlCompiler_(std::make_unique<CHTLCompiler>()),
      chtlJsCompiler_(std::make_unique<CHTLJSCompiler>()),
      cssCompiler_(std::make_unique<CSSCompiler>()),
      jsCompiler_(std::make_unique<JavaScriptCompiler>()) {
}

CompilerDispatcher::~CompilerDispatcher() = default;

CompileResult CompilerDispatcher::dispatch(std::vector<std::unique_ptr<CodeFragment>>& fragments) {
    CompileResult result;
    result.success = true;
    
    // 清理之前的结果
    htmlFragments_.clear();
    globalStyles_.clear();
    globalScripts_.clear();
    
    // 处理每个代码片段
    for (auto& fragment : fragments) {
        bool fragmentSuccess = false;
        
        switch (fragment->getType()) {
            case FragmentType::CHTL:
                fragmentSuccess = processCHTLFragment(
                    static_cast<CHTLFragment*>(fragment.get()), result);
                break;
                
            case FragmentType::CHTL_JS:
                fragmentSuccess = processCHTLJSFragment(
                    static_cast<CHTLJSFragment*>(fragment.get()), result);
                break;
                
            case FragmentType::CSS:
                fragmentSuccess = processCSSFragment(
                    static_cast<CSSFragment*>(fragment.get()), result);
                break;
                
            case FragmentType::JAVASCRIPT:
                fragmentSuccess = processJavaScriptFragment(
                    static_cast<JavaScriptFragment*>(fragment.get()), result);
                break;
                
            case FragmentType::LOCAL_STYLE:
                fragmentSuccess = processLocalStyleFragment(
                    static_cast<LocalStyleFragment*>(fragment.get()), result);
                break;
                
            case FragmentType::ORIGIN_HTML:
                // 原始HTML直接添加
                fragment->setCompiledResult(fragment->getContent());
                htmlFragments_.push_back(fragment->getContent());
                fragmentSuccess = true;
                break;
                
            case FragmentType::ORIGIN_CSS:
                // 原始CSS直接添加
                fragment->setCompiledResult(fragment->getContent());
                globalStyles_.push_back(fragment->getContent());
                fragmentSuccess = true;
                break;
                
            case FragmentType::ORIGIN_JS:
                // 原始JavaScript直接添加
                fragment->setCompiledResult(fragment->getContent());
                globalScripts_.push_back(fragment->getContent());
                fragmentSuccess = true;
                break;
                
            default:
                result.errors.push_back("Unknown fragment type: " + 
                    CodeFragment::fragmentTypeToString(fragment->getType()));
                fragmentSuccess = false;
                break;
        }
        
        if (!fragmentSuccess) {
            result.success = false;
        }
    }
    
    // 合并结果
    if (result.success) {
        mergeResults();
    }
    
    return result;
}

std::string CompilerDispatcher::getCompiledHTML() const {
    ResultMerger merger;
    
    // 添加HTML片段
    for (const auto& html : htmlFragments_) {
        merger.addHTMLFragment(html);
    }
    
    // 添加全局样式
    for (const auto& style : globalStyles_) {
        merger.addGlobalStyle(style);
    }
    
    // 添加全局脚本
    for (const auto& script : globalScripts_) {
        merger.addGlobalScript(script);
    }
    
    // 设置文档选项
    auto it = options_.find("title");
    if (it != options_.end()) {
        merger.setDocumentOption("title", it->second);
    }
    
    it = options_.find("lang");
    if (it != options_.end()) {
        merger.setDocumentOption("lang", it->second);
    }
    
    it = options_.find("charset");
    if (it != options_.end()) {
        merger.setDocumentOption("charset", it->second);
    }
    
    return merger.merge();
}

void CompilerDispatcher::reset() {
    chtlCompiler_->reset();
    chtlJsCompiler_->reset();
    cssCompiler_->reset();
    jsCompiler_->reset();
    
    htmlFragments_.clear();
    globalStyles_.clear();
    globalScripts_.clear();
    options_.clear();
}

void CompilerDispatcher::setOption(const std::string& key, const std::string& value) {
    options_[key] = value;
    
    // 传递选项到各个编译器
    chtlCompiler_->setOption(key, value);
    chtlJsCompiler_->setOption(key, value);
    cssCompiler_->setOption(key, value);
    jsCompiler_->setOption(key, value);
}

bool CompilerDispatcher::processCHTLFragment(CHTLFragment* fragment, CompileResult& result) {
    try {
        std::string compiledHTML = chtlCompiler_->compile(fragment->getContent());
        fragment->setCompiledResult(compiledHTML);
        htmlFragments_.push_back(compiledHTML);
        
        // 收集错误和警告
        for (const auto& error : chtlCompiler_->getErrors()) {
            result.errors.push_back("CHTL Compiler: " + error);
        }
        
        for (const auto& warning : chtlCompiler_->getWarnings()) {
            result.warnings.push_back("CHTL Compiler: " + warning);
        }
        
        return chtlCompiler_->getErrors().empty();
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL Compiler Exception: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processCHTLJSFragment(CHTLJSFragment* fragment, CompileResult& result) {
    try {
        std::string compiledJS = chtlJsCompiler_->compile(fragment->getContent());
        fragment->setCompiledResult(compiledJS);
        globalScripts_.push_back(compiledJS);
        
        // 收集错误和警告
        for (const auto& error : chtlJsCompiler_->getErrors()) {
            result.errors.push_back("CHTL JS Compiler: " + error);
        }
        
        for (const auto& warning : chtlJsCompiler_->getWarnings()) {
            result.warnings.push_back("CHTL JS Compiler: " + warning);
        }
        
        return chtlJsCompiler_->getErrors().empty();
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL JS Compiler Exception: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processCSSFragment(CSSFragment* fragment, CompileResult& result) {
    try {
        std::string compiledCSS = cssCompiler_->compile(fragment->getContent());
        fragment->setCompiledResult(compiledCSS);
        globalStyles_.push_back(compiledCSS);
        
        // 收集错误和警告
        for (const auto& error : cssCompiler_->getErrors()) {
            result.errors.push_back("CSS Compiler: " + error);
        }
        
        for (const auto& warning : cssCompiler_->getWarnings()) {
            result.warnings.push_back("CSS Compiler: " + warning);
        }
        
        return cssCompiler_->getErrors().empty();
    } catch (const std::exception& e) {
        result.errors.push_back("CSS Compiler Exception: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processJavaScriptFragment(JavaScriptFragment* fragment, CompileResult& result) {
    try {
        std::string compiledJS = jsCompiler_->compile(fragment->getContent());
        fragment->setCompiledResult(compiledJS);
        globalScripts_.push_back(compiledJS);
        
        // 收集错误和警告
        for (const auto& error : jsCompiler_->getErrors()) {
            result.errors.push_back("JavaScript Compiler: " + error);
        }
        
        for (const auto& warning : jsCompiler_->getWarnings()) {
            result.warnings.push_back("JavaScript Compiler: " + warning);
        }
        
        return jsCompiler_->getErrors().empty();
    } catch (const std::exception& e) {
        result.errors.push_back("JavaScript Compiler Exception: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processLocalStyleFragment(LocalStyleFragment* fragment, CompileResult& result) {
    try {
        // 局部样式由CHTL编译器处理
        std::string elementId = "element_" + std::to_string(htmlFragments_.size());
        auto [processedStyle, generatedClassName] = chtlCompiler_->compileLocalStyle(
            fragment->getContent(), elementId);
        
        fragment->setCompiledResult(processedStyle);
        
        // 局部样式会被提升到全局样式中
        globalStyles_.push_back(processedStyle);
        
        // 收集错误和警告
        for (const auto& error : chtlCompiler_->getErrors()) {
            result.errors.push_back("CHTL Compiler (Local Style): " + error);
        }
        
        for (const auto& warning : chtlCompiler_->getWarnings()) {
            result.warnings.push_back("CHTL Compiler (Local Style): " + warning);
        }
        
        return chtlCompiler_->getErrors().empty();
    } catch (const std::exception& e) {
        result.errors.push_back("CHTL Compiler Exception (Local Style): " + std::string(e.what()));
        return false;
    }
}

void CompilerDispatcher::mergeResults() {
    // 结果已经在处理过程中收集到相应的容器中
    // 这里可以进行额外的优化或处理
    
    // 例如：合并相同的样式规则，优化脚本等
}

} // namespace chtl