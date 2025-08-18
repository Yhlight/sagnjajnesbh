#include "JavaScriptCompilerCore.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>

using namespace antlr4;
using namespace antlr4::tree;

namespace chtl {
namespace javascript {

JavaScriptCompilerCore::JavaScriptCompilerCore() 
    : minify_output_(false), generate_source_map_(false), es_modules_enabled_(true),
      babel_transform_(false), typescript_support_(false), jsx_support_(false),
      target_ecma_version_("ES2020"), target_environment_("universal") {
}

std::string JavaScriptCompilerCore::compileJavaScript(const std::string& js_code) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    clearErrors();
    
    try {
        // 1. 解析JavaScript
        auto parse_tree = parseJavaScript(js_code);
        if (!parse_tree) {
            addError("JavaScript解析失败");
            return "";
        }
        
        // 2. 遍历解析树生成JavaScript
        JavaScriptTreeWalker walker(this);
        walker.setMinify(minify_output_);
        walker.setTargetVersion(target_ecma_version_);
        
        ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        std::string result = walker.getGeneratedJavaScript();
        
        // 3. 应用优化
        if (babel_transform_) {
            result = transformToES5(result);
        }
        
        result = optimizeCode(result);
        
        if (minify_output_) {
            result = minifyJavaScript(result);
        }
        
        // 4. 处理模块
        if (es_modules_enabled_) {
            result = processImports(result);
            result = processExports(result);
        }
        
        // 5. 更新统计
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        updateStats(js_code, result, duration.count() / 1000.0);
        
        return result;
        
    } catch (const std::exception& e) {
        addError("编译过程中发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string JavaScriptCompilerCore::compileJavaScriptFile(const std::string& file_path) {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            addError("无法打开文件: " + file_path);
            return "";
        }
        
        std::ostringstream content;
        content << file.rdbuf();
        file.close();
        
        return compileJavaScript(content.str());
        
    } catch (const std::exception& e) {
        addError("读取文件失败: " + std::string(e.what()));
        return "";
    }
}

std::unique_ptr<JavaScriptParser::ProgramContext> JavaScriptCompilerCore::parseJavaScript(const std::string& js_code) {
    try {
        // 创建输入流
        ANTLRInputStream input(js_code);
        
        // 创建词法分析器
        JavaScriptLexer lexer(&input);
        
        // 创建token流
        CommonTokenStream tokens(&lexer);
        
        // 创建解析器
        JavaScriptParser parser(&tokens);
        
        // 添加错误监听器
        JavaScriptErrorListener error_listener(this);
        parser.removeErrorListeners();
        parser.addErrorListener(&error_listener);
        
        // 解析
        auto tree = parser.program();
        
        if (hasErrors()) {
            return nullptr;
        }
        
        return std::unique_ptr<JavaScriptParser::ProgramContext>(
            static_cast<JavaScriptParser::ProgramContext*>(tree));
        
    } catch (const std::exception& e) {
        addError("解析器创建失败: " + std::string(e.what()));
        return nullptr;
    }
}

bool JavaScriptCompilerCore::validateJavaScript(const std::string& js_code) {
    clearErrors();
    auto parse_tree = parseJavaScript(js_code);
    return parse_tree != nullptr && !hasErrors();
}

std::string JavaScriptCompilerCore::minifyJavaScript(const std::string& js) {
    std::string result = js;
    
    // 移除多行注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除单行注释
    result = std::regex_replace(result, std::regex(R"(//.*$)"), "", std::regex_constants::match_not_null);
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空白
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除大括号周围的空白
    result = std::regex_replace(result, std::regex(R"(\s*{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*}\s*)"), "}");
    
    // 移除逗号周围的空白
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除开头和结尾的空白
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

std::string JavaScriptCompilerCore::transformToES5(const std::string& js) {
    std::string result = js;
    
    // 简化的ES6+ -> ES5转换
    
    // 箭头函数转换
    result = std::regex_replace(result, std::regex(R"((\w+)\s*=>\s*(.+))"), 
                               "function($1) { return $2; }");
    
    // const/let -> var
    result = std::regex_replace(result, std::regex(R"(\bconst\b)"), "var");
    result = std::regex_replace(result, std::regex(R"(\blet\b)"), "var");
    
    // 模板字符串简化处理
    result = std::regex_replace(result, std::regex(R"(`([^`]*)`)"),"\"$1\"");
    
    return result;
}

std::string JavaScriptCompilerCore::optimizeCode(const std::string& js) {
    JavaScriptOptimizer optimizer(this);
    return optimizer.optimizeCode(js);
}

std::string JavaScriptCompilerCore::processImports(const std::string& js) {
    // 简化的import处理
    std::string result = js;
    
    // 转换ES6 import为require (Node.js环境)
    if (target_environment_ == "node") {
        result = std::regex_replace(result, 
                                   std::regex(R"(import\s+(\w+)\s+from\s+['"](.*?)['"])"),
                                   "const $1 = require('$2')");
    }
    
    return result;
}

std::string JavaScriptCompilerCore::processExports(const std::string& js) {
    // 简化的export处理
    std::string result = js;
    
    // 转换ES6 export
    if (target_environment_ == "node") {
        result = std::regex_replace(result,
                                   std::regex(R"(export\s+default\s+(\w+))"),
                                   "module.exports = $1");
        
        result = std::regex_replace(result,
                                   std::regex(R"(export\s+\{([^}]+)\})"),
                                   "module.exports = { $1 }");
    }
    
    return result;
}

std::vector<std::string> JavaScriptCompilerCore::extractDependencies(const std::string& js_code) {
    std::vector<std::string> dependencies;
    
    // 查找import语句
    std::regex import_regex(R"(import\s+.*?\s+from\s+['"](.*?)['"])");
    std::sregex_iterator iter(js_code.begin(), js_code.end(), import_regex);
    std::sregex_iterator end;
    
    while (iter != end) {
        dependencies.push_back((*iter)[1].str());
        ++iter;
    }
    
    // 查找require语句
    std::regex require_regex(R"(require\s*\(\s*['"](.*?)['"])");
    iter = std::sregex_iterator(js_code.begin(), js_code.end(), require_regex);
    
    while (iter != end) {
        dependencies.push_back((*iter)[1].str());
        ++iter;
    }
    
    return dependencies;
}

void JavaScriptCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
}

void JavaScriptCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void JavaScriptCompilerCore::updateStats(const std::string& input, const std::string& output, double time_ms) {
    last_stats_.bytes_input = input.length();
    last_stats_.bytes_output = output.length();
    last_stats_.compilation_time_ms = time_ms;
    last_stats_.target_version = target_ecma_version_;
    
    if (last_stats_.bytes_input > 0) {
        last_stats_.compression_ratio = static_cast<double>(last_stats_.bytes_output) / last_stats_.bytes_input;
    }
    
    // 简化的统计计算
    last_stats_.functions_processed = std::count(input.begin(), input.end(), '{');
    last_stats_.classes_processed = std::regex_search(input, std::regex(R"(\bclass\b)")) ? 1 : 0;
    last_stats_.variables_processed = std::count(input.begin(), input.end(), '=');
    last_stats_.statements_processed = std::count(input.begin(), input.end(), ';');
}

void JavaScriptCompilerCore::printCompilationInfo() const {
    std::cout << "=== JavaScript编译器统计信息 ===" << std::endl;
    std::cout << "输入大小: " << last_stats_.bytes_input << " 字节" << std::endl;
    std::cout << "输出大小: " << last_stats_.bytes_output << " 字节" << std::endl;
    std::cout << "压缩比: " << (last_stats_.compression_ratio * 100) << "%" << std::endl;
    std::cout << "编译时间: " << last_stats_.compilation_time_ms << "ms" << std::endl;
    std::cout << "目标版本: " << last_stats_.target_version << std::endl;
    std::cout << "处理函数数: " << last_stats_.functions_processed << std::endl;
    std::cout << "处理类数: " << last_stats_.classes_processed << std::endl;
    std::cout << "处理变量数: " << last_stats_.variables_processed << std::endl;
    std::cout << "处理语句数: " << last_stats_.statements_processed << std::endl;
    
    if (hasErrors()) {
        std::cout << "错误数: " << errors_.size() << std::endl;
        for (const auto& error : errors_) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    if (!warnings_.empty()) {
        std::cout << "警告数: " << warnings_.size() << std::endl;
        for (const auto& warning : warnings_) {
            std::cout << "  - " << warning << std::endl;
        }
    }
}

// JavaScriptErrorListener 实现
void JavaScriptErrorListener::syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                                         size_t line, size_t charPositionInLine, 
                                         const std::string &msg, std::exception_ptr e) {
    std::ostringstream error;
    error << "第" << line << "行第" << charPositionInLine << "列: " << msg;
    if (offendingSymbol) {
        error << " (token: '" << offendingSymbol->getText() << "')";
    }
    compiler_->addError(error.str());
}

// JavaScriptTreeWalker 实现
void JavaScriptTreeWalker::enterProgram(JavaScriptParser::ProgramContext *ctx) {
    reset();
    appendJS("// CHTL JavaScript编译器生成\n");
    appendJS("'use strict';\n\n");
}

void JavaScriptTreeWalker::exitProgram(JavaScriptParser::ProgramContext *ctx) {
    // 程序处理完成
}

void JavaScriptTreeWalker::enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) {
    // 开始处理函数声明
}

void JavaScriptTreeWalker::exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) {
    std::string function_code = generateFunction(ctx);
    appendJS(function_code + "\n\n");
}

void JavaScriptTreeWalker::enterClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) {
    // 开始处理类声明
}

void JavaScriptTreeWalker::exitClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) {
    std::string class_code = generateClass(ctx);
    appendJS(class_code + "\n\n");
}

void JavaScriptTreeWalker::enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) {
    // 开始处理变量声明
}

void JavaScriptTreeWalker::exitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) {
    std::string variable_code = generateVariable(ctx);
    appendJS(variable_code + "\n");
}

void JavaScriptTreeWalker::enterExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) {
    // 开始处理表达式语句
}

void JavaScriptTreeWalker::exitExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) {
    std::string expr_text = extractText(ctx);
    appendJS(expr_text + "\n");
}

std::string JavaScriptTreeWalker::extractText(ParseTree* ctx) {
    if (!ctx) return "";
    
    std::ostringstream text;
    
    if (ctx->children.empty()) {
        return ctx->getText();
    }
    
    for (auto child : ctx->children) {
        text << extractText(child);
    }
    
    return text.str();
}

void JavaScriptTreeWalker::appendJS(const std::string& js) {
    generated_js_ += js;
}

std::string JavaScriptTreeWalker::generateFunction(JavaScriptParser::FunctionDeclarationContext* ctx) {
    if (!ctx) return "";
    
    std::ostringstream func;
    
    // 检查是否为异步函数
    bool is_async = false;
    if (ctx->Async()) {
        is_async = true;
        func << "async ";
    }
    
    func << "function ";
    
    // 函数名
    if (ctx->identifier()) {
        func << ctx->identifier()->getText();
    }
    
    // 参数列表
    func << "(";
    if (ctx->formalParameterList()) {
        func << extractText(ctx->formalParameterList());
    }
    func << ") ";
    
    // 函数体
    if (ctx->functionBody()) {
        func << extractText(ctx->functionBody());
    }
    
    return func.str();
}

std::string JavaScriptTreeWalker::generateClass(JavaScriptParser::ClassDeclarationContext* ctx) {
    if (!ctx) return "";
    
    std::ostringstream class_code;
    
    class_code << "class ";
    
    // 类名
    if (ctx->identifier()) {
        class_code << ctx->identifier()->getText();
    }
    
    // 继承
    if (ctx->Extends()) {
        class_code << " extends " << extractText(ctx->singleExpression());
    }
    
    // 类体
    class_code << " {";
    
    for (auto element : ctx->classElement()) {
        class_code << "\n  " << extractText(element);
    }
    
    class_code << "\n}";
    
    return class_code.str();
}

std::string JavaScriptTreeWalker::generateVariable(JavaScriptParser::VariableDeclarationContext* ctx) {
    if (!ctx) return "";
    
    std::ostringstream var_code;
    
    // 变量类型
    if (ctx->Var()) {
        var_code << "var ";
    } else if (ctx->Let()) {
        var_code << (target_version_ == "ES5" ? "var " : "let ");
    } else if (ctx->Const()) {
        var_code << (target_version_ == "ES5" ? "var " : "const ");
    }
    
    // 变量列表
    if (ctx->variableDeclarationList()) {
        var_code << extractText(ctx->variableDeclarationList());
    }
    
    var_code << ";";
    
    return var_code.str();
}

// JavaScriptOptimizer 实现
std::string JavaScriptOptimizer::optimizeCode(const std::string& js_code) {
    std::string result = js_code;
    
    // 常量折叠
    result = constantFolding(result);
    
    // 移除未使用的代码
    result = removeUnusedCode(result);
    
    // 内联小函数
    result = inlineFunctions(result);
    
    return result;
}

std::string JavaScriptOptimizer::constantFolding(const std::string& js_code) {
    std::string result = js_code;
    
    // 简单的常量折叠
    result = std::regex_replace(result, std::regex(R"(\btrue\s*&&\s*(.+))"), "$1");
    result = std::regex_replace(result, std::regex(R"((.+)\s*&&\s*true\b)"), "$1");
    result = std::regex_replace(result, std::regex(R"(\bfalse\s*\|\|\s*(.+))"), "$1");
    result = std::regex_replace(result, std::regex(R"((.+)\s*\|\|\s*false\b)"), "$1");
    
    // 数学常量折叠
    result = std::regex_replace(result, std::regex(R"(\b(\d+)\s*\+\s*(\d+)\b)"), "$1+$2");
                                   int a = std::stoi(match[1].str());
                                   int b = std::stoi(match[2].str());
                                   return std::to_string(a + b);
                               });
    
    return result;
}

std::string JavaScriptOptimizer::removeUnusedCode(const std::string& js_code) {
    // 简化的死代码移除
    std::string result = js_code;
    
    // 移除永远不会执行的代码
    result = std::regex_replace(result, std::regex(R"(if\s*\(\s*false\s*\)\s*\{[^}]*\})"), "");
    result = std::regex_replace(result, std::regex(R"(while\s*\(\s*false\s*\)\s*\{[^}]*\})"), "");
    
    return result;
}

std::string JavaScriptOptimizer::inlineFunctions(const std::string& js_code) {
    // 简化的函数内联
    // 在真实实现中，这里会有更复杂的分析逻辑
    return js_code;
}

JavaScriptOptimizer::PerformanceReport JavaScriptOptimizer::analyzePerformance(const std::string& js_code) {
    PerformanceReport report;
    
    report.function_call_counts = countFunctionCalls(js_code);
    report.variable_usage_counts = countVariableUsage(js_code);
    report.optimization_suggestions = detectPerformanceIssues(js_code);
    
    // 估算性能提升
    report.estimated_performance_gain = report.optimization_suggestions.size() * 5.0; // 每个优化5%提升
    
    return report;
}

std::unordered_map<std::string, int> JavaScriptOptimizer::countFunctionCalls(const std::string& js_code) {
    std::unordered_map<std::string, int> counts;
    
    std::regex call_regex(R"((\w+)\s*\()");
    std::sregex_iterator iter(js_code.begin(), js_code.end(), call_regex);
    std::sregex_iterator end;
    
    while (iter != end) {
        counts[(*iter)[1].str()]++;
        ++iter;
    }
    
    return counts;
}

std::vector<std::string> JavaScriptOptimizer::detectPerformanceIssues(const std::string& js_code) {
    std::vector<std::string> issues;
    
    // 检查常见性能问题
    if (js_code.find("document.getElementById") != std::string::npos) {
        issues.push_back("建议缓存DOM元素引用");
    }
    
    if (js_code.find("for (") != std::string::npos && js_code.find(".length") != std::string::npos) {
        issues.push_back("建议缓存数组长度");
    }
    
    if (std::count(js_code.begin(), js_code.end(), '+') > 10) {
        issues.push_back("考虑使用数组join()替代字符串连接");
    }
    
    return issues;
}

} // namespace javascript
} // namespace chtl