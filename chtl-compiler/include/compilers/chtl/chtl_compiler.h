#ifndef CHTL_COMPILER_H
#define CHTL_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace chtl {

// 前向声明
class CHTLParser;
class CHTLCodeGenerator;
class CHTLASTNode;

/**
 * CHTL编译器 - 手写实现
 * 负责编译CHTL代码和局部样式
 */
class CHTLCompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    /**
     * 编译CHTL代码
     * @param source CHTL源代码
     * @return 编译后的HTML代码
     */
    std::string compile(const std::string& source);
    
    /**
     * 编译局部样式
     * @param styleContent 局部样式内容
     * @param elementId 元素ID（用于生成唯一类名）
     * @return 处理后的样式和生成的类名
     */
    std::pair<std::string, std::string> compileLocalStyle(
        const std::string& styleContent, 
        const std::string& elementId
    );
    
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
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLCodeGenerator> codeGenerator_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 编译选项
    std::unordered_map<std::string, std::string> options_;
    
    // 模板和自定义元素注册表
    std::unordered_map<std::string, std::shared_ptr<CHTLASTNode>> templates_;
    std::unordered_map<std::string, std::shared_ptr<CHTLASTNode>> customElements_;
    
    // 辅助方法
    void registerTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> node);
    void registerCustomElement(const std::string& name, std::shared_ptr<CHTLASTNode> node);
    std::string generateUniqueClassName();
    
    // 局部样式处理
    void processLocalStyleSelectors(const std::string& style, std::string& processedStyle, 
                                  std::vector<std::string>& generatedClasses);
};

} // namespace chtl

#endif // CHTL_COMPILER_H