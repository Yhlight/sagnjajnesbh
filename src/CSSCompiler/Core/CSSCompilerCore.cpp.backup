#include "CSSCompilerCore.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <regex>

using namespace antlr4;
using namespace antlr4::tree;

namespace chtl {
namespace css {

CSSCompilerCore::CSSCompilerCore() 
    : minify_output_(false), add_vendor_prefixes_(false), autoprefixer_enabled_(false) {
    target_browsers_ = {"Chrome >= 60", "Firefox >= 55", "Safari >= 12", "Edge >= 79"};
}

std::string CSSCompilerCore::compileCSS(const std::string& css_code) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    clearErrors();
    
    try {
        // 1. 解析CSS
        auto parse_tree = parseCSS(css_code);
        if (!parse_tree) {
            addError("CSS解析失败");
            return "";
        }
        
        // 2. 遍历解析树生成CSS
        CSSTreeWalker walker(this);
        ParseTreeWalker::DEFAULT.walk(&walker, parse_tree.get());
        
        std::string result = walker.getGeneratedCSS();
        
        // 3. 应用优化
        if (autoprefixer_enabled_) {
            result = autoprefixCSS(result);
        }
        
        if (add_vendor_prefixes_) {
            result = addVendorPrefixes(result);
        }
        
        if (minify_output_) {
            result = minifyCSS(result);
        }
        
        // 4. 更新统计
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        updateStats(css_code, result, duration.count() / 1000.0);
        
        return result;
        
    } catch (const std::exception& e) {
        addError("编译过程中发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CSSCompilerCore::compileCSSFile(const std::string& file_path) {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            addError("无法打开文件: " + file_path);
            return "";
        }
        
        std::ostringstream content;
        content << file.rdbuf();
        file.close();
        
        return compileCSS(content.str());
        
    } catch (const std::exception& e) {
        addError("读取文件失败: " + std::string(e.what()));
        return "";
    }
}

std::unique_ptr<css3Parser::StylesheetContext> CSSCompilerCore::parseCSS(const std::string& css_code) {
    try {
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
        
        // 解析
        auto tree = parser.stylesheet();
        
        if (hasErrors()) {
            return nullptr;
        }
        
        return std::unique_ptr<css3Parser::StylesheetContext>(
            static_cast<css3Parser::StylesheetContext*>(tree));
        
    } catch (const std::exception& e) {
        addError("解析器创建失败: " + std::string(e.what()));
        return nullptr;
    }
}

bool CSSCompilerCore::validateCSS(const std::string& css_code) {
    clearErrors();
    auto parse_tree = parseCSS(css_code);
    return parse_tree != nullptr && !hasErrors();
}

std::string CSSCompilerCore::minifyCSS(const std::string& css) {
    std::string result = css;
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除空白在特定字符周围
    result = std::regex_replace(result, std::regex(R"(\s*{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*}\s*)"), "}");
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    result = std::regex_replace(result, std::regex(R"(\s*:\s*)"), ":");
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除最后的分号
    result = std::regex_replace(result, std::regex(R"(;})"), "}");
    
    // 移除开头和结尾的空白
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

std::string CSSCompilerCore::addVendorPrefixes(const std::string& css) {
    std::string result = css;
    
    // 常见需要前缀的属性
    std::vector<std::pair<std::string, std::vector<std::string>>> prefixes = {
        {"transform", {"-webkit-transform", "-moz-transform", "-ms-transform"}},
        {"transition", {"-webkit-transition", "-moz-transition", "-ms-transition"}},
        {"animation", {"-webkit-animation", "-moz-animation", "-ms-animation"}},
        {"box-shadow", {"-webkit-box-shadow", "-moz-box-shadow"}},
        {"border-radius", {"-webkit-border-radius", "-moz-border-radius"}},
        {"user-select", {"-webkit-user-select", "-moz-user-select", "-ms-user-select"}},
        {"flex", {"-webkit-flex", "-moz-flex", "-ms-flex"}},
        {"grid", {"-webkit-grid", "-moz-grid", "-ms-grid"}}
    };
    
    for (const auto& [property, vendor_prefixes] : prefixes) {
        std::regex property_regex(property + R"(\s*:)");
        std::smatch match;
        
        if (std::regex_search(result, match, property_regex)) {
            std::string replacement = match.str();
            for (const auto& prefix : vendor_prefixes) {
                replacement = prefix + ":" + replacement.substr(property.length() + 1) + "\n  " + replacement;
            }
            result = std::regex_replace(result, property_regex, replacement);
        }
    }
    
    return result;
}

std::string CSSCompilerCore::autoprefixCSS(const std::string& css) {
    // 简化的autoprefixer实现
    // 在真实实现中，这里会使用更复杂的浏览器兼容性数据库
    return addVendorPrefixes(css);
}

void CSSCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
}

void CSSCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CSSCompilerCore::updateStats(const std::string& input, const std::string& output, double time_ms) {
    last_stats_.bytes_input = input.length();
    last_stats_.bytes_output = output.length();
    last_stats_.compilation_time_ms = time_ms;
    
    if (last_stats_.bytes_input > 0) {
        last_stats_.compression_ratio = static_cast<double>(last_stats_.bytes_output) / last_stats_.bytes_input;
    }
    
    // 简化的规则计数
    last_stats_.rules_processed = std::count(input.begin(), input.end(), '{');
    last_stats_.selectors_processed = std::count(input.begin(), input.end(), ',') + last_stats_.rules_processed;
    last_stats_.declarations_processed = std::count(input.begin(), input.end(), ':');
}

void CSSCompilerCore::printCompilationInfo() const {
    std::cout << "=== CSS编译器统计信息 ===" << std::endl;
    std::cout << "输入大小: " << last_stats_.bytes_input << " 字节" << std::endl;
    std::cout << "输出大小: " << last_stats_.bytes_output << " 字节" << std::endl;
    std::cout << "压缩比: " << (last_stats_.compression_ratio * 100) << "%" << std::endl;
    std::cout << "编译时间: " << last_stats_.compilation_time_ms << "ms" << std::endl;
    std::cout << "处理规则数: " << last_stats_.rules_processed << std::endl;
    std::cout << "处理选择器数: " << last_stats_.selectors_processed << std::endl;
    std::cout << "处理声明数: " << last_stats_.declarations_processed << std::endl;
    
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

// CSSErrorListener 实现
void CSSErrorListener::syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                                  size_t line, size_t charPositionInLine, 
                                  const std::string &msg, std::exception_ptr e) {
    std::ostringstream error;
    error << "第" << line << "行第" << charPositionInLine << "列: " << msg;
    if (offendingSymbol) {
        error << " (token: '" << offendingSymbol->getText() << "')";
    }
    compiler_->addError(error.str());
}

// CSSTreeWalker 实现
void CSSTreeWalker::enterStylesheet(css3Parser::StylesheetContext *ctx) {
    reset();
    appendCSS("/* CHTL CSS编译器生成 */\n");
}

void CSSTreeWalker::exitStylesheet(css3Parser::StylesheetContext *ctx) {
    // 样式表处理完成
}

void CSSTreeWalker::enterRuleset(css3Parser::RulesetContext *ctx) {
    // 开始处理规则集
}

void CSSTreeWalker::exitRuleset(css3Parser::RulesetContext *ctx) {
    std::string ruleset_text = extractText(ctx);
    appendCSS(ruleset_text + "\n");
}

void CSSTreeWalker::enterAtRule(css3Parser::AtRuleContext *ctx) {
    // 开始处理@规则
}

void CSSTreeWalker::exitAtRule(css3Parser::AtRuleContext *ctx) {
    std::string at_rule_text = extractText(ctx);
    appendCSS(at_rule_text + "\n");
}

void CSSTreeWalker::enterDeclaration(css3Parser::DeclarationContext *ctx) {
    // 开始处理声明
}

void CSSTreeWalker::exitDeclaration(css3Parser::DeclarationContext *ctx) {
    // 声明处理完成
}

std::string CSSTreeWalker::extractText(ParseTree* ctx) {
    if (!ctx) return "";
    
    std::ostringstream text;
    
    if (ctx->children.empty()) {
        // 叶子节点，直接返回文本
        return ctx->getText();
    }
    
    // 递归提取子节点文本
    for (auto child : ctx->children) {
        text << extractText(child);
    }
    
    return text.str();
}

void CSSTreeWalker::appendCSS(const std::string& css) {
    generated_css_ += css;
}

} // namespace css
} // namespace chtl