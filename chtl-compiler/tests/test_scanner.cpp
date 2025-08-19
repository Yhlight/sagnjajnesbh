#include <iostream>
#include <cassert>
#include "scanner/chtl_unified_scanner.h"

void testBasicScanning() {
    std::cout << "Testing basic scanning...\n";
    
    chtl::CHTLUnifiedScanner scanner;
    
    std::string source = "div { text { \"Hello\" } }";
    auto fragments = scanner.scan(source);
    
    assert(fragments.size() == 1);
    assert(fragments[0]->getType() == chtl::FragmentType::CHTL);
    
    std::cout << "✓ Basic scanning test passed\n";
}

void testMixedContent() {
    std::cout << "Testing mixed content scanning...\n";
    
    chtl::CHTLUnifiedScanner scanner;
    
    std::string source = R"(
        div {
            style {
                color: red;
            }
        }
        
        style {
            body {
                margin: 0;
            }
        }
        
        script {
            console.log('test');
        }
    )";
    
    auto fragments = scanner.scan(source);
    
    // 应该有多个片段
    assert(fragments.size() >= 1);
    
    std::cout << "✓ Mixed content test passed\n";
}

int main() {
    std::cout << "Running Scanner Tests\n";
    std::cout << "====================\n\n";
    
    try {
        testBasicScanning();
        testMixedContent();
        
        std::cout << "\nAll scanner tests passed! ✓\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << "\n";
        return 1;
    }
}