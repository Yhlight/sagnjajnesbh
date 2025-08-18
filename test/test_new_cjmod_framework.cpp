#include "../src/CHTLJSCompiler/Module/CHTLJSIntegration.h"
#include "../src/CHTLJSCompiler/Module/Examples/SimpleMath.cpp"
#include "../src/CHTLJSCompiler/Module/Examples/StringUtils.cpp"
#include "../src/CHTLJSCompiler/Module/Examples/EasyExample.cpp"
#include <iostream>
#include <cassert>
#include <cstring>

using namespace chtl::cjmod;

void testFrameworkBasics() {
    std::cout << "=== 测试CJMOD框架基础功能 ===" << std::endl;
    
    // 测试JSValue类型系统
    JSValue str_val("Hello World");
    JSValue num_val(42.5);
    JSValue bool_val(true);
    JSValue array_val(std::vector<JSValue>{JSValue(1), JSValue(2), JSValue(3)});
    
    assert(str_val.isString());
    assert(num_val.isNumber());
    assert(bool_val.isBool());
    assert(array_val.isArray());
    
    std::cout << "字符串值: " << str_val.toString() << std::endl;
    std::cout << "数字值: " << num_val.toDouble() << std::endl;
    std::cout << "布尔值: " << bool_val.toBool() << std::endl;
    std::cout << "数组长度: " << array_val.toArray().size() << std::endl;
    
    // 测试对象
    JSValue obj;
    obj["name"] = JSValue("CHTL");
    obj["version"] = JSValue("1.0.0");
    obj["active"] = JSValue(true);
    
    assert(obj.isObject());
    assert(obj["name"].toString() == "CHTL");
    assert(obj["version"].toString() == "1.0.0");
    assert(obj["active"].toBool() == true);
    
    std::cout << "对象调试信息: " << obj.toDebugString() << std::endl;
    
    std::cout << "✅ 框架基础功能测试通过!" << std::endl;
}

void testModuleRegistration() {
    std::cout << "=== 测试模块注册 ===" << std::endl;
    
    auto& registry = CJMODRegistry::getInstance();
    
    // 检查自动注册的模块
    auto module_names = registry.getModuleNames();
    std::cout << "已注册模块数量: " << module_names.size() << std::endl;
    
    for (const auto& name : module_names) {
        std::cout << "模块: " << name << std::endl;
        auto module = registry.getModule(name);
        if (module) {
            auto func_names = module->getFunctionNames();
            std::cout << "  函数数量: " << func_names.size() << std::endl;
            for (const auto& func_name : func_names) {
                std::cout << "    - " << func_name << std::endl;
            }
        }
    }
    
    // 验证特定模块存在
    assert(registry.hasModule("SimpleMath"));
    assert(registry.hasModule("StringUtils"));
    assert(registry.hasModule("EasyDemo"));
    
    std::cout << "✅ 模块注册测试通过!" << std::endl;
}

void testFunctionCalls() {
    std::cout << "=== 测试函数调用 ===" << std::endl;
    
    auto& registry = CJMODRegistry::getInstance();
    
    // 测试SimpleMath模块
    std::cout << "测试SimpleMath.add(10, 20):" << std::endl;
    std::vector<JSValue> args = {JSValue(10), JSValue(20)};
    JSValue result = registry.callFunction("SimpleMath", "add", args);
    std::cout << "结果: " << result.toDebugString() << std::endl;
    assert(result.isNumber());
    assert(result.toDouble() == 30.0);
    
    // 测试multiply函数
    std::cout << "测试SimpleMath.multiply(2, 3, 4):" << std::endl;
    std::vector<JSValue> multiply_args = {JSValue(2), JSValue(3), JSValue(4)};
    JSValue multiply_result = registry.callFunction("SimpleMath", "multiply", multiply_args);
    std::cout << "结果: " << multiply_result.toDebugString() << std::endl;
    assert(multiply_result.toDouble() == 24.0);
    
    // 测试StringUtils模块
    std::cout << "测试StringUtils.reverse('Hello'):" << std::endl;
    std::vector<JSValue> reverse_args = {JSValue("Hello")};
    JSValue reverse_result = registry.callFunction("StringUtils", "reverse", reverse_args);
    std::cout << "结果: " << reverse_result.toString() << std::endl;
    assert(reverse_result.toString() == "olleH");
    
    // 测试EasyDemo模块
    std::cout << "测试EasyDemo.sayHello('World'):" << std::endl;
    std::vector<JSValue> hello_args = {JSValue("World")};
    JSValue hello_result = registry.callFunction("EasyDemo", "sayHello", hello_args);
    std::cout << "结果: " << hello_result.toString() << std::endl;
    assert(hello_result.toString() == "Hello, World! 欢迎使用CHTL JS!");
    
    std::cout << "✅ 函数调用测试通过!" << std::endl;
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理 ===" << std::endl;
    
    auto& registry = CJMODRegistry::getInstance();
    
    // 测试不存在的模块
    std::vector<JSValue> args = {JSValue(1), JSValue(2)};
    JSValue result1 = registry.callFunction("NonExistentModule", "someFunction", args);
    std::cout << "不存在模块的调用结果: " << result1.toDebugString() << std::endl;
    assert(result1.isObject());
    assert(result1["error"].toBool() == true);
    
    // 测试不存在的函数
    JSValue result2 = registry.callFunction("SimpleMath", "nonExistentFunction", args);
    std::cout << "不存在函数的调用结果: " << result2.toDebugString() << std::endl;
    assert(result2.isObject());
    assert(result2["error"].toBool() == true);
    
    // 测试参数错误
    std::vector<JSValue> wrong_args = {JSValue("not a number")};
    JSValue result3 = registry.callFunction("SimpleMath", "add", wrong_args);
    std::cout << "参数错误的调用结果: " << result3.toDebugString() << std::endl;
    
    // 测试除零错误
    std::vector<JSValue> divide_args = {JSValue(10), JSValue(0)};
    JSValue result4 = registry.callFunction("EasyDemo", "calculate", 
        {JSValue(10), JSValue(0), JSValue("divide")});
    std::cout << "除零错误的调用结果: " << result4.toDebugString() << std::endl;
    assert(result4.isObject());
    assert(result4["error"].toBool() == true);
    
    std::cout << "✅ 错误处理测试通过!" << std::endl;
}

void testCHTLJSIntegration() {
    std::cout << "=== 测试CHTL JS集成 ===" << std::endl;
    
    auto& integration = CHTLJSIntegration::getInstance();
    
    // 初始化集成系统
    bool init_result = integration.initialize();
    assert(init_result);
    std::cout << "集成系统初始化: " << (init_result ? "成功" : "失败") << std::endl;
    
    // 测试函数调用接口
    std::string call_result = integration.processCJMODCall("EasyDemo", "sayHello", "[\"CHTL\"]");
    std::cout << "集成调用结果: " << call_result << std::endl;
    
    // 生成JavaScript代码
    std::string prelude = integration.generateCJMODPrelude();
    std::cout << "前导代码长度: " << prelude.length() << " 字符" << std::endl;
    assert(!prelude.empty());
    
    std::string bindings = integration.generateModuleBindings();
    std::cout << "绑定代码长度: " << bindings.length() << " 字符" << std::endl;
    assert(!bindings.empty());
    
    std::string epilogue = integration.generateCJMODEpilogue();
    std::cout << "结尾代码长度: " << epilogue.length() << " 字符" << std::endl;
    assert(!epilogue.empty());
    
    // 生成完整的集成代码
    std::string integrated_js = integration.generateIntegratedJavaScript();
    std::cout << "完整集成代码长度: " << integrated_js.length() << " 字符" << std::endl;
    
    // 显示部分生成的代码
    std::cout << "生成的JavaScript代码片段:" << std::endl;
    std::cout << integrated_js.substr(0, 300) << "..." << std::endl;
    
    std::cout << "✅ CHTL JS集成测试通过!" << std::endl;
}

void testComplexScenarios() {
    std::cout << "=== 测试复杂场景 ===" << std::endl;
    
    auto& registry = CJMODRegistry::getInstance();
    
    // 场景1: 字符串处理链
    std::cout << "场景1: 字符串处理链" << std::endl;
    
    // 分割字符串
    std::vector<JSValue> split_args = {JSValue("apple,banana,cherry"), JSValue(",")};
    JSValue split_result = registry.callFunction("StringUtils", "split", split_args);
    std::cout << "分割结果: " << split_result.toDebugString() << std::endl;
    assert(split_result.isArray());
    assert(split_result.toArray().size() == 3);
    
    // 连接字符串
    std::vector<JSValue> join_args = {split_result, JSValue(" | ")};
    JSValue join_result = registry.callFunction("StringUtils", "join", join_args);
    std::cout << "连接结果: " << join_result.toString() << std::endl;
    assert(join_result.toString() == "apple | banana | cherry");
    
    // 场景2: 数学计算链
    std::cout << "场景2: 数学计算链" << std::endl;
    
    // 幂运算
    std::vector<JSValue> power_args = {JSValue(2), JSValue(3)};
    JSValue power_result = registry.callFunction("SimpleMath", "power", power_args);
    std::cout << "幂运算结果: " << power_result.toDebugString() << std::endl;
    assert(power_result.isObject());
    assert(power_result["result"].toDouble() == 8.0);
    
    // 场景3: 数据处理
    std::cout << "场景3: 数据处理" << std::endl;
    
    // 处理数组
    std::vector<JSValue> array_data = {JSValue(1), JSValue(2), JSValue(3), JSValue(4), JSValue(5)};
    std::vector<JSValue> process_args = {JSValue(array_data)};
    JSValue process_result = registry.callFunction("EasyDemo", "processArray", process_args);
    std::cout << "数组处理结果: " << process_result.toDebugString() << std::endl;
    assert(process_result.isObject());
    assert(process_result["length"].toInt() == 5);
    assert(process_result["sum"].toDouble() == 15.0);
    assert(process_result["average"].toDouble() == 3.0);
    
    std::cout << "✅ 复杂场景测试通过!" << std::endl;
}

void testCInterfaceCompatibility() {
    std::cout << "=== 测试C接口兼容性 ===" << std::endl;
    
    // 测试C接口初始化
    int c_init = cjmod_initialize();
    assert(c_init == 1);
    std::cout << "C接口初始化: " << (c_init ? "成功" : "失败") << std::endl;
    
    // 测试C接口函数调用
    const char* c_result = cjmod_call_function("EasyDemo", "sayHello", "[\"C Interface\"]");
    std::cout << "C接口调用结果: " << (c_result ? c_result : "null") << std::endl;
    assert(c_result != nullptr);
    
    // 测试C接口代码生成
    const char* c_prelude = cjmod_generate_prelude();
    assert(c_prelude != nullptr);
    std::cout << "C接口前导代码长度: " << strlen(c_prelude) << " 字符" << std::endl;
    
    const char* c_bindings = cjmod_generate_bindings();
    assert(c_bindings != nullptr);
    std::cout << "C接口绑定代码长度: " << strlen(c_bindings) << " 字符" << std::endl;
    
    // 测试错误处理
    int has_error = cjmod_has_error();
    std::cout << "是否有错误: " << has_error << std::endl;
    
    if (has_error) {
        const char* error_msg = cjmod_get_error();
        std::cout << "错误信息: " << (error_msg ? error_msg : "null") << std::endl;
    }
    
    std::cout << "✅ C接口兼容性测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试新的CJMOD框架..." << std::endl;
        std::cout << "让CHTL JS扩展开发变得超级简单！" << std::endl << std::endl;
        
        testFrameworkBasics();
        std::cout << std::endl;
        
        testModuleRegistration();
        std::cout << std::endl;
        
        testFunctionCalls();
        std::cout << std::endl;
        
        testErrorHandling();
        std::cout << std::endl;
        
        testCHTLJSIntegration();
        std::cout << std::endl;
        
        testComplexScenarios();
        std::cout << std::endl;
        
        testCInterfaceCompatibility();
        std::cout << std::endl;
        
        // 打印完整的注册表信息
        std::cout << "=== 完整注册表信息 ===" << std::endl;
        CJMODRegistry::getInstance().printRegistryInfo();
        
        // 打印集成系统信息
        std::cout << "=== 集成系统信息 ===" << std::endl;
        CHTLJSIntegration::getInstance().printIntegrationInfo();
        
        std::cout << std::endl;
        std::cout << "🎉 所有新CJMOD框架测试通过!" << std::endl;
        std::cout << "✅ 超简单的API: 任何开发者都能轻松使用!" << std::endl;
        std::cout << "✅ 自动类型转换: 无需手动处理复杂类型!" << std::endl;
        std::cout << "✅ 完善的错误处理: 友好的错误信息!" << std::endl;
        std::cout << "✅ 无缝CHTL JS集成: 一键生成JavaScript绑定!" << std::endl;
        std::cout << "✅ 丰富的示例: 从简单到复杂的完整演示!" << std::endl;
        std::cout << std::endl;
        std::cout << "现在，普通开发者也能轻松扩展CHTL JS功能了！🚀" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}