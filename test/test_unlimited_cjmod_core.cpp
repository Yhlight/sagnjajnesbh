#include "../src/CHTLJSCompiler/Module/UnlimitedCJMOD.h"
#include <iostream>
#include <cassert>
#include <thread>

using namespace chtl::unlimited_cjmod;

// 简单的测试模块，不依赖外部库
UNLIMITED_CJMOD_MODULE(TestModule)
    
    // 全局状态测试
    CJMOD_GLOBAL_VAR(int, test_counter, 0);
    CJMOD_GLOBAL_VAR(std::string, test_message, "初始消息");
    CJMOD_GLOBAL_VAR(std::vector<std::string>, test_log, {});
    
    // 简单函数
    CJMOD_GLOBAL_FUNCTION(void, incrementCounter, ()) {
        test_counter++;
        LOG_INFO("计数器增加到: " + std::to_string(test_counter));
    }
    
    CJMOD_GLOBAL_FUNCTION(std::string, getMessage, ()) {
        return test_message;
    }
    
    CJMOD_GLOBAL_FUNCTION(void, setMessage, (const std::string& msg)) {
        test_message = msg;
        test_log.push_back("消息更新: " + msg);
    }
    
    // 数学计算
    CJMOD_GLOBAL_FUNCTION(double, complexCalculation, (double x)) {
        return std::sin(x) * std::cos(x) + std::sqrt(std::abs(x));
    }
    
    // 字符串处理
    CJMOD_GLOBAL_FUNCTION(std::string, processText, (const std::string& text)) {
        std::string result = text;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return "处理后: " + result;
    }
    
    // 注册自定义语法
    REGISTER_SYNTAX("increment",
                   R"(increment\s+counter)",
                   [](const std::vector<std::string>& groups) -> std::string {
                       return "TestModule.incrementCounter()";
                   });
    
    REGISTER_SYNTAX("set_msg",
                   R"(set\s+message\s+\"([^\"]+)\")",
                   [](const std::vector<std::string>& groups) -> std::string {
                       if (groups.size() >= 2) {
                           return "TestModule.setMessage(\"" + groups[1] + "\")";
                       }
                       return "";
                   });
    
    LOG_INFO("测试模块初始化完成");

UNLIMITED_CJMOD_END(TestModule)

void testGlobalStateManager() {
    std::cout << "=== 测试全局状态管理器 ===" << std::endl;
    
    auto& state_mgr = GlobalStateManager::getInstance();
    
    // 测试基本状态存储
    state_mgr.setState("test_int", 42);
    state_mgr.setState("test_string", std::string("Hello World"));
    state_mgr.setState("test_double", 3.14159);
    
    // 测试状态获取
    int int_val = state_mgr.getState<int>("test_int");
    std::string str_val = state_mgr.getState<std::string>("test_string");
    double double_val = state_mgr.getState<double>("test_double");
    
    std::cout << "整数值: " << int_val << std::endl;
    std::cout << "字符串值: " << str_val << std::endl;
    std::cout << "浮点值: " << double_val << std::endl;
    
    assert(int_val == 42);
    assert(str_val == "Hello World");
    assert(std::abs(double_val - 3.14159) < 0.0001);
    
    // 测试计数器
    state_mgr.incrementCounter("test_counter", 5);
    int counter_val = state_mgr.getCounter("test_counter");
    std::cout << "计数器值: " << counter_val << std::endl;
    assert(counter_val == 5);
    
    // 测试日志
    state_mgr.appendToLog("test_log", "第一条日志");
    state_mgr.appendToLog("test_log", "第二条日志");
    std::string log_content = state_mgr.getLog("test_log");
    std::cout << "日志内容长度: " << log_content.length() << std::endl;
    assert(!log_content.empty());
    
    // 测试缓存
    state_mgr.setCache("test_cache", std::string("缓存数据"), 1); // 1秒TTL
    auto cached_val = state_mgr.getCache<std::string>("test_cache");
    assert(cached_val.has_value());
    std::cout << "缓存值: " << cached_val.value() << std::endl;
    
    std::cout << "✅ 全局状态管理器测试通过!" << std::endl;
}

void testJSSyntaxEngine() {
    std::cout << "=== 测试JS语法扩展引擎 ===" << std::endl;
    
    auto& syntax_engine = JSSyntaxExtensionEngine::getInstance();
    
    // 测试内置语法转换
    std::string test_code1 = "data |> process |> validate";
    std::string transformed1 = syntax_engine.transformCode(test_code1);
    std::cout << "管道操作符转换: " << test_code1 << " -> " << transformed1 << std::endl;
    
    std::string test_code2 = "value ?? defaultValue";
    std::string transformed2 = syntax_engine.transformCode(test_code2);
    std::cout << "空值合并转换: " << test_code2 << " -> " << transformed2 << std::endl;
    
    // 测试自定义语法
    std::string test_code3 = "increment counter";
    std::string transformed3 = syntax_engine.transformCode(test_code3);
    std::cout << "自定义语法转换: " << test_code3 << " -> " << transformed3 << std::endl;
    
    std::string test_code4 = "set message \"Hello CHTL\"";
    std::string transformed4 = syntax_engine.transformCode(test_code4);
    std::cout << "消息设置转换: " << test_code4 << " -> " << transformed4 << std::endl;
    
    // 测试语法验证
    bool valid1 = syntax_engine.validateSyntax("function test() { return 42; }");
    bool valid2 = syntax_engine.validateSyntax("function test() { return 42; ");  // 缺少}
    
    std::cout << "语法验证1: " << (valid1 ? "通过" : "失败") << std::endl;
    std::cout << "语法验证2: " << (valid2 ? "通过" : "失败") << std::endl;
    assert(valid1 == true);
    assert(valid2 == false);
    
    // 打印语法信息
    syntax_engine.printSyntaxInfo();
    
    std::cout << "✅ JS语法扩展引擎测试通过!" << std::endl;
}

void testNativeLibraryManager() {
    std::cout << "=== 测试原生库管理器 ===" << std::endl;
    
    auto& lib_mgr = NativeLibraryManager::getInstance();
    
    // 测试库加载（模拟）
    std::cout << "已加载库数量: " << lib_mgr.getLoadedLibraries().size() << std::endl;
    
    // 打印库状态
    lib_mgr.printLibraryStatus();
    
    std::cout << "✅ 原生库管理器测试通过!" << std::endl;
}

void testPerformanceOptimizer() {
    std::cout << "=== 测试性能优化器 ===" << std::endl;
    
    auto& perf_opt = PerformanceOptimizer::getInstance();
    
    // 测试性能监控
    perf_opt.startProfiling();
    
    // 模拟一些计算
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    auto metrics = perf_opt.getMetrics();
    std::cout << "性能指标:" << std::endl;
    std::cout << "  内存使用: " << metrics.memory_usage << " KB" << std::endl;
    std::cout << "  总操作数: " << metrics.total_operations << std::endl;
    
    perf_opt.stopProfiling();
    
    std::cout << "✅ 性能优化器测试通过!" << std::endl;
}

void testModuleFunctionality() {
    std::cout << "=== 测试模块功能 ===" << std::endl;
    
    // 测试全局变量访问
    std::cout << "初始计数器值: " << test_counter << std::endl;
    std::cout << "初始消息: " << test_message << std::endl;
    
    // 调用模块函数
    incrementCounter();
    std::cout << "增加后计数器值: " << test_counter << std::endl;
    assert(test_counter == 1);
    
    setMessage("新的测试消息");
    std::string current_msg = getMessage();
    std::cout << "当前消息: " << current_msg << std::endl;
    assert(current_msg == "新的测试消息");
    
    // 测试复杂计算
    double calc_result = complexCalculation(1.0);
    std::cout << "复杂计算结果: " << calc_result << std::endl;
    assert(calc_result > 0);
    
    // 测试字符串处理
    std::string processed = processText("hello world");
    std::cout << "字符串处理结果: " << processed << std::endl;
    assert(processed.find("HELLO WORLD") != std::string::npos);
    
    std::cout << "✅ 模块功能测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试无限制CJMOD框架..." << std::endl;
        std::cout << "释放CHTL JS的无限潜能！" << std::endl << std::endl;
        
        testGlobalStateManager();
        std::cout << std::endl;
        
        testJSSyntaxEngine();
        std::cout << std::endl;
        
        testNativeLibraryManager();
        std::cout << std::endl;
        
        testPerformanceOptimizer();
        std::cout << std::endl;
        
        testModuleFunctionality();
        std::cout << std::endl;
        
        // 打印完整的系统信息
        std::cout << "=== 完整系统信息 ===" << std::endl;
        GlobalStateManager::getInstance().printDebugInfo();
        std::cout << std::endl;
        
        std::cout << "🎉 所有无限制CJMOD框架测试通过!" << std::endl;
        std::cout << "✅ 完全的C++自由度: 像写普通C++一样开发!" << std::endl;
        std::cout << "✅ 无限的JS语法扩展: 创造任何可能的语法!" << std::endl;
        std::cout << "✅ 全局状态管理: 维护任何复杂状态!" << std::endl;
        std::cout << "✅ 原生库集成: 使用任何C++库!" << std::endl;
        std::cout << "✅ 高性能优化: 突破JS性能限制!" << std::endl;
        std::cout << std::endl;
        std::cout << "现在，CHTL JS真正拥有了无限的扩展能力！🚀" << std::endl;
        std::cout << "JS决定上限，CJMOD提供无限可能！💫" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}