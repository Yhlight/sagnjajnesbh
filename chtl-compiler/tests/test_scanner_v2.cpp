#include <iostream>
#include <cassert>
#include "scanner/chtl_unified_scanner_v2.h"

void testVariableLengthSlice() {
    std::cout << "Testing variable length slice mechanism...\n";
    
    chtl::CHTLUnifiedScannerV2 scanner;
    scanner.setInitialSliceSize(50);  // 小切片以测试扩展
    scanner.setExpandStep(25);
    
    // 测试需要扩展的情况
    std::string source = R"(
        div {
            style {
                .box {
                    width: 100px;
                    height: 100px;
                }
            }
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    assert(!fragments.empty());
    assert(scanner.getErrors().empty());
    
    std::cout << "✓ Variable length slice test passed\n";
}

void testSyntaxUnitSplitting() {
    std::cout << "Testing syntax unit splitting...\n";
    
    chtl::CHTLUnifiedScannerV2 scanner;
    
    // 测试CHTL JS语法单元切割
    std::string source = R"(
        script {
            {{box}}->addEventListener('click', function() {
                console.log('clicked');
            });
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    assert(fragments.size() >= 1);
    bool foundCHTLJS = false;
    for (const auto& fragment : fragments) {
        if (fragment->getType() == chtl::FragmentType::CHTL_JS) {
            foundCHTLJS = true;
            break;
        }
    }
    assert(foundCHTLJS);
    
    std::cout << "✓ Syntax unit splitting test passed\n";
}

void testBoundaryCutoffDetection() {
    std::cout << "Testing boundary cutoff detection...\n";
    
    chtl::CHTLUnifiedScannerV2 scanner;
    scanner.setInitialSliceSize(30);  // 故意设置小切片
    
    // 测试在不合理位置的截断
    std::string source = R"(
        div {
            text {
                "This is a very long text content that should not be cut in the middle"
            }
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    // 应该正确处理而不会在字符串中间截断
    assert(!fragments.empty());
    assert(scanner.getErrors().empty());
    
    std::cout << "✓ Boundary cutoff detection test passed\n";
}

void testCHTLJSBoundaryDetection() {
    std::cout << "Testing CHTL JS boundary detection...\n";
    
    chtl::CHTLUnifiedScannerV2 scanner;
    
    // 测试各种CHTL JS语法边界
    std::string source = R"(
        script {
            vir Test = listen({
                click: function() {
                    {{#button}}->style.color = 'red';
                }
            });
            
            {{.container}}->delegate({
                target: {{.item}},
                click: function(e) {
                    Test->click();
                }
            });
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    assert(!fragments.empty());
    
    // 验证CHTL JS片段被正确识别
    bool foundCHTLJS = false;
    for (const auto& fragment : fragments) {
        if (fragment->getType() == chtl::FragmentType::CHTL_JS) {
            foundCHTLJS = true;
            // 检查内容包含所有CHTL JS特性
            const std::string& content = fragment->getContent();
            assert(content.find("vir") != std::string::npos);
            assert(content.find("{{") != std::string::npos);
            assert(content.find("->") != std::string::npos);
            break;
        }
    }
    assert(foundCHTLJS);
    
    std::cout << "✓ CHTL JS boundary detection test passed\n";
}

void testComplexMixedContent() {
    std::cout << "Testing complex mixed content...\n";
    
    chtl::CHTLUnifiedScannerV2 scanner;
    
    std::string source = R"(
        [Template] @Style DefaultText {
            color: #333;
            line-height: 1.6;
        }
        
        div {
            style {
                .container {
                    max-width: 800px;
                }
                
                &:hover {
                    background-color: #f0f0f0;
                }
            }
            
            text {
                "Hello CHTL"
            }
            
            script {
                {{container}}->addEventListener('click', () => {
                    console.log('Container clicked');
                });
            }
        }
        
        style {
            body {
                margin: 0;
                padding: 0;
            }
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    // 应该有多种类型的片段
    assert(fragments.size() >= 3);
    
    // 验证不同类型的片段
    bool hasCHTL = false;
    bool hasLocalStyle = false;
    bool hasCHTLJS = false;
    bool hasCSS = false;
    
    for (const auto& fragment : fragments) {
        switch (fragment->getType()) {
            case chtl::FragmentType::CHTL:
                hasCHTL = true;
                break;
            case chtl::FragmentType::LOCAL_STYLE:
                hasLocalStyle = true;
                break;
            case chtl::FragmentType::CHTL_JS:
                hasCHTLJS = true;
                break;
            case chtl::FragmentType::CSS:
                hasCSS = true;
                break;
            default:
                break;
        }
    }
    
    assert(hasCHTL);
    assert(hasLocalStyle);
    assert(hasCHTLJS);
    
    std::cout << "✓ Complex mixed content test passed\n";
}

int main() {
    std::cout << "Running Scanner V2 Tests\n";
    std::cout << "========================\n\n";
    
    try {
        testVariableLengthSlice();
        testSyntaxUnitSplitting();
        testBoundaryCutoffDetection();
        testCHTLJSBoundaryDetection();
        testComplexMixedContent();
        
        std::cout << "\nAll Scanner V2 tests passed! ✓\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << "\n";
        return 1;
    }
}