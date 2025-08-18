#include "CHTLSyntaxComprehensiveTest.h"
#include "../Utils/TestUtils.h"
#include <regex>
#include <fstream>

namespace chtl {
namespace test {
namespace unit {

CHTLSyntaxComprehensiveTest::CHTLSyntaxComprehensiveTest() : TestBase("CHTLSyntaxComprehensive") {
    
    // === 基础语法测试 ===
    TEST_CATEGORY("testComments", "测试注释语法", "BasicSyntax") {
        testComments();
    };
    
    TEST_CATEGORY("testTextNodes", "测试文本节点", "BasicSyntax") {
        testTextNodes();
    };
    
    TEST_CATEGORY("testLiterals", "测试字面量", "BasicSyntax") {
        testLiterals();
    };
    
    TEST_CATEGORY("testCEEquality", "测试CE对等式", "BasicSyntax") {
        testCEEquality();
    };
    
    TEST_CATEGORY("testElementNodes", "测试元素节点", "BasicSyntax") {
        testElementNodes();
    };
    
    // === 样式系统测试 ===
    TEST_CATEGORY("testInlineStyles", "测试内联样式", "StyleSystem") {
        testInlineStyles();
    };
    
    TEST_CATEGORY("testAutoClassId", "测试自动化类名/id", "StyleSystem") {
        testAutoClassId();
    };
    
    TEST_CATEGORY("testContextInference", "测试上下文推导", "StyleSystem") {
        testContextInference();
    };
    
    // === CHTL JS语法测试 ===
    TEST_CATEGORY("testCHTLJSSelectors", "测试CHTL JS选择器", "CHTLJS") {
        testCHTLJSSelectors();
    };
    
    TEST_CATEGORY("testArrowOperator", "测试箭头操作符", "CHTLJS") {
        testArrowOperator();
    };
    
    TEST_CATEGORY("testListenFunction", "测试listen函数", "CHTLJS") {
        testListenFunction();
    };
    
    TEST_CATEGORY("testDelegateFunction", "测试delegate函数", "CHTLJS") {
        testDelegateFunction();
    };
    
    TEST_CATEGORY("testAnimateFunction", "测试animate函数", "CHTLJS") {
        testAnimateFunction();
    };
    
    TEST_PRIORITY("testVirtualObjects", "测试虚拟对象", "CHTLJS", 9) {
        testVirtualObjects();
    };
    
    // === 导入系统测试 ===
    TEST_CATEGORY("testChtlImport", "测试@Chtl导入", "ImportSystem") {
        testChtlImport();
    };
    
    TEST_CATEGORY("testCJmodImport", "测试@CJmod导入", "ImportSystem") {
        testCJmodImport();
    };
    
    TEST_CATEGORY("testWildcardImports", "测试通配符导入", "ImportSystem") {
        testWildcardImports();
    };
    
    TEST_CATEGORY("testSubmoduleImports", "测试子模块导入", "ImportSystem") {
        testSubmoduleImports();
    };
    
    // === 命名空间系统测试 ===
    TEST_CATEGORY("testBasicNamespaces", "测试基础命名空间", "NamespaceSystem") {
        testBasicNamespaces();
    };
    
    TEST_CATEGORY("testNestedNamespaces", "测试嵌套命名空间", "NamespaceSystem") {
        testNestedNamespaces();
    };
    
    // === 模块系统测试 ===
    TEST_PRIORITY("testCMODStructure", "测试CMOD结构", "ModuleSystem", 9) {
        testCMODStructure();
    };
    
    TEST_PRIORITY("testCJMODStructure", "测试CJMOD结构", "ModuleSystem", 9) {
        testCJMODStructure();
    };
    
    TEST_PRIORITY("testChthollyModule", "测试珂朵莉模块", "ModuleSystem", 10) {
        testChthollyModule();
    };
    
    // === 高级特性测试 ===
    TEST_PRIORITY("testINeverAwayFunction", "测试iNeverAway功能", "AdvancedFeatures", 8) {
        testINeverAwayFunction();
    };
    
    TEST_PRIORITY("testPrintMyloveFunction", "测试printMylove功能", "AdvancedFeatures", 8) {
        testPrintMyloveFunction();
    };
}

void CHTLSyntaxComprehensiveTest::setUp() {
    // 测试前准备
}

void CHTLSyntaxComprehensiveTest::tearDown() {
    // 测试后清理
    utils::FileSystemTestUtils::cleanupTempFiles();
}

// === 基础语法测试实现 ===

void CHTLSyntaxComprehensiveTest::testComments() {
    std::string codeWithComments = R"(
// 单行注释
/* 多行注释 */
-- 生成器识别的注释

div
{
    // 这是注释
    text { "Hello World" }
    /* 
     * 多行注释
     * 测试
     */
}
)";
    
    assertTrue(validateCHTLCode(codeWithComments), "包含注释的CHTL代码应该有效");
    
    // 验证注释类型
    assertTrue(codeWithComments.find("//") != std::string::npos, "应该支持单行注释");
    assertTrue(codeWithComments.find("/*") != std::string::npos, "应该支持多行注释");
    assertTrue(codeWithComments.find("--") != std::string::npos, "应该支持生成器注释");
}

void CHTLSyntaxComprehensiveTest::testTextNodes() {
    std::string textNodeCode = R"(
text
{
    "这是一段文本"
}

div
{
    text
    {
        这是无修饰字面量文本
    }
}
)";
    
    assertTrue(validateCHTLCode(textNodeCode), "文本节点代码应该有效");
    
    // 验证text节点语法
    assertTrue(textNodeCode.find("text\n{") != std::string::npos, "应该支持text节点");
}

void CHTLSyntaxComprehensiveTest::testLiterals() {
    std::string literalCode = R"(
div
{
    class: my-class
    id: my-id
    title: "带引号的字符串"
    data-value: '单引号字符串'
    
    text
    {
        无修饰字面量文本
    }
    
    style
    {
        color: red;
        background: blue;
    }
}
)";
    
    assertTrue(validateCHTLCode(literalCode), "字面量代码应该有效");
    
    // 验证不同类型的字面量
    assertTrue(literalCode.find("class: my-class") != std::string::npos, "应该支持无修饰字面量");
    assertTrue(literalCode.find("\"带引号的字符串\"") != std::string::npos, "应该支持双引号字符串");
    assertTrue(literalCode.find("'单引号字符串'") != std::string::npos, "应该支持单引号字符串");
}

void CHTLSyntaxComprehensiveTest::testCEEquality() {
    std::string ceCode = R"(
div
{
    class: my-class     // 使用冒号
    id = my-id          // 使用等号
    
    style
    {
        color: red;     // CSS中使用冒号
        width = 100px;  // 也可以使用等号
    }
}
)";
    
    assertTrue(validateCHTLCode(ceCode), "CE对等式代码应该有效");
    
    // 验证冒号和等号都被支持
    assertTrue(ceCode.find("class:") != std::string::npos, "应该支持冒号语法");
    assertTrue(ceCode.find("id =") != std::string::npos, "应该支持等号语法");
}

void CHTLSyntaxComprehensiveTest::testElementNodes() {
    std::string elementCode = R"(
html
{
    head
    {
        title { "CHTL测试页面" }
        meta { charset: "utf-8" }
    }
    
    body
    {
        header
        {
            h1 { "标题" }
            nav
            {
                ul
                {
                    li { "首页" }
                    li { "关于" }
                }
            }
        }
        
        main
        {
            section
            {
                article
                {
                    h2 { "文章标题" }
                    p { "文章内容" }
                }
            }
        }
        
        footer
        {
            p { "版权信息" }
        }
    }
}
)";
    
    assertTrue(validateCHTLCode(elementCode), "HTML元素代码应该有效");
    
    // 验证各种HTML元素
    std::vector<std::string> elements = {"html", "head", "body", "header", "main", "footer", "nav", "section", "article"};
    for (const auto& element : elements) {
        assertTrue(elementCode.find(element) != std::string::npos, "应该支持" + element + "元素");
    }
}

// === 样式系统测试实现 ===

void CHTLSyntaxComprehensiveTest::testInlineStyles() {
    std::string inlineStyleCode = R"(
div
{
    style
    {
        width: 100px;
        height: 200px;
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        border-radius: 10px;
        box-shadow: 0 4px 15px rgba(0,0,0,0.1);
    }
    
    text { "带内联样式的div" }
}
)";
    
    assertTrue(validateCHTLCode(inlineStyleCode), "内联样式代码应该有效");
    assertTrue(inlineStyleCode.find("style\n    {") != std::string::npos, "应该支持内联样式块");
}

void CHTLSyntaxComprehensiveTest::testAutoClassId() {
    std::string autoClassCode = R"(
div
{
    style
    {
        .box  // 自动添加类名box
        {
            width: 300px;
            height: 300px;
        }
        
        #container  // 自动添加id container
        {
            padding: 20px;
        }
    }
}
)";
    
    assertTrue(validateCHTLCode(autoClassCode), "自动类名/id代码应该有效");
    assertTrue(autoClassCode.find(".box") != std::string::npos, "应该支持自动类名");
    assertTrue(autoClassCode.find("#container") != std::string::npos, "应该支持自动id");
}

void CHTLSyntaxComprehensiveTest::testContextInference() {
    std::string contextCode = R"(
div
{
    class: "box"
    
    style
    {
        .box
        {
            width: 200px;
        }
        
        &:hover  // 上下文推导为 .box:hover
        {
            transform: scale(1.1);
        }
        
        &::before  // 上下文推导为 .box::before
        {
            content: "";
        }
    }
}
)";
    
    assertTrue(validateCHTLCode(contextCode), "上下文推导代码应该有效");
    assertTrue(contextCode.find("&:hover") != std::string::npos, "应该支持&伪类语法");
    assertTrue(contextCode.find("&::before") != std::string::npos, "应该支持&伪元素语法");
}

// === CHTL JS语法测试实现 ===

void CHTLSyntaxComprehensiveTest::testCHTLJSSelectors() {
    std::string selectorCode = R"(
{{.button}}
    Click me
{{/.button}}

{{#main-container}}
    {{.sidebar}}
        Sidebar content
    {{/.sidebar}}
    
    {{.content}}
        Main content
    {{/.content}}
{{/#main-container}}
)";
    
    assertTrue(validateCHTLJSCode(selectorCode), "CHTL JS选择器代码应该有效");
    assertTrue(selectorCode.find("{{.button}}") != std::string::npos, "应该支持类选择器");
    assertTrue(selectorCode.find("{{#main-container}}") != std::string::npos, "应该支持id选择器");
}

void CHTLSyntaxComprehensiveTest::testArrowOperator() {
    std::string arrowCode = R"(
{{.button -> click}}
    点击按钮
{{/.button}}

{{#menu -> hover}}
    菜单项
{{/menu}}

{{.form-input -> focus}}
    输入框
{{/.form-input}}
)";
    
    assertTrue(validateCHTLJSCode(arrowCode), "箭头操作符代码应该有效");
    assertTrue(arrowCode.find("-> click") != std::string::npos, "应该支持click事件");
    assertTrue(arrowCode.find("-> hover") != std::string::npos, "应该支持hover事件");
    assertTrue(arrowCode.find("-> focus") != std::string::npos, "应该支持focus事件");
}

void CHTLSyntaxComprehensiveTest::testListenFunction() {
    std::string listenCode = R"(
{{.button -> listen}}
    event: click,
    handler: function(e) {
        console.log('Button clicked');
    },
    capture: false
{{/.button}}

{{#menu -> listen}}
    event: mouseenter,
    handler: function(e) {
        showSubmenu(e.target);
    }
{{/menu}}
)";
    
    assertTrue(validateCHTLJSCode(listenCode), "listen函数代码应该有效");
    assertTrue(listenCode.find("-> listen") != std::string::npos, "应该支持listen函数");
    assertTrue(listenCode.find("event:") != std::string::npos, "应该支持event参数");
    assertTrue(listenCode.find("handler:") != std::string::npos, "应该支持handler参数");
}

void CHTLSyntaxComprehensiveTest::testDelegateFunction() {
    std::string delegateCode = R"(
{{#container -> delegate}}
    target: '.button',
    event: click,
    handler: function(e) {
        handleButtonClick(e);
    }
{{/container}}

{{.list -> delegate}}
    target: 'li',
    event: hover,
    handler: function(e) {
        highlightItem(e.target);
    }
{{/.list}}
)";
    
    assertTrue(validateCHTLJSCode(delegateCode), "delegate函数代码应该有效");
    assertTrue(delegateCode.find("-> delegate") != std::string::npos, "应该支持delegate函数");
    assertTrue(delegateCode.find("target:") != std::string::npos, "应该支持target参数");
}

void CHTLSyntaxComprehensiveTest::testAnimateFunction() {
    std::string animateCode = R"(
{{.box -> animate}}
    properties: {
        opacity: 1,
        transform: 'translateY(0)'
    },
    duration: 300,
    easing: 'ease-in-out',
    delay: 0
{{/.box}}

{{#modal -> animate}}
    properties: {
        scale: 1,
        opacity: 1
    },
    duration: 500
{{/modal}}
)";
    
    assertTrue(validateCHTLJSCode(animateCode), "animate函数代码应该有效");
    assertTrue(animateCode.find("-> animate") != std::string::npos, "应该支持animate函数");
    assertTrue(animateCode.find("properties:") != std::string::npos, "应该支持properties参数");
    assertTrue(animateCode.find("duration:") != std::string::npos, "应该支持duration参数");
}

void CHTLSyntaxComprehensiveTest::testVirtualObjects() {
    std::string virCode = R"(
{{.component -> myFunction}}
    param1: value1,
    param2: vir.helperFunction,
    param3: {
        nested: vir.anotherFunction
    }
{{/.component}}

// 虚拟对象优化测试
{{.optimized -> testFunction}}
    directValue: "直接值",
    functionRef: vir.externalFunction
{{/.optimized}}
)";
    
    assertTrue(validateCHTLJSCode(virCode), "虚拟对象代码应该有效");
    assertTrue(virCode.find("vir.helperFunction") != std::string::npos, "应该支持vir对象引用");
    assertTrue(virCode.find("vir.anotherFunction") != std::string::npos, "应该支持嵌套vir引用");
}

// === 导入系统测试实现 ===

void CHTLSyntaxComprehensiveTest::testChtlImport() {
    std::vector<std::string> validChtlImports = {
        "@Chtl from ModuleName",                    // 名称（不带后缀）
        "@Chtl from ModuleName.cmod",               // 具体名称（带后缀）
        "@Chtl from ./path/to/module.chtl",        // 具体路径（含文件信息）
        "@Chtl from Chtholly.Accordion",           // 子模块导入
        "@Chtl from Yuigahama.MusicPlayer"         // 子模块导入
    };
    
    for (const auto& import : validChtlImports) {
        assertTrue(validateImportStatement(import), "有效的@Chtl导入应该通过: " + import);
    }
    
    std::vector<std::string> invalidChtlImports = {
        "@Chtl from ./folder/",                    // 具体路径（不含文件信息）
        "@Chtl from",                              // 缺少路径
        "Chtl from ModuleName"                     // 缺少@符号
    };
    
    for (const auto& import : invalidChtlImports) {
        assertFalse(validateImportStatement(import), "无效的@Chtl导入应该失败: " + import);
    }
}

void CHTLSyntaxComprehensiveTest::testCJmodImport() {
    std::vector<std::string> validCJmodImports = {
        "@CJmod from ModuleName",                   // 名称（不带后缀）
        "@CJmod from ModuleName.cjmod",             // 具体名称（带后缀）
        "@CJmod from ./path/to/module.cjmod"        // 具体路径（含文件信息）
    };
    
    for (const auto& import : validCJmodImports) {
        assertTrue(validateImportStatement(import), "有效的@CJmod导入应该通过: " + import);
    }
}

void CHTLSyntaxComprehensiveTest::testWildcardImports() {
    std::vector<std::string> wildcardImports = {
        "@Chtl from ./path/*",                      // /* 通配符
        "@Chtl from ./path/*.cmod",                 // /*.cmod 通配符
        "@Chtl from ./path/*.chtl",                 // /*.chtl 通配符
        "@Chtl from ./path.*",                      // .* 通配符
        "@Chtl from ./path.*.cmod",                 // .*.cmod 通配符
        "@Chtl from ./path.*.chtl"                  // .*.chtl 通配符
    };
    
    for (const auto& import : wildcardImports) {
        assertTrue(validateImportStatement(import), "通配符导入应该有效: " + import);
    }
}

void CHTLSyntaxComprehensiveTest::testSubmoduleImports() {
    std::vector<std::string> submoduleImports = {
        "@Chtl from Chtholly.*",                   // 导入所有子模块
        "@Chtl from Chtholly.Space",               // 导入指定子模块
        "@Chtl from Yuigahama.*",                  // 导入所有子模块
        "@Chtl from Yuigahama/MusicPlayer",        // 使用/分隔符
        "@Chtl from Module.SubModule.Component"    // 多级子模块
    };
    
    for (const auto& import : submoduleImports) {
        assertTrue(validateImportStatement(import), "子模块导入应该有效: " + import);
    }
}

// === 命名空间系统测试实现 ===

void CHTLSyntaxComprehensiveTest::testBasicNamespaces() {
    std::string namespaceCode = R"(
[Namespace] MyNamespace

[Custom] @Element Box
{
    div
    {
        style
        {
            width: 100px;
            height: 100px;
        }
    }
}
)";
    
    assertTrue(validateCHTLCode(namespaceCode), "基础命名空间代码应该有效");
    assertTrue(namespaceCode.find("[Namespace] MyNamespace") != std::string::npos, "应该支持命名空间声明");
}

void CHTLSyntaxComprehensiveTest::testNestedNamespaces() {
    std::string nestedCode = R"(
[Namespace] space
{
    [Namespace] room
    
    [Custom] @Element Box
    {
        div
        {
            style
            {
                color: red;
            }
        }
    }
}

[Namespace] space2
{
    [Namespace] room1
    {
        // 嵌套命名空间内容
    }
    
    [Namespace] room2
    {
        [Custom] @Element Card
        {
            div
            {
                style
                {
                    background: blue;
                }
            }
        }
    }
}
)";
    
    assertTrue(validateCHTLCode(nestedCode), "嵌套命名空间代码应该有效");
    assertTrue(nestedCode.find("[Namespace] room") != std::string::npos, "应该支持嵌套命名空间");
}

// === 模块系统测试实现 ===

void CHTLSyntaxComprehensiveTest::testCMODStructure() {
    // 创建测试CMOD
    std::string cmodPath = utils::FileSystemTestUtils::createTestCMOD("TestCMOD");
    
    // 验证CMOD结构
    assertTrue(utils::CHTLTestUtils::validateCMODStructure(cmodPath), "CMOD结构应该有效");
    
    // 验证三者同名
    std::string moduleName = std::filesystem::path(cmodPath).filename().string();
    std::string srcFile = cmodPath + "/src/" + moduleName + ".chtl";
    std::string infoFile = cmodPath + "/info/" + moduleName + ".chtl";
    
    assertTrue(utils::FileSystemTestUtils::fileExists(srcFile), "源文件应该存在");
    assertTrue(utils::FileSystemTestUtils::fileExists(infoFile), "信息文件应该存在");
    
    // 验证info文件内容
    std::string infoContent = utils::FileSystemTestUtils::readFile(infoFile);
    assertTrue(infoContent.find("[Info]") != std::string::npos, "info文件应该包含[Info]段落");
    assertTrue(infoContent.find("[Export]") != std::string::npos, "info文件应该包含[Export]段落");
}

void CHTLSyntaxComprehensiveTest::testCJMODStructure() {
    // 创建测试CJMOD结构
    std::string tempDir = utils::FileSystemTestUtils::createTempDirectory();
    std::string cjmodPath = tempDir + "/TestCJMOD";
    
    // 创建CJMOD目录结构（二者同名：模块名和信息文件相同）
    std::filesystem::create_directories(cjmodPath + "/src");
    
    // 创建info文件（注意：CJMOD的info文件没有[Export]段落）
    std::string infoContent = R"([Info]
name = "Test CJMOD";
version = "1.0.0";
description = "测试CJMOD模块";
author = "Test";
license = "MIT";
dependencies = "";
category = "test";
minCHTLVersion = "1.0.0";
maxCHTLVersion = "2.0.0";
)";
    
    utils::FileSystemTestUtils::writeFile(cjmodPath + "/TestCJMOD.chtl", infoContent);
    
    // 创建C++源文件
    utils::FileSystemTestUtils::writeFile(cjmodPath + "/src/test.cpp", "// Test C++ source");
    utils::FileSystemTestUtils::writeFile(cjmodPath + "/src/test.h", "// Test C++ header");
    
    assertTrue(utils::CHTLTestUtils::validateCJMODStructure(cjmodPath), "CJMOD结构应该有效");
    
    // 验证CJMOD特点：info文件没有[Export]段落
    assertFalse(infoContent.find("[Export]") != std::string::npos, "CJMOD的info文件不应该有[Export]段落");
}

void CHTLSyntaxComprehensiveTest::testChthollyModule() {
    std::string chthollyImportCode = R"(
[Import]
@Chtl from Chtholly.Accordion
@CJmod from Chtholly

{{ChthollyAccordion}}
    {{.accordion-elegant}}
        珂朵莉的优雅手风琴
    {{/.accordion-elegant}}
{{/ChthollyAccordion}}

// 测试珂朵莉CJMOD特性
{{.image -> printMylove}}
    url: "chtholly.jpg",
    mode: "ASCII",
    width: 80,
    height: 40,
    scale: 1.0
{{/.image}}
)";
    
    assertTrue(validateCHTLCode(chthollyImportCode), "珂朵莉模块代码应该有效");
    assertTrue(chthollyImportCode.find("Chtholly.Accordion") != std::string::npos, "应该支持珂朵莉CMOD导入");
    assertTrue(chthollyImportCode.find("@CJmod from Chtholly") != std::string::npos, "应该支持珂朵莉CJMOD导入");
    assertTrue(chthollyImportCode.find("printMylove") != std::string::npos, "应该支持printMylove功能");
}

// === 高级特性测试实现 ===

void CHTLSyntaxComprehensiveTest::testINeverAwayFunction() {
    std::string iNeverAwayCode = R"(
// iNeverAway创建标记函数组
vir Test = iNeverAway({
    Void<A>: function(int, int) {
        // 状态A的函数实现
    },
    
    Void<B>: function(int, int) {
        // 状态B的函数实现，同名同参重载
    },
    
    Process<Init>: function(string) {
        // 初始化处理函数
    }
});

// 使用虚拟对象调用
Test->Void<A>(10, 20);
Test->Void<B>(30, 40);
Test->Process<Init>("initialize");
)";
    
    assertTrue(validateCHTLJSCode(iNeverAwayCode), "iNeverAway代码应该有效");
    assertTrue(iNeverAwayCode.find("vir Test = iNeverAway") != std::string::npos, "应该支持iNeverAway语法");
    assertTrue(iNeverAwayCode.find("Void<A>") != std::string::npos, "应该支持状态标记");
    assertTrue(iNeverAwayCode.find("Test->Void<A>") != std::string::npos, "应该支持虚拟对象调用");
}

void CHTLSyntaxComprehensiveTest::testPrintMyloveFunction() {
    std::string printMyloveCode = R"(
const asciiArt = printMylove({
    url: "https://example.com/image.jpg",
    mode: "ASCII",
    width: 80,
    height: 40,
    scale: 1.0
});

const pixelArt = printMylove({
    url: "./local/image.png",
    mode: "Pixel",
    width: "50%",
    height: "auto",
    scale: 0.8
});

console.log(asciiArt);
console.log(pixelArt);
)";
    
    assertTrue(validateCHTLJSCode(printMyloveCode), "printMylove代码应该有效");
    assertTrue(printMyloveCode.find("printMylove({") != std::string::npos, "应该支持printMylove函数");
    assertTrue(printMyloveCode.find("mode: \"ASCII\"") != std::string::npos, "应该支持ASCII模式");
    assertTrue(printMyloveCode.find("mode: \"Pixel\"") != std::string::npos, "应该支持Pixel模式");
}

// === 辅助方法实现 ===

bool CHTLSyntaxComprehensiveTest::validateCHTLCode(const std::string& code) {
    return utils::CHTLTestUtils::validateCHTLSyntax(code);
}

bool CHTLSyntaxComprehensiveTest::validateCHTLJSCode(const std::string& code) {
    return utils::CHTLTestUtils::validateCHTLJSSyntax(code);
}

bool CHTLSyntaxComprehensiveTest::validateImportStatement(const std::string& import) {
    return utils::CHTLTestUtils::validateImportStatement(import);
}

std::string CHTLSyntaxComprehensiveTest::createTestCHTLFile(const std::string& content) {
    return utils::FileSystemTestUtils::createTempFile(content);
}

} // namespace unit
} // namespace test
} // namespace chtl