#include "../src/CHTLCompiler/Core/CHTLCompilerCore.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testCHTLCompilerBasic() {
    std::cout << "=== 测试CHTL编译器基础功能 ===" << std::endl;
    
    CHTLCompilerCore compiler;
    compiler.setDebugMode(true);
    
    // 只测试初始化，不进行复杂编译
    assert(compiler.initialize());
    std::cout << "✅ CHTL编译器初始化成功!" << std::endl;
    
    // 测试错误处理
    compiler.clearErrors();
    assert(compiler.getErrors().empty());
    std::cout << "✅ 错误处理功能正常!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL编译器（简化版本）..." << std::endl << std::endl;
        
        testCHTLCompilerBasic();
        std::cout << std::endl;
        
        std::cout << "🎉 CHTL编译器基础功能测试通过!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
