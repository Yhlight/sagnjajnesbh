#pragma once
#include "../Parser/css3Lexer.h"
#include "../Parser/css3Parser.h"
#include "../Parser/css3ParserBaseListener.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace chtl {
namespace css {

// CSS编译器核心类
class CSSCompilerCore {
public:
    CSSCompilerCore();
    ~CSSCompilerCore();
    
    // 主要编译接口
    std::string compileCSS(const std::string& css_code);
    bool validateCSS(const std::string& css_code);
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    
    // 代码优化
    std::string optimizeCSS(const std::string& css_code);
    std::string minifyCSS(const std::string& css_code);
    std::string formatCSS(const std::string& css_code);
    
    // 分析功能
    std::vector<std::string> extractSelectors(const std::string& css_code);
    std::vector<std::string> extractProperties(const std::string& css_code);
    std::vector<std::string> extractValues(const std::string& css_code);
    
    // 错误处理
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    
    // 配置选项
    struct CompilerOptions {
        bool enable_optimization = true;
        bool enable_minification = false;
        bool strict_mode = false;
        bool preserve_comments = false;
        std::string target_version = "css3";
    };
    
    void setOptions(const CompilerOptions& options) { options_ = options; }
    const CompilerOptions& getOptions() const { return options_; }

private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    CompilerOptions options_;
    
    // 内部方法
    std::unique_ptr<css3Parser::StylesheetContext> parseCSS(const std::string& css_code);
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
};

// CSS错误监听器
class CSSErrorListener : public antlr4::BaseErrorListener {
public:
    explicit CSSErrorListener(CSSCompilerCore* compiler) : compiler_(compiler) {}
    
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override;

private:
    CSSCompilerCore* compiler_;
};

// CSS树遍历器
class CSSTreeWalker : public css3ParserBaseListener {
public:
    explicit CSSTreeWalker(CSSCompilerCore* compiler) : compiler_(compiler) {}
    
    // 监听器方法
    void enterStylesheet(css3Parser::StylesheetContext* ctx) override;
    void exitStylesheet(css3Parser::StylesheetContext* ctx) override;
    
    void enterQualifiedRule(css3Parser::QualifiedRuleContext* ctx) override;
    void exitQualifiedRule(css3Parser::QualifiedRuleContext* ctx) override;
    
    void enterDeclaration(css3Parser::DeclarationContext* ctx) override;
    void exitDeclaration(css3Parser::DeclarationContext* ctx) override;
    
    // 结果获取
    std::string getCompiledCSS() const { return output_.str(); }
    std::vector<std::string> getSelectors() const { return selectors_; }
    std::vector<std::string> getProperties() const { return properties_; }

private:
    CSSCompilerCore* compiler_;
    std::ostringstream output_;
    std::vector<std::string> selectors_;
    std::vector<std::string> properties_;
    
    // 辅助方法
    std::string extractText(antlr4::tree::ParseTree* tree);
    void processSelector(css3Parser::QualifiedRuleContext* ctx);
    void processDeclaration(css3Parser::DeclarationContext* ctx);
};

// CSS优化器
class CSSOptimizer {
public:
    CSSOptimizer();
    ~CSSOptimizer();
    
    // 优化方法
    std::string optimizeRules(const std::string& css_code);
    std::string mergeSelectors(const std::string& css_code);
    std::string removeUnusedRules(const std::string& css_code);
    std::string compressValues(const std::string& css_code);
    
    // 分析方法
    std::vector<std::string> findDuplicateRules(const std::string& css_code);
    std::vector<std::string> findUnusedSelectors(const std::string& css_code);
    
private:
    // 内部优化逻辑
    std::string normalizeWhitespace(const std::string& css_code);
    std::string removeComments(const std::string& css_code);
    std::string shortenValues(const std::string& css_code);
};

} // namespace css
} // namespace chtl