// ========================================
// CMOD打包工具 - 严格按照CHTL语法文档
// 将符合格式的文件夹打包成.cmod文件
// ========================================

#include <iostream>
#include <string>
#include <filesystem>
#include "../src/Common/SimpleZip/SimpleZip.h"

namespace fs = std::filesystem;

/**
 * CMOD打包器
 */
class CmodPacker {
public:
    /**
     * 验证CMOD结构
     */
    static bool validateCmodStructure(const std::string& modulePath) {
        std::cout << "🔍 验证CMOD结构: " << modulePath << std::endl;
        
        fs::path path(modulePath);
        std::string moduleName = path.filename().string();
        
        // 检查必需的目录结构
        fs::path srcPath = path / "src";
        fs::path infoPath = path / "info";
        
        if (!fs::exists(srcPath) || !fs::exists(infoPath)) {
            std::cout << "❌ 缺少src或info目录" << std::endl;
            return false;
        }
        
        // 检查三同名规则
        fs::path mainChtlFile = srcPath / (moduleName + ".chtl");
        fs::path infoChtlFile = infoPath / (moduleName + ".chtl");
        
        if (!fs::exists(infoChtlFile)) {
            std::cout << "❌ 缺少info中的" << moduleName << ".chtl文件" << std::endl;
            return false;
        }
        
        // 检查是否有子模块
        bool hasSubmodules = false;
        for (const auto& entry : fs::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                hasSubmodules = true;
                break;
            }
        }
        
        if (!hasSubmodules && !fs::exists(mainChtlFile)) {
            std::cout << "❌ 无子模块时必须存在主模块文件: " << mainChtlFile << std::endl;
            return false;
        }
        
        std::cout << "✅ CMOD结构验证通过" << std::endl;
        return true;
    }
    
    /**
     * 打包CMOD
     */
    static bool packCmod(const std::string& modulePath, const std::string& outputPath = "") {
        if (!validateCmodStructure(modulePath)) {
            return false;
        }
        
        fs::path path(modulePath);
        std::string moduleName = path.filename().string();
        std::string outputFile = outputPath.empty() ? 
            (moduleName + ".cmod") : outputPath;
        
        std::cout << "📦 开始打包CMOD: " << moduleName << std::endl;
        
        try {
            chtl::SimpleZip zip;
            
            // 添加src目录
            addDirectoryToZip(zip, path / "src", "src");
            
            // 添加info目录
            addDirectoryToZip(zip, path / "info", "info");
            
            // 保存ZIP文件
            if (zip.save(outputFile)) {
                std::cout << "✅ CMOD打包成功: " << outputFile << std::endl;
                return true;
            } else {
                std::cout << "❌ CMOD打包失败" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "❌ 打包过程出错: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    static void addDirectoryToZip(chtl::SimpleZip& zip, const fs::path& dirPath, const std::string& zipPath) {
        if (!fs::exists(dirPath)) return;
        
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fs::path relativePath = fs::relative(entry.path(), dirPath);
                std::string zipFilePath = zipPath + "/" + relativePath.string();
                
                zip.addFile(zipFilePath, entry.path().string());
                std::cout << "📄 添加文件: " << zipFilePath << std::endl;
            }
        }
    }
};

/**
 * CJMOD打包器
 */
class CjmodPacker {
public:
    /**
     * 验证CJMOD结构
     */
    static bool validateCjmodStructure(const std::string& modulePath) {
        std::cout << "🔍 验证CJMOD结构: " << modulePath << std::endl;
        
        fs::path path(modulePath);
        std::string moduleName = path.filename().string();
        
        // 检查必需的目录结构
        fs::path srcPath = path / "src";
        fs::path infoPath = path / "info";
        
        if (!fs::exists(srcPath) || !fs::exists(infoPath)) {
            std::cout << "❌ 缺少src或info目录" << std::endl;
            return false;
        }
        
        // 检查二同名规则
        fs::path infoChtlFile = infoPath / (moduleName + ".chtl");
        
        if (!fs::exists(infoChtlFile)) {
            std::cout << "❌ 缺少info中的" << moduleName << ".chtl文件" << std::endl;
            return false;
        }
        
        // 检查是否有C++源文件
        bool hasCppFiles = false;
        for (const auto& entry : fs::recursive_directory_iterator(srcPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".cpp" || ext == ".h") {
                    hasCppFiles = true;
                    break;
                }
            }
        }
        
        if (!hasCppFiles) {
            std::cout << "❌ src目录中没有找到C++源文件" << std::endl;
            return false;
        }
        
        std::cout << "✅ CJMOD结构验证通过" << std::endl;
        return true;
    }
    
    /**
     * 打包CJMOD
     */
    static bool packCjmod(const std::string& modulePath, const std::string& outputPath = "") {
        if (!validateCjmodStructure(modulePath)) {
            return false;
        }
        
        fs::path path(modulePath);
        std::string moduleName = path.filename().string();
        std::string outputFile = outputPath.empty() ? 
            (moduleName + ".cjmod") : outputPath;
        
        std::cout << "📦 开始打包CJMOD: " << moduleName << std::endl;
        
        try {
            chtl::SimpleZip zip;
            
            // 添加src目录
            addDirectoryToZip(zip, path / "src", "src");
            
            // 添加info目录
            addDirectoryToZip(zip, path / "info", "info");
            
            // 保存ZIP文件
            if (zip.save(outputFile)) {
                std::cout << "✅ CJMOD打包成功: " << outputFile << std::endl;
                return true;
            } else {
                std::cout << "❌ CJMOD打包失败" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "❌ 打包过程出错: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    static void addDirectoryToZip(chtl::SimpleZip& zip, const fs::path& dirPath, const std::string& zipPath) {
        if (!fs::exists(dirPath)) return;
        
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                fs::path relativePath = fs::relative(entry.path(), dirPath);
                std::string zipFilePath = zipPath + "/" + relativePath.string();
                
                zip.addFile(zipFilePath, entry.path().string());
                std::cout << "📄 添加文件: " << zipFilePath << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "📦 CHTL模块打包工具" << std::endl;
    std::cout << "==================" << std::endl;
    
    if (argc < 3) {
        std::cout << "用法: " << argv[0] << " <类型> <模块路径> [输出文件]" << std::endl;
        std::cout << "类型: cmod 或 cjmod" << std::endl;
        std::cout << "示例: " << argv[0] << " cmod ./Chtholly ./Chtholly.cmod" << std::endl;
        return 1;
    }
    
    std::string type = argv[1];
    std::string modulePath = argv[2];
    std::string outputPath = argc > 3 ? argv[3] : "";
    
    bool success = false;
    
    if (type == "cmod") {
        success = CmodPacker::packCmod(modulePath, outputPath);
    } else if (type == "cjmod") {
        success = CjmodPacker::packCjmod(modulePath, outputPath);
    } else {
        std::cout << "❌ 不支持的类型: " << type << std::endl;
        std::cout << "支持的类型: cmod, cjmod" << std::endl;
        return 1;
    }
    
    return success ? 0 : 1;
}