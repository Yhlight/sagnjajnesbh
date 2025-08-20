#pragma once

#include <string>
#include <memory>
#include <vector>

namespace chtl {

// 前向声明
class CHTLUnifiedScanner;
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;
class ResultMerger;

/**
 * 编译器调度器
 * 负责协调各个编译器组件，管理整个编译流程
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    /**
     * 编译 CHTL 文件
     * @param inputFile 输入文件路径
     * @param outputFile 输出文件路径
     * @return 编译是否成功
     */
    bool Compile(const std::string& inputFile, const std::string& outputFile);

    /**
     * 设置模块搜索路径
     * @param path 模块路径
     */
    void SetModulePath(const std::string& path);

    /**
     * 添加额外的模块搜索路径
     * @param path 模块路径
     */
    void AddModulePath(const std::string& path);

    /**
     * 设置调试模式
     * @param debug 是否启用调试
     */
    void SetDebugMode(bool debug);

private:
    // 编译器组件
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;
    // TODO: 添加其他编译器组件
    // std::unique_ptr<CHTLCompiler> m_ChtlCompiler;
    // std::unique_ptr<CHTLJSCompiler> m_ChtlJsCompiler;
    // std::unique_ptr<CSSCompiler> m_CssCompiler;
    // std::unique_ptr<JavaScriptCompiler> m_JsCompiler;
    // std::unique_ptr<ResultMerger> m_Merger;

    // 配置
    std::vector<std::string> m_ModulePaths;
    bool m_DebugMode;

    // 私有方法
    void InitializeCompilers();
    bool ProcessFile(const std::string& inputFile);
    bool MergeResults(const std::string& outputFile);
};

} // namespace chtl