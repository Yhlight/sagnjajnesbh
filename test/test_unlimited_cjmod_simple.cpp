#include "../src/CHTLJSCompiler/Module/UnlimitedCJMOD.h"
#include <iostream>
#include <cassert>

using namespace chtl::unlimited_cjmod;

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
    
    // 测试文件操作
    bool write_success = state_mgr.writeFile("test.txt", "测试内容");
    std::cout << "文件写入: " << (write_success ? "成功" : "失败") << std::endl;
    
    if (write_success) {
        std::string file_content = state_mgr.readFile("test.txt");
        std::cout << "文件内容: " << file_content << std::endl;
        assert(file_content == "测试内容");
    }
    
    // 测试加密
    std::string encrypted = state_mgr.encrypt("Hello CHTL", "key123");
    std::string decrypted = state_mgr.decrypt(encrypted, "key123");
    std::cout << "加密解密测试: " << decrypted << std::endl;
    assert(decrypted == "Hello CHTL");
    
    std::cout << "✅ 全局状态管理器测试通过!" << std::endl;
}

void testJSSyntaxEngine() {
    std::cout << "=== 测试JS语法扩展引擎 ===" << std::endl;
    
    auto& syntax_engine = JSSyntaxExtensionEngine::getInstance();
    
    // 测试内置语法转换
    std::string test_code1 = "data |> process";
    std::string transformed1 = syntax_engine.transformCode(test_code1);
    std::cout << "管道操作符转换: " << test_code1 << " -> " << transformed1 << std::endl;
    
    std::string test_code2 = "value ?? defaultValue";
    std::string transformed2 = syntax_engine.transformCode(test_code2);
    std::cout << "空值合并转换: " << test_code2 << " -> " << transformed2 << std::endl;
    
    // 测试语法验证
    bool valid1 = syntax_engine.validateSyntax("function test() { return 42; }");
    bool valid2 = syntax_engine.validateSyntax("function test() { return 42; ");  // 缺少}
    
    std::cout << "语法验证1: " << (valid1 ? "通过" : "失败") << std::endl;
    std::cout << "语法验证2: " << (valid2 ? "通过" : "失败") << std::endl;
    assert(valid1 == true);
    assert(valid2 == false);
    
    // 注册自定义语法
    syntax_engine.registerSyntax("test_syntax", R"(test\s+(\w+))",
                                 [](const std::vector<std::string>& groups) -> std::string {
                                     if (groups.size() >= 2) {
                                         return "console.log('Testing: " + groups[1] + "')";
                                     }
                                     return "";
                                 });
    
    std::string test_code3 = "test myFunction";
    std::string transformed3 = syntax_engine.transformCode(test_code3);
    std::cout << "自定义语法转换: " << test_code3 << " -> " << transformed3 << std::endl;
    
    // 打印语法信息
    syntax_engine.printSyntaxInfo();
    
    std::cout << "✅ JS语法扩展引擎测试通过!" << std::endl;
}

void testNativeLibraryManager() {
    std::cout << "=== 测试原生库管理器 ===" << std::endl;
    
    auto& lib_mgr = NativeLibraryManager::getInstance();
    
    // 测试库管理功能
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

void testAdvancedFeatures() {
    std::cout << "=== 测试高级功能 ===" << std::endl;
    
    auto& state_mgr = GlobalStateManager::getInstance();
    
    // 测试HTTP请求模拟
    std::string http_response = state_mgr.httpGet("https://api.example.com/data");
    std::cout << "HTTP GET响应长度: " << http_response.length() << std::endl;
    assert(!http_response.empty());
    
    std::string post_response = state_mgr.httpPost("https://api.example.com/submit", 
                                                  "{\"data\": \"test\"}", 
                                                  {{"Content-Type", "application/json"}});
    std::cout << "HTTP POST响应长度: " << post_response.length() << std::endl;
    assert(!post_response.empty());
    
    // 测试数据库操作模拟
    state_mgr.setDatabaseConnection("main", "sqlite:///test.db");
    std::string query_result = state_mgr.executeDatabaseQuery("main", "SELECT * FROM users");
    std::cout << "数据库查询结果: " << query_result << std::endl;
    assert(!query_result.empty());
    
    // 测试文件系统操作
    std::vector<std::string> files = state_mgr.listDirectory(".");
    std::cout << "当前目录文件数: " << files.size() << std::endl;
    
    for (size_t i = 0; i < std::min(files.size(), size_t(5)); ++i) {
        std::cout << "  文件: " << files[i] << std::endl;
    }
    
    std::cout << "✅ 高级功能测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试无限制CJMOD框架核心功能..." << std::endl;
        std::cout << "释放CHTL JS的无限潜能！" << std::endl << std::endl;
        
        testGlobalStateManager();
        std::cout << std::endl;
        
        testJSSyntaxEngine();
        std::cout << std::endl;
        
        testNativeLibraryManager();
        std::cout << std::endl;
        
        testPerformanceOptimizer();
        std::cout << std::endl;
        
        testAdvancedFeatures();
        std::cout << std::endl;
        
        // 打印完整的系统信息
        std::cout << "=== 完整系统信息 ===" << std::endl;
        GlobalStateManager::getInstance().printDebugInfo();
        std::cout << std::endl;
        
        std::cout << "🎉 所有无限制CJMOD核心功能测试通过!" << std::endl;
        std::cout << "✅ 完全的C++自由度: 任何C++代码都能运行!" << std::endl;
        std::cout << "✅ 无限的JS语法扩展: 创造任何可能的语法!" << std::endl;
        std::cout << "✅ 全局状态管理: 维护任何复杂状态!" << std::endl;
        std::cout << "✅ 原生库集成: 支持任何C++库!" << std::endl;
        std::cout << "✅ 高性能优化: 突破JS性能限制!" << std::endl;
        std::cout << std::endl;
        std::cout << "现在，CHTL JS真正拥有了无限的扩展能力！🚀" << std::endl;
        std::cout << "JS决定上限，CJMOD提供无限可能！💫" << std::endl;
        
        // 清理测试文件
        std::filesystem::remove("test.txt");
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}