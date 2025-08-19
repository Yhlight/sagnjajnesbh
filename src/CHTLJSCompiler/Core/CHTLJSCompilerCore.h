#pragma once
#include "../Lexer/CHTLJSLexer.h"
#include "../AST/VirtualObjectManager.h"
#include "../Module/CJmodManager.h"
#include <string>
#include <memory>
#include <vector>

namespace chtl {

/**
 * CHTL JS编译器核心（简化版本）
 * 专注于CHTL JS语法的核心功能
 */
class CHTLJSCompilerCore {
public:
    CHTLJSCompilerCore();
    ~CHTLJSCompilerCore();
    
    // 主编译接口
    std::string compile(const std::string& source_code);
    
    // 编译流程控制
    bool initialize();
    void cleanup();
    
    // 组件访问
    chtljs::CHTLJSLexer& getLexer() { return *lexer_; }
    ast::VirtualObjectManager& getVirtualObjectManager() { return *virtual_object_manager_; }
    CJmodManager& getModuleManager() { return *module_manager_; }
    
    // 配置管理
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
private:
    std::unique_ptr<chtljs::CHTLJSLexer> lexer_;
    std::unique_ptr<ast::VirtualObjectManager> virtual_object_manager_;
    std::unique_ptr<CJmodManager> module_manager_;
    
    bool debug_mode_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 内部方法
    std::string generateJavaScript(const std::vector<chtljs::Token>& tokens);
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
};

} // namespace chtl