#include "include/ModuleManager.h"
#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <filesystem>

using namespace chtl;
namespace fs = std::filesystem;

void printUsage() {
    std::cout << "CHTL模块管理工具 v1.0" << std::endl;
    std::cout << "用法:" << std::endl;
    std::cout << "  chtl_module_tool <命令> [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "命令:" << std::endl;
    std::cout << "  pack-cmod <源目录> [输出路径]     - 打包CMOD模块" << std::endl;
    std::cout << "  pack-cjmod <源目录> [输出路径]    - 打包CJMOD模块" << std::endl;
    std::cout << "  unpack-cmod <CMOD文件> [输出目录] - 解压CMOD模块" << std::endl;
    std::cout << "  unpack-cjmod <CJMOD文件> [输出目录] - 解压CJMOD模块" << std::endl;
    std::cout << "  test-import <CHTL文件>            - 测试导入功能" << std::endl;
    std::cout << "  discover [搜索路径]               - 发现模块" << std::endl;
    std::cout << "  compile <CHTL文件>                - 完整编译测试" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  chtl_module_tool pack-cmod ./MyModule" << std::endl;
    std::cout << "  chtl_module_tool test-import test.chtl" << std::endl;
    std::cout << "  chtl_module_tool compile example.chtl" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    String command = argv[1];
    String currentDir = fs::current_path().string();
    
    // 创建模块管理器
    auto moduleManager = ModuleManagerFactory::createWithDefaults();
    
    try {
        if (command == "pack-cmod") {
            if (argc < 3) {
                std::cerr << "错误: 缺少源目录参数" << std::endl;
                return 1;
            }
            
            String sourceDir = argv[2];
            String outputPath = (argc >= 4) ? argv[3] : "";
            
            std::cout << "正在打包CMOD模块: " << sourceDir << std::endl;
            auto result = moduleManager->packCmod(sourceDir, outputPath);
            
            if (result.success) {
                std::cout << "✅ 打包成功!" << std::endl;
                std::cout << "输出文件: " << result.outputPath << std::endl;
                std::cout << "包含文件数: " << result.fileCount << std::endl;
            } else {
                std::cerr << "❌ 打包失败: " << result.error << std::endl;
                return 1;
            }
        }
        else if (command == "pack-cjmod") {
            if (argc < 3) {
                std::cerr << "错误: 缺少源目录参数" << std::endl;
                return 1;
            }
            
            String sourceDir = argv[2];
            String outputPath = (argc >= 4) ? argv[3] : "";
            
            std::cout << "正在打包CJMOD模块: " << sourceDir << std::endl;
            auto result = moduleManager->packCJmod(sourceDir, outputPath);
            
            if (result.success) {
                std::cout << "✅ 打包成功!" << std::endl;
                std::cout << "输出文件: " << result.outputPath << std::endl;
                std::cout << "包含文件数: " << result.fileCount << std::endl;
            } else {
                std::cerr << "❌ 打包失败: " << result.error << std::endl;
                return 1;
            }
        }
        else if (command == "unpack-cmod") {
            if (argc < 3) {
                std::cerr << "错误: 缺少CMOD文件参数" << std::endl;
                return 1;
            }
            
            String cmodPath = argv[2];
            String outputDir = (argc >= 4) ? argv[3] : "";
            
            std::cout << "正在解压CMOD模块: " << cmodPath << std::endl;
            bool success = moduleManager->unpackCmod(cmodPath, outputDir);
            
            if (!success) {
                std::cerr << "❌ 解压失败" << std::endl;
                return 1;
            }
        }
        else if (command == "unpack-cjmod") {
            if (argc < 3) {
                std::cerr << "错误: 缺少CJMOD文件参数" << std::endl;
                return 1;
            }
            
            String cjmodPath = argv[2];
            String outputDir = (argc >= 4) ? argv[3] : "";
            
            std::cout << "正在解压CJMOD模块: " << cjmodPath << std::endl;
            bool success = moduleManager->unpackCJmod(cjmodPath, outputDir);
            
            if (!success) {
                std::cerr << "❌ 解压失败" << std::endl;
                return 1;
            }
        }
        else if (command == "test-import") {
            if (argc < 3) {
                std::cerr << "错误: 缺少CHTL文件参数" << std::endl;
                return 1;
            }
            
            String chtlFile = argv[2];
            
            std::cout << "测试导入功能: " << chtlFile << std::endl;
            
            // 创建测试导入节点
            std::vector<ImportNode> testImports = {
                ImportNode(ImportNode::ImportType::HTML, "test.html", "MyHtml"),
                ImportNode(ImportNode::ImportType::STYLE, "style.css", "MyStyle"),
                ImportNode(ImportNode::ImportType::JAVASCRIPT, "script.js", "MyScript"),
                ImportNode(ImportNode::ImportType::CHTL, "TestModule"),
                ImportNode(ImportNode::ImportType::CJMOD, "TestCJmod")
            };
            
            auto results = moduleManager->loadMultipleModules(testImports);
            
            std::cout << "导入测试结果:" << std::endl;
            for (size_t i = 0; i < results.size(); ++i) {
                const auto& result = results[i];
                std::cout << "  " << (i + 1) << ". ";
                
                if (result.success) {
                    std::cout << "✅ " << result.modulePath;
                    if (!result.moduleAlias.empty()) {
                        std::cout << " (as " << result.moduleAlias << ")";
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "❌ " << testImports[i].getPath() << " - " << result.error << std::endl;
                }
            }
        }
        else if (command == "discover") {
            String searchPath = (argc >= 3) ? argv[2] : "";
            
            std::cout << "发现模块..." << std::endl;
            
            auto cmodModules = moduleManager->discoverModules(searchPath);
            auto cjmodModules = moduleManager->discoverCJmodules(searchPath);
            
            std::cout << "找到的CMOD/CHTL模块 (" << cmodModules.size() << " 个):" << std::endl;
            for (const auto& module : cmodModules) {
                std::cout << "  - " << module << std::endl;
            }
            
            std::cout << "找到的CJMOD模块 (" << cjmodModules.size() << " 个):" << std::endl;
            for (const auto& module : cjmodModules) {
                std::cout << "  - " << module << std::endl;
            }
        }
        else if (command == "compile") {
            if (argc < 3) {
                std::cerr << "错误: 缺少CHTL文件参数" << std::endl;
                return 1;
            }
            
            String chtlFile = argv[2];
            
            std::cout << "完整编译测试: " << chtlFile << std::endl;
            
            // 读取文件
            std::ifstream file(chtlFile);
            if (!file.is_open()) {
                std::cerr << "❌ 无法打开文件: " << chtlFile << std::endl;
                return 1;
            }
            
            String content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            
            // 解析
            CompilerOptions options;
            CHTLParser parser(options);
            auto parseResult = parser.parse(content, chtlFile);
            
            if (!parseResult.success) {
                std::cerr << "❌ 解析失败:" << std::endl;
                for (const auto& error : parseResult.errors) {
                    std::cerr << "  " << error.message << std::endl;
                }
                return 1;
            }
            
            // 生成
            CHTLGenerator generator(options);
            auto generateResult = generator.generate(parseResult.ast);
            
            if (!generateResult.success) {
                std::cerr << "❌ 生成失败:" << std::endl;
                for (const auto& error : generateResult.errors) {
                    std::cerr << "  " << error.message << std::endl;
                }
                return 1;
            }
            
            // 输出结果
            String outputFile = fs::path(chtlFile).stem().string() + "_output.html";
            std::ofstream outFile(outputFile);
            
            outFile << "<!DOCTYPE html>" << std::endl;
            outFile << "<html>" << std::endl;
            outFile << "<head>" << std::endl;
            outFile << "<meta charset=\"UTF-8\">" << std::endl;
            outFile << "<title>CHTL编译结果</title>" << std::endl;
            
            if (!generateResult.cssOutput.empty()) {
                outFile << "<style>" << std::endl;
                outFile << generateResult.cssOutput << std::endl;
                outFile << "</style>" << std::endl;
            }
            
            outFile << "</head>" << std::endl;
            outFile << "<body>" << std::endl;
            outFile << generateResult.htmlOutput << std::endl;
            
            if (!generateResult.jsOutput.empty()) {
                outFile << "<script>" << std::endl;
                outFile << generateResult.jsOutput << std::endl;
                outFile << "</script>" << std::endl;
            }
            
            outFile << "</body>" << std::endl;
            outFile << "</html>" << std::endl;
            outFile.close();
            
            std::cout << "✅ 编译成功!" << std::endl;
            std::cout << "输出文件: " << outputFile << std::endl;
        }
        else {
            std::cerr << "错误: 未知命令 '" << command << "'" << std::endl;
            printUsage();
            return 1;
        }
        
        // 输出统计信息
        auto stats = moduleManager->getStats();
        std::cout << std::endl << "=== 模块管理统计 ===" << std::endl;
        std::cout << "加载的模块数: " << stats.loadedModules << std::endl;
        std::cout << "缓存的导入数: " << stats.cachedImports << std::endl;
        std::cout << "注册的命名空间数: " << stats.namespacesRegistered << std::endl;
        std::cout << "检测到的循环依赖数: " << stats.circularDependenciesDetected << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 执行失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}