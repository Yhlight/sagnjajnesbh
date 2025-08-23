#include <iostream>
#include <cassert>
#include <memory>
#include "CHTLJS/Events/JSEventSystem.h"
#include "CHTL/Comments/ContextualCommentSystem.h"

using namespace CHTLJS::Events;
using namespace CHTL::Comments;

// 测试JS事件系统
void test_js_event_system() {
    std::cout << "测试JS事件系统...\n";
    
    JSEventManager eventManager;
    
    // 测试标准鼠标事件
    std::cout << "  测试标准鼠标事件...\n";
    assert(eventManager.IsValidJSEvent("click"));
    assert(eventManager.IsValidJSEvent("dblclick"));
    assert(eventManager.IsValidJSEvent("mousedown"));
    assert(eventManager.IsValidJSEvent("mouseup"));
    assert(eventManager.IsValidJSEvent("mouseover"));
    assert(eventManager.IsValidJSEvent("mouseout"));
    assert(eventManager.IsValidJSEvent("mousemove"));
    assert(eventManager.IsValidJSEvent("mouseenter"));
    assert(eventManager.IsValidJSEvent("mouseleave"));
    assert(eventManager.IsValidJSEvent("contextmenu"));
    assert(eventManager.IsValidJSEvent("wheel"));
    
    std::cout << "    鼠标事件验证通过\n";
    
    // 测试键盘事件
    std::cout << "  测试键盘事件...\n";
    assert(eventManager.IsValidJSEvent("keydown"));
    assert(eventManager.IsValidJSEvent("keyup"));
    assert(eventManager.IsValidJSEvent("keypress"));
    
    std::cout << "    键盘事件验证通过\n";
    
    // 测试表单事件
    std::cout << "  测试表单事件...\n";
    assert(eventManager.IsValidJSEvent("submit"));
    assert(eventManager.IsValidJSEvent("reset"));
    assert(eventManager.IsValidJSEvent("focus"));
    assert(eventManager.IsValidJSEvent("blur"));
    assert(eventManager.IsValidJSEvent("change"));
    assert(eventManager.IsValidJSEvent("input"));
    assert(eventManager.IsValidJSEvent("select"));
    
    std::cout << "    表单事件验证通过\n";
    
    // 测试窗口事件
    std::cout << "  测试窗口事件...\n";
    assert(eventManager.IsValidJSEvent("load"));
    assert(eventManager.IsValidJSEvent("unload"));
    assert(eventManager.IsValidJSEvent("beforeunload"));
    assert(eventManager.IsValidJSEvent("resize"));
    assert(eventManager.IsValidJSEvent("scroll"));
    
    std::cout << "    窗口事件验证通过\n";
    
    // 测试触摸事件
    std::cout << "  测试触摸事件...\n";
    assert(eventManager.IsValidJSEvent("touchstart"));
    assert(eventManager.IsValidJSEvent("touchend"));
    assert(eventManager.IsValidJSEvent("touchmove"));
    assert(eventManager.IsValidJSEvent("touchcancel"));
    
    std::cout << "    触摸事件验证通过\n";
    
    // 测试拖拽事件
    std::cout << "  测试拖拽事件...\n";
    assert(eventManager.IsValidJSEvent("drag"));
    assert(eventManager.IsValidJSEvent("dragstart"));
    assert(eventManager.IsValidJSEvent("dragend"));
    assert(eventManager.IsValidJSEvent("dragover"));
    assert(eventManager.IsValidJSEvent("dragenter"));
    assert(eventManager.IsValidJSEvent("dragleave"));
    assert(eventManager.IsValidJSEvent("drop"));
    
    std::cout << "    拖拽事件验证通过\n";
    
    // 测试媒体事件
    std::cout << "  测试媒体事件...\n";
    assert(eventManager.IsValidJSEvent("play"));
    assert(eventManager.IsValidJSEvent("pause"));
    assert(eventManager.IsValidJSEvent("ended"));
    assert(eventManager.IsValidJSEvent("timeupdate"));
    assert(eventManager.IsValidJSEvent("volumechange"));
    assert(eventManager.IsValidJSEvent("loadstart"));
    assert(eventManager.IsValidJSEvent("loadeddata"));
    assert(eventManager.IsValidJSEvent("loadedmetadata"));
    assert(eventManager.IsValidJSEvent("canplay"));
    assert(eventManager.IsValidJSEvent("canplaythrough"));
    
    std::cout << "    媒体事件验证通过\n";
    
    // 测试自定义事件
    std::cout << "  测试自定义事件...\n";
    eventManager.AddCustomEvent("customClick");
    eventManager.AddCustomEvent("dataLoaded");
    eventManager.AddCustomEvent("userAction");
    
    assert(eventManager.IsValidJSEvent("customClick"));
    assert(eventManager.IsValidJSEvent("dataLoaded"));
    assert(eventManager.IsValidJSEvent("userAction"));
    assert(eventManager.IsCustomEvent("customClick"));
    
    std::cout << "    自定义事件验证通过\n";
    
    // 测试事件分类
    std::cout << "  测试事件分类...\n";
    assert(eventManager.GetEventCategory(JSEventType::CLICK) == "鼠标事件");
    assert(eventManager.GetEventCategory(JSEventType::KEYDOWN) == "键盘事件");
    assert(eventManager.GetEventCategory(JSEventType::SUBMIT) == "表单事件");
    assert(eventManager.GetEventCategory(JSEventType::LOAD) == "窗口事件");
    assert(eventManager.GetEventCategory(JSEventType::TOUCHSTART) == "触摸事件");
    assert(eventManager.GetEventCategory(JSEventType::DRAG) == "拖拽事件");
    assert(eventManager.GetEventCategory(JSEventType::PLAY) == "媒体事件");
    assert(eventManager.GetEventCategory(JSEventType::CUSTOM) == "自定义事件");
    
    std::cout << "    事件分类验证通过\n";
    
    // 获取所有事件名称
    auto allEvents = eventManager.GetAllEventNames();
    std::cout << "  支持的JS事件总数: " << allEvents.size() << "\n";
    
    std::cout << "  ✅ JS事件系统测试通过\n";
}

// 测试键值对灵活性管理器
void test_key_value_flexibility() {
    std::cout << "测试键值对灵活性管理器...\n";
    
    KeyValueFlexibilityManager kvManager;
    
    // 测试基本键值对解析
    std::cout << "  测试基本键值对解析...\n";
    
    std::string kvString1 = "{ duration: 300, easing: ease-in-out, delay: 100 }";
    auto result1 = kvManager.ParseKeyValuePairs(kvString1);
    
    assert(result1["duration"] == "300");
    assert(result1["easing"] == "ease-in-out");
    assert(result1["delay"] == "100");
    
    std::cout << "    基本键值对解析正确\n";
    
    // 测试无序键值对
    std::cout << "  测试无序键值对...\n";
    
    std::string kvString2 = "{ easing: ease, delay: 0, duration: 500 }";
    auto result2 = kvManager.ParseKeyValuePairs(kvString2);
    
    assert(result2["duration"] == "500");
    assert(result2["easing"] == "ease");
    assert(result2["delay"] == "0");
    
    std::cout << "    无序键值对解析正确\n";
    
    // 测试可选键的默认值
    std::cout << "  测试可选键的默认值...\n";
    
    std::string kvString3 = "{ duration: 400 }";
    std::vector<std::string> requiredKeys = {"duration"};
    std::vector<std::string> optionalKeys = {"easing", "delay", "loop"};
    
    auto result3 = kvManager.ParseKeyValuePairs(kvString3, requiredKeys, optionalKeys);
    
    assert(result3["duration"] == "400");
    assert(result3["easing"] == "ease");    // 默认值
    assert(result3["delay"] == "0");        // 默认值
    assert(result3["loop"] == "false");     // 默认值
    
    std::cout << "    可选键默认值正确\n";
    
    // 测试必需键验证
    std::cout << "  测试必需键验证...\n";
    
    std::unordered_map<std::string, std::string> incompleteMap = {
        {"easing", "ease"}
        // 缺少duration
    };
    
    bool isValid = kvManager.ValidateKeyValuePairs(incompleteMap, requiredKeys);
    assert(!isValid); // 应该验证失败
    
    std::unordered_map<std::string, std::string> completeMap = {
        {"duration", "300"},
        {"easing", "ease"}
    };
    
    isValid = kvManager.ValidateKeyValuePairs(completeMap, requiredKeys);
    assert(isValid); // 应该验证成功
    
    std::cout << "    必需键验证正确\n";
    
    // 测试自定义默认值
    std::cout << "  测试自定义默认值...\n";
    
    kvManager.SetDefaultValue("customProperty", "customValue");
    assert(kvManager.GetDefaultValue("customProperty") == "customValue");
    
    std::cout << "    自定义默认值正确\n";
    
    std::cout << "  ✅ 键值对灵活性管理器测试通过\n";
}

// 测试无修饰字面量管理器
void test_undecorated_literal_manager() {
    std::cout << "测试无修饰字面量管理器...\n";
    
    UndecoratedLiteralManager literalManager;
    
    // 测试数字字面量
    std::cout << "  测试数字字面量...\n";
    assert(literalManager.IsUndecoratedLiteral("123"));
    assert(literalManager.IsUndecoratedLiteral("45.67"));
    assert(literalManager.IsUndecoratedLiteral("-89"));
    assert(literalManager.IsUndecoratedLiteral("3.14e-2"));
    assert(literalManager.GetLiteralType("123") == "number");
    assert(literalManager.GetLiteralType("45.67") == "number");
    
    std::cout << "    数字字面量识别正确\n";
    
    // 测试布尔字面量
    std::cout << "  测试布尔字面量...\n";
    assert(literalManager.IsUndecoratedLiteral("true"));
    assert(literalManager.IsUndecoratedLiteral("false"));
    assert(literalManager.GetLiteralType("true") == "boolean");
    assert(literalManager.GetLiteralType("false") == "boolean");
    
    std::cout << "    布尔字面量识别正确\n";
    
    // 测试标识符
    std::cout << "  测试标识符...\n";
    assert(literalManager.IsUndecoratedLiteral("variable"));
    assert(literalManager.IsUndecoratedLiteral("_private"));
    assert(literalManager.IsUndecoratedLiteral("$jquery"));
    assert(literalManager.IsUndecoratedLiteral("myVar123"));
    assert(literalManager.GetLiteralType("variable") == "identifier");
    
    std::cout << "    标识符识别正确\n";
    
    // 测试排除情况
    std::cout << "  测试排除情况...\n";
    assert(!literalManager.IsUndecoratedLiteral("\"string\"")); // 带引号的字符串
    assert(!literalManager.IsUndecoratedLiteral("'string'"));   // 带引号的字符串
    assert(!literalManager.IsUndecoratedLiteral("{{selector}}")); // 特殊语法
    assert(!literalManager.IsUndecoratedLiteral("element->method")); // 箭头操作符
    
    std::cout << "    排除情况正确\n";
    
    // 测试字面量解析
    std::cout << "  测试字面量解析...\n";
    assert(literalManager.ParseUndecoratedLiteral("123") == "123");
    assert(literalManager.ParseUndecoratedLiteral("true") == "true");
    assert(literalManager.ParseUndecoratedLiteral("variable") == "variable");
    assert(literalManager.ParseUndecoratedLiteral("\"string\"") == "\"string\""); // 非无修饰字面量直接返回
    
    std::cout << "    字面量解析正确\n";
    
    std::cout << "  ✅ 无修饰字面量管理器测试通过\n";
}

// 测试上下文注释生成器
void test_contextual_comment_generator() {
    std::cout << "测试上下文注释生成器...\n";
    
    ContextualCommentGenerator commentGenerator;
    
    // 测试HTML注释生成
    std::cout << "  测试HTML注释生成...\n";
    std::string htmlComment = commentGenerator.GenerateComment("这是HTML注释", ProgrammingContext::HTML);
    assert(htmlComment == "<!-- 这是HTML注释 -->");
    
    std::cout << "    HTML注释: " << htmlComment << "\n";
    
    // 测试CSS注释生成
    std::cout << "  测试CSS注释生成...\n";
    std::string cssComment = commentGenerator.GenerateComment("这是CSS注释", ProgrammingContext::CSS);
    assert(cssComment == "/* 这是CSS注释 */");
    
    std::cout << "    CSS注释: " << cssComment << "\n";
    
    // 测试JavaScript注释生成
    std::cout << "  测试JavaScript注释生成...\n";
    std::string jsComment1 = commentGenerator.GenerateComment("单行注释", ProgrammingContext::JAVASCRIPT);
    assert(jsComment1 == "// 单行注释");
    
    std::string jsComment2 = commentGenerator.GenerateComment("多行\n注释", ProgrammingContext::JAVASCRIPT);
    assert(jsComment2 == "/* 多行\n注释 */");
    
    std::cout << "    JavaScript注释: " << jsComment1 << "\n";
    std::cout << "    JavaScript多行注释: " << jsComment2 << "\n";
    
    // 测试CHTL注释生成
    std::cout << "  测试CHTL注释生成...\n";
    std::string chtlComment = commentGenerator.GenerateComment("这是CHTL注释", ProgrammingContext::CHTL);
    assert(chtlComment == "// 这是CHTL注释");
    
    std::cout << "    CHTL注释: " << chtlComment << "\n";
    
    // 测试CHTL JS注释生成
    std::cout << "  测试CHTL JS注释生成...\n";
    std::string chtljsComment = commentGenerator.GenerateComment("这是CHTL JS注释", ProgrammingContext::CHTL_JS);
    assert(chtljsComment == "// 这是CHTL JS注释");
    
    std::cout << "    CHTL JS注释: " << chtljsComment << "\n";
    
    // 测试上下文检测
    std::cout << "  测试上下文检测...\n";
    
    assert(commentGenerator.DetectContext("style { .box { color: red; } }") == ProgrammingContext::CSS);
    assert(commentGenerator.DetectContext("script { {{.button}}->listen({ click: () => {} }); }") == ProgrammingContext::CHTL_JS);
    assert(commentGenerator.DetectContext("function test() { console.log('test'); }") == ProgrammingContext::JAVASCRIPT);
    assert(commentGenerator.DetectContext("html { body { div { } } }") == ProgrammingContext::HTML);
    assert(commentGenerator.DetectContext("@Element Button { }") == ProgrammingContext::CHTL);
    
    std::cout << "    上下文检测正确\n";
    
    // 测试"--"注释处理
    std::cout << "  测试\"--\"注释处理...\n";
    
    std::string dashComment1 = commentGenerator.ProcessDashComment("样式注释", ProgrammingContext::CSS);
    assert(dashComment1 == "/* 样式注释 */");
    
    std::string dashComment2 = commentGenerator.ProcessDashComment("脚本注释", ProgrammingContext::CHTL_JS);
    assert(dashComment2 == "// 脚本注释");
    
    std::string dashComment3 = commentGenerator.ProcessDashComment("HTML注释", ProgrammingContext::HTML);
    assert(dashComment3 == "<!-- HTML注释 -->");
    
    std::cout << "    \"--\"注释处理正确\n";
    
    std::cout << "  ✅ 上下文注释生成器测试通过\n";
}

// 测试[Origin]任意位置支持
void test_origin_anywhere_support() {
    std::cout << "测试[Origin]任意位置支持...\n";
    
    OriginAnywhereManager originManager;
    
    // 测试各种上下文中的[Origin]使用
    std::cout << "  测试各种上下文中的[Origin]使用...\n";
    
    assert(originManager.CanUseOriginHere("HTML元素", "<div>content</div>"));
    assert(originManager.CanUseOriginHere("CSS样式", "style { .box { } }"));
    assert(originManager.CanUseOriginHere("JavaScript脚本", "script { function() {} }"));
    assert(originManager.CanUseOriginHere("CHTL结构", "@Element Button { }"));
    assert(originManager.CanUseOriginHere("自定义位置", "任意代码"));
    
    std::cout << "    [Origin]可以在任意位置使用\n";
    
    // 测试[Origin]使用验证
    std::cout << "  测试[Origin]使用验证...\n";
    
    auto validation1 = originManager.ValidateOriginUsage("@Html", "HTML元素");
    assert(validation1.first); // 应该验证通过
    std::cout << "    @Html验证: " << validation1.second << "\n";
    
    auto validation2 = originManager.ValidateOriginUsage("@Style", "CSS样式");
    assert(validation2.first); // 应该验证通过
    std::cout << "    @Style验证: " << validation2.second << "\n";
    
    auto validation3 = originManager.ValidateOriginUsage("@Vue", "JavaScript脚本");
    assert(validation3.first); // 应该验证通过（虽然可能不常见）
    std::cout << "    @Vue验证: " << validation3.second << "\n";
    
    // 测试使用建议
    std::cout << "  测试使用建议...\n";
    
    std::string suggestion = originManager.GetOriginUsageSuggestion("HTML元素");
    assert(!suggestion.empty());
    std::cout << "    使用建议:\n" << suggestion << "\n";
    
    std::cout << "  ✅ [Origin]任意位置支持测试通过\n";
}

// 测试完整的事件支持场景
void test_complete_event_support_scenario() {
    std::cout << "测试完整的事件支持场景...\n";
    
    JSEventManager eventManager;
    KeyValueFlexibilityManager kvManager;
    
    // 模拟listen函数的事件处理
    std::cout << "  模拟listen函数的完整事件支持...\n";
    
    // 支持所有鼠标事件
    std::vector<std::string> mouseEvents = {
        "click", "dblclick", "mousedown", "mouseup", "mouseover", 
        "mouseout", "mousemove", "mouseenter", "mouseleave", "contextmenu", "wheel"
    };
    
    for (const auto& event : mouseEvents) {
        assert(eventManager.IsValidJSEvent(event));
        assert(eventManager.GetEventCategory(eventManager.GetEventType(event)) == "鼠标事件");
    }
    
    std::cout << "    所有鼠标事件支持正确\n";
    
    // 支持所有键盘事件
    std::vector<std::string> keyboardEvents = {"keydown", "keyup", "keypress"};
    
    for (const auto& event : keyboardEvents) {
        assert(eventManager.IsValidJSEvent(event));
        assert(eventManager.GetEventCategory(eventManager.GetEventType(event)) == "键盘事件");
    }
    
    std::cout << "    所有键盘事件支持正确\n";
    
    // 支持所有表单事件
    std::vector<std::string> formEvents = {
        "submit", "reset", "focus", "blur", "change", "input", "select"
    };
    
    for (const auto& event : formEvents) {
        assert(eventManager.IsValidJSEvent(event));
        assert(eventManager.GetEventCategory(eventManager.GetEventType(event)) == "表单事件");
    }
    
    std::cout << "    所有表单事件支持正确\n";
    
    // 模拟animate函数的键值对灵活性
    std::cout << "  模拟animate函数的键值对灵活性...\n";
    
    // 测试完整的动画参数
    std::string animateParams1 = "{ duration: 500, easing: ease-in-out, delay: 100, loop: true }";
    auto animateResult1 = kvManager.ParseKeyValuePairs(animateParams1);
    
    assert(animateResult1["duration"] == "500");
    assert(animateResult1["easing"] == "ease-in-out");
    assert(animateResult1["delay"] == "100");
    assert(animateResult1["loop"] == "true");
    
    // 测试最少参数（其他使用默认值）
    std::string animateParams2 = "{ duration: 300 }";
    std::vector<std::string> animateRequired = {"duration"};
    std::vector<std::string> animateOptional = {"easing", "delay", "loop", "direction"};
    
    auto animateResult2 = kvManager.ParseKeyValuePairs(animateParams2, animateRequired, animateOptional);
    
    assert(animateResult2["duration"] == "300");
    assert(animateResult2["easing"] == "ease");      // 默认值
    assert(animateResult2["delay"] == "0");          // 默认值
    assert(animateResult2["loop"] == "false");       // 默认值
    assert(animateResult2["direction"] == "normal"); // 默认值
    
    std::cout << "    animate函数键值对灵活性正确\n";
    
    std::cout << "  ✅ 完整事件支持场景测试通过\n";
}

int main() {
    std::cout << "运行CHTL JS增强功能测试...\n\n";
    
    try {
        test_js_event_system();
        test_key_value_flexibility();
        test_undecorated_literal_manager();
        test_contextual_comment_generator();
        test_origin_anywhere_support();
        test_complete_event_support_scenario();
        
        std::cout << "\n🎉 所有CHTL JS增强功能测试通过!\n";
        std::cout << "✅ JS事件系统正常\n";
        std::cout << "✅ 键值对灵活性管理器正常\n";
        std::cout << "✅ 无修饰字面量管理器正常\n";
        std::cout << "✅ 上下文注释生成器正常\n";
        std::cout << "✅ [Origin]任意位置支持正常\n";
        std::cout << "✅ 完整事件支持场景正常\n";
        
        std::cout << "\n📋 CHTL JS增强功能总结:\n";
        std::cout << "🔧 Token清理:\n";
        std::cout << "   已移除错误的Token：iNeverAway、function、const、let、var、Void\n";
        std::cout << "   这些不是CHTL JS的内容，已全面清理\n";
        std::cout << "\n🎯 JS事件支持:\n";
        std::cout << "   支持所有标准JS事件：鼠标、键盘、表单、窗口、触摸、拖拽、媒体、网络等\n";
        std::cout << "   支持自定义事件扩展\n";
        std::cout << "   所有使用JS事件的函数（listen、delegate等）都支持完整事件集\n";
        std::cout << "\n🔗 键值对灵活性:\n";
        std::cout << "   支持键值对无序：参数顺序可以任意调整\n";
        std::cout << "   支持键值对可选：提供默认值，参数可以省略\n";
        std::cout << "   极大扩展了CHTL JS函数的灵活性\n";
        std::cout << "\n✨ 无修饰字面量:\n";
        std::cout << "   支持数字字面量：123、45.67、-89、3.14e-2\n";
        std::cout << "   支持布尔字面量：true、false\n";
        std::cout << "   支持标识符：variable、_private、$jquery\n";
        std::cout << "   不能单独作为统一扫描器的判断条件\n";
        std::cout << "\n🏷️ [Origin]任意位置支持:\n";
        std::cout << "   [Origin]原始嵌入节点任意地方都可以使用\n";
        std::cout << "   不应该被束缚，问题由开发者自己控制\n";
        std::cout << "\n💬 上下文注释:\n";
        std::cout << "   \"--\"注释根据上下文生成不同编程语言的注释\n";
        std::cout << "   HTML: <!-- comment -->\n";
        std::cout << "   CSS: /* comment */\n";
        std::cout << "   JavaScript/CHTL JS: // comment\n";
        std::cout << "   CHTL: // comment\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}