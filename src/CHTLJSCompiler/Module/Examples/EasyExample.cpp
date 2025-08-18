// 超级简单的CJMOD示例 - 展示新框架的易用性
// 任何C++初学者都能轻松理解和使用！

#include "../CHTLJSIntegration.h"
#include <chrono>
#include <ctime>

using namespace chtl::cjmod;

// 方式1：使用新的便利宏（超级简单！）
CHTL_CJMOD_MODULE(EasyDemo)
    
    // 设置模块信息（可选）
    module->setDescription("超级简单的演示模块")
          .setAuthor("开发者")
          .setVersion("1.0.0");
    
    // 最简单的函数 - 问候功能
    CHTL_FUNCTION(sayHello, "向用户问候") {
        // 自动参数验证
        CHTL_REQUIRE_ARGS(1);
        CHTL_REQUIRE_STRING(0);
        
        // 获取参数
        std::string name = ctx.getArg(0).toString();
        
        // 返回结果
        return JSValue("Hello, " + name + "! 欢迎使用CHTL JS!");
    }, "向指定用户问候");
    
    // 简单的计算器
    CHTL_FUNCTION(calculate, "简单计算器") {
        CHTL_REQUIRE_ARGS_RANGE(2, 3);
        CHTL_REQUIRE_NUMBER(0);
        CHTL_REQUIRE_NUMBER(1);
        
        double a = ctx.getArg(0).toDouble();
        double b = ctx.getArg(1).toDouble();
        std::string operation = ctx.getArg(2, JSValue("add")).toString();
        
        double result = 0;
        if (operation == "add") {
            result = a + b;
        } else if (operation == "subtract") {
            result = a - b;
        } else if (operation == "multiply") {
            result = a * b;
        } else if (operation == "divide") {
            if (b == 0) {
                return helpers::makeError("不能除以零!");
            }
            result = a / b;
        } else {
            return helpers::makeError("不支持的操作: " + operation);
        }
        
        return JSValue(result);
    }, "执行基本数学运算");
    
    // 获取当前时间
    CHTL_FUNCTION(getCurrentTime, "获取当前时间") {
        // 无需参数
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        JSValue timeInfo;
        timeInfo["timestamp"] = JSValue(static_cast<double>(time_t));
        timeInfo["readable"] = JSValue(std::ctime(&time_t));
        timeInfo["message"] = JSValue("当前时间获取成功");
        
        return timeInfo;
    }, "获取当前系统时间");
    
    // 数组处理示例
    CHTL_FUNCTION(processArray, "处理数组") {
        CHTL_REQUIRE_ARGS(1);
        
        if (!ctx.getArg(0).isArray()) {
            return helpers::makeError("参数必须是数组");
        }
        
        auto input_array = ctx.getArg(0).toArray();
        
        JSValue result;
        result["length"] = JSValue(static_cast<int>(input_array.size()));
        
        // 计算数字总和
        double sum = 0;
        int number_count = 0;
        for (const auto& item : input_array) {
            if (item.isNumber()) {
                sum += item.toDouble();
                number_count++;
            }
        }
        
        result["sum"] = JSValue(sum);
        result["average"] = JSValue(number_count > 0 ? sum / number_count : 0);
        result["numbers_found"] = JSValue(number_count);
        
        return result;
    }, "处理数组并返回统计信息");

CHTL_CJMOD_END(EasyDemo)

// 方式2：传统方式（给熟悉的开发者）
static void register_advanced_demo() {
    auto module = std::make_unique<CJMODModule>("AdvancedDemo", "1.0.0");
    
    module->setDescription("高级功能演示")
          .setAuthor("CHTL Team");
    
    // 复杂的数据处理
    module->addFunction("processData", [](const JSContext& ctx) -> JSValue {
        if (ctx.getArgCount() < 1) {
            return helpers::makeError("需要至少1个参数");
        }
        
        JSValue input = ctx.getArg(0);
        JSValue result;
        
        // 根据输入类型进行不同处理
        switch (input.getType()) {
            case JSValue::STRING: {
                std::string text = input.toString();
                result["type"] = JSValue("string");
                result["length"] = JSValue(static_cast<int>(text.length()));
                result["uppercase"] = JSValue(text); // 简化，实际应该转大写
                break;
            }
            case JSValue::NUMBER: {
                double num = input.toDouble();
                result["type"] = JSValue("number");
                result["value"] = JSValue(num);
                result["squared"] = JSValue(num * num);
                result["is_positive"] = JSValue(num > 0);
                break;
            }
            case JSValue::ARRAY: {
                auto arr = input.toArray();
                result["type"] = JSValue("array");
                result["length"] = JSValue(static_cast<int>(arr.size()));
                result["first"] = arr.empty() ? JSValue() : arr[0];
                break;
            }
            default:
                result["type"] = JSValue("unknown");
                result["message"] = JSValue("不支持的数据类型");
        }
        
        return result;
    }, "智能数据处理函数");
    
    // 异步操作模拟
    module->addFunction("simulateAsync", [](const JSContext& ctx) -> JSValue {
        std::string operation = ctx.getArg(0, JSValue("default")).toString();
        
        ctx.logInfo("开始异步操作: " + operation);
        
        // 模拟异步处理
        JSValue result;
        result["operation"] = JSValue(operation);
        result["status"] = JSValue("completed");
        result["duration"] = JSValue("模拟1秒");
        result["success"] = JSValue(true);
        
        return result;
    }, "模拟异步操作");
    
    CJMODRegistry::getInstance().registerModule(std::move(module));
}

// 自动注册高级演示模块
static bool advanced_demo_registered = []() {
    register_advanced_demo();
    return true;
}();