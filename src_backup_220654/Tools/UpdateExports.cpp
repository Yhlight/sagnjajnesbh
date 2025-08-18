#include "../UnifiedCore/Export/ExportGenerator.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::cout << "🔄 CHTL自动导出更新工具" << std::endl;
    std::cout << "正在更新Info文件的[Export]节..." << std::endl;
    
    std::string rootDir = (argc > 1) ? argv[1] : "src/Module";
    
    if (!std::filesystem::exists(rootDir)) {
        std::cerr << "❌ 错误: 目录不存在 " << rootDir << std::endl;
        return 1;
    }
    
    auto exportGen = chtl::export_gen::g_exportGenerator;
    
    try {
        // 遍历所有模块目录
        for (const auto& moduleEntry : std::filesystem::directory_iterator(rootDir)) {
            if (!moduleEntry.is_directory()) continue;
            
            std::string moduleName = moduleEntry.path().filename().string();
            std::cout << "\n📂 处理模块: " << moduleName << std::endl;
            
            // 处理CMOD部分
            std::string cmodDir = moduleEntry.path().string() + "/CMOD";
            if (std::filesystem::exists(cmodDir)) {
                std::cout << "  🔧 处理CMOD组件..." << std::endl;
                
                for (const auto& componentEntry : std::filesystem::directory_iterator(cmodDir)) {
                    if (!componentEntry.is_directory()) continue;
                    
                    std::string componentName = componentEntry.path().filename().string();
                    std::string srcFile = componentEntry.path().string() + "/src/" + componentName + ".chtl";
                    std::string infoFile = componentEntry.path().string() + "/info/" + componentName + ".chtl";
                    
                    if (std::filesystem::exists(srcFile) && std::filesystem::exists(infoFile)) {
                        std::cout << "    📝 更新组件: " << componentName << std::endl;
                        
                        // 清除之前的导出项
                        exportGen->clearExports();
                        
                        // 分析源文件
                        if (exportGen->analyzeSourceFile(srcFile)) {
                            // 更新Info文件
                            if (exportGen->updateInfoFileExports(infoFile)) {
                                std::cout << "      ✅ 成功更新 " << componentName << std::endl;
                            } else {
                                std::cout << "      ❌ 更新失败 " << componentName << std::endl;
                            }
                        }
                    }
                }
            }
            
            // 处理CJMOD部分（CJMOD不需要[Export]节）
            std::string cjmodDir = moduleEntry.path().string() + "/CJMOD";
            if (std::filesystem::exists(cjmodDir)) {
                std::cout << "  🔧 CJMOD组件无需更新导出（由系统自动管理）" << std::endl;
            }
        }
        
        std::cout << "\n🎉 所有模块的导出信息已更新完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 更新过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}