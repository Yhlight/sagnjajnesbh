// ========================================
// CMOD/CJMOD解包工具 - 严格按照CHTL语法文档
// 解包.cmod/.cjmod文件到指定目录
// ========================================

#include <iostream>
#include <string>
#include <filesystem>
#include "../src/Common/SimpleZip/SimpleZip.h"

namespace fs = std::filesystem;

/**
 * 模块解包器
 */
class ModuleUnpacker {
public:
    /**
     * 解包模块文件
     */
    static bool unpackModule(const std::string& moduleFile, const std::string& outputDir = "") {
        fs::path filePath(moduleFile);
        
        if (!fs::exists(filePath)) {
            std::cout << "❌ 模块文件不存在: " << moduleFile << std::endl;
            return false;
        }
        
        std::string extension = filePath.extension().string();
        if (extension != ".cmod" && extension != ".cjmod") {
            std::cout << "❌ 不支持的文件类型: " << extension << std::endl;
            std::cout << "支持的类型: .cmod, .cjmod" << std::endl;
            return false;
        }
        
        std::string moduleName = filePath.stem().string();
        std::string outputPath = outputDir.empty() ? 
            ("./modules/" + moduleName) : outputDir;
        
        std::cout << "📦 开始解包模块: " << moduleName << std::endl;
        std::cout << "📁 输出目录: " << outputPath << std::endl;
        
        try {
            // 创建输出目录
            fs::create_directories(outputPath);
            
            chtl::SimpleZip zip;
            if (!zip.load(moduleFile)) {
                std::cout << "❌ 无法加载模块文件" << std::endl;
                return false;
            }
            
            // 解包所有文件
            auto files = zip.getFileList();
            for (const auto& file : files) {
                fs::path outputFilePath = fs::path(outputPath) / file;
                
                // 确保目录存在
                fs::create_directories(outputFilePath.parent_path());
                
                // 提取文件
                auto content = zip.getFile(file);
                if (content.empty()) {
                    std::cout << "⚠️ 文件为空: " << file << std::endl;
                    continue;
                }
                
                // 写入文件
                std::ofstream outFile(outputFilePath, std::ios::binary);
                outFile.write(content.data(), content.size());
                outFile.close();
                
                std::cout << "📄 解包文件: " << file << std::endl;
            }
            
            std::cout << "✅ 模块解包成功: " << outputPath << std::endl;
            
            // 验证解包后的结构
            if (extension == ".cmod") {
                validateUnpackedCmod(outputPath);
            } else if (extension == ".cjmod") {
                validateUnpackedCjmod(outputPath);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "❌ 解包过程出错: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    static void validateUnpackedCmod(const std::string& path) {
        std::cout << "🔍 验证解包的CMOD结构..." << std::endl;
        
        fs::path srcPath = fs::path(path) / "src";
        fs::path infoPath = fs::path(path) / "info";
        
        if (fs::exists(srcPath) && fs::exists(infoPath)) {
            std::cout << "✅ CMOD结构完整" << std::endl;
        } else {
            std::cout << "⚠️ CMOD结构可能不完整" << std::endl;
        }
    }
    
    static void validateUnpackedCjmod(const std::string& path) {
        std::cout << "🔍 验证解包的CJMOD结构..." << std::endl;
        
        fs::path srcPath = fs::path(path) / "src";
        fs::path infoPath = fs::path(path) / "info";
        
        if (fs::exists(srcPath) && fs::exists(infoPath)) {
            std::cout << "✅ CJMOD结构完整" << std::endl;
        } else {
            std::cout << "⚠️ CJMOD结构可能不完整" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "📦 CHTL模块解包工具" << std::endl;
    std::cout << "==================" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <模块文件> [输出目录]" << std::endl;
        std::cout << "示例: " << argv[0] << " Chtholly.cmod ./modules/" << std::endl;
        return 1;
    }
    
    std::string moduleFile = argv[1];
    std::string outputDir = argc > 2 ? argv[2] : "";
    
    bool success = ModuleUnpacker::unpackModule(moduleFile, outputDir);
    
    return success ? 0 : 1;
}