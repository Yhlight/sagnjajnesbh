#pragma once
#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * JavaScript编译器核心
 * 基于ANTLR实现JavaScript语法编译
 */
class JavaScriptCompilerCore {
public:
    JavaScriptCompilerCore();
    ~JavaScriptCompilerCore();
    
    // 主编译接口
    std::string compile(const std::string& js_code);
    std::string compileFile(const std::string& file_path);
    
    // 编译控制
    bool initialize();
    void cleanup();
    void reset();
    
    // 配置管理
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
private:
    bool debug_mode_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 内部方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // ANTLR相关（待实现）
    bool initializeANTLR();
    void cleanupANTLR();
};

} // namespace chtl