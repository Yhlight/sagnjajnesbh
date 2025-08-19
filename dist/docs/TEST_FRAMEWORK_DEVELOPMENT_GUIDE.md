# 测试框架开发说明

## 概述

CHTL采用基于进程的单元测试框架，为编译器系统提供全面的测试覆盖。本指南介绍测试框架的设计、开发和使用方法。

---

## 测试框架架构

### 核心组件

```
src/Test/
├── Framework/              # 测试框架核心
│   ├── TestBase.h/cpp     # 测试基类
│   ├── TestRunner.h/cpp   # 测试运行器
│   ├── TestUtils.h/cpp    # 测试工具
│   └── Assertions.h       # 断言宏
├── Unit/                  # 单元测试
├── Integration/           # 集成测试
├── Performance/           # 性能测试
├── Compliance/            # 合规性测试
└── Utils/                 # 测试辅助工具
```

### 测试类型

1. **单元测试**: 测试单个组件功能
2. **集成测试**: 测试组件间协作
3. **性能测试**: 测试编译器性能
4. **合规性测试**: 验证语法文档合规性
5. **回归测试**: 防止功能回退

---

## 测试框架设计

### 测试基类

```cpp
// src/Test/Framework/TestBase.h
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace chtl {
namespace test {

class TestBase {
public:
    TestBase(const std::string& testName);
    virtual ~TestBase() = default;
    
    // 测试生命周期
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void run() = 0;
    
    // 测试执行
    bool execute();
    
    // 测试结果
    bool passed() const { return passed_; }
    const std::string& getName() const { return testName_; }
    const std::vector<std::string>& getErrors() const { return errors_; }
    std::chrono::milliseconds getExecutionTime() const { return executionTime_; }
    
protected:
    // 断言方法
    void assertTrue(bool condition, const std::string& message = "");
    void assertFalse(bool condition, const std::string& message = "");
    void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "");
    void assertNotEqual(const std::string& unexpected, const std::string& actual, const std::string& message = "");
    void assertContains(const std::string& container, const std::string& content, const std::string& message = "");
    void assertThrows(std::function<void()> operation, const std::string& message = "");
    
    // 工具方法
    void log(const std::string& message);
    void logError(const std::string& error);
    
private:
    std::string testName_;
    bool passed_ = true;
    std::vector<std::string> errors_;
    std::vector<std::string> logs_;
    std::chrono::milliseconds executionTime_{0};
    
    void recordError(const std::string& error);
};

// 测试注册宏
#define REGISTER_TEST(TestClass) \
    static TestRegistrar<TestClass> testRegistrar##TestClass(#TestClass)

template<typename T>
class TestRegistrar {
public:
    TestRegistrar(const std::string& name) {
        TestRegistry::getInstance().registerTest(name, []() {
            return std::make_unique<T>();
        });
    }
};

} // namespace test
} // namespace chtl
```

### 测试运行器

```cpp
// src/Test/Framework/TestRunner.h
#pragma once
#include "TestBase.h"
#include <memory>
#include <map>

namespace chtl {
namespace test {

class TestRunner {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::vector<std::string> errors;
        std::chrono::milliseconds executionTime;
    };
    
    struct TestSuite {
        std::string suiteName;
        std::vector<TestResult> results;
        int passedCount = 0;
        int failedCount = 0;
        std::chrono::milliseconds totalTime{0};
    };
    
    // 运行所有测试
    TestSuite runAllTests();
    
    // 运行特定测试
    TestResult runTest(const std::string& testName);
    
    // 运行测试套件
    TestSuite runTestSuite(const std::vector<std::string>& testNames);
    
    // 过滤运行
    TestSuite runTestsMatching(const std::string& pattern);
    
    // 并行运行
    TestSuite runTestsParallel(int threadCount = 0);
    
    // 报告生成
    std::string generateTextReport(const TestSuite& suite);
    std::string generateXMLReport(const TestSuite& suite);
    std::string generateHTMLReport(const TestSuite& suite);
    
    // 配置
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setTimeout(std::chrono::seconds timeout) { timeout_ = timeout; }
    void setOutputDirectory(const std::string& dir) { outputDir_ = dir; }

private:
    bool verbose_ = false;
    std::chrono::seconds timeout_{30};
    std::string outputDir_ = "./test_results";
    
    void logTestStart(const std::string& testName);
    void logTestEnd(const std::string& testName, bool passed, std::chrono::milliseconds time);
};

// 测试注册表
class TestRegistry {
public:
    using TestFactory = std::function<std::unique_ptr<TestBase>()>;
    
    static TestRegistry& getInstance();
    
    void registerTest(const std::string& name, TestFactory factory);
    std::vector<std::string> getTestNames() const;
    std::unique_ptr<TestBase> createTest(const std::string& name);
    
private:
    std::map<std::string, TestFactory> tests_;
};

} // namespace test
} // namespace chtl
```

---

## 断言系统

### 断言宏定义

```cpp
// src/Test/Framework/Assertions.h
#pragma once
#include <sstream>

namespace chtl {
namespace test {

// 基础断言宏
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << "断言失败: " << #condition << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::ostringstream oss; \
            oss << "断言失败: 期望 " << (expected) << ", 实际 " << (actual) \
                << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_NE(unexpected, actual) \
    do { \
        if ((unexpected) == (actual)) { \
            std::ostringstream oss; \
            oss << "断言失败: 不应该等于 " << (unexpected) \
                << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_CONTAINS(container, content) \
    do { \
        if ((container).find(content) == std::string::npos) { \
            std::ostringstream oss; \
            oss << "断言失败: '" << (container) << "' 不包含 '" << (content) << "'" \
                << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_THROWS(operation, exceptionType) \
    do { \
        bool caught = false; \
        try { \
            operation; \
        } catch (const exceptionType&) { \
            caught = true; \
        } catch (...) { \
            std::ostringstream oss; \
            oss << "断言失败: 抛出了错误的异常类型 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
        if (!caught) { \
            std::ostringstream oss; \
            oss << "断言失败: 没有抛出期望的异常 " << #exceptionType \
                << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

// 性能断言
#define ASSERT_PERFORMANCE(operation, maxTimeMs) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        operation; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); \
        if (duration.count() > (maxTimeMs)) { \
            std::ostringstream oss; \
            oss << "性能断言失败: 操作耗时 " << duration.count() << "ms, 期望 < " << (maxTimeMs) << "ms" \
                << " 在 " << __FILE__ << ":" << __LINE__; \
            recordError(oss.str()); \
            return; \
        } \
    } while(0)

} // namespace test
} // namespace chtl
```

---

## 测试开发示例

### 单元测试示例

```cpp
// src/Test/Unit/ScannerTest.cpp
#include "../Framework/TestBase.h"
#include "../../Scanner/CHTLUnifiedScanner.h"

class ScannerTest : public chtl::test::TestBase {
public:
    ScannerTest() : TestBase("ScannerTest") {}
    
    void setUp() override {
        scanner_ = std::make_unique<chtl::scanner::CHTLUnifiedScanner>();
    }
    
    void tearDown() override {
        scanner_.reset();
    }
    
    void run() override {
        testBasicScanning();
        testContextDetection();
        testVariableLengthSlicing();
        testPerformance();
    }
    
private:
    std::unique_ptr<chtl::scanner::CHTLUnifiedScanner> scanner_;
    
    void testBasicScanning() {
        log("测试基础扫描功能");
        
        std::string source = "div { class: test; }";
        auto fragments = scanner_->scan(source);
        
        ASSERT_TRUE(!fragments.empty());
        ASSERT_EQ("div", fragments[0].content);
        
        log("✅ 基础扫描测试通过");
    }
    
    void testContextDetection() {
        log("测试上下文检测");
        
        std::string source = R"(
            div {
                style {
                    color: red;
                }
                script {
                    {{.test}}->click();
                }
            }
        )";
        
        auto fragments = scanner_->contextAwareScan(source);
        
        bool foundCSSContext = false;
        bool foundCHTLJSContext = false;
        
        for (const auto& fragment : fragments) {
            if (fragment.context == "CSS") foundCSSContext = true;
            if (fragment.context == "CHTLJS") foundCHTLJSContext = true;
        }
        
        ASSERT_TRUE(foundCSSContext);
        ASSERT_TRUE(foundCHTLJSContext);
        
        log("✅ 上下文检测测试通过");
    }
    
    void testVariableLengthSlicing() {
        log("测试变量长度切片");
        
        std::string source = "{{.button}}->listen({ click: function() { console.log('test'); } })";
        auto fragments = scanner_->scan(source);
        
        // 验证切片完整性
        for (const auto& fragment : fragments) {
            ASSERT_TRUE(fragment.isComplete);
        }
        
        log("✅ 变量长度切片测试通过");
    }
    
    void testPerformance() {
        log("测试扫描器性能");
        
        // 生成大型测试文件
        std::string largeSource = generateLargeTestFile(1000);
        
        ASSERT_PERFORMANCE({
            auto fragments = scanner_->scan(largeSource);
            ASSERT_TRUE(fragments.size() > 100);
        }, 100); // 应在100ms内完成
        
        log("✅ 性能测试通过");
    }
    
    std::string generateLargeTestFile(int lineCount) {
        std::ostringstream oss;
        for (int i = 0; i < lineCount; i++) {
            oss << "div" << i << " { class: test" << i << "; text { 内容" << i << " } }\n";
        }
        return oss.str();
    }
};

REGISTER_TEST(ScannerTest);
```

### 集成测试示例

```cpp
// src/Test/Integration/CompilerIntegrationTest.cpp
#include "../Framework/TestBase.h"
#include "../../CHTLCompiler/CHTLCompilerCore.h"
#include "../../CHTLJSCompiler/CHTLJSCompiler.h"

class CompilerIntegrationTest : public chtl::test::TestBase {
public:
    CompilerIntegrationTest() : TestBase("CompilerIntegrationTest") {}
    
    void run() override {
        testCHTLCompilation();
        testCHTLJSIntegration();
        testModuleIntegration();
        testEndToEndCompilation();
    }
    
private:
    void testCHTLCompilation() {
        log("测试CHTL编译集成");
        
        chtl::compiler::CHTLCompilerCore compiler;
        
        std::string chtlCode = R"(
            [Custom] @Element TestButton {
                button {
                    class: test-btn;
                    text { 测试按钮 }
                }
            }
            
            html {
                body {
                    TestButton
                }
            }
        )";
        
        auto result = compiler.compile(chtlCode);
        
        ASSERT_TRUE(result.success);
        ASSERT_CONTAINS(result.html, "<button class=\"test-btn\">");
        ASSERT_CONTAINS(result.html, "测试按钮");
        
        log("✅ CHTL编译集成测试通过");
    }
    
    void testCHTLJSIntegration() {
        log("测试CHTL JS集成");
        
        chtl::chtljs::CHTLJSCompiler jsCompiler;
        
        std::string chtljsCode = R"(
            {{.button}}->listen({
                click: function(e) {
                    console.log('按钮被点击');
                }
            });
        )";
        
        auto result = jsCompiler.compile(chtljsCode);
        
        ASSERT_TRUE(result.success);
        ASSERT_CONTAINS(result.javascript, "addEventListener");
        ASSERT_CONTAINS(result.javascript, "click");
        
        log("✅ CHTL JS集成测试通过");
    }
    
    void testModuleIntegration() {
        log("测试模块系统集成");
        
        // 创建临时模块
        createTempModule();
        
        std::string chtlCode = R"(
            [Import] @Chtl from TempModule
            
            html {
                body {
                    TempModule::TempComponent
                }
            }
        )";
        
        chtl::compiler::CHTLCompilerCore compiler;
        auto result = compiler.compile(chtlCode);
        
        ASSERT_TRUE(result.success);
        
        // 清理临时模块
        cleanupTempModule();
        
        log("✅ 模块系统集成测试通过");
    }
    
    void testEndToEndCompilation() {
        log("测试端到端编译");
        
        std::string complexChtlCode = R"(
            [Import] @Chtl from Chtholly
            
            [Custom] @Var AppTheme {
                primaryColor: "#3b82f6";
                spacing: "16px";
            }
            
            [Custom] @Element App {
                div {
                    class: app;
                    
                    style {
                        .app {
                            padding: AppTheme(spacing);
                            color: AppTheme(primaryColor);
                        }
                    }
                    
                    script {
                        {{.app}}->listen({
                            load: function() {
                                console.log('应用已加载');
                            }
                        });
                        
                        vir AppManager = listen({
                            init: function() {
                                console.log('初始化');
                            }
                        });
                        
                        AppManager->init();
                    }
                    
                    ChthollyButton {
                        text { 珂朵莉按钮 }
                    }
                }
            }
            
            html {
                body {
                    App
                }
            }
        )";
        
        // 完整编译流程测试
        chtl::dispatcher::CompilerDispatcher dispatcher;
        auto result = dispatcher.compileComplete(complexChtlCode);
        
        ASSERT_TRUE(result.success);
        ASSERT_CONTAINS(result.html, "<div class=\"app\">");
        ASSERT_CONTAINS(result.css, ".app");
        ASSERT_CONTAINS(result.javascript, "addEventListener");
        
        log("✅ 端到端编译测试通过");
    }
    
    void createTempModule() {
        // 创建临时测试模块
        std::filesystem::create_directories("temp_module/src");
        std::filesystem::create_directories("temp_module/info");
        
        // 创建模块文件...
    }
    
    void cleanupTempModule() {
        std::filesystem::remove_all("temp_module");
    }
};

REGISTER_TEST(CompilerIntegrationTest);
```

### 性能测试示例

```cpp
// src/Test/Performance/CompilerPerformanceTest.cpp
#include "../Framework/TestBase.h"
#include "../../CHTLCompiler/CHTLCompilerCore.h"

class CompilerPerformanceTest : public chtl::test::TestBase {
public:
    CompilerPerformanceTest() : TestBase("CompilerPerformanceTest") {}
    
    void run() override {
        testCompilationSpeed();
        testMemoryUsage();
        testScalability();
        testCacheEffectiveness();
    }
    
private:
    void testCompilationSpeed() {
        log("测试编译速度");
        
        chtl::compiler::CHTLCompilerCore compiler;
        
        // 小文件编译 (< 10ms)
        std::string smallFile = generateTestFile(10);
        ASSERT_PERFORMANCE({
            auto result = compiler.compile(smallFile);
            ASSERT_TRUE(result.success);
        }, 10);
        
        // 中等文件编译 (< 100ms)
        std::string mediumFile = generateTestFile(100);
        ASSERT_PERFORMANCE({
            auto result = compiler.compile(mediumFile);
            ASSERT_TRUE(result.success);
        }, 100);
        
        // 大文件编译 (< 1000ms)
        std::string largeFile = generateTestFile(1000);
        ASSERT_PERFORMANCE({
            auto result = compiler.compile(largeFile);
            ASSERT_TRUE(result.success);
        }, 1000);
        
        log("✅ 编译速度测试通过");
    }
    
    void testMemoryUsage() {
        log("测试内存使用");
        
        // 监控内存使用
        auto initialMemory = getCurrentMemoryUsage();
        
        chtl::compiler::CHTLCompilerCore compiler;
        std::string largeFile = generateTestFile(1000);
        
        auto result = compiler.compile(largeFile);
        ASSERT_TRUE(result.success);
        
        auto finalMemory = getCurrentMemoryUsage();
        auto memoryIncrease = finalMemory - initialMemory;
        
        // 内存增长应该合理 (< 100MB)
        ASSERT_TRUE(memoryIncrease < 100 * 1024 * 1024);
        
        log("✅ 内存使用测试通过");
    }
    
    void testScalability() {
        log("测试可扩展性");
        
        chtl::compiler::CHTLCompilerCore compiler;
        
        // 测试不同规模的文件
        std::vector<int> fileSizes = {10, 50, 100, 500, 1000};
        std::vector<std::chrono::milliseconds> compileTimes;
        
        for (int size : fileSizes) {
            std::string testFile = generateTestFile(size);
            
            auto start = std::chrono::high_resolution_clock::now();
            auto result = compiler.compile(testFile);
            auto end = std::chrono::high_resolution_clock::now();
            
            ASSERT_TRUE(result.success);
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            compileTimes.push_back(duration);
            
            log("文件大小 " + std::to_string(size) + " 行，编译时间: " + std::to_string(duration.count()) + "ms");
        }
        
        // 验证编译时间增长是线性的（而非指数级）
        for (size_t i = 1; i < compileTimes.size(); i++) {
            double ratio = static_cast<double>(compileTimes[i].count()) / compileTimes[i-1].count();
            ASSERT_TRUE(ratio < 5.0); // 时间增长应该合理
        }
        
        log("✅ 可扩展性测试通过");
    }
    
    std::string generateTestFile(int lineCount) {
        std::ostringstream oss;
        oss << "html { body {\n";
        
        for (int i = 0; i < lineCount; i++) {
            oss << "div" << i << " {\n";
            oss << "  class: test" << i << ";\n";
            oss << "  style { .test" << i << " { color: red; } }\n";
            oss << "  script { {{.test" << i << "}}->listen({click: function(){}});}\n";
            oss << "  text { 内容" << i << " }\n";
            oss << "}\n";
        }
        
        oss << "}}";
        return oss.str();
    }
    
    size_t getCurrentMemoryUsage() {
        // 获取当前进程内存使用（简化实现）
        std::ifstream statm("/proc/self/statm");
        size_t size;
        statm >> size;
        return size * getpagesize();
    }
};

REGISTER_TEST(CompilerPerformanceTest);
```

### 合规性测试示例

```cpp
// src/Test/Compliance/SyntaxComplianceTest.cpp
#include "../Framework/TestBase.h"

class SyntaxComplianceTest : public chtl::test::TestBase {
public:
    SyntaxComplianceTest() : TestBase("SyntaxComplianceTest") {}
    
    void run() override {
        testBasicSyntaxCompliance();
        testAdvancedSyntaxCompliance();
        testCHTLJSCompliance();
        testModuleSystemCompliance();
    }
    
private:
    void testBasicSyntaxCompliance() {
        log("测试基础语法合规性");
        
        struct SyntaxTest {
            std::string description;
            std::string chtlCode;
            bool shouldPass;
        };
        
        std::vector<SyntaxTest> tests = {
            {
                "注释系统",
                "// 单行注释\n/* 多行注释 */\n-- 生成器注释\ndiv { text { 测试 } }",
                true
            },
            {
                "文本节点",
                "text { 这是文本内容 }",
                true
            },
            {
                "无修饰字面量",
                "div { class: test; id: main; }",
                true
            },
            {
                "CE对等式",
                "div { class: test; id = main; }",
                true
            },
            {
                "错误的变量组语法",
                "[Var] Colors { primary = blue; }",
                false
            },
            {
                "正确的变量组语法",
                "[Custom] @Var Colors { primary: \"blue\"; }",
                true
            }
        };
        
        chtl::compiler::CHTLCompilerCore compiler;
        
        for (const auto& test : tests) {
            auto result = compiler.compile(test.chtlCode);
            
            if (test.shouldPass) {
                ASSERT_TRUE(result.success);
            } else {
                ASSERT_FALSE(result.success);
            }
            
            log("  ✅ " + test.description);
        }
        
        log("✅ 基础语法合规性测试通过");
    }
    
    void testCHTLJSCompliance() {
        log("测试CHTL JS合规性");
        
        std::vector<std::string> chtljsFeatures = {
            "{{.selector}}",           // 增强选择器
            "element->method()",       // 箭头操作符
            "listen({ click: handler })", // listen函数
            "delegate({ target: sel, click: handler })", // delegate函数
            "animate({ target: el, duration: 1000 })",   // animate函数
            "vir obj = listen({ method: func })"         // 虚对象
        };
        
        chtl::chtljs::CHTLJSCompiler jsCompiler;
        
        for (const auto& feature : chtljsFeatures) {
            std::string testCode = "script { " + feature + "; }";
            auto result = jsCompiler.compile(testCode);
            
            ASSERT_TRUE(result.success);
            log("  ✅ " + feature);
        }
        
        log("✅ CHTL JS合规性测试通过");
    }
};

REGISTER_TEST(SyntaxComplianceTest);
```

---

## 测试工具开发

### 模拟对象 (Mock)

```cpp
// src/Test/Utils/MockObjects.h
#pragma once

namespace chtl {
namespace test {

class MockScanner : public chtl::scanner::CHTLUnifiedScanner {
public:
    // 模拟扫描结果
    std::vector<Fragment> mockFragments;
    
    std::vector<Fragment> scan(const std::string& source) override {
        if (!mockFragments.empty()) {
            return mockFragments; // 返回预设的模拟结果
        }
        return CHTLUnifiedScanner::scan(source);
    }
    
    void setMockResult(const std::vector<Fragment>& fragments) {
        mockFragments = fragments;
    }
};

class MockErrorHandler : public chtl::shared::core::ErrorHandler {
public:
    std::vector<std::string> capturedErrors;
    
    void reportError(ErrorType type, const std::string& code, const std::string& message,
                     const std::string& file = "", int line = 0, int column = 0) override {
        capturedErrors.push_back(message);
    }
    
    bool hasError(const std::string& expectedError) const {
        return std::find(capturedErrors.begin(), capturedErrors.end(), expectedError) != capturedErrors.end();
    }
};

} // namespace test
} // namespace chtl
```

### 测试数据生成器

```cpp
// src/Test/Utils/TestDataGenerator.h
class TestDataGenerator {
public:
    // 生成随机CHTL代码
    static std::string generateRandomCHTL(int complexity = 10) {
        std::ostringstream oss;
        
        oss << "html { body {\n";
        
        for (int i = 0; i < complexity; i++) {
            oss << generateRandomElement(i);
        }
        
        oss << "}}";
        return oss.str();
    }
    
    // 生成边界测试用例
    static std::vector<std::string> generateBoundaryTestCases() {
        return {
            "",                    // 空文件
            " ",                   // 只有空格
            "{}",                  // 空块
            "div{}",               // 最小元素
            std::string(10000, 'a'), // 超长字符串
            generateDeepNesting(100)  // 深度嵌套
        };
    }
    
    // 生成错误测试用例
    static std::vector<std::string> generateErrorTestCases() {
        return {
            "[Var] Colors { primary = blue; }",     // 错误的变量组语法
            "delete .class-name;",                  // 错误的delete语法
            "inherit ButtonStyles(base);",          // 错误的inherit语法
            "except [Origin] @JavaScript;",         // 错误的except语法
            "[Constraint] Rules { };",              // 不存在的语法
        };
    }
    
private:
    static std::string generateRandomElement(int index) {
        std::ostringstream oss;
        
        oss << "div" << index << " {\n";
        oss << "  class: element" << index << ";\n";
        
        if (index % 3 == 0) {
            oss << "  style { .element" << index << " { color: red; } }\n";
        }
        
        if (index % 2 == 0) {
            oss << "  script { {{.element" << index << "}}->listen({click: function(){}}); }\n";
        }
        
        oss << "  text { 元素" << index << " }\n";
        oss << "}\n";
        
        return oss.str();
    }
    
    static std::string generateDeepNesting(int depth) {
        std::ostringstream oss;
        
        for (int i = 0; i < depth; i++) {
            oss << "div" << i << " {\n";
        }
        
        oss << "text { 深度嵌套测试 }\n";
        
        for (int i = 0; i < depth; i++) {
            oss << "}\n";
        }
        
        return oss.str();
    }
};
```

---

## 测试自动化

### 测试运行脚本

```bash
#!/bin/bash
# scripts/run_tests.sh

set -e

echo "🧪 CHTL测试套件运行"
echo "==================="

# 进入构建目录
cd build

# 运行不同类型的测试
run_test_category() {
    local category=$1
    local description=$2
    
    echo ""
    echo "📋 运行${description}..."
    echo "$(printf '=%.0s' {1..50})"
    
    for test in ./bin/test_*${category}*; do
        if [ -f "$test" ]; then
            echo "🔍 运行: $(basename $test)"
            if $test; then
                echo "✅ $(basename $test) 通过"
            else
                echo "❌ $(basename $test) 失败"
                exit 1
            fi
        fi
    done
}

# 按类别运行测试
run_test_category "compliance" "合规性测试"
run_test_category "unit" "单元测试"
run_test_category "integration" "集成测试"
run_test_category "performance" "性能测试"

# 运行特殊测试
echo ""
echo "🎯 运行特色功能测试..."
echo "$(printf '=%.0s' {1..50})"

# 珂朵莉模块测试
if [ -f "./test_chtholly_module" ]; then
    echo "💖 运行珂朵莉模块测试..."
    ./test_chtholly_module
    echo "✅ 珂朵莉模块测试通过"
fi

# CJMOD设计测试
if [ -f "./test_cjmod_design" ]; then
    echo "🚀 运行CJMOD设计测试..."
    ./test_cjmod_design
    echo "✅ CJMOD设计测试通过"
fi

echo ""
echo "🎉 所有测试通过！"
echo "✨ CHTL系统功能正常"
```

### 持续测试

```bash
#!/bin/bash
# scripts/continuous_testing.sh

# 监控源码变化并自动运行测试
inotifywait -m -r src/ --format '%w%f %e' -e modify,create,delete | while read file event; do
    echo "📁 文件变化: $file ($event)"
    
    # 确定受影响的测试
    affected_tests=$(determine_affected_tests "$file")
    
    # 重新构建
    echo "🔧 重新构建..."
    make -j$(nproc) > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo "✅ 构建成功"
        
        # 运行相关测试
        for test in $affected_tests; do
            echo "🧪 运行测试: $test"
            ./build/bin/$test
        done
    else
        echo "❌ 构建失败"
    fi
    
    echo "$(printf '=%.0s' {1..50})"
done
```

---

## 测试报告

### 报告生成器

```cpp
// src/Test/Utils/ReportGenerator.h
class TestReportGenerator {
public:
    static std::string generateHTMLReport(const TestRunner::TestSuite& suite) {
        std::ostringstream html;
        
        html << "<!DOCTYPE html><html><head>";
        html << "<title>CHTL测试报告</title>";
        html << "<style>";
        html << "body { font-family: 'Segoe UI', sans-serif; margin: 20px; }";
        html << ".passed { color: #10b981; }";
        html << ".failed { color: #ef4444; }";
        html << ".test-item { margin: 10px 0; padding: 10px; border-radius: 5px; }";
        html << ".test-passed { background: #ecfdf5; border-left: 4px solid #10b981; }";
        html << ".test-failed { background: #fef2f2; border-left: 4px solid #ef4444; }";
        html << "</style></head><body>";
        
        // 概要信息
        html << "<h1>🧪 CHTL测试报告</h1>";
        html << "<div class='summary'>";
        html << "<p><strong>测试套件:</strong> " << suite.suiteName << "</p>";
        html << "<p><strong>总测试数:</strong> " << suite.results.size() << "</p>";
        html << "<p><strong>通过:</strong> <span class='passed'>" << suite.passedCount << "</span></p>";
        html << "<p><strong>失败:</strong> <span class='failed'>" << suite.failedCount << "</span></p>";
        html << "<p><strong>总耗时:</strong> " << suite.totalTime.count() << "ms</p>";
        html << "</div>";
        
        // 详细结果
        html << "<h2>📋 详细结果</h2>";
        
        for (const auto& result : suite.results) {
            std::string cssClass = result.passed ? "test-passed" : "test-failed";
            std::string status = result.passed ? "✅" : "❌";
            
            html << "<div class='test-item " << cssClass << "'>";
            html << "<h3>" << status << " " << result.testName << "</h3>";
            html << "<p><strong>耗时:</strong> " << result.executionTime.count() << "ms</p>";
            
            if (!result.errors.empty()) {
                html << "<h4>错误信息:</h4><ul>";
                for (const auto& error : result.errors) {
                    html << "<li>" << error << "</li>";
                }
                html << "</ul>";
            }
            
            html << "</div>";
        }
        
        html << "</body></html>";
        return html.str();
    }
    
    static std::string generateJUnitXML(const TestRunner::TestSuite& suite) {
        std::ostringstream xml;
        
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml << "<testsuite name=\"" << suite.suiteName << "\" ";
        xml << "tests=\"" << suite.results.size() << "\" ";
        xml << "failures=\"" << suite.failedCount << "\" ";
        xml << "time=\"" << (suite.totalTime.count() / 1000.0) << "\">\n";
        
        for (const auto& result : suite.results) {
            xml << "  <testcase name=\"" << result.testName << "\" ";
            xml << "time=\"" << (result.executionTime.count() / 1000.0) << "\"";
            
            if (!result.passed) {
                xml << ">\n";
                xml << "    <failure message=\"测试失败\">";
                for (const auto& error : result.errors) {
                    xml << error << "\n";
                }
                xml << "</failure>\n";
                xml << "  </testcase>\n";
            } else {
                xml << "/>\n";
            }
        }
        
        xml << "</testsuite>";
        return xml.str();
    }
};
```

---

## 测试最佳实践

### 测试设计原则

1. **独立性**: 每个测试应该独立运行
2. **可重复性**: 测试结果应该可重复
3. **清晰性**: 测试意图应该清晰明确
4. **快速性**: 测试应该快速执行
5. **全面性**: 测试应该覆盖所有重要功能

### 测试命名规范

```cpp
// 测试类命名: [Component]Test
class ScannerTest : public TestBase { };
class CompilerTest : public TestBase { };

// 测试方法命名: test[Feature][Scenario]
void testBasicScanning();
void testScanningWithErrors();
void testScanningPerformance();

// 测试文件命名: [component]_test.cpp
// scanner_test.cpp
// compiler_test.cpp
```

### 测试数据管理

```cpp
// 测试数据组织
class TestData {
public:
    static const std::string VALID_CHTL_CODE;
    static const std::string INVALID_CHTL_CODE;
    static const std::vector<std::string> SYNTAX_ERROR_CASES;
    static const std::vector<std::string> PERFORMANCE_TEST_FILES;
    
    static std::string loadTestFile(const std::string& filename) {
        std::ifstream file("test_data/" + filename);
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
};
```

---

## 测试覆盖率

### 覆盖率工具

```bash
# 安装gcov和lcov
sudo apt install gcov lcov

# 编译时启用覆盖率
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="--coverage" \
      -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
      ..

# 运行测试
make test

# 生成覆盖率报告
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/external/*' '*/test/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_html

# 查看报告
open coverage_html/index.html
```

### 覆盖率目标

- **行覆盖率**: ≥ 85%
- **函数覆盖率**: ≥ 90%
- **分支覆盖率**: ≥ 80%
- **核心组件覆盖率**: ≥ 95%

---

## 性能测试

### 基准测试

```cpp
// src/Test/Performance/BenchmarkTest.cpp
class BenchmarkTest : public chtl::test::TestBase {
public:
    void run() override {
        benchmarkCompilationSpeed();
        benchmarkMemoryUsage();
        benchmarkThroughput();
    }
    
private:
    void benchmarkCompilationSpeed() {
        log("📊 编译速度基准测试");
        
        struct Benchmark {
            std::string name;
            std::string code;
            int maxTimeMs;
        };
        
        std::vector<Benchmark> benchmarks = {
            {"小文件", generateTestFile(10), 10},
            {"中等文件", generateTestFile(100), 100},
            {"大文件", generateTestFile(1000), 1000}
        };
        
        chtl::compiler::CHTLCompilerCore compiler;
        
        for (const auto& benchmark : benchmarks) {
            auto start = std::chrono::high_resolution_clock::now();
            auto result = compiler.compile(benchmark.code);
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            ASSERT_TRUE(result.success);
            ASSERT_TRUE(duration.count() <= benchmark.maxTimeMs);
            
            log("  " + benchmark.name + ": " + std::to_string(duration.count()) + "ms");
        }
        
        log("✅ 编译速度基准测试通过");
    }
};
```

---

## 测试CI/CD集成

### GitHub Actions测试

```yaml
# .github/workflows/tests.yml
name: CHTL Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    strategy:
      matrix:
        test-type: [unit, integration, performance, compliance]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Environment
      run: ./scripts/install_deps_ubuntu.sh
    
    - name: Build
      run: |
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON ..
        make -j$(nproc)
    
    - name: Run Tests
      run: |
        cd build
        ./scripts/run_tests.sh ${{ matrix.test-type }}
    
    - name: Generate Coverage Report
      if: matrix.test-type == 'unit'
      run: |
        lcov --capture --directory build --output-file coverage.info
        lcov --remove coverage.info '/usr/*' '*/external/*' --output-file coverage_filtered.info
    
    - name: Upload Coverage
      if: matrix.test-type == 'unit'
      uses: codecov/codecov-action@v3
      with:
        file: coverage_filtered.info
    
    - name: Upload Test Results
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.test-type }}
        path: build/test_results/
```

---

## 测试维护

### 测试更新策略

1. **功能测试**: 每个新功能都必须有对应测试
2. **回归测试**: 每个修复的bug都要添加回归测试
3. **性能测试**: 定期更新性能基准
4. **合规性测试**: 语法文档更新时同步更新测试

### 测试清理

```bash
# 定期清理测试数据
find test_results/ -name "*.log" -mtime +7 -delete
find test_results/ -name "*.tmp" -delete

# 清理测试缓存
rm -rf build/test_cache/
```

---

通过CHTL的测试框架，开发者可以确保代码质量和功能正确性。测试框架的设计考虑了编译器系统的特殊需求，提供了全面的测试能力和工具支持。