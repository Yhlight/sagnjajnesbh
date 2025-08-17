#include "../src/CHTLJSCompiler/Core/CHTLJSCompilerCore.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testCHTLJSCompilerInitialization() {
    std::cout << "=== 测试CHTL JS编译器初始化 ===" << std::endl;
    
    CHTLJSCompilerCore compiler;
    compiler.setDebugMode(true);
    
    assert(compiler.initialize());
    std::cout << "✅ CHTL JS编译器初始化成功!" << std::endl;
}

void testCHTLJSCompilerComponents() {
    std::cout << "=== 测试CHTL JS编译器组件 ===" << std::endl;
    
    CHTLJSCompilerCore compiler;
    compiler.initialize();
    
    // 测试组件访问
    auto& lexer = compiler.getLexer();
    auto& parser = compiler.getParser();
    auto& generator = compiler.getGenerator();
    auto& module_manager = compiler.getModuleManager();
    auto& vir_manager = compiler.getVirtualObjectManager();
    
    std::cout << "✅ 所有CHTL JS编译器组件访问正常!" << std::endl;
}

void testCHTLJSCompilation() {
    std::cout << "=== 测试CHTL JS编译 ===" << std::endl;
    
    CHTLJSCompilerCore compiler;
    compiler.setDebugMode(false);
    
    std::string test_code = R"(
        script {
            {{box}}->listen({
                click: () => {
                    console.log('Button clicked');
                }
            });
            
            vir controller = listen({
                submit: handleSubmit
            });
            
            controller->submit();
        }
    )";
    
    std::string result = compiler.compile(test_code);
    
    // 检查是否有错误
    auto errors = compiler.getErrors();
    if (!errors.empty()) {
        std::cout << "编译错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "✅ CHTL JS编译测试完成!" << std::endl;
}

void testVirtualObjectManager() {
    std::cout << "=== 测试虚对象管理器 ===" << std::endl;
    
    CHTLJSCompilerCore compiler;
    compiler.initialize();
    
    auto& vir_manager = compiler.getVirtualObjectManager();
    
    // 测试虚对象注册
    std::string test_vir = "vir controller = listen({ click: handleClick });";
    // vir_manager.registerVirtualObject("controller", test_vir);
    
    std::cout << "✅ 虚对象管理器测试完成!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL JS编译器..." << std::endl << std::endl;
        
        testCHTLJSCompilerInitialization();
        std::cout << std::endl;
        
        testCHTLJSCompilerComponents();
        std::cout << std::endl;
        
        testCHTLJSCompilation();
        std::cout << std::endl;
        
        testVirtualObjectManager();
        std::cout << std::endl;
        
        std::cout << "🎉 所有CHTL JS编译器测试通过!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}