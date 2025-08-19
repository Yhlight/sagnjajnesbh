#include <iostream>
#include "CHTLCompiler/Parser/CHTLContext.h"
#include "CHTLCompiler/State/CHTLState.h"
#include "CHTLCompiler/Template/TemplateParser.h"

using namespace chtl;

int main() {
    std::cout << "🔍 逐步测试Parser创建" << std::endl;
    std::cout << "=====================" << std::endl;

    try {
        std::cout << "1. 创建CHTLContext..." << std::endl;
        auto context = std::make_unique<CHTLContext>();
        std::cout << "✅ CHTLContext创建成功" << std::endl;
        
        std::cout << "2. 创建StateManager..." << std::endl;
        auto stateManager = std::make_unique<StateManager>();
        std::cout << "✅ StateManager创建成功" << std::endl;
        
        std::cout << "3. 创建TemplateParser..." << std::endl;
        auto templateParser = std::make_unique<template_system::TemplateParser>(*context, *stateManager);
        std::cout << "✅ TemplateParser创建成功" << std::endl;
        
        std::cout << "4. 测试CHTLParser构造..." << std::endl;
        // 这里不创建CHTLParser，因为它会创建所有Parser
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 逐步测试完成" << std::endl;
    return 0;
}