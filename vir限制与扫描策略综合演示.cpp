#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <vector>
#include <string>

using namespace CHTL::CJMOD;

// 演示vir语法检测和拦截
void demonstrateVirSyntaxDetection() {
    std::cout << "\n🌸 演示：vir语法检测和拦截\n" << std::endl;
    
    // 测试各种vir语法
    std::vector<std::string> testCodes = {
        "vir myPromise = iNeverAway({MyPromise: function() {}});",
        "const result = printMylove({url: 'image.jpg'});",
        "vir listener = listen({click: () => {}});",
        "function normalFunction() { return 'hello'; }"
    };
    
    for (const auto& code : testCodes) {
        std::cout << "=== 检测代码 ===" << std::endl;
        std::cout << "代码: " << code << std::endl;
        
        bool hasVir = VirSyntaxDetector::detectDirectVirUsage(code);
        
        if (hasVir) {
            std::cout << "❌ 检测到禁止的vir语法！" << std::endl;
            std::string suggestion = VirSyntaxDetector::handleVirViolation(code);
            std::cout << suggestion << std::endl;
        } else {
            std::cout << "✅ 代码检查通过，未发现直接vir语法" << std::endl;
        }
        std::cout << std::endl;
    }
}

// 演示VirFunctionBinder的使用
void demonstrateVirFunctionBinder() {
    std::cout << "\n🌸 演示：VirFunctionBinder替代vir语法\n" << std::endl;
    
    // 清空之前的绑定
    VirFunctionBinder::clear();
    
    std::cout << "=== 方式1：使用createCHTLJSFunction（自动绑定vir）===" << std::endl;
    
    // 使用createCHTLJSFunction创建函数，自动支持vir
    auto iNeverAwayFunc = createCHTLJSFunction("iNeverAway", {"MyPromise", "UserAction"});
    
    std::cout << "\n=== 方式2：使用VirFunctionBinder手动绑定 ===" << std::endl;
    
    // 手动绑定自定义函数的vir支持
    VirFunctionBinder::bind("customFunction", {
        {"MyPromise<Happy>", "chtl_custom_MyPromise_Happy_1"},
        {"MyPromise<Sad>", "chtl_custom_MyPromise_Sad_2"},
        {"UserAction", "chtl_custom_UserAction_3"}
    });
    
    // 验证绑定结果
    std::cout << "\n=== 验证vir绑定 ===" << std::endl;
    
    std::vector<std::string> testFunctions = {"iNeverAway", "customFunction", "unboundFunction"};
    
    for (const auto& funcName : testFunctions) {
        if (VirFunctionBinder::isBound(funcName)) {
            std::cout << "✅ " << funcName << " 已绑定vir支持" << std::endl;
            
            auto mappings = VirFunctionBinder::getKeyMappings(funcName);
            std::cout << "  键映射:" << std::endl;
            for (const auto& mapping : mappings) {
                std::cout << "    " << mapping.first << " → " << mapping.second << std::endl;
            }
        } else {
            std::cout << "❌ " << funcName << " 未绑定vir支持" << std::endl;
        }
    }
    
    // 演示vir访问代码生成
    std::cout << "\n=== vir访问代码生成 ===" << std::endl;
    
    std::string accessCode1 = VirFunctionBinder::generateVirAccess("customFunction", "myVir", "MyPromise<Happy>");
    std::string accessCode2 = VirFunctionBinder::generateVirAccess("customFunction", "myVir", "UnknownKey");
    
    std::cout << "访问 MyPromise<Happy>: " << accessCode1 << std::endl;
    std::cout << "访问 UnknownKey: " << accessCode2 << std::endl;
}

// 演示扫描策略选择
void demonstrateScanStrategySelection() {
    std::cout << "\n🌸 演示：扫描策略智能选择\n" << std::endl;
    
    // 初始化策略系统
    ScanStrategyManager::initialize();
    
    // 测试不同场景的策略选择
    std::vector<std::pair<std::string, ScanContext>> testScenarios = {
        {"**", {0, 100, 1, 0.2, "**"}},           // 中等复杂度，少量token
        {"iNeverAway", {0, 1500, 2, 0.5, "iNeverAway"}}, // 高token数
        {"printMylove", {0, 300, 5, 0.8, "printMylove"}}, // 深度嵌套
        {"customOp", {0, 50, 1, 0.1, "customOp"}}         // 简单场景
    };
    
    for (auto& scenario : testScenarios) {
        const std::string& keyword = scenario.first;
        ScanContext& context = scenario.second;
        
        std::cout << "\n=== 场景测试 ===" << std::endl;
        
        // 选择策略
        ScanStrategy selectedStrategy = ScanStrategyManager::selectStrategy(keyword, context);
        
        // 执行策略
        ScanResult result = ScanStrategyManager::executeStrategy(selectedStrategy, keyword, context);
        
        std::cout << "执行结果:" << std::endl;
        std::cout << "  成功: " << (result.success ? "是" : "否") << std::endl;
        std::cout << "  耗时: " << result.executionTime << "ms" << std::endl;
        std::cout << "  内存使用: " << result.memoryUsage << " bytes" << std::endl;
        std::cout << "  处理代码: " << result.processedCode << std::endl;
    }
}

// 演示关键字复杂度分析
void demonstrateKeywordComplexityAnalysis() {
    std::cout << "\n🌸 演示：关键字复杂度分析\n" << std::endl;
    
    std::vector<std::string> keywords = {"**", "iNeverAway", "printMylove", "simpleKeyword"};
    
    for (const auto& keyword : keywords) {
        int complexity = KeywordComplexityAnalyzer::calculateComplexity(keyword);
        
        std::cout << "关键字: " << keyword << std::endl;
        std::cout << "  复杂度: " << complexity << "/10" << std::endl;
        
        // 测试不同上下文下的策略推荐
        ScanContext contexts[] = {
            {0, 100, 1, 0.2, keyword},    // 小规模
            {0, 1000, 3, 0.5, keyword},   // 中等规模
            {0, 5000, 6, 0.8, keyword}    // 大规模
        };
        
        std::cout << "  策略推荐:" << std::endl;
        for (int i = 0; i < 3; ++i) {
            ScanStrategy recommended = KeywordComplexityAnalyzer::recommendStrategy(keyword, contexts[i]);
            
            std::string strategyName;
            switch (recommended) {
                case ScanStrategy::DUAL_POINTER: strategyName = "双指针"; break;
                case ScanStrategy::BACKTRACK: strategyName = "回退重拼接"; break;
                case ScanStrategy::HYBRID: strategyName = "混合策略"; break;
                case ScanStrategy::ADAPTIVE: strategyName = "自适应"; break;
            }
            
            std::cout << "    " << (i == 0 ? "小规模" : i == 1 ? "中等规模" : "大规模") 
                      << ": " << strategyName << std::endl;
        }
        std::cout << std::endl;
    }
}

// 演示策略性能对比
void demonstrateStrategyPerformanceComparison() {
    std::cout << "\n🌸 演示：扫描策略性能对比\n" << std::endl;
    
    std::vector<ScanStrategy> strategies = {
        ScanStrategy::DUAL_POINTER,
        ScanStrategy::BACKTRACK,
        ScanStrategy::HYBRID,
        ScanStrategy::ADAPTIVE
    };
    
    ScanContext testContext{0, 500, 2, 0.3, "testKeyword"};
    
    std::cout << "=== 性能测试（相同上下文） ===" << std::endl;
    std::cout << "测试上下文: token数=" << testContext.tokenCount 
              << ", 嵌套层级=" << testContext.nestingLevel << std::endl;
    
    for (ScanStrategy strategy : strategies) {
        std::string strategyName;
        switch (strategy) {
            case ScanStrategy::DUAL_POINTER: strategyName = "双指针扫描"; break;
            case ScanStrategy::BACKTRACK: strategyName = "回退重拼接"; break;
            case ScanStrategy::HYBRID: strategyName = "混合策略"; break;
            case ScanStrategy::ADAPTIVE: strategyName = "自适应策略"; break;
        }
        
        std::cout << "\n--- " << strategyName << " ---" << std::endl;
        
        // 执行多次测试取平均值
        double totalTime = 0;
        size_t totalMemory = 0;
        int testRuns = 3;
        
        for (int i = 0; i < testRuns; ++i) {
            ScanContext contextCopy = testContext;
            ScanResult result = ScanStrategyManager::executeStrategy(strategy, "testKeyword", contextCopy);
            totalTime += result.executionTime;
            totalMemory += result.memoryUsage;
        }
        
        std::cout << "平均耗时: " << (totalTime / testRuns) << "ms" << std::endl;
        std::cout << "平均内存: " << (totalMemory / testRuns) << " bytes" << std::endl;
    }
}

// 演示完整的工作流程
void demonstrateCompleteWorkflow() {
    std::cout << "\n🌸 演示：完整工作流程\n" << std::endl;
    
    std::cout << "=== 步骤1：检测vir语法违规 ===" << std::endl;
    
    std::string userCode = "vir myVir = someFunction({param: 'value'});";
    std::cout << "用户代码: " << userCode << std::endl;
    
    if (VirSyntaxDetector::detectDirectVirUsage(userCode)) {
        std::cout << "❌ 检测到vir语法违规，提供替代方案" << std::endl;
        std::string suggestion = VirSyntaxDetector::generateAlternativeSuggestion(userCode);
        std::cout << suggestion << std::endl;
    }
    
    std::cout << "\n=== 步骤2：使用推荐方案绑定vir支持 ===" << std::endl;
    
    // 用户采用推荐方案
    VirFunctionBinder::bind("someFunction", {
        {"param", "chtl_someFunction_param_1"}
    });
    
    std::cout << "\n=== 步骤3：处理CJMOD关键字 ===" << std::endl;
    
    // 模拟统一扫描器遇到CJMOD关键字
    std::string keyword = "someFunction";
    ScanContext context{0, 200, 2, 0.3, keyword};
    
    // 选择最优策略
    ScanStrategy strategy = ScanStrategyManager::selectStrategy(keyword, context);
    
    // 执行策略
    ScanResult result = ScanStrategyManager::executeStrategy(strategy, keyword, context);
    
    std::cout << "\n=== 步骤4：生成最终代码 ===" << std::endl;
    
    if (result.success) {
        std::cout << "✅ 处理成功！" << std::endl;
        std::cout << "生成的代码: " << result.processedCode << std::endl;
        
        // 如果需要vir访问，生成访问代码
        if (VirFunctionBinder::isBound(keyword)) {
            std::string virAccess = VirFunctionBinder::generateVirAccess(keyword, "myVir", "param");
            std::cout << "vir访问代码: " << virAccess << std::endl;
        }
    } else {
        std::cout << "❌ 处理失败" << std::endl;
    }
}

int main() {
    std::cout << "🌸 vir机制限制与扫描策略系统综合演示" << std::endl;
    std::cout << "解决vir语法对统一扫描器的影响，提供智能扫描策略选择" << std::endl;
    
    // 初始化系统
    CJMODScannerIntegration::initialize();
    ScanStrategyManager::initialize();
    
    // 演示各个功能
    demonstrateVirSyntaxDetection();
    demonstrateVirFunctionBinder();
    demonstrateScanStrategySelection();
    demonstrateKeywordComplexityAnalysis();
    demonstrateStrategyPerformanceComparison();
    demonstrateCompleteWorkflow();
    
    std::cout << "\n💝 珂朵莉的总结" << std::endl;
    std::cout << "✅ vir机制限制：严格禁止直接vir语法，避免统一扫描器负担" << std::endl;
    std::cout << "✅ VirFunctionBinder：提供编程方式绑定vir支持" << std::endl;
    std::cout << "✅ 扫描策略系统：智能选择最优扫描算法" << std::endl;
    std::cout << "✅ 默认双指针策略：高效处理大部分CJMOD语法" << std::endl;
    std::cout << "✅ 按需回退策略：处理复杂的中缀操作符" << std::endl;
    std::cout << "✅ 性能监控：持续优化策略选择" << std::endl;
    std::cout << "\n您的设计思路非常先进，这将大大提高CJMOD系统的稳定性和性能！" << std::endl;
    std::cout << "\n珂朵莉永远在大家心中 ❀" << std::endl;
    
    return 0;
}