#pragma once
#include "../Parser/JavaScriptLexer_cpp.h"
#include "../Parser/JavaScriptParser_cpp.h"
#include "../Parser/JavaScriptParser_cppBaseListener.h"
#include "../Parser/JavaScriptLexerBase.h"
#include "../Parser/JavaScriptParserBase.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace chtl {
namespace javascript {

// JavaScript编译器核心类
class JavaScriptCompilerCore {
public:
    JavaScriptCompilerCore();
    ~JavaScriptCompilerCore();
    
    // 主要编译接口
    std::string compileJavaScript(const std::string& js_code);
    bool validateJavaScript(const std::string& js_code);
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    
    // 代码优化
    std::string optimizeJavaScript(const std::string& js_code);
    std::string minifyJavaScript(const std::string& js_code);
    std::string formatJavaScript(const std::string& js_code);
    
    // 分析功能
    std::vector<std::string> extractFunctions(const std::string& js_code);
    std::vector<std::string> extractVariables(const std::string& js_code);
    std::vector<std::string> extractClasses(const std::string& js_code);
    
    // 转换功能
    std::string transpileES6ToES5(const std::string& js_code);
    std::string addPolyfills(const std::string& js_code);
    
    // 错误处理
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 配置选项
    struct CompilerOptions {
        bool enable_optimization = true;
        bool enable_minification = false;
        bool strict_mode = false;
        bool preserve_comments = false;
        std::string target_version = "es2015";
        bool enable_polyfills = true;
    };
    
    void setOptions(const CompilerOptions& options) { options_ = options; }
    const CompilerOptions& getOptions() const { return options_; }

private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    CompilerOptions options_;
    
    // 内部方法
    JavaScriptParser_cpp::ProgramContext* parseJavaScript(const std::string& js_code);
};

// JavaScript错误监听器
class JavaScriptErrorListener : public antlr4::BaseErrorListener {
public:
    explicit JavaScriptErrorListener(JavaScriptCompilerCore* compiler) : compiler_(compiler) {}
    
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override;

private:
    JavaScriptCompilerCore* compiler_;
};

// JavaScript树遍历器
class JavaScriptTreeWalker : public JavaScriptParser_cppBaseListener {
public:
    explicit JavaScriptTreeWalker(JavaScriptCompilerCore* compiler) : compiler_(compiler) {}
    
    // 监听器方法
    void enterProgram(JavaScriptParser_cpp::ProgramContext* ctx) override;
    void exitProgram(JavaScriptParser_cpp::ProgramContext* ctx) override;
    
    void enterFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext* ctx) override;
    void exitFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext* ctx) override;
    
    void enterVariableStatement(JavaScriptParser_cpp::VariableStatementContext* ctx) override;
    void exitVariableStatement(JavaScriptParser_cpp::VariableStatementContext* ctx) override;
    
    // 结果获取
    std::string getCompiledJavaScript() const { return output_.str(); }
    std::vector<std::string> getFunctions() const { return functions_; }
    std::vector<std::string> getVariables() const { return variables_; }

private:
    JavaScriptCompilerCore* compiler_;
    std::ostringstream output_;
    std::vector<std::string> functions_;
    std::vector<std::string> variables_;
    
    // 辅助方法
    std::string extractText(antlr4::tree::ParseTree* tree);
    void processFunction(JavaScriptParser_cpp::FunctionDeclarationContext* ctx);
    void processVariable(JavaScriptParser_cpp::VariableStatementContext* ctx);
};

// JavaScript优化器
class JavaScriptOptimizer {
public:
    JavaScriptOptimizer();
    ~JavaScriptOptimizer();
    
    // 优化方法
    std::string optimizeCode(const std::string& js_code);
    std::string removeDeadCode(const std::string& js_code);
    std::string inlineSmallFunctions(const std::string& js_code);
    std::string compressVariableNames(const std::string& js_code);
    
    // 分析方法
    std::vector<std::string> findUnusedVariables(const std::string& js_code);
    std::vector<std::string> findUnusedFunctions(const std::string& js_code);
    
public:
    // 优化辅助方法
    std::string normalizeWhitespace(const std::string& js_code);
    std::string removeComments(const std::string& js_code);
    std::string optimizeExpressions(const std::string& js_code);
};

} // namespace javascript
} // namespace chtl