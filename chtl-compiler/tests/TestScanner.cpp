#include <iostream>
#include <cassert>
#include "scanner/CHTLUnifiedScanner.h"
#include "utils/Logger.h"

using namespace chtl;

void TestBasicScanning() {
    CHTLUnifiedScanner scanner;
    
    // 测试基本的CHTL代码
    std::string source = R"(
html {
    head {
        title {
            text {
                "Hello CHTL"
            }
        }
    }
    
    body {
        div {
            id: "main";
            class: "container";
            
            style {
                width: 100%;
                height: 200px;
                .box {
                    color: red;
                }
                &:hover {
                    background: blue;
                }
            }
            
            script {
                {{box}}->addEventListener('click', () => {
                    console.log('Clicked');
                });
                
                vir Test = listen({
                    click: () => {
                        alert('Hello');
                    }
                });
            }
        }
    }
}
)";
    
    auto fragments = scanner.Scan(source, "test.chtl");
    
    // 验证片段数量
    assert(fragments.size() > 0);
    
    // 打印片段信息
    std::cout << "Found " << fragments.size() << " fragments:" << std::endl;
    for (const auto& fragment : fragments) {
        std::cout << "  " << fragment.ToString() << std::endl;
    }
    
    std::cout << "TestBasicScanning passed!" << std::endl;
}

void TestTemplateScanning() {
    CHTLUnifiedScanner scanner;
    
    std::string source = R"(
[Template] @Style DefaultText {
    color: black;
    font-size: 16px;
}

[Custom] @Element Box {
    div {
        style {
            @Style DefaultText;
            background: white;
        }
    }
}

[Import] @Chtl from "module.chtl"
[Import] @Style from "styles.css" as MainStyles

[Configuration] {
    DEBUG_MODE = true;
}
)";
    
    auto fragments = scanner.Scan(source, "template.chtl");
    
    // 验证是否正确识别模板和自定义块
    bool hasTemplate = false;
    bool hasCustom = false;
    bool hasImport = false;
    bool hasConfig = false;
    
    for (const auto& fragment : fragments) {
        if (fragment.IsCHTLTemplate()) hasTemplate = true;
        if (fragment.IsCHTLCustom()) hasCustom = true;
        if (fragment.IsCHTLImport()) hasImport = true;
        if (fragment.IsCHTLConfiguration()) hasConfig = true;
    }
    
    assert(hasTemplate);
    assert(hasCustom);
    assert(hasImport);
    assert(hasConfig);
    
    std::cout << "TestTemplateScanning passed!" << std::endl;
}

void TestCHTLJSScanning() {
    CHTLUnifiedScanner scanner;
    
    std::string source = R"(
div {
    script {
        // CHTL JS 增强选择器
        {{.box}}->textContent = "Hello";
        {{#id}}->style->color = "red";
        {{button[0]}}->click();
        
        // 虚对象
        vir MyHandler = listen({
            click: () => {
                console.log("Clicked");
            },
            hover: () => {
                {{&}}->addClass('hovered');
            }
        });
        
        // 事件委托
        {{body}}->delegate({
            target: {{.dynamic-button}},
            click: MyHandler->click
        });
        
        // 普通JS代码
        let x = 10;
        function test() {
            return x * 2;
        }
    }
}
)";
    
    auto fragments = scanner.Scan(source, "chtljs.chtl");
    
    // 验证CHTL JS片段
    int chtlJsCount = 0;
    int jsCount = 0;
    
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == FragmentType::CHTL_JS) {
            chtlJsCount++;
            std::cout << "CHTL JS Fragment: " << fragment.GetContent() << std::endl;
        } else if (fragment.GetType() == FragmentType::JAVASCRIPT) {
            jsCount++;
        }
    }
    
    assert(chtlJsCount > 0);
    assert(jsCount > 0);
    
    std::cout << "TestCHTLJSScanning passed!" << std::endl;
}

void TestOriginBlock() {
    CHTLUnifiedScanner scanner;
    
    std::string source = R"(
[Origin] @Html {
    <div class="legacy">
        <p>This is raw HTML</p>
    </div>
}

[Origin] @Style custom_styles {
    .special { color: purple; }
}

[Origin] @JavaScript {
    // Raw JavaScript code
    window.onload = function() {
        console.log("Page loaded");
    };
}

[Origin] @Vue my_component {
    <template>
        <div>{{ message }}</div>
    </template>
}
)";
    
    auto fragments = scanner.Scan(source, "origin.chtl");
    
    // 验证原始嵌入块
    int originCount = 0;
    for (const auto& fragment : fragments) {
        if (fragment.IsCHTLOrigin()) {
            originCount++;
        }
    }
    
    assert(originCount == 4);
    
    std::cout << "TestOriginBlock passed!" << std::endl;
}

void TestCommentHandling() {
    CHTLUnifiedScanner scanner;
    
    std::string source = R"(
// 这是单行注释，不会被生成
div {
    /* 这是多行注释
       也不会被生成 */
    
    -- 这是生成器注释，会被保留
    
    style {
        color: red; // 行内注释
    }
}
)";
    
    auto fragments = scanner.Scan(source, "comments.chtl");
    
    // 验证生成器注释被保留
    bool hasGeneratorComment = false;
    for (const auto& fragment : fragments) {
        if (fragment.GetType() == FragmentType::CHTL &&
            fragment.GetContent().find("--") != std::string::npos) {
            hasGeneratorComment = true;
        }
    }
    
    assert(hasGeneratorComment);
    
    std::cout << "TestCommentHandling passed!" << std::endl;
}

int main() {
    // 设置日志级别
    utils::Logger::GetInstance().SetLevel(utils::LogLevel::DEBUG);
    
    std::cout << "Running Scanner tests..." << std::endl;
    
    TestBasicScanning();
    TestTemplateScanning();
    TestCHTLJSScanning();
    TestOriginBlock();
    TestCommentHandling();
    
    std::cout << "\nAll Scanner tests passed!" << std::endl;
    return 0;
}