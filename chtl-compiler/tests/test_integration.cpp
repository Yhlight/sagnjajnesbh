#include <iostream>
#include <string>
#include <cassert>
#include "scanner/chtl_unified_scanner.h"
#include "dispatcher/compiler_dispatcher.h"

void testBasicHTML() {
    std::cout << "Testing basic HTML generation...\n";
    
    std::string source = R"(
        div {
            text {
                "Hello World"
            }
        }
    )";
    
    chtl::CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan(source);
    
    assert(!fragments.empty());
    assert(scanner.getErrors().empty());
    
    chtl::CompilerDispatcher dispatcher;
    auto result = dispatcher.dispatch(fragments);
    
    assert(result.success);
    assert(result.errors.empty());
    
    std::string html = dispatcher.getCompiledHTML();
    assert(html.find("<div>") != std::string::npos);
    assert(html.find("Hello World") != std::string::npos);
    assert(html.find("</div>") != std::string::npos);
    
    std::cout << "✓ Basic HTML test passed\n";
}

void testLocalStyle() {
    std::cout << "Testing local style processing...\n";
    
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
    
    chtl::CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan(source);
    
    chtl::CompilerDispatcher dispatcher;
    auto result = dispatcher.dispatch(fragments);
    
    assert(result.success);
    
    std::string html = dispatcher.getCompiledHTML();
    assert(html.find(".box") != std::string::npos);
    assert(html.find("width: 100px") != std::string::npos);
    
    std::cout << "✓ Local style test passed\n";
}

void testCHTLJS() {
    std::cout << "Testing CHTL JS compilation...\n";
    
    std::string source = R"(
        script {
            {{#myButton}}->addEventListener('click', function() {
                console.log('Clicked!');
            });
        }
    )";
    
    chtl::CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan(source);
    
    chtl::CompilerDispatcher dispatcher;
    auto result = dispatcher.dispatch(fragments);
    
    assert(result.success);
    
    std::string html = dispatcher.getCompiledHTML();
    assert(html.find("document.querySelector") != std::string::npos);
    assert(html.find("addEventListener") != std::string::npos);
    
    std::cout << "✓ CHTL JS test passed\n";
}

int main() {
    std::cout << "Running CHTL Compiler Integration Tests\n";
    std::cout << "=====================================\n\n";
    
    try {
        testBasicHTML();
        testLocalStyle();
        testCHTLJS();
        
        std::cout << "\nAll tests passed! ✓\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << "\n";
        return 1;
    }
}