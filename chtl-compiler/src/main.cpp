#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include "scanner/chtl_unified_scanner.h"
#include "dispatcher/compiler_dispatcher.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input.chtl>\n"
              << "\nOptions:\n"
              << "  -o <output>     Specify output file (default: output.html)\n"
              << "  -h, --help      Show this help message\n"
              << "  -v, --version   Show version information\n"
              << "  --title <title> Set HTML document title\n"
              << "  --lang <lang>   Set HTML language (default: en)\n"
              << "  --charset <cs>  Set charset (default: UTF-8)\n"
              << "\nExamples:\n"
              << "  " << programName << " input.chtl\n"
              << "  " << programName << " -o index.html --title \"My Page\" input.chtl\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n"
              << "Copyright (c) 2024 CHTL Team\n"
              << "MIT License\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }
    
    file << content;
    file.close();
}

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile = "output.html";
    std::string title;
    std::string lang = "en";
    std::string charset = "UTF-8";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--title" && i + 1 < argc) {
            title = argv[++i];
        } else if (arg == "--lang" && i + 1 < argc) {
            lang = argv[++i];
        } else if (arg == "--charset" && i + 1 < argc) {
            charset = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // 检查输入文件
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        // 读取输入文件
        std::cout << "Reading " << inputFile << "...\n";
        std::string source = readFile(inputFile);
        
        // 创建扫描器和调度器
        chtl::CHTLUnifiedScanner scanner;
        chtl::CompilerDispatcher dispatcher;
        
        // 设置编译选项
        if (!title.empty()) {
            dispatcher.setOption("title", title);
        }
        dispatcher.setOption("lang", lang);
        dispatcher.setOption("charset", charset);
        
        // 扫描源代码
        std::cout << "Scanning CHTL code...\n";
        auto fragments = scanner.scan(source);
        
        // 检查扫描错误
        if (!scanner.getErrors().empty()) {
            std::cerr << "Scanner errors:\n";
            for (const auto& error : scanner.getErrors()) {
                std::cerr << "  " << error << "\n";
            }
            return 1;
        }
        
        std::cout << "Found " << fragments.size() << " code fragments\n";
        
        // 编译代码片段
        std::cout << "Compiling fragments...\n";
        auto result = dispatcher.dispatch(fragments);
        
        // 检查编译结果
        if (!result.success) {
            std::cerr << "Compilation failed!\n";
            if (!result.errors.empty()) {
                std::cerr << "Errors:\n";
                for (const auto& error : result.errors) {
                    std::cerr << "  " << error << "\n";
                }
            }
            return 1;
        }
        
        // 显示警告
        if (!result.warnings.empty()) {
            std::cout << "Warnings:\n";
            for (const auto& warning : result.warnings) {
                std::cout << "  " << warning << "\n";
            }
        }
        
        // 生成HTML
        std::cout << "Generating HTML...\n";
        std::string html = dispatcher.getCompiledHTML();
        
        // 写入输出文件
        std::cout << "Writing to " << outputFile << "...\n";
        writeFile(outputFile, html);
        
        std::cout << "Compilation successful!\n";
        std::cout << "Output written to: " << outputFile << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}