#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <codecvt>
#include "Scanner/CHTLUnifiedScanner.h"
#include "Dispatcher/CompilerDispatcher.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"

using namespace CHTL;

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -h, --help           显示帮助信息\n";
    std::cout << "  -v, --verbose        启用详细输出模式\n";
    std::cout << "  -o, --output <文件>  指定输出文件\n";
    std::cout << "  --scan-only          仅进行扫描测试，不执行编译\n";
    std::cout << "  --no-context-check   禁用上下文检查\n";
    std::cout << "  --no-minimal-unit    禁用最小单元切割\n";
    std::cout << "\n";
    std::cout << "示例:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
    std::cout << "  " << programName << " --scan-only --verbose test.chtl\n";
}

void PrintFragmentType(CHTL::Scanner::FragmentType type) {
    switch (type) {
        case CHTL::Scanner::FragmentType::CHTL:
            std::cout << "CHTL";
            break;
        case CHTL::Scanner::FragmentType::CHTL_JS:
            std::cout << "CHTL JS";
            break;
        case CHTL::Scanner::FragmentType::CSS:
            std::cout << "CSS";
            break;
        case CHTL::Scanner::FragmentType::JS:
            std::cout << "JavaScript";
            break;
        case CHTL::Scanner::FragmentType::Unknown:
            std::cout << "Unknown";
            break;
    }
}

int main(int argc, char* argv[]) {
    // 设置UTF-8语言环境
    std::locale::global(std::locale(""));
    std::ios_base::sync_with_stdio(false);
    std::cin.imbue(std::locale());
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    
    std::string inputFile;
    std::string outputFile;
    bool verbose = false;
    bool scanOnly = false;
    bool enableContextCheck = true;
    bool enableMinimalUnit = true;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o 选项需要指定输出文件名\n";
                return 1;
            }
        } else if (arg == "--scan-only") {
            scanOnly = true;
        } else if (arg == "--no-context-check") {
            enableContextCheck = false;
        } else if (arg == "--no-minimal-unit") {
            enableMinimalUnit = false;
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件\n";
                return 1;
            }
        } else {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            PrintUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 必须指定输入文件\n";
        PrintUsage(argv[0]);
        return 1;
    }
    
    // 设置错误处理器
    Utils::ErrorHandler::GetInstance().SetVerbose(verbose);
    
    // 读取输入文件
    if (!Utils::FileUtils::FileExists(inputFile)) {
        std::cerr << "错误: 输入文件不存在: " << inputFile << "\n";
        return 1;
    }
    
    std::string sourceCode = Utils::FileUtils::ReadFileUTF8(inputFile);
    if (sourceCode.empty()) {
        std::cerr << "错误: 无法读取输入文件或文件为空: " << inputFile << "\n";
        return 1;
    }
    
    if (verbose) {
        std::cout << "读取文件: " << inputFile << " (大小: " << sourceCode.length() << " 字节)\n";
    }
    
    // 配置扫描器
    CHTL::Scanner::CHTLUnifiedScanner scanner;
    scanner.SetVerbose(verbose);
    
    // 执行扫描
    if (verbose) {
        std::cout << "开始扫描源代码...\n";
    }
    
    auto fragments = scanner.ScanSource(sourceCode, inputFile);
    
    if (verbose) {
        std::cout << "扫描完成，发现 " << fragments.size() << " 个代码片段\n";
    }
    
    // 如果只是扫描测试，打印结果并退出
    if (scanOnly) {
        std::cout << "\n=== 扫描结果 ===\n";
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            std::cout << "片段 " << (i + 1) << ":\n";
            std::cout << "  类型: ";
            PrintFragmentType(fragment.type);
            std::cout << "\n";
            std::cout << "  位置: " << fragment.startLine << ":" << fragment.startColumn 
                      << " - " << fragment.endLine << ":" << fragment.endColumn << "\n";
            // 上下文信息在新的扫描器中不再需要
            std::cout << "  内容: ";
            std::string content = Utils::StringUtils::Trim(fragment.content);
            if (content.length() > 50) {
                content = content.substr(0, 47) + "...";
            }
            content = Utils::StringUtils::ReplaceAll(content, "\n", "\\n");
            std::cout << "\"" << content << "\"\n";
            std::cout << "\n";
        }
        
        // 打印统计信息
        size_t chtlCount = 0, chtlJSCount = 0, cssCount = 0, jsCount = 0, unknownCount = 0;
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case CHTL::Scanner::FragmentType::CHTL: chtlCount++; break;
                case CHTL::Scanner::FragmentType::CHTL_JS: chtlJSCount++; break;
                case CHTL::Scanner::FragmentType::CSS: cssCount++; break;
                case CHTL::Scanner::FragmentType::JS: jsCount++; break;
                case CHTL::Scanner::FragmentType::Unknown: unknownCount++; break;
            }
        }
        
        std::cout << "=== 统计信息 ===\n";
        std::cout << "CHTL片段: " << chtlCount << "\n";
        std::cout << "CHTL JS片段: " << chtlJSCount << "\n";
        std::cout << "CSS片段: " << cssCount << "\n";
        std::cout << "JavaScript片段: " << jsCount << "\n";
        std::cout << "未知片段: " << unknownCount << "\n";
        std::cout << "总计: " << fragments.size() << "\n";
        
        return 0;
    }
    
    // 实现完整的编译流程 - 使用CompilerDispatcher
    std::cout << "开始完整编译流程...\n";
    
    try {
        // 创建编译器调度器
        Dispatcher::DispatcherConfig dispatcherConfig;
        dispatcherConfig.enableDebugOutput = verbose;
        dispatcherConfig.outputDirectory = outputFile.empty() ? "." : outputFile.substr(0, outputFile.find_last_of('/'));
        
        Dispatcher::CompilerDispatcher dispatcher(dispatcherConfig);
        
        // 进行完整编译
        auto result = dispatcher.Compile(sourceCode, inputFile);
        
        if (result.success) {
            // 输出编译结果
            if (!outputFile.empty()) {
                std::ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    outFile << result.htmlOutput;
                    outFile.close();
                    std::cout << "编译成功！输出文件: " << outputFile << "\n";
                } else {
                    std::cerr << "无法写入输出文件: " << outputFile << "\n";
                    return 1;
                }
            } else {
                // 如果没有指定输出文件，输出到控制台
                std::cout << "\n=== 编译结果 ===\n";
                std::cout << result.htmlOutput << "\n";
            }
            
            if (verbose) {
                std::cout << "\n=== CSS输出 ===\n" << result.cssOutput << "\n";
                std::cout << "\n=== JavaScript输出 ===\n" << result.javascriptOutput << "\n";
            }
        } else {
            std::cerr << "编译失败！\n";
            for (const auto& error : result.errors) {
                std::cerr << "错误: " << error << "\n";
            }
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "编译器调度异常: " << e.what() << "\n";
        return 1;
    }
    
    // 检查是否有错误
    if (Utils::ErrorHandler::GetInstance().HasErrors()) {
        std::cerr << "编译过程中发现错误:\n";
        Utils::ErrorHandler::GetInstance().PrintErrors();
        return 1;
    }
    
    if (Utils::ErrorHandler::GetInstance().HasWarnings() && verbose) {
        std::cout << "编译过程中发现警告:\n";
        Utils::ErrorHandler::GetInstance().PrintErrors();
    }
    
    return 0;
}