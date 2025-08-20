#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include "dispatcher/CompilerDispatcher.h"
#include "utils/Logger.h"
#include "utils/CommandLineParser.h"

namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
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
        
        if (parser.hasOption("-h") || parser.hasOption("--help") || argc == 1) {
            printUsage(argv[0]);
            return 0;
        }
        
        // 处理模块打包
        if (parser.hasOption("--pack-cmod")) {
            std::string folderPath = parser.getOptionValue("--pack-cmod");
            // TODO: 实现CMOD打包逻辑
            std::cout << "打包CMOD模块: " << folderPath << std::endl;
            return 0;
        }
        
        if (parser.hasOption("--pack-cjmod")) {
            std::string folderPath = parser.getOptionValue("--pack-cjmod");
            // TODO: 实现CJMOD打包逻辑
            std::cout << "打包CJMOD模块: " << folderPath << std::endl;
            return 0;
        }
        
        // 获取输入文件
        std::vector<std::string> args = parser.getPositionalArgs();
        if (args.empty()) {
            std::cerr << "错误: 未指定输入文件\n";
            printUsage(argv[0]);
            return 1;
        }
        
        std::string inputFile = args[0];
        if (!fs::exists(inputFile)) {
            std::cerr << "错误: 输入文件不存在: " << inputFile << std::endl;
            return 1;
        }
        
        // 设置输出文件
        std::string outputFile = parser.getOptionValue("-o", "output.html");
        if (parser.hasOption("--output")) {
            outputFile = parser.getOptionValue("--output");
        }
        
        // 配置日志
        chtl::utils::Logger& logger = chtl::utils::Logger::getInstance();
        if (parser.hasOption("-d") || parser.hasOption("--debug")) {
            logger.setLevel(chtl::utils::LogLevel::DEBUG);
        }
        if (parser.hasOption("-v") || parser.hasOption("--verbose")) {
            logger.setLevel(chtl::utils::LogLevel::VERBOSE);
        }
        
        // 设置模块路径
        std::string modulePath = parser.getOptionValue("-m", "");
        if (parser.hasOption("--module")) {
            modulePath = parser.getOptionValue("--module");
        }
        
        // 创建编译器调度器
        auto dispatcher = std::make_unique<chtl::CompilerDispatcher>();
        if (!modulePath.empty()) {
            dispatcher->setModulePath(modulePath);
        }
        
        // 编译文件
        logger.info("开始编译: " + inputFile);
        bool success = dispatcher->compile(inputFile, outputFile);
        
        if (success) {
            logger.info("编译成功: " + outputFile);
            return 0;
        } else {
            logger.error("编译失败");
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}