#include "CHTLCompiler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

void PrintUsage(const char* program) {
    std::cout << "CHTL Compiler v2.0\n";
    std::cout << "Usage: " << program << " [options] <input.chtl> [output.html]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help           Show this help message\n";
    std::cout << "  -d, --debug          Enable debug mode\n";
    std::cout << "  -m, --minify         Disable pretty printing\n";
    std::cout << "  -f, --fragment       Generate HTML fragment only (no <html>, <head>, etc.)\n";
    std::cout << "  -o, --output <file>  Specify output file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program << " input.chtl\n";
    std::cout << "  " << program << " -d input.chtl output.html\n";
    std::cout << "  " << program << " --fragment input.chtl -o output.html\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    bool debug = false;
    bool prettyPrint = true;
    bool fullDocument = true;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "-m" || arg == "--minify") {
            prettyPrint = false;
        } else if (arg == "-f" || arg == "--fragment") {
            fullDocument = false;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else if (outputFile.empty()) {
                outputFile = arg;
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        return 1;
    }
    
    // 如果没有指定输出文件，使用默认名称
    if (outputFile.empty()) {
        size_t lastDot = inputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputFile = inputFile.substr(0, lastDot) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    // 创建编译器并设置选项
    chtl::simple::CHTLCompiler compiler;
    compiler.SetDebugMode(debug);
    compiler.SetPrettyPrint(prettyPrint);
    compiler.SetGenerateFullDocument(fullDocument);
    
    // 编译文件
    std::string html = compiler.CompileFile(inputFile);
    
    // 检查错误
    if (compiler.HasErrors()) {
        std::cerr << "Compilation failed with errors:\n";
        for (const auto& error : compiler.GetAllErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    
    // 输出结果
    if (outputFile == "-") {
        std::cout << html;
    } else {
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot write to file " << outputFile << "\n";
            return 1;
        }
        output << html;
        output.close();
        
        if (debug) {
            std::cout << "Successfully compiled " << inputFile << " to " << outputFile << "\n";
        }
    }
    
    return 0;
}