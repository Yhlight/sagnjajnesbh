#pragma once

#include "CHTLCommon.h"
#include "CHTLUnifiedScanner.h"

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JSCompiler;

/**
 * 编译器调度器
 * 负责协调不同类型的编译器处理相应的代码片段
 */
class CompilerDispatcher {
public:
    explicit CompilerDispatcher(const CompilerOptions& options = CompilerOptions{});
    ~CompilerDispatcher();
    
    // 编译源代码
    CompilerResult compile(const String& sourceCode, const String& filename = "");
    CompilerResult compileFile(const String& filename);
    
    // 设置编译选项
    void setOptions(const CompilerOptions& options);
    const CompilerOptions& getOptions() const { return options_; }
    
    // 获取编译统计信息
    struct CompileStats {
        size_t chtlFragments = 0;
        size_t chtlJSFragments = 0;
        size_t cssFragments = 0;
        size_t jsFragments = 0;
        size_t htmlFragments = 0;
        double compilationTime = 0.0;
    };
    
    const CompileStats& getStats() const { return stats_; }
    
private:
    CompilerOptions options_;
    CompileStats stats_;
    
    // 编译器实例
    std::unique_ptr<CHTLCompiler> chtlCompiler_;
    std::unique_ptr<CHTLJSCompiler> chtlJSCompiler_;
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JSCompiler> jsCompiler_;
    
    // 编译过程
    CompilerResult processFragments(const std::vector<CodeFragment>& fragments, const String& filename);
    String mergeResults(const std::vector<String>& results);
    
    // 编译器管理
    void initializeCompilers();
    void cleanupCompilers();
    
    // 结果处理
    struct FragmentResult {
        String html;
        String css;
        String javascript;
        std::vector<CompilerError> errors;
        std::vector<String> warnings;
    };
    
    FragmentResult compileFragment(const CodeFragment& fragment, const String& filename);
    
    // 错误处理
    void collectErrors(CompilerResult& result, const std::vector<CompilerError>& errors);
    void collectWarnings(CompilerResult& result, const std::vector<String>& warnings);
};

} // namespace CHTL