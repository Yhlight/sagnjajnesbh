#ifndef CHTL_JS_COMPILER_H
#define CHTL_JS_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace chtl {

// 前向声明
class CHTLJSParser;
class CHTLJSCodeGenerator;
class CHTLJSASTNode;

/**
 * CHTL JS编译器 - 手写实现
 * 负责编译CHTL JS代码
 */
class CHTLJSCompiler {
public:
    CHTLJSCompiler();
    ~CHTLJSCompiler();
    
    /**
     * 编译CHTL JS代码
     * @param source CHTL JS源代码
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
    
    /**
     * 注册虚对象
     */
    void registerVirtualObject(const std::string& name, const std::string& type);
    
private:
    std::unique_ptr<CHTLJSParser> parser_;
    std::unique_ptr<CHTLJSCodeGenerator> codeGenerator_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 编译选项
    std::unordered_map<std::string, std::string> options_;
    
    // 虚对象注册表
    std::unordered_map<std::string, std::string> virtualObjects_;
    
    // 增强选择器缓存
    std::unordered_map<std::string, std::string> selectorCache_;
    
    // 辅助方法
    std::string processEnhancedSelector(const std::string& selector);
    std::string processArrowOperator(const std::string& code);
    std::string processVirtualObject(const std::string& virName, const std::string& method);
    std::string generateFunctionName(const std::string& prefix);
    
    // 事件委托管理
    void generateEventDelegation(const std::string& parent, const std::string& target, 
                               const std::string& events);
};

} // namespace chtl

#endif // CHTL_JS_COMPILER_H