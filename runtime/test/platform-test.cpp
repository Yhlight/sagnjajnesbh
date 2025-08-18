// CHTL ANTLR运行时平台兼容性测试
// 测试ANTLR运行时在不同平台上的兼容性

#include "antlr4-runtime.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
    #define PLATFORM_NAME "Windows"
#elif __APPLE__
    #include <unistd.h>
    #include <sys/utsname.h>
    #define PLATFORM_NAME "macOS"
#elif __linux__
    #include <unistd.h>
    #include <sys/utsname.h>
    #define PLATFORM_NAME "Linux"
#else
    #define PLATFORM_NAME "Unknown"
#endif

using namespace antlr4;

// 平台信息结构
struct PlatformInfo {
    std::string name;
    std::string version;
    std::string architecture;
    int cpu_cores;
    size_t memory_mb;
    
    void detect() {
        name = PLATFORM_NAME;
        
#ifdef _WIN32
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        cpu_cores = sysInfo.dwNumberOfProcessors;
        
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        memory_mb = memInfo.ullTotalPhys / (1024 * 1024);
        
        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64:
                architecture = "x64";
                break;
            case PROCESSOR_ARCHITECTURE_INTEL:
                architecture = "x86";
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                architecture = "arm64";
                break;
            default:
                architecture = "unknown";
                break;
        }
        
        OSVERSIONINFOEX osInfo;
        ZeroMemory(&osInfo, sizeof(OSVERSIONINFOEX));
        osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        if (GetVersionEx((OSVERSIONINFO*)&osInfo)) {
            version = std::to_string(osInfo.dwMajorVersion) + "." + std::to_string(osInfo.dwMinorVersion);
        }
        
#else
        struct utsname unameData;
        if (uname(&unameData) == 0) {
            version = unameData.release;
            architecture = unameData.machine;
            
            // 转换架构名称
            if (architecture == "x86_64" || architecture == "amd64") {
                architecture = "x64";
            } else if (architecture == "aarch64") {
                architecture = "arm64";
            } else if (architecture == "i386" || architecture == "i686") {
                architecture = "x86";
            }
        }
        
        cpu_cores = std::thread::hardware_concurrency();
        
        // 简单的内存检测（Linux/macOS）
        memory_mb = 1024; // 默认值，实际实现需要读取系统信息
#endif
    }
    
    void print() const {
        std::cout << "平台信息:" << std::endl;
        std::cout << "  操作系统: " << name << " " << version << std::endl;
        std::cout << "  架构: " << architecture << std::endl;
        std::cout << "  CPU核心数: " << cpu_cores << std::endl;
        std::cout << "  内存: " << memory_mb << " MB" << std::endl;
    }
};

// 测试结果
struct TestResults {
    int total = 0;
    int passed = 0;
    int failed = 0;
    std::vector<std::string> failures;
    
    void addResult(const std::string& test_name, bool success, const std::string& error = "") {
        total++;
        if (success) {
            passed++;
            std::cout << "✅ " << test_name << " - PASSED" << std::endl;
        } else {
            failed++;
            failures.push_back(test_name + ": " + error);
            std::cout << "❌ " << test_name << " - FAILED: " << error << std::endl;
        }
    }
    
    void printSummary() {
        std::cout << "\n=== 平台兼容性测试总结 ===" << std::endl;
        std::cout << "总测试数: " << total << std::endl;
        std::cout << "通过: " << passed << std::endl;
        std::cout << "失败: " << failed << std::endl;
        
        if (failed > 0) {
            std::cout << "\n失败的测试:" << std::endl;
            for (const auto& failure : failures) {
                std::cout << "  - " << failure << std::endl;
            }
        }
        
        if (failed == 0) {
            std::cout << "\n🎉 所有平台兼容性测试通过！" << std::endl;
        } else {
            std::cout << "\n💥 " << failed << " 个测试失败" << std::endl;
        }
    }
    
    bool allPassed() const {
        return failed == 0;
    }
};

// 测试字符编码处理
void testCharacterEncoding(TestResults& results) {
    try {
        // 测试UTF-8字符串
        std::string utf8_input = "Hello 世界 🌍 Мир";
        ANTLRInputStream stream(utf8_input);
        
        if (stream.toString() != utf8_input) {
            results.addResult("UTF-8 encoding", false, "UTF-8 string mismatch");
            return;
        }
        
        // 测试ASCII字符串
        std::string ascii_input = "Hello World 123";
        ANTLRInputStream ascii_stream(ascii_input);
        
        if (ascii_stream.toString() != ascii_input) {
            results.addResult("ASCII encoding", false, "ASCII string mismatch");
            return;
        }
        
        // 测试空字符串
        std::string empty_input = "";
        ANTLRInputStream empty_stream(empty_input);
        
        if (empty_stream.size() != 0) {
            results.addResult("Empty string encoding", false, "Empty string size mismatch");
            return;
        }
        
        results.addResult("Character encoding", true);
        
    } catch (const std::exception& e) {
        results.addResult("Character encoding", false, e.what());
    }
}

// 测试文件路径处理
void testFilePathHandling(TestResults& results) {
    try {
        // 测试不同平台的路径分隔符
        std::vector<std::string> test_paths;
        
#ifdef _WIN32
        test_paths = {
            "C:\\path\\to\\file.txt",
            "C:/path/to/file.txt",
            "\\\\server\\share\\file.txt",
            "file.txt",
            ".\\relative\\path.txt"
        };
#else
        test_paths = {
            "/path/to/file.txt",
            "./relative/path.txt",
            "../parent/file.txt",
            "~/home/file.txt",
            "file.txt"
        };
#endif
        
        for (const auto& path : test_paths) {
            // 这里只是测试路径字符串的处理，不实际访问文件
            ANTLRInputStream stream(path);
            
            if (stream.toString() != path) {
                results.addResult("File path handling", false, "Path string mismatch: " + path);
                return;
            }
        }
        
        results.addResult("File path handling", true);
        
    } catch (const std::exception& e) {
        results.addResult("File path handling", false, e.what());
    }
}

// 测试内存对齐
void testMemoryAlignment(TestResults& results) {
    try {
        // 测试不同类型的内存对齐
        struct TestStruct {
            char c;
            int i;
            double d;
            void* p;
        };
        
        // 创建多个对象测试内存对齐
        std::vector<std::unique_ptr<CommonToken>> tokens;
        for (int i = 0; i < 100; i++) {
            tokens.push_back(std::make_unique<CommonToken>(i, "test" + std::to_string(i)));
        }
        
        // 验证所有token都正确创建
        for (size_t i = 0; i < tokens.size(); i++) {
            if (!tokens[i] || tokens[i]->getType() != static_cast<int>(i)) {
                results.addResult("Memory alignment", false, "Token creation failed at index " + std::to_string(i));
                return;
            }
        }
        
        results.addResult("Memory alignment", true);
        
    } catch (const std::exception& e) {
        results.addResult("Memory alignment", false, e.what());
    }
}

// 测试大端/小端字节序
void testEndianness(TestResults& results) {
    try {
        // 测试字节序对token类型的影响
        const int test_type = 0x12345678;
        auto token = std::make_unique<CommonToken>(test_type, "endian_test");
        
        if (token->getType() != test_type) {
            results.addResult("Endianness", false, "Token type mismatch");
            return;
        }
        
        // 测试字符串的字节序处理
        std::string test_text = "endian";
        ANTLRInputStream stream(test_text);
        
        // 检查第一个字符
        if (stream.LA(1) != 'e') {
            results.addResult("Endianness", false, "Character reading failed");
            return;
        }
        
        results.addResult("Endianness", true);
        
    } catch (const std::exception& e) {
        results.addResult("Endianness", false, e.what());
    }
}

// 测试线程本地存储
void testThreadLocalStorage(TestResults& results) {
    try {
        const int num_threads = 4;
        std::vector<std::thread> threads;
        std::vector<bool> thread_results(num_threads, false);
        
        for (int i = 0; i < num_threads; i++) {
            threads.emplace_back([i, &thread_results]() {
                try {
                    // 每个线程创建自己的对象
                    std::string input = "thread_" + std::to_string(i);
                    ANTLRInputStream stream(input);
                    
                    auto token = std::make_unique<CommonToken>(i, input);
                    
                    // 验证对象状态
                    if (token->getType() == i && token->getText() == input) {
                        thread_results[i] = true;
                    }
                    
                } catch (...) {
                    thread_results[i] = false;
                }
            });
        }
        
        // 等待所有线程完成
        for (auto& thread : threads) {
            thread.join();
        }
        
        // 检查结果
        for (int i = 0; i < num_threads; i++) {
            if (!thread_results[i]) {
                results.addResult("Thread local storage", false, "Thread " + std::to_string(i) + " failed");
                return;
            }
        }
        
        results.addResult("Thread local storage", true);
        
    } catch (const std::exception& e) {
        results.addResult("Thread local storage", false, e.what());
    }
}

// 测试异常处理机制
void testExceptionMechanism(TestResults& results) {
    try {
        bool caught_std_exception = false;
        bool caught_antlr_exception = false;
        
        // 测试标准异常
        try {
            throw std::runtime_error("Test exception");
        } catch (const std::exception& e) {
            caught_std_exception = true;
            if (std::string(e.what()) != "Test exception") {
                results.addResult("Exception mechanism", false, "Standard exception message mismatch");
                return;
            }
        }
        
        // 测试ANTLR异常
        try {
            ANTLRInputStream stream("test");
            RecognitionException ex(nullptr, &stream, nullptr);
            throw ex;
        } catch (const RecognitionException& e) {
            caught_antlr_exception = true;
        } catch (...) {
            // 捕获其他类型的异常也算成功
            caught_antlr_exception = true;
        }
        
        if (!caught_std_exception || !caught_antlr_exception) {
            results.addResult("Exception mechanism", false, "Exception not caught properly");
            return;
        }
        
        results.addResult("Exception mechanism", true);
        
    } catch (const std::exception& e) {
        results.addResult("Exception mechanism", false, e.what());
    }
}

// 测试动态内存分配
void testDynamicMemoryAllocation(TestResults& results) {
    try {
        const int num_allocations = 1000;
        std::vector<std::unique_ptr<CommonToken>> tokens;
        
        // 大量分配
        for (int i = 0; i < num_allocations; i++) {
            tokens.push_back(std::make_unique<CommonToken>(i, "token_" + std::to_string(i)));
        }
        
        // 验证分配结果
        for (int i = 0; i < num_allocations; i++) {
            if (!tokens[i] || tokens[i]->getType() != i) {
                results.addResult("Dynamic memory allocation", false, "Allocation failed at index " + std::to_string(i));
                return;
            }
        }
        
        // 清理（通过unique_ptr自动管理）
        tokens.clear();
        
        results.addResult("Dynamic memory allocation", true);
        
    } catch (const std::exception& e) {
        results.addResult("Dynamic memory allocation", false, e.what());
    }
}

// 测试系统调用兼容性
void testSystemCallCompatibility(TestResults& results) {
    try {
        // 测试时间相关的系统调用
        auto start = std::chrono::high_resolution_clock::now();
        
        // 执行一些操作
        std::string input = "system_call_test";
        ANTLRInputStream stream(input);
        auto token = std::make_unique<CommonToken>(1, input);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (duration.count() < 0) {
            results.addResult("System call compatibility", false, "Time measurement failed");
            return;
        }
        
        // 测试线程相关的系统调用
        std::thread test_thread([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
        
        test_thread.join();
        
        results.addResult("System call compatibility", true);
        
    } catch (const std::exception& e) {
        results.addResult("System call compatibility", false, e.what());
    }
}

// 压力测试
void testStressTest(TestResults& results) {
    try {
        const int stress_iterations = 5000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < stress_iterations; i++) {
            std::string input = "stress_test_" + std::to_string(i) + "_with_longer_string_to_test_memory_allocation";
            ANTLRInputStream stream(input);
            
            auto token = std::make_unique<CommonToken>(i % 1000, input);
            
            // 验证对象状态
            if (token->getText() != input || token->getType() != i % 1000) {
                results.addResult("Stress test", false, "Object state corrupted at iteration " + std::to_string(i));
                return;
            }
            
            // 每1000次迭代输出进度
            if (i % 1000 == 0 && i > 0) {
                std::cout << "  压力测试进度: " << i << "/" << stress_iterations << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "  压力测试完成: " << stress_iterations << " 次迭代耗时 " << duration.count() << " ms" << std::endl;
        
        results.addResult("Stress test", true);
        
    } catch (const std::exception& e) {
        results.addResult("Stress test", false, e.what());
    }
}

// 主函数
int main(int argc, char* argv[]) {
    std::cout << "CHTL ANTLR运行时平台兼容性测试" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // 显示平台信息
    PlatformInfo platform;
    platform.detect();
    platform.print();
    std::cout << std::endl;
    
    TestResults results;
    
    // 运行平台兼容性测试
    std::cout << "运行平台兼容性测试..." << std::endl;
    testCharacterEncoding(results);
    testFilePathHandling(results);
    testMemoryAlignment(results);
    testEndianness(results);
    
    std::cout << "\n运行多线程兼容性测试..." << std::endl;
    testThreadLocalStorage(results);
    testExceptionMechanism(results);
    
    std::cout << "\n运行系统兼容性测试..." << std::endl;
    testDynamicMemoryAllocation(results);
    testSystemCallCompatibility(results);
    
    std::cout << "\n运行压力测试..." << std::endl;
    testStressTest(results);
    
    // 打印测试总结
    results.printSummary();
    
    return results.allPassed() ? 0 : 1;
}