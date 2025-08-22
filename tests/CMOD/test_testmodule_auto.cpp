#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include "CMOD/CMODManager.h"
#include "Utils/FileUtils.h"

using namespace CHTL;

int main() {
    std::cout << "测试TestModule自动导出功能...\n";
    
    try {
        CMOD::CMODManager manager;
        
        // 加载TestModule（应该自动生成Export块）
        bool loaded = manager.LoadModule("modules/TestModule");
        assert(loaded);
        
        // 验证模块加载
        const CMOD::CMODModule* module = manager.FindModule("TestModule");
        assert(module != nullptr);
        
        std::cout << "模块信息:\n";
        std::cout << "  名称: " << module->info.name << "\n";
        std::cout << "  版本: " << module->info.version << "\n";
        std::cout << "  描述: " << module->info.description << "\n";
        
        std::cout << "\n自动生成的导出信息:\n";
        std::cout << "  模板样式: " << module->exports.templateStyles.size() << "个\n";
        for (const auto& style : module->exports.templateStyles) {
            std::cout << "    - " << style << "\n";
        }
        
        std::cout << "  自定义样式: " << module->exports.customStyles.size() << "个\n";
        for (const auto& style : module->exports.customStyles) {
            std::cout << "    - " << style << "\n";
        }
        
        std::cout << "  自定义元素: " << module->exports.customElements.size() << "个\n";
        for (const auto& element : module->exports.customElements) {
            std::cout << "    - " << element << "\n";
        }
        
        std::cout << "  自定义变量: " << module->exports.customVars.size() << "个\n";
        for (const auto& var : module->exports.customVars) {
            std::cout << "    - " << var << "\n";
        }
        
        std::cout << "  模板元素: " << module->exports.templateElements.size() << "个\n";
        for (const auto& element : module->exports.templateElements) {
            std::cout << "    - " << element << "\n";
        }
        
        // 验证自动生成的Export块
        std::string infoFilePath = "modules/TestModule/info/TestModule.chtl";
        std::string infoContent = Utils::FileUtils::ReadFile(infoFilePath);
        
        if (infoContent.find("[Export]") != std::string::npos) {
            std::cout << "\n✅ Export块已自动生成到信息文件中\n";
            
            // 显示生成的Export块内容
            size_t exportPos = infoContent.find("[Export]");
            size_t exportEnd = infoContent.find("}", exportPos);
            if (exportEnd != std::string::npos) {
                std::string exportBlock = infoContent.substr(exportPos, exportEnd - exportPos + 1);
                std::cout << "\n生成的Export块:\n" << exportBlock << "\n";
            }
        }
        
        std::cout << "\nTestModule自动导出功能测试完成!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}