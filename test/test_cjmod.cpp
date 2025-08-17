#include "../src/common/CJmodManager.h"
#include "../src/common/SimpleZip.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace chtl;

// 测试用的临时目录和文件
const std::string TEST_DIR = "test_cjmod_temp";
const std::string TEST_CJMOD_DIR = TEST_DIR + "/TestCJModule";
const std::string TEST_CJMOD_FILE = TEST_DIR + "/TestCJModule.cjmod";

// 创建测试用的CJmod目录结构
void createTestCJmodStructure() {
    std::filesystem::create_directories(TEST_CJMOD_DIR + "/src");
    std::filesystem::create_directories(TEST_CJMOD_DIR + "/info");
    std::filesystem::create_directories(TEST_CJMOD_DIR + "/src/SubCJModule/src");
    std::filesystem::create_directories(TEST_CJMOD_DIR + "/src/SubCJModule/info");
    
    // 创建主模块C++源文件
    std::ofstream main_cpp(TEST_CJMOD_DIR + "/src/TestCJModule.cpp");
    main_cpp << R"(
#include "TestCJModule.h"
#include <iostream>
#include <string>

namespace chtl_js {

// CHTL JS扩展函数
extern "C" {
    CHTL_JS_EXPORT void customAlert(const std::string& message) {
        std::cout << "Custom Alert: " << message << std::endl;
    }
    
    CHTL_JS_EXPORT int customMath(int a, int b) {
        return a * b + 10;
    }
    
    CHTL_JS_EXPORT void initTestCJModule() {
        std::cout << "TestCJModule initialized" << std::endl;
    }
}

} // namespace chtl_js
)";
    main_cpp.close();
    
    // 创建主模块头文件
    std::ofstream main_h(TEST_CJMOD_DIR + "/src/TestCJModule.h");
    main_h << R"(
#pragma once
#include <string>

#define CHTL_JS_EXPORT __attribute__((visibility("default")))

namespace chtl_js {

extern "C" {
    CHTL_JS_EXPORT void customAlert(const std::string& message);
    CHTL_JS_EXPORT int customMath(int a, int b);
    CHTL_JS_EXPORT void initTestCJModule();
}

} // namespace chtl_js
)";
    main_h.close();
    
    // 创建主模块信息文件（注意：CJmod的info.chtl不具有[Export]节）
    std::ofstream main_info(TEST_CJMOD_DIR + "/info/TestCJModule.chtl");
    main_info << R"(
[Info]
{
    name = "TestCJModule";
    version = "1.0.0";
    description = "Test CJmod module for CHTL JS extensions";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "js-extension";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
    apiVersion = "1.0.0";
    entryPoint = "initTestCJModule";
}
)";
    main_info.close();
    
    // 创建子模块C++源文件
    std::ofstream sub_cpp(TEST_CJMOD_DIR + "/src/SubCJModule/src/SubCJModule.cpp");
    sub_cpp << R"(
#include "SubCJModule.h"

namespace chtl_js {

extern "C" {
    CHTL_JS_EXPORT void subModuleFunction() {
        std::cout << "Sub CJModule function called" << std::endl;
    }
}

} // namespace chtl_js
)";
    sub_cpp.close();
    
    // 创建子模块头文件
    std::ofstream sub_h(TEST_CJMOD_DIR + "/src/SubCJModule/src/SubCJModule.h");
    sub_h << R"(
#pragma once

#define CHTL_JS_EXPORT __attribute__((visibility("default")))

namespace chtl_js {

extern "C" {
    CHTL_JS_EXPORT void subModuleFunction();
}

} // namespace chtl_js
)";
    sub_h.close();
    
    // 创建子模块信息文件
    std::ofstream sub_info(TEST_CJMOD_DIR + "/src/SubCJModule/info/SubCJModule.chtl");
    sub_info << R"(
[Info]
{
    name = "SubCJModule";
    version = "1.0.0";
    description = "Test sub CJmod module";
    author = "CHTL Team";
    license = "MIT";
    apiVersion = "1.0.0";
    entryPoint = "subModuleFunction";
}
)";
    sub_info.close();
}

// 清理测试目录
void cleanupTestDirectory() {
    if (std::filesystem::exists(TEST_DIR)) {
        std::filesystem::remove_all(TEST_DIR);
    }
}

void testCJmodStructureValidation() {
    std::cout << "=== 测试CJmod结构验证 ===" << std::endl;
    
    CJmodManager manager;
    
    // 创建测试结构
    createTestCJmodStructure();
    
    // 测试结构验证
    assert(manager.validateCJmodStructure(TEST_CJMOD_DIR));
    
    // 测试结构分析
    CJmodStructure structure;
    assert(manager.analyzeCJmodStructure(TEST_CJMOD_DIR, structure));
    
    assert(structure.main_module_name == "TestCJModule");
    assert(structure.src_dir == "src");
    assert(structure.info_dir == "info");
    assert(structure.has_main_cpp == true);
    assert(structure.has_main_header == true);
    assert(!structure.cpp_files.empty());
    assert(!structure.header_files.empty());
    assert(!structure.submodules.empty());
    
    std::cout << "✅ CJmod结构验证测试通过!" << std::endl;
}

void testCJmodPacking() {
    std::cout << "=== 测试CJmod打包功能 ===" << std::endl;
    
    CJmodManager manager;
    
    // 确保测试结构存在
    createTestCJmodStructure();
    
    // 测试打包
    assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    assert(std::filesystem::exists(TEST_CJMOD_FILE));
    
    // 验证打包后的文件
    assert(manager.validateCJmodFile(TEST_CJMOD_FILE));
    
    std::cout << "✅ CJmod打包功能测试通过!" << std::endl;
}

void testCJmodUnpacking() {
    std::cout << "=== 测试CJmod解包功能 ===" << std::endl;
    
    CJmodManager manager;
    
    // 确保有打包好的CJmod文件
    if (!std::filesystem::exists(TEST_CJMOD_FILE)) {
        createTestCJmodStructure();
        assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    }
    
    // 测试解包
    std::string unpack_dir = TEST_DIR + "/unpacked";
    assert(manager.unpackCJmod(TEST_CJMOD_FILE, unpack_dir));
    
    // 验证解包后的结构
    assert(std::filesystem::exists(unpack_dir + "/src"));
    assert(std::filesystem::exists(unpack_dir + "/info"));
    assert(std::filesystem::exists(unpack_dir + "/src/TestCJModule.cpp"));
    assert(std::filesystem::exists(unpack_dir + "/src/TestCJModule.h"));
    assert(std::filesystem::exists(unpack_dir + "/info/TestCJModule.chtl"));
    
    std::cout << "✅ CJmod解包功能测试通过!" << std::endl;
}

void testCJmodInfoParsing() {
    std::cout << "=== 测试CJmod信息解析 ===" << std::endl;
    
    CJmodManager manager;
    
    // 确保有CJmod文件
    if (!std::filesystem::exists(TEST_CJMOD_FILE)) {
        createTestCJmodStructure();
        assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    }
    
    // 测试信息提取
    CJmodInfo info;
    assert(manager.extractCJmodInfo(TEST_CJMOD_FILE, info));
    
    // 验证信息内容
    assert(info.name == "TestCJModule");
    assert(info.version == "1.0.0");
    assert(info.author == "CHTL Team");
    assert(info.license == "MIT");
    assert(info.category == "js-extension");
    assert(info.api_version == "1.0.0");
    assert(info.entry_point == "initTestCJModule");
    
    std::cout << "✅ CJmod信息解析测试通过!" << std::endl;
}

void testCJmodSubmoduleHandling() {
    std::cout << "=== 测试CJmod子模块处理 ===" << std::endl;
    
    CJmodManager manager;
    
    // 确保有CJmod文件
    if (!std::filesystem::exists(TEST_CJMOD_FILE)) {
        createTestCJmodStructure();
        assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    }
    
    // 测试子模块获取
    auto submodules = manager.getCJmodSubmodules(TEST_CJMOD_FILE);
    assert(!submodules.empty());
    assert(std::find(submodules.begin(), submodules.end(), "SubCJModule") != submodules.end());
    
    // 测试子模块检查
    assert(manager.hasCJmodSubmodule(TEST_CJMOD_FILE, "SubCJModule"));
    assert(!manager.hasCJmodSubmodule(TEST_CJMOD_FILE, "NonExistentSubModule"));
    
    std::cout << "✅ CJmod子模块处理测试通过!" << std::endl;
}

void testCJmodFileOperations() {
    std::cout << "=== 测试CJmod文件操作 ===" << std::endl;
    
    CJmodManager manager;
    
    // 确保有CJmod文件
    if (!std::filesystem::exists(TEST_CJMOD_FILE)) {
        createTestCJmodStructure();
        assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    }
    
    // 测试文件列表
    auto files = manager.listCJmodFiles(TEST_CJMOD_FILE);
    assert(!files.empty());
    
    bool found_main_cpp = false;
    bool found_main_h = false;
    bool found_main_info = false;
    
    for (const std::string& file : files) {
        if (file == "src/TestCJModule.cpp") found_main_cpp = true;
        if (file == "src/TestCJModule.h") found_main_h = true;
        if (file == "info/TestCJModule.chtl") found_main_info = true;
    }
    
    assert(found_main_cpp);
    assert(found_main_h);
    assert(found_main_info);
    
    // 测试文件提取
    std::string cpp_content;
    assert(manager.extractCJmodFile(TEST_CJMOD_FILE, "src/TestCJModule.cpp", cpp_content));
    assert(!cpp_content.empty());
    assert(cpp_content.find("customAlert") != std::string::npos);
    
    std::cout << "✅ CJmod文件操作测试通过!" << std::endl;
}

void testOfficialModulePrefix() {
    std::cout << "=== 测试chtl::官方模块前缀 ===" << std::endl;
    
    CJmodManager manager;
    
    // 测试官方模块识别
    assert(manager.isOfficialModule("chtl::TestModule"));
    assert(!manager.isOfficialModule("TestModule"));
    assert(!manager.isOfficialModule("other::TestModule"));
    
    // 测试前缀处理
    assert(manager.stripOfficialPrefix("chtl::TestModule") == "TestModule");
    assert(manager.stripOfficialPrefix("TestModule") == "TestModule");
    assert(manager.addOfficialPrefix("TestModule") == "chtl::TestModule");
    assert(manager.addOfficialPrefix("chtl::TestModule") == "chtl::TestModule");
    
    std::cout << "✅ chtl::官方模块前缀测试通过!" << std::endl;
}

void testCJmodPathResolution() {
    std::cout << "=== 测试CJmod路径解析功能 ===" << std::endl;
    
    CJmodManager manager;
    
    // 添加测试搜索路径
    manager.addCJmodSearchPath(TEST_DIR);
    
    // 确保有CJmod文件
    if (!std::filesystem::exists(TEST_CJMOD_FILE)) {
        createTestCJmodStructure();
        assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    }
    
    // 测试路径解析
    std::string resolved_path = manager.resolveCJmodPath("TestCJModule", manager.getCJmodSearchPaths());
    assert(!resolved_path.empty());
    assert(resolved_path.size() >= 17 && resolved_path.substr(resolved_path.size() - 17) == "TestCJModule.cjmod");
    
    std::cout << "✅ CJmod路径解析功能测试通过!" << std::endl;
}

void testCJmodVersionCompatibility() {
    std::cout << "=== 测试CJmod版本兼容性检查 ===" << std::endl;
    
    CJmodManager manager;
    
    // 测试版本比较
    assert(manager.isCJmodVersionCompatible("1.0.0", "1.0.0"));
    assert(manager.isCJmodVersionCompatible("1.0.0", "1.1.0"));
    assert(!manager.isCJmodVersionCompatible("2.0.0", "1.0.0"));
    
    // 测试API版本兼容性
    assert(manager.isAPIVersionCompatible("1.0.0", "1.0.0"));
    assert(manager.isAPIVersionCompatible("1.0.0", "1.1.0"));
    assert(!manager.isAPIVersionCompatible("2.0.0", "1.0.0"));
    
    std::cout << "✅ CJmod版本兼容性检查测试通过!" << std::endl;
}

void testCJmodCppAnalysis() {
    std::cout << "=== 测试C++代码分析功能 ===" << std::endl;
    
    CJmodManager manager;
    
    std::string test_cpp_content = R"(
#include <iostream>
#include <string>
#include "TestModule.h"

namespace chtl_js {

extern "C" {
    CHTL_JS_EXPORT void testFunction1();
    CHTL_JS_EXPORT int testFunction2(int param);
}

} // namespace chtl_js
)";
    
    // 测试头文件提取
    auto includes = manager.extractIncludeHeaders(test_cpp_content);
    assert(!includes.empty());
    
    // 测试导出函数提取
    auto functions = manager.extractExportedFunctions(test_cpp_content);
    assert(!functions.empty());
    
    std::cout << "✅ C++代码分析功能测试通过!" << std::endl;
}

void testCJmodWrapperGeneration() {
    std::cout << "=== 测试CJmod包装代码生成 ===" << std::endl;
    
    CJmodManager manager;
    
    // 创建测试CJmod信息
    CJmodInfo test_info;
    test_info.name = "TestWrapper";
    test_info.version = "1.0.0";
    test_info.api_version = "1.0.0";
    test_info.entry_point = "initTestWrapper";
    
    // 生成包装代码
    std::string wrapper_path = TEST_DIR + "/test_wrapper.cpp";
    std::filesystem::create_directories(TEST_DIR);
    assert(manager.generateCJmodWrapper(test_info, wrapper_path));
    assert(std::filesystem::exists(wrapper_path));
    
    // 验证生成的代码
    std::string wrapper_content;
    assert(manager.readFileContent(wrapper_path, wrapper_content));
    assert(wrapper_content.find("getCJmodName") != std::string::npos);
    assert(wrapper_content.find("getCJmodVersion") != std::string::npos);
    assert(wrapper_content.find("getCJmodAPIVersion") != std::string::npos);
    assert(wrapper_content.find("initTestWrapper") != std::string::npos);
    
    std::cout << "✅ CJmod包装代码生成测试通过!" << std::endl;
}

void testCJmodUtils() {
    std::cout << "=== 测试CJmodUtils工具函数 ===" << std::endl;
    
    // 测试模块名解析
    auto [main, sub] = CJmodUtils::parseCJmodModuleName("TestCJModule.SubModule");
    assert(main == "TestCJModule");
    assert(sub == "SubModule");
    
    // 测试官方模块名解析
    auto [official_main, official_sub] = CJmodUtils::parseCJmodModuleName("chtl::OfficialModule.SubModule");
    assert(official_main == "OfficialModule");
    assert(official_sub == "SubModule");
    
    // 测试官方模块识别
    assert(CJmodUtils::isOfficialModuleName("chtl::TestModule"));
    assert(!CJmodUtils::isOfficialModuleName("TestModule"));
    
    // 测试版本比较
    assert(CJmodUtils::compareVersions("1.0.0", "1.0.0") == 0);
    assert(CJmodUtils::compareVersions("1.1.0", "1.0.0") > 0);
    assert(CJmodUtils::compareVersions("1.0.0", "1.1.0") < 0);
    
    std::cout << "✅ CJmodUtils工具函数测试通过!" << std::endl;
}

void testCJmodErrorHandling() {
    std::cout << "=== 测试CJmod错误处理 ===" << std::endl;
    
    CJmodManager manager;
    
    // 测试不存在的目录
    assert(!manager.validateCJmodStructure("nonexistent_cjmod_directory"));
    assert(!manager.getLastError().empty());
    
    // 测试无效的CJmod文件
    assert(!manager.validateCJmodFile("nonexistent.cjmod"));
    assert(!manager.getLastError().empty());
    
    // 测试路径解析失败
    std::string resolved = manager.resolveCJmodPath("NonExistentCJModule", manager.getCJmodSearchPaths());
    assert(resolved.empty());
    
    std::cout << "✅ CJmod错误处理测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CJmod功能..." << std::endl << std::endl;
        
        // 清理之前的测试数据
        cleanupTestDirectory();
        
        testCJmodStructureValidation();
        std::cout << std::endl;
        
        testCJmodPacking();
        std::cout << std::endl;
        
        testCJmodUnpacking();
        std::cout << std::endl;
        
        testCJmodInfoParsing();
        std::cout << std::endl;
        
        testCJmodSubmoduleHandling();
        std::cout << std::endl;
        
        testCJmodFileOperations();
        std::cout << std::endl;
        
        testOfficialModulePrefix();
        std::cout << std::endl;
        
        testCJmodPathResolution();
        std::cout << std::endl;
        
        testCJmodVersionCompatibility();
        std::cout << std::endl;
        
        testCJmodCppAnalysis();
        std::cout << std::endl;
        
        testCJmodWrapperGeneration();
        std::cout << std::endl;
        
        testCJmodUtils();
        std::cout << std::endl;
        
        testCJmodErrorHandling();
        std::cout << std::endl;
        
        std::cout << "🎉 所有CJmod功能测试通过!" << std::endl;
        
        // 清理测试数据
        cleanupTestDirectory();
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        cleanupTestDirectory();
        return 1;
    }
    
    return 0;
}