#pragma once
#include "../Framework/TestBase.h"

namespace chtl {
namespace test {
namespace unit {

/**
 * CHTL JS高级特性测试
 * 专门测试CHTL JS的高级功能
 */
class CHTLJSAdvancedTest : public framework::TestBase {
public:
    CHTLJSAdvancedTest();
    
    void setUp() override;
    void tearDown() override;

private:
    // === 虚拟对象系统测试 ===
    void testVirtualObjectBasics();         // 虚拟对象基础
    void testVirtualObjectOptimization();   // 虚拟对象优化
    void testVirtualObjectNesting();        // 虚拟对象嵌套
    void testVirtualObjectEdgeCases();      // 虚拟对象边界情况
    
    // === iNeverAway功能测试 ===
    void testINeverAwayBasics();            // iNeverAway基础功能
    void testINeverAwayStateOverload();     // 状态重载
    void testINeverAwayComplexTypes();      // 复杂类型支持
    void testINeverAwayErrorHandling();     // 错误处理
    
    // === 无修饰字面量测试 ===
    void testUndecoratedInTextBlocks();     // 文本块中的无修饰字面量
    void testUndecoratedInAttributes();     // 属性值中的无修饰字面量
    void testUndecoratedLimitations();      // 无修饰字面量限制
    void testUndecoratedMixedUsage();       // 混合使用场景
    
    // === 函数增强测试 ===
    void testUnorderedKeyValues();          // 无序键值对
    void testOptionalKeyValues();           // 可选键值对
    void testAllJavaScriptEvents();         // 所有JavaScript事件
    void testEventParameterValidation();    // 事件参数验证
    
    // === 自动添加系统测试 ===
    void testAutoLocalStyleAddition();      // 自动添加本地样式块
    void testAutoLocalScriptAddition();     // 自动添加本地脚本块
    void testIDPriorityRule();              // ID优先级规则
    void testExistingBlockHandling();       // 现有块处理
    
    // === 语法约束测试 ===
    void testGlobalStyleConstraints();      // 全局样式约束
    void testGlobalScriptConstraints();     // 全局脚本约束
    void testLocalStyleConstraints();       // 本地样式约束
    void testLocalScriptConstraints();      // 本地脚本约束
    void testConstraintViolations();        // 约束违反检测
    
    // === 统一扫描器测试 ===
    void testVariableLengthSlicing();       // 变长切片
    void testContextAwareAggregation();     // 上下文感知聚合
    void testMinimalSyntaxUnits();          // 最小语法单元
    void testNestedStructureHandling();     // 嵌套结构处理
    void testScannerPrecision();            // 扫描器精度
    
    // === 辅助方法 ===
    bool isValidCHTLJSSyntax(const std::string& code);
    bool hasVirtualObjectOptimization(const std::string& code);
    std::vector<std::string> extractCHTLJSFragments(const std::string& code);
    bool validateEventSupport(const std::string& eventName);
};

} // namespace unit
} // namespace test
} // namespace chtl