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
    
    // 创建主模块C++源文件
    std::ofstream main_cpp(TEST_CJMOD_DIR + "/src/TestCJModule.cpp");
    main_cpp << R"(
#include <iostream>
#include <string>

namespace chtl_js {

extern "C" {
    void customAlert(const std::string& message) {
        std::cout << "Custom Alert: " << message << std::endl;
    }
    
    void initTestCJModule() {
        std::cout << "TestCJModule initialized" << std::endl;
    }
}

} // namespace chtl_js
)";
    main_cpp.close();
    
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
}

// 清理测试目录
void cleanupTestDirectory() {
    if (std::filesystem::exists(TEST_DIR)) {
        std::filesystem::remove_all(TEST_DIR);
    }
}

void testCJmodBasicFunctionality() {
    std::cout << "=== 测试CJmod基础功能 ===" << std::endl;
    
    CJmodManager manager;
    
    // 创建测试结构
    createTestCJmodStructure();
    
    // 测试结构验证
    assert(manager.validateCJmodStructure(TEST_CJMOD_DIR));
    
    // 测试打包
    assert(manager.packCJmod(TEST_CJMOD_DIR, TEST_CJMOD_FILE));
    assert(std::filesystem::exists(TEST_CJMOD_FILE));
    
    // 测试验证打包后的文件
    assert(manager.validateCJmodFile(TEST_CJMOD_FILE));
    
    // 测试解包
    std::string unpack_dir = TEST_DIR + "/unpacked";
    assert(manager.unpackCJmod(TEST_CJMOD_FILE, unpack_dir));
    
    // 验证解包后的结构
    assert(std::filesystem::exists(unpack_dir + "/src"));
    assert(std::filesystem::exists(unpack_dir + "/info"));
    assert(std::filesystem::exists(unpack_dir + "/src/TestCJModule.cpp"));
    assert(std::filesystem::exists(unpack_dir + "/info/TestCJModule.chtl"));
    
    std::cout << "✅ CJmod基础功能测试通过!" << std::endl;
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

void testOfficialModulePrefix() {
    std::cout << "=== 测试chtl::官方模块前缀 ===" << std::endl;
    
    CJmodManager manager;
    
    // 测试官方模块识别
    assert(manager.isOfficialModule("chtl::TestModule"));
    assert(!manager.isOfficialModule("TestModule"));
    
    // 测试前缀处理
    assert(manager.stripOfficialPrefix("chtl::TestModule") == "TestModule");
    assert(manager.stripOfficialPrefix("TestModule") == "TestModule");
    assert(manager.addOfficialPrefix("TestModule") == "chtl::TestModule");
    
    std::cout << "✅ chtl::官方模块前缀测试通过!" << std::endl;
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
    bool found_main_info = false;
    
    for (const std::string& file : files) {
        if (file == "src/TestCJModule.cpp") found_main_cpp = true;
        if (file == "info/TestCJModule.chtl") found_main_info = true;
    }
    
    assert(found_main_cpp);
    assert(found_main_info);
    
    // 测试文件提取
    std::string cpp_content;
    assert(manager.extractCJmodFile(TEST_CJMOD_FILE, "src/TestCJModule.cpp", cpp_content));
    assert(!cpp_content.empty());
    assert(cpp_content.find("customAlert") != std::string::npos);
    
    std::cout << "✅ CJmod文件操作测试通过!" << std::endl;
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
    
    std::cout << "✅ CJmod错误处理测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CJmod功能..." << std::endl << std::endl;
        
        // 清理之前的测试数据
        cleanupTestDirectory();
        
        testCJmodBasicFunctionality();
        std::cout << std::endl;
        
        testCJmodInfoParsing();
        std::cout << std::endl;
        
        testOfficialModulePrefix();
        std::cout << std::endl;
        
        testCJmodFileOperations();
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