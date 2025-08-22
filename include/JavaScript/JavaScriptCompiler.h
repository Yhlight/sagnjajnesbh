#pragma once

#include <string>
#include <vector>

namespace CHTL {
namespace JavaScript {

/**
 * @brief JavaScript编译器类
 * 使用ANTLR4进行JavaScript编译 - 目标规划第32行：JavaScript Compiler (ANTLR)
 */
class JavaScriptCompiler {
public:
    /**
     * @brief 构造函数
     */
    JavaScriptCompiler();
    
    /**
     * @brief 析构函数
     */
    ~JavaScriptCompiler();
    
    /**
     * @brief 编译JavaScript代码
     * @param jsCode JavaScript源代码
     * @param fileName 文件名
     * @return 编译后的JavaScript代码
     */
    std::string Compile(const std::string& jsCode, const std::string& fileName = "");
    
    /**
     * @brief 验证JavaScript语法
     * @param jsCode JavaScript源代码
     * @return 是否语法正确
     */
    bool Validate(const std::string& jsCode);
    
    /**
     * @brief 获取编译错误
     * @return 错误列表
     */
    const std::vector<std::string>& GetErrors() const { return errors_; }

private:
    std::vector<std::string> errors_;   // 编译错误
    
    /**
     * @brief 初始化ANTLR4 JavaScript解析器
     */
    void InitializeParser();
    
    /**
     * @brief 清理资源
     */
    void Cleanup();
};

} // namespace JavaScript
} // namespace CHTL