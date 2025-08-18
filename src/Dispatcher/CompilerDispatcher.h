#pragma once
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLCompiler/CHTLCompilerCore.h"
#include "../CHTLJSCompiler/CHTLJSCompilerCore.h"
#include "../CSSCompiler/CSSCompilerCore.h"
#include "../JavaScriptCompiler/JavaScriptCompilerCore.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace chtl {

/**
 * 编译器调度器
 * 根据统一扫描器的结果，将代码片段分发给对应的编译器
 */
class CompilerDispatcher {
public:
    struct CompilationResult {
        std::string compiledCode;           // 编译后的代码
        bool success = false;               // 编译是否成功
        std::vector<std::string> errors;    // 错误信息
        std::vector<std::string> warnings;  // 警告信息
        std::string sourceMap;              // 源码映射
    };
    
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // === 初始化 ===
    
    /**
     * 初始化调度器和所有编译器
     */
    bool initialize();
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool enabled);
    
    // === 主编译接口 ===
    
    /**
     * 编译CHTL代码
     * 使用统一扫描器分析代码，然后分发给对应编译器
     */
    CompilationResult compileCode(const std::string& code);
    
    /**
     * 编译代码片段
     * 直接编译扫描器产生的代码片段
     */
    CompilationResult compileFragment(const scanner::CHTLUnifiedScanner::CodeFragment& fragment);
    
    // === 编译器管理 ===
    
    /**
     * 获取CHTL编译器
     */
    CHTLCompilerCore* getCHTLCompiler();
    
    /**
     * 获取CHTL JS编译器
     */
    CHTLJSCompilerCore* getCHTLJSCompiler();
    
    /**
     * 获取CSS编译器
     */
    CSSCompilerCore* getCSSCompiler();
    
    /**
     * 获取JavaScript编译器
     */
    JavaScriptCompilerCore* getJavaScriptCompiler();
    
    // === 统一扫描器接口 ===
    
    /**
     * 获取统一扫描器
     */
    scanner::CHTLUnifiedScanner* getScanner();
    
    /**
     * 扫描代码并返回片段
     */
    std::vector<scanner::CHTLUnifiedScanner::CodeFragment> scanCode(const std::string& code);
    
    // === 智能调度 ===
    
    /**
     * 根据代码片段上下文选择合适的编译器
     */
    std::string selectCompilerForFragment(const scanner::CHTLUnifiedScanner::CodeFragment& fragment);
    
    /**
     * 验证编译器与片段的兼容性
     */
    bool isCompilerCompatible(const std::string& compilerName, const scanner::CHTLUnifiedScanner::CodeFragment& fragment);
    
    // === 编译优化 ===
    
    /**
     * 并行编译代码片段
     */
    std::vector<CompilationResult> compileFragmentsParallel(const std::vector<scanner::CHTLUnifiedScanner::CodeFragment>& fragments);
    
    /**
     * 合并编译结果
     */
    CompilationResult mergeCompilationResults(const std::vector<CompilationResult>& results);
    
    // === 错误处理 ===
    
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void clearErrors();

private:
    // 编译器实例
    std::unique_ptr<CHTLCompilerCore> chtlCompiler_;
    std::unique_ptr<CHTLJSCompilerCore> chtljsCompiler_;
    std::unique_ptr<CSSCompilerCore> cssCompiler_;
    std::unique_ptr<JavaScriptCompilerCore> jsCompiler_;
    
    // 统一扫描器
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    
    // 配置
    bool debugMode_ = false;
    bool initialized_ = false;
    
    // 错误处理
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 编译器映射
    std::unordered_map<scanner::CHTLUnifiedScanner::CodeContext, std::string> contextToCompilerMap_;
    
    void initializeContextMapping();
    void addError(const std::string& message);
    void addWarning(const std::string& message);
};

} // namespace chtl