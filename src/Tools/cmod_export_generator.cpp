#include <iostream>
#include <string>
#include "CMOD/CMODSystem.h"
#include "Utils/ErrorHandler.h"

using namespace CHTL;

void printUsage() {
    std::cout << "CMOD导出生成工具\n";
    std::cout << "用法: cmod_export_generator <模块路径>\n";
    std::cout << "功能: 自动扫描模块源文件并生成Export块\n";
    std::cout << "\n示例:\n";
    std::cout << "  cmod_export_generator ./modules/MyModule\n";
    std::cout << "  cmod_export_generator ../external/ChthollyModule\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage();
        return 1;
    }
    
    std::string modulePath = argv[1];
    
    std::cout << "CMOD导出生成工具\n";
    std::cout << "目标模块: " << modulePath << "\n\n";
    
    try {
        CMOD::CompleteCMODManager manager("", "");
        
        // 生成自动导出信息
        std::cout << "扫描模块源文件...\n";
        CMOD::ModuleExports exports = manager.GenerateAutoExports(modulePath);
        
        // 显示扫描结果
        std::cout << "\n扫描结果:\n";
        std::cout << "  模板样式: " << exports.templateStyles.size() << "个\n";
        for (const auto& style : exports.templateStyles) {
            std::cout << "    - " << style << "\n";
        }
        
        std::cout << "  自定义样式: " << exports.customStyles.size() << "个\n";
        for (const auto& style : exports.customStyles) {
            std::cout << "    - " << style << "\n";
        }
        
        std::cout << "  自定义元素: " << exports.customElements.size() << "个\n";
        for (const auto& element : exports.customElements) {
            std::cout << "    - " << element << "\n";
        }
        
        std::cout << "  自定义变量: " << exports.customVars.size() << "个\n";
        for (const auto& var : exports.customVars) {
            std::cout << "    - " << var << "\n";
        }
        
        std::cout << "  模板元素: " << exports.templateElements.size() << "个\n";
        for (const auto& element : exports.templateElements) {
            std::cout << "    - " << element << "\n";
        }
        
        std::cout << "  模板变量: " << exports.templateVars.size() << "个\n";
        for (const auto& var : exports.templateVars) {
            std::cout << "    - " << var << "\n";
        }
        
        std::cout << "  原始HTML: " << exports.originHtml.size() << "个\n";
        for (const auto& html : exports.originHtml) {
            std::cout << "    - " << html << "\n";
        }
        
        std::cout << "  原始样式: " << exports.originStyle.size() << "个\n";
        for (const auto& style : exports.originStyle) {
            std::cout << "    - " << style << "\n";
        }
        
        std::cout << "  原始JavaScript: " << exports.originJavascript.size() << "个\n";
        for (const auto& js : exports.originJavascript) {
            std::cout << "    - " << js << "\n";
        }
        
        std::cout << "  配置: " << exports.configurations.size() << "个\n";
        for (const auto& config : exports.configurations) {
            std::cout << "    - " << config << "\n";
        }
        
        // 更新模块的导出信息
        std::cout << "\n更新模块导出信息...\n";
        bool updated = manager.UpdateModuleExports(modulePath, exports);
        
        if (updated) {
            std::cout << "✅ 成功更新模块导出信息\n";
            std::cout << "✅ Export块已自动生成到模块信息文件中\n";
        } else {
            std::cout << "❌ 更新模块导出信息失败\n";
            return 1;
        }
        
        std::cout << "\n导出生成完成!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}