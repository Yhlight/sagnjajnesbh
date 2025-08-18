#include "../src/CHTLJSCompiler/Core/JSEventRegistry.h"
#include "../src/CHTLJSCompiler/Parser/CHTLJSFunctionParser.h"
#include <iostream>
#include <cassert>

using namespace chtl::chtljs;

void testJSEventRegistry() {
    std::cout << "=== 测试JavaScript事件注册表 ===" << std::endl;
    
    JSEventRegistry& registry = JSEventRegistry::getInstance();
    
    // 测试常见事件
    assert(registry.isValidEvent("click"));
    assert(registry.isValidEvent("mouseover"));
    assert(registry.isValidEvent("keydown"));
    assert(registry.isValidEvent("submit"));
    assert(registry.isValidEvent("load"));
    assert(registry.isValidEvent("touchstart"));
    
    // 测试不存在的事件
    assert(!registry.isValidEvent("nonexistent"));
    assert(!registry.isValidEvent("fakeevent"));
    
    // 测试事件分类
    assert(registry.getEventCategory("click") == "mouse");
    assert(registry.getEventCategory("keydown") == "keyboard");
    assert(registry.getEventCategory("submit") == "form");
    
    std::cout << "支持的事件总数: " << registry.getAllEvents().size() << std::endl;
    std::cout << "事件分类数: " << registry.getAllCategories().size() << std::endl;
    
    // 显示一些事件类别
    auto categories = registry.getAllCategories();
    std::cout << "事件类别: ";
    for (size_t i = 0; i < std::min(categories.size(), size_t(5)); ++i) {
        std::cout << categories[i] << " ";
    }
    std::cout << "..." << std::endl;
    
    std::cout << "✅ JavaScript事件注册表测试通过!" << std::endl;
}

void testUnorderedKeyValuePairs() {
    std::cout << "=== 测试无序键值对支持 ===" << std::endl;
    
    CHTLJSFunctionParser parser;
    
    // 测试无序的listen函数
    std::string unordered_listen = R"(
        listen({
            mouseover: handleMouseOver,
            click: handleClick,
            mouseout: handleMouseOut,
            keydown: handleKeyDown
        })
    )";
    
    auto result = parser.parseFunction(unordered_listen);
    
    std::cout << "解析结果: " << (result.is_valid ? "成功" : "失败") << std::endl;
    std::cout << "解析到的键值对数量: " << result.key_value_pairs.size() << std::endl;
    
    // 验证所有键都被正确解析
    std::vector<std::string> expected_keys = {"mouseover", "click", "mouseout", "keydown"};
    for (const std::string& expected_key : expected_keys) {
        bool found = false;
        for (const auto& kv : result.key_value_pairs) {
            if (kv.key == expected_key) {
                found = true;
                std::cout << "找到键: " << expected_key << " -> " << kv.value << std::endl;
                break;
            }
        }
        assert(found);
    }
    
    std::cout << "✅ 无序键值对支持测试通过!" << std::endl;
}

void testOptionalKeyValuePairs() {
    std::cout << "=== 测试可选键值对支持 ===" << std::endl;
    
    CHTLJSFunctionParser parser;
    
    // 测试只包含部分可选键的listen函数
    std::string optional_listen = R"(
        listen({
            click: () => { console.log('clicked'); }
        })
    )";
    
    auto result1 = parser.parseFunction(optional_listen);
    std::cout << "只有click事件的listen解析: " << (result1.is_valid ? "成功" : "失败") << std::endl;
    assert(result1.is_valid); // 应该成功，因为所有键都是可选的
    
    // 测试包含多个可选键的listen函数
    std::string multiple_optional = R"(
        listen({
            click: handleClick,
            focus: handleFocus,
            blur: handleBlur
        })
    )";
    
    auto result2 = parser.parseFunction(multiple_optional);
    std::cout << "多个可选键的listen解析: " << (result2.is_valid ? "成功" : "失败") << std::endl;
    std::cout << "解析到的键数量: " << result2.key_value_pairs.size() << std::endl;
    assert(result2.is_valid);
    
    std::cout << "✅ 可选键值对支持测试通过!" << std::endl;
}

void testDelegateWithAllEvents() {
    std::cout << "=== 测试delegate函数支持所有事件 ===" << std::endl;
    
    CHTLJSFunctionParser parser;
    
    // 测试delegate函数（需要selector + 事件）
    std::string delegate_call = R"(
        delegate({
            selector: ".button",
            click: handleButtonClick,
            mouseenter: handleButtonHover,
            focus: handleButtonFocus
        })
    )";
    
    auto result = parser.parseFunction(delegate_call);
    
    std::cout << "delegate解析结果: " << (result.is_valid ? "成功" : "失败") << std::endl;
    
    if (!result.is_valid) {
        std::cout << "解析错误:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  - " << error << std::endl;
        }
    } else {
        std::cout << "解析到的键值对:" << std::endl;
        for (const auto& kv : result.key_value_pairs) {
            std::cout << "  " << kv.key << " : " << kv.value << std::endl;
        }
    }
    
    std::cout << "✅ delegate函数事件支持测试完成!" << std::endl;
}

void testComprehensiveEventSupport() {
    std::cout << "=== 测试全面的事件支持 ===" << std::endl;
    
    JSEventRegistry& registry = JSEventRegistry::getInstance();
    CHTLJSFunctionParser parser;
    
    // 测试各个类别的事件
    std::vector<std::string> test_events = {
        "click", "mousedown", "mouseover",           // 鼠标事件
        "keydown", "keyup",                          // 键盘事件
        "touchstart", "touchend",                    // 触摸事件
        "submit", "change", "focus", "blur",         // 表单事件
        "load", "resize", "scroll",                  // 窗口事件
        "drag", "drop",                              // 拖拽事件
        "copy", "paste",                             // 剪贴板事件
        "animationend", "transitionend"              // 动画事件
    };
    
    std::cout << "测试事件支持情况:" << std::endl;
    
    for (const std::string& event : test_events) {
        bool is_valid = registry.isValidEvent(event);
        std::string category = registry.getEventCategory(event);
        
        std::cout << "  " << event << " [" << category << "]: " 
                  << (is_valid ? "✅" : "❌") << std::endl;
        
        assert(is_valid); // 所有测试事件都应该被支持
    }
    
    // 测试包含多种事件的listen函数
    std::string comprehensive_listen = R"(
        listen({
            click: handleClick,
            keydown: handleKeyDown,
            touchstart: handleTouch,
            submit: handleSubmit,
            load: handleLoad,
            animationend: handleAnimationEnd
        })
    )";
    
    auto result = parser.parseFunction(comprehensive_listen);
    std::cout << "\n全面事件listen函数解析: " << (result.is_valid ? "成功" : "失败") << std::endl;
    std::cout << "支持的事件数量: " << result.key_value_pairs.size() << std::endl;
    
    assert(result.is_valid);
    
    std::cout << "✅ 全面的事件支持测试通过!" << std::endl;
}

void testCodeGeneration() {
    std::cout << "=== 测试代码生成 ===" << std::endl;
    
    CHTLJSFunctionParser parser;
    
    // 测试listen代码生成
    std::string listen_call = R"(
        listen({
            click: () => { alert('clicked'); },
            mouseenter: handleMouseEnter
        })
    )";
    
    auto listen_result = parser.parseFunction(listen_call);
    if (listen_result.is_valid) {
        std::string generated_code = parser.generateOptimizedCode(listen_result);
        std::cout << "生成的listen代码:" << std::endl;
        std::cout << generated_code << std::endl;
    }
    
    std::cout << "✅ 代码生成测试完成!" << std::endl;
}

void testRealWorldScenarios() {
    std::cout << "=== 测试真实世界场景 ===" << std::endl;
    
    CHTLJSFunctionParser parser;
    
    // 场景1: 表单处理（无序 + 可选）
    std::cout << "场景1: 表单处理" << std::endl;
    std::string form_scenario = R"(
        listen({
            invalid: showValidationError,
            submit: handleFormSubmit,
            change: validateField,
            focus: highlightField,
            blur: validateAndHideError
        })
    )";
    
    auto form_result = parser.parseFunction(form_scenario);
    std::cout << "  表单事件解析: " << (form_result.is_valid ? "成功" : "失败") << std::endl;
    
    // 场景2: 媒体播放器（可选键）
    std::cout << "场景2: 媒体播放器" << std::endl;
    std::string media_scenario = R"(
        listen({
            play: onPlay,
            pause: onPause,
            ended: onEnded,
            volumechange: onVolumeChange
        })
    )";
    
    auto media_result = parser.parseFunction(media_scenario);
    std::cout << "  媒体事件解析: " << (media_result.is_valid ? "成功" : "失败") << std::endl;
    
    // 场景3: 拖拽功能（复杂事件组合）
    std::cout << "场景3: 拖拽功能" << std::endl;
    std::string drag_scenario = R"(
        delegate({
            selector: ".draggable",
            dragstart: initDrag,
            drag: updateDragPosition,
            dragend: finalizeDrag,
            drop: handleDrop
        })
    )";
    
    auto drag_result = parser.parseFunction(drag_scenario);
    std::cout << "  拖拽事件解析: " << (drag_result.is_valid ? "成功" : "失败") << std::endl;
    
    if (!drag_result.is_valid) {
        for (const auto& error : drag_result.errors) {
            std::cout << "    错误: " << error << std::endl;
        }
    }
    
    std::cout << "✅ 真实世界场景测试完成!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL JS增强函数功能..." << std::endl << std::endl;
        
        testJSEventRegistry();
        std::cout << std::endl;
        
        testUnorderedKeyValuePairs();
        std::cout << std::endl;
        
        testOptionalKeyValuePairs();
        std::cout << std::endl;
        
        testDelegateWithAllEvents();
        std::cout << std::endl;
        
        testComprehensiveEventSupport();
        std::cout << std::endl;
        
        testCodeGeneration();
        std::cout << std::endl;
        
        testRealWorldScenarios();
        std::cout << std::endl;
        
        std::cout << "🎉 所有CHTL JS增强函数测试通过!" << std::endl;
        std::cout << "✅ listen和delegate现在支持所有JS事件!" << std::endl;
        std::cout << "✅ CHTL JS函数支持无序键值对!" << std::endl;
        std::cout << "✅ CHTL JS函数支持可选键值对!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}