#pragma once
#include "ContextManager.h"
#include "Lexer.h"
#include "../parser/Parser.h"
#include <chrono>
#include "../ast/ASTNode.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace chtl {

// 编译结果
struct CompilationResult {
    bool success;
    std::unique_ptr<ast::ProgramNode> ast;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string generated_code;
    double compilation_time_ms;
    
    CompilationResult() : success(false), compilation_time_ms(0.0) {}
};

// 编译选项
struct CompilationOptions {
    bool enable_optimization;
    bool enable_debug_info;
    bool strict_mode;
    bool enable_warnings;
    std::string output_format;
    std::string target_directory;
    
    CompilationOptions() 
        : enable_optimization(false), enable_debug_info(false),
          strict_mode(true), enable_warnings(true),
          output_format("html") {}
};

// 编译进度回调
using CompilationProgressCallback = std::function<void(CompilationPhase, double)>;

// 编译器核心 - 协调所有编译组件
class CompilerCore {
public:
    CompilerCore();
    ~CompilerCore() = default;
    
    // 基础编译接口
    CompilationResult compileFromString(const std::string& source_code, 
                                      const CompilationOptions& options = CompilationOptions());
    CompilationResult compileFromFile(const std::string& file_path,
                                    const CompilationOptions& options = CompilationOptions());
    CompilationResult compileFromFiles(const std::vector<std::string>& file_paths,
                                     const CompilationOptions& options = CompilationOptions());
    
    // 分步编译接口（用于调试和精确控制）
    bool initializeCompilation(const CompilationOptions& options = CompilationOptions());
    bool lexicalAnalysis(const std::string& input, const std::string& file_path = "");
    bool syntaxAnalysis();
    bool semanticAnalysis();
    bool optimization();
    bool codeGeneration();
    bool finalize();
    
    // 获取编译结果
    CompilationResult getCompilationResult();
    
    // 状态查询
    CompilationPhase getCurrentPhase() const;
    bool isCompiling() const;
    double getProgress() const;
    
    // 组件访问
    ContextManager& getContextManager() { return context_manager_; }
    const ContextManager& getContextManager() const { return context_manager_; }
    
    // 错误和警告管理
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // 回调管理
    void setProgressCallback(CompilationProgressCallback callback);
    void removeProgressCallback();
    
    // AST操作
    ast::ProgramNode* getAST() const { return current_ast_.get(); }
    void setAST(std::unique_ptr<ast::ProgramNode> ast);
    
    // 调试和诊断
    std::string getCompilationStatistics() const;
    void printCompilationTrace() const;
    void dumpAST() const;
    void dumpStates() const;
    
    // 重置和清理
    void reset();
    void clear();
    
private:
    ContextManager context_manager_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<parser::CHTLParser> parser_;
    // std::unique_ptr<generator::Generator> generator_;  // 待实现
    
    // 编译状态
    std::unique_ptr<ast::ProgramNode> current_ast_;
    CompilationOptions current_options_;
    CompilationProgressCallback progress_callback_;
    std::chrono::steady_clock::time_point compilation_start_time_;
    bool is_compiling_;
    double current_progress_;
    
    // 编译结果缓存
    std::string generated_code_;
    std::vector<std::string> compilation_errors_;
    std::vector<std::string> compilation_warnings_;
    
    // 内部辅助方法
    bool validateInput(const std::string& input) const;
    bool validateOptions(const CompilationOptions& options) const;
    void updateProgress(CompilationPhase phase);
    void reportProgress(CompilationPhase phase, double progress);
    
    // 错误处理
    void handleCompilationError(const std::string& error_message, CompilationPhase phase);
    void handleCompilationWarning(const std::string& warning_message, CompilationPhase phase);
    
    // 阶段间的状态同步
    void syncStatesBetweenPhases();
    bool validatePhaseTransition(CompilationPhase from, CompilationPhase to) const;
    
    // 组件初始化和清理
    void initializeComponents();
    void cleanupComponents();
    
    // 编译统计
    void collectCompilationStatistics();
    
    // 状态一致性检查
    bool validateStateConsistency() const;

    // 语义分析方法
    bool validateCHTLSemantics(ast::ASTNode* node);
    bool buildSymbolTable(ast::ASTNode* node);
    bool performSemanticValidation(ast::ASTNode* node);
    bool optimizeAST(ast::ASTNode* node);
    
    // 具体语义验证方法
    bool validateTemplateSemantics(ast::ASTNode* node);
    bool validateCustomSemantics(ast::ASTNode* node);
    bool validateStyleBlockSemantics(ast::ASTNode* node);
    bool validateScriptBlockSemantics(ast::ASTNode* node);
    bool validateImportSemantics(ast::ASTNode* node);
    bool validateNamespaceSemantics(ast::ASTNode* node);
};

// 编译器工厂 - 创建预配置的编译器实例
class CompilerFactory {
public:
    // 创建标准编译器
    static std::unique_ptr<CompilerCore> createStandardCompiler();
    
    // 创建调试编译器
    static std::unique_ptr<CompilerCore> createDebugCompiler();
    
    // 创建优化编译器
    static std::unique_ptr<CompilerCore> createOptimizedCompiler();
    
    // 创建自定义编译器
    static std::unique_ptr<CompilerCore> createCustomCompiler(const CompilationOptions& options);
};

// 便利函数
namespace compiler_utils {
    // 快速编译
    CompilationResult quickCompile(const std::string& source_code);
    CompilationResult quickCompileFile(const std::string& file_path);
    
    // 验证CHTL语法
    bool validateSyntax(const std::string& source_code);
    bool validateFile(const std::string& file_path);
    
    // 格式化CHTL代码
    std::string formatCode(const std::string& source_code);
    
    // AST转换
    std::string astToString(const ast::ASTNode* node);
    std::string astToJson(const ast::ASTNode* node);
}

} // namespace chtl
