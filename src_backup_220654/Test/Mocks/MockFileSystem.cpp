#include "MockFileSystem.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace chtl {
namespace test {
namespace mocks {

// === MockFileSystem 实现 ===

MockFileSystem::MockFileSystem() : readCount_(0), writeCount_(0) {
    addPredefinedStructure();
}

bool MockFileSystem::exists(const std::string& path) const {
    std::string normalizedPath = normalizePath(path);
    return files_.find(normalizedPath) != files_.end();
}

bool MockFileSystem::createFile(const std::string& path, const std::string& content) {
    if (!isValidPath(path)) return false;
    
    std::string normalizedPath = normalizePath(path);
    files_[normalizedPath] = FileInfo(content, false);
    return true;
}

bool MockFileSystem::createDirectory(const std::string& path) {
    if (!isValidPath(path)) return false;
    
    std::string normalizedPath = normalizePath(path);
    files_[normalizedPath] = FileInfo("", true);
    return true;
}

std::string MockFileSystem::readFile(const std::string& path) const {
    readCount_++;
    std::string normalizedPath = normalizePath(path);
    auto it = files_.find(normalizedPath);
    
    if (it != files_.end() && !it->second.isDirectory) {
        return it->second.content;
    }
    
    return "";
}

bool MockFileSystem::writeFile(const std::string& path, const std::string& content) {
    writeCount_++;
    return createFile(path, content);
}

std::vector<std::string> MockFileSystem::listDirectory(const std::string& path) const {
    std::vector<std::string> result;
    std::string normalizedPath = normalizePath(path);
    
    if (!normalizedPath.empty() && normalizedPath.back() != '/') {
        normalizedPath += '/';
    }
    
    for (const auto& [filePath, fileInfo] : files_) {
        if (filePath.find(normalizedPath) == 0) {
            std::string relativePath = filePath.substr(normalizedPath.length());
            if (relativePath.find('/') == std::string::npos) {
                result.push_back(relativePath);
            }
        }
    }
    
    return result;
}

void MockFileSystem::clear() {
    files_.clear();
    resetCounters();
}

void MockFileSystem::addPredefinedStructure() {
    // 添加预定义的文件结构
    createDirectory("src");
    createDirectory("src/Module");
    createDirectory("src/Module/Chtholly");
    createDirectory("src/Module/Yuigahama");
    
    // 珂朵莉模块
    createFile("src/Module/Chtholly/Accordion.chtl", "[Info]\nName: Chtholly Accordion\n");
    createFile("src/Module/Chtholly/Gallery.chtl", "[Info]\nName: Chtholly Gallery\n");
    
    // 由比滨结衣模块
    createFile("src/Module/Yuigahama/MusicPlayer.chtl", "[Info]\nName: Yui Music Player\n");
    createFile("src/Module/Yuigahama/Accordion.chtl", "[Info]\nName: Yui Accordion\n");
}

std::string MockFileSystem::normalizePath(const std::string& path) const {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除重复的斜杠
    std::regex doubleSlash("/+");
    normalized = std::regex_replace(normalized, doubleSlash, "/");
    
    // 移除开头的斜杠
    if (!normalized.empty() && normalized[0] == '/') {
        normalized = normalized.substr(1);
    }
    
    return normalized;
}

bool MockFileSystem::isValidPath(const std::string& path) const {
    return !path.empty() && path.find('\0') == std::string::npos;
}

void MockFileSystem::resetCounters() {
    readCount_ = 0;
    writeCount_ = 0;
}

// === MockCompiler 实现 ===

MockCompiler::MockCompiler() 
    : strictMode_(false), optimizationLevel_(1), compilationCount_(0), validationCount_(0) {
}

bool MockCompiler::compileCSS(const std::string& css) {
    compilationCount_++;
    return simulateCompilation(css);
}

bool MockCompiler::compileJavaScript(const std::string& js) {
    compilationCount_++;
    return simulateCompilation(js);
}

bool MockCompiler::compileCHTL(const std::string& chtl) {
    compilationCount_++;
    return simulateCompilation(chtl);
}

std::vector<std::string> MockCompiler::validateCSS(const std::string& css) {
    validationCount_++;
    return simulateValidation(css);
}

std::vector<std::string> MockCompiler::validateJavaScript(const std::string& js) {
    validationCount_++;
    return simulateValidation(js);
}

std::vector<std::string> MockCompiler::validateCHTL(const std::string& chtl) {
    validationCount_++;
    return simulateValidation(chtl);
}

bool MockCompiler::simulateCompilation(const std::string& code) {
    // 模拟编译过程
    if (code.empty()) return false;
    if (strictMode_ && code.find("error") != std::string::npos) return false;
    return true;
}

std::vector<std::string> MockCompiler::simulateValidation(const std::string& code) {
    std::vector<std::string> errors;
    
    // 模拟验证过程
    if (code.find("syntax_error") != std::string::npos) {
        errors.push_back("语法错误: 发现syntax_error标记");
    }
    
    if (code.find("undefined_reference") != std::string::npos) {
        errors.push_back("未定义引用: 发现undefined_reference标记");
    }
    
    return errors;
}

void MockCompiler::resetCounters() {
    compilationCount_ = 0;
    validationCount_ = 0;
}

// === MockModuleManager 实现 ===

MockModuleManager::MockModuleManager() {
    // 添加预定义模块
    addMockModule("Chtholly.Accordion", "珂朵莉手风琴组件");
    addMockModule("Chtholly.Gallery", "珂朵莉相册组件");
    addMockModule("Yuigahama.MusicPlayer", "由比滨结衣音乐播放器");
    addMockModule("Yuigahama.Accordion", "由比滨结衣手风琴组件");
}

bool MockModuleManager::loadModule(const std::string& moduleName) {
    if (availableModules_.find(moduleName) != availableModules_.end()) {
        if (std::find(loadedModules_.begin(), loadedModules_.end(), moduleName) == loadedModules_.end()) {
            loadedModules_.push_back(moduleName);
        }
        return true;
    }
    return false;
}

bool MockModuleManager::unloadModule(const std::string& moduleName) {
    auto it = std::find(loadedModules_.begin(), loadedModules_.end(), moduleName);
    if (it != loadedModules_.end()) {
        loadedModules_.erase(it);
        return true;
    }
    return false;
}

bool MockModuleManager::isModuleLoaded(const std::string& moduleName) const {
    return std::find(loadedModules_.begin(), loadedModules_.end(), moduleName) != loadedModules_.end();
}

std::vector<std::string> MockModuleManager::getLoadedModules() const {
    return loadedModules_;
}

std::vector<std::string> MockModuleManager::getAvailableModules() const {
    std::vector<std::string> modules;
    for (const auto& [name, _] : availableModules_) {
        modules.push_back(name);
    }
    return modules;
}

std::string MockModuleManager::getModuleInfo(const std::string& moduleName) const {
    auto it = availableModules_.find(moduleName);
    return (it != availableModules_.end()) ? it->second : "";
}

std::vector<std::string> MockModuleManager::searchModules(const std::string& query) const {
    std::vector<std::string> results;
    
    for (const auto& [name, info] : availableModules_) {
        if (name.find(query) != std::string::npos || info.find(query) != std::string::npos) {
            results.push_back(name);
        }
    }
    
    return results;
}

void MockModuleManager::addMockModule(const std::string& name, const std::string& info) {
    availableModules_[name] = info;
}

void MockModuleManager::removeMockModule(const std::string& name) {
    availableModules_.erase(name);
    unloadModule(name);
}

void MockModuleManager::clear() {
    availableModules_.clear();
    loadedModules_.clear();
    moduleDependencies_.clear();
}

} // namespace mocks
} // namespace test
} // namespace chtl