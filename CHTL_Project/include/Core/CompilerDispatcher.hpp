#pragma once

#include "CHTLTypes.hpp"
#include "CHTLUnifiedScanner.hpp"

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;

/**
 * @brief 编译器调度器
 * 
 * 负责协调CHTL、CHTL JS、CSS、JavaScript编译器的工作
 * 管理编译流程和结果合并
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    // 禁用拷贝构造和赋值
    CompilerDispatcher(const CompilerDispatcher&) = delete;
    CompilerDispatcher& operator=(const CompilerDispatcher&) = delete;

    /**
     * @brief 编译CHTL源代码
     * @param source 源代码
     * @param filename 文件名
     * @return 编译结果
     */
    CompilationResult Compile(const String& source, const String& filename = "");

    /**
     * @brief 设置编译选项
     * @param enableDebug 是否启用调试模式
     * @param enableOptimization 是否启用优化
     */
    void SetCompilationOptions(bool enableDebug = false, bool enableOptimization = true);

    /**
     * @brief 获取编译统计信息
     */
    struct CompilationStatistics {
        size_t totalFiles;
        size_t successfulCompilations;
        size_t failedCompilations;
        double averageCompileTime;
    };

    CompilationStatistics GetStatistics() const { return statistics_; }

private:
    // 编译阶段
    enum class CompilationPhase {
        Scanning,
        CHTLCompilation,
        CHTLJSCompilation,
        CSSCompilation,
        JavaScriptCompilation,
        ResultMerging,
        Complete
    };

    // 编译上下文
    struct CompilationContext {
        String sourceFilename;
        std::vector<CodeFragment> fragments;
        StringMap compilationResults;
        std::vector<CompilerError> errors;
        CompilationPhase currentPhase;
        
        CompilationContext() : currentPhase(CompilationPhase::Scanning) {}
    };

    // 内部方法
    bool ScanSource(CompilationContext& context, const String& source);
    bool CompileCHTLFragments(CompilationContext& context);
    bool CompileCHTLJSFragments(CompilationContext& context);
    bool CompileCSSFragments(CompilationContext& context);
    bool CompileJavaScriptFragments(CompilationContext& context);
    String MergeResults(const CompilationContext& context);
    void UpdateStatistics(bool success, double compileTime);

    // 编译器实例
    UniquePtr<CHTLUnifiedScanner> scanner_;
    // UniquePtr<CHTLCompiler> chtlCompiler_;           // 暂时注释
    // UniquePtr<CHTLJSCompiler> chtljsCompiler_;       // 暂时注释
    // UniquePtr<CSSCompiler> cssCompiler_;             // 暂时注释
    // UniquePtr<JavaScriptCompiler> jsCompiler_;       // 暂时注释

    // 编译选项
    bool debugMode_;
    bool optimizationEnabled_;

    // 统计信息
    CompilationStatistics statistics_;
};

} // namespace CHTL