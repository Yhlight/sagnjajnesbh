#include <iostream>
#include "CHTLCompiler/Template/TemplateManager.h"
#include "CHTLCompiler/Custom/CustomManager.h"
#include "CHTLCompiler/Origin/OriginManager.h"
#include "CHTLCompiler/Constraint/ConstraintManager.h"

using namespace chtl;

int main() {
    std::cout << "🔍 测试全局Manager初始化" << std::endl;
    std::cout << "========================" << std::endl;

    try {
        std::cout << "1. 检查g_templateManager..." << std::endl;
        if (template_system::g_templateManager) {
            std::cout << "✅ g_templateManager正常" << std::endl;
        } else {
            std::cout << "❌ g_templateManager为空" << std::endl;
        }
        
        std::cout << "2. 检查g_customManager..." << std::endl;
        if (custom_system::g_customManager) {
            std::cout << "✅ g_customManager正常" << std::endl;
        } else {
            std::cout << "❌ g_customManager为空" << std::endl;
        }
        
        std::cout << "3. 检查g_originManager..." << std::endl;
        if (origin_system::g_originManager) {
            std::cout << "✅ g_originManager正常" << std::endl;
        } else {
            std::cout << "❌ g_originManager为空" << std::endl;
        }
        
        std::cout << "4. 检查g_constraintManager..." << std::endl;
        if (constraint_system::g_constraintManager) {
            std::cout << "✅ g_constraintManager正常" << std::endl;
        } else {
            std::cout << "❌ g_constraintManager为空" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 Manager检查完成" << std::endl;
    return 0;
}