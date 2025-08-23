#pragma once

#include <string>
#include <vector>

namespace CHTL {
namespace CSS {

/**
 * @brief CSS编译器类
 * 使用ANTLR4进行CSS编译 - 目标规划第32行：CSS Compiler (ANTLR)
 */
class CSSCompiler {
public:
    /**
     * @brief 构造函数
     */
    CSSCompiler();
    
    /**
     * @brief 析构函数
     */
    ~CSSCompiler();
    
    /**
     * @brief 编译CSS代码
     * @param cssCode CSS源代码
     * @param fileName 文件名
     * @return 编译后的CSS代码
     */
    std::string Compile(const std::string& cssCode, const std::string& fileName = "");
    
    /**
     * @brief 验证CSS语法
     * @param cssCode CSS源代码
     * @return 是否语法正确
     */
    bool Validate(const std::string& cssCode);
    
    /**
     * @brief 获取编译错误
     * @return 错误列表
     */
    const std::vector<std::string>& GetErrors() const { return errors_; }

private:
    std::vector<std::string> errors_;   // 编译错误
    
    /**
     * @brief 初始化ANTLR4 CSS解析器
     */
    void InitializeParser();
    
    /**
     * @brief 清理资源
     */
    void Cleanup();
};

} // namespace CSS
} // namespace CHTL