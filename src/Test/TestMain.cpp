#include "Framework/TestRunner.h"
#include "Unit/NamespaceAlgorithmsTest.h"
#include "Integration/ModuleSystemTest.h"
#include "Performance/CompilerPerformanceTest.h"
#include <iostream>
#include <string>

using namespace chtl::test::framework;
using namespace chtl::test::unit;
using namespace chtl::test::integration;
using namespace chtl::test::performance;

/**
 * CHTL测试框架主程序
 * 提供完整的单元测试、集成测试和性能测试
 */

void printUsage() {
    std::cout << "CHTL测试框架 v1.0.0\n" << std::endl;
    std::cout << "用法: TestMain [选项] [测试套件]\n" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help              显示帮助信息" << std::endl;
    std::cout << "  -v, --verbose           详细输出模式" << std::endl;
    std::cout << "  -p, --parallel          并行执行测试" << std::endl;
    std::cout << "  -j N                    设置最大并行数 (默认: 4)" << std::endl;
    std::cout << "  -f, --format FORMAT     输出格式 (console|json|xml)" << std::endl;
    std::cout << "  -o, --output FILE       输出报告到文件" << std::endl;
    std::cout << "  -c, --category CAT      只运行指定分类的测试" << std::endl;
    std::cout << "  -n, --name PATTERN      只运行匹配名称模式的测试" << std::endl;
    std::cout << "  --list                  列出所有可用的测试套件" << std::endl;
    std::cout << "  --unit                  只运行单元测试" << std::endl;
    std::cout << "  --integration           只运行集成测试" << std::endl;
    std::cout << "  --performance           只运行性能测试" << std::endl;
    std::cout << std::endl;
    std::cout << "测试套件:" << std::endl;
    std::cout << "  NamespaceAlgorithms     命名空间算法测试" << std::endl;
    std::cout << "  ModuleSystem            模块系统测试" << std::endl;
    std::cout << "  CompilerPerformance     编译器性能测试" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  TestMain --unit                    # 运行所有单元测试" << std::endl;
    std::cout << "  TestMain -v NamespaceAlgorithms    # 详细模式运行命名空间测试" << std::endl;
    std::cout << "  TestMain -p --performance          # 并行运行性能测试" << std::endl;
    std::cout << "  TestMain -c Performance -o report.json  # 运行性能分类测试并输出JSON报告" << std::endl;
}

int main(int argc, char* argv[]) {
    TestRunner runner;
    
    // 解析命令行参数
    bool verbose = false;
    bool parallel = false;
    bool listSuites = false;
    bool unitOnly = false;
    bool integrationOnly = false;
    bool performanceOnly = false;
    int maxParallel = 4;
    std::string outputFormat = "console";
    std::string outputFile;
    std::string category;
    std::string namePattern;
    std::vector<std::string> specificSuites;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-p" || arg == "--parallel") {
            parallel = true;
        } else if (arg == "-j" && i + 1 < argc) {
            maxParallel = std::stoi(argv[++i]);
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 < argc) {
                outputFormat = argv[++i];
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg == "-c" || arg == "--category") {
            if (i + 1 < argc) {
                category = argv[++i];
            }
        } else if (arg == "-n" || arg == "--name") {
            if (i + 1 < argc) {
                namePattern = argv[++i];
            }
        } else if (arg == "--list") {
            listSuites = true;
        } else if (arg == "--unit") {
            unitOnly = true;
        } else if (arg == "--integration") {
            integrationOnly = true;
        } else if (arg == "--performance") {
            performanceOnly = true;
        } else if (arg[0] != '-') {
            specificSuites.push_back(arg);
        }
    }
    
    // 配置测试运行器
    runner.setVerbose(verbose);
    runner.setParallel(parallel);
    runner.setMaxParallel(maxParallel);
    runner.setOutputFormat(outputFormat);
    
    if (!outputFile.empty()) {
        runner.setReportPath(outputFile);
    }
    
    if (!category.empty()) {
        runner.setCategoryFilter(category);
    }
    
    if (!namePattern.empty()) {
        runner.setNameFilter(namePattern);
    }
    
    // 添加测试套件
    if (unitOnly || (!integrationOnly && !performanceOnly && specificSuites.empty())) {
        runner.addSuite(std::make_unique<NamespaceAlgorithmsTest>());
    }
    
    if (integrationOnly || (!unitOnly && !performanceOnly && specificSuites.empty())) {
        runner.addSuite(std::make_unique<ModuleSystemTest>());
    }
    
    if (performanceOnly || (!unitOnly && !integrationOnly && specificSuites.empty())) {
        runner.addSuite(std::make_unique<CompilerPerformanceTest>());
    }
    
    // 添加指定的测试套件
    for (const auto& suiteName : specificSuites) {
        if (suiteName == "NamespaceAlgorithms") {
            runner.addSuite(std::make_unique<NamespaceAlgorithmsTest>());
        } else if (suiteName == "ModuleSystem") {
            runner.addSuite(std::make_unique<ModuleSystemTest>());
        } else if (suiteName == "CompilerPerformance") {
            runner.addSuite(std::make_unique<CompilerPerformanceTest>());
        } else {
            std::cerr << "⚠️ 警告: 未知的测试套件 " << suiteName << std::endl;
        }
    }
    
    // 列出测试套件
    if (listSuites) {
        runner.listAvailableSuites();
        return 0;
    }
    
    // 运行测试
    std::cout << "🚀 启动CHTL测试框架..." << std::endl;
    std::cout << "配置: " << (verbose ? "详细模式" : "简洁模式") 
              << ", " << (parallel ? "并行执行" : "顺序执行") << std::endl;
    
    TestStatistics stats;
    
    if (!category.empty()) {
        stats = runner.runCategory(category);
    } else {
        stats = runner.runAllSuites();
    }
    
    // 输出最终结果
    std::cout << "\n🎊 测试完成！" << std::endl;
    std::cout << "总体成功率: " << std::fixed << std::setprecision(1) << stats.successRate << "%" << std::endl;
    
    if (stats.failedTests > 0 || stats.errorTests > 0) {
        std::cout << "❌ 存在失败的测试，请检查详细报告" << std::endl;
        return 1;
    } else {
        std::cout << "✅ 所有测试通过！" << std::endl;
        return 0;
    }
}