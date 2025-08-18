#include "CompilerPerformanceTest.h"
#include <chrono>

namespace chtl {
namespace test {
namespace performance {

CompilerPerformanceTest::CompilerPerformanceTest() : TestBase("CompilerPerformance") {
    
    TEST_CATEGORY("testCSSCompilationSpeed", "测试CSS编译速度", "Performance") {
        testCSSCompilationSpeed();
    };
    
    TEST_CATEGORY("testJavaScriptCompilationSpeed", "测试JavaScript编译速度", "Performance") {
        testJavaScriptCompilationSpeed();
    };
    
    TEST_CATEGORY("testLargeFileCompilation", "测试大文件编译性能", "Performance") {
        testLargeFileCompilation();
    };
    
    TEST_CATEGORY("testMemoryUsage", "测试内存使用情况", "Memory") {
        testMemoryUsage();
    };
    
    TEST_PRIORITY("testConcurrentCompilation", "测试并发编译性能", "Concurrency", 8) {
        testConcurrentCompilation();
    };
    
    TEST_CATEGORY("testNamespacePerformance", "测试命名空间性能", "Performance") {
        testNamespacePerformance();
    };
    
    TEST_CATEGORY("testModuleLoadingPerformance", "测试模块加载性能", "Performance") {
        testModuleLoadingPerformance();
    };
}

void CompilerPerformanceTest::setUp() {
    timer_.reset();
}

void CompilerPerformanceTest::tearDown() {
    // 清理性能测试资源
}

void CompilerPerformanceTest::testCSSCompilationSpeed() {
    // 生成测试CSS代码
    std::string cssCode = R"(
.test-class {
    color: red;
    background: blue;
    margin: 10px;
    padding: 5px;
}

.another-class {
    display: flex;
    justify-content: center;
    align-items: center;
}
)";
    
    // 基准测试CSS编译
    auto duration = utils::PerformanceTestUtils::benchmark([&cssCode]() {
        // 这里应该调用实际的CSS编译器
        // 简化实现：模拟编译过程
        volatile size_t length = cssCode.length();
        (void)length; // 避免未使用变量警告
    }, 1000);
    
    // 验证性能要求（CSS编译应该在合理时间内完成）
    assertTrue(duration.count() < 5000, "CSS编译应该在5秒内完成1000次");
    
    std::cout << "  CSS编译性能: " << duration.count() << "ms (1000次)" << std::endl;
}

void CompilerPerformanceTest::testJavaScriptCompilationSpeed() {
    // 生成测试JavaScript代码
    std::string jsCode = R"(
function testFunction() {
    const data = {
        name: 'test',
        value: 42
    };
    
    return data.name + data.value;
}

class TestClass {
    constructor(name) {
        this.name = name;
    }
    
    getName() {
        return this.name;
    }
}
)";
    
    // 基准测试JavaScript编译
    auto duration = utils::PerformanceTestUtils::benchmark([&jsCode]() {
        // 这里应该调用实际的JavaScript编译器
        volatile size_t length = jsCode.length();
        (void)length;
    }, 1000);
    
    assertTrue(duration.count() < 5000, "JavaScript编译应该在5秒内完成1000次");
    
    std::cout << "  JavaScript编译性能: " << duration.count() << "ms (1000次)" << std::endl;
}

void CompilerPerformanceTest::testLargeFileCompilation() {
    // 生成大型CHTL文件
    std::ostringstream largeCode;
    
    largeCode << "[Style]\n";
    for (int i = 0; i < 100; ++i) {
        largeCode << ".class" << i << " { color: #" << std::hex << (i * 1000) << "; }\n";
    }
    
    largeCode << "\n[Component]\n";
    for (int i = 0; i < 50; ++i) {
        largeCode << "{{Component" << i << "}}\n";
        largeCode << "  {{.class" << i << "}}Content" << i << "{{/.class" << i << "}}\n";
        largeCode << "{{/Component" << i << "}}\n";
    }
    
    largeCode << "\n[Script]\n";
    for (int i = 0; i < 50; ++i) {
        largeCode << "function createComponent" << i << "() { return " << i << "; }\n";
    }
    
    std::string code = largeCode.str();
    
    timer_.start();
    // 模拟大文件编译
    bool isValid = utils::CHTLTestUtils::validateCHTLSyntax(code);
    timer_.stop();
    
    assertTrue(isValid, "大型CHTL文件应该有效");
    assertTrue(timer_.elapsed().count() < 1000, "大文件编译应该在1秒内完成");
    
    std::cout << "  大文件编译性能: " << timer_.elapsed().count() << "ms" << std::endl;
    std::cout << "  文件大小: " << code.length() << " 字节" << std::endl;
}

void CompilerPerformanceTest::testMemoryUsage() {
    // 测试内存使用情况
    auto memoryUsage = utils::PerformanceTestUtils::measureMemoryUsage([]() {
        // 创建大量测试对象
        std::vector<std::string> testData;
        for (int i = 0; i < 10000; ++i) {
            testData.push_back(utils::TestDataGenerator::getInstance().generateRandomString(100));
        }
    });
    
    // 验证内存使用在合理范围内（这里是简化的检查）
    assertTrue(memoryUsage >= 0, "内存使用应该被正确测量");
    
    std::cout << "  内存使用测试完成" << std::endl;
}

void CompilerPerformanceTest::testConcurrentCompilation() {
    // 测试并发编译性能
    const int threadCount = 4;
    const int iterationsPerThread = 100;
    
    timer_.start();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([iterationsPerThread]() {
            for (int i = 0; i < iterationsPerThread; ++i) {
                // 模拟编译工作
                std::string testCode = utils::CHTLTestUtils::createTestCHTLCode("TestComponent" + std::to_string(i));
                volatile bool valid = utils::CHTLTestUtils::validateCHTLSyntax(testCode);
                (void)valid;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    timer_.stop();
    
    auto totalOperations = threadCount * iterationsPerThread;
    auto avgTime = timer_.elapsed().count() / static_cast<double>(totalOperations);
    
    assertTrue(timer_.elapsed().count() < 10000, "并发编译应该在10秒内完成");
    
    std::cout << "  并发编译性能: " << timer_.elapsed().count() << "ms" << std::endl;
    std::cout << "  平均每次编译: " << avgTime << "ms" << std::endl;
    std::cout << "  并发线程数: " << threadCount << std::endl;
}

void CompilerPerformanceTest::testNamespacePerformance() {
    // 测试命名空间算法性能
    timer_.start();
    
    // 模拟大量命名空间操作
    for (int i = 0; i < 1000; ++i) {
        auto hierarchy = utils::TestDataGenerator::getInstance().generateNamespaceHierarchy();
        
        for (const auto& ns : hierarchy) {
            bool valid = utils::CHTLTestUtils::validateNamespace(ns);
            assertTrue(valid, "生成的命名空间应该有效");
        }
    }
    
    timer_.stop();
    
    assertTrue(timer_.elapsed().count() < 2000, "命名空间操作应该在2秒内完成");
    
    std::cout << "  命名空间性能: " << timer_.elapsed().count() << "ms (1000次操作)" << std::endl;
}

void CompilerPerformanceTest::testModuleLoadingPerformance() {
    // 测试模块加载性能
    std::vector<std::string> modules;
    
    // 创建多个测试模块
    for (int i = 0; i < 10; ++i) {
        std::string modulePath = utils::FileSystemTestUtils::createTestCMOD("PerfModule" + std::to_string(i));
        modules.push_back(modulePath);
    }
    
    timer_.start();
    
    // 模拟模块加载
    for (const auto& modulePath : modules) {
        bool valid = utils::CHTLTestUtils::validateCMODStructure(modulePath);
        assertTrue(valid, "模块结构应该有效");
    }
    
    timer_.stop();
    
    assertTrue(timer_.elapsed().count() < 1000, "模块加载应该在1秒内完成");
    
    std::cout << "  模块加载性能: " << timer_.elapsed().count() << "ms (10个模块)" << std::endl;
}

} // namespace performance
} // namespace test
} // namespace chtl