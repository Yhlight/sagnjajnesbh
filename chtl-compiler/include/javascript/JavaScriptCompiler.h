#pragma once

#include "compiler/ICompiler.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

// 前向声明ANTLR生成的类
namespace antlr4 {
    namespace tree {
        class ParseTree;
    }
}

class JavaScriptLexer;
class JavaScriptParser;

namespace chtl {
namespace javascript {

/**
 * JavaScript编译器
 * 使用ANTLR4解析JavaScript代码
 */
class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override;
    
    /**
     * 编译代码片段
     */
    CompileResult Compile(const CodeFragment& fragment) override;
    
    /**
     * 编译多个代码片段
     */
    CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    
    /**
     * 重置编译器状态
     */
    void Reset() override;
    
    /**
     * 获取编译器名称
     */
    std::string GetName() const override { return "JavaScriptCompiler"; }
    
    /**
     * 设置调试模式
     */
    void SetDebugMode(bool debug) override { m_Debug = debug; }
    
    /**
     * 获取错误列表
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    /**
     * 清除错误
     */
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 设置选项
     */
    void SetMinify(bool minify) { m_Minify = minify; }
    void SetSourceMap(bool enable) { m_SourceMap = enable; }
    void SetStrictMode(bool strict) { m_StrictMode = strict; }
    void SetTargetES(const std::string& version) { m_TargetES = version; }
    
    /**
     * 模块管理
     */
    void SetModuleType(const std::string& type) { m_ModuleType = type; }
    void AddImportPath(const std::string& path) { m_ImportPaths.push_back(path); }
    
private:
    std::vector<std::string> m_Errors;
    bool m_Debug;
    bool m_Minify;
    bool m_SourceMap;
    bool m_StrictMode;
    std::string m_TargetES;
    std::string m_ModuleType;
    std::vector<std::string> m_ImportPaths;
    
    // 解析JavaScript
    std::unique_ptr<antlr4::tree::ParseTree> ParseJS(const std::string& js,
                                                     const std::string& filename);
    
    // 处理解析树
    std::string ProcessParseTree(antlr4::tree::ParseTree* tree);
    
    // 错误处理
    void ReportError(const std::string& message, const std::string& filename = "",
                    size_t line = 0, size_t column = 0);
    
    // JavaScript处理
    std::string MinifyJS(const std::string& js);
    std::string TranspileES(const std::string& js);
    std::string ProcessModules(const std::string& js);
    std::string GenerateSourceMap(const std::string& js, const std::string& filename);
};

// 暂时注释掉ANTLR相关的类，等ANTLR头文件正确包含后再启用
#if 0
/**
 * JavaScript访问器
 * 用于遍历和处理JavaScript解析树
 */
class JSVisitor : public JavaScriptParserBaseVisitor {
public:
    JSVisitor(bool minify = false, bool strictMode = false);
    ~JSVisitor();
    
    /**
     * 获取处理后的JavaScript
     */
    std::string GetResult() const { return m_Result.str(); }
    
    // 访问方法（根据JavaScriptParser.g4生成）
    antlrcpp::Any visitProgram(JavaScriptParser::ProgramContext* ctx) override;
    antlrcpp::Any visitStatement(JavaScriptParser::StatementContext* ctx) override;
    antlrcpp::Any visitBlock(JavaScriptParser::BlockContext* ctx) override;
    antlrcpp::Any visitVariableStatement(JavaScriptParser::VariableStatementContext* ctx) override;
    antlrcpp::Any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override;
    antlrcpp::Any visitClassDeclaration(JavaScriptParser::ClassDeclarationContext* ctx) override;
    antlrcpp::Any visitExpression(JavaScriptParser::ExpressionContext* ctx) override;
    
private:
    std::stringstream m_Result;
    int m_IndentLevel;
    bool m_Minify;
    bool m_StrictMode;
    bool m_NeedSemicolon;
    
    // 辅助方法
    void Indent();
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteSpace();
    void WriteSemicolon();
    
    // 处理特定结构
    std::string ProcessIdentifier(JavaScriptParser::IdentifierContext* ctx);
    std::string ProcessLiteral(JavaScriptParser::LiteralContext* ctx);
    void ProcessImportStatement(JavaScriptParser::ImportStatementContext* ctx);
    void ProcessExportStatement(JavaScriptParser::ExportStatementContext* ctx);
};
#endif

/**
 * JavaScript工具类
 */
class JSUtils {
public:
    /**
     * 检查是否是保留字
     */
    static bool IsReservedWord(const std::string& word);
    
    /**
     * 检查是否是有效的标识符
     */
    static bool IsValidIdentifier(const std::string& identifier);
    
    /**
     * 转义字符串
     */
    static std::string EscapeString(const std::string& str);
    
    /**
     * 规范化行结束符
     */
    static std::string NormalizeLineEndings(const std::string& code);
    
    /**
     * 检查ES版本特性
     */
    static bool IsES6Feature(const std::string& feature);
    static bool IsES2015Feature(const std::string& feature);
    static bool IsES2017Feature(const std::string& feature);
    
    /**
     * 获取ASI（自动分号插入）提示
     */
    static bool NeedsSemicolon(const std::string& line);
};

/**
 * JavaScript模块解析器
 */
class JSModuleResolver {
public:
    JSModuleResolver();
    ~JSModuleResolver();
    
    /**
     * 添加模块搜索路径
     */
    void AddSearchPath(const std::string& path);
    
    /**
     * 解析模块路径
     */
    std::string ResolveModule(const std::string& moduleName,
                             const std::string& fromFile);
    
    /**
     * 获取模块依赖
     */
    std::vector<std::string> GetDependencies(const std::string& code);
    
    /**
     * 转换模块格式
     */
    std::string ConvertModule(const std::string& code,
                             const std::string& fromType,
                             const std::string& toType);
    
private:
    std::vector<std::string> m_SearchPaths;
    std::unordered_map<std::string, std::string> m_ModuleCache;
    
    // 解析import/export语句
    void ParseImports(const std::string& code, std::vector<std::string>& imports);
    void ParseExports(const std::string& code, std::vector<std::string>& exports);
};

} // namespace javascript
} // namespace chtl