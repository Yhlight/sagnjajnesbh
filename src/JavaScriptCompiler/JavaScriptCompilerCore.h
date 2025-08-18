#pragma once
#include <string>
#include <vector>

namespace chtl {

/**
 * JavaScript编译器核心
 */
class JavaScriptCompilerCore {
public:
    struct CompilationResult {
        std::string compiledCode;
        bool success = false;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    JavaScriptCompilerCore();
    ~JavaScriptCompilerCore();
    
    bool initialize();
    void setDebugMode(bool enabled);
    CompilationResult compileCode(const std::string& jsCode);

private:
    bool debugMode_ = false;
    bool initialized_ = false;
};

} // namespace chtl