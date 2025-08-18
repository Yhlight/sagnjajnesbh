#include "CSSCompilerCore.h"
#include "antlr4-runtime.h"
#include <regex>
#include <algorithm>

using namespace antlr4;

namespace chtl {
namespace css {

// CSSCompilerCore 实现
CSSCompilerCore::CSSCompilerCore() {
    // 设置默认选项
    options_.enable_optimization = true;
    options_.enable_minification = false;
    options_.strict_mode = false;
    options_.preserve_comments = false;
    options_.target_version = "css3";
}

CSSCompilerCore::~CSSCompilerCore() = default;

std::string CSSCompilerCore::compileCSS(const std::string& css_code) {
    clearErrors();
    
    try {
        // 首先验证CSS语法
        if (!validateCSS(css_code)) {
            return ""; // 验证失败
        }
        
        std::string result = css_code;
        
        // 应用优化
        if (options_.enable_optimization) {
            result = optimizeCSS(result);
        }
        
        // 应用压缩
        if (options_.enable_minification) {
            result = minifyCSS(result);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        addError("CSS编译异常: " + std::string(e.what()));
        return "";
    }
}

bool CSSCompilerCore::validateCSS(const std::string& css_code) {
    try {
        auto parse_tree = parseCSS(css_code);
        return parse_tree != nullptr && errors_.empty();
    } catch (...) {
        addError("CSS语法验证失败");
        return false;
    }
}

std::unique_ptr<css3Parser::StylesheetContext> CSSCompilerCore::parseCSS(const std::string& css_code) {
    // 创建输入流
    ANTLRInputStream input(css_code);
    
    // 创建词法分析器
    css3Lexer lexer(&input);
    
    // 创建token流
    CommonTokenStream tokens(&lexer);
    
    // 创建解析器
    css3Parser parser(&tokens);
    
    // 添加错误监听器
    CSSErrorListener error_listener(this);
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);
    
    // 解析CSS
    auto tree = parser.stylesheet();
    
    return std::unique_ptr<css3Parser::StylesheetContext>(
        static_cast<css3Parser::StylesheetContext*>(tree));
}

std::string CSSCompilerCore::optimizeCSS(const std::string& css_code) {
    CSSOptimizer optimizer;
    std::string result = css_code;
    
    // 应用各种优化
    result = optimizer.optimizeRules(result);
    result = optimizer.mergeSelectors(result);
    result = optimizer.compressValues(result);
    
    return result;
}

std::string CSSCompilerCore::minifyCSS(const std::string& css_code) {
    CSSOptimizer optimizer;
    std::string result = css_code;
    
    // 移除注释和多余空白
    result = optimizer.removeComments(result);
    result = optimizer.normalizeWhitespace(result);
    
    return result;
}

std::string CSSCompilerCore::formatCSS(const std::string& css_code) {
    try {
        auto parse_tree = parseCSS(css_code);
        if (!parse_tree || hasErrors()) {
            return css_code; // 解析失败，返回原始代码
        }
        
        // 使用树遍历器格式化
        CSSTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getCompiledCSS();
        
    } catch (const std::exception& e) {
        addError("CSS格式化失败: " + std::string(e.what()));
        return css_code;
    }
}

std::vector<std::string> CSSCompilerCore::extractSelectors(const std::string& css_code) {
    try {
        auto parse_tree = parseCSS(css_code);
        if (!parse_tree || hasErrors()) {
            return {};
        }
        
        CSSTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getSelectors();
        
    } catch (const std::exception& e) {
        addError("选择器提取失败: " + std::string(e.what()));
        return {};
    }
}

std::vector<std::string> CSSCompilerCore::extractProperties(const std::string& css_code) {
    try {
        auto parse_tree = parseCSS(css_code);
        if (!parse_tree || hasErrors()) {
            return {};
        }
        
        CSSTreeWalker walker(this);
        tree::ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        return walker.getProperties();
        
    } catch (const std::exception& e) {
        addError("属性提取失败: " + std::string(e.what()));
        return {};
    }
}

std::vector<std::string> CSSCompilerCore::extractValues(const std::string& css_code) {
    // 简化实现，提取所有属性值
    std::vector<std::string> values;
    std::regex value_pattern(R"(:\s*([^;{}]+);)");
    std::sregex_iterator iter(css_code.begin(), css_code.end(), value_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string value = match[1].str();
        // 去除前后空白
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);
        values.push_back(value);
    }
    
    return values;
}

void CSSCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
}

void CSSCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

// CSSErrorListener 实现
void CSSErrorListener::syntaxError(antlr4::Recognizer* /*recognizer*/,
                                  antlr4::Token* /*offendingSymbol*/,
                                  size_t line,
                                  size_t charPositionInLine,
                                  const std::string& msg,
                                  std::exception_ptr /*e*/) {
    std::ostringstream error_msg;
    error_msg << "CSS语法错误 [" << line << ":" << charPositionInLine << "] " << msg;
    compiler_->addError(error_msg.str());
}

// CSSTreeWalker 实现
void CSSTreeWalker::enterStylesheet(css3Parser::StylesheetContext* /*ctx*/) {
    // CSS文档开始
}

void CSSTreeWalker::exitStylesheet(css3Parser::StylesheetContext* /*ctx*/) {
    // CSS文档结束
}

void CSSTreeWalker::enterQualifiedRule(css3Parser::QualifiedRuleContext* ctx) {
    processSelector(ctx);
}

void CSSTreeWalker::exitQualifiedRule(css3Parser::QualifiedRuleContext* /*ctx*/) {
    output_ << "}\n";
}

void CSSTreeWalker::enterDeclaration(css3Parser::DeclarationContext* ctx) {
    processDeclaration(ctx);
}

void CSSTreeWalker::exitDeclaration(css3Parser::DeclarationContext* /*ctx*/) {
    // 声明处理完成
}

std::string CSSTreeWalker::extractText(antlr4::tree::ParseTree* tree) {
    if (!tree) return "";
    return tree->getText();
}

void CSSTreeWalker::processSelector(css3Parser::QualifiedRuleContext* ctx) {
    std::string selector = extractText(ctx);
    
    // 简化：提取选择器部分（在{之前）
    size_t brace_pos = selector.find('{');
    if (brace_pos != std::string::npos) {
        selector = selector.substr(0, brace_pos);
    }
    
    // 去除前后空白
    selector.erase(0, selector.find_first_not_of(" \t\n\r"));
    selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
    
    if (!selector.empty()) {
        selectors_.push_back(selector);
        output_ << selector << " {\n";
    }
}

void CSSTreeWalker::processDeclaration(css3Parser::DeclarationContext* ctx) {
    std::string declaration = extractText(ctx);
    
    // 提取属性名
    size_t colon_pos = declaration.find(':');
    if (colon_pos != std::string::npos) {
        std::string property = declaration.substr(0, colon_pos);
        property.erase(0, property.find_first_not_of(" \t\n\r"));
        property.erase(property.find_last_not_of(" \t\n\r") + 1);
        
        if (!property.empty()) {
            properties_.push_back(property);
        }
    }
    
    output_ << "    " << declaration;
    if (declaration.back() != ';') {
        output_ << ";";
    }
    output_ << "\n";
}

// CSSOptimizer 实现
CSSOptimizer::CSSOptimizer() = default;
CSSOptimizer::~CSSOptimizer() = default;

std::string CSSOptimizer::optimizeRules(const std::string& css_code) {
    std::string result = css_code;
    
    // 基本优化：移除多余空白
    result = normalizeWhitespace(result);
    
    // 移除空规则
    std::regex empty_rules(R"([^{}]*\{\s*\})");
    result = std::regex_replace(result, empty_rules, "");
    
    return result;
}

std::string CSSOptimizer::mergeSelectors(const std::string& css_code) {
    // 简化实现：合并相同的声明块
    return css_code; // TODO: 实现选择器合并逻辑
}

std::string CSSOptimizer::removeUnusedRules(const std::string& css_code) {
    // 简化实现：移除明显未使用的规则
    return css_code; // TODO: 实现未使用规则检测
}

std::string CSSOptimizer::compressValues(const std::string& css_code) {
    std::string result = css_code;
    
    // 压缩颜色值
    std::regex hex_color(R"(#([0-9a-fA-F])\1([0-9a-fA-F])\2([0-9a-fA-F])\3)");
    result = std::regex_replace(result, hex_color, "#$1$2$3");
    
    // 压缩margin/padding值
    std::regex margin_padding(R"((margin|padding):\s*(\d+px)\s+\2\s+\2\s+\2)");
    result = std::regex_replace(result, margin_padding, "$1: $2");
    
    return result;
}

std::vector<std::string> CSSOptimizer::findDuplicateRules(const std::string& css_code) {
    // TODO: 实现重复规则检测
    return {};
}

std::vector<std::string> CSSOptimizer::findUnusedSelectors(const std::string& css_code) {
    // TODO: 实现未使用选择器检测
    return {};
}

std::string CSSOptimizer::normalizeWhitespace(const std::string& css_code) {
    std::string result = css_code;
    
    // 移除多余的空白字符
    std::regex multiple_spaces(R"(\s+)");
    result = std::regex_replace(result, multiple_spaces, " ");
    
    // 移除不必要的空格
    std::regex unnecessary_spaces(R"(\s*([{}:;,])\s*)");
    result = std::regex_replace(result, unnecessary_spaces, "$1");
    
    return result;
}

std::string CSSOptimizer::removeComments(const std::string& css_code) {
    std::string result = css_code;
    
    // 移除CSS注释
    std::regex css_comments(R"(/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, css_comments, "");
    
    return result;
}

std::string CSSOptimizer::shortenValues(const std::string& css_code) {
    std::string result = css_code;
    
    // 简化0值
    std::regex zero_values(R"(\b0px\b|\b0em\b|\b0rem\b|\b0%\b)");
    result = std::regex_replace(result, zero_values, "0");
    
    return result;
}

} // namespace css
} // namespace chtl