#include <iostream>
#include <csignal>
#include <cstdlib>
#include "Dispatcher/CompilerDispatcher.h"

using namespace chtl;

// 信号处理器
void segfault_handler(int sig) {
    std::cerr << "\n💥 段错误信号捕获！信号: " << sig << std::endl;
    std::cerr << "正在退出..." << std::endl;
    exit(1);
}

int main() {
    // 注册信号处理器
    signal(SIGSEGV, segfault_handler);
    
    std::cout << "🔍 完整编译器段错误调试" << std::endl;
    std::cout << "========================" << std::endl;

    try {
        std::cout << "步骤 1: 创建CompilerDispatcher对象..." << std::endl;
        CompilerDispatcher* dispatcher = new CompilerDispatcher();
        std::cout << "✅ CompilerDispatcher创建成功" << std::endl;
        
        std::cout << "步骤 2: 调用initialize()..." << std::endl;
        bool initResult = dispatcher->initialize();
        if (!initResult) {
            std::cerr << "❌ 初始化失败" << std::endl;
            delete dispatcher;
            return 1;
        }
        std::cout << "✅ CompilerDispatcher初始化成功" << std::endl;
        
        std::cout << "步骤 3: 准备测试代码..." << std::endl;
        std::string testCode = "html { body { text { Hello } } }";
        std::cout << "✅ 测试代码准备完成" << std::endl;
        
        std::cout << "步骤 4: 调用compileCode()..." << std::endl;
        std::cout << "即将调用 dispatcher->compileCode()..." << std::endl;
        
        auto result = dispatcher->compileCode(testCode);
        
        std::cout << "✅ compileCode()调用成功！" << std::endl;
        
        if (result.success) {
            std::cout << "✅ 编译成功！" << std::endl;
            std::cout << "结果长度: " << result.compiledCode.length() << std::endl;
        } else {
            std::cout << "❌ 编译失败" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
        
        delete dispatcher;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ 未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "🎉 调试完成，无段错误！" << std::endl;
    return 0;
}