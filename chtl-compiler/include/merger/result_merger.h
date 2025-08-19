#ifndef RESULT_MERGER_H
#define RESULT_MERGER_H

#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * HTML文档结构
 */
struct HTMLDocument {
    std::string doctype;
    std::string htmlTag;
    std::string head;
    std::string body;
    std::vector<std::string> globalStyles;
    std::vector<std::string> globalScripts;
    std::vector<std::string> localStyles;
    std::vector<std::string> localScripts;
};

/**
 * ResultMerger - 编译结果合并器
 * 负责将各个编译器的输出合并成最终的HTML
 */
class ResultMerger {
public:
    ResultMerger();
    ~ResultMerger();
    
    /**
     * 添加HTML片段
     */
    void addHTMLFragment(const std::string& fragment);
    
    /**
     * 添加全局样式
     */
    void addGlobalStyle(const std::string& style);
    
    /**
     * 添加局部样式
     */
    void addLocalStyle(const std::string& style);
    
    /**
     * 添加全局脚本
     */
    void addGlobalScript(const std::string& script);
    
    /**
     * 添加局部脚本
     */
    void addLocalScript(const std::string& script);
    
    /**
     * 合并所有结果生成最终HTML
     * @return 完整的HTML文档
     */
    std::string merge();
    
    /**
     * 重置合并器状态
     */
    void reset();
    
    /**
     * 设置文档选项
     */
    void setDocumentOption(const std::string& key, const std::string& value);
    
private:
    HTMLDocument document_;
    std::vector<std::string> htmlFragments_;
    
    // 文档选项
    std::string charset_;
    std::string title_;
    std::string lang_;
    std::vector<std::string> metaTags_;
    
    // 辅助方法
    std::string buildHead();
    std::string buildBody();
    std::string mergeStyles();
    std::string mergeScripts();
    void optimizeOutput(std::string& html);
};

} // namespace chtl

#endif // RESULT_MERGER_H