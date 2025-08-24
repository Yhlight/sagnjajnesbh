#pragma once

#include <string>
#include <memory>
#include <vector>
#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include "CHTLJS/Lexer/CHTLJSLexer.h"
#include "CHTLJS/Parser/CHTLJSParser.h"
#include "CHTLJS/Generator/CHTLJSGenerator.h"

namespace CHTL {
namespace CHTLJS {
namespace Compiler {

/**
 * @brief CHTL JS编译器配置
 */
struct CHTLJSCompilerConfig {
    bool enableDebug;           // 启用调试模式
    bool strictMode;            // 严格模式
    bool optimizeOutput;        // 优化输出
    bool generateComments;      // 生成注释
    bool enableVirtualObjects;  // 启用虚对象支持
    
    CHTLJSCompilerConfig() : enableDebug(false), strictMode(true), 
                            optimizeOutput(true), generateComments(false),
                            enableVirtualObjects(true) {}
};

/**
 * @brief CHTL JS编译结果
 */
struct CHTLJSCompilationResult {
    std::string jsOutput;       // 生成的JavaScript代码
    bool success;               // 编译是否成功
    std::vector<std::string> errors;    // 错误信息
    std::vector<std::string> warnings;  // 警告信息
    
    CHTLJSCompilationResult() : success(false) {}
};

/**
 * @brief 独立的CHTL JS编译器
 * 
 * 职责：
 * 1. 编译CHTL JS语法片段 -> JavaScript
 * 2. 处理虚对象(vir)语法
 * 3. 处理{{}}表达式语法
 * 4. 与您的架构完美集成
 * 
 * 根据架构图：
 * - CHTL JS片段 -> CHTL JS编译器
 */
class CHTLJSCompiler {
public:
    /**
     * @brief 构造函数
     * @param config 编译器配置
     */
    explicit CHTLJSCompiler(const CHTLJSCompilerConfig& config = CHTLJSCompilerConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLJSCompiler();
    
    /**
     * @brief 编译CHTL JS代码片段
     * @param chtlJSCode CHTL JS源代码
     * @param fileName 文件名（用于错误报告）
     * @return 编译结果
     */
    CHTLJSCompilationResult Compile(const std::string& chtlJSCode, const std::string& fileName = "");
    
    /**
     * @brief 编译虚对象语法
     * @param virCode vir对象代码
     * @param fileName 文件名
     * @return 编译结果
     */
    CHTLJSCompilationResult CompileVirtualObject(const std::string& virCode, const std::string& fileName = "");
    
    // 移除了错误的{{}}表达式语法支持
    
    /**
     * @brief 验证CHTL JS语法
     * @param chtlJSCode CHTL JS源代码
     * @return 是否语法正确
     */
    bool Validate(const std::string& chtlJSCode);
    
    /**
     * @brief 获取编译器配置
     * @return 当前配置
     */
    const CHTLJSCompilerConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 设置编译器配置
     * @param config 新配置
     */
    void SetConfig(const CHTLJSCompilerConfig& config);
    
    /**
     * @brief 获取编译器版本
     * @return 版本字符串
     */
    static std::string GetVersion() { return "1.0.0"; }
    
    /**
     * @brief 获取支持的CHTL JS语法特性
     * @return 特性列表
     */
    static std::vector<std::string> GetSupportedFeatures();

private:
    CHTLJSCompilerConfig config_;                               // 编译器配置
    std::unique_ptr<Core::CHTLJSState> stateManager_;          // 状态管理器
    std::unique_ptr<Lexer::CHTLJSLexer> lexer_;                // 词法分析器
    std::unique_ptr<Parser::CHTLJSParser> parser_;             // 语法分析器
    std::unique_ptr<Generator::CHTLJSGenerator> generator_;    // 代码生成器
    
    /**
     * @brief 初始化编译器组件
     */
    void InitializeComponents();
    
    /**
     * @brief 清理资源
     */
    void Cleanup();
    
    /**
     * @brief 预处理CHTL JS代码
     * @param code 原始代码
     * @return 预处理后的代码
     */
    std::string PreprocessCode(const std::string& code);
    
    /**
     * @brief 后处理生成的JavaScript
     * @param js 原始JavaScript
     * @return 优化后的JavaScript
     */
    std::string PostprocessJavaScript(const std::string& js);
    
    /**
     * @brief 处理编译错误
     * @param error 错误信息
     * @param result 编译结果
     */
    void HandleCompilationError(const std::string& error, CHTLJSCompilationResult& result);
    
    /**
     * @brief 检查是否为虚对象语法
     * @param code 代码内容
     * @return 是否为vir语法
     */
    bool IsVirtualObjectSyntax(const std::string& code);
    
    /**
     * @brief 检查是否为有效的CHTL JS片段
     * @param code 代码内容
     * @return 是否为有效的CHTL JS语法
     */
    bool IsValidCHTLJSFragment(const std::string& code);
    
    /**
     * @brief 处理虚对象转换
     * @param virCode vir对象代码
     * @return 转换后的JavaScript
     */
    std::string TransformVirtualObject(const std::string& virCode);
};

} // namespace Compiler
} // namespace CHTLJS
} // namespace CHTL