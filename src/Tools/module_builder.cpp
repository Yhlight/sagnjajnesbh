// 模块构建工具
// 按照您的设计：自动构建src/Module中的模块，输出到与exe同级的module文件夹

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "CMOD/CMODSystem.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"

namespace fs = std::filesystem;
using namespace CHTL;

struct ModuleInfo {
    std::string name;
    std::string type;  // "CMOD", "CJMOD", "MIXED", "SIMPLE"
    fs::path sourcePath;
    fs::path outputPath;
    bool hasInfoFile;
    bool hasCJMOD;
    std::vector<fs::path> sourceFiles;
    std::vector<fs::path> cjmodFiles;
};

class ModuleBuilder {
private:
    fs::path srcModuleDir_;
    fs::path outputModuleDir_;
    std::vector<ModuleInfo> modules_;

public:
    ModuleBuilder(const fs::path& srcDir, const fs::path& outputDir) 
        : srcModuleDir_(srcDir), outputModuleDir_(outputDir) {}

    // 扫描并分析所有模块
    bool scanModules() {
        std::cout << "扫描模块目录: " << srcModuleDir_ << std::endl;
        
        if (!fs::exists(srcModuleDir_)) {
            std::cout << "模块源目录不存在: " << srcModuleDir_ << std::endl;
            return false;
        }

        for (const auto& entry : fs::directory_iterator(srcModuleDir_)) {
            if (entry.is_directory()) {
                ModuleInfo module = analyzeModule(entry.path());
                if (!module.name.empty()) {
                    modules_.push_back(module);
                    std::cout << "发现模块: " << module.name << " (类型: " << module.type << ")" << std::endl;
                }
            }
        }

        std::cout << "总共发现 " << modules_.size() << " 个模块" << std::endl;
        return true;
    }

    // 分析单个模块
    ModuleInfo analyzeModule(const fs::path& modulePath) {
        ModuleInfo module;
        module.name = modulePath.filename().string();
        module.sourcePath = modulePath;
        module.outputPath = outputModuleDir_ / module.name;

        // 检查模块结构
        fs::path infoDir = modulePath / "info";
        fs::path srcDir = modulePath / "src";
        fs::path cjmodDir = modulePath / "CJMOD";

        module.hasInfoFile = fs::exists(infoDir);
        module.hasCJMOD = fs::exists(cjmodDir);

        if (module.hasInfoFile) {
            // 标准CMOD结构
            module.type = module.hasCJMOD ? "MIXED" : "CMOD";
            
            // 查找源文件
            if (fs::exists(srcDir)) {
                for (const auto& file : fs::recursive_directory_iterator(srcDir)) {
                    if (file.is_regular_file() && file.path().extension() == ".chtl") {
                        module.sourceFiles.push_back(file.path());
                    }
                }
            }

            // 查找CJMOD文件
            if (module.hasCJMOD) {
                for (const auto& file : fs::recursive_directory_iterator(cjmodDir)) {
                    if (file.is_regular_file() && 
                        (file.path().extension() == ".cpp" || file.path().extension() == ".h")) {
                        module.cjmodFiles.push_back(file.path());
                    }
                }
            }
        } else {
            // 简单模块结构（无序）
            module.type = "SIMPLE";
            for (const auto& file : fs::directory_iterator(modulePath)) {
                if (file.is_regular_file() && 
                    (file.path().extension() == ".cmod" || file.path().extension() == ".chtl")) {
                    module.sourceFiles.push_back(file.path());
                }
            }
        }

        return module;
    }

    // 构建所有模块
    bool buildAllModules() {
        std::cout << "\n开始构建所有模块..." << std::endl;
        
        bool success = true;
        for (const auto& module : modules_) {
            std::cout << "\n构建模块: " << module.name << std::endl;
            if (!buildModule(module)) {
                std::cerr << "模块构建失败: " << module.name << std::endl;
                success = false;
            } else {
                std::cout << "模块构建成功: " << module.name << std::endl;
            }
        }

        return success;
    }

    // 构建单个模块
    bool buildModule(const ModuleInfo& module) {
        try {
            // 创建输出目录
            fs::create_directories(module.outputPath);

            if (module.type == "CMOD" || module.type == "MIXED") {
                return buildCMODModule(module);
            } else if (module.type == "SIMPLE") {
                return buildSimpleModule(module);
            }

            return false;
        } catch (const std::exception& e) {
            std::cerr << "构建模块时出错 " << module.name << ": " << e.what() << std::endl;
            return false;
        }
    }

private:
    // 构建CMOD模块
    bool buildCMODModule(const ModuleInfo& module) {
        // 1. 复制info文件
        fs::path infoSrcDir = module.sourcePath / "info";
        fs::path infoDestDir = module.outputPath / "info";
        
        if (fs::exists(infoSrcDir)) {
            fs::create_directories(infoDestDir);
            for (const auto& file : fs::directory_iterator(infoSrcDir)) {
                if (file.is_regular_file()) {
                    fs::copy_file(file.path(), infoDestDir / file.path().filename(), 
                                fs::copy_options::overwrite_existing);
                    std::cout << "  复制info文件: " << file.path().filename() << std::endl;
                }
            }
        }

        // 2. 复制源文件
        if (!module.sourceFiles.empty()) {
            fs::path srcDestDir = module.outputPath / "src";
            fs::create_directories(srcDestDir);
            
            for (const auto& srcFile : module.sourceFiles) {
                fs::path relativePath = fs::relative(srcFile, module.sourcePath / "src");
                fs::path destFile = srcDestDir / relativePath;
                
                fs::create_directories(destFile.parent_path());
                fs::copy_file(srcFile, destFile, fs::copy_options::overwrite_existing);
                std::cout << "  复制源文件: " << relativePath << std::endl;
            }
        }

        // 3. 处理CJMOD扩展（如果是混合模块）
        if (module.type == "MIXED" && !module.cjmodFiles.empty()) {
            return buildCJMODExtension(module);
        }

        return true;
    }

    // 构建简单模块
    bool buildSimpleModule(const ModuleInfo& module) {
        for (const auto& srcFile : module.sourceFiles) {
            fs::path destFile = module.outputPath / srcFile.filename();
            fs::copy_file(srcFile, destFile, fs::copy_options::overwrite_existing);
            std::cout << "  复制文件: " << srcFile.filename() << std::endl;
        }
        return true;
    }

    // 构建CJMOD扩展
    bool buildCJMODExtension(const ModuleInfo& module) {
        std::cout << "  构建CJMOD扩展..." << std::endl;

        // 复制CJMOD源文件
        fs::path cjmodDestDir = module.outputPath / "CJMOD";
        fs::create_directories(cjmodDestDir);

        for (const auto& cjmodFile : module.cjmodFiles) {
            fs::path relativePath = fs::relative(cjmodFile, module.sourcePath / "CJMOD");
            fs::path destFile = cjmodDestDir / relativePath;
            
            fs::create_directories(destFile.parent_path());
            fs::copy_file(cjmodFile, destFile, fs::copy_options::overwrite_existing);
            std::cout << "    复制CJMOD文件: " << relativePath << std::endl;
        }

        // 生成编译脚本（用于后续动态库编译）
        generateCJMODBuildScript(module);

        return true;
    }

    // 生成CJMOD编译脚本
    void generateCJMODBuildScript(const ModuleInfo& module) {
        fs::path scriptPath = module.outputPath / "build_cjmod.sh";
        std::ofstream script(scriptPath);

        script << "#!/bin/bash\n";
        script << "# 自动生成的CJMOD编译脚本\n";
        script << "# 模块: " << module.name << "\n\n";

        script << "echo \"编译 " << module.name << " CJMOD扩展...\"\n\n";

        // 查找所有cpp文件
        script << "CPP_FILES=\"";
        for (const auto& cjmodFile : module.cjmodFiles) {
            if (cjmodFile.extension() == ".cpp") {
                fs::path relativePath = fs::relative(cjmodFile, module.sourcePath);
                script << relativePath.string() << " ";
            }
        }
        script << "\"\n\n";

        // 编译命令
        script << "g++ -shared -fPIC -std=c++17 \\\n";
        script << "    -I../../include \\\n";
        script << "    -I../../include/CJMOD \\\n";
        script << "    $CPP_FILES \\\n";
        script << "    -o lib/" << module.name << "_cjmod.so\n\n";

        script << "if [ $? -eq 0 ]; then\n";
        script << "    echo \"CJMOD扩展编译成功: " << module.name << "_cjmod.so\"\n";
        script << "else\n";
        script << "    echo \"CJMOD扩展编译失败\"\n";
        script << "    exit 1\n";
        script << "fi\n";

        script.close();

        // 设置执行权限
        fs::permissions(scriptPath, fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                       fs::perm_options::add);

        std::cout << "    生成编译脚本: build_cjmod.sh" << std::endl;
    }

public:
    // 生成模块清单
    void generateModuleManifest() {
        fs::path manifestPath = outputModuleDir_ / "module_manifest.json";
        std::ofstream manifest(manifestPath);

        manifest << "{\n";
        manifest << "  \"buildTime\": \"" << getCurrentTimestamp() << "\",\n";
        manifest << "  \"totalModules\": " << modules_.size() << ",\n";
        manifest << "  \"modules\": [\n";

        for (size_t i = 0; i < modules_.size(); i++) {
            const auto& module = modules_[i];
            manifest << "    {\n";
            manifest << "      \"name\": \"" << module.name << "\",\n";
            manifest << "      \"type\": \"" << module.type << "\",\n";
            manifest << "      \"hasInfoFile\": " << (module.hasInfoFile ? "true" : "false") << ",\n";
            manifest << "      \"hasCJMOD\": " << (module.hasCJMOD ? "true" : "false") << ",\n";
            manifest << "      \"sourceFiles\": " << module.sourceFiles.size() << ",\n";
            manifest << "      \"cjmodFiles\": " << module.cjmodFiles.size() << "\n";
            manifest << "    }";
            if (i < modules_.size() - 1) manifest << ",";
            manifest << "\n";
        }

        manifest << "  ]\n";
        manifest << "}\n";

        manifest.close();
        std::cout << "\n生成模块清单: module_manifest.json" << std::endl;
    }

private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "CHTL 模块构建工具 v1.0" << std::endl;
    std::cout << "按照您的设计：自动构建src/Module中的模块\n" << std::endl;

    // 解析命令行参数
    fs::path srcDir = "src/Module";
    fs::path outputDir = "module";

    if (argc >= 2) {
        srcDir = argv[1];
    }
    if (argc >= 3) {
        outputDir = argv[2];
    }

    std::cout << "源模块目录: " << fs::absolute(srcDir) << std::endl;
    std::cout << "输出目录: " << fs::absolute(outputDir) << std::endl;

    // 创建模块构建器
    ModuleBuilder builder(srcDir, outputDir);

    // 扫描模块
    if (!builder.scanModules()) {
        std::cerr << "模块扫描失败" << std::endl;
        return 1;
    }

    // 构建所有模块
    if (!builder.buildAllModules()) {
        std::cerr << "模块构建过程中有错误发生" << std::endl;
        return 1;
    }

    // 生成模块清单
    builder.generateModuleManifest();

    std::cout << "\n=== 模块构建完成 ===" << std::endl;
    std::cout << "所有模块已成功构建到: " << fs::absolute(outputDir) << std::endl;
    std::cout << "珂朵莉模块系统已就绪 - 世界上最幸福的女孩 ❀" << std::endl;

    return 0;
}