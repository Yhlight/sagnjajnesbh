#include "CHTLCompiler.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <functional>

namespace fs = std::filesystem;

namespace chtl {

// ==================== CHTLCompiler ====================

CHTLCompiler::CHTLCompiler() : CHTLCompiler(CompilerConfig{}) {
}

CHTLCompiler::CHTLCompiler(const CompilerConfig& config) : config_(config) {
    initializeSystems();
}

bool CHTLCompiler::initializeSystems() {
    try {
        // 初始化核心组件
        parser_ = std::make_unique<CHTLParser>();
        generator_ = std::make_unique<CHTLGenerator>();
        
        // 初始化导入管理器
        if (config_.enableImportSystem) {
            importManager_ = std::make_unique<ImportManager>(config_.currentDirectory, config_.officialModulePath);
        }
        
        // 初始化命名空间管理器
        if (config_.enableNamespaceSystem) {
            namespaceManager_ = std::make_unique<NamespaceManager>();
        }
        
        // 初始化模块管理器
        if (config_.enableModuleSystem) {
            moduleManager_ = std::make_unique<ModuleManager>();
            moduleManager_->initialize(config_.officialModulePath);
        }
        
        return true;
    } catch (const std::exception& e) {
        lastErrors_.push_back("Failed to initialize compiler systems: " + String(e.what()));
        return false;
    }
}

CompilationResult CHTLCompiler::compile(const String& sourceCode, const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilationResult result;
    clearErrors();
    
    try {
        // 解析源代码
        auto parseResult = parser_->parse(sourceCode, filename);
        
        if (!parseResult.success) {
            result.success = false;
            for (const auto& error : parseResult.errors) {
                result.addError("Parse error at line " + std::to_string(error.line) + 
                               ", column " + std::to_string(error.column) + ": " + error.message);
            }
            for (const auto& warning : parseResult.warnings) {
                result.addWarning(warning);
            }
            return result;
        }
        
        // 处理AST
        result = processAST(parseResult.ast, filename);
        
        // 收集所有错误和警告
        collectErrors();
        
        // 更新统计信息
        updateStatistics(result);
        
    } catch (const std::exception& e) {
        result.addError("Compilation failed: " + String(e.what()));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    statistics_.totalCompileTime += duration.count();
    
    return result;
}

CompilationResult CHTLCompiler::compileFile(const String& inputFile, const String& outputFile) {
    CompilationResult result;
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        result.addError("Cannot open input file: " + inputFile);
        return result;
    }
    
    String sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // 更新当前目录
    String oldCurrentDir = config_.currentDirectory;
    config_.currentDirectory = fs::path(inputFile).parent_path().string();
    if (config_.currentDirectory.empty()) {
        config_.currentDirectory = ".";
    }
    
    // 重新初始化系统以使用新的目录
    if (importManager_) {
        importManager_->setCurrentDirectory(config_.currentDirectory);
    }
    
    // 编译源代码
    result = compile(sourceCode, inputFile);
    
    // 恢复原来的目录
    config_.currentDirectory = oldCurrentDir;
    if (importManager_) {
        importManager_->setCurrentDirectory(config_.currentDirectory);
    }
    
    // 写入输出文件
    if (result.success && !outputFile.empty()) {
        try {
            // 确保输出目录存在
            fs::create_directories(fs::path(outputFile).parent_path());
            
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << result.generatedCode;
                outFile.close();
            } else {
                result.addError("Cannot write to output file: " + outputFile);
            }
        } catch (const std::exception& e) {
            result.addError("Failed to write output file: " + String(e.what()));
        }
    }
    
    statistics_.compiledFiles++;
    return result;
}

CompilationResult CHTLCompiler::compileBatch(const std::vector<String>& inputFiles, const String& outputDirectory) {
    CompilationResult batchResult;
    batchResult.success = true;
    
    for (const auto& inputFile : inputFiles) {
        String outputFile;
        if (!outputDirectory.empty()) {
            fs::path inputPath(inputFile);
            fs::path outputPath = fs::path(outputDirectory) / inputPath.filename();
            outputPath.replace_extension(".html"); // 默认输出为HTML
            outputFile = outputPath.string();
        }
        
        auto result = compileFile(inputFile, outputFile);
        batchResult.mergeFrom(result);
        
        if (!result.success) {
            batchResult.addError("Failed to compile: " + inputFile);
        }
    }
    
    return batchResult;
}

CompilationResult CHTLCompiler::processAST(std::shared_ptr<CHTLASTNode> ast, const String& filename) {
    CompilationResult result;
    
    if (!ast) {
        result.addError("Invalid AST");
        return result;
    }
    
    // 处理导入
    if (config_.enableImportSystem && importManager_) {
        processImports(ast, result, filename);
    }
    
    // 处理命名空间
    if (config_.enableNamespaceSystem && namespaceManager_) {
        processNamespaces(ast, result, filename);
    }
    
    // 生成代码
    if (generator_) {
        try {
            result.generatedCode = generator_->generate(ast);
            result.success = true;
        } catch (const std::exception& e) {
            result.addError("Code generation failed: " + String(e.what()));
        }
    } else {
        result.addError("Generator not initialized");
    }
    
    return result;
}

void CHTLCompiler::processImports(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result, const String& currentFile) {
    if (!importManager_) return;
    
    // 遍历AST查找导入节点
    traverseAST(ast, [&](std::shared_ptr<CHTLASTNode> node) {
        if (node->getType() == ASTNodeType::IMPORT_STATEMENT) {
            auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
            if (importNode) {
                auto importResult = importManager_->processImport(importNode, currentFile);
                
                if (!importResult.success) {
                    result.addError("Import failed: " + importResult.error);
                } else {
                    if (!importResult.resolvedPath.empty()) {
                        result.importedFiles.push_back(importResult.resolvedPath);
                    }
                    
                    // 添加通配符导入的文件
                    for (const auto& path : importResult.resolvedPaths) {
                        result.importedFiles.push_back(path);
                    }
                }
            }
        }
    });
    
    // 收集导入管理器的错误和警告
    auto importErrors = importManager_->getErrors();
    auto importWarnings = importManager_->getWarnings();
    
    for (const auto& error : importErrors) {
        result.addError(error);
    }
    
    for (const auto& warning : importWarnings) {
        result.addWarning(warning);
    }
    
    statistics_.importedFiles += result.importedFiles.size();
}

void CHTLCompiler::processNamespaces(std::shared_ptr<CHTLASTNode> ast, CompilationResult& result, const String& currentFile) {
    if (!namespaceManager_) return;
    
    // 从AST构建命名空间
    if (!namespaceManager_->buildFromAST(ast, currentFile)) {
        auto nsErrors = namespaceManager_->getErrors();
        for (const auto& error : nsErrors) {
            result.addError("Namespace error: " + error);
        }
    }
    
    // 检查命名空间冲突
    auto conflicts = namespaceManager_->detectAllConflicts();
    for (const auto& conflict : conflicts) {
        result.addWarning("Namespace conflict in " + conflict.namespaceName + 
                         ": " + conflict.itemName + " (multiple definitions)");
    }
    
    // 合并同名命名空间
    auto availableNamespaces = namespaceManager_->getAvailableNamespaces();
    for (const auto& nsName : availableNamespaces) {
        if (!namespaceManager_->mergeNamespaces(nsName)) {
            result.addWarning("Failed to merge namespace: " + nsName);
        } else {
            result.usedNamespaces.push_back(nsName);
        }
    }
    
    statistics_.namespacesUsed += result.usedNamespaces.size();
}

void CHTLCompiler::traverseAST(std::shared_ptr<CHTLASTNode> node, std::function<void(std::shared_ptr<CHTLASTNode>)> callback) {
    if (!node) return;
    
    callback(node);
    
    for (const auto& child : node->getChildren()) {
        traverseAST(child, callback);
    }
}

void CHTLCompiler::collectErrors() {
    lastErrors_.clear();
    lastWarnings_.clear();
    
    // 收集导入管理器的错误
    if (importManager_) {
        auto errors = importManager_->getErrors();
        auto warnings = importManager_->getWarnings();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
        lastWarnings_.insert(lastWarnings_.end(), warnings.begin(), warnings.end());
    }
    
    // 收集命名空间管理器的错误
    if (namespaceManager_) {
        auto errors = namespaceManager_->getErrors();
        auto warnings = namespaceManager_->getWarnings();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
        lastWarnings_.insert(lastWarnings_.end(), warnings.begin(), warnings.end());
    }
    
    // 收集模块管理器的错误
    if (moduleManager_) {
        auto errors = moduleManager_->getErrors();
        auto warnings = moduleManager_->getWarnings();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
        lastWarnings_.insert(lastWarnings_.end(), warnings.begin(), warnings.end());
    }
}

void CHTLCompiler::updateStatistics(const CompilationResult& result) {
    statistics_.errorsCount += result.errors.size();
    statistics_.warningsCount += result.warnings.size();
    statistics_.importedFiles += result.importedFiles.size();
    statistics_.loadedModules += result.loadedModules.size();
    statistics_.namespacesUsed += result.usedNamespaces.size();
}

// 配置和系统管理方法

void CHTLCompiler::setConfig(const CompilerConfig& config) {
    config_ = config;
    initializeSystems();
}

void CHTLCompiler::setOfficialModulePath(const String& path) {
    config_.officialModulePath = path;
    
    if (importManager_) {
        importManager_->setOfficialModulePath(path);
    }
    
    if (moduleManager_) {
        moduleManager_->setOfficialModulePath(path);
    }
}

void CHTLCompiler::setCurrentDirectory(const String& directory) {
    config_.currentDirectory = directory;
    
    if (importManager_) {
        importManager_->setCurrentDirectory(directory);
    }
}

// 模块操作

bool CHTLCompiler::installModule(const String& modulePath, const String& installDirectory) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    bool result = moduleManager_->installModule(modulePath, installDirectory);
    collectErrors();
    return result;
}

bool CHTLCompiler::uninstallModule(const String& moduleName) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    bool result = moduleManager_->uninstallModule(moduleName);
    collectErrors();
    return result;
}

std::vector<ModuleInfo> CHTLCompiler::listInstalledModules() {
    if (!moduleManager_) {
        return {};
    }
    
    return moduleManager_->listInstalledModules();
}

// Cmod操作

bool CHTLCompiler::packCmod(const String& sourceDirectory, const String& outputPath) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    // 使用内部的CmodManager
    CmodManager cmodManager;
    bool result = cmodManager.packCmod(sourceDirectory, outputPath);
    
    if (!result) {
        auto errors = cmodManager.getErrors();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
    }
    
    return result;
}

bool CHTLCompiler::unpackCmod(const String& cmodPath, const String& outputDirectory) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    CmodManager cmodManager;
    bool result = cmodManager.unpackCmod(cmodPath, outputDirectory);
    
    if (!result) {
        auto errors = cmodManager.getErrors();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
    }
    
    return result;
}

// CJmod操作

bool CHTLCompiler::packCJmod(const String& sourceDirectory, const String& outputPath) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    CJmodManager cjmodManager;
    bool result = cjmodManager.packCJmod(sourceDirectory, outputPath);
    
    if (!result) {
        auto errors = cjmodManager.getErrors();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
    }
    
    return result;
}

bool CHTLCompiler::unpackCJmod(const String& cjmodPath, const String& outputDirectory) {
    if (!moduleManager_) {
        lastErrors_.push_back("Module system not enabled");
        return false;
    }
    
    CJmodManager cjmodManager;
    bool result = cjmodManager.unpackCJmod(cjmodPath, outputDirectory);
    
    if (!result) {
        auto errors = cjmodManager.getErrors();
        lastErrors_.insert(lastErrors_.end(), errors.begin(), errors.end());
    }
    
    return result;
}

// 命名空间操作

std::vector<String> CHTLCompiler::listAvailableNamespaces() const {
    if (!namespaceManager_) {
        return {};
    }
    
    return namespaceManager_->getAvailableNamespaces();
}

std::vector<String> CHTLCompiler::listNamespaceItems(const String& namespaceName) const {
    if (!namespaceManager_) {
        return {};
    }
    
    return namespaceManager_->getAvailableItems(namespaceName);
}

// 缓存管理

void CHTLCompiler::clearImportCache() {
    if (importManager_) {
        importManager_->clearImportCache();
    }
}

size_t CHTLCompiler::getImportCacheSize() const {
    return importManager_ ? importManager_->getImportCacheSize() : 0;
}

void CHTLCompiler::clearModuleCache() {
    if (moduleManager_) {
        moduleManager_->clearModuleCache();
    }
}

size_t CHTLCompiler::getModuleCacheSize() const {
    return moduleManager_ ? moduleManager_->getModuleCacheSize() : 0;
}

// 辅助方法

String CHTLCompiler::resolveRelativePath(const String& basePath, const String& relativePath) {
    fs::path base(basePath);
    fs::path relative(relativePath);
    fs::path resolved = base / relative;
    return resolved.string();
}

bool CHTLCompiler::isAbsolutePath(const String& path) {
    fs::path fsPath(path);
    return fsPath.is_absolute();
}

String CHTLCompiler::getCurrentWorkingDirectory() {
    try {
        return fs::current_path().string();
    } catch (const std::exception& e) {
        return ".";
    }
}

// ==================== 便利函数 ====================

namespace compiler_utils {

CompilationResult compileFile(const String& inputFile, const String& outputFile, const CompilerConfig& config) {
    CHTLCompiler compiler(config);
    return compiler.compileFile(inputFile, outputFile);
}

CompilationResult compileString(const String& sourceCode, const String& filename, const CompilerConfig& config) {
    CHTLCompiler compiler(config);
    return compiler.compile(sourceCode, filename);
}

std::vector<CompilationResult> compileBatch(const std::vector<String>& inputFiles, 
                                           const String& outputDirectory, 
                                           const CompilerConfig& config) {
    std::vector<CompilationResult> results;
    CHTLCompiler compiler(config);
    
    for (const auto& inputFile : inputFiles) {
        String outputFile;
        if (!outputDirectory.empty()) {
            fs::path inputPath(inputFile);
            fs::path outputPath = fs::path(outputDirectory) / inputPath.filename();
            outputPath.replace_extension(".html");
            outputFile = outputPath.string();
        }
        
        results.push_back(compiler.compileFile(inputFile, outputFile));
    }
    
    return results;
}

bool validateSyntax(const String& sourceCode, std::vector<String>& errors) {
    CompilerConfig config;
    config.enableImportSystem = false;
    config.enableNamespaceSystem = false;
    config.enableModuleSystem = false;
    
    CHTLCompiler compiler(config);
    auto result = compiler.compile(sourceCode, "validation");
    
    errors = result.errors;
    return result.success;
}

String formatCode(const String& sourceCode, const CompilerConfig& config) {
    // 简单的代码格式化实现
    // 这里应该实现更复杂的格式化逻辑
    String formatted = sourceCode;
    
    // 基本的缩进处理
    std::istringstream iss(formatted);
    std::ostringstream oss;
    String line;
    int indentLevel = 0;
    
    while (std::getline(iss, line)) {
        // 去除前后空格
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) {
            oss << "\n";
            continue;
        }
        
        // 处理缩进
        if (line.find('}') == 0) {
            indentLevel--;
        }
        
        for (int i = 0; i < indentLevel; ++i) {
            oss << "    ";
        }
        oss << line << "\n";
        
        if (line.find('{') != String::npos) {
            indentLevel++;
        }
    }
    
    return oss.str();
}

String getCompilerVersion() {
    return "CHTL Compiler v1.0.0";
}

String getCompilerInfo() {
    return "CHTL (Chtholly Template Language) Compiler\n"
           "Version: 1.0.0\n"
           "Features: Import System, Namespace System, Module System (Cmod/CJmod)\n"
           "License: MIT\n";
}

} // namespace compiler_utils

} // namespace chtl