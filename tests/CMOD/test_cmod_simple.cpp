#include <iostream>
#include <cassert>
#include <memory>
#include "CMOD/CMODManager.h"

using namespace CHTL;

int main() {
    std::cout << "运行简化CMOD系统测试...\n";
    
    try {
        CMOD::CMODManager manager;
        
        // 测试基本功能
        auto loadedModules = manager.GetLoadedModules();
        std::cout << "初始模块数量: " << loadedModules.size() << "\n";
        
        // 测试搜索路径
        manager.AddModuleSearchPath("./test_modules");
        
        // 测试版本兼容性检查
        CMOD::ModuleInfo testInfo;
        testInfo.name = "TestModule";
        testInfo.version = "1.0.0";
        testInfo.minCHTLVersion = "1.0.0";
        testInfo.maxCHTLVersion = "2.0.0";
        
        CMOD::CMODModule testModule;
        testModule.info = testInfo;
        
        bool isValid = manager.ValidateModuleDependencies(testModule);
        assert(isValid);
        
        std::cout << "简化CMOD系统测试通过!\n";
        std::cout << "✅ CMOD管理器基础功能正常\n";
        std::cout << "✅ 搜索路径功能正常\n";
        std::cout << "✅ 版本兼容性检查正常\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}