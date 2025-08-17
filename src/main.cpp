#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/CHTLCommon.h"
#include "../include/CompilerDispatcher.h"

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << programName << " [options] <input-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>     指定输出文件 (默认: output.html)\n";
    std::cout << "  -d, --debug             启用调试模式\n";
    std::cout << "  -m, --module-path <dir> 添加模块搜索路径\n";
    std::cout << "  -h, --help              显示此帮助信息\n";
    std::cout << "  -v, --version           显示版本信息\n";
    std::cout << "\n";
    std::cout << "示例:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html -d input.chtl\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "基于C++17实现的超文本语言编译器\n";
    std::cout << "使用MIT开源协议\n";
}

chtl::CompilerOptions parseArguments(int argc, char* argv[], std::string& inputFile) {
    chtl::CompilerOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        }
        else if (arg == "-v" || arg == "--version") {
            printVersion();
            exit(0);
        }
        else if (arg == "-d" || arg == "--debug") {
            options.debugMode = true;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputPath = argv[++i];
            } else {
                std::cerr << "错误: " << arg << " 需要一个参数\n";
                exit(1);
            }
        }
        else if (arg == "-m" || arg == "--module-path") {
            if (i + 1 < argc) {
                options.modulePaths.push_back(argv[++i]);
            } else {
                std::cerr << "错误: " << arg << " 需要一个参数\n";
                exit(1);
            }
        }
        else if (arg[0] == '-') {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            exit(1);
        }
        else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "错误: 只能指定一个输入文件\n";
                exit(1);
            }
        }
    }
    
    return options;
}

int main(int argc, char* argv[]) {
    try {
        std::string inputFile;
        chtl::CompilerOptions options = parseArguments(argc, argv, inputFile);
        
        if (inputFile.empty()) {
            std::cerr << "错误: 未指定输入文件\n";
            printUsage(argv[0]);
            return 1;
        }
        
        // 检查输入文件是否存在
        std::ifstream file(inputFile);
        if (!file.good()) {
            std::cerr << "错误: 无法打开输入文件 '" << inputFile << "'\n";
            return 1;
        }
        file.close();
        
        if (options.debugMode) {
            std::cout << "CHTL编译器启动 (调试模式)\n";
            std::cout << "输入文件: " << inputFile << "\n";
            std::cout << "输出文件: " << options.outputPath << "\n";
        }
        
        // 创建编译器调度器
        chtl::CompilerDispatcher dispatcher(options);
        
        // 编译文件
        chtl::CompilerResult result = dispatcher.compileFile(inputFile);
        
        if (result.success) {
            // 写入输出文件
            std::ofstream outputFile(options.outputPath);
            if (outputFile.good()) {
                outputFile << result.output;
                outputFile.close();
                
                if (options.debugMode) {
                    const auto& stats = dispatcher.getStats();
                    std::cout << "编译成功!\n";
                    std::cout << "统计信息:\n";
                    std::cout << "  CHTL片段: " << stats.chtlFragments << "\n";
                    std::cout << "  CHTL JS片段: " << stats.chtlJSFragments << "\n";
                    std::cout << "  CSS片段: " << stats.cssFragments << "\n";
                    std::cout << "  JavaScript片段: " << stats.jsFragments << "\n";
                    std::cout << "  HTML片段: " << stats.htmlFragments << "\n";
                    std::cout << "  编译时间: " << stats.compilationTime << "ms\n";
                }
                
                std::cout << "输出已写入: " << options.outputPath << "\n";
            } else {
                std::cerr << "错误: 无法写入输出文件 '" << options.outputPath << "'\n";
                return 1;
            }
        } else {
            std::cerr << "编译失败:\n";
            for (const auto& error : result.errors) {
                std::cerr << "错误";
                if (!error.filename.empty()) {
                    std::cerr << " (" << error.filename;
                    if (error.line > 0) {
                        std::cerr << ":" << error.line;
                        if (error.column > 0) {
                            std::cerr << ":" << error.column;
                        }
                    }
                    std::cerr << ")";
                }
                std::cerr << ": " << error.message << "\n";
            }
            
            if (!result.warnings.empty()) {
                std::cerr << "\n警告:\n";
                for (const auto& warning : result.warnings) {
                    std::cerr << "警告: " << warning << "\n";
                }
            }
            
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "致命错误: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "未知错误发生\n";
        return 1;
    }
    
    return 0;
}