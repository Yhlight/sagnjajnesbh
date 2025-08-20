#include "dispatcher/CompilerDispatcher.h"
#include "scanner/CHTLUnifiedScanner.h"
#include "chtl/compiler/CHTLCompiler.h"
#include "chtl_js/compiler/CHTLJSCompiler.h"
#include "css/CSSCompiler.h"
#include "javascript/JavaScriptCompiler.h"
#include "merger/ResultMerger.h"
#include "utils/FileUtils.h"
#include "utils/Logger.h"
#include <algorithm>

namespace chtl {

CompilerDispatcher::CompilerDispatcher()
    : m_DebugMode(false) {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::InitializeCompilers() {
    // 创建各个编译器组件
    m_Scanner = std::make_unique<CHTLUnifiedScanner>();
    m_ChtlCompiler = std::make_unique<compiler::CHTLCompiler>();
    m_ChtlJsCompiler = std::make_unique<chtljs::CHTLJSCompiler>();
    m_CssCompiler = std::make_unique<css::CSSCompiler>();
    m_JsCompiler = std::make_unique<javascript::JavaScriptCompiler>();
    m_ResultMerger = std::make_unique<merger::ResultMerger>();
    
    utils::Logger::GetInstance().Info("编译器调度器初始化完成");
}

bool CompilerDispatcher::Compile(const std::string& inputFile, const std::string& outputFile) {
    ClearErrors();
    
    // 读取源文件
    std::string source;
    if (!utils::FileUtils::ReadFile(inputFile, source)) {
        ReportError("无法读取输入文件: " + inputFile);
        return false;
    }
    
    utils::Logger::GetInstance().Info("开始编译文件: " + inputFile);
    
    // 处理源代码
    if (!ProcessSource(source, inputFile)) {
        return false;
    }
    
    // 生成最终HTML
    std::string html = m_ResultMerger->GenerateHTML();
    
    // 写入输出文件
    if (!utils::FileUtils::WriteFile(outputFile, html)) {
        ReportError("无法写入输出文件: " + outputFile);
        return false;
    }
    
    utils::Logger::GetInstance().Info("编译完成，输出文件: " + outputFile);
    return true;
}

bool CompilerDispatcher::CompileString(const std::string& source, const std::string& outputFile) {
    ClearErrors();
    
    utils::Logger::GetInstance().Info("开始编译字符串源码");
    
    // 处理源代码
    if (!ProcessSource(source, "<string>")) {
        return false;
    }
    
    // 生成最终HTML
    std::string html = m_ResultMerger->GenerateHTML();
    
    // 写入输出文件
    if (!utils::FileUtils::WriteFile(outputFile, html)) {
        ReportError("无法写入输出文件: " + outputFile);
        return false;
    }
    
    utils::Logger::GetInstance().Info("编译完成，输出文件: " + outputFile);
    return true;
}

bool CompilerDispatcher::ProcessSource(const std::string& source, const std::string& filename) {
    // 使用统一扫描器切割代码
    m_Scanner->Initialize(source, filename);
    std::vector<CodeFragment> fragments = m_Scanner->Scan();
    
    if (m_Scanner->HasErrors()) {
        for (const auto& error : m_Scanner->GetErrors()) {
            ReportError(error);
        }
        return false;
    }
    
    utils::Logger::GetInstance().Info("扫描完成，找到 " + 
                                    std::to_string(fragments.size()) + " 个代码片段");
    
    // 分发片段到对应编译器
    return DispatchFragments(fragments);
}

bool CompilerDispatcher::DispatchFragments(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        if (!DispatchFragment(fragment)) {
            return false;
        }
    }
    return true;
}

bool CompilerDispatcher::DispatchFragment(const CodeFragment& fragment) {
    utils::Logger::GetInstance().Debug("处理片段: " + fragment.GetTypeString() + 
                                     " at " + std::to_string(fragment.GetLine()) + 
                                     ":" + std::to_string(fragment.GetColumn()));
    
    switch (fragment.GetType()) {
        case CodeFragmentType::CHTL:
            return ProcessCHTLFragment(fragment);
            
        case CodeFragmentType::CHTL_JS:
            return ProcessCHTLJSFragment(fragment);
            
        case CodeFragmentType::CSS:
            return ProcessCSSFragment(fragment);
            
        case CodeFragmentType::JAVASCRIPT:
            return ProcessJavaScriptFragment(fragment);
            
        case CodeFragmentType::HTML:
            return ProcessHTMLFragment(fragment);
            
        case CodeFragmentType::TEXT:
            return ProcessTextFragment(fragment);
            
        case CodeFragmentType::SCRIPT:
            // script块需要特殊处理，可能包含CHTL、CHTL JS和JavaScript
            return ProcessScriptBlock(fragment);
            
        default:
            ReportError("未知的代码片段类型: " + fragment.GetTypeString());
            return false;
    }
}

bool CompilerDispatcher::ProcessCHTLFragment(const CodeFragment& fragment) {
    CompileResult result = m_ChtlCompiler->Compile(fragment);
    
    if (!result.success) {
        ReportError("CHTL编译错误: " + result.errorMessage);
        return false;
    }
    
    // 将结果添加到合并器
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::HTML, 
        result.output
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessCHTLJSFragment(const CodeFragment& fragment) {
    CompileResult result = m_ChtlJsCompiler->Compile(fragment);
    
    if (!result.success) {
        ReportError("CHTL JS编译错误: " + result.errorMessage);
        return false;
    }
    
    // CHTL JS生成的JavaScript代码
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::CHTL_JS, 
        result.output
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessCSSFragment(const CodeFragment& fragment) {
    CompileResult result = m_CssCompiler->Compile(fragment);
    
    if (!result.success) {
        ReportError("CSS编译错误: " + result.errorMessage);
        return false;
    }
    
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::CSS, 
        result.output
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessJavaScriptFragment(const CodeFragment& fragment) {
    CompileResult result = m_JsCompiler->Compile(fragment);
    
    if (!result.success) {
        ReportError("JavaScript编译错误: " + result.errorMessage);
        return false;
    }
    
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::JAVASCRIPT, 
        result.output
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessHTMLFragment(const CodeFragment& fragment) {
    // HTML片段直接添加到结果
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::HTML, 
        fragment.GetContent()
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessTextFragment(const CodeFragment& fragment) {
    // 文本片段需要转义后添加到HTML
    std::string escapedText = utils::StringUtils::EscapeHtml(fragment.GetContent());
    
    m_ResultMerger->AddFragment(merger::ResultFragment(
        merger::ResultFragment::HTML, 
        escapedText
    ));
    
    return true;
}

bool CompilerDispatcher::ProcessScriptBlock(const CodeFragment& fragment) {
    // script块的内容可能包含CHTL、CHTL JS和纯JavaScript
    // 需要进一步细分
    std::vector<CodeFragment> subFragments = SplitScriptContent(fragment.GetContent());
    
    // 处理子片段
    for (const auto& subFragment : subFragments) {
        if (!DispatchFragment(subFragment)) {
            return false;
        }
    }
    
    return true;
}

std::vector<CodeFragment> CompilerDispatcher::SplitScriptContent(const std::string& content) {
    // 使用扫描器的精准切割功能来分割script内容
    // 这里创建一个临时扫描器来处理script块内容
    CHTLUnifiedScanner scriptScanner;
    scriptScanner.Initialize(content, "<script>");
    scriptScanner.SetScriptContext(true);  // 设置为script上下文
    
    return scriptScanner.Scan();
}

void CompilerDispatcher::SetModulePaths(const std::vector<std::string>& paths) {
    m_ModulePaths = paths;
    
    // 更新CHTL编译器的模块路径
    if (m_ChtlCompiler) {
        m_ChtlCompiler->SetModulePaths(paths);
    }
}

void CompilerDispatcher::AddModulePath(const std::string& path) {
    m_ModulePaths.push_back(path);
    
    // 更新CHTL编译器的模块路径
    if (m_ChtlCompiler) {
        m_ChtlCompiler->SetModulePaths(m_ModulePaths);
    }
}

void CompilerDispatcher::SetDebugMode(bool debug) {
    m_DebugMode = debug;
    
    // 设置所有组件的调试模式
    if (m_Scanner) m_Scanner->SetDebugMode(debug);
    if (m_ChtlCompiler) m_ChtlCompiler->SetDebugMode(debug);
    if (m_ChtlJsCompiler) m_ChtlJsCompiler->SetDebugMode(debug);
    if (m_CssCompiler) m_CssCompiler->SetDebugMode(debug);
    if (m_JsCompiler) m_JsCompiler->SetDebugMode(debug);
    if (m_ResultMerger) m_ResultMerger->SetDebugMode(debug);
    
    // 设置日志级别
    if (debug) {
        utils::Logger::GetInstance().SetLevel(utils::LogLevel::DEBUG);
    }
}

ICompiler* CompilerDispatcher::GetCompiler(CodeFragmentType type) {
    switch (type) {
        case CodeFragmentType::CHTL:
            return m_ChtlCompiler.get();
            
        case CodeFragmentType::CHTL_JS:
            return m_ChtlJsCompiler.get();
            
        case CodeFragmentType::CSS:
            return m_CssCompiler.get();
            
        case CodeFragmentType::JAVASCRIPT:
            return m_JsCompiler.get();
            
        default:
            return nullptr;
    }
}

void CompilerDispatcher::ReportError(const std::string& message) {
    m_Errors.push_back(message);
    utils::Logger::GetInstance().Error(message);
}

void CompilerDispatcher::ClearErrors() {
    m_Errors.clear();
}

} // namespace chtl