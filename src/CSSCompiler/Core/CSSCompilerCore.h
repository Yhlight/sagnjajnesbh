#pragma once
#include "../Parser/CSS3Parser.h"
#include "../Parser/CSS3Lexer.h"
#include "../Parser/CSS3Listener.h"
#include "antlr4-runtime.h"
#include <string>
#include <memory>
#include <vector>

namespace chtl {
namespace css {

/**
 * CSS编译器核心 - 使用ANTLR4实现原生CSS解析和编译
 */
class CSSCompilerCore {
public:
    CSSCompilerCore();
    ~CSSCompilerCore() = default;
    
    // 主编译接口
    std::string compileCSS(const std::string& css_code);
    std::string compileCSSFile(const std::string& file_path);
    
    // 解析接口
    std::unique_ptr<CSS3Parser::StylesheetContext> parseCSS(const std::string& css_code);
    bool validateCSS(const std::string& css_code);
    
    // 优化选项
    void enableMinification(bool enable) { minify_output_ = enable; }
    void enableVendorPrefixes(bool enable) { add_vendor_prefixes_ = enable; }
    void enableAutoprefixer(bool enable) { autoprefixer_enabled_ = enable; }
    void setTargetBrowsers(const std::vector<std::string>& browsers) { target_browsers_ = browsers; }
    
    // 错误处理
    bool hasErrors() const { return !errors_.empty(); }
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
    // 统计信息
    struct CompilationStats {
        size_t rules_processed;
        size_t selectors_processed;
        size_t declarations_processed;
        size_t bytes_input;
        size_t bytes_output;
        double compression_ratio;
        double compilation_time_ms;
        
        CompilationStats() : rules_processed(0), selectors_processed(0), 
                           declarations_processed(0), bytes_input(0), bytes_output(0),
                           compression_ratio(0.0), compilation_time_ms(0.0) {}
    };
    
    CompilationStats getLastStats() const { return last_stats_; }
    void printCompilationInfo() const;
    
private:
    bool minify_output_;
    bool add_vendor_prefixes_;
    bool autoprefixer_enabled_;
    std::vector<std::string> target_browsers_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    CompilationStats last_stats_;
    
    // 内部处理方法
    std::string processStylesheet(CSS3Parser::StylesheetContext* ctx);
    std::string processRuleset(CSS3Parser::RulesetContext* ctx);
    std::string processAtRule(CSS3Parser::At_ruleContext* ctx);
    std::string processSelector(CSS3Parser::SelectorContext* ctx);
    std::string processDeclaration(CSS3Parser::DeclarationContext* ctx);
    
    // 优化方法
    std::string minifyCSS(const std::string& css);
    std::string addVendorPrefixes(const std::string& css);
    std::string autoprefixCSS(const std::string& css);
    
    // 错误收集
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 统计更新
    void updateStats(const std::string& input, const std::string& output, double time_ms);
};

/**
 * CSS错误监听器 - 收集解析错误
 */
class CSSErrorListener : public antlr4::BaseErrorListener {
public:
    CSSErrorListener(CSSCompilerCore* compiler) : compiler_(compiler) {}
    
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
                    size_t line, size_t charPositionInLine, const std::string &msg,
                    std::exception_ptr e) override;
    
private:
    CSSCompilerCore* compiler_;
};

/**
 * CSS树遍历器 - 处理解析树并生成优化的CSS
 */
class CSSTreeWalker : public CSS3BaseListener {
public:
    CSSTreeWalker(CSSCompilerCore* compiler) : compiler_(compiler) {}
    
    // 重写监听器方法
    void enterStylesheet(CSS3Parser::StylesheetContext *ctx) override;
    void exitStylesheet(CSS3Parser::StylesheetContext *ctx) override;
    
    void enterRuleset(CSS3Parser::RulesetContext *ctx) override;
    void exitRuleset(CSS3Parser::RulesetContext *ctx) override;
    
    void enterAt_rule(CSS3Parser::At_ruleContext *ctx) override;
    void exitAt_rule(CSS3Parser::At_ruleContext *ctx) override;
    
    void enterDeclaration(CSS3Parser::DeclarationContext *ctx) override;
    void exitDeclaration(CSS3Parser::DeclarationContext *ctx) override;
    
    // 获取生成的CSS
    std::string getGeneratedCSS() const { return generated_css_; }
    void reset() { generated_css_.clear(); }
    
private:
    CSSCompilerCore* compiler_;
    std::string generated_css_;
    
    // 辅助方法
    std::string extractText(antlr4::tree::ParseTree* ctx);
    void appendCSS(const std::string& css);
};

} // namespace css
} // namespace chtl