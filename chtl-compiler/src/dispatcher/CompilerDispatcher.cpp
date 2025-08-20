#include "dispatcher/CompilerDispatcher.h"
#include "scanner/CHTLUnifiedScanner.h"
#include "utils/Logger.h"
#include "utils/FileUtils.h"

namespace chtl {

CompilerDispatcher::CompilerDispatcher()
    : m_Scanner(std::make_unique<CHTLUnifiedScanner>())
    , m_DebugMode(false) {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

bool CompilerDispatcher::Compile(const std::string& inputFile, const std::string& outputFile) {
    auto& logger = utils::Logger::GetInstance();
    
    try {
        // 读取源文件
        std::string source = utils::FileUtils::ReadFile(inputFile);
        
        // 扫描源代码
        logger.Info("正在扫描源文件...");
        auto fragments = m_Scanner->Scan(source, inputFile);
        
        if (!m_Scanner->GetErrors().empty()) {
            for (const auto& error : m_Scanner->GetErrors()) {
                logger.Error(error);
            }
            return false;
        }
        
        logger.Info("扫描完成，找到 " + std::to_string(fragments.size()) + " 个代码片段");
        
        // TODO: 处理片段
        if (!ProcessFile(inputFile)) {
            return false;
        }
        
        // TODO: 合并结果
        if (!MergeResults(outputFile)) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger.Error(std::string("编译错误: ") + e.what());
        return false;
    }
}

void CompilerDispatcher::SetModulePath(const std::string& path) {
    m_ModulePaths.clear();
    m_ModulePaths.push_back(path);
}

void CompilerDispatcher::AddModulePath(const std::string& path) {
    m_ModulePaths.push_back(path);
}

void CompilerDispatcher::SetDebugMode(bool debug) {
    m_DebugMode = debug;
}

void CompilerDispatcher::InitializeCompilers() {
    // TODO: 初始化各个编译器
    // m_ChtlCompiler = std::make_unique<CHTLCompiler>();
    // m_ChtlJsCompiler = std::make_unique<CHTLJSCompiler>();
    // m_CssCompiler = std::make_unique<CSSCompiler>();
    // m_JsCompiler = std::make_unique<JavaScriptCompiler>();
    // m_Merger = std::make_unique<ResultMerger>();
}

bool CompilerDispatcher::ProcessFile(const std::string& inputFile) {
    // TODO: 实现文件处理逻辑
    return true;
}

bool CompilerDispatcher::MergeResults(const std::string& outputFile) {
    // TODO: 实现结果合并逻辑
    
    // 临时输出
    std::string html = R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CHTL Output</title>
</head>
<body>
    <h1>CHTL Compiler Output</h1>
    <p>This is a placeholder output from the CHTL compiler.</p>
</body>
</html>)";
    
    utils::FileUtils::WriteFile(outputFile, html);
    return true;
}

} // namespace chtl