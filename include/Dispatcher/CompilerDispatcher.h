#pragma once

#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// 前向声明
namespace Parser { class CHTLParser; }
namespace CHTLJS { namespace Parser { class CHTLJSParser; } }
namespace CSS { class CSSCompiler; }
namespace JavaScript { class JavaScriptCompiler; }
namespace Import { class EnhancedImportSystem; }

namespace Dispatcher {

/**
 * @brief 编译结果结构体
 */
struct CompilationResult {
    std::string htmlOutput;         // HTML输出
    std::string cssOutput;          // CSS输出  
    std::string javascriptOutput;   // JavaScript输出
    bool success;                   // 编译是否成功
    std::vector<std::string> errors; // 错误信息
    std::vector<std::string> warnings; // 警告信息
    
    CompilationResult() : success(false) {}
};

/**
 * @brief 编译器调度器配置
 */
struct DispatcherConfig {
    bool enableDebugOutput;         // 启用调试输出
    bool enableOptimization;        // 启用优化
    std::string outputDirectory;    // 输出目录
    
    DispatcherConfig() : enableDebugOutput(false), enableOptimization(true) {}
};

/**
 * @brief 编译器调度器类
 * 严格按照目标规划第24-41行：编译器调度器负责将Scanner切割的片段分发给对应的编译器
 * 
 * 架构流程：
 * CHTLUnifiedScanner → CompilerDispatcher → [CHTL/CHTL JS/CSS/JavaScript编译器] → 编译结果合并
 */
class CompilerDispatcher {
public:
    /**
     * @brief 构造函数
     * @param config 调度器配置
     */
    explicit CompilerDispatcher(const DispatcherConfig& config = DispatcherConfig());
    
    /**
     * @brief 析构函数
     */
    ~CompilerDispatcher();
    
    /**
     * @brief 编译CHTL源代码
     * @param source 源代码
     * @param fileName 文件名
     * @return 编译结果
     */
    CompilationResult Compile(const std::string& source, const std::string& fileName = "");
    
    /**
     * @brief 设置调度器配置
     * @param config 新配置
     */
    void SetConfig(const DispatcherConfig& config) { config_ = config; }
    
    /**
     * @brief 获取调度器配置
     * @return 当前配置
     */
    const DispatcherConfig& GetConfig() const { return config_; }

private:
    DispatcherConfig config_;                           // 调度器配置
    
    // 核心状态管理组件
    std::unique_ptr<Core::CHTLGlobalMap> globalMap_;    // 全局映射表
    std::unique_ptr<Core::CHTLState> stateManager_;     // CHTL状态管理器
    std::unique_ptr<CHTLJS::Core::CHTLJSState> chtlJSStateManager_; // CHTL JS状态管理器
    
    // 统一扫描器
    std::unique_ptr<Scanner::CHTLUnifiedScanner> scanner_;
    
    // Import系统
    std::unique_ptr<Import::EnhancedImportSystem> importSystem_;

    // 解析器组件
    std::unique_ptr<Parser::CHTLParser> chtlParser_;        // CHTL解析器
    std::unique_ptr<CHTLJS::Parser::CHTLJSParser> chtlJSParser_; // CHTL JS解析器
    std::unique_ptr<CSS::CSSCompiler> cssCompiler_;         // CSS编译器
    std::unique_ptr<JavaScript::JavaScriptCompiler> jsCompiler_; // JavaScript编译器
    
    // 核心分发方法 - 严格按照目标规划第43-46行
    
    /**
     * @brief 分发代码片段给对应的编译器
     * @param fragments 代码片段列表
     * @param fileName 文件名
     * @return 编译结果
     */
    CompilationResult DispatchFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName);
    
    /**
     * @brief 处理CHTL片段
     * @param fragments CHTL片段列表
     * @param fileName 文件名
     * @return HTML输出
     */
    std::string ProcessCHTLFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName);
    
    /**
     * @brief 基础CHTL片段处理（回退方案）
     * @param fragments CHTL片段列表
     * @return HTML输出
     */
    std::string ProcessCHTLFragmentsBasic(const std::vector<Scanner::CodeFragment>& fragments);
    
    /**
     * @brief 处理CHTL JS片段
     * @param fragments CHTL JS片段列表
     * @param fileName 文件名
     * @return JavaScript输出
     */
    std::string ProcessCHTLJSFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName);
    
    /**
     * @brief 处理CSS片段
     * @param fragments CSS片段列表
     * @param fileName 文件名
     * @return CSS输出
     */
    std::string ProcessCSSFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName);
    
    /**
     * @brief 处理JavaScript片段
     * @param fragments JavaScript片段列表
     * @param fileName 文件名
     * @return JavaScript输出
     */
    std::string ProcessJavaScriptFragments(const std::vector<Scanner::CodeFragment>& fragments, const std::string& fileName);
    
    /**
     * @brief 处理Script的共同管理
     * 目标规划第46行：script → 由CHTL，CHTL JS，JS编译器共同管理
     * @param scriptFragments Script片段列表
     * @param fileName 文件名
     * @return JavaScript输出
     */
    std::string ProcessSharedScriptFragments(const std::vector<Scanner::CodeFragment>& scriptFragments, const std::string& fileName);
    
    /**
     * @brief 合并编译结果
     * @param htmlOutput HTML输出
     * @param cssOutput CSS输出
     * @param jsOutput JavaScript输出
     * @return 最终HTML输出
     */
    std::string MergeCompilationResults(const std::string& htmlOutput, const std::string& cssOutput, const std::string& jsOutput);
    
    /**
     * @brief 按类型分组代码片段
     * @param fragments 所有片段
     * @return 按类型分组的片段映射
     */
    std::map<Scanner::FragmentType, std::vector<Scanner::CodeFragment>> GroupFragmentsByType(const std::vector<Scanner::CodeFragment>& fragments);
    
    /**
     * @brief 初始化编译器
     */
    void InitializeCompilers();
    
    /**
     * @brief 清理资源
     */
    void Cleanup();
    
    /**
     * @brief 编译CHTL片段
     * @param content CHTL代码内容
     * @return 生成的HTML代码
     */
    std::string CompileCHTLFragment(const std::string& content);
    
    /**
     * @brief 编译CHTL JS片段
     * @param content CHTL JS代码内容
     * @return 生成的JavaScript代码
     */
    std::string CompileCHTLJSFragment(const std::string& content);

    /**
     * @brief 处理源代码中的Import语句
     * @param source 源代码
     */
    void ProcessImportStatements(const std::string& source);
    
         /**
      * @brief 获取可执行文件所在目录
      * @return 可执行文件目录路径
      */
     std::string GetExecutableDirectory() const;
};

} // namespace Dispatcher
} // namespace CHTL