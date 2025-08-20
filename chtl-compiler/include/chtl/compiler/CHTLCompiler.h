#pragma once

#include "compiler/ICompiler.h"
#include "chtl/core/CHTLGlobalMap.h"
#include "chtl/state/CHTLState.h"
#include "chtl/context/CHTLContext.h"
#include "chtl/lexer/CHTLLexer.h"
#include <memory>

namespace chtl {
namespace compiler {

/**
 * CHTL 编译器
 * 负责编译CHTL代码片段，生成对应的HTML/CSS/JS
 */
class CHTLCompiler : public ICompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler() override;
    
    // 实现ICompiler接口
    CompileResult Compile(const std::string& sourceCode, const std::string& filename = "") override;
    CompileResult CompileFragments(const std::vector<CodeFragment>& fragments) override;
    void Reset() override;
    const std::vector<std::string>& GetErrors() const override { return m_Errors; }
    void ClearErrors() override { m_Errors.clear(); }
    
    /**
     * 获取全局映射表
     */
    CHTLGlobalMap& GetGlobalMap() { return *m_GlobalMap; }
    const CHTLGlobalMap& GetGlobalMap() const { return *m_GlobalMap; }
    
    /**
     * 设置模块路径
     */
    void SetModulePaths(const std::vector<std::string>& paths);
    
private:
    // 核心组件
    std::unique_ptr<CHTLGlobalMap> m_GlobalMap;
    std::unique_ptr<CHTLStateMachine> m_StateMachine;
    std::unique_ptr<CHTLContext> m_Context;
    std::unique_ptr<CHTLLexer> m_Lexer;
    
    // 配置
    bool m_DebugMode;
    std::vector<std::string> m_ModulePaths;
    std::vector<std::string> m_Errors;
    
    // 编译方法
    CompileResult CompileElement(const std::string& code);
    CompileResult CompileTemplate(const std::string& code);
    CompileResult CompileCustom(const std::string& code);
    CompileResult CompileImport(const std::string& code);
    CompileResult CompileNamespace(const std::string& code);
    CompileResult CompileConfiguration(const std::string& code);
    CompileResult CompileOrigin(const std::string& code);
    
    // 辅助方法
    void ProcessTokens(const std::vector<CHTLToken>& tokens);
    std::string GenerateOutput();
};

} // namespace compiler
} // namespace chtl