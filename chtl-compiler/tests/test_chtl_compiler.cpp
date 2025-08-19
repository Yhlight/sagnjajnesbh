#include <iostream>
#include <cassert>
#include "compilers/chtl/chtl_compiler.h"

void testBasicCompilation() {
    std::cout << "Testing basic CHTL compilation...\n";
    
    chtl::CHTLCompiler compiler;
    
    std::string source = R"(
        div {
            id: test;
            class: container;
            
            text {
                "Test Content"
            }
        }
    )";
    
    std::string result = compiler.compile(source);
    
    assert(!result.empty());
    assert(compiler.getErrors().empty());
    
    std::cout << "✓ Basic compilation test passed\n";
}

void testLocalStyleCompilation() {
    std::cout << "Testing local style compilation...\n";
    
    chtl::CHTLCompiler compiler;
    
    std::string styleContent = R"(
        .box {
            width: 200px;
            height: 200px;
        }
        
        &:hover {
            background-color: blue;
        }
    )";
    
    auto [processedStyle, className] = compiler.compileLocalStyle(styleContent, "element1");
    
    assert(!processedStyle.empty());
    assert(!className.empty());
    
    std::cout << "✓ Local style compilation test passed\n";
}

int main() {
    std::cout << "Running CHTL Compiler Tests\n";
    std::cout << "==========================\n\n";
    
    try {
        testBasicCompilation();
        testLocalStyleCompilation();
        
        std::cout << "\nAll CHTL compiler tests passed! ✓\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << "\n";
        return 1;
    }
}