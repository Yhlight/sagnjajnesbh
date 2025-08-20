#pragma once

#include "compiler/ICompiler.h"

namespace chtl {
namespace javascript {

/**
 * JavaScript 编译器
 * 使用ANTLR4处理纯JavaScript代码片段
 * 注意：只处理纯JS，不处理CHTL JS的内容
 */
class JavaScriptCompiler : public ICompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler() override;
    
    // 实现ICompiler接口
    CompileResult Compile(const CodeFragment& fragment) override;
    CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    void Reset() override;
    std::string GetName() const override { return "JavaScript Compiler"; }
    void SetDebugMode(bool debug) override;
    
private:
    bool m_DebugMode;
    
    // TODO: ANTLR4集成
    std::string ProcessJavaScript(const std::string& js);
};

} // namespace javascript
} // namespace chtl