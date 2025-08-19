// ========================================
// 模块打包器实现 - 严格按照CHTL语法文档
// ========================================

#include "ModulePackager.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace chtl {
namespace module {

bool ModulePackager::packCmod(const std::string& modulePath, const std::string& outputPath) {
    if (!validateCmodStructure(modulePath)) {
        return false;
    }
    
    std::filesystem::path path(modulePath);
    std::string moduleName = path.filename().string();
    std::string outputFile = outputPath.empty() ? (moduleName + ".cmod") : outputPath;
    
    std::cout << "📦 打包CMOD: " << moduleName << std::endl;
    
    try {
        SimpleZip zip;
        
        // 添加src和info目录
        addDirectoryToZip(zip, path / "src", "src");
        addDirectoryToZip(zip, path / "info", "info");
        
        if (zip.save(outputFile)) {
            std::cout << "✅ CMOD打包成功: " << outputFile << std::endl;
            return true;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 打包失败: " << e.what() << std::endl;
    }
    
    return false;
}

bool ModulePackager::packCjmod(const std::string& modulePath, const std::string& outputPath) {
    if (!validateCjmodStructure(modulePath)) {
        return false;
    }
    
    std::filesystem::path path(modulePath);
    std::string moduleName = path.filename().string();
    std::string outputFile = outputPath.empty() ? (moduleName + ".cjmod") : outputPath;
    
    std::cout << "📦 打包CJMOD: " << moduleName << std::endl;
    
    try {
        SimpleZip zip;
        
        // 添加src和info目录
        addDirectoryToZip(zip, path / "src", "src");
        addDirectoryToZip(zip, path / "info", "info");
        
        if (zip.save(outputFile)) {
            std::cout << "✅ CJMOD打包成功: " << outputFile << std::endl;
            return true;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 打包失败: " << e.what() << std::endl;
    }
    
    return false;
}

bool ModulePackager::unpackModule(const std::string& moduleFile, const std::string& outputDir) {
    std::filesystem::path filePath(moduleFile);
    
    if (!std::filesystem::exists(filePath)) {
        std::cout << "❌ 模块文件不存在: " << moduleFile << std::endl;
        return false;
    }
    
    std::string moduleName = filePath.stem().string();
    std::string outputPath = outputDir.empty() ? ("./modules/" + moduleName) : outputDir;
    
    std::cout << "📦 解包模块: " << moduleName << std::endl;
    
    try {
        std::filesystem::create_directories(outputPath);
        
        SimpleZip zip;
        if (!zip.load(moduleFile)) {
            std::cout << "❌ 无法加载模块文件" << std::endl;
            return false;
        }
        
        auto files = zip.getFileList();
        for (const auto& file : files) {
            std::filesystem::path outputFilePath = std::filesystem::path(outputPath) / file;
            std::filesystem::create_directories(outputFilePath.parent_path());
            
            auto content = zip.getFile(file);
            std::ofstream outFile(outputFilePath, std::ios::binary);
            outFile.write(content.data(), content.size());
            outFile.close();
            
            std::cout << "📄 解包: " << file << std::endl;
        }
        
        std::cout << "✅ 模块解包成功: " << outputPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 解包失败: " << e.what() << std::endl;
        return false;
    }
}

bool ModulePackager::validateModuleStructure(const std::string& modulePath, const std::string& type) {
    if (type == "cmod") {
        return validateCmodStructure(modulePath);
    } else if (type == "cjmod") {
        return validateCjmodStructure(modulePath);
    }
    
    std::cout << "❌ 不支持的模块类型: " << type << std::endl;
    return false;
}

std::string ModulePackager::getModuleInfo(const std::string& modulePath) {
    std::filesystem::path path(modulePath);
    std::string moduleName = path.filename().string();
    
    // 查找info文件
    std::filesystem::path infoFile = path / "info" / (moduleName + ".chtl");
    
    if (!std::filesystem::exists(infoFile)) {
        return "";
    }
    
    std::ifstream file(infoFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

std::vector<std::string> ModulePackager::listModuleFiles(const std::string& moduleFile) {
    std::vector<std::string> files;
    
    try {
        SimpleZip zip;
        if (zip.load(moduleFile)) {
            files = zip.getFileList();
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 列出文件失败: " << e.what() << std::endl;
    }
    
    return files;
}

bool ModulePackager::validateCmodStructure(const std::string& modulePath) {
    std::filesystem::path path(modulePath);
    std::string moduleName = path.filename().string();
    
    // 检查基本结构
    std::filesystem::path srcPath = path / "src";
    std::filesystem::path infoPath = path / "info";
    
    if (!std::filesystem::exists(srcPath) || !std::filesystem::exists(infoPath)) {
        std::cout << "❌ CMOD缺少src或info目录" << std::endl;
        return false;
    }
    
    // 检查三同名规则
    std::filesystem::path infoChtlFile = infoPath / (moduleName + ".chtl");
    if (!std::filesystem::exists(infoChtlFile)) {
        std::cout << "❌ CMOD缺少info中的" << moduleName << ".chtl文件" << std::endl;
        return false;
    }
    
    std::cout << "✅ CMOD结构验证通过" << std::endl;
    return true;
}

bool ModulePackager::validateCjmodStructure(const std::string& modulePath) {
    std::filesystem::path path(modulePath);
    std::string moduleName = path.filename().string();
    
    // 检查基本结构
    std::filesystem::path srcPath = path / "src";
    std::filesystem::path infoPath = path / "info";
    
    if (!std::filesystem::exists(srcPath) || !std::filesystem::exists(infoPath)) {
        std::cout << "❌ CJMOD缺少src或info目录" << std::endl;
        return false;
    }
    
    // 检查二同名规则
    std::filesystem::path infoChtlFile = infoPath / (moduleName + ".chtl");
    if (!std::filesystem::exists(infoChtlFile)) {
        std::cout << "❌ CJMOD缺少info中的" << moduleName << ".chtl文件" << std::endl;
        return false;
    }
    
    // 检查C++源文件
    bool hasCppFiles = false;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".cpp" || ext == ".h") {
                hasCppFiles = true;
                break;
            }
        }
    }
    
    if (!hasCppFiles) {
        std::cout << "❌ CJMOD src目录中没有C++源文件" << std::endl;
        return false;
    }
    
    std::cout << "✅ CJMOD结构验证通过" << std::endl;
    return true;
}

void ModulePackager::addDirectoryToZip(SimpleZip& zip, const std::filesystem::path& dirPath, const std::string& zipPath) {
    if (!std::filesystem::exists(dirPath)) return;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::path relativePath = std::filesystem::relative(entry.path(), dirPath);
            std::string zipFilePath = zipPath + "/" + relativePath.string();
            
            zip.addFile(zipFilePath, entry.path().string());
        }
    }
}

// ModuleInstaller实现
bool ModuleInstaller::installModule(const std::string& moduleFile, const std::string& projectPath) {
    std::cout << "🔧 安装模块: " << moduleFile << std::endl;
    
    std::string moduleDir = getProjectModuleDir(projectPath);
    if (!createModuleDirectory(moduleDir)) {
        return false;
    }
    
    // 解包到项目模块目录
    return ModulePackager::unpackModule(moduleFile, moduleDir);
}

bool ModuleInstaller::uninstallModule(const std::string& moduleName, const std::string& projectPath) {
    std::cout << "🗑️ 卸载模块: " << moduleName << std::endl;
    
    std::string moduleDir = getProjectModuleDir(projectPath);
    std::filesystem::path modulePath = std::filesystem::path(moduleDir) / moduleName;
    
    if (std::filesystem::exists(modulePath)) {
        std::filesystem::remove_all(modulePath);
        std::cout << "✅ 模块卸载成功" << std::endl;
        return true;
    } else {
        std::cout << "❌ 模块不存在" << std::endl;
        return false;
    }
}

std::vector<std::string> ModuleInstaller::listInstalledModules(const std::string& projectPath) {
    std::vector<std::string> modules;
    std::string moduleDir = getProjectModuleDir(projectPath);
    
    if (std::filesystem::exists(moduleDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(moduleDir)) {
            if (entry.is_directory()) {
                modules.push_back(entry.path().filename().string());
            }
        }
    }
    
    return modules;
}

bool ModuleInstaller::checkDependencies(const std::string& modulePath) {
    // 简单的依赖检查实现
    std::string info = ModulePackager::getModuleInfo(modulePath);
    
    // 检查dependencies字段
    if (info.find("dependencies = \"\"") != std::string::npos || 
        info.find("dependencies = ''") != std::string::npos) {
        return true; // 无依赖
    }
    
    std::cout << "⚠️ 模块可能有依赖需要检查" << std::endl;
    return true; // 暂时总是返回true
}

std::string ModuleInstaller::getProjectModuleDir(const std::string& projectPath) {
    return projectPath + "/module";
}

bool ModuleInstaller::createModuleDirectory(const std::string& moduleDir) {
    try {
        std::filesystem::create_directories(moduleDir);
        return true;
    } catch (const std::exception& e) {
        std::cout << "❌ 创建模块目录失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace module
} // namespace chtl