#include <chrono>
#pragma once
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../Constraints/SyntaxConstrainer.h"
#include "../../parser/Parser.h"
#include "../../generator/HTMLGenerator.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {

// 编译器类型
enum class CompilerType {
    CHTL_COMPILER,      // CHTL编译器（手写）
    CHTL_JS_COMPILER,   // CHTL JS编译器（手写）
    CSS_COMPILER,       // CSS编译器（ANTLR）
    JAVASCRIPT_COMPILER // JavaScript编译器（ANTLR）
};

// 编译结果
struct CompilationFragment {
    std::string compiled_code;      // 编译后的代码
    FragmentType original_type;     // 原始片段类型
    CompilerType compiler_used;     // 使用的编译器
    bool compilation_success;       // 编译是否成功
    std::vector<std::string> errors; // 编译错误
    std::vector<std::string> warnings; // 编译警告
};

// 编译器调度器配置
struct DispatcherConfig {
    bool enable_syntax_constraints;    // 启用语法约束检查
    bool enable_fragment_optimization; // 启用片段优化
    bool enable_parallel_compilation;  // 启用并行编译
    bool strict_mode;                  // 严格模式
    
    DispatcherConfig()
        : enable_syntax_constraints(true), enable_fragment_optimization(true),
          enable_parallel_compilation(false), strict_mode(true) {}
};

// 编译器调度器
class CompilerDispatcher {
public:
    CompilerDispatcher(const DispatcherConfig& config = DispatcherConfig());
    ~CompilerDispatcher();
    
    // 主编译接口
    std::string compileSource(const std::string& source_code);
    std::vector<CompilationFragment> compileFragments(const std::vector<CodeSlice>& slices);
    
    // 分步编译流程
    std::vector<CodeSlice> scanAndSlice(const std::string& source_code);
    std::vector<ConstraintViolation> validateSyntaxConstraints(const std::vector<CodeSlice>& slices);
    std::vector<CompilationFragment> dispatchToCompilers(const std::vector<CodeSlice>& slices);
    std::string mergeCompilationResults(const std::vector<CompilationFragment>& fragments);
    
    // 特定编译器调用
    CompilationFragment compileCHTLFragment(const CodeSlice& slice);
    CompilationFragment compileCHTLJSFragment(const CodeSlice& slice);
    CompilationFragment compileCSSFragment(const CodeSlice& slice);
    CompilationFragment compileJavaScriptFragment(const CodeSlice& slice);
    
    // 编译器管理
    void setCompilerPath(CompilerType type, const std::string& path);
    std::string getCompilerPath(CompilerType type) const;
    bool isCompilerAvailable(CompilerType type) const;
    
    // 语法约束集成
    void enableSyntaxConstraints(bool enable);
    std::vector<ConstraintViolation> checkConstraints(const std::vector<CodeSlice>& slices);
    void reportConstraintViolations(const std::vector<ConstraintViolation>& violations);
    
    // 优化和缓存
    void enableFragmentCaching(bool enable);
    bool isCached(const std::string& fragment_hash);
    void cacheCompilationResult(const std::string& fragment_hash, const CompilationFragment& result);
    
    // 错误处理和报告
    struct CompilerError {
        CompilerType compiler;
        std::string fragment_content;
        std::string error_message;
        size_t line;
        size_t column;
    };
    
    std::vector<CompilerError> getCompilerErrors() const;
    std::vector<std::string> getCompilerWarnings() const;
    void clearErrors();
    
    // 统计和调试
    struct DispatcherStatistics {
        size_t total_fragments;
        size_t chtl_fragments;
        size_t chtl_js_fragments;
        size_t css_fragments;
        size_t js_fragments;
        size_t successful_compilations;
        size_t failed_compilations;
        double compilation_time_ms;
        size_t cache_hits;
        size_t cache_misses;
    };
    
    DispatcherStatistics getStatistics() const;
    void resetStatistics();
    void printDispatcherInfo() const;
    
    // 配置管理
    void setConfig(const DispatcherConfig& config);
    DispatcherConfig getConfig() const;

private:
    DispatcherConfig config_;
    std::unique_ptr<CHTLUnifiedScanner> scanner_;
    std::unique_ptr<SyntaxConstrainer> constrainer_;
    std::unique_ptr<parser::CHTLParser> chtl_parser_;
    std::unique_ptr<generator::HTMLGenerator> html_generator_;
    
    // 编译器路径
    std::unordered_map<CompilerType, std::string> compiler_paths_;
    
    // 缓存系统
    std::unordered_map<std::string, CompilationFragment> compilation_cache_;
    bool cache_enabled_;
    
    // 统计信息
    mutable DispatcherStatistics stats_;
    std::vector<CompilerError> errors_;
    std::vector<std::string> warnings_;
    
    // 初始化
    void initializeCompilers();
    void initializeDefaultCompilerPaths();
    
    // 编译辅助
    std::string generateFragmentHash(const CodeSlice& slice);
    CompilerType selectCompiler(FragmentType fragment_type);
    
    // 外部编译器调用（ANTLR）
    bool callExternalCompiler(CompilerType type, const std::string& input, std::string& output);
    std::string buildCompilerCommand(CompilerType type, const std::string& input_file, const std::string& output_file);
    
    // 结果合并策略
    std::string mergeHTMLResults(const std::vector<CompilationFragment>& fragments);
    std::string mergeCSSResults(const std::vector<CompilationFragment>& fragments);
    std::string mergeJavaScriptResults(const std::vector<CompilationFragment>& fragments);
    
    // 错误处理
    void addError(CompilerType compiler, const std::string& fragment, const std::string& error, size_t line = 0, size_t column = 0);
    void addWarning(const std::string& warning);
    
    // 性能监控
    void startTimer();
    void endTimer();
    std::chrono::steady_clock::time_point start_time_;
};

// 调度器工具函数
namespace DispatcherUtils {
    // 编译器类型转换
    std::string compilerTypeToString(CompilerType type);
    std::string fragmentTypeToString(FragmentType type);
    
    // 文件操作
    bool writeToTempFile(const std::string& content, std::string& temp_file_path);
    bool readFromFile(const std::string& file_path, std::string& content);
    void cleanupTempFile(const std::string& file_path);
    
    // 哈希计算
    std::string calculateHash(const std::string& content);
    
    // 命令执行
    bool executeCommand(const std::string& command, std::string& output, std::string& error);
    
    // 结果验证
    bool validateCompilationResult(const std::string& result, FragmentType original_type);
}

} // namespace chtl