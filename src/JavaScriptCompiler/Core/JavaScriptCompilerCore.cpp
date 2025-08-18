#include "JavaScriptCompilerCore.h"
#include "antlr4-runtime.h"
#include <regex>
#include <algorithm>

using namespace antlr4;

namespace chtl {
namespace javascript {

// JavaScriptCompilerCore 实现
JavaScriptCompilerCore::JavaScriptCompilerCore() {
    // 设置默认选项
    options_.enable_optimization = true;
    options_.enable_minification = false;
    options_.strict_mode = false;
    options_.preserve_comments = false;
    options_.target_version = "es2015";
    options_.enable_polyfills = true;
}

JavaScriptCompilerCore::~JavaScriptCompilerCore() = default;

std::string JavaScriptCompilerCore::compileJavaScript(const std::string& js_code) {
    clearErrors();
    
    try {
        // 首先验证JavaScript语法
        if (!validateJavaScript(js_code)) {
            return ""; // 验证失败
        }
        
        std::string result = js_code;
        
        // 应用优化
        if (options_.enable_optimization) {
            result = optimizeJavaScript(result);
        }
        
        // 应用压缩
        if (options_.enable_minification) {
            result = minifyJavaScript(result);
        }
        
        // 添加polyfills
        if (options_.enable_polyfills) {
            result = addPolyfills(result);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        addError("JavaScript编译异常: " + std::string(e.what()));
        return "";
    }
}

bool JavaScriptCompilerCore::validateJavaScript(const std::string& js_code) {
    try {
        auto parse_tree = parseJavaScript(js_code);
        return parse_tree != nullptr && errors_.empty();
    } catch (...) {
        addError("JavaScript语法验证失败");
        return false;
    }
}

std::unique_ptr<JavaScriptParser_cpp::ProgramContext> JavaScriptCompilerCore::parseJavaScript(const std::string& js_code) {
    // 创建输入流
    ANTLRInputStream input(js_code);
    
    // 创建词法分析器
    JavaScriptLexer_cpp lexer(&input);
    
    // 创建token流
    CommonTokenStream tokens(&lexer);
    
    // 创建解析器
    JavaScriptParser_cpp parser(&tokens);
    
    // 添加错误监听器
    JavaScriptErrorListener error_listener(this);
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);
    
    // 解析JavaScript
    auto tree = parser.program();
    
    return std::unique_ptr<JavaScriptParser_cpp::ProgramContext>(
        static_cast<JavaScriptParser_cpp::ProgramContext*>(tree));
}

std::string JavaScriptCompilerCore::optimizeJavaScript(const std::string& js_code) {
    JavaScriptOptimizer optimizer;
    std::string result = js_code;
    
    // 应用各种优化
    result = optimizer.optimizeCode(result);
    result = optimizer.removeDeadCode(result);
    result = optimizer.inlineSmallFunctions(result);
    
    return result;
}

std::string JavaScriptCompilerCore::minifyJavaScript(const std::string& js_code) {
    JavaScriptOptimizer optimizer;
    std::string result = js_code;
    
    // 移除注释和多余空白
    result = optimizer.removeComments(result);
    result = optimizer.normalizeWhitespace(result);
    result = optimizer.compressVariableNames(result);
    
    return result;
}

std::string JavaScriptCompilerCore::formatJavaScript(const std::string& js_code) {
    try {
        auto parse_tree = parseJavaScript(js_code);
        if (!parse_tree || hasErrors()) {
            return js_code; // 解析失败，返回原始代码
        }
        
        // 使用树遍历器格式化
        JavaScriptTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getCompiledJavaScript();
        
    } catch (const std::exception& e) {
        addError("JavaScript格式化失败: " + std::string(e.what()));
        return js_code;
    }
}

std::vector<std::string> JavaScriptCompilerCore::extractFunctions(const std::string& js_code) {
    try {
        auto parse_tree = parseJavaScript(js_code);
        if (!parse_tree || hasErrors()) {
            return {};
        }
        
        JavaScriptTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getFunctions();
        
    } catch (const std::exception& e) {
        addError("函数提取失败: " + std::string(e.what()));
        return {};
    }
}

std::vector<std::string> JavaScriptCompilerCore::extractVariables(const std::string& js_code) {
    try {
        auto parse_tree = parseJavaScript(js_code);
        if (!parse_tree || hasErrors()) {
            return {};
        }
        
        JavaScriptTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getVariables();
        
    } catch (const std::exception& e) {
        addError("变量提取失败: " + std::string(e.what()));
        return {};
    }
}

std::vector<std::string> JavaScriptCompilerCore::extractClasses(const std::string& js_code) {
    // 简化实现：使用正则表达式提取类定义
    std::vector<std::string> classes;
    std::regex class_pattern(R"(class\s+(\w+))");
    std::sregex_iterator iter(js_code.begin(), js_code.end(), class_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        classes.push_back(match[1].str());
    }
    
    return classes;
}

std::string JavaScriptCompilerCore::transpileES6ToES5(const std::string& js_code) {
    std::string result = js_code;
    
    // 简化的ES6到ES5转换
    // 转换箭头函数
    std::regex arrow_function(R"((\w+)\s*=>\s*\{([^}]*)\})");
    result = std::regex_replace(result, arrow_function, "function($1) { $2 }");
    
    // 转换const/let到var
    std::regex const_let(R"(\b(const|let)\b)");
    result = std::regex_replace(result, const_let, "var");
    
    return result;
}

std::string JavaScriptCompilerCore::addPolyfills(const std::string& js_code) {
    std::string result = js_code;
    
    // 检查是否需要polyfills
    if (js_code.find("Promise") != std::string::npos) {
        result = "// Promise polyfill would be added here\n" + result;
    }
    
    if (js_code.find("fetch") != std::string::npos) {
        result = "// Fetch polyfill would be added here\n" + result;
    }
    
    return result;
}

void JavaScriptCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
}

void JavaScriptCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

// JavaScriptErrorListener 实现
void JavaScriptErrorListener::syntaxError(antlr4::Recognizer* /*recognizer*/,
                                         antlr4::Token* /*offendingSymbol*/,
                                         size_t line,
                                         size_t charPositionInLine,
                                         const std::string& msg,
                                         std::exception_ptr /*e*/) {
    std::ostringstream error_msg;
    error_msg << "JavaScript语法错误 [" << line << ":" << charPositionInLine << "] " << msg;
    compiler_->addError(error_msg.str());
}

// JavaScriptTreeWalker 实现
void JavaScriptTreeWalker::enterProgram(JavaScriptParser_cpp::ProgramContext* /*ctx*/) {
    // JavaScript程序开始
}

void JavaScriptTreeWalker::exitProgram(JavaScriptParser_cpp::ProgramContext* /*ctx*/) {
    // JavaScript程序结束
}

void JavaScriptTreeWalker::enterFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext* ctx) {
    processFunction(ctx);
}

void JavaScriptTreeWalker::exitFunctionDeclaration(JavaScriptParser_cpp::FunctionDeclarationContext* /*ctx*/) {
    // 函数处理完成
}

void JavaScriptTreeWalker::enterVariableStatement(JavaScriptParser_cpp::VariableStatementContext* ctx) {
    processVariable(ctx);
}

void JavaScriptTreeWalker::exitVariableStatement(JavaScriptParser_cpp::VariableStatementContext* /*ctx*/) {
    // 变量处理完成
}

std::string JavaScriptTreeWalker::extractText(antlr4::tree::ParseTree* tree) {
    if (!tree) return "";
    return tree->getText();
}

void JavaScriptTreeWalker::processFunction(JavaScriptParser_cpp::FunctionDeclarationContext* ctx) {
    std::string function_text = extractText(ctx);
    functions_.push_back(function_text);
    output_ << function_text << "\n";
}

void JavaScriptTreeWalker::processVariable(JavaScriptParser_cpp::VariableStatementContext* ctx) {
    std::string variable_text = extractText(ctx);
    variables_.push_back(variable_text);
    output_ << variable_text << "\n";
}

// JavaScriptOptimizer 实现
JavaScriptOptimizer::JavaScriptOptimizer() = default;
JavaScriptOptimizer::~JavaScriptOptimizer() = default;

std::string JavaScriptOptimizer::optimizeCode(const std::string& js_code) {
    std::string result = js_code;
    
    // 基本优化：移除多余空白
    result = normalizeWhitespace(result);
    
    // 优化表达式
    result = optimizeExpressions(result);
    
    return result;
}

std::string JavaScriptOptimizer::removeDeadCode(const std::string& js_code) {
    std::string result = js_code;
    
    // 移除未使用的变量声明（简化实现）
    std::regex unused_vars(R"(var\s+\w+\s*=\s*[^;]+;\s*(?=var|\}|$))");
    // 这是一个简化的实现，实际需要更复杂的分析
    
    return result;
}

std::string JavaScriptOptimizer::inlineSmallFunctions(const std::string& js_code) {
    // TODO: 实现小函数内联
    return js_code;
}

std::string JavaScriptOptimizer::compressVariableNames(const std::string& js_code) {
    // TODO: 实现变量名压缩
    return js_code;
}

std::vector<std::string> JavaScriptOptimizer::findUnusedVariables(const std::string& js_code) {
    // TODO: 实现未使用变量检测
    return {};
}

std::vector<std::string> JavaScriptOptimizer::findUnusedFunctions(const std::string& js_code) {
    // TODO: 实现未使用函数检测
    return {};
}

std::string JavaScriptOptimizer::normalizeWhitespace(const std::string& js_code) {
    std::string result = js_code;
    
    // 移除多余的空白字符
    std::regex multiple_spaces(R"(\s+)");
    result = std::regex_replace(result, multiple_spaces, " ");
    
    return result;
}

std::string JavaScriptOptimizer::removeComments(const std::string& js_code) {
    std::string result = js_code;
    
    // 移除单行注释
    std::regex single_line_comments(R"(//.*?$)", std::regex::multiline);
    result = std::regex_replace(result, single_line_comments, "");
    
    // 移除多行注释
    std::regex multi_line_comments(R"(/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, multi_line_comments, "");
    
    return result;
}

std::string JavaScriptOptimizer::optimizeExpressions(const std::string& js_code) {
    std::string result = js_code;
    
    // 简化布尔表达式
    std::regex bool_true(R"(\btrue\s*===?\s*true\b)");
    result = std::regex_replace(result, bool_true, "true");
    
    std::regex bool_false(R"(\bfalse\s*===?\s*false\b)");
    result = std::regex_replace(result, bool_false, "false");
    
    return result;
}

} // namespace javascript
} // namespace chtl