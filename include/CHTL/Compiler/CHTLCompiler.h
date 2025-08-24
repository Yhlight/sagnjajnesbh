#pragma once

#include <string>
#include <memory>
#include <vector>
#include "CHTL/Core/CHTLToken.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Generator/CHTLGenerator.h"

namespace CHTL {
namespace Compiler {

/**
 * @brief CHTL编译器配置
 */
struct CHTLCompilerConfig {
    bool enableDebug;           // 启用调试模式
    bool strictMode;            // 严格模式
    bool optimizeOutput;        // 优化输出
    bool generateComments;      // 生成注释
    
    CHTLCompilerConfig() : enableDebug(false), strictMode(true), 
                          optimizeOutput(true), generateComments(false) {}
};

/**
 * @brief CHTL编译结果
 */
struct CHTLCompilationResult {
    std::string htmlOutput;     // 生成的HTML代码
    std::string cssOutput;      // 生成的CSS代码（来自局部style）
    bool success;               // 编译是否成功
    std::vector<std::string> errors;    // 错误信息
    std::vector<std::string> warnings;  // 警告信息
    
    CHTLCompilationResult() : success(false) {}
};

/**
 * @brief 独立的CHTL编译器
 * 
 * 职责：
 * 1. 编译CHTL语法片段 -> HTML
 * 2. 编译局部style块 -> CSS
 * 3. 处理CHTL特有语法（模板、自定义组件、命名空间等）
 * 
 * 根据架构图：
 * - 局部style -> CHTL编译器
 * - CHTL片段 -> CHTL编译器
 */
class CHTLCompiler {
public:
    /**
     * @brief 构造函数
     * @param config 编译器配置
     */
    explicit CHTLCompiler(const CHTLCompilerConfig& config = CHTLCompilerConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLCompiler();
    
    /**
     * @brief 编译CHTL代码片段
     * @param chtlCode CHTL源代码
     * @param fileName 文件名（用于错误报告）
     * @return 编译结果
     */
    CHTLCompilationResult Compile(const std::string& chtlCode, const std::string& fileName = "");
    
    /**
     * @brief 编译局部style块
     * @param styleCode 局部style代码
     * @param fileName 文件名
     * @return 编译结果（主要是cssOutput）
     */
    CHTLCompilationResult CompileLocalStyle(const std::string& styleCode, const std::string& fileName = "");
    
    /**
     * @brief 验证CHTL语法
     * @param chtlCode CHTL源代码
     * @return 是否语法正确
     */
    bool Validate(const std::string& chtlCode);
    
    /**
     * @brief 获取编译器配置
     * @return 当前配置
     */
    const CHTLCompilerConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 设置编译器配置
     * @param config 新配置
     */
    void SetConfig(const CHTLCompilerConfig& config);
    
    /**
     * @brief 获取编译器版本
     * @return 版本字符串
     */
    static std::string GetVersion() { return "1.0.0"; }
    
    /**
     * @brief 获取支持的CHTL语法特性
     * @return 特性列表
     */
    static std::vector<std::string> GetSupportedFeatures();

private:
    CHTLCompilerConfig config_;                                 // 编译器配置
    std::unique_ptr<Core::CHTLGlobalMap> globalMap_;           // 全局映射
    std::unique_ptr<Core::CHTLState> stateManager_;            // 状态管理器
    std::unique_ptr<Lexer::CHTLLexer> lexer_;                  // 词法分析器
    std::unique_ptr<Parser::CHTLParser> parser_;               // 语法分析器
    std::unique_ptr<Generator::CHTLGenerator> generator_;      // 代码生成器
    
    /**
     * @brief 初始化编译器组件
     */
    void InitializeComponents();
    
    /**
     * @brief 清理资源
     */
    void Cleanup();
    
    /**
     * @brief 预处理CHTL代码
     * @param code 原始代码
     * @return 预处理后的代码
     */
    std::string PreprocessCode(const std::string& code);
    
    /**
     * @brief 后处理生成的HTML
     * @param html 原始HTML
     * @return 优化后的HTML
     */
    std::string PostprocessHTML(const std::string& html);
    
    /**
     * @brief 处理编译错误
     * @param error 错误信息
     * @param result 编译结果
     */
    void HandleCompilationError(const std::string& error, CHTLCompilationResult& result);
    
    /**
     * @brief 检查是否为局部style块
     * @param code 代码内容
     * @return 是否为局部style
     */
    bool IsLocalStyleBlock(const std::string& code);
    
    /**
     * @brief 提取局部style内容
     * @param styleBlock style块代码
     * @return 纯CSS内容
     */
    std::string ExtractStyleContent(const std::string& styleBlock);
    
    /**
     * @brief 处理CSS中的CHTL变量
     * @param cssContent CSS内容
     * @return 处理后的CSS
     */
    std::string ProcessCHTLVariablesInCSS(const std::string& cssContent);
};

} // namespace Compiler
} // namespace CHTL