#pragma once
#include "../Lexer/CHTLToken.h"
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <unordered_map>

namespace chtl {

/**
 * CHTL编译上下文
 * 管理编译过程中的状态和符号表
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 作用域管理
    void enterScope(const std::string& scope_name = "");
    void exitScope();
    std::string getCurrentScope() const;
    
    // 符号管理
    void addSymbol(const std::string& name, const std::string& type);
    bool hasSymbol(const std::string& name) const;
    std::string getSymbolType(const std::string& name) const;
    
    // 模板和自定义定义
    void addTemplate(const std::string& name, const std::string& definition);
    void addCustom(const std::string& name, const std::string& definition);
    bool hasTemplate(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    
    // 命名空间管理
    void enterNamespace(const std::string& ns_name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
    // 调试
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
private:
    std::stack<std::string> scope_stack_;
    std::stack<std::string> namespace_stack_;
    std::unordered_map<std::string, std::string> symbols_;
    std::unordered_map<std::string, std::string> templates_;
    std::unordered_map<std::string, std::string> customs_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    bool debug_mode_;
    
    // 辅助方法
    std::string buildFullName(const std::string& name) const;
};

} // namespace chtl