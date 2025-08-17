#include "../include/ModuleManager.h"
#include "../include/CHTLParser.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace chtl {

ModuleManager::ModuleManager(const String& currentDirectory, const String& officialModulePath)
    : currentDirectory_(currentDirectory), officialModulePath_(officialModulePath) {
    
    // 初始化核心组件
    importManager_ = std::make_unique<ImportManager>(currentDirectory, officialModulePath);
    namespaceManager_ = std::make_unique<NamespaceManager>();
    
    // 确保官方模块路径存在
    if (!fs::exists(officialModulePath_)) {
        try {
            fs::create_directories(officialModulePath_);
        } catch (const std::exception& e) {
            reportWarning("无法创建官方模块目录: " + String(e.what()));
        }
    }
}

ModuleLoadResult ModuleManager::loadModule(const ImportNode& importNode) {
    ModuleLoadResult result;
    
    try {
        // 使用ImportManager处理导入
        auto importResult = importManager_->importFile(importNode);
        
        result.success = importResult.success;
        result.error = importResult.error;
        result.modulePath = importResult.resolvedPath;
        result.moduleAlias = importNode.getAlias();
        result.ast = importResult.ast;
        result.astNodes = importResult.astNodes;
        
        // 确定模块类型
        result.type = detectModuleType(result.modulePath);
        
        if (result.success) {
            stats_.loadedModules++;
            
            // 如果有命名空间，注册到命名空间管理器
            if (result.ast && result.ast->getType() == ASTNodeType::NAMESPACE_DECLARATION) {
                auto nsNode = std::static_pointer_cast<NamespaceNode>(result.ast);
                auto ns = std::make_shared<Namespace>(nsNode->getName(), result.modulePath);
                registerNamespace(nsNode->getName(), ns);
            }
        }
        
    } catch (const std::exception& e) {
        result.error = "加载模块失败: " + String(e.what());
        result.success = false;
    }
    
    return result;
}

std::vector<ModuleLoadResult> ModuleManager::loadMultipleModules(const std::vector<ImportNode>& importNodes) {
    std::vector<ModuleLoadResult> results;
    
    for (const auto& importNode : importNodes) {
        auto result = loadModule(importNode);
        results.push_back(result);
    }
    
    return results;
}

ModulePackResult ModuleManager::packCmod(const String& sourceDir, const String& outputPath) {
    ModulePackResult result;
    
    try {
        // 生成输出路径
        String finalOutputPath = outputPath;
        if (finalOutputPath.empty()) {
            finalOutputPath = generateOutputPath(sourceDir, ModuleType::CMOD_MODULE);
        }
        
        // 验证结构
        if (!CmodPacker::validateCmodStructure(sourceDir)) {
            result.error = "CMOD结构验证失败";
            return result;
        }
        
        // 打包
        bool packSuccess = CmodPacker::packCmod(sourceDir, finalOutputPath);
        
        if (packSuccess) {
            result.success = true;
            result.outputPath = finalOutputPath;
            
            // 计算文件数量
            auto entries = SimpleZip::createEntriesFromDirectory(sourceDir);
            result.fileCount = entries.size();
            
            std::cout << "✅ CMOD模块打包成功: " << finalOutputPath << std::endl;
        } else {
            result.error = "CMOD打包失败";
        }
        
    } catch (const std::exception& e) {
        result.error = "打包CMOD模块失败: " + String(e.what());
    }
    
    return result;
}

bool ModuleManager::unpackCmod(const String& cmodPath, const String& outputDir) {
    try {
        String finalOutputDir = outputDir;
        if (finalOutputDir.empty()) {
            // 使用CMOD文件名作为输出目录名
            fs::path cmodFile(cmodPath);
            finalOutputDir = currentDirectory_ + "/extracted_" + cmodFile.stem().string();
        }
        
        bool success = CmodPacker::unpackCmod(cmodPath, finalOutputDir);
        
        if (success) {
            std::cout << "✅ CMOD模块解压成功: " << finalOutputDir << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        reportError("解压CMOD模块失败: " + String(e.what()));
        return false;
    }
}

ModulePackResult ModuleManager::packCJmod(const String& sourceDir, const String& outputPath) {
    ModulePackResult result;
    
    try {
        // 生成输出路径
        String finalOutputPath = outputPath;
        if (finalOutputPath.empty()) {
            finalOutputPath = generateOutputPath(sourceDir, ModuleType::CJMOD_MODULE);
        }
        
        // 验证结构
        if (!CJmodPacker::validateCJmodStructure(sourceDir)) {
            result.error = "CJMOD结构验证失败";
            return result;
        }
        
        // 打包
        bool packSuccess = CJmodPacker::packCJmod(sourceDir, finalOutputPath);
        
        if (packSuccess) {
            result.success = true;
            result.outputPath = finalOutputPath;
            
            // 计算文件数量
            auto entries = SimpleZip::createEntriesFromDirectory(sourceDir);
            result.fileCount = entries.size();
            
            std::cout << "✅ CJMOD模块打包成功: " << finalOutputPath << std::endl;
        } else {
            result.error = "CJMOD打包失败";
        }
        
    } catch (const std::exception& e) {
        result.error = "打包CJMOD模块失败: " + String(e.what());
    }
    
    return result;
}

bool ModuleManager::unpackCJmod(const String& cjmodPath, const String& outputDir) {
    try {
        String finalOutputDir = outputDir;
        if (finalOutputDir.empty()) {
            // 使用CJMOD文件名作为输出目录名
            fs::path cjmodFile(cjmodPath);
            finalOutputDir = currentDirectory_ + "/extracted_" + cjmodFile.stem().string();
        }
        
        bool success = CJmodPacker::unpackCJmod(cjmodPath, finalOutputDir);
        
        if (success) {
            std::cout << "✅ CJMOD模块解压成功: " << finalOutputDir << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        reportError("解压CJMOD模块失败: " + String(e.what()));
        return false;
    }
}

bool ModuleManager::registerNamespace(const String& name, std::shared_ptr<Namespace> ns) {
    if (!ns || name.empty()) {
        return false;
    }
    
    // 检查是否已存在同名命名空间
    auto existing = namespaceCache_.find(name);
    if (existing != namespaceCache_.end()) {
        // 尝试合并命名空间
        std::vector<NamespaceConflict> conflicts;
        bool mergeSuccess = existing->second->mergeWith(*ns, conflicts);
        
        if (!mergeSuccess) {
            // 处理冲突
            reportWarning("命名空间 " + name + " 存在冲突，使用冲突解决策略");
            
            for (const auto& conflict : conflicts) {
                // 默认策略：保持现有项目
                existing->second->resolveConflict(conflict, ConflictResolutionStrategy::KEEP_EXISTING);
            }
        }
        
        return true;
    } else {
        namespaceCache_[name] = ns;
        stats_.namespacesRegistered++;
        return true;
    }
}

std::shared_ptr<Namespace> ModuleManager::getNamespace(const String& name) {
    auto it = namespaceCache_.find(name);
    return (it != namespaceCache_.end()) ? it->second : nullptr;
}

bool ModuleManager::isOfficialModule(const String& moduleName) {
    return moduleName.find("chtl::") == 0;
}

String ModuleManager::resolveOfficialModulePath(const String& moduleName) {
    if (isOfficialModule(moduleName)) {
        return officialModulePath_ + "/" + moduleName.substr(6); // 移除 "chtl::" 前缀
    }
    return moduleName;
}

std::vector<String> ModuleManager::discoverModules(const String& searchPath) {
    std::vector<String> modules;
    
    String targetPath = searchPath.empty() ? (currentDirectory_ + "/module") : searchPath;
    
    try {
        if (fs::exists(targetPath) && fs::is_directory(targetPath)) {
            for (const auto& entry : fs::directory_iterator(targetPath)) {
                if (entry.is_regular_file()) {
                    String extension = entry.path().extension().string();
                    if (extension == ".cmod" || extension == ".chtl") {
                        modules.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        reportWarning("搜索模块失败: " + String(e.what()));
    }
    
    return modules;
}

std::vector<String> ModuleManager::discoverCJmodules(const String& searchPath) {
    std::vector<String> modules;
    
    String targetPath = searchPath.empty() ? (currentDirectory_ + "/module") : searchPath;
    
    try {
        if (fs::exists(targetPath) && fs::is_directory(targetPath)) {
            for (const auto& entry : fs::directory_iterator(targetPath)) {
                if (entry.is_regular_file()) {
                    String extension = entry.path().extension().string();
                    if (extension == ".cjmod") {
                        modules.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        reportWarning("搜索CJMOD模块失败: " + String(e.what()));
    }
    
    return modules;
}

ModuleType ModuleManager::detectModuleType(const String& path) {
    if (path.empty()) {
        return ModuleType::CHTL_MODULE;
    }
    
    String extension = fs::path(path).extension().string();
    
    if (extension == ".cmod") {
        return ModuleType::CMOD_MODULE;
    } else if (extension == ".cjmod") {
        return ModuleType::CJMOD_MODULE;
    } else if (extension == ".chtl") {
        return ModuleType::CHTL_MODULE;
    } else if (extension == ".html" || extension == ".htm") {
        return ModuleType::HTML_FILE;
    } else if (extension == ".css") {
        return ModuleType::CSS_FILE;
    } else if (extension == ".js" || extension == ".mjs") {
        return ModuleType::JS_FILE;
    }
    
    return ModuleType::CHTL_MODULE;
}

String ModuleManager::generateOutputPath(const String& sourceDir, ModuleType type) {
    fs::path sourcePath(sourceDir);
    String baseName = sourcePath.filename().string();
    
    String extension;
    switch (type) {
        case ModuleType::CMOD_MODULE:
            extension = ".cmod";
            break;
        case ModuleType::CJMOD_MODULE:
            extension = ".cjmod";
            break;
        default:
            extension = ".cmod";
            break;
    }
    
    return currentDirectory_ + "/" + baseName + extension;
}

ModuleManager::ModuleStats ModuleManager::getStats() const {
    return stats_;
}

void ModuleManager::resetStats() {
    stats_ = ModuleStats{};
}

void ModuleManager::reportError(const String& error) {
    errors_.push_back(error);
    std::cerr << "错误: " << error << std::endl;
}

void ModuleManager::reportWarning(const String& warning) {
    warnings_.push_back(warning);
    std::cout << "警告: " << warning << std::endl;
}

// ==================== ModuleManagerFactory ====================

std::unique_ptr<ModuleManager> ModuleManagerFactory::createManager(const String& currentDirectory, const String& officialModulePath) {
    return std::make_unique<ModuleManager>(currentDirectory, officialModulePath);
}

std::unique_ptr<ModuleManager> ModuleManagerFactory::createWithDefaults() {
    String currentDir = fs::current_path().string();
    String officialPath = currentDir + "/modules";
    return createManager(currentDir, officialPath);
}

} // namespace chtl