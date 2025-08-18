#include "TestUtils.h"
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

namespace chtl {
namespace test {
namespace utils {

// === TestDataGenerator 实现 ===

TestDataGenerator& TestDataGenerator::getInstance() {
    static TestDataGenerator instance;
    return instance;
}

TestDataGenerator::TestDataGenerator() : rng_(std::random_device{}()) {
    componentNames_ = {"Button", "Card", "Modal", "Form", "List", "Table", "Header", "Footer"};
    selectorTypes_ = {".", "#"};
    functionNames_ = {"click", "hover", "focus", "change", "input", "submit"};
}

std::string TestDataGenerator::generateRandomString(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::uniform_int_distribution<> dis(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(rng_)];
    }
    
    return result;
}

std::string TestDataGenerator::generateValidIdentifier() {
    const std::string firstChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    const std::string otherChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    
    std::uniform_int_distribution<> firstDis(0, firstChars.size() - 1);
    std::uniform_int_distribution<> otherDis(0, otherChars.size() - 1);
    std::uniform_int_distribution<> lengthDis(3, 15);
    
    std::string result;
    size_t length = lengthDis(rng_);
    
    result += firstChars[firstDis(rng_)];
    for (size_t i = 1; i < length; ++i) {
        result += otherChars[otherDis(rng_)];
    }
    
    return result;
}

std::string TestDataGenerator::generateInvalidIdentifier() {
    std::vector<std::string> invalidIds = {
        "123invalid",  // 数字开头
        "invalid-name", // 包含连字符
        "invalid.name", // 包含点号
        "invalid name", // 包含空格
        "",            // 空字符串
        "class",       // 保留关键字
        "function"     // 保留关键字
    };
    
    std::uniform_int_distribution<> dis(0, invalidIds.size() - 1);
    return invalidIds[dis(rng_)];
}

int TestDataGenerator::generateRandomInt(int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(rng_);
}

double TestDataGenerator::generateRandomDouble(double min, double max) {
    std::uniform_real_distribution<> dis(min, max);
    return dis(rng_);
}

std::string TestDataGenerator::generateCHTLComponent() {
    std::uniform_int_distribution<> dis(0, componentNames_.size() - 1);
    return componentNames_[dis(rng_)];
}

std::string TestDataGenerator::generateCSSSelector() {
    std::uniform_int_distribution<> typeDis(0, selectorTypes_.size() - 1);
    std::string type = selectorTypes_[typeDis(rng_)];
    std::string name = generateValidIdentifier();
    return type + name;
}

std::string TestDataGenerator::generateCHTLJSFunction() {
    std::uniform_int_distribution<> dis(0, functionNames_.size() - 1);
    return functionNames_[dis(rng_)];
}

std::vector<std::string> TestDataGenerator::generateNamespaceHierarchy() {
    std::vector<std::string> hierarchy;
    int depth = generateRandomInt(1, 4);
    
    for (int i = 0; i < depth; ++i) {
        hierarchy.push_back(generateValidIdentifier());
    }
    
    return hierarchy;
}

std::string TestDataGenerator::generateModulePath() {
    std::vector<std::string> modules = {"Chtholly", "Yuigahama", "Core", "UIKit"};
    std::vector<std::string> components = {"Accordion", "Gallery", "MusicPlayer", "Memo"};
    
    std::uniform_int_distribution<> moduleDis(0, modules.size() - 1);
    std::uniform_int_distribution<> compDis(0, components.size() - 1);
    
    return modules[moduleDis(rng_)] + "." + components[compDis(rng_)];
}

std::string TestDataGenerator::generateFilePath() {
    return generateValidIdentifier() + "/" + generateValidIdentifier() + ".chtl";
}

// === FileSystemTestUtils 实现 ===

std::vector<std::string> FileSystemTestUtils::tempFiles_;
std::vector<std::string> FileSystemTestUtils::tempDirectories_;

std::string FileSystemTestUtils::createTempDirectory() {
    auto tempPath = std::filesystem::temp_directory_path() / 
                   ("chtl_test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    
    std::filesystem::create_directories(tempPath);
    tempDirectories_.push_back(tempPath.string());
    
    return tempPath.string();
}

std::string FileSystemTestUtils::createTempFile(const std::string& content) {
    auto tempDir = std::filesystem::temp_directory_path();
    auto tempFile = tempDir / ("chtl_test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".tmp");
    
    std::ofstream file(tempFile);
    if (file.is_open()) {
        file << content;
        file.close();
        tempFiles_.push_back(tempFile.string());
        return tempFile.string();
    }
    
    return "";
}

void FileSystemTestUtils::cleanupTempFiles() {
    for (const auto& file : tempFiles_) {
        try {
            std::filesystem::remove(file);
        } catch (...) {
            // 忽略清理错误
        }
    }
    
    for (const auto& dir : tempDirectories_) {
        try {
            std::filesystem::remove_all(dir);
        } catch (...) {
            // 忽略清理错误
        }
    }
    
    tempFiles_.clear();
    tempDirectories_.clear();
}

std::string FileSystemTestUtils::createTestCMOD(const std::string& moduleName, const std::string& content) {
    auto tempDir = createTempDirectory();
    auto moduleDir = std::filesystem::path(tempDir) / moduleName;
    auto srcDir = moduleDir / "src";
    auto infoDir = moduleDir / "info";
    
    std::filesystem::create_directories(srcDir);
    std::filesystem::create_directories(infoDir);
    
    // 创建info文件
    std::ofstream infoFile(infoDir / (moduleName + ".chtl"));
    infoFile << "[Info]\n";
    infoFile << "Name: Test " << moduleName << "\n";
    infoFile << "Version: 1.0.0\n";
    infoFile << "Author: Test\n";
    infoFile << "Description: Test module\n";
    infoFile << "\n[Export]\n";
    infoFile << "Component: " << moduleName << "\n";
    infoFile.close();
    
    // 创建src文件
    std::ofstream srcFile(srcDir / (moduleName + ".chtl"));
    if (!content.empty()) {
        srcFile << content;
    } else {
        srcFile << "[Style]\n";
        srcFile << "." << moduleName << " { color: red; }\n";
        srcFile << "\n[Component]\n";
        srcFile << "{{" << moduleName << "}}\n";
        srcFile << "  {{." << moduleName << "}}Test{{/." << moduleName << "}}\n";
        srcFile << "{{/" << moduleName << "}}\n";
    }
    srcFile.close();
    
    return moduleDir.string();
}

bool FileSystemTestUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string FileSystemTestUtils::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool FileSystemTestUtils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

// === PerformanceTestUtils 实现 ===

PerformanceTestUtils::Timer::Timer() : running_(false) {}

void PerformanceTestUtils::Timer::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
    running_ = true;
}

void PerformanceTestUtils::Timer::stop() {
    if (running_) {
        endTime_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
}

void PerformanceTestUtils::Timer::reset() {
    running_ = false;
}

std::chrono::milliseconds PerformanceTestUtils::Timer::elapsed() const {
    auto end = running_ ? std::chrono::high_resolution_clock::now() : endTime_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - startTime_);
}

std::chrono::milliseconds PerformanceTestUtils::benchmark(std::function<void()> func, int iterations) {
    Timer timer;
    timer.start();
    
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    
    timer.stop();
    return timer.elapsed();
}

// === StringTestUtils 实现 ===

bool StringTestUtils::compareIgnoreWhitespace(const std::string& str1, const std::string& str2) {
    auto normalize = [](const std::string& str) {
        std::string result;
        for (char c : str) {
            if (!std::isspace(c)) {
                result += std::tolower(c);
            }
        }
        return result;
    };
    
    return normalize(str1) == normalize(str2);
}

bool StringTestUtils::compareMultiline(const std::string& str1, const std::string& str2) {
    std::istringstream iss1(str1), iss2(str2);
    std::string line1, line2;
    
    while (std::getline(iss1, line1) && std::getline(iss2, line2)) {
        if (line1 != line2) {
            return false;
        }
    }
    
    return iss1.eof() && iss2.eof();
}

std::vector<std::string> StringTestUtils::findDifferences(const std::string& expected, const std::string& actual) {
    std::vector<std::string> differences;
    
    std::istringstream expectedStream(expected), actualStream(actual);
    std::string expectedLine, actualLine;
    int lineNumber = 1;
    
    while (std::getline(expectedStream, expectedLine) || std::getline(actualStream, actualLine)) {
        if (expectedLine != actualLine) {
            std::ostringstream oss;
            oss << "第" << lineNumber << "行不同:\n";
            oss << "  期望: " << formatForDisplay(expectedLine) << "\n";
            oss << "  实际: " << formatForDisplay(actualLine);
            differences.push_back(oss.str());
        }
        lineNumber++;
    }
    
    return differences;
}

std::string StringTestUtils::formatForDisplay(const std::string& str, size_t maxLength) {
    if (str.length() <= maxLength) {
        return str;
    }
    
    return str.substr(0, maxLength - 3) + "...";
}

// === CHTLTestUtils 实现 ===

bool CHTLTestUtils::validateCHTLSyntax(const std::string& code) {
    // 简化的CHTL语法验证
    std::regex sectionRegex(R"(\[(?:Info|Export|Style|Component|Script|Import|Global|Local|Namespace\s+\w+)\])");
    return std::regex_search(code, sectionRegex);
}

bool CHTLTestUtils::validateComponentStructure(const std::string& componentCode) {
    // 检查组件标签匹配
    std::regex openTag(R"(\{\{([A-Z][a-zA-Z0-9]*)\}\})");
    std::regex closeTag(R"(\{\{/([A-Z][a-zA-Z0-9]*)\}\})");
    
    std::vector<std::string> openTags, closeTags;
    
    std::sregex_iterator openIter(componentCode.begin(), componentCode.end(), openTag);
    std::sregex_iterator closeIter(componentCode.begin(), componentCode.end(), closeTag);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = openIter; i != end; ++i) {
        openTags.push_back((*i)[1]);
    }
    
    for (std::sregex_iterator i = closeIter; i != end; ++i) {
        closeTags.push_back((*i)[1]);
    }
    
    return openTags.size() == closeTags.size();
}

bool CHTLTestUtils::validateCHTLJSSyntax(const std::string& chtlJSCode) {
    // 检查CHTL JS语法
    std::regex chtlJSRegex(R"(\{\{([.#][a-zA-Z0-9_-]+)\s*->\s*(\w+)\}\})");
    return std::regex_search(chtlJSCode, chtlJSRegex);
}

bool CHTLTestUtils::validateImportStatement(const std::string& importStatement) {
    std::regex importRegex(R"(@(Html|Style|JavaScript|Chtl|CJmod)\s+from\s+(.+?)(?:\s+as\s+(.+))?)");
    return std::regex_match(importStatement, importRegex);
}

bool CHTLTestUtils::validateNamespace(const std::string& namespaceName) {
    std::regex namespaceRegex(R"(^[a-zA-Z][a-zA-Z0-9_.]*$)");
    return std::regex_match(namespaceName, namespaceRegex);
}

std::string CHTLTestUtils::createTestCHTLCode(const std::string& componentName) {
    std::ostringstream oss;
    
    oss << "[Style]\n";
    oss << "." << componentName << " {\n";
    oss << "    color: #333;\n";
    oss << "    padding: 10px;\n";
    oss << "}\n\n";
    
    oss << "[Component]\n";
    oss << "{{" << componentName << "}}\n";
    oss << "    {{." << componentName << "}}\n";
    oss << "        Test Content\n";
    oss << "    {{/." << componentName << "}}\n";
    oss << "{{/" << componentName << "}}\n\n";
    
    oss << "[Script]\n";
    oss << "function create" << componentName << "(container, options = {}) {\n";
    oss << "    console.log('Creating " << componentName << "');\n";
    oss << "    return container;\n";
    oss << "}\n";
    
    return oss.str();
}

std::map<std::string, std::string> CHTLTestUtils::parseCHTLSections(const std::string& code) {
    std::map<std::string, std::string> sections;
    
    std::istringstream iss(code);
    std::string line;
    std::string currentSection;
    std::ostringstream currentContent;
    
    while (std::getline(iss, line)) {
        std::regex sectionRegex(R"(\[(.+)\])");
        std::smatch match;
        
        if (std::regex_match(line, match, sectionRegex)) {
            // 保存前一个段落
            if (!currentSection.empty()) {
                sections[currentSection] = currentContent.str();
            }
            
            // 开始新段落
            currentSection = match[1];
            currentContent.str("");
            currentContent.clear();
        } else if (!currentSection.empty()) {
            currentContent << line << "\n";
        }
    }
    
    // 保存最后一个段落
    if (!currentSection.empty()) {
        sections[currentSection] = currentContent.str();
    }
    
    return sections;
}

bool CHTLTestUtils::validateCMODStructure(const std::string& cmodPath) {
    std::filesystem::path path(cmodPath);
    std::string moduleName = path.filename().string();
    
    auto srcFile = path / "src" / (moduleName + ".chtl");
    auto infoFile = path / "info" / (moduleName + ".chtl");
    
    return std::filesystem::exists(srcFile) && std::filesystem::exists(infoFile);
}

bool CHTLTestUtils::validateCJMODStructure(const std::string& cjmodPath) {
    std::filesystem::path path(cjmodPath);
    std::string moduleName = path.filename().string();
    
    auto infoFile = path / (moduleName + ".chtl");
    auto srcDir = path / "src";
    
    return std::filesystem::exists(infoFile) && std::filesystem::exists(srcDir);
}

} // namespace utils
} // namespace test
} // namespace chtl