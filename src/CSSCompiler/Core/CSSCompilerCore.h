#pragma once
#include <string>
#include <vector>

namespace chtl {

/**
 * CSS编译器核心
 */
class CSSCompilerCore {
public:
    struct CompilationResult {
        std::string compiledCode;
        bool success = false;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    CSSCompilerCore();
    ~CSSCompilerCore();
    
    bool initialize();
    void setDebugMode(bool enabled);
    CompilationResult compileCode(const std::string& cssCode);

private:
    bool debugMode_ = false;
    bool initialized_ = false;
};

} // namespace chtl