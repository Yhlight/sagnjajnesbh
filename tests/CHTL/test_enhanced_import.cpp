#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Import/EnhancedImportSystem.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Core/CHTLToken.h"

using namespace CHTL;
using namespace CHTL::Import;

// 测试导入类型识别
void test_import_type_recognition() {
    std::cout << "测试导入类型识别...\n";
    
    ImportParser parser;
    
    // 测试各种导入类型
    assert(parser.GetImportType("@Html") == ImportType::HTML);
    assert(parser.GetImportType("@Style") == ImportType::STYLE);
    assert(parser.GetImportType("@JavaScript") == ImportType::JAVASCRIPT);
    assert(parser.GetImportType("@Chtl") == ImportType::CHTL);
    assert(parser.GetImportType("@CJmod") == ImportType::CJMOD);
    assert(parser.GetImportType("[Origin] @Vue") == ImportType::ORIGIN_CUSTOM);
    assert(parser.GetImportType("@Unknown") == ImportType::UNKNOWN);
    
    std::cout << "  导入类型识别验证:\n";
    std::cout << "    @Html -> " << (parser.GetImportType("@Html") == ImportType::HTML ? "HTML" : "错误") << "\n";
    std::cout << "    @Style -> " << (parser.GetImportType("@Style") == ImportType::STYLE ? "STYLE" : "错误") << "\n";
    std::cout << "    @JavaScript -> " << (parser.GetImportType("@JavaScript") == ImportType::JAVASCRIPT ? "JAVASCRIPT" : "错误") << "\n";
    std::cout << "    @Chtl -> " << (parser.GetImportType("@Chtl") == ImportType::CHTL ? "CHTL" : "错误") << "\n";
    std::cout << "    @CJmod -> " << (parser.GetImportType("@CJmod") == ImportType::CJMOD ? "CJMOD" : "错误") << "\n";
    std::cout << "    [Origin] @Vue -> " << (parser.GetImportType("[Origin] @Vue") == ImportType::ORIGIN_CUSTOM ? "ORIGIN_CUSTOM" : "错误") << "\n";
    
    std::cout << "  ✅ 导入类型识别测试通过\n";
}

// 测试路径类型识别
void test_path_type_recognition() {
    std::cout << "测试路径类型识别...\n";
    
    ImportParser parser;
    
    // 测试各种路径类型
    assert(parser.GetPathType("index") == PathType::NAME_ONLY);
    assert(parser.GetPathType("index.html") == PathType::SPECIFIC_FILE);
    assert(parser.GetPathType("./assets/index.html") == PathType::SPECIFIC_PATH);
    assert(parser.GetPathType("./assets/") == PathType::DIRECTORY_PATH);
    assert(parser.GetPathType("assets/*") == PathType::WILDCARD_ALL);
    assert(parser.GetPathType("assets/*.cmod") == PathType::WILDCARD_CMOD);
    assert(parser.GetPathType("assets/*.chtl") == PathType::WILDCARD_CHTL);
    assert(parser.GetPathType("Chtholly.*") == PathType::SUBMODULE_ALL);
    assert(parser.GetPathType("Chtholly.Space") == PathType::SUBMODULE_SPECIFIC);
    
    std::cout << "  路径类型识别验证:\n";
    std::cout << "    index -> NAME_ONLY\n";
    std::cout << "    index.html -> SPECIFIC_FILE\n";
    std::cout << "    ./assets/index.html -> SPECIFIC_PATH\n";
    std::cout << "    ./assets/ -> DIRECTORY_PATH\n";
    std::cout << "    assets/* -> WILDCARD_ALL\n";
    std::cout << "    assets/*.cmod -> WILDCARD_CMOD\n";
    std::cout << "    assets/*.chtl -> WILDCARD_CHTL\n";
    std::cout << "    Chtholly.* -> SUBMODULE_ALL\n";
    std::cout << "    Chtholly.Space -> SUBMODULE_SPECIFIC\n";
    
    std::cout << "  ✅ 路径类型识别测试通过\n";
}

// 测试导入语句解析
void test_import_statement_parsing() {
    std::cout << "测试导入语句解析...\n";
    
    ImportParser parser;
    
    // 测试@Html导入（无as子句）
    std::cout << "  测试@Html导入解析...\n";
    auto htmlInfo1 = parser.ParseImportStatement("[Import] @Html from \"index.html\"");
    assert(htmlInfo1.type == ImportType::HTML);
    assert(htmlInfo1.path == "index.html");
    assert(!htmlInfo1.hasAsClause);
    assert(htmlInfo1.pathType == PathType::SPECIFIC_FILE);
    
    // 测试@Html导入（有as子句）
    auto htmlInfo2 = parser.ParseImportStatement("[Import] @Html from \"index.html\" as index");
    assert(htmlInfo2.type == ImportType::HTML);
    assert(htmlInfo2.path == "index.html");
    assert(htmlInfo2.hasAsClause);
    assert(htmlInfo2.asName == "index");
    
    std::cout << "    @Html导入解析正确\n";
    
    // 测试@Chtl导入（带import列表）
    std::cout << "  测试@Chtl导入解析...\n";
    auto chtlInfo = parser.ParseImportStatement("[Import] @Chtl from \"Chtholly\" { import: [Accordion, CloverAlbum, Memo] }");
    assert(chtlInfo.type == ImportType::CHTL);
    assert(chtlInfo.path == "Chtholly");
    assert(chtlInfo.importList.size() == 3);
    assert(std::find(chtlInfo.importList.begin(), chtlInfo.importList.end(), "Accordion") != chtlInfo.importList.end());
    assert(std::find(chtlInfo.importList.begin(), chtlInfo.importList.end(), "CloverAlbum") != chtlInfo.importList.end());
    assert(std::find(chtlInfo.importList.begin(), chtlInfo.importList.end(), "Memo") != chtlInfo.importList.end());
    
    std::cout << "    @Chtl导入解析正确，导入列表: " << chtlInfo.importList.size() << " 项\n";
    
    // 测试官方模块前缀
    std::cout << "  测试官方模块前缀...\n";
    auto officialInfo = parser.ParseImportStatement("[Import] @Chtl from \"chtl::Chtholly\"");
    assert(officialInfo.type == ImportType::CHTL);
    assert(officialInfo.useOfficialPrefix);
    assert(officialInfo.path == "Chtholly"); // 应该移除chtl::前缀
    
    std::cout << "    官方模块前缀解析正确\n";
    
    // 测试[Origin]自定义类型
    std::cout << "  测试[Origin]自定义类型解析...\n";
    auto originInfo = parser.ParseImportStatement("[Import] [Origin] @Vue from \"component.vue\" as VueComponent");
    assert(originInfo.type == ImportType::ORIGIN_CUSTOM);
    assert(originInfo.path == "component.vue");
    assert(originInfo.hasAsClause);
    assert(originInfo.asName == "VueComponent");
    
    std::cout << "    [Origin]自定义类型解析正确\n";
    
    std::cout << "  ✅ 导入语句解析测试通过\n";
}

// 测试路径规范化
void test_path_normalization() {
    std::cout << "测试路径规范化...\n";
    
    ImportConflictDetector detector;
    
    // 测试各种路径规范化
    assert(detector.NormalizePath("./index.html") == "index.html");
    assert(detector.NormalizePath("../parent/index.html") == "parent/index.html");
    assert(detector.NormalizePath("folder//subfolder///file.txt") == "folder/subfolder/file.txt");
    assert(detector.NormalizePath("folder\\subfolder\\file.txt") == "folder/subfolder/file.txt");
    
    std::cout << "  路径规范化验证:\n";
    std::cout << "    ./index.html -> " << detector.NormalizePath("./index.html") << "\n";
    std::cout << "    ../parent/index.html -> " << detector.NormalizePath("../parent/index.html") << "\n";
    std::cout << "    folder//subfolder///file.txt -> " << detector.NormalizePath("folder//subfolder///file.txt") << "\n";
    std::cout << "    folder\\subfolder\\file.txt -> " << detector.NormalizePath("folder\\subfolder\\file.txt") << "\n";
    
    std::cout << "  ✅ 路径规范化测试通过\n";
}

// 测试冲突检测
void test_conflict_detection() {
    std::cout << "测试冲突检测...\n";
    
    ImportConflictDetector detector;
    
    // 测试重复导入检测
    std::cout << "  测试重复导入检测...\n";
    
    assert(!detector.CheckDuplicateImport("file1.chtl"));  // 第一次导入
    detector.AddImportRecord("current.chtl", "file1.chtl");
    assert(detector.CheckDuplicateImport("file1.chtl"));   // 重复导入
    assert(detector.CheckDuplicateImport("./file1.chtl")); // 同一文件的不同表达方式
    
    std::cout << "    重复导入检测正确\n";
    
    // 测试循环依赖检测
    std::cout << "  测试循环依赖检测...\n";
    
    detector.Reset();
    
    // 构建依赖关系：A -> B -> C -> A
    detector.AddImportRecord("A.chtl", "B.chtl");
    detector.AddImportRecord("B.chtl", "C.chtl");
    
    // 检查是否会形成循环
    assert(detector.CheckCircularDependency("C.chtl", "A.chtl")); // 应该检测到循环
    assert(!detector.CheckCircularDependency("C.chtl", "D.chtl")); // 不应该有循环
    
    std::cout << "    循环依赖检测正确\n";
    
    std::cout << "  ✅ 冲突检测测试通过\n";
}

// 测试模块目录结构管理器
void test_module_directory_manager() {
    std::cout << "测试模块目录结构管理器...\n";
    
    ModuleDirectoryManager manager("/test/modules");
    
    // 测试目录结构检测（模拟）
    // 注意：实际测试中这些目录可能不存在，这里主要测试逻辑
    
    std::cout << "  模块目录结构管理器创建成功\n";
    std::cout << "  官方模块路径: /test/modules\n";
    
    // 测试分类结构和混杂结构的搜索逻辑
    // 这里主要验证方法调用不会崩溃
    std::vector<std::string> extensions = {".cmod", ".chtl"};
    std::string result1 = manager.SearchInClassifiedStructure("/test/modules", "TestModule", extensions);
    std::string result2 = manager.SearchInMixedStructure("/test/modules", "TestModule", extensions);
    
    std::cout << "    分类结构搜索完成（结果可能为空）\n";
    std::cout << "    混杂结构搜索完成（结果可能为空）\n";
    
    std::cout << "  ✅ 模块目录结构管理器测试通过\n";
}

// 测试增强导入系统基础功能
void test_enhanced_import_system_basic() {
    std::cout << "测试增强导入系统基础功能...\n";
    
    EnhancedImportSystem importSystem("/test/current.chtl", "/test/modules");
    
    // 测试配置设置
    importSystem.SetConfig(false); // 启用默认命名空间
    
    // 测试统计信息
    std::string stats = importSystem.GetStatistics();
    assert(!stats.empty());
    
    std::cout << "  初始统计信息:\n" << stats;
    
    // 测试重置
    importSystem.Reset();
    std::string resetStats = importSystem.GetStatistics();
    assert(resetStats.find("0") != std::string::npos); // 应该包含0的统计
    
    std::cout << "  重置后统计信息:\n" << resetStats;
    
    std::cout << "  ✅ 增强导入系统基础功能测试通过\n";
}

// 测试Web资源导入规则
void test_web_asset_import_rules() {
    std::cout << "测试Web资源导入规则...\n";
    
    ImportParser parser;
    
    // 测试@Html导入（没有as子句，应该跳过）
    std::cout << "  测试@Html导入（无as子句）...\n";
    auto htmlInfo1 = parser.ParseImportStatement("[Import] @Html from \"index.html\"");
    assert(htmlInfo1.type == ImportType::HTML);
    assert(htmlInfo1.path == "index.html");
    assert(!htmlInfo1.hasAsClause);
    
    std::cout << "    无as子句的@Html导入解析正确（应该跳过）\n";
    
    // 测试@Html导入（有as子句，应该创建带名原始嵌入节点）
    std::cout << "  测试@Html导入（有as子句）...\n";
    auto htmlInfo2 = parser.ParseImportStatement("[Import] @Html from \"index.html\" as index");
    assert(htmlInfo2.type == ImportType::HTML);
    assert(htmlInfo2.path == "index.html");
    assert(htmlInfo2.hasAsClause);
    assert(htmlInfo2.asName == "index");
    
    std::cout << "    有as子句的@Html导入解析正确（应该创建带名原始嵌入节点）\n";
    
    // 测试@Style导入
    std::cout << "  测试@Style导入...\n";
    auto styleInfo = parser.ParseImportStatement("[Import] @Style from \"styles.css\" as mainStyles");
    assert(styleInfo.type == ImportType::STYLE);
    assert(styleInfo.path == "styles.css");
    assert(styleInfo.hasAsClause);
    assert(styleInfo.asName == "mainStyles");
    
    std::cout << "    @Style导入解析正确\n";
    
    // 测试@JavaScript导入
    std::cout << "  测试@JavaScript导入...\n";
    auto jsInfo = parser.ParseImportStatement("[Import] @JavaScript from \"utils.js\" as utilities");
    assert(jsInfo.type == ImportType::JAVASCRIPT);
    assert(jsInfo.path == "utils.js");
    assert(jsInfo.hasAsClause);
    assert(jsInfo.asName == "utilities");
    
    std::cout << "    @JavaScript导入解析正确\n";
    
    std::cout << "  ✅ Web资源导入规则测试通过\n";
}

// 测试@Chtl导入规则
void test_chtl_import_rules() {
    std::cout << "测试@Chtl导入规则...\n";
    
    ImportParser parser;
    
    // 测试基本@Chtl导入
    std::cout << "  测试基本@Chtl导入...\n";
    auto chtlInfo1 = parser.ParseImportStatement("[Import] @Chtl from \"Chtholly\"");
    assert(chtlInfo1.type == ImportType::CHTL);
    assert(chtlInfo1.path == "Chtholly");
    assert(chtlInfo1.pathType == PathType::NAME_ONLY);
    assert(!chtlInfo1.useOfficialPrefix);
    
    std::cout << "    基本@Chtl导入解析正确\n";
    
    // 测试官方模块前缀
    std::cout << "  测试官方模块前缀...\n";
    auto chtlInfo2 = parser.ParseImportStatement("[Import] @Chtl from \"chtl::Chtholly\"");
    assert(chtlInfo2.type == ImportType::CHTL);
    assert(chtlInfo2.path == "Chtholly");
    assert(chtlInfo2.useOfficialPrefix);
    
    std::cout << "    官方模块前缀解析正确\n";
    
    // 测试通配符导入
    std::cout << "  测试通配符导入...\n";
    auto wildcardInfo1 = parser.ParseImportStatement("[Import] @Chtl from \"modules/*\"");
    assert(wildcardInfo1.type == ImportType::CHTL);
    assert(wildcardInfo1.path == "modules/*");
    assert(wildcardInfo1.pathType == PathType::WILDCARD_ALL);
    
    auto wildcardInfo2 = parser.ParseImportStatement("[Import] @Chtl from \"modules/*.cmod\"");
    assert(wildcardInfo2.type == ImportType::CHTL);
    assert(wildcardInfo2.path == "modules/*.cmod");
    assert(wildcardInfo2.pathType == PathType::WILDCARD_CMOD);
    
    std::cout << "    通配符导入解析正确\n";
    
    // 测试子模块导入
    std::cout << "  测试子模块导入...\n";
    auto submoduleInfo1 = parser.ParseImportStatement("[Import] @Chtl from \"Chtholly.*\"");
    assert(submoduleInfo1.type == ImportType::CHTL);
    assert(submoduleInfo1.path == "Chtholly.*");
    assert(submoduleInfo1.pathType == PathType::SUBMODULE_ALL);
    
    auto submoduleInfo2 = parser.ParseImportStatement("[Import] @Chtl from \"Chtholly.Space\"");
    assert(submoduleInfo2.type == ImportType::CHTL);
    assert(submoduleInfo2.path == "Chtholly.Space");
    assert(submoduleInfo2.pathType == PathType::SUBMODULE_SPECIFIC);
    
    std::cout << "    子模块导入解析正确\n";
    
    std::cout << "  ✅ @Chtl导入规则测试通过\n";
}

// 测试@CJmod导入规则
void test_cjmod_import_rules() {
    std::cout << "测试@CJmod导入规则...\n";
    
    ImportParser parser;
    
    // 测试基本@CJmod导入
    auto cjmodInfo1 = parser.ParseImportStatement("[Import] @CJmod from \"PrintMylove\"");
    assert(cjmodInfo1.type == ImportType::CJMOD);
    assert(cjmodInfo1.path == "PrintMylove");
    assert(cjmodInfo1.pathType == PathType::NAME_ONLY);
    
    std::cout << "  基本@CJmod导入解析正确\n";
    
    // 测试官方CJmod导入
    auto cjmodInfo2 = parser.ParseImportStatement("[Import] @CJmod from \"chtl::INeverAway\"");
    assert(cjmodInfo2.type == ImportType::CJMOD);
    assert(cjmodInfo2.path == "INeverAway");
    assert(cjmodInfo2.useOfficialPrefix);
    
    std::cout << "  官方@CJmod导入解析正确\n";
    
    std::cout << "  ✅ @CJmod导入规则测试通过\n";
}

// 测试[Origin]自定义类型导入
void test_origin_custom_import() {
    std::cout << "测试[Origin]自定义类型导入...\n";
    
    ImportParser parser;
    
    // 测试各种自定义原始嵌入类型
    auto vueInfo = parser.ParseImportStatement("[Import] [Origin] @Vue from \"component.vue\" as VueComponent");
    assert(vueInfo.type == ImportType::ORIGIN_CUSTOM);
    assert(vueInfo.path == "component.vue");
    assert(vueInfo.hasAsClause);
    assert(vueInfo.asName == "VueComponent");
    
    auto reactInfo = parser.ParseImportStatement("[Import] [Origin] @React from \"component.jsx\" as ReactComponent");
    assert(reactInfo.type == ImportType::ORIGIN_CUSTOM);
    assert(reactInfo.path == "component.jsx");
    assert(reactInfo.hasAsClause);
    assert(reactInfo.asName == "ReactComponent");
    
    std::cout << "  [Origin]自定义类型导入解析验证:\n";
    std::cout << "    @Vue导入: " << vueInfo.path << " as " << vueInfo.asName << "\n";
    std::cout << "    @React导入: " << reactInfo.path << " as " << reactInfo.asName << "\n";
    
    std::cout << "  ✅ [Origin]自定义类型导入测试通过\n";
}

// 测试导入规则的边界情况
void test_import_edge_cases() {
    std::cout << "测试导入规则的边界情况...\n";
    
    ImportParser parser;
    
    // 测试不存在的类型
    std::cout << "  测试不存在的类型...\n";
    auto unknownInfo = parser.ParseImportStatement("[Import] @Vue from \"index.vue\"");
    assert(unknownInfo.type == ImportType::UNKNOWN);
    
    std::cout << "    不存在的类型正确识别为UNKNOWN\n";
    
    // 测试目录路径（应该报错）
    std::cout << "  测试目录路径...\n";
    auto dirInfo = parser.ParseImportStatement("[Import] @Html from \"assets/\"");
    assert(dirInfo.type == ImportType::HTML);
    assert(dirInfo.pathType == PathType::DIRECTORY_PATH);
    
    std::cout << "    目录路径正确识别为DIRECTORY_PATH\n";
    
    // 测试空路径
    std::cout << "  测试空路径...\n";
    auto emptyInfo = parser.ParseImportStatement("[Import] @Html from \"\"");
    assert(emptyInfo.type == ImportType::HTML);
    assert(emptyInfo.path.empty());
    
    std::cout << "    空路径处理正确\n";
    
    std::cout << "  ✅ 导入规则边界情况测试通过\n";
}

int main() {
    std::cout << "运行增强Import系统测试...\n\n";
    
    try {
        test_import_type_recognition();
        test_path_type_recognition();
        test_import_statement_parsing();
        test_path_normalization();
        test_conflict_detection();
        test_module_directory_manager();
        test_enhanced_import_system_basic();
        test_web_asset_import_rules();
        test_chtl_import_rules();
        test_cjmod_import_rules();
        test_origin_custom_import();
        test_import_edge_cases();
        
        std::cout << "\n🎉 所有增强Import系统测试通过!\n";
        std::cout << "✅ 导入类型识别正常\n";
        std::cout << "✅ 路径类型识别正常\n";
        std::cout << "✅ 导入语句解析正常\n";
        std::cout << "✅ 路径规范化正常\n";
        std::cout << "✅ 冲突检测正常\n";
        std::cout << "✅ 模块目录结构管理器正常\n";
        std::cout << "✅ 增强导入系统基础功能正常\n";
        std::cout << "✅ Web资源导入规则正常\n";
        std::cout << "✅ @Chtl导入规则正常\n";
        std::cout << "✅ @CJmod导入规则正常\n";
        std::cout << "✅ [Origin]自定义类型导入正常\n";
        std::cout << "✅ 导入规则边界情况正常\n";
        
        std::cout << "\n📋 增强Import系统功能总结:\n";
        std::cout << "🔧 支持的导入类型:\n";
        std::cout << "   @Html - HTML文件导入（需要as子句）\n";
        std::cout << "   @Style - CSS文件导入（需要as子句）\n";
        std::cout << "   @JavaScript - JS文件导入（需要as子句）\n";
        std::cout << "   @Chtl - CHTL模块导入\n";
        std::cout << "   @CJmod - CJmod扩展导入\n";
        std::cout << "   [Origin] @CustomType - 自定义原始嵌入类型导入\n";
        std::cout << "\n🎯 路径解析规则:\n";
        std::cout << "   名称（不带后缀）：按类型搜索相关文件\n";
        std::cout << "   具体文件名（带后缀）：直接搜索该文件\n";
        std::cout << "   具体路径（含文件信息）：按路径查找\n";
        std::cout << "   目录路径（不含文件信息）：触发报错\n";
        std::cout << "\n🔗 搜索优先级:\n";
        std::cout << "   @Chtl: 官方模块目录 → 当前目录module文件夹 → 当前目录\n";
        std::cout << "   @CJmod: 官方模块目录 → 当前目录module文件夹 → 当前目录\n";
        std::cout << "   Web资源: 当前目录（非递归）\n";
        std::cout << "\n✨ 特殊功能:\n";
        std::cout << "   通配符导入：.*、/*、.*.cmod、/*.cmod、.*.chtl、/*.chtl\n";
        std::cout << "   子模块导入：Module.*、Module.SubModule\n";
        std::cout << "   官方模块前缀：chtl::ModuleName\n";
        std::cout << "   循环依赖检测和重复导入检测\n";
        std::cout << "   路径规范化（解决同一路径多表达方式问题）\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}