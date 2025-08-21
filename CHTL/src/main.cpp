/**
 * @file main.cpp
 * @brief CHTL编译器主程序入口
 * @author CHTL Team
 * @date 2024
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <locale>
#include <codecvt>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Core/Version.h"
#include "Dispatcher/CompilerDispatcher.h"
#include "Utils/FileUtils.h"
#include "Utils/Logger.h"

namespace fs = std::filesystem;

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL编译器 v" << CHTL_VERSION_MAJOR << "." 
              << CHTL_VERSION_MINOR << "." << CHTL_VERSION_PATCH << "\n\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -o, --output <文件>      指定输出文件\n";
    std::cout << "  -d, --debug              启用调试模式\n";
    std::cout << "  -m, --module <目录>      指定模块搜索目录\n";
    std::cout << "  --pack <目录>            打包目录为CMOD文件\n";
    std::cout << "  --unpack <文件>          解包CMOD文件\n";
    std::cout << "  --pack-cjmod <目录>      打包目录为CJMOD文件\n";
    std::cout << "  --unpack-cjmod <文件>    解包CJMOD文件\n";
    std::cout << "  -h, --help               显示此帮助信息\n";
    std::cout << "  -v, --version            显示版本信息\n\n";
}

void PrintVersion() {
    std::cout << "CHTL编译器 v" << CHTL_VERSION_MAJOR << "." 
              << CHTL_VERSION_MINOR << "." << CHTL_VERSION_PATCH << "\n";
    std::cout << "基于C++17实现的超文本语言编译器\n";
    std::cout << "版权所有 (c) 2024 CHTL Team\n";
    std::cout << "使用MIT开源协议授权\n";
}

int main(int argc, char* argv[]) {
    // 设置UTF-8编码
#ifdef _WIN32
    std::locale::global(std::locale(".UTF-8"));
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    std::locale::global(std::locale("C.UTF-8"));
#endif
    
    // 解析命令行参数
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() < 2) {
        PrintUsage(args[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile;
    bool debugMode = false;
    std::vector<std::string> modulePaths;
    std::string packDir;
    std::string unpackFile;
    bool packCJMod = false;
    
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage(args[0]);
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            PrintVersion();
            return 0;
        }
        else if (arg == "-o" || arg == "--output") {
            if (++i >= args.size()) {
                std::cerr << "错误: " << arg << " 需要参数\n";
                return 1;
            }
            outputFile = args[i];
        }
        else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        }
        else if (arg == "-m" || arg == "--module") {
            if (++i >= args.size()) {
                std::cerr << "错误: " << arg << " 需要参数\n";
                return 1;
            }
            modulePaths.push_back(args[i]);
        }
        else if (arg == "--pack") {
            if (++i >= args.size()) {
                std::cerr << "错误: " << arg << " 需要参数\n";
                return 1;
            }
            packDir = args[i];
            packCJMod = false;
        }
        else if (arg == "--pack-cjmod") {
            if (++i >= args.size()) {
                std::cerr << "错误: " << arg << " 需要参数\n";
                return 1;
            }
            packDir = args[i];
            packCJMod = true;
        }
        else if (arg == "--unpack" || arg == "--unpack-cjmod") {
            if (++i >= args.size()) {
                std::cerr << "错误: " << arg << " 需要参数\n";
                return 1;
            }
            unpackFile = args[i];
        }
        else if (arg[0] != '-') {
            inputFile = arg;
        }
        else {
            std::cerr << "错误: 未知选项 " << arg << "\n";
            PrintUsage(args[0]);
            return 1;
        }
    }
    
    // 初始化日志系统
    auto& logger = Utils::Logger::GetInstance();
    logger.SetDebugMode(debugMode);
    
    try {
        // 处理打包/解包请求
        if (!packDir.empty()) {
            // TODO: 实现模块打包功能
            logger.Info("打包功能尚未实现");
            return 1;
        }
        
        if (!unpackFile.empty()) {
            // TODO: 实现模块解包功能
            logger.Info("解包功能尚未实现");
            return 1;
        }
        
        // 检查输入文件
        if (inputFile.empty()) {
            std::cerr << "错误: 未指定输入文件\n";
            PrintUsage(args[0]);
            return 1;
        }
        
        if (!fs::exists(inputFile)) {
            std::cerr << "错误: 输入文件不存在: " << inputFile << "\n";
            return 1;
        }
        
        // 自动生成输出文件名
        if (outputFile.empty()) {
            fs::path inputPath(inputFile);
            outputFile = (inputPath.parent_path() / inputPath.stem()).string() + ".html";
        }
        
        // 创建编译器调度器
        auto dispatcher = std::make_unique<Dispatcher::CompilerDispatcher>();
        
        // 添加模块搜索路径
        for (const auto& path : modulePaths) {
            dispatcher->AddModulePath(path);
        }
        
        // 添加默认模块路径
        fs::path execPath = fs::path(args[0]).parent_path();
        dispatcher->AddModulePath((execPath / "module").string());
        dispatcher->AddModulePath("./module");
        
        // 编译文件
        logger.Info("编译文件: " + inputFile);
        bool success = dispatcher->CompileFile(inputFile, outputFile);
        
        if (success) {
            logger.Info("编译成功: " + outputFile);
            return 0;
        } else {
            logger.Error("编译失败");
            return 1;
        }
    }
    catch (const std::exception& e) {
        logger.Error("异常: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}