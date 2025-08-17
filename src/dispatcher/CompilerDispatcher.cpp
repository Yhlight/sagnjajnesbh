#include "../../include/CompilerDispatcher.h"
#include <fstream>
#include <chrono>

namespace CHTL {

// 临时前向声明的编译器类
class CHTLCompiler {
public:
    String compile(const String& code) { return "<!-- CHTL: " + code + " -->"; }
};

class CHTLJSCompiler {
public:
    String compile(const String& code) { return "/* CHTL JS: " + code + " */"; }
};

class CSSCompiler {
public:
    String compile(const String& code) { return "/* CSS: " + code + " */"; }
};

class JSCompiler {
public:
    String compile(const String& code) { return "/* JS: " + code + " */"; }
};

CompilerDispatcher::CompilerDispatcher(const CompilerOptions& options)
    : options_(options) {
    initializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() {
    cleanupCompilers();
}

CompilerResult CompilerDispatcher::compile(const String& sourceCode, const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 重置统计信息
    stats_ = CompileStats{};
    
    // 扫描代码片段
    CHTLUnifiedScanner scanner(sourceCode);
    std::vector<CodeFragment> fragments = scanner.scanAllFragments();
    
    // 检查扫描错误
    CompilerResult result;
    const auto& scanErrors = scanner.getErrors();
    if (!scanErrors.empty()) {
        result.success = false;
        result.errors = scanErrors;
        return result;
    }
    
    // 处理片段
    result = processFragments(fragments, filename);
    
    // 计算编译时间
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    stats_.compilationTime = duration.count() / 1000.0; // 转换为毫秒
    
    return result;
}

CompilerResult CompilerDispatcher::compileFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        CompilerResult result;
        result.success = false;
        result.errors.emplace_back("无法打开文件: " + filename, filename);
        return result;
    }
    
    String sourceCode((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    
    return compile(sourceCode, filename);
}

void CompilerDispatcher::setOptions(const CompilerOptions& options) {
    options_ = options;
}

CompilerResult CompilerDispatcher::processFragments(const std::vector<CodeFragment>& fragments, const String& filename) {
    CompilerResult result;
    result.success = true;
    
    std::vector<String> htmlParts;
    std::vector<String> cssParts;
    std::vector<String> jsParts;
    
    for (const auto& fragment : fragments) {
        FragmentResult fragResult = compileFragment(fragment, filename);
        
        // 收集结果
        if (!fragResult.html.empty()) {
            htmlParts.push_back(fragResult.html);
        }
        if (!fragResult.css.empty()) {
            cssParts.push_back(fragResult.css);
        }
        if (!fragResult.javascript.empty()) {
            jsParts.push_back(fragResult.javascript);
        }
        
        // 收集错误和警告
        collectErrors(result, fragResult.errors);
        collectWarnings(result, fragResult.warnings);
        
        // 更新统计信息
        switch (fragment.type) {
            case FragmentType::CHTL:
                stats_.chtlFragments++;
                break;
            case FragmentType::CHTL_JS:
                stats_.chtlJSFragments++;
                break;
            case FragmentType::CSS:
                stats_.cssFragments++;
                break;
            case FragmentType::JAVASCRIPT:
                stats_.jsFragments++;
                break;
            case FragmentType::HTML:
                stats_.htmlFragments++;
                break;
            default:
                break;
        }
    }
    
    if (!result.errors.empty()) {
        result.success = false;
        return result;
    }
    
    // 合并结果为HTML
    String html = "<!DOCTYPE html>\n<html>\n<head>\n";
    if (!cssParts.empty()) {
        html += "<style>\n";
        for (const auto& css : cssParts) {
            html += css + "\n";
        }
        html += "</style>\n";
    }
    html += "</head>\n<body>\n";
    
    for (const auto& htmlPart : htmlParts) {
        html += htmlPart + "\n";
    }
    
    if (!jsParts.empty()) {
        html += "<script>\n";
        for (const auto& js : jsParts) {
            html += js + "\n";
        }
        html += "</script>\n";
    }
    
    html += "</body>\n</html>";
    result.output = html;
    
    return result;
}

CompilerDispatcher::FragmentResult CompilerDispatcher::compileFragment(const CodeFragment& fragment, const String& filename) {
    FragmentResult result;
    
    try {
        switch (fragment.type) {
            case FragmentType::CHTL:
                result.html = chtlCompiler_->compile(fragment.content);
                break;
            case FragmentType::CHTL_JS:
                result.javascript = chtlJSCompiler_->compile(fragment.content);
                break;
            case FragmentType::CSS:
                result.css = cssCompiler_->compile(fragment.content);
                break;
            case FragmentType::JAVASCRIPT:
                result.javascript = jsCompiler_->compile(fragment.content);
                break;
            case FragmentType::HTML:
                result.html = fragment.content; // HTML直接输出
                break;
            default:
                result.warnings.push_back("未知片段类型，跳过处理");
                break;
        }
    } catch (const std::exception& e) {
        result.errors.emplace_back("编译片段时出错: " + String(e.what()), filename, 
                                   fragment.startLine, fragment.startColumn);
    }
    
    return result;
}

void CompilerDispatcher::initializeCompilers() {
    chtlCompiler_ = std::make_unique<CHTLCompiler>();
    chtlJSCompiler_ = std::make_unique<CHTLJSCompiler>();
    cssCompiler_ = std::make_unique<CSSCompiler>();
    jsCompiler_ = std::make_unique<JSCompiler>();
}

void CompilerDispatcher::cleanupCompilers() {
    chtlCompiler_.reset();
    chtlJSCompiler_.reset();
    cssCompiler_.reset();
    jsCompiler_.reset();
}

void CompilerDispatcher::collectErrors(CompilerResult& result, const std::vector<CompilerError>& errors) {
    result.errors.insert(result.errors.end(), errors.begin(), errors.end());
}

void CompilerDispatcher::collectWarnings(CompilerResult& result, const std::vector<String>& warnings) {
    result.warnings.insert(result.warnings.end(), warnings.begin(), warnings.end());
}

} // namespace CHTL