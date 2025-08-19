#include <iostream>
#include <string>
#include "Dispatcher/CompilerDispatcher.h"

using namespace chtl;

int main() {
    std::cout << "🔍 CHTL编译器调试版本" << std::endl;
    std::cout << "======================" << std::endl;

    try {
        std::cout << "1. 创建CompilerDispatcher..." << std::endl;
        CompilerDispatcher dispatcher;
        std::cout << "✅ CompilerDispatcher创建成功" << std::endl;
        
        std::cout << "2. 初始化CompilerDispatcher..." << std::endl;
        if (!dispatcher.initialize()) {
            std::cerr << "❌ 初始化失败" << std::endl;
            return 1;
        }
        std::cout << "✅ CompilerDispatcher初始化成功" << std::endl;
        
        std::cout << "3. 测试简单代码编译..." << std::endl;
        std::string simpleCode = "html { body { text { Hello } } }";
        
        std::cout << "4. 调用compileCode..." << std::endl;
        auto result = dispatcher.compileCode(simpleCode);
        
        std::cout << "5. 检查结果..." << std::endl;
        if (result.success) {
            std::cout << "✅ 编译成功！" << std::endl;
            std::cout << "结果: " << result.compiledCode.substr(0, 100) << "..." << std::endl;
        } else {
            std::cout << "❌ 编译失败" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ 未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "🎉 调试测试完成" << std::endl;
    return 0;
}