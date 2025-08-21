#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace merger {

/**
 * 编译结果片段
 */
struct ResultFragment {
    enum Type {
        HTML,
        CSS,
        JAVASCRIPT,
        CHTL_JS  // CHTL JS生成的JavaScript
    };
    
    Type type;
    std::string content;
    std::string id;        // 用于去重和合并
    int priority;          // 优先级，用于排序
    
    ResultFragment(Type t, const std::string& c, const std::string& i = "", int p = 0)
        : type(t), content(c), id(i), priority(p) {}
};

/**
 * 结果合并器
 * 负责将各个编译器的输出合并成最终的HTML文件
 */
class ResultMerger {
public:
    ResultMerger();
    ~ResultMerger();
    
    /**
     * 添加编译结果片段
     */
    void AddFragment(const ResultFragment& fragment);
    void AddFragments(const std::vector<ResultFragment>& fragments);
    
    /**
     * 清空所有片段
     */
    void Clear();
    
    /**
     * 生成最终的HTML
     */
    std::string GenerateHTML(const std::string& title = "CHTL Output");
    
    /**
     * 设置HTML模板
     */
    void SetHTMLTemplate(const std::string& templateStr);
    
    /**
     * 设置调试模式
     */
    void SetDebugMode(bool debug) { m_DebugMode = debug; }
    
private:
    std::vector<ResultFragment> m_HtmlFragments;
    std::vector<ResultFragment> m_CssFragments;
    std::vector<ResultFragment> m_JavaScriptFragments;
    std::vector<ResultFragment> m_ChtlJsFragments;
    
    std::string m_HtmlTemplate;
    bool m_DebugMode;
    
    // 合并方法
    std::string MergeHTML();
    std::string MergeCSS();
    std::string MergeJavaScript();
    std::string MergeCHTLJS();
    
    // 辅助方法
    void SortFragments(std::vector<ResultFragment>& fragments);
    void RemoveDuplicates(std::vector<ResultFragment>& fragments);
    std::string FormatOutput(const std::string& content);
};

} // namespace merger
} // namespace chtl