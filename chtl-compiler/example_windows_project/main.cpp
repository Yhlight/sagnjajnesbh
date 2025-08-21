#include <iostream>
#include <fstream>
#include <sstream>
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    try {
        // Example 1: Compile CSS
        std::cout << "=== CSS Compilation Example ===" << std::endl;
        
        chtl::css::CSSCompiler cssCompiler;
        cssCompiler.SetMinify(false);
        
        // You can load from file or use inline CSS
        std::string cssCode = R"(
            /* Main styles */
            body {
                font-family: 'Segoe UI', Tahoma, sans-serif;
                margin: 0;
                padding: 0;
                background-color: #f5f5f5;
            }
            
            .container {
                max-width: 1200px;
                margin: 0 auto;
                padding: 20px;
            }
            
            @media (max-width: 768px) {
                .container {
                    padding: 10px;
                }
            }
        )";
        
        if (cssCompiler.Compile(cssCode, "inline.css")) {
            std::cout << "CSS compiled successfully!" << std::endl;
            std::cout << "Output:" << std::endl;
            std::cout << cssCompiler.GetOutput() << std::endl;
        } else {
            std::cout << "CSS compilation failed!" << std::endl;
            for (const auto& error : cssCompiler.GetErrors()) {
                std::cerr << error << std::endl;
            }
        }
        
        // Example 2: Compile JavaScript
        std::cout << "\n=== JavaScript Compilation Example ===" << std::endl;
        
        chtl::javascript::JavaScriptCompiler jsCompiler;
        jsCompiler.SetStrictMode(true);
        jsCompiler.SetMinify(false);
        
        std::string jsCode = R"(
            // Modern JavaScript example
            class Calculator {
                constructor() {
                    this.result = 0;
                }
                
                add(value) {
                    this.result += value;
                    return this;
                }
                
                multiply(value) {
                    this.result *= value;
                    return this;
                }
                
                getResult() {
                    return this.result;
                }
            }
            
            // Arrow functions and template literals
            const calc = new Calculator();
            const finalResult = calc.add(10).multiply(2).getResult();
            console.log(`Final result: ${finalResult}`);
            
            // Async example
            async function fetchData() {
                try {
                    const response = await fetch('/api/data');
                    const data = await response.json();
                    return data;
                } catch (error) {
                    console.error('Error:', error);
                }
            }
        )";
        
        if (jsCompiler.Compile(jsCode, "inline.js")) {
            std::cout << "JavaScript compiled successfully!" << std::endl;
            std::cout << "Output:" << std::endl;
            std::cout << jsCompiler.GetOutput() << std::endl;
        } else {
            std::cout << "JavaScript compilation failed!" << std::endl;
            for (const auto& error : jsCompiler.GetErrors()) {
                std::cerr << error << std::endl;
            }
        }
        
        // Example 3: Error handling
        std::cout << "\n=== Error Handling Example ===" << std::endl;
        
        std::string invalidCss = "body { color: #xyz; invalid-property: value }";
        if (!cssCompiler.Compile(invalidCss)) {
            std::cout << "Expected CSS errors:" << std::endl;
            for (const auto& error : cssCompiler.GetErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}