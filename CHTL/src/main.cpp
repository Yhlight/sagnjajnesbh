#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <locale>
#include <codecvt>
#include "CHTL/Compiler/CHTLCompiler.h"

// 设置UTF-8编码
void SetupUTF8() {
#ifdef _WIN32
    // Windows下设置控制台UTF-8
    #include <windows.h>
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    // 设置C++流的locale为UTF-8
    try {
        std::locale::global(std::locale("en_US.UTF-8"));
    } catch (const std::exception&) {
        // 如果设置失败，继续使用默认locale
    }
}

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL编译器 v1.0.0\n\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -o <文件>    指定输出文件（默认：output.html）\n";
    std::cout << "  -m <目录>    指定模块目录（默认：./modules）\n";
    std::cout << "  --debug      启用调试模式\n";
    std::cout << "  --pack       打包CMOD模块\n";
    std::cout << "  --unpack     解包CMOD模块\n";
    std::cout << "  --help       显示此帮助信息\n";
}

int main(int argc, char* argv[]) {
    SetupUTF8();
    
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile = "output.html";
    std::string moduleDir = "./modules";
    bool debugMode = false;
    bool packMode = false;
    bool unpackMode = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            moduleDir = argv[++i];
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--pack") {
            packMode = true;
        } else if (arg == "--unpack") {
            unpackMode = true;
        } else if (arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "错误：未知选项 " << arg << "\n";
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误：未指定输入文件\n";
        return 1;
    }
    
    if (!std::filesystem::exists(inputFile)) {
        std::cerr << "错误：输入文件不存在：" << inputFile << "\n";
        return 1;
    }
    
    std::cout << "CHTL编译器启动...\n";
    std::cout << "输入文件：" << inputFile << "\n";
    std::cout << "输出文件：" << outputFile << "\n";
    std::cout << "模块目录：" << moduleDir << "\n";
    
    if (debugMode) {
        std::cout << "调试模式：已启用\n";
    }
    
    if (packMode) {
        // TODO: 实现打包功能
        std::cerr << "打包功能尚未实现\n";
        return 1;
    }
    
    if (unpackMode) {
        // TODO: 实现解包功能
        std::cerr << "解包功能尚未实现\n";
        return 1;
    }
    
    // 编译CHTL文件
    CHTL::Compiler::CHTLCompilerConfig config;
    config.enableDebug = debugMode;
    config.moduleSearchPath = moduleDir;
    
    CHTL::Compiler::CHTLCompiler compiler(config);
    auto result = compiler.CompileFile(inputFile);
    
    if (!result.success) {
        std::cerr << "\n编译失败！\n";
        for (const auto& error : result.errors) {
            std::cerr << error << "\n";
        }
        return 1;
    }
    
    // 输出警告
    if (!result.warnings.empty()) {
        std::cout << "\n警告：\n";
        for (const auto& warning : result.warnings) {
            std::cout << warning << "\n";
        }
    }
    
    // 生成输出文件
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "错误：无法创建输出文件：" << outputFile << "\n";
        return 1;
    }
    
    // 检查是否已有html元素
    bool hasHtmlElement = result.html.find("<html") != std::string::npos;
    
    if (hasHtmlElement) {
        // 已有完整的HTML结构，需要插入CSS和JS
        std::string output = result.html;
        
        // 在</head>前插入CSS
        if (!result.css.empty()) {
            size_t headEnd = output.find("</head>");
            if (headEnd != std::string::npos) {
                std::string styleTag = "  <style>\n" + result.css + "  </style>\n";
                output.insert(headEnd, styleTag);
            }
        }
        
        // 在</body>前插入JavaScript
        if (!result.javascript.empty()) {
            size_t bodyEnd = output.find("</body>");
            if (bodyEnd != std::string::npos) {
                std::string scriptTag = "  <script>\n" + result.javascript + "  </script>\n";
                output.insert(bodyEnd, scriptTag);
            }
        }
        
        outFile << "<!DOCTYPE html>\n";
        outFile << output;
    } else {
        // 生成完整的HTML文档
        outFile << "<!DOCTYPE html>\n";
        outFile << "<html>\n";
        outFile << "<head>\n";
        outFile << "  <meta charset=\"UTF-8\">\n";
        outFile << "  <title>CHTL Generated Page</title>\n";
        
        // 输出CSS
        if (!result.css.empty()) {
            outFile << "  <style>\n";
            outFile << result.css;
            outFile << "  </style>\n";
        }
        
        outFile << "</head>\n";
        outFile << "<body>\n";
        
        // 输出HTML内容
        outFile << result.html;
        
        // 输出JavaScript
        if (!result.javascript.empty()) {
            outFile << "  <script>\n";
            outFile << result.javascript;
            outFile << "  </script>\n";
        }
        
        outFile << "</body>\n";
        outFile << "</html>\n";
    }
    
    outFile.close();
    
    std::cout << "\n编译成功！输出文件：" << outputFile << "\n";
    
    return 0;
}