#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace simple {

// 前向声明
class Lexer;
class Parser;
class Generator;

/**
 * CHTL 编译器主类
 * 协调词法分析、语法分析和代码生成
 */
class CHTLCompiler {
public:
    CHTLCompiler();
    ~CHTLCompiler();
    
    /**
     * 编译 CHTL 代码
     */
    std::string Compile(const std::string& chtlCode);
    
    /**
     * 编译文件
     */
    std::string CompileFile(const std::string& filename);
    
    /**
     * 获取所有错误信息
     */
    std::vector<std::string> GetAllErrors() const;
    bool HasErrors() const;
    
    /**
     * 配置选项
     */
    void SetPrettyPrint(bool pretty);
    void SetGenerateFullDocument(bool full);
    void SetDebugMode(bool debug);
    
private:
    std::unique_ptr<Lexer> m_Lexer;
    std::unique_ptr<Parser> m_Parser;
    std::unique_ptr<Generator> m_Generator;
    bool m_DebugMode;
};

} // namespace simple
} // namespace chtl