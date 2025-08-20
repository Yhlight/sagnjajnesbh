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
    bool compile(const std::string& inputFile, const std::string& outputFile);

    /**
     * 设置模块搜索路径
     * @param path 模块路径
     */
    void setModulePath(const std::string& path);

    /**
     * 添加额外的模块搜索路径
     * @param path 模块路径
     */
    void addModulePath(const std::string& path);

    /**
     * 设置调试模式
     * @param debug 是否启用调试
     */
    void setDebugMode(bool debug);

private:
    // 编译器组件
    std::unique_ptr<CHTLUnifiedScanner> scanner_;
    std::unique_ptr<CHTLCompiler> chtlCompiler_;
    std::unique_ptr<CHTLJSCompiler> chtlJsCompiler_;
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JavaScriptCompiler> jsCompiler_;
    std::unique_ptr<ResultMerger> merger_;

    // 配置
    std::vector<std::string> modulePaths_;
    bool debugMode_;

    // 私有方法
    void initializeCompilers();
    bool processFile(const std::string& inputFile);
    bool mergeResults(const std::string& outputFile);
};

} // namespace chtl