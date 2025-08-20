#pragma once

#include "compiler/ICompiler.h"

namespace chtl {
namespace css {

/**
 * CSS 编译器
 * 使用ANTLR4处理全局style块中的CSS代码
 */
class CSSCompiler : public ICompiler {
public:
    CSSCompiler();
    ~CSSCompiler() override;
    
    // 实现ICompiler接口
    CompileResult Compile(const CodeFragment& fragment) override;
    CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    void Reset() override;
    std::string GetName() const override { return "CSS Compiler"; }
    void SetDebugMode(bool debug) override;
    
private:
    bool m_DebugMode;
    
    // TODO: ANTLR4集成
    std::string ProcessCSS(const std::string& css);
};

} // namespace css
} // namespace chtl