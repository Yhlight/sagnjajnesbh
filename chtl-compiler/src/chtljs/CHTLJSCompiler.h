#pragma once

#include <string>
#include <memory>
#include <vector>

namespace chtljs {

// 前向声明
namespace ast { class ScriptBlockNode; }
class CHTLJSParser;
class CHTLJSGenerator;

/**
 * CHTL JS 编译器
 * 完全独立的编译器，不与CHTL编译器共享任何组件
 */
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    /**
     * 编译CHTL JS代码为JavaScript
     */
    std::string Compile(const std::string& chtljsCode);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
    /**
     * 设置选项
     */
    void SetPrettyPrint(bool pretty);
    void SetDebugMode(bool debug);
    
private:
    std::unique_ptr<CHTLJSParser> m_Parser;
    std::unique_ptr<CHTLJSGenerator> m_Generator;
    std::vector<std::string> m_Errors;
    bool m_DebugMode;
};

} // namespace chtljs