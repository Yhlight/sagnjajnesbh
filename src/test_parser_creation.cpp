#include <iostream>
#include "CHTLCompiler/Parser/CHTLContext.h"
#include "CHTLCompiler/State/CHTLState.h"
#include "CHTLCompiler/Custom/CustomParser.h"
#include "CHTLCompiler/Origin/OriginParser.h"
#include "CHTLCompiler/Constraint/ConstraintParser.h"
#include "CHTLCompiler/Template/TemplateParser.h"

using namespace chtl;

int main() {
    std::cout << "🔍 测试Parser创建" << std::endl;
    std::cout << "=================" << std::endl;

    try {
        std::cout << "1. 创建CHTLContext..." << std::endl;
        CHTLContext context;
        std::cout << "✅ CHTLContext创建成功" << std::endl;
        
        std::cout << "2. 创建StateManager..." << std::endl;
        StateManager stateManager;
        std::cout << "✅ StateManager创建成功" << std::endl;
        
        std::cout << "3. 创建TemplateParser..." << std::endl;
        auto templateParser = std::make_unique<template_system::TemplateParser>(context, stateManager);
        std::cout << "✅ TemplateParser创建成功" << std::endl;
        
        std::cout << "4. 创建CustomParser..." << std::endl;
        auto customParser = std::make_unique<custom_system::CustomParser>(context, stateManager);
        std::cout << "✅ CustomParser创建成功" << std::endl;
        
        std::cout << "5. 创建OriginParser..." << std::endl;
        auto originParser = std::make_unique<origin_system::OriginParser>(context, stateManager);
        std::cout << "✅ OriginParser创建成功" << std::endl;
        
        std::cout << "6. 创建ConstraintParser..." << std::endl;
        auto constraintParser = std::make_unique<constraint_system::ConstraintParser>(context, stateManager);
        std::cout << "✅ ConstraintParser创建成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 所有Parser创建测试完成" << std::endl;
    return 0;
}