#include <iostream>
#include "CHTLCompiler/Template/TemplateManager.h"
#include "CHTLCompiler/Custom/CustomManager.h"
#include "CHTLCompiler/Origin/OriginManager.h"
#include "CHTLCompiler/Constraint/ConstraintManager.h"
#include "CHTLCompiler/Parser/CHTLContext.h"
#include "CHTLCompiler/State/CHTLState.h"
#include "CHTLCompiler/Template/TemplateParser.h"

using namespace chtl;

int main() {
    std::cout << "🔧 测试安全Parser初始化" << std::endl;
    std::cout << "=======================" << std::endl;

    try {
        // 1. 确保所有全局Manager已初始化
        std::cout << "1. 检查全局Manager..." << std::endl;
        if (!template_system::g_templateManager) {
            std::cout << "❌ g_templateManager未初始化" << std::endl;
            return 1;
        }
        if (!custom_system::g_customManager) {
            std::cout << "❌ g_customManager未初始化" << std::endl;
            return 1;
        }
        if (!origin_system::g_originManager) {
            std::cout << "❌ g_originManager未初始化" << std::endl;
            return 1;
        }
        if (!constraint_system::g_constraintManager) {
            std::cout << "❌ g_constraintManager未初始化" << std::endl;
            return 1;
        }
        std::cout << "✅ 所有全局Manager已初始化" << std::endl;
        
        // 2. 创建上下文
        std::cout << "2. 创建CHTLContext和StateManager..." << std::endl;
        auto context = std::make_unique<CHTLContext>();
        auto stateManager = std::make_unique<StateManager>();
        std::cout << "✅ 上下文创建成功" << std::endl;
        
        // 3. 逐个测试Parser创建
        std::cout << "3. 测试TemplateParser创建..." << std::endl;
        auto templateParser = std::make_unique<template_system::TemplateParser>(*context, *stateManager);
        std::cout << "✅ TemplateParser创建成功" << std::endl;
        
        // 4. 测试所有Parser一起创建（模拟CHTLParser构造函数）
        std::cout << "4. 测试所有Parser一起创建..." << std::endl;
        auto context2 = std::make_unique<CHTLContext>();
        auto stateManager2 = std::make_unique<StateManager>();
        
        auto templateParser2 = std::make_unique<template_system::TemplateParser>(*context2, *stateManager2);
        auto customParser2 = std::make_unique<custom_system::CustomParser>(*context2, *stateManager2);
        auto originParser2 = std::make_unique<origin_system::OriginParser>(*context2, *stateManager2);
        auto constraintParser2 = std::make_unique<constraint_system::ConstraintParser>(*context2, *stateManager2);
        
        std::cout << "✅ 所有Parser创建成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 安全Parser测试完成" << std::endl;
    return 0;
}