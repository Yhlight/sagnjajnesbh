#include "../src/CHTLJSCompiler/AST/VirtualObjectManager.h"
#include <iostream>
#include <cassert>

using namespace chtl::ast;

void testFunctionTypeDetection() {
    std::cout << "=== 测试函数类型检测 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 测试箭头函数检测
    FunctionType arrow_type = manager.detectFunctionType("() => { console.log('test'); }");
    assert(arrow_type == FunctionType::INLINE_ARROW);
    
    // 测试普通函数检测
    FunctionType func_type = manager.detectFunctionType("function() { return true; }");
    assert(arrow_type == FunctionType::INLINE_ARROW);
    
    // 测试函数引用检测
    FunctionType ref_type = manager.detectFunctionType("mouseEnterEvent");
    assert(ref_type == FunctionType::REFERENCE);
    
    // 测试复杂函数引用
    FunctionType complex_ref = manager.detectFunctionType("myModule.handleClick");
    assert(complex_ref == FunctionType::UNKNOWN); // 复杂引用暂时识别为未知
    
    std::cout << "✅ 函数类型检测测试通过!" << std::endl;
}

void testVirtualObjectRegistration() {
    std::cout << "=== 测试虚对象注册 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 注册已存在的函数
    assert(manager.registerExistingFunction("mouseEnterEvent"));
    assert(manager.registerExistingFunction("clickHandler"));
    assert(manager.registerExistingFunction("submitForm"));
    
    // 验证函数存在性检查
    assert(manager.isFunctionExists("mouseEnterEvent"));
    assert(manager.isFunctionExists("clickHandler"));
    assert(!manager.isFunctionExists("nonExistentFunction"));
    
    // 注册虚对象
    assert(manager.registerVirtualObject("buttonVir", "listen"));
    assert(!manager.registerVirtualObject("buttonVir", "listen")); // 重复注册应该失败
    
    std::cout << "✅ 虚对象注册测试通过!" << std::endl;
}

void testFunctionReferenceOptimization() {
    std::cout << "=== 测试函数引用优化 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 注册已存在的函数
    manager.registerExistingFunction("existingClickHandler");
    manager.registerExistingFunction("existingMouseEnter");
    
    // 注册虚对象
    assert(manager.registerVirtualObject("testVir", "listen"));
    
    // 添加内联函数（应该生成新代码）
    assert(manager.addFunctionToVirtualObject("testVir", "click", "() => { console.log('inline click'); }"));
    
    // 添加函数引用（不应该生成新代码）
    assert(manager.addFunctionToVirtualObject("testVir", "mouseenter", "existingMouseEnter"));
    
    // 添加另一个内联函数
    assert(manager.addFunctionToVirtualObject("testVir", "mouseleave", "function() { console.log('leave'); }"));
    
    // 验证函数信息
    auto vir_obj = manager.getVirtualObject("testVir");
    assert(vir_obj != nullptr);
    assert(vir_obj->functions.size() == 3);
    
    // 验证函数类型
    assert(vir_obj->functions["click"].type == FunctionType::INLINE_ARROW);
    assert(vir_obj->functions["mouseenter"].type == FunctionType::REFERENCE);
    assert(vir_obj->functions["mouseleave"].type == FunctionType::INLINE_FUNCTION);
    
    // 验证引用函数不会生成新代码
    assert(!manager.shouldGenerateFunction(vir_obj->functions["mouseenter"]));
    assert(manager.shouldGenerateFunction(vir_obj->functions["click"]));
    assert(manager.shouldGenerateFunction(vir_obj->functions["mouseleave"]));
    
    std::cout << "✅ 函数引用优化测试通过!" << std::endl;
}

void testVirtualObjectCodeGeneration() {
    std::cout << "=== 测试虚对象代码生成 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 注册已存在的函数
    manager.registerExistingFunction("handleExistingEvent");
    
    // 注册虚对象
    assert(manager.registerVirtualObject("myVir", "listen"));
    
    // 添加混合类型的函数
    assert(manager.addFunctionToVirtualObject("myVir", "newFunction", "() => { alert('new'); }"));
    assert(manager.addFunctionToVirtualObject("myVir", "existingFunction", "handleExistingEvent"));
    
    // 生成代码
    std::string generated_code = manager.generateVirtualObjectCode("myVir");
    
    std::cout << "生成的虚对象代码:" << std::endl;
    std::cout << generated_code << std::endl;
    
    // 验证生成的代码
    assert(!generated_code.empty());
    
    // 应该包含新函数的生成代码
    assert(generated_code.find("function") != std::string::npos);
    
    // 应该包含对已存在函数的引用，但不重复生成函数体
    assert(generated_code.find("handleExistingEvent") != std::string::npos);
    
    // 不应该包含已存在函数的重复定义
    assert(generated_code.find("function handleExistingEvent") == std::string::npos);
    
    std::cout << "✅ 虚对象代码生成测试通过!" << std::endl;
}

void testVirtualObjectCallResolution() {
    std::cout << "=== 测试虚对象调用解析 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 设置测试环境
    manager.registerExistingFunction("existingHandler");
    assert(manager.registerVirtualObject("testVir", "listen"));
    assert(manager.addFunctionToVirtualObject("testVir", "click", "() => { console.log('click'); }"));
    assert(manager.addFunctionToVirtualObject("testVir", "hover", "existingHandler"));
    
    // 测试虚对象调用解析
    auto call1 = manager.parseVirtualCall("testVir->click()");
    assert(call1.is_valid);
    assert(call1.vir_name == "testVir");
    assert(call1.function_key == "click");
    assert(!call1.resolved_function_name.empty());
    
    auto call2 = manager.parseVirtualCall("testVir->hover()");
    assert(call2.is_valid);
    assert(call2.vir_name == "testVir");
    assert(call2.function_key == "hover");
    assert(call2.resolved_function_name == "existingHandler"); // 应该直接引用
    
    std::cout << "✅ 虚对象调用解析测试通过!" << std::endl;
}

void testComplexScenario() {
    std::cout << "=== 测试复杂场景 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 模拟真实场景：已存在多个事件处理函数
    manager.registerExistingFunction("handleSubmit");
    manager.registerExistingFunction("validateForm");
    manager.registerExistingFunction("showTooltip");
    manager.registerExistingFunction("hideTooltip");
    
    // 创建复杂的虚对象
    assert(manager.registerVirtualObject("formVir", "listen"));
    
    // 混合使用内联函数和函数引用
    assert(manager.addFunctionToVirtualObject("formVir", "submit", "handleSubmit"));           // 引用
    assert(manager.addFunctionToVirtualObject("formVir", "validate", "validateForm"));         // 引用
    assert(manager.addFunctionToVirtualObject("formVir", "mouseenter", "showTooltip"));        // 引用
    assert(manager.addFunctionToVirtualObject("formVir", "mouseleave", "hideTooltip"));        // 引用
    assert(manager.addFunctionToVirtualObject("formVir", "focus", "() => { this.classList.add('focused'); }")); // 内联
    assert(manager.addFunctionToVirtualObject("formVir", "blur", "function() { this.classList.remove('focused'); }")); // 内联
    
    // 生成代码
    std::string code = manager.generateVirtualObjectCode("formVir");
    
    std::cout << "复杂场景生成的代码:" << std::endl;
    std::cout << code << std::endl;
    
    // 验证优化效果
    // 1. 引用函数不应该重复生成函数体
    assert(code.find("function handleSubmit") == std::string::npos);
    assert(code.find("function validateForm") == std::string::npos);
    assert(code.find("function showTooltip") == std::string::npos);
    assert(code.find("function hideTooltip") == std::string::npos);
    
    // 2. 应该包含对已存在函数的引用
    assert(code.find("handleSubmit") != std::string::npos);
    assert(code.find("validateForm") != std::string::npos);
    
    // 3. 内联函数应该生成新的函数体
    assert(code.find("function") != std::string::npos); // 应该有新生成的函数
    
    // 获取统计信息
    std::cout << "\n" << manager.getStatistics() << std::endl;
    
    std::cout << "✅ 复杂场景测试通过!" << std::endl;
}

void testCHTLJSSyntaxExamples() {
    std::cout << "=== 测试正确的CHTL JS语法示例 ===" << std::endl;
    
    VirtualObjectManager manager;
    
    // 注册一些已存在的函数（模拟真实环境）
    manager.registerExistingFunction("mouseEnterEvent");
    manager.registerExistingFunction("handleFormSubmit");
    manager.registerExistingFunction("validateInput");
    
    // 测试正确的CHTL JS虚对象语法
    assert(manager.registerVirtualObject("test", "listen"));
    
    // 正确的语法：混合内联函数和函数引用
    assert(manager.addFunctionToVirtualObject("test", "click", "() => { console.log('clicked'); }"));
    assert(manager.addFunctionToVirtualObject("test", "mouseenter", "mouseEnterEvent")); // 函数引用
    assert(manager.addFunctionToVirtualObject("test", "mousemove", "function() { console.log('moving'); }"));
    
    // 生成代码并验证
    std::string code = manager.generateVirtualObjectCode("test");
    
    std::cout << "正确CHTL JS语法生成的代码:" << std::endl;
    std::cout << code << std::endl;
    
    // 验证：
    // 1. 内联函数应该生成新代码
    assert(code.find("console.log('clicked')") != std::string::npos || 
           code.find("function") != std::string::npos);
    
    // 2. 函数引用应该直接引用，不重复生成
    assert(code.find("mouseEnterEvent") != std::string::npos);
    assert(code.find("function mouseEnterEvent") == std::string::npos); // 不应该重复定义
    
    std::cout << "✅ 正确的CHTL JS语法示例测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试虚对象函数引用优化..." << std::endl << std::endl;
        
        testFunctionTypeDetection();
        std::cout << std::endl;
        
        testVirtualObjectRegistration();
        std::cout << std::endl;
        
        testFunctionReferenceOptimization();
        std::cout << std::endl;
        
        testVirtualObjectCodeGeneration();
        std::cout << std::endl;
        
        testVirtualObjectCallResolution();
        std::cout << std::endl;
        
        testComplexScenario();
        std::cout << std::endl;
        
        testCHTLJSSyntaxExamples();
        std::cout << std::endl;
        
        std::cout << "🎉 所有虚对象优化功能测试通过!" << std::endl;
        std::cout << "\n📝 优化效果总结:" << std::endl;
        std::cout << "✅ 函数引用不会重复生成代码体" << std::endl;
        std::cout << "✅ 内联函数会生成新的全局函数" << std::endl;
        std::cout << "✅ 虚对象调用正确解析到对应函数" << std::endl;
        std::cout << "✅ 避免了不必要的代码重复" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}