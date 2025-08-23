#pragma once

#include "Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// 前向声明
namespace Parser { class CHTLParser; }
namespace CHTLJS { namespace Parser { class CHTLJSParser; } }

namespace Dispatcher {

/**
 * @brief 处理后的片段结构
 */
struct ProcessedFragment {
    Scanner::FragmentType originalType;     // 原始片段类型
    std::string generatedCode;              // 生成的代码
    size_t originalPosition;                // 原始位置
    bool isOpenTag;                         // 是否为开始标签
    bool isCloseTag;                        // 是否为结束标签
    bool isContent;                         // 是否为内容
    bool isSelfClosing;                     // 是否为自闭合
    std::string elementType;                // 元素类型（如div, span）
    std::string attributes;                 // 属性
    
    ProcessedFragment() : originalType(Scanner::FragmentType::Unknown), 
                         originalPosition(0), isOpenTag(false), isCloseTag(false), 
                         isContent(false), isSelfClosing(false) {}
};

/**
 * @brief 片段处理器基类
 */
class FragmentProcessor {
public:
    virtual ~FragmentProcessor() = default;
    
    /**
     * @brief 处理单个片段
     * @param fragment 代码片段
     * @return 处理后的片段
     */
    virtual ProcessedFragment ProcessFragment(const Scanner::CodeFragment& fragment) = 0;
    
    /**
     * @brief 获取处理器名称
     */
    virtual std::string GetName() const = 0;
};

/**
 * @brief CHTL片段处理器
 * 处理CHTL语法片段，生成HTML代码
 */
class CHTLFragmentProcessor : public FragmentProcessor {
public:
    /**
     * @brief 构造函数
     * @param parser CHTL解析器指针
     */
    CHTLFragmentProcessor(Parser::CHTLParser* parser = nullptr);
    
    ProcessedFragment ProcessFragment(const Scanner::CodeFragment& fragment) override;
    std::string GetName() const override { return "CHTL"; }

private:
    /**
     * @brief 检查是否为HTML元素名
     */
    bool IsHTMLElement(const std::string& content);
    
    /**
     * @brief 检查是否为属性
     */
    bool IsAttribute(const std::string& content);
    
    /**
     * @brief 解析属性
     */
    std::string ParseAttribute(const std::string& content);
    
    /**
     * @brief 检查是否为文本内容
     */
    bool IsTextContent(const std::string& content);

private:
    Parser::CHTLParser* parser_;  // CHTL解析器指针
};

/**
 * @brief CHTL JS片段处理器
 * 处理CHTL JS语法片段，生成JavaScript代码
 */
class CHTLJSFragmentProcessor : public FragmentProcessor {
public:
    /**
     * @brief 构造函数
     * @param parser CHTL JS解析器指针
     */
    CHTLJSFragmentProcessor(CHTLJS::Parser::CHTLJSParser* parser = nullptr);
    
    ProcessedFragment ProcessFragment(const Scanner::CodeFragment& fragment) override;
    std::string GetName() const override { return "CHTL JS"; }

private:
    /**
     * @brief 处理增强选择器
     */
    std::string ProcessEnhancedSelector(const std::string& content);
    
    /**
     * @brief 处理虚对象调用
     */
    std::string ProcessVirtualObjectCall(const std::string& content);
    
    /**
     * @brief 处理监听器语法
     */
    std::string ProcessListenerSyntax(const std::string& content);

private:
    CHTLJS::Parser::CHTLJSParser* parser_;  // CHTL JS解析器指针
};

/**
 * @brief CSS片段处理器
 * 处理CSS语法片段
 */
class CSSFragmentProcessor : public FragmentProcessor {
public:
    ProcessedFragment ProcessFragment(const Scanner::CodeFragment& fragment) override;
    std::string GetName() const override { return "CSS"; }

private:
    /**
     * @brief 处理局部样式
     */
    std::string ProcessLocalStyle(const std::string& content);
    
    /**
     * @brief 处理全局样式
     */
    std::string ProcessGlobalStyle(const std::string& content);
};

/**
 * @brief JavaScript片段处理器
 * 处理纯JavaScript代码片段
 */
class JavaScriptFragmentProcessor : public FragmentProcessor {
public:
    ProcessedFragment ProcessFragment(const Scanner::CodeFragment& fragment) override;
    std::string GetName() const override { return "JavaScript"; }

private:
    /**
     * @brief 验证JavaScript语法
     */
    bool ValidateJavaScript(const std::string& content);
};

/**
 * @brief 智能合并器
 * 根据片段类型和位置智能合并处理后的片段
 */
class IntelligentMerger {
public:
    /**
     * @brief 合并处理后的片段
     * @param fragments 处理后的片段列表
     * @param supportSPA 是否支持SPA模式
     * @return 合并后的结果
     */
    struct MergedResult {
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        bool isSPAComponent;
        
        MergedResult() : isSPAComponent(false) {}
    };
    
    MergedResult Merge(const std::vector<ProcessedFragment>& fragments, bool supportSPA = true);
    
    /**
     * @brief 构建HTML结构
     */
    std::string BuildHTMLStructure(const std::string& htmlContent, const std::string& cssContent, 
                                  const std::string& jsContent, bool isSPA);

private:
    /**
     * @brief 检测是否为SPA组件
     */
    bool DetectSPAComponent(const std::vector<ProcessedFragment>& fragments);
    
    /**
     * @brief 合并HTML片段
     */
    std::string MergeHTMLFragments(const std::vector<ProcessedFragment>& fragments);
    
    /**
     * @brief 合并CSS片段
     */
    std::string MergeCSSFragments(const std::vector<ProcessedFragment>& fragments);
    
    /**
     * @brief 合并JavaScript片段
     */
    std::string MergeJavaScriptFragments(const std::vector<ProcessedFragment>& fragments);
};

} // namespace Dispatcher
} // namespace CHTL