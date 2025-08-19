// ========================================
// 模块打包器 - 集成到编译器
// 严格按照CHTL语法文档
// ========================================

#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "../../Common/SimpleZip/SimpleZip.h"

namespace chtl {
namespace module {

/**
 * 模块打包器 - 集成到CHTL编译器
 */
class ModulePackager {
public:
    /**
     * 打包CMOD模块
     */
    static bool packCmod(const std::string& modulePath, const std::string& outputPath = "");
    
    /**
     * 打包CJMOD模块
     */
    static bool packCjmod(const std::string& modulePath, const std::string& outputPath = "");
    
    /**
     * 解包模块
     */
    static bool unpackModule(const std::string& moduleFile, const std::string& outputDir = "");
    
    /**
     * 验证模块结构
     */
    static bool validateModuleStructure(const std::string& modulePath, const std::string& type);
    
    /**
     * 获取模块信息
     */
    static std::string getModuleInfo(const std::string& modulePath);
    
    /**
     * 列出模块中的所有文件
     */
    static std::vector<std::string> listModuleFiles(const std::string& moduleFile);

private:
    static bool validateCmodStructure(const std::string& modulePath);
    static bool validateCjmodStructure(const std::string& modulePath);
    static void addDirectoryToZip(SimpleZip& zip, const std::filesystem::path& dirPath, const std::string& zipPath);
};

/**
 * 模块安装器 - 安装模块到项目
 */
class ModuleInstaller {
public:
    /**
     * 安装模块到项目
     */
    static bool installModule(const std::string& moduleFile, const std::string& projectPath = ".");
    
    /**
     * 卸载模块
     */
    static bool uninstallModule(const std::string& moduleName, const std::string& projectPath = ".");
    
    /**
     * 列出已安装的模块
     */
    static std::vector<std::string> listInstalledModules(const std::string& projectPath = ".");
    
    /**
     * 检查模块依赖
     */
    static bool checkDependencies(const std::string& modulePath);

private:
    static std::string getProjectModuleDir(const std::string& projectPath);
    static bool createModuleDirectory(const std::string& moduleDir);
};

} // namespace module
} // namespace chtl