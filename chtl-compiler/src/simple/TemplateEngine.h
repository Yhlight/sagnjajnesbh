#pragma once

#include "ASTNodeV2.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace chtl {
namespace simple {

/**
 * 模板定义
 */
struct TemplateDefinition {
    enum Type {
        STYLE,
        ELEMENT,
        VARIABLE
    };
    
    Type type;
    std::string name;
    std::vector<std::string> parameters;
    std::shared_ptr<ASTNode> body;
};

/**
 * 模板引擎
 * 管理模板的定义和实例化
 */
class TemplateEngine {
public:
    /**
     * 注册模板
     */
    void RegisterTemplate(const std::string& name, std::shared_ptr<TemplateDefinition> definition);
    
    /**
     * 检查模板是否存在
     */
    bool HasTemplate(const std::string& name) const;
    
    /**
     * 实例化模板
     */
    std::shared_ptr<ASTNode> InstantiateTemplate(
        const std::string& name,
        const std::unordered_map<std::string, std::string>& arguments
    );
    
    /**
     * 获取所有样式模板
     */
    std::vector<std::string> GetStyleTemplates() const;
    
    /**
     * 清空所有模板
     */
    void Clear();
    
private:
    /**
     * 替换模板中的参数
     */
    std::shared_ptr<ASTNode> SubstituteParameters(
        std::shared_ptr<ASTNode> node,
        const std::unordered_map<std::string, std::string>& arguments
    );
    
    /**
     * 克隆 AST 节点
     */
    std::shared_ptr<ASTNode> CloneNode(std::shared_ptr<ASTNode> node);
    
    // 模板存储
    std::unordered_map<std::string, std::shared_ptr<TemplateDefinition>> m_Templates;
};

/**
 * 模板处理器
 * 扫描 AST 并处理模板定义和使用
 */
class TemplateProcessor : public DefaultASTVisitor {
public:
    TemplateProcessor(TemplateEngine* engine);
    
    /**
     * 处理 AST，返回转换后的 AST
     */
    std::shared_ptr<ASTNode> Process(std::shared_ptr<ASTNode> root);
    
    // 访问者方法
    void VisitTemplate(TemplateNode* node) override;
    void VisitCustom(CustomNode* node) override;
    
private:
    TemplateEngine* m_Engine;
    std::shared_ptr<DocumentNode> m_Result;
    bool m_InTemplateDefinition;
};

} // namespace simple
} // namespace chtl