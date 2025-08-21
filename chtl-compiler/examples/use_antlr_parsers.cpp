#include <iostream>
#include "../src/chtl/css/CSSCompiler.h"
#include "../src/chtl/javascript/JavaScriptCompiler.h"

int main() {
    // CSS Example
    {
        std::cout << "=== CSS Compilation Example ===" << std::endl;
        
        chtl::css::CSSCompiler cssCompiler;
        
        std::string cssSource = R"(
            body {
                margin: 0;
                padding: 0;
                font-family: Arial, sans-serif;
            }
            
            .container {
                max-width: 1200px;
                margin: 0 auto;
            }
            
            @media (max-width: 768px) {
                .container {
                    padding: 0 20px;
                }
            }
        )";
        
        if (cssCompiler.Compile(cssSource)) {
            std::cout << "CSS compiled successfully!" << std::endl;
            std::cout << "Output:\n" << cssCompiler.GetOutput() << std::endl;
        } else {
            std::cout << "CSS compilation failed!" << std::endl;
            for (const auto& error : cssCompiler.GetErrors()) {
                std::cout << "Error: " << error << std::endl;
            }
        }
    }
    
    // JavaScript Example
    {
        std::cout << "\n=== JavaScript Compilation Example ===" << std::endl;
        
        chtl::javascript::JavaScriptCompiler jsCompiler;
        
        std::string jsSource = R"(
            function calculateSum(a, b) {
                return a + b;
            }
            
            const result = calculateSum(5, 3);
            console.log('Result:', result);
            
            class Calculator {
                constructor() {
                    this.value = 0;
                }
                
                add(n) {
                    this.value += n;
                    return this;
                }
                
                multiply(n) {
                    this.value *= n;
                    return this;
                }
                
                getValue() {
                    return this.value;
                }
            }
            
            const calc = new Calculator();
            calc.add(5).multiply(2);
            console.log('Calculator value:', calc.getValue());
        )";
        
        if (jsCompiler.Compile(jsSource)) {
            std::cout << "JavaScript compiled successfully!" << std::endl;
            std::cout << "Output:\n" << jsCompiler.GetOutput() << std::endl;
        } else {
            std::cout << "JavaScript compilation failed!" << std::endl;
            for (const auto& error : jsCompiler.GetErrors()) {
                std::cout << "Error: " << error << std::endl;
            }
        }
    }
    
    return 0;
}