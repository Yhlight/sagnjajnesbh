#pragma once

#include "compiler/ICompiler.h"
#include "chtl_js/core/CHTLJSGlobalMap.h"
#include "chtl_js/state/CHTLJSState.h"
#include "chtl_js/context/CHTLJSContext.h"
#include "chtl_js/lexer/CHTLJSLexer.h"
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS 编译器
 * 负责编译CHTL JS代码片段，生成对应的JavaScript
 * 注意：CHTL JS不是JavaScript，只处理CHTL JS特有的语法
 */
class CHTLJSCompiler : public ICompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler() override;
    
    // 实现ICompiler接口
    CompileResult Compile(const CodeFragment& fragment) override;
    CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    void Reset() override;
    std::string GetName() const override { return "CHTL JS Compiler"; }
    void SetDebugMode(bool debug) override;
    
    /**
     * 获取全局映射表
     */
    CHTLJSGlobalMap& GetGlobalMap() { return *m_GlobalMap; }
    const CHTLJSGlobalMap& GetGlobalMap() const { return *m_GlobalMap; }
    
private:
    // 核心组件（完全独立于CHTL）
    std::shared_ptr<CHTLJSGlobalMap> m_GlobalMap;
    std::unique_ptr<CHTLJSStateMachine> m_StateMachine;
    std::unique_ptr<CHTLJSContext> m_Context;
    std::unique_ptr<CHTLJSLexer> m_Lexer;
    
    // 配置
    bool m_DebugMode;
    
    // 编译方法
    CompileResult CompileEnhancedSelector(const std::string& code);
    CompileResult CompileVirtualObject(const std::string& code);
    CompileResult CompileListen(const std::string& code);
    CompileResult CompileDelegate(const std::string& code);
    CompileResult CompileAnimate(const std::string& code);
    CompileResult CompileINeverAway(const std::string& code);
    CompileResult CompilePrintMylove(const std::string& code);
    
    // 辅助方法
    void ProcessTokens(const std::vector<CHTLJSToken>& tokens);
    std::string GenerateJavaScript();
    std::string TranslateSelector(const std::string& selector);
    std::string TranslateArrowChain(const std::vector<std::string>& chain);
};

} // namespace chtljs
} // namespace chtl