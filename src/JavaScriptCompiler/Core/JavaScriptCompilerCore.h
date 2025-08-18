#pragma once
#include "../Parser/JavaScriptParser.h"
#include "../Parser/JavaScriptLexer.h"
#include "../Parser/JavaScriptParserBaseListener.h"
#include "antlr4-runtime.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace javascript {

/**
 * JavaScript编译器核心 - 使用ANTLR4实现原生JavaScript解析和编译
 */
class JavaScriptCompilerCore {
public:
    // 友元声明
    friend class JavaScriptTreeWalker;
    friend class JavaScriptErrorListener;

    JavaScriptCompilerCore();
    ~JavaScriptCompilerCore() = default;
    
    // 主编译接口
    std::string compileJavaScript(const std::string& js_code);
    std::string compileJavaScriptFile(const std::string& file_path);
    
    // 解析接口
    std::unique_ptr<JavaScriptParser::ProgramContext> parseJavaScript(const std::string& js_code);
    bool validateJavaScript(const std::string& js_code);
    
    // 优化选项
    void enableMinification(bool enable) { minify_output_ = enable; }
    void enableSourceMap(bool enable) { generate_source_map_ = enable; }
    void enableESModules(bool enable) { es_modules_enabled_ = enable; }
    void setTargetECMAScript(const std::string& version) { target_ecma_version_ = version; }
    void setTargetEnvironment(const std::string& env) { target_environment_ = env; } // "browser", "node", "universal"
    
    // 代码转换选项
    void enableBabelTransform(bool enable) { babel_transform_ = enable; }
    void enableTypeScript(bool enable) { typescript_support_ = enable; }
    void enableJSX(bool enable) { jsx_support_ = enable; }
    
    // 错误处理
    bool hasErrors() const { return !errors_.empty(); }
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
    // 统计信息
    struct CompilationStats {
        size_t functions_processed;
        size_t classes_processed;
        size_t variables_processed;
        size_t statements_processed;
        size_t bytes_input;
        size_t bytes_output;
        double compression_ratio;
        double compilation_time_ms;
        std::string target_version;
        
        CompilationStats() : functions_processed(0), classes_processed(0), 
                           variables_processed(0), statements_processed(0),
                           bytes_input(0), bytes_output(0), compression_ratio(0.0), 
                           compilation_time_ms(0.0) {}
    };
    
    CompilationStats getLastStats() const { return last_stats_; }
    void printCompilationInfo() const;
    
    // 高级功能
    std::string generateSourceMap() const;
    std::vector<std::string> extractDependencies(const std::string& js_code);
    std::string bundleModules(const std::vector<std::string>& module_paths);
    
private:
    bool minify_output_;
    bool generate_source_map_;
    bool es_modules_enabled_;
    bool babel_transform_;
    bool typescript_support_;
    bool jsx_support_;
    std::string target_ecma_version_;
    std::string target_environment_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    CompilationStats last_stats_;
    
    // 源映射信息
    struct SourceMapEntry {
        size_t original_line;
        size_t original_column;
        size_t generated_line;
        size_t generated_column;
        std::string source_file;
    };
    std::vector<SourceMapEntry> source_map_entries_;
    
    // 内部处理方法
    std::string processProgram(JavaScriptParser::ProgramContext* ctx);
    std::string processStatement(JavaScriptParser::StatementContext* ctx);
    std::string processDeclaration(JavaScriptParser::DeclarationContext* ctx);
    std::string processExpression(JavaScriptParser::SingleExpressionContext* ctx);
    
    // 优化方法
    std::string minifyJavaScript(const std::string& js);
    std::string transformToES5(const std::string& js);
    std::string optimizeCode(const std::string& js);
    std::string removeDeadCode(const std::string& js);
    
    // 模块处理
    std::string processImports(const std::string& js);
    std::string processExports(const std::string& js);
    std::string resolveModulePaths(const std::string& js);
    
    // 错误收集
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 统计更新
    void updateStats(const std::string& input, const std::string& output, double time_ms);
    
    // 源映射
    void addSourceMapEntry(size_t orig_line, size_t orig_col, size_t gen_line, size_t gen_col, const std::string& source = "");
};

/**
 * JavaScript错误监听器 - 收集解析错误
 */
class JavaScriptErrorListener : public antlr4::BaseErrorListener {
public:
    JavaScriptErrorListener(JavaScriptCompilerCore* compiler) : compiler_(compiler) {}
    
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
                    size_t line, size_t charPositionInLine, const std::string &msg,
                    std::exception_ptr e) override;
    
private:
    JavaScriptCompilerCore* compiler_;
};

/**
 * JavaScript树遍历器 - 处理解析树并生成优化的JavaScript
 */
class JavaScriptTreeWalker : public JavaScriptParserBaseListener {
public:
    JavaScriptTreeWalker(JavaScriptCompilerCore* compiler) : compiler_(compiler) {}
    
    // 重写监听器方法
    void enterProgram(JavaScriptParser::ProgramContext *ctx) override;
    void exitProgram(JavaScriptParser::ProgramContext *ctx) override;
    
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override;
    void exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override;
    
    void enterClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) override;
    void exitClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) override;
    
    void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override;
    void exitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override;
    
    void enterExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) override;
    void exitExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) override;
    
    // 获取生成的JavaScript
    std::string getGeneratedJavaScript() const { return generated_js_; }
    void reset() { generated_js_.clear(); }
    
    // 设置生成选项
    void setMinify(bool minify) { minify_ = minify; }
    void setTargetVersion(const std::string& version) { target_version_ = version; }
    
private:
    JavaScriptCompilerCore* compiler_;
    std::string generated_js_;
    bool minify_;
    std::string target_version_;
    
    // 辅助方法
    std::string extractText(antlr4::tree::ParseTree* ctx);
    void appendJS(const std::string& js);
    std::string processNode(antlr4::tree::ParseTree* ctx);
    
    // 代码生成
    std::string generateFunction(JavaScriptParser::FunctionDeclarationContext* ctx);
    std::string generateClass(JavaScriptParser::ClassDeclarationContext* ctx);
    std::string generateVariable(JavaScriptParser::VariableDeclarationContext* ctx);
};

/**
 * JavaScript代码优化器 - 高级优化功能
 */
class JavaScriptOptimizer {
public:
    JavaScriptOptimizer(JavaScriptCompilerCore* compiler) : compiler_(compiler) {}
    
    // 优化方法
    std::string optimizeCode(const std::string& js_code);
    std::string removeUnusedCode(const std::string& js_code);
    std::string inlineFunctions(const std::string& js_code);
    std::string constantFolding(const std::string& js_code);
    std::string minimizeVariableNames(const std::string& js_code);
    
    // 性能分析
    struct PerformanceReport {
        std::unordered_map<std::string, int> function_call_counts;
        std::unordered_map<std::string, int> variable_usage_counts;
        std::vector<std::string> optimization_suggestions;
        double estimated_performance_gain;
        
        PerformanceReport() : estimated_performance_gain(0.0) {}
    };
    
    PerformanceReport analyzePerformance(const std::string& js_code);
    
private:
    JavaScriptCompilerCore* compiler_;
    
    // 分析方法
    std::unordered_map<std::string, int> countFunctionCalls(const std::string& js_code);
    std::unordered_map<std::string, int> countVariableUsage(const std::string& js_code);
    std::vector<std::string> detectPerformanceIssues(const std::string& js_code);
};

} // namespace javascript
} // namespace chtl