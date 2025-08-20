#pragma once

#include "compiler/ICompiler.h"
#include <memory>
#include <string>
#include <vector>

// 前向声明ANTLR生成的类
namespace antlr4 {
    class ANTLRInputStream;
    class CommonTokenStream;
    namespace tree {
        class ParseTree;
    }
}

// 前向声明生成的解析器类
class css3Lexer;
class css3Parser;

namespace chtl {
namespace css {

/**
 * CSS编译器
 * 使用ANTLR4解析CSS代码
 */
class CSSCompiler : public compiler::ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler() override;
    
    /**
     * 编译CSS代码
     */
    compiler::CompileResult Compile(const std::string& sourceCode, 
                                  const std::string& filename = "") override;
    
    /**
     * 编译代码片段
     */
    compiler::CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    
    /**
     * 重置编译器状态
     */
    void Reset() override;
    
    /**
     * 获取错误列表
     */
    const std::vector<std::string>& GetErrors() const override { return m_Errors; }
    
    /**
     * 清除错误
     */
    void ClearErrors() override { m_Errors.clear(); }
    
    /**
     * 设置选项
     */
    void SetMinify(bool minify) { m_Minify = minify; }
    void SetSourceMap(bool enable) { m_SourceMap = enable; }
    void SetCompatibilityMode(const std::string& mode) { m_CompatibilityMode = mode; }
    
private:
    std::vector<std::string> m_Errors;
    bool m_Minify;
    bool m_SourceMap;
    std::string m_CompatibilityMode;
    
    // 解析CSS
    std::unique_ptr<antlr4::tree::ParseTree> ParseCSS(const std::string& css,
                                                      const std::string& filename);
    
    // 处理解析树
    std::string ProcessParseTree(antlr4::tree::ParseTree* tree);
    
    // 错误处理
    void ReportError(const std::string& message, const std::string& filename = "",
                    size_t line = 0, size_t column = 0);
    
    // CSS处理
    std::string MinifyCSS(const std::string& css);
    std::string GenerateSourceMap(const std::string& css, const std::string& filename);
    std::string ApplyCompatibility(const std::string& css);
};

/**
 * CSS错误监听器
 * 用于收集ANTLR解析错误
 */
class CSSErrorListener : public antlr4::BaseErrorListener {
public:
    CSSErrorListener(CSSCompiler* compiler) : m_Compiler(compiler) {}
    
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override;
    
private:
    CSSCompiler* m_Compiler;
};

/**
 * CSS访问器
 * 用于遍历和处理CSS解析树
 */
class CSSVisitor : public css3BaseVisitor {
public:
    CSSVisitor();
    ~CSSVisitor();
    
    /**
     * 获取处理后的CSS
     */
    std::string GetResult() const { return m_Result.str(); }
    
    // 访问方法（根据css3Parser.g4生成）
    antlrcpp::Any visitStylesheet(css3Parser::StylesheetContext* ctx) override;
    antlrcpp::Any visitRuleset(css3Parser::RulesetContext* ctx) override;
    antlrcpp::Any visitSelector(css3Parser::SelectorContext* ctx) override;
    antlrcpp::Any visitDeclaration(css3Parser::DeclarationContext* ctx) override;
    antlrcpp::Any visitProperty(css3Parser::PropertyContext* ctx) override;
    antlrcpp::Any visitExpr(css3Parser::ExprContext* ctx) override;
    
private:
    std::stringstream m_Result;
    int m_IndentLevel;
    bool m_Minify;
    
    // 辅助方法
    void Indent();
    void Write(const std::string& text);
    void WriteLine(const std::string& text);
    std::string ProcessSelector(css3Parser::SelectorContext* ctx);
    std::string ProcessValue(css3Parser::ExprContext* ctx);
};

/**
 * CSS工具类
 */
class CSSUtils {
public:
    /**
     * 验证CSS属性
     */
    static bool IsValidProperty(const std::string& property);
    
    /**
     * 验证CSS值
     */
    static bool IsValidValue(const std::string& property, const std::string& value);
    
    /**
     * 规范化颜色值
     */
    static std::string NormalizeColor(const std::string& color);
    
    /**
     * 规范化单位
     */
    static std::string NormalizeUnit(const std::string& value);
    
    /**
     * 检查浏览器兼容性
     */
    static bool CheckCompatibility(const std::string& property, 
                                 const std::string& compatMode);
    
    /**
     * 添加浏览器前缀
     */
    static std::vector<std::string> AddVendorPrefixes(const std::string& property);
};

} // namespace css
} // namespace chtl