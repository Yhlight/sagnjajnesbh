#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "Core/CompilerDispatcher.hpp"
#include "Core/CHTLTypes.hpp"

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -o, --output <文件>    指定输出文件\n";
    std::cout << "  -d, --debug           启用调试模式\n";
    std::cout << "  -O, --optimize        启用优化\n";
    std::cout << "  -h, --help            显示此帮助信息\n";
    std::cout << "  -v, --version         显示版本信息\n";
    std::cout << "\n示例:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
    std::cout << "  " << programName << " -d --optimize input.chtl\n";
}

void PrintVersion() {
    std::cout << "CHTL编译器 v1.0.0\n";
    std::cout << "基于C++17实现的超文本语言编译器\n";
    std::cout << "MIT开源协议\n";
}

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string content(size, '\0');
    file.read(content.data(), size);
    
    return content;
}

void WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("无法写入文件: " + filename);
    }
    
    file << content;
}

int main(int argc, char* argv[]) {
    // 设置UTF-8编码支持
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    bool debugMode = false;
    bool optimizeMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            PrintVersion();
            return 0;
        }
        else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        }
        else if (arg == "-O" || arg == "--optimize") {
            optimizeMode = true;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "错误: " << arg << " 选项需要一个参数\n";
                return 1;
            }
            outputFile = argv[++i];
        }
        else if (arg.front() == '-') {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            return 1;
        }
        else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件\n";
                return 1;
            }
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件\n";
        PrintUsage(argv[0]);
        return 1;
    }
    
    // 如果未指定输出文件，则根据输入文件生成
    if (outputFile.empty()) {
        size_t dotPos = inputFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            outputFile = inputFile.substr(0, dotPos) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    try {
        // 读取输入文件
        std::cout << "正在读取文件: " << inputFile << "\n";
        std::string sourceCode = ReadFile(inputFile);
        
        // 创建编译器调度器
        CHTL::CompilerDispatcher dispatcher;
        dispatcher.SetCompilationOptions(debugMode, optimizeMode);
        
        if (debugMode) {
            std::cout << "调试模式已启用\n";
        }
        if (optimizeMode) {
            std::cout << "优化模式已启用\n";
        }
        
        // 编译
        std::cout << "正在编译...\n";
        auto startTime = std::chrono::high_resolution_clock::now();
        
        CHTL::CompilationResult result = dispatcher.Compile(sourceCode, inputFile);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 检查编译结果
        if (result.success) {
            // 写入输出文件
            WriteFile(outputFile, result.output);
            
            std::cout << "编译成功！\n";
            std::cout << "输出文件: " << outputFile << "\n";
            std::cout << "编译时间: " << duration.count() << "ms\n";
            
            // 显示统计信息
            if (debugMode) {
                auto stats = dispatcher.GetStatistics();
                std::cout << "\n编译统计:\n";
                std::cout << "  总文件数: " << stats.totalFiles << "\n";
                std::cout << "  成功编译: " << stats.successfulCompilations << "\n";
                std::cout << "  编译失败: " << stats.failedCompilations << "\n";
                std::cout << "  平均编译时间: " << stats.averageCompileTime << "ms\n";
            }
            
            return 0;
        } else {
            // 显示错误信息
            std::cerr << "编译失败！\n";
            for (const auto& error : result.errors) {
                std::cerr << "错误 [" << error.filename << ":" << error.line << ":" << error.column << "]: " 
                         << error.message << "\n";
            }
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}