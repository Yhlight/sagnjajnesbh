#pragma once
#include "../Framework/TestBase.h"

namespace chtl {
namespace test {
namespace unit {

/**
 * CHTL语法全面测试
 * 根据CHTL语法文档测试所有语法特性
 */
class CHTLSyntaxComprehensiveTest : public framework::TestBase {
public:
    CHTLSyntaxComprehensiveTest();
    
    void setUp() override;
    void tearDown() override;

private:
    // === 基础语法测试 ===
    void testComments();                    // 注释语法
    void testTextNodes();                   // 文本节点
    void testLiterals();                    // 字面量和无修饰字符串
    void testCEEquality();                  // CE对等式 (: = 等价)
    void testElementNodes();                // 元素节点
    
    // === 样式系统测试 ===
    void testInlineStyles();                // 内联样式
    void testAutoClassId();                 // 自动化类名/id
    void testContextInference();            // 上下文推导 (&)
    void testLocalStyleBlocks();            // 本地样式块
    void testGlobalStyleBlocks();           // 全局样式块
    
    // === 模板系统测试 ===
    void testStyleGroupTemplates();         // 样式组模板
    void testElementTemplates();            // 元素模板
    void testVariableGroupTemplates();      // 变量组模板
    void testTemplateUsage();               // 模板使用
    
    // === CHTL JS语法测试 ===
    void testCHTLJSSelectors();             // {{.class}} {{#id}} 选择器
    void testArrowOperator();               // -> 箭头操作符
    void testEventHandling();               // 事件处理
    void testListenFunction();              // listen函数
    void testDelegateFunction();            // delegate函数
    void testAnimateFunction();             // animate函数
    void testVirtualObjects();              // 虚拟对象 vir
    void testUndecoratedLiterals();         // 无修饰字面量
    
    // === 导入系统测试 ===
    void testHtmlImport();                  // @Html导入
    void testStyleImport();                 // @Style导入
    void testJavaScriptImport();            // @JavaScript导入
    void testChtlImport();                  // @Chtl导入
    void testCJmodImport();                 // @CJmod导入
    void testWildcardImports();             // 通配符导入 (.*, /*)
    void testSubmoduleImports();            // 子模块导入
    void testImportAliases();               // 导入别名 (as)
    
    // === 命名空间系统测试 ===
    void testBasicNamespaces();             // 基础命名空间
    void testNestedNamespaces();            // 嵌套命名空间
    void testNamespaceUsage();              // 命名空间使用
    void testNamespaceConflicts();          // 命名空间冲突
    void testNamespaceMerging();            // 命名空间合并
    
    // === 模块系统测试 ===
    void testCMODStructure();               // CMOD结构
    void testCJMODStructure();              // CJMOD结构
    void testModuleCombination();           // CMOD + CJMOD组合
    void testChthollyModule();              // 珂朵莉模块特性
    void testModuleVersioning();            // 模块版本管理
    
    // === 高级特性测试 ===
    void testINeverAwayFunction();          // iNeverAway特殊功能
    void testPrintMyloveFunction();         // printMylove功能
    void testVirtualObjectOptimization();   // 虚拟对象优化
    void testAutoAdditionSystem();          // 自动添加系统
    void testSyntaxConstraints();           // 语法约束系统
    
    // === 错误处理测试 ===
    void testSyntaxErrors();                // 语法错误检测
    void testImportErrors();                // 导入错误处理
    void testNamespaceErrors();             // 命名空间错误
    void testModuleErrors();                // 模块错误处理
    
    // === 辅助方法 ===
    bool validateCHTLCode(const std::string& code);
    bool validateCHTLJSCode(const std::string& code);
    bool validateImportStatement(const std::string& import);
    std::string createTestCHTLFile(const std::string& content);
};

} // namespace unit
} // namespace test
} // namespace chtl