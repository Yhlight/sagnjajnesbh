#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include "dispatcher/CompilerDispatcher.h"
#include "utils/Logger.h"
#include "utils/CommandLineParser.h"

namespace fs = std::filesystem;

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL 编译器 v1.0.0\n";
    std::cout << "用法: " << programName << " [选项] <输入文件>\n\n";
    std::cout << "选项:\n";
    std::cout << "  -o, --output <文件>     指定输出文件 (默认: output.html)\n";
    std::cout << "  -m, --module <路径>     指定模块搜索路径\n";
    std::cout << "  -d, --debug            启用调试模式\n";
    std::cout << "  -v, --verbose          显示详细输出\n";
    std::cout << "  --pack-cmod <文件夹>   打包CMOD模块\n";
    std::cout << "  --pack-cjmod <文件夹>  打包CJMOD模块\n";
    std::cout << "  -h, --help             显示帮助信息\n";
}

int main(int argc, char* argv[]) {
    try {
        // 解析命令行参数
        chtl::utils::CommandLineParser parser(argc, argv);
        
        if (parser.HasOption("-h") || parser.HasOption("--help") || argc == 1) {
            PrintUsage(argv[0]);
            return 0;
        }
        
        // 处理模块打包
        if (parser.HasOption("--pack-cmod")) {
            std::string folderPath = parser.GetOptionValue("--pack-cmod");
            // TODO: 实现CMOD打包逻辑
            std::cout << "打包CMOD模块: " << folderPath << std::endl;
            return 0;
        }
        
        if (parser.HasOption("--pack-cjmod")) {
            std::string folderPath = parser.GetOptionValue("--pack-cjmod");
            // TODO: 实现CJMOD打包逻辑
            std::cout << "打包CJMOD模块: " << folderPath << std::endl;
            return 0;
        }
        
        // 获取输入文件
        std::vector<std::string> args = parser.GetPositionalArgs();
        if (args.empty()) {
            std::cerr << "错误: 未指定输入文件\n";
            PrintUsage(argv[0]);
            return 1;
        }
        
        std::string inputFile = args[0];
        if (!fs::exists(inputFile)) {
            std::cerr << "错误: 输入文件不存在: " << inputFile << std::endl;
            return 1;
        }
        
        // 设置输出文件
        std::string outputFile = parser.GetOptionValue("-o", "output.html");
        if (parser.HasOption("--output")) {
            outputFile = parser.GetOptionValue("--output");
        }
        
        // 配置日志
        chtl::utils::Logger& logger = chtl::utils::Logger::GetInstance();
        if (parser.HasOption("-d") || parser.HasOption("--debug")) {
            logger.SetLevel(chtl::utils::LogLevel::DEBUG);
        }
        if (parser.HasOption("-v") || parser.HasOption("--verbose")) {
            logger.SetLevel(chtl::utils::LogLevel::VERBOSE);
        }
        
        // 设置模块路径
        std::string modulePath = parser.GetOptionValue("-m", "");
        if (parser.HasOption("--module")) {
            modulePath = parser.GetOptionValue("--module");
        }
        
        // 创建编译器调度器
        auto dispatcher = std::make_unique<chtl::CompilerDispatcher>();
        if (!modulePath.empty()) {
            dispatcher->SetModulePath(modulePath);
        }
        
        // 编译文件
        logger.Info("开始编译: " + inputFile);
        bool success = dispatcher->Compile(inputFile, outputFile);
        
        if (success) {
            logger.Info("编译成功: " + outputFile);
            return 0;
        } else {
            logger.Error("编译失败");
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}