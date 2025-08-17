#include "../src/common/CmodManager.h"
#include "../src/common/SimpleZip.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace chtl;

// 测试用的临时目录和文件
const std::string TEST_DIR = "test_cmod_temp";
const std::string TEST_CMOD_DIR = TEST_DIR + "/TestModule";
const std::string TEST_CMOD_FILE = TEST_DIR + "/TestModule.cmod";

// 创建测试用的Cmod目录结构
void createTestCmodStructure() {
    std::filesystem::create_directories(TEST_CMOD_DIR + "/src");
    std::filesystem::create_directories(TEST_CMOD_DIR + "/info");
    std::filesystem::create_directories(TEST_CMOD_DIR + "/src/SubModule/src");
    std::filesystem::create_directories(TEST_CMOD_DIR + "/src/SubModule/info");
    
    // 创建主模块源文件
    std::ofstream main_src(TEST_CMOD_DIR + "/src/TestModule.chtl");
    main_src << R"(
[Template] @Style MainStyle {
    background-color: blue;
    color: white;
}

[Template] @Element MainElement {
    div {
        @Style MainStyle;
        text { "Main Module Content" }
    }
}
)";
    main_src.close();
    
    // 创建主模块信息文件
    std::ofstream main_info(TEST_CMOD_DIR + "/info/TestModule.chtl");
    main_info << R"(
[Info]
{
    name = "TestModule";
    version = "1.0.0";
    description = "Test module for CHTL Cmod system";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "test";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

[Export]
{
    @Style MainStyle;
    @Element MainElement;
}
)";
    main_info.close();
    
    // 创建子模块源文件
    std::ofstream sub_src(TEST_CMOD_DIR + "/src/SubModule/src/SubModule.chtl");
    sub_src << R"(
[Template] @Style SubStyle {
    border: 1px solid red;
}

[Template] @Element SubElement {
    span {
        @Style SubStyle;
        text { "Sub Module Content" }
    }
}
)";
    sub_src.close();
    
    // 创建子模块信息文件
    std::ofstream sub_info(TEST_CMOD_DIR + "/src/SubModule/info/SubModule.chtl");
    sub_info << R"(
[Info]
{
    name = "SubModule";
    version = "1.0.0";
    description = "Test sub module";
    author = "CHTL Team";
    license = "MIT";
}

[Export]
{
    @Style SubStyle;
    @Element SubElement;
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

void testSimpleZip() {
    std::cout << "=== 测试SimpleZip基础功能 ===" << std::endl;
    
    SimpleZip zip;
    
    // 测试添加文件
    std::string test_content = "Hello, CHTL Cmod!";
    assert(zip.addFile("test.txt", test_content));
    
    // 测试保存ZIP文件
    std::string zip_path = TEST_DIR + "/test.zip";
    std::filesystem::create_directories(TEST_DIR);
    assert(zip.saveToFile(zip_path));
    
    // 测试加载ZIP文件
    SimpleZip zip2;
    assert(zip2.loadFromFile(zip_path));
    
    // 测试提取文件
    std::string extracted_content;
    assert(zip2.extractFile("test.txt", extracted_content));
    assert(extracted_content == test_content);
    
    // 测试文件列表
    auto files = zip2.getFileList();
    assert(files.size() == 1);
    assert(files[0] == "test.txt");
    
    std::cout << "✅ SimpleZip基础功能测试通过!" << std::endl;
}

void testCmodStructureValidation() {
    std::cout << "=== 测试Cmod结构验证 ===" << std::endl;
    
    CmodManager manager;
    
    // 创建测试结构
    createTestCmodStructure();
    
    // 测试结构验证
    assert(manager.validateCmodStructure(TEST_CMOD_DIR));
    
    // 测试结构分析
    CmodStructure structure;
    assert(manager.analyzeCmodStructure(TEST_CMOD_DIR, structure));
    
    assert(structure.main_module_name == "TestModule");
    assert(structure.src_dir == "src");
    assert(structure.info_dir == "info");
    assert(structure.has_main_file == true);
    assert(!structure.submodules.empty());
    
    std::cout << "✅ Cmod结构验证测试通过!" << std::endl;
}

void testCmodPacking() {
    std::cout << "=== 测试Cmod打包功能 ===" << std::endl;
    
    CmodManager manager;
    
    // 确保测试结构存在
    createTestCmodStructure();
    
    // 测试打包
    assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    assert(std::filesystem::exists(TEST_CMOD_FILE));
    
    // 验证打包后的文件
    assert(manager.validateCmodFile(TEST_CMOD_FILE));
    
    std::cout << "✅ Cmod打包功能测试通过!" << std::endl;
}

void testCmodUnpacking() {
    std::cout << "=== 测试Cmod解包功能 ===" << std::endl;
    
    CmodManager manager;
    
    // 确保有打包好的Cmod文件
    if (!std::filesystem::exists(TEST_CMOD_FILE)) {
        createTestCmodStructure();
        assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    }
    
    // 测试解包
    std::string unpack_dir = TEST_DIR + "/unpacked";
    assert(manager.unpackCmod(TEST_CMOD_FILE, unpack_dir));
    
    // 验证解包后的结构
    assert(std::filesystem::exists(unpack_dir + "/src"));
    assert(std::filesystem::exists(unpack_dir + "/info"));
    assert(std::filesystem::exists(unpack_dir + "/src/TestModule.chtl"));
    assert(std::filesystem::exists(unpack_dir + "/info/TestModule.chtl"));
    
    std::cout << "✅ Cmod解包功能测试通过!" << std::endl;
}

void testCmodInfoParsing() {
    std::cout << "=== 测试Cmod信息解析 ===" << std::endl;
    
    CmodManager manager;
    
    // 确保有Cmod文件
    if (!std::filesystem::exists(TEST_CMOD_FILE)) {
        createTestCmodStructure();
        assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    }
    
    // 测试信息提取
    CmodInfo info;
    assert(manager.extractCmodInfo(TEST_CMOD_FILE, info));
    
    // 验证信息内容
    assert(info.name == "TestModule");
    assert(info.version == "1.0.0");
    assert(info.author == "CHTL Team");
    assert(info.license == "MIT");
    assert(info.category == "test");
    assert(!info.exported_styles.empty());
    assert(!info.exported_elements.empty());
    
    std::cout << "✅ Cmod信息解析测试通过!" << std::endl;
}

void testSubmoduleHandling() {
    std::cout << "=== 测试子模块处理 ===" << std::endl;
    
    CmodManager manager;
    
    // 确保有Cmod文件
    if (!std::filesystem::exists(TEST_CMOD_FILE)) {
        createTestCmodStructure();
        assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    }
    
    // 测试子模块获取
    auto submodules = manager.getSubmodules(TEST_CMOD_FILE);
    assert(!submodules.empty());
    assert(std::find(submodules.begin(), submodules.end(), "SubModule") != submodules.end());
    
    // 测试子模块检查
    assert(manager.hasSubmodule(TEST_CMOD_FILE, "SubModule"));
    assert(!manager.hasSubmodule(TEST_CMOD_FILE, "NonExistentModule"));
    
    std::cout << "✅ 子模块处理测试通过!" << std::endl;
}

void testCmodFileOperations() {
    std::cout << "=== 测试Cmod文件操作 ===" << std::endl;
    
    CmodManager manager;
    
    // 确保有Cmod文件
    if (!std::filesystem::exists(TEST_CMOD_FILE)) {
        createTestCmodStructure();
        assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    }
    
    // 测试文件列表
    auto files = manager.listCmodFiles(TEST_CMOD_FILE);
    assert(!files.empty());
    
    bool found_main_src = false;
    bool found_main_info = false;
    
    for (const std::string& file : files) {
        if (file == "src/TestModule.chtl") found_main_src = true;
        if (file == "info/TestModule.chtl") found_main_info = true;
    }
    
    assert(found_main_src);
    assert(found_main_info);
    
    // 测试文件提取
    std::string content;
    assert(manager.extractCmodFile(TEST_CMOD_FILE, "src/TestModule.chtl", content));
    assert(!content.empty());
    assert(content.find("MainStyle") != std::string::npos);
    
    std::cout << "✅ Cmod文件操作测试通过!" << std::endl;
}

void testPathResolution() {
    std::cout << "=== 测试路径解析功能 ===" << std::endl;
    
    CmodManager manager;
    
    // 添加测试搜索路径
    manager.addSearchPath(TEST_DIR);
    
    // 确保有Cmod文件
    if (!std::filesystem::exists(TEST_CMOD_FILE)) {
        createTestCmodStructure();
        assert(manager.packCmod(TEST_CMOD_DIR, TEST_CMOD_FILE));
    }
    
    // 测试路径解析
    std::string resolved_path = manager.resolveCmodPath("TestModule", manager.getSearchPaths());
    assert(!resolved_path.empty());
    assert(resolved_path.size() >= 15 && resolved_path.substr(resolved_path.size() - 15) == "TestModule.cmod");
    
    std::cout << "✅ 路径解析功能测试通过!" << std::endl;
}

void testVersionCompatibility() {
    std::cout << "=== 测试版本兼容性检查 ===" << std::endl;
    
    CmodManager manager;
    
    // 测试版本比较
    assert(manager.isVersionCompatible("1.0.0", "1.0.0"));
    assert(manager.isVersionCompatible("1.0.0", "1.1.0"));
    assert(!manager.isVersionCompatible("2.0.0", "1.0.0"));
    
    // 测试CHTL版本兼容性
    assert(manager.isCHTLVersionCompatible("1.0.0", "2.0.0", "1.5.0"));
    assert(!manager.isCHTLVersionCompatible("1.0.0", "2.0.0", "0.9.0"));
    assert(!manager.isCHTLVersionCompatible("1.0.0", "2.0.0", "2.1.0"));
    
    std::cout << "✅ 版本兼容性检查测试通过!" << std::endl;
}

void testCmodUtils() {
    std::cout << "=== 测试CmodUtils工具函数 ===" << std::endl;
    
    // 测试模块名解析
    auto [main, sub] = CmodUtils::parseModuleName("TestModule.SubModule");
    assert(main == "TestModule");
    assert(sub == "SubModule");
    
    auto [single, empty] = CmodUtils::parseModuleName("SingleModule");
    assert(single == "SingleModule");
    assert(empty.empty());
    
    // 测试版本比较
    assert(CmodUtils::compareVersions("1.0.0", "1.0.0") == 0);
    assert(CmodUtils::compareVersions("1.1.0", "1.0.0") > 0);
    assert(CmodUtils::compareVersions("1.0.0", "1.1.0") < 0);
    
    // 测试通配符匹配
    assert(CmodUtils::matchesPattern("any/path", "*"));
    assert(CmodUtils::matchesPattern("src/file.chtl", "src/*"));
    assert(!CmodUtils::matchesPattern("other/file.chtl", "src/*"));
    
    std::cout << "✅ CmodUtils工具函数测试通过!" << std::endl;
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理 ===" << std::endl;
    
    CmodManager manager;
    
    // 测试不存在的目录
    assert(!manager.validateCmodStructure("nonexistent_directory"));
    assert(!manager.getLastError().empty());
    
    // 测试无效的Cmod文件
    assert(!manager.validateCmodFile("nonexistent.cmod"));
    assert(!manager.getLastError().empty());
    
    // 测试路径解析失败
    std::string resolved = manager.resolveCmodPath("NonExistentModule", manager.getSearchPaths());
    assert(resolved.empty());
    
    std::cout << "✅ 错误处理测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试Cmod功能..." << std::endl << std::endl;
        
        // 清理之前的测试数据
        cleanupTestDirectory();
        
        testSimpleZip();
        std::cout << std::endl;
        
        testCmodStructureValidation();
        std::cout << std::endl;
        
        testCmodPacking();
        std::cout << std::endl;
        
        testCmodUnpacking();
        std::cout << std::endl;
        
        testCmodInfoParsing();
        std::cout << std::endl;
        
        testSubmoduleHandling();
        std::cout << std::endl;
        
        testCmodFileOperations();
        std::cout << std::endl;
        
        testPathResolution();
        std::cout << std::endl;
        
        testVersionCompatibility();
        std::cout << std::endl;
        
        testCmodUtils();
        std::cout << std::endl;
        
        testErrorHandling();
        std::cout << std::endl;
        
        std::cout << "🎉 所有Cmod功能测试通过!" << std::endl;
        
        // 清理测试数据
        cleanupTestDirectory();
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        cleanupTestDirectory();
        return 1;
    }
    
    return 0;
}