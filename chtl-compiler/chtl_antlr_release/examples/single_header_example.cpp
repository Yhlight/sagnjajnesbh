#include <iostream>
#include "chtl_antlr_parsers.h"

int main() {
    // CSS Example
    {
        chtl::css::CSSCompiler css;
        
        std::string cssCode = R"(
            body {
                margin: 0;
                padding: 0;
            }
            
            .container {
                max-width: 1200px;
                margin: 0 auto;
            }
        )";
        
        if (css.Compile(cssCode)) {
            std::cout << "CSS compiled successfully!\n";
            std::cout << css.GetOutput() << "\n";
        } else {
            std::cout << "CSS compilation failed!\n";
            for (const auto& error : css.GetErrors()) {
                std::cout << error << "\n";
            }
        }
    }
    
    // JavaScript Example
    {
        chtl::javascript::JavaScriptCompiler js;
        js.SetStrictMode(true);
        
        std::string jsCode = R"(
            function hello(name) {
                console.log(`Hello, ${name}!`);
            }
            
            hello('World');
        )";
        
        if (js.Compile(jsCode)) {
            std::cout << "\nJavaScript compiled successfully!\n";
            std::cout << js.GetOutput() << "\n";
        } else {
            std::cout << "JavaScript compilation failed!\n";
            for (const auto& error : js.GetErrors()) {
                std::cout << error << "\n";
            }
        }
    }
    
    return 0;
}