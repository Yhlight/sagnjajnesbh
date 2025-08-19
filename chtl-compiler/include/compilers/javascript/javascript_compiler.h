#ifndef JAVASCRIPT_COMPILER_H
#define JAVASCRIPT_COMPILER_H

#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * JavaScript编译器 - 使用ANTLR实现
 * 负责编译标准JavaScript代码
 */
class JavaScriptCompiler {
public:
    JavaScriptCompiler();
    ~JavaScriptCompiler();
    
    /**
     * 编译JavaScript代码
     * @param source JavaScript源代码
     * @return 编译后的JavaScript代码
     */
    std::string compile(const std::string& source);
    
    /**
     * 获取编译错误
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * 获取编译警告
     */
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    /**
     * 重置编译器状态
     */
    void reset();
    
    /**
     * 设置编译选项
     */
    void setOption(const std::string& key, const std::string& value);
    
private:
    class Impl; // PIMPL模式，隐藏ANTLR依赖
    std::unique_ptr<Impl> pImpl_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 备用实现（当ANTLR不可用时）
    std::string fallbackCompile(const std::string& source);
};

} // namespace chtl

#endif // JAVASCRIPT_COMPILER_H