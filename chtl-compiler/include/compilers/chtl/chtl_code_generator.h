#ifndef CHTL_CODE_GENERATOR_H
#define CHTL_CODE_GENERATOR_H

#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include "compilers/chtl/chtl_ast.h"

namespace chtl {

/**
 * CHTL代码生成器
 * 将AST转换为HTML代码
 */
class CHTLCodeGenerator : public CHTLASTVisitor {
public:
    CHTLCodeGenerator();
    ~CHTLCodeGenerator();
    
    /**
     * 生成HTML代码
     * @param ast AST根节点
     * @return 生成的HTML代码
     */
    std::string generate(std::shared_ptr<CHTLASTNode> ast);
    
    /**
     * 获取生成的全局样式
     */
    std::string getGlobalStyles() const { return globalStyles_.str(); }
    
    /**
     * 获取生成的全局脚本
     */
    std::string getGlobalScripts() const { return globalScripts_.str(); }
    
    // 访问者模式实现
    void visitDocument(DocumentNode* node) override;
    void visitElement(ElementNode* node) override;
    void visitText(TextNode* node) override;
    void visitStyleBlock(StyleBlockNode* node) override;
    void visitTemplateDefinition(TemplateDefinitionNode* node) override;
    
private:
    std::stringstream output_;
    std::stringstream globalStyles_;
    std::stringstream globalScripts_;
    
    // 缩进管理
    int indentLevel_;
    std::string getIndent() const;
    
    // 生成唯一的类名
    int classNameCounter_;
    std::string generateClassName();
    
    // 处理局部样式
    std::string processLocalStyle(const std::string& style, const std::string& elementClass);
    
    // HTML转义
    std::string escapeHTML(const std::string& text);
    
    // 属性处理
    std::string generateAttributes(const std::unordered_map<std::string, std::string>& attributes);
};

} // namespace chtl

#endif // CHTL_CODE_GENERATOR_H