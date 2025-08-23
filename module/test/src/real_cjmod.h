#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

// ============================================================================
// 真正的CJMOD API - 不是模拟
// ============================================================================

namespace CHTL { namespace CJMOD {

// 真正的词法单元
struct Token {
    enum Type {
        IDENTIFIER, NUMBER, STRING, OPERATOR, 
        LBRACE, RBRACE, LPAREN, RPAREN,
        COMMA, COLON, SEMICOLON, EOF_TOKEN
    };
    
    Type type;
    std::string value;
    size_t position;
    size_t line;
    size_t column;
};

// 真正的代码片段（从编译器传递）
struct RealCodeFragment {
    std::string sourceCode;           // 完整源代码
    std::vector<Token> tokens;        // 词法分析结果
    size_t startPos;                  // 片段开始位置
    size_t endPos;                    // 片段结束位置
    std::string triggerKeyword;       // 触发关键字
    std::unordered_map<std::string, std::string> context; // 上下文信息
};

// 真正的双指针扫描器
class RealDualPointerScanner {
public:
    RealDualPointerScanner(const std::string& sourceCode);
    
    // 设置扫描关键字
    void setKeywords(const std::vector<std::string>& keywords);
    
    // 执行双指针扫描
    std::vector<RealCodeFragment> scan();
    
    // 前置截取机制
    RealCodeFragment frontExtract(const RealCodeFragment& fragment, const std::string& pattern);

private:
    std::string sourceCode_;
    std::vector<std::string> keywords_;
    std::vector<Token> tokens_;
    
    // 真正的词法分析
    std::vector<Token> tokenize(const std::string& code);
    
    // 双指针算法实现
    std::pair<size_t, size_t> findFragmentBounds(size_t keywordPos);
};

// 真正的参数类
class RealArg {
public:
    RealArg(const std::string& name, const Token& token);
    
    // 从Token中提取真实值
    std::string extractValue() const;
    
    // 应用处理函数
    std::string process(const std::function<std::string(const std::string&)>& processor);
    
    // 生成JS代码
    std::string generateJS(const std::string& jsTemplate) const;

private:
    std::string name_;
    Token token_;
    std::string processedValue_;
};

// 真正的语法分析器
class RealSyntax {
public:
    RealSyntax(const RealCodeFragment& fragment);
    
    // 真正的语法分析
    bool parse();
    
    // 提取参数
    std::vector<RealArg> extractArgs();
    
    // 验证语法正确性
    bool validate() const;

private:
    RealCodeFragment fragment_;
    std::vector<RealArg> args_;
    bool parsed_;
};

// 真正的CJMOD处理器
class RealCJMODProcessor {
public:
    RealCJMODProcessor();
    
    // 注册CJMOD函数
    void registerFunction(const std::string& name, 
                         std::function<std::string(const RealCodeFragment&)> handler);
    
    // 处理代码片段（从编译器调用）
    std::string processFragment(const RealCodeFragment& fragment);
    
    // 设置扫描器
    void setScanner(std::unique_ptr<RealDualPointerScanner> scanner);

private:
    std::unordered_map<std::string, std::function<std::string(const RealCodeFragment&)>> handlers_;
    std::unique_ptr<RealDualPointerScanner> scanner_;
};

} // namespace CJMOD
} // namespace CHTL

// ============================================================================
// 真正的CJMOD扩展接口
// ============================================================================

// CJMOD扩展必须实现的接口
extern "C" {
    // 初始化CJMOD扩展
    void InitializeCJMOD(CHTL::CJMOD::RealCJMODProcessor* processor);
    
    // 处理特定函数调用
    const char* ProcessCJMODFunction(const char* functionName, 
                                   const char* sourceCode,
                                   size_t startPos, 
                                   size_t endPos);
    
    // 清理资源
    void CleanupCJMOD();
}

// ============================================================================
// 真正的testFunction实现
// ============================================================================

// 真正处理testFunction的函数
std::string ProcessTestFunction(const CHTL::CJMOD::RealCodeFragment& fragment);

// 真正处理simpleTest的函数  
std::string ProcessSimpleTest(const CHTL::CJMOD::RealCodeFragment& fragment);