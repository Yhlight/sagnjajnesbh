#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "scanner/CodeFragment.h"
#include "compiler/ICompiler.h"

namespace chtl {

// 前向声明
class CHTLUnifiedScanner;
namespace compiler { class CHTLCompiler; }
namespace chtljs { class CHTLJSCompiler; }
namespace css { class CSSCompiler; }
namespace javascript { class JavaScriptCompiler; }
namespace merger { class ResultMerger; }

/**
 * 编译器调度器
 * 负责协调各个编译器组件，管理整个编译流程
 * 根据代码片段类型分发到对应的编译器
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
     * 编译 CHTL 字符串
     * @param source CHTL源代码
     * @param outputFile 输出文件路径
     * @return 编译是否成功
     */
    bool CompileString(const std::string& source, const std::string& outputFile);
    
    /**
     * 设置模块搜索路径
     * @param paths 模块路径列表
     */
    void SetModulePaths(const std::vector<std::string>& paths);
    
    /**
     * 添加模块搜索路径
     * @param path 模块路径
     */
    void AddModulePath(const std::string& path);
    
    /**
     * 设置调试模式
     * @param debug 是否启用调试
     */
    void SetDebugMode(bool debug);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 编译器组件
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;
    std::unique_ptr<compiler::CHTLCompiler> m_ChtlCompiler;
    std::unique_ptr<chtljs::CHTLJSCompiler> m_ChtlJsCompiler;
    std::unique_ptr<css::CSSCompiler> m_CssCompiler;
    std::unique_ptr<javascript::JavaScriptCompiler> m_JsCompiler;
    std::unique_ptr<merger::ResultMerger> m_ResultMerger;
    
    // 配置
    std::vector<std::string> m_ModulePaths;
    bool m_DebugMode;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    
    // 私有方法
    void InitializeCompilers();
    bool ProcessSource(const std::string& source, const std::string& filename);
    bool DispatchFragment(const CodeFragment& fragment);
    bool DispatchFragments(const std::vector<CodeFragment>& fragments);
    
    // 根据片段类型分发到对应编译器
    bool ProcessCHTLFragment(const CodeFragment& fragment);
    bool ProcessCHTLJSFragment(const CodeFragment& fragment);
    bool ProcessCSSFragment(const CodeFragment& fragment);
    bool ProcessJavaScriptFragment(const CodeFragment& fragment);
    bool ProcessHTMLFragment(const CodeFragment& fragment);
    bool ProcessTextFragment(const CodeFragment& fragment);
    
    // 处理script块的特殊逻辑
    bool ProcessScriptBlock(const CodeFragment& fragment);
    std::vector<CodeFragment> SplitScriptContent(const std::string& content);
    
    // 错误处理
    void ReportError(const std::string& message);
    void ClearErrors();
    
    // 获取编译器
    ICompiler* GetCompiler(FragmentType type);
};

} // namespace chtl