// 示例：超级简单的数学模块
// 展示CJMOD框架的易用性

#include "../CJMODFramework.h"
#include <cmath>

using namespace chtl::cjmod;

// 方式1：使用宏定义（最简单）
CJMOD_MODULE(SimpleMath)
    
    // 设置模块信息
    module->setDescription("简单的数学计算模块")
          .setAuthor("CHTL Team")
          .setVersion("1.0.0")
          .setHomepage("https://github.com/chtl-lang/chtl");
    
    // 方式1：直接使用Lambda（超级简单！）
    CJMOD_FUNCTION_DESC(add, [](const JSContext& ctx) -> JSValue {
        // 自动参数验证
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要2个参数");
        }
        
        // 自动类型转换
        double a = ctx.getArg(0).toDouble();
        double b = ctx.getArg(1).toDouble();
        
        // 返回结果
        return JSValue(a + b);
    }, "两个数字相加");
    
    // 方式2：更详细的函数定义
    module->addFunction("multiply", [](const JSContext& ctx) -> JSValue {
        if (ctx.getArgCount() < 2) {
            ctx.throwError("至少需要2个参数");
        }
        
        double result = 1.0;
        for (size_t i = 0; i < ctx.getArgCount(); ++i) {
            result *= ctx.getArg(i).toDouble();
        }
        
        return JSValue(result);
    }, "多个数字相乘");
    
    module->getFunction("multiply")
          ->addParameter("numbers", "number...", "要相乘的数字")
          .setReturnType("number", "乘积结果")
          .addExample("const result = SimpleMath.multiply(2, 3, 4); // 返回 24", "基本用法");
    
    // 方式3：复杂函数
    module->addFunction("power", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要base和exponent两个参数");
        }
        
        double base = ctx.getArg(0).toDouble();
        double exponent = ctx.getArg(1).toDouble();
        
        if (base == 0 && exponent < 0) {
            return helpers::makeError("0的负数次幂未定义");
        }
        
        double result = std::pow(base, exponent);
        
        // 返回详细结果
        JSValue response;
        response["result"] = JSValue(result);
        response["base"] = JSValue(base);
        response["exponent"] = JSValue(exponent);
        response["success"] = JSValue(true);
        
        return response;
    }, "计算幂运算");
    
    // 方式4：异步函数示例（模拟）
    module->addFunction("slowAdd", [](const JSContext& ctx) -> JSValue {
        if (!helpers::validateArgCount(ctx, 2)) {
            return helpers::makeError("需要2个参数");
        }
        
        double a = ctx.getArg(0).toDouble();
        double b = ctx.getArg(1).toDouble();
        
        // 模拟慢速计算
        ctx.logInfo("开始慢速计算: " + std::to_string(a) + " + " + std::to_string(b));
        
        // 在真实场景中，这里可能是异步操作
        // 现在只是模拟
        
        JSValue result;
        result["value"] = JSValue(a + b);
        result["time"] = JSValue("计算耗时: 模拟1秒");
        result["status"] = JSValue("completed");
        
        return result;
    }, "慢速加法（演示异步概念）");

CJMOD_MODULE_END(SimpleMath)