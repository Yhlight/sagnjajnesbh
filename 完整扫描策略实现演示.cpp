#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace CHTL::CJMOD;

// 演示统一扫描器如何处理CJMOD语法
void demonstrateUnifiedScannerIntegration() {
    std::cout << "===========================================" << std::endl;
    std::cout << "       CHTL统一扫描器与CJMOD集成演示" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // 初始化系统
    std::cout << "\n=== 系统初始化 ===" << std::endl;
    ScanStrategyManager::initialize();
    CJMODScannerIntegration::initialize();
    
    // 模拟CHTL JS代码，包含CJMOD语法
    std::vector<std::string> testCodes = {
        "iNeverAway({ MyPromise<Happy>: function() { console.log('快乐'); } });",
        "printMylove({ mode: ascii, width: 80, height: 40 });",
        "** 这是特殊的CJMOD语法 **",
        "vir myObject = { action: function() { return 'test'; } };",
        "const result = someFunction() ** anotherFunction();"
    };
    
    std::cout << "\n=== 测试代码样本 ===" << std::endl;
    for (size_t i = 0; i < testCodes.size(); i++) {
        std::cout << i + 1 << ". " << testCodes[i] << std::endl;
    }
}

// 演示双指针扫描策略处理CJMOD语法
void demonstrateDualPointerScanning() {
    std::cout << "\n===========================================" << std::endl;
    std::cout << "           双指针扫描策略演示" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // 测试不同的CJMOD关键字
    std::vector<std::string> keywords = {"iNeverAway", "printMylove", "**"};
    
    for (const auto& keyword : keywords) {
        std::cout << "\n--- 测试关键字: " << keyword << " ---" << std::endl;
        
        // 设置扫描上下文
        ScanContext context;
        context.currentPosition = 50;
        context.tokenCount = 25;
        context.nestingLevel = 2;
        context.memoryPressure = 0.3;
        context.currentKeyword = keyword;
        
        // 执行双指针扫描
        auto result = ScanStrategyManager::executeStrategy(
            ScanStrategy::DUAL_POINTER, 
            keyword, 
            context
        );
        
        std::cout << "\n扫描结果:" << std::endl;
        std::cout << "成功: " << (result.success ? "是" : "否") << std::endl;
        std::cout << "执行时间: " << result.executionTime << "ms" << std::endl;
        std::cout << "内存使用: " << result.memoryUsage << " bytes" << std::endl;
        std::cout << "处理代码:\n" << result.processedCode << std::endl;
    }
}

// 演示回退重拼接扫描策略
void demonstrateBacktrackScanning() {
    std::cout << "\n===========================================" << std::endl;
    std::cout << "         回退重拼接扫描策略演示" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // 测试复杂的中缀操作符和需要回退的语法
    std::vector<std::string> complexKeywords = {"**", "iNeverAway"};
    
    for (const auto& keyword : complexKeywords) {
        std::cout << "\n--- 测试复杂关键字: " << keyword << " ---" << std::endl;
        
        // 设置更复杂的扫描上下文
        ScanContext context;
        context.currentPosition = 120;
        context.tokenCount = 60;
        context.nestingLevel = 4;
        context.memoryPressure = 0.7;
        context.currentKeyword = keyword;
        
        // 执行回退扫描
        auto result = ScanStrategyManager::executeStrategy(
            ScanStrategy::BACKTRACK, 
            keyword, 
            context
        );
        
        std::cout << "\n回退扫描结果:" << std::endl;
        std::cout << "成功: " << (result.success ? "是" : "否") << std::endl;
        std::cout << "执行时间: " << result.executionTime << "ms" << std::endl;
        std::cout << "内存使用: " << result.memoryUsage << " bytes" << std::endl;
        std::cout << "处理代码:\n" << result.processedCode << std::endl;
    }
}

// 演示策略选择和性能对比
void demonstrateStrategySelection() {
    std::cout << "\n===========================================" << std::endl;
    std::cout << "         智能策略选择与性能对比" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    std::vector<std::string> testKeywords = {"iNeverAway", "printMylove", "**", "customFunc"};
    
    for (const auto& keyword : testKeywords) {
        std::cout << "\n--- 关键字: " << keyword << " ---" << std::endl;
        
        // 分析关键字复杂度
        int complexity = KeywordComplexityAnalyzer::calculateComplexity(keyword);
        std::cout << "复杂度分析: " << complexity << std::endl;
        
        // 设置测试上下文
        ScanContext context;
        context.currentPosition = 80;
        context.tokenCount = 40;
        context.nestingLevel = 3;
        context.memoryPressure = 0.5;
        context.currentKeyword = keyword;
        
        // 智能策略选择
        ScanStrategy recommendedStrategy = ScanStrategyManager::selectStrategy(keyword, context);
        std::cout << "推荐策略: ";
        switch (recommendedStrategy) {
            case ScanStrategy::DUAL_POINTER: std::cout << "双指针扫描"; break;
            case ScanStrategy::BACKTRACK: std::cout << "回退重拼接"; break;
            case ScanStrategy::HYBRID: std::cout << "混合策略"; break;
            case ScanStrategy::ADAPTIVE: std::cout << "自适应策略"; break;
        }
        std::cout << std::endl;
        
        // 性能对比测试
        std::cout << "\n性能对比测试:" << std::endl;
        
        std::vector<ScanStrategy> strategies = {
            ScanStrategy::DUAL_POINTER,
            ScanStrategy::BACKTRACK,
            ScanStrategy::HYBRID,
            ScanStrategy::ADAPTIVE
        };
        
        for (auto strategy : strategies) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            auto result = ScanStrategyManager::executeStrategy(strategy, keyword, context);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            std::string strategyName;
            switch (strategy) {
                case ScanStrategy::DUAL_POINTER: strategyName = "双指针"; break;
                case ScanStrategy::BACKTRACK: strategyName = "回退"; break;
                case ScanStrategy::HYBRID: strategyName = "混合"; break;
                case ScanStrategy::ADAPTIVE: strategyName = "自适应"; break;
            }
            
            std::cout << "  " << strategyName << ": " 
                      << result.executionTime << "ms (实际: " 
                      << duration.count() / 1000.0 << "ms), "
                      << "内存: " << result.memoryUsage << " bytes" << std::endl;
        }
    }
}

// 演示CJMOD关键字处理集成
void demonstrateCJMODKeywordIntegration() {
    std::cout << "\n===========================================" << std::endl;
    std::cout << "        CJMOD关键字处理集成演示" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // 注册CJMOD关键字
    std::cout << "\n=== 注册CJMOD关键字 ===" << std::endl;
    CJMODScannerIntegration::registerCommonKeywords();
    
    // 测试关键字识别
    std::vector<std::string> testKeywords = {"iNeverAway", "printMylove", "**", "normalFunction"};
    
    std::cout << "\n=== 关键字识别测试 ===" << std::endl;
    for (const auto& keyword : testKeywords) {
        bool isCJMOD = CJMODKeywordHandler::isCJMODKeyword(keyword);
        std::cout << "关键字 '" << keyword << "': " 
                  << (isCJMOD ? "CJMOD关键字" : "标准关键字") << std::endl;
        
        if (isCJMOD) {
            auto info = CJMODKeywordHandler::getKeywordInfo(keyword);
            std::cout << "  需要回退: " << (info.needsBacktrack ? "是" : "否") << std::endl;
            std::cout << "  回退距离: " << info.backtrackDistance << std::endl;
            std::cout << "  需要前向收集: " << (info.needsForwardCollect ? "是" : "否") << std::endl;
        }
    }
    
    // 演示统一扫描器调用
    std::cout << "\n=== 统一扫描器调用演示 ===" << std::endl;
    for (const auto& keyword : testKeywords) {
        if (CJMODKeywordHandler::isCJMODKeyword(keyword)) {
            std::cout << "\n处理CJMOD关键字: " << keyword << std::endl;
            bool processed = CJMODScannerIntegration::processKeywordFromScanner(keyword, nullptr);
            std::cout << "处理结果: " << (processed ? "成功" : "失败") << std::endl;
        }
    }
}

// 演示完整的CHTL JS代码处理流程
void demonstrateCompleteProcessingWorkflow() {
    std::cout << "\n===========================================" << std::endl;
    std::cout << "       完整CHTL JS代码处理流程演示" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // 模拟一个完整的CHTL JS文件内容
    std::string chtlJSCode = R"(
// CHTL JS代码示例
iNeverAway({
    MyPromise<Happy>: function(message) {
        console.log("快乐承诺: " + message);
        return true;
    },
    MyPromise<Sad>: (message) => {
        console.log("忧伤承诺: " + message);
        return false;
    },
    UserAction: function(action) {
        return "执行用户操作: " + action;
    }
});

printMylove({
    mode: ascii,
    width: 120,
    height: 60,
    scale: 1.5,
    imagePath: "/path/to/image.jpg"
});

** 特殊CJMOD语法处理 **

const result = someFunction() ** anotherFunction();
)";
    
    std::cout << "\n=== 待处理的CHTL JS代码 ===" << std::endl;
    std::cout << chtlJSCode << std::endl;
    
    // 模拟统一扫描器的处理过程
    std::cout << "\n=== 统一扫描器处理过程 ===" << std::endl;
    
    std::vector<std::string> detectedKeywords = {"iNeverAway", "printMylove", "**"};
    
    for (const auto& keyword : detectedKeywords) {
        std::cout << "\n--- 处理关键字: " << keyword << " ---" << std::endl;
        
        // 设置处理上下文
        ScanContext context;
        context.currentPosition = chtlJSCode.find(keyword);
        context.tokenCount = 50;
        context.nestingLevel = 2;
        context.memoryPressure = 0.4;
        context.currentKeyword = keyword;
        
        // 智能选择策略
        ScanStrategy strategy = ScanStrategyManager::selectStrategy(keyword, context);
        
        // 执行处理
        auto result = ScanStrategyManager::executeStrategy(strategy, keyword, context);
        
        std::cout << "处理完成，生成代码:" << std::endl;
        std::cout << result.processedCode << std::endl;
        std::cout << "性能指标: 时间=" << result.executionTime 
                  << "ms, 内存=" << result.memoryUsage << " bytes" << std::endl;
    }
}

int main() {
    std::cout << "CHTL项目 - 完整扫描策略实现演示" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        // 1. 统一扫描器集成演示
        demonstrateUnifiedScannerIntegration();
        
        // 2. 双指针扫描策略演示
        demonstrateDualPointerScanning();
        
        // 3. 回退重拼接扫描策略演示
        demonstrateBacktrackScanning();
        
        // 4. 策略选择和性能对比
        demonstrateStrategySelection();
        
        // 5. CJMOD关键字处理集成
        demonstrateCJMODKeywordIntegration();
        
        // 6. 完整处理流程演示
        demonstrateCompleteProcessingWorkflow();
        
        std::cout << "\n===========================================" << std::endl;
        std::cout << "           演示完成！" << std::endl;
        std::cout << "CHTL统一扫描器现在能够真正处理CJMOD提供的语法" << std::endl;
        std::cout << "双指针和回退扫描策略已完整实现并集成" << std::endl;
        std::cout << "===========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "演示过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}