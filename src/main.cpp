#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <filesystem>

#include "../include/common/Error.h"
#include "../include/common/Context.h"
#include "../include/scanner/UnifiedScanner.h"

namespace fs = std::filesystem;

namespace chtl {

// 版本信息
const char* VERSION = "1.0.0";
const char* BUILD_DATE = __DATE__;

// 命令行选项
struct CommandLineOptions {
    std::string inputFile;
    std::string outputFile;
    std::string outputDir = "./output";
    bool debugMode = false;
    bool verboseMode = false;
    bool showVersion = false;
    bool showHelp = false;
    bool checkOnly = false;  // 仅检查语法，不生成输出
    std::vector<std::string> includePaths;
    std::vector<std::string> modulePaths;
};

// 显示版本信息
void showVersion() {
    std::cout << "CHTL Compiler Version " << VERSION << "\n";
    std::cout << "Build Date: " << BUILD_DATE << "\n";
    std::cout << "Copyright (c) 2024 CHTL Team\n";
    std::cout << "License: MIT\n";
}

// 显示帮助信息
void showHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input-file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>      指定输出文件名\n";
    std::cout << "  -d, --output-dir <dir>   指定输出目录 (默认: ./output)\n";
    std::cout << "  -I, --include <path>     添加包含路径\n";
    std::cout << "  -M, --module <path>      添加模块搜索路径\n";
    std::cout << "  --check                  仅检查语法，不生成输出\n";
    std::cout << "  --debug                  启用调试模式\n";
    std::cout << "  -v, --verbose            启用详细输出\n";
    std::cout << "  --version                显示版本信息\n";
    std::cout << "  -h, --help               显示此帮助信息\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
    std::cout << "  " << programName << " --debug -v input.chtl\n";
}

// 解析命令行参数
CommandLineOptions parseCommandLine(int argc, char* argv[]) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                throw std::runtime_error("选项 " + arg + " 需要参数");
            }
        } else if (arg == "-d" || arg == "--output-dir") {
            if (i + 1 < argc) {
                options.outputDir = argv[++i];
            } else {
                throw std::runtime_error("选项 " + arg + " 需要参数");
            }
        } else if (arg == "-I" || arg == "--include") {
            if (i + 1 < argc) {
                options.includePaths.push_back(argv[++i]);
            } else {
                throw std::runtime_error("选项 " + arg + " 需要参数");
            }
        } else if (arg == "-M" || arg == "--module") {
            if (i + 1 < argc) {
                options.modulePaths.push_back(argv[++i]);
            } else {
                throw std::runtime_error("选项 " + arg + " 需要参数");
            }
        } else if (arg == "--check") {
            options.checkOnly = true;
        } else if (arg == "--debug") {
            options.debugMode = true;
        } else if (arg == "-v" || arg == "--verbose") {
            options.verboseMode = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        } else if (arg[0] == '-') {
            throw std::runtime_error("未知选项: " + arg);
        } else {
            // 输入文件
            if (options.inputFile.empty()) {
                options.inputFile = arg;
            } else {
                throw std::runtime_error("只能指定一个输入文件");
            }
        }
    }
    
    return options;
}

// 读取文件内容
std::string readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 写入文件
void writeFile(const std::string& filename, const std::string& content) {
    // 创建输出目录
    fs::path filePath(filename);
    fs::path dirPath = filePath.parent_path();
    if (!dirPath.empty() && !fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法写入文件: " + filename);
    }
    
    file << content;
}

// 编译单个文件
bool compileFile(const std::string& inputFile, const CommandLineOptions& options) {
    try {
        // 读取输入文件
        if (options.verboseMode) {
            std::cout << "正在读取文件: " << inputFile << "\n";
        }
        std::string content = readFile(inputFile);
        
        // 创建诊断收集器
        auto diagnostics = std::make_unique<DiagnosticCollector>();
        
        // 创建编译上下文
        auto context = std::make_unique<CompileContext>();
        context->setCurrentFile(inputFile);
        context->setDebugMode(options.debugMode);
        
        // 设置路径
        for (const auto& path : options.includePaths) {
            context->getOptions().includePaths.push_back(path);
        }
        for (const auto& path : options.modulePaths) {
            context->getOptions().modulePaths.push_back(path);
        }
        
        // 创建扫描器
        if (options.verboseMode) {
            std::cout << "正在扫描代码...\n";
        }
        UnifiedScanner scanner(inputFile, content, diagnostics.get());
        
        // 扫描代码片段
        auto fragments = scanner.scan();
        
        if (options.debugMode) {
            std::cout << "扫描到 " << fragments.size() << " 个代码片段\n";
            scanner.dumpFragments(fragments);
        }
        
        // 如果只是检查语法
        if (options.checkOnly) {
            if (diagnostics->hasErrors()) {
                diagnostics->print(std::cerr);
                return false;
            }
            std::cout << "语法检查通过\n";
            return true;
        }
        
        // TODO: 调用编译器调度器处理各个片段
        // TODO: 合并编译结果
        // TODO: 生成最终的HTML输出
        
        // 临时：输出诊断信息
        if (diagnostics->hasErrors() || options.verboseMode) {
            diagnostics->print(diagnostics->hasErrors() ? std::cerr : std::cout);
        }
        
        if (diagnostics->hasErrors()) {
            return false;
        }
        
        // 确定输出文件名
        std::string outputFile = options.outputFile;
        if (outputFile.empty()) {
            fs::path inputPath(inputFile);
            outputFile = (fs::path(options.outputDir) / 
                         inputPath.stem()).string() + ".html";
        }
        
        // TODO: 写入输出文件
        if (options.verboseMode) {
            std::cout << "输出文件: " << outputFile << "\n";
        }
        
        std::cout << "编译成功\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return false;
    }
}

} // namespace chtl

int main(int argc, char* argv[]) {
    try {
        // 解析命令行参数
        chtl::CommandLineOptions options = chtl::parseCommandLine(argc, argv);
        
        // 显示版本信息
        if (options.showVersion) {
            chtl::showVersion();
            return 0;
        }
        
        // 显示帮助信息
        if (options.showHelp || options.inputFile.empty()) {
            chtl::showHelp(argv[0]);
            return options.showHelp ? 0 : 1;
        }
        
        // 编译文件
        bool success = chtl::compileFile(options.inputFile, options);
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}