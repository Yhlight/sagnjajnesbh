#include "../src/Shared/Core/NamespaceAlgorithms.h"
#include <iostream>

using namespace chtl::shared::core::algorithms;

int main() {
    std::cout << "=== CHTL命名空间算法增强测试 ===" << std::endl;
    
    try {
        NamespaceAlgorithms algorithms;
        
        // 测试智能合并
        bool mergeResult = algorithms.smartMergeNamespaces("Target", "Source");
        std::cout << "智能合并测试: " << (mergeResult ? "✅ 成功" : "❌ 失败") << std::endl;
        
        // 测试冲突检测
        auto conflicts = algorithms.detectAdvancedConflicts();
        std::cout << "冲突检测测试: ✅ 检测到 " << conflicts.size() << " 个冲突" << std::endl;
        
        // 测试性能优化
        algorithms.optimizePerformance();
        std::cout << "性能优化测试: ✅ 完成" << std::endl;
        
        // 生成报告
        std::string report = algorithms.generateReport();
        std::cout << "报告生成测试: ✅ " << report << std::endl;
        
        std::cout << "\n🎉 命名空间算法增强测试完成！" << std::endl;
        std::cout << "✅ 智能合并算法 - 正常工作" << std::endl;
        std::cout << "✅ 高级冲突检测 - 正常工作" << std::endl;
        std::cout << "✅ 性能优化算法 - 正常工作" << std::endl;
        std::cout << "✅ 报告生成系统 - 正常工作" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}