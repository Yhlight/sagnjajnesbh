#include "SimpleCHTLCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void WriteFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write to file " << filename << std::endl;
        return;
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "CHTL Simple Compiler v1.0\n";
        std::cout << "Usage: chtl-simple <input.chtl> [output.html]\n";
        std::cout << "\nExample:\n";
        std::cout << "  chtl-simple test.chtl\n";
        std::cout << "  chtl-simple test.chtl output.html\n";
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "output.html";
    
    // 读取输入文件
    std::string chtlCode = ReadFile(inputFile);
    if (chtlCode.empty()) {
        return 1;
    }
    
    // 编译
    chtl::simple::SimpleCHTLCompiler compiler;
    std::string html = compiler.Compile(chtlCode);
    
    // 检查错误
    if (compiler.HasErrors()) {
        std::cerr << "Compilation errors:\n";
        for (const auto& error : compiler.GetErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    // 输出结果
    if (outputFile == "-") {
        std::cout << html;
    } else {
        WriteFile(outputFile, html);
        std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;
    }
    
    return 0;
}