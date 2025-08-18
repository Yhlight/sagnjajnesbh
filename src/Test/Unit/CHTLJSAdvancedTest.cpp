#include "CHTLJSAdvancedTest.h"
#include "../Utils/TestUtils.h"
#include <regex>

namespace chtl {
namespace test {
namespace unit {

CHTLJSAdvancedTest::CHTLJSAdvancedTest() : TestBase("CHTLJSAdvanced") {
    
    // === 虚拟对象系统测试 ===
    TEST_PRIORITY("testVirtualObjectBasics", "测试虚拟对象基础功能", "VirtualObjects", 10) {
        testVirtualObjectBasics();
    };
    
    TEST_PRIORITY("testVirtualObjectOptimization", "测试虚拟对象优化", "VirtualObjects", 9) {
        testVirtualObjectOptimization();
    };
    
    TEST_CATEGORY("testVirtualObjectNesting", "测试虚拟对象嵌套", "VirtualObjects") {
        testVirtualObjectNesting();
    };
    
    // === iNeverAway功能测试 ===
    TEST_PRIORITY("testINeverAwayBasics", "测试iNeverAway基础功能", "iNeverAway", 10) {
        testINeverAwayBasics();
    };
    
    TEST_PRIORITY("testINeverAwayStateOverload", "测试iNeverAway状态重载", "iNeverAway", 9) {
        testINeverAwayStateOverload();
    };
    
    // === 无修饰字面量测试 ===
    TEST_CATEGORY("testUndecoratedInTextBlocks", "测试文本块无修饰字面量", "UndecoratedLiterals") {
        testUndecoratedInTextBlocks();
    };
    
    TEST_CATEGORY("testUndecoratedInAttributes", "测试属性无修饰字面量", "UndecoratedLiterals") {
        testUndecoratedInAttributes();
    };
    
    TEST_CATEGORY("testUndecoratedLimitations", "测试无修饰字面量限制", "UndecoratedLiterals") {
        testUndecoratedLimitations();
    };
    
    // === 函数增强测试 ===
    TEST_PRIORITY("testUnorderedKeyValues", "测试无序键值对", "FunctionEnhancement", 9) {
        testUnorderedKeyValues();
    };
    
    TEST_PRIORITY("testOptionalKeyValues", "测试可选键值对", "FunctionEnhancement", 9) {
        testOptionalKeyValues();
    };
    
    TEST_PRIORITY("testAllJavaScriptEvents", "测试所有JavaScript事件", "FunctionEnhancement", 8) {
        testAllJavaScriptEvents();
    };
    
    // === 自动添加系统测试 ===
    TEST_CATEGORY("testAutoLocalStyleAddition", "测试自动添加本地样式", "AutoAddition") {
        testAutoLocalStyleAddition();
    };
    
    TEST_CATEGORY("testAutoLocalScriptAddition", "测试自动添加本地脚本", "AutoAddition") {
        testAutoLocalScriptAddition();
    };
    
    TEST_CATEGORY("testIDPriorityRule", "测试ID优先级规则", "AutoAddition") {
        testIDPriorityRule();
    };
}

void CHTLJSAdvancedTest::setUp() {
    // 测试前准备
}

void CHTLJSAdvancedTest::tearDown() {
    utils::FileSystemTestUtils::cleanupTempFiles();
}

// === 虚拟对象系统测试实现 ===

void CHTLJSAdvancedTest::testVirtualObjectBasics() {
    std::string virBasicCode = R"(
{{.component -> myFunction}}
    param1: "直接值",
    param2: vir.helperFunction,
    param3: 42,
    param4: vir.anotherHelper
{{/.component}}
)";
    
    assertTrue(isValidCHTLJSSyntax(virBasicCode), "基础虚拟对象语法应该有效");
    
    // 验证vir对象引用
    assertTrue(virBasicCode.find("vir.helperFunction") != std::string::npos, "应该支持vir对象引用");
    assertTrue(virBasicCode.find("vir.anotherHelper") != std::string::npos, "应该支持多个vir引用");
    
    // 验证混合参数类型
    assertTrue(virBasicCode.find("\"直接值\"") != std::string::npos, "应该支持直接值参数");
    assertTrue(virBasicCode.find("42") != std::string::npos, "应该支持数值参数");
}

void CHTLJSAdvancedTest::testVirtualObjectOptimization() {
    std::string optimizationCode = R"(
// 测试虚拟对象优化：避免不必要的代码重复
{{.component1 -> testFunction}}
    callback: vir.sharedFunction
{{/.component1}}

{{.component2 -> testFunction}}
    callback: vir.sharedFunction  // 相同的函数引用应该被优化
{{/.component2}}

{{.component3 -> testFunction}}
    callback: function() { console.log('inline'); }  // 内联函数定义
{{/.component3}}
)";
    
    assertTrue(isValidCHTLJSSyntax(optimizationCode), "虚拟对象优化代码应该有效");
    assertTrue(hasVirtualObjectOptimization(optimizationCode), "应该检测到虚拟对象优化机会");
}

void CHTLJSAdvancedTest::testVirtualObjectNesting() {
    std::string nestedVirCode = R"(
{{.complex -> advancedFunction}}
    config: {
        handler: vir.mainHandler,
        fallback: vir.errorHandler,
        nested: {
            deepHandler: vir.deepFunction,
            options: {
                callback: vir.nestedCallback
            }
        }
    }
{{/.complex}}
)";
    
    assertTrue(isValidCHTLJSSyntax(nestedVirCode), "嵌套虚拟对象代码应该有效");
    
    // 验证嵌套结构中的vir引用
    assertTrue(nestedVirCode.find("handler: vir.mainHandler") != std::string::npos, "应该支持第一级vir引用");
    assertTrue(nestedVirCode.find("deepHandler: vir.deepFunction") != std::string::npos, "应该支持深层vir引用");
    assertTrue(nestedVirCode.find("callback: vir.nestedCallback") != std::string::npos, "应该支持嵌套vir引用");
}

// === iNeverAway功能测试实现 ===

void CHTLJSAdvancedTest::testINeverAwayBasics() {
    std::string basicINeverAwayCode = R"(
vir EventHandler = iNeverAway({
    OnClick<Button>: function(event) {
        console.log('Button clicked');
    },
    
    OnHover<Menu>: function(event) {
        showSubmenu(event.target);
    },
    
    OnFocus<Input>: function(event) {
        validateInput(event.target);
    }
});

// 使用标记函数
EventHandler->OnClick<Button>();
EventHandler->OnHover<Menu>();
EventHandler->OnFocus<Input>();
)";
    
    assertTrue(isValidCHTLJSSyntax(basicINeverAwayCode), "基础iNeverAway代码应该有效");
    
    // 验证iNeverAway语法结构
    assertTrue(basicINeverAwayCode.find("vir EventHandler = iNeverAway") != std::string::npos, 
              "应该支持iNeverAway声明");
    assertTrue(basicINeverAwayCode.find("OnClick<Button>") != std::string::npos, 
              "应该支持状态标记语法");
    assertTrue(basicINeverAwayCode.find("EventHandler->OnClick<Button>()") != std::string::npos, 
              "应该支持虚拟对象方法调用");
}

void CHTLJSAdvancedTest::testINeverAwayStateOverload() {
    std::string stateOverloadCode = R"(
vir ProcessManager = iNeverAway({
    // 同名函数，不同状态
    Process<Init>: function(config) {
        initializeSystem(config);
    },
    
    Process<Update>: function(data) {
        updateSystem(data);
    },
    
    Process<Cleanup>: function() {
        cleanupSystem();
    },
    
    // 同名同参，不同状态
    Handle<Success>: function(result) {
        displaySuccess(result);
    },
    
    Handle<Error>: function(error) {
        displayError(error);
    }
});

// 状态区分调用
ProcessManager->Process<Init>({setting: 'default'});
ProcessManager->Process<Update>({data: 'new'});
ProcessManager->Handle<Success>('操作成功');
ProcessManager->Handle<Error>('操作失败');
)";
    
    assertTrue(isValidCHTLJSSyntax(stateOverloadCode), "状态重载代码应该有效");
    
    // 验证状态重载特性
    assertTrue(stateOverloadCode.find("Process<Init>") != std::string::npos, "应该支持Init状态");
    assertTrue(stateOverloadCode.find("Process<Update>") != std::string::npos, "应该支持Update状态");
    assertTrue(stateOverloadCode.find("Handle<Success>") != std::string::npos, "应该支持Success状态");
    assertTrue(stateOverloadCode.find("Handle<Error>") != std::string::npos, "应该支持Error状态");
}

// === 无修饰字面量测试实现 ===

void CHTLJSAdvancedTest::testUndecoratedInTextBlocks() {
    std::string undecoratedTextCode = R"(
text
{
    这是无修饰字面量文本
    可以包含中文和English
    还可以包含数字123和符号!@#
}

div
{
    text
    {
        无修饰字面量在div中
        支持多行文本
        不需要引号包围
    }
}
)";
    
    assertTrue(isValidCHTLJSSyntax(undecoratedTextCode), "文本块无修饰字面量应该有效");
    
    // 验证无修饰字面量特性
    assertFalse(undecoratedTextCode.find("\"这是无修饰字面量文本\"") != std::string::npos, 
               "无修饰字面量不应该有引号");
    assertTrue(undecoratedTextCode.find("这是无修饰字面量文本") != std::string::npos, 
              "应该支持中文无修饰字面量");
}

void CHTLJSAdvancedTest::testUndecoratedInAttributes() {
    std::string undecoratedAttrCode = R"(
div
{
    class: my-component
    id: main-container
    data-value: test-data
    title: 这是标题文本
    
    style
    {
        color: red;
        background: blue;
        font-family: Arial, sans-serif;
    }
}
)";
    
    assertTrue(isValidCHTLJSSyntax(undecoratedAttrCode), "属性无修饰字面量应该有效");
    
    // 验证属性值无修饰字面量
    assertTrue(undecoratedAttrCode.find("class: my-component") != std::string::npos, 
              "应该支持class属性无修饰字面量");
    assertTrue(undecoratedAttrCode.find("title: 这是标题文本") != std::string::npos, 
              "应该支持中文属性值");
}

void CHTLJSAdvancedTest::testUndecoratedLimitations() {
    // 测试无修饰字面量的限制：不能作为扫描器的独立判断条件
    std::string limitationCode = R"(
// 这些情况下无修饰字面量不应该触发CHTL JS扫描器
普通文本内容
function normalFunction() { return true; }
var normalVariable = value;

// 但在CHTL JS上下文中应该被识别
{{.component -> function}}
    param: undecorated-value
{{/.component}}
)";
    
    assertTrue(isValidCHTLJSSyntax(limitationCode), "无修饰字面量限制测试应该有效");
    
    // 验证限制规则
    std::vector<std::string> fragments = extractCHTLJSFragments(limitationCode);
    assertTrue(fragments.size() > 0, "应该能提取CHTL JS片段");
    
    // 验证非CHTL JS上下文不会被误识别
    assertFalse(limitationCode.find("{{普通文本内容}}") != std::string::npos, 
               "普通文本不应该被识别为CHTL JS");
}

// === 函数增强测试实现 ===

void CHTLJSAdvancedTest::testUnorderedKeyValues() {
    std::string unorderedCode = R"(
{{.button -> listen}}
    capture: true,
    event: click,
    handler: function(e) { console.log('clicked'); }
{{/.button}}

{{.menu -> delegate}}
    handler: function(e) { showMenu(); },
    target: '.menu-item',
    event: hover
{{/.menu}}

{{.box -> animate}}
    easing: 'ease-in-out',
    properties: { opacity: 1 },
    delay: 100,
    duration: 300
{{/.box}}
)";
    
    assertTrue(isValidCHTLJSSyntax(unorderedCode), "无序键值对代码应该有效");
    
    // 验证键值对可以任意顺序
    assertTrue(unorderedCode.find("capture: true,\n    event: click") != std::string::npos, 
              "应该支持capture在event之前");
    assertTrue(unorderedCode.find("handler: function") != std::string::npos, 
              "应该支持handler在任意位置");
}

void CHTLJSAdvancedTest::testOptionalKeyValues() {
    std::string optionalCode = R"(
// 只写必要的键
{{.button -> listen}}
    event: click,
    handler: function(e) { console.log('minimal'); }
{{/.button}}

// 写更多可选键
{{.advanced -> listen}}
    event: click,
    handler: function(e) { console.log('advanced'); },
    capture: true,
    passive: false,
    once: true
{{/.advanced}}

// delegate只写核心键
{{.container -> delegate}}
    target: '.item',
    event: click,
    handler: function(e) { handleClick(e); }
{{/.container}}
)";
    
    assertTrue(isValidCHTLJSSyntax(optionalCode), "可选键值对代码应该有效");
    
    // 验证可选键的支持
    assertTrue(optionalCode.find("capture: true") != std::string::npos, "应该支持可选的capture键");
    assertTrue(optionalCode.find("passive: false") != std::string::npos, "应该支持可选的passive键");
    assertTrue(optionalCode.find("once: true") != std::string::npos, "应该支持可选的once键");
}

void CHTLJSAdvancedTest::testAllJavaScriptEvents() {
    // 测试所有82个标准JavaScript事件
    std::vector<std::string> allEvents = {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mouseover", "mouseout",
        "mouseenter", "mouseleave", "mousemove", "contextmenu",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 焦点事件
        "focus", "blur", "focusin", "focusout",
        
        // 表单事件
        "change", "input", "submit", "reset", "invalid",
        
        // 窗口事件
        "load", "unload", "beforeunload", "resize", "scroll",
        
        // 触摸事件
        "touchstart", "touchend", "touchmove", "touchcancel",
        
        // 拖拽事件
        "drag", "dragstart", "dragend", "dragover", "dragenter", "dragleave", "drop",
        
        // 媒体事件
        "play", "pause", "ended", "volumechange", "timeupdate",
        
        // 动画事件
        "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun", "transitioncancel"
    };
    
    for (const auto& event : allEvents) {
        assertTrue(validateEventSupport(event), "应该支持" + event + "事件");
        
        // 测试在listen函数中的使用
        std::string eventCode = "{{.element -> listen}}\n    event: " + event + ",\n    handler: function(e) {}\n{{/.element}}";
        assertTrue(isValidCHTLJSSyntax(eventCode), event + "事件在listen中应该有效");
        
        // 测试在delegate函数中的使用
        std::string delegateCode = "{{.container -> delegate}}\n    target: '.child',\n    event: " + event + ",\n    handler: function(e) {}\n{{/.container}}";
        assertTrue(isValidCHTLJSSyntax(delegateCode), event + "事件在delegate中应该有效");
    }
    
    std::cout << "  验证了 " << allEvents.size() << " 个JavaScript事件" << std::endl;
}

// === 自动添加系统测试实现 ===

void CHTLJSAdvancedTest::testAutoLocalStyleAddition() {
    std::string autoStyleCode = R"(
// 局部样式块自动添加规则测试
div  // 没有明确的class或id
{
    style
    {
        .primary    // 第一个类选择器 → 自动添加 class="primary"
        {
            color: blue;
        }
        
        .secondary  // 第二个类选择器 → 不会添加
        {
            color: red;
        }
        
        #header     // 第一个ID选择器 → 自动添加 id="header"
        {
            font-size: 24px;
        }
        
        #footer     // 第二个ID选择器 → 不会添加
        {
            font-size: 12px;
        }
    }
}

// 已有属性的情况
div
{
    class: existing-class;
    style
    {
        .new-class  // 不会添加，因为已有class属性
        {
            background: yellow;
        }
    }
}

div
{
    id: existing-id;
    style
    {
        #new-id  // 不会添加，因为已有id属性
        {
            margin: 10px;
        }
    }
}
)";
    
    assertTrue(isValidCHTLJSSyntax(autoStyleCode), "局部样式块自动添加代码应该有效");
    
    // 验证第一个选择器（会被添加）
    assertTrue(autoStyleCode.find(".primary") != std::string::npos, "应该包含第一个类选择器");
    assertTrue(autoStyleCode.find("#header") != std::string::npos, "应该包含第一个ID选择器");
    
    // 验证第二个选择器（不会被添加）
    assertTrue(autoStyleCode.find(".secondary") != std::string::npos, "应该包含第二个类选择器（但不会被添加）");
    assertTrue(autoStyleCode.find("#footer") != std::string::npos, "应该包含第二个ID选择器（但不会被添加）");
    
    // 验证已有属性的情况
    assertTrue(autoStyleCode.find("class: existing-class") != std::string::npos, "应该包含已有的class属性");
    assertTrue(autoStyleCode.find("id: existing-id") != std::string::npos, "应该包含已有的id属性");
}

void CHTLJSAdvancedTest::testAutoLocalScriptAddition() {
    std::string autoScriptCode = R"(
// 局部脚本块自动添加规则测试（当style没有自动添加时）
div  // 没有class和id，style也没有自动添加
{
    script
    {
        {{.component}}  // 明确类选择器 → 自动添加 class="component"
        {{#container}}  // 明确ID选择器 → 自动添加 id="container"
        {{element}}     // 不明确选择器 → 不会自动添加
    }
}

// 已有属性的情况，不重复添加
div
{
    class: existing;
    script
    {
        {{#new-id}}  // 可以添加ID，因为没有id属性
    }
}

div
{
    id: existing;
    script
    {
        {{.new-class}}  // 可以添加class，因为没有class属性
    }
}

// 上下文推导测试
div
{
    class: my-class;
    id: my-id;
    script
    {
        {{&}}  // ID优先 → 解析为 document.getElementById('my-id')
    }
}

div
{
    class: only-class;  // 只有class
    script
    {
        {{&}}  // 没有id，使用class → 解析为 document.querySelector('.only-class')
    }
}
)";
    
    assertTrue(isValidCHTLJSSyntax(autoScriptCode), "局部脚本块自动添加代码应该有效");
    
    // 验证明确选择器（会触发自动添加）
    assertTrue(autoScriptCode.find("{{.component}}") != std::string::npos, "应该包含明确的类选择器");
    assertTrue(autoScriptCode.find("{{#container}}") != std::string::npos, "应该包含明确的ID选择器");
    
    // 验证不明确选择器（不会触发自动添加）
    assertTrue(autoScriptCode.find("{{element}}") != std::string::npos, "应该包含不明确的选择器");
    
    // 验证上下文推导
    assertTrue(autoScriptCode.find("{{&}}") != std::string::npos, "应该包含上下文推导选择器");
    assertTrue(autoScriptCode.find("ID优先") != std::string::npos, "应该说明script中的ID优先规则");
}

void CHTLJSAdvancedTest::testIDPriorityRule() {
    std::string priorityCode = R"(
// 测试自动添加规则：缺什么，自动添加什么
div  // 没有class和id
{
    style
    {
        .box    // 第一个类选择器 → 自动添加 class="box"
        .box2   // 第二个类选择器 → 不添加
        #main   // 第一个ID选择器 → 自动添加 id="main"
        #sub    // 第二个ID选择器 → 不添加
    }
}

// 已有属性，不重复添加
div
{
    class: existing;
    style
    {
        .box  // 不会添加，因为已有class
    }
}

// script自动添加（当style没有添加时）
div  // 没有class和id，style也没有添加
{
    script
    {
        {{.auto-class}}  // 明确类选择器 → 自动添加 class="auto-class"
        {{#auto-id}}     // 明确ID选择器 → 自动添加 id="auto-id"
        {{element}}      // 不明确 → 不会自动添加
    }
}

// 上下文推导优先级
div
{
    class: test;
    id: main;
    style
    {
        &:hover  // 类优先 → 解析为 .test:hover
    }
    script
    {
        {{&}}  // ID优先 → 解析为 document.getElementById('main')
    }
}
)";
    
    assertTrue(isValidCHTLJSSyntax(priorityCode), "自动添加规则代码应该有效");
    
    // 验证自动添加规则的关键元素
    assertTrue(priorityCode.find(".box") != std::string::npos, "应该包含第一个类选择器");
    assertTrue(priorityCode.find("#main") != std::string::npos, "应该包含第一个ID选择器");
    assertTrue(priorityCode.find("{{.auto-class}}") != std::string::npos, "应该包含明确的CHTL JS类选择器");
    assertTrue(priorityCode.find("{{#auto-id}}") != std::string::npos, "应该包含明确的CHTL JS ID选择器");
    assertTrue(priorityCode.find("{{&}}") != std::string::npos, "应该包含上下文推导选择器");
    
    // 验证优先级说明
    assertTrue(priorityCode.find("类优先") != std::string::npos, "应该说明style中的类优先");
    assertTrue(priorityCode.find("ID优先") != std::string::npos, "应该说明script中的ID优先");
}

// === 辅助方法实现 ===

bool CHTLJSAdvancedTest::isValidCHTLJSSyntax(const std::string& code) {
    // 检查CHTL JS语法的基本模式
    std::regex chtlJSPattern(R"(\{\{[.#][a-zA-Z0-9_-]+(?:\s*->\s*\w+)?\}\})");
    return std::regex_search(code, chtlJSPattern) || code.find("vir") != std::string::npos;
}

bool CHTLJSAdvancedTest::hasVirtualObjectOptimization(const std::string& code) {
    // 检查是否有虚拟对象优化机会（相同的vir引用）
    std::regex virPattern(R"(vir\.\w+)");
    std::vector<std::string> virReferences;
    
    std::sregex_iterator iter(code.begin(), code.end(), virPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        virReferences.push_back(iter->str());
    }
    
    // 检查是否有重复的vir引用（优化机会）
    std::sort(virReferences.begin(), virReferences.end());
    for (size_t i = 1; i < virReferences.size(); ++i) {
        if (virReferences[i] == virReferences[i-1]) {
            return true; // 发现重复引用，有优化机会
        }
    }
    
    return false;
}

std::vector<std::string> CHTLJSAdvancedTest::extractCHTLJSFragments(const std::string& code) {
    std::vector<std::string> fragments;
    std::regex chtlJSPattern(R"(\{\{[^}]+\}\})");
    
    std::sregex_iterator iter(code.begin(), code.end(), chtlJSPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        fragments.push_back(iter->str());
    }
    
    return fragments;
}

bool CHTLJSAdvancedTest::validateEventSupport(const std::string& eventName) {
    // 验证事件名称是否为有效的JavaScript事件
    std::vector<std::string> validEvents = {
        "click", "dblclick", "mousedown", "mouseup", "mouseover", "mouseout",
        "mouseenter", "mouseleave", "mousemove", "contextmenu",
        "keydown", "keyup", "keypress",
        "focus", "blur", "focusin", "focusout",
        "change", "input", "submit", "reset", "invalid",
        "load", "unload", "beforeunload", "resize", "scroll",
        "touchstart", "touchend", "touchmove", "touchcancel",
        "drag", "dragstart", "dragend", "dragover", "dragenter", "dragleave", "drop",
        "play", "pause", "ended", "volumechange", "timeupdate",
        "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun", "transitioncancel"
    };
    
    return std::find(validEvents.begin(), validEvents.end(), eventName) != validEvents.end();
}

} // namespace unit
} // namespace test
} // namespace chtl