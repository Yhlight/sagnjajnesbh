#pragma once
#include "../AST/CHTLNodes.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace chtl {
namespace template_system {

// 模板类型
enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

// 样式组模板
struct StyleTemplate {
    std::string name;
    std::unordered_map<std::string, std::string> properties;
    std::vector<std::string> inheritedTemplates;
};

// 元素模板
struct ElementTemplate {
    std::string name;
    std::vector<std::unique_ptr<ast::ASTNode>> elements;
    std::vector<std::string> inheritedTemplates;
};

// 变量组模板
struct VariableTemplate {
    std::string name;
    std::unordered_map<std::string, std::string> variables;
    std::vector<std::string> inheritedTemplates;
};

// 模板管理器
class TemplateManager {
public:
    TemplateManager();
    ~TemplateManager();
    
    // 样式组模板管理
    void addStyleTemplate(const std::string& name, const StyleTemplate& tmpl);
    std::shared_ptr<StyleTemplate> getStyleTemplate(const std::string& name);
    bool hasStyleTemplate(const std::string& name) const;
    
    // 元素模板管理
    void addElementTemplate(const std::string& name, const ElementTemplate& tmpl);
    std::shared_ptr<ElementTemplate> getElementTemplate(const std::string& name);
    bool hasElementTemplate(const std::string& name) const;
    
    // 变量组模板管理
    void addVariableTemplate(const std::string& name, const VariableTemplate& tmpl);
    std::shared_ptr<VariableTemplate> getVariableTemplate(const std::string& name);
    bool hasVariableTemplate(const std::string& name) const;
    
    // 模板继承处理
    std::unordered_map<std::string, std::string> resolveStyleInheritance(const std::string& templateName);
    std::vector<std::unique_ptr<ast::ASTNode>> resolveElementInheritance(const std::string& templateName);
    std::unordered_map<std::string, std::string> resolveVariableInheritance(const std::string& templateName);
    
    // 模板使用
    bool applyStyleTemplate(const std::string& templateName, std::unordered_map<std::string, std::string>& targetProperties);
    std::vector<std::unique_ptr<ast::ASTNode>> instantiateElementTemplate(const std::string& templateName);
    std::unordered_map<std::string, std::string> getVariableValues(const std::string& templateName);
    
    // 模板验证
    bool validateTemplate(const std::string& name, TemplateType type);
    std::vector<std::string> getTemplateErrors() const { return errors_; }
    
    // 清理
    void clear();
    void clearErrors() { errors_.clear(); }
    
    // 统计
    size_t getStyleTemplateCount() const { return styleTemplates_.size(); }
    size_t getElementTemplateCount() const { return elementTemplates_.size(); }
    size_t getVariableTemplateCount() const { return variableTemplates_.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<StyleTemplate>> styleTemplates_;
    std::unordered_map<std::string, std::shared_ptr<ElementTemplate>> elementTemplates_;
    std::unordered_map<std::string, std::shared_ptr<VariableTemplate>> variableTemplates_;
    
    std::vector<std::string> errors_;
    
    // 内部方法
    void processInheritance(StyleTemplate& tmpl);
    void processInheritance(ElementTemplate& tmpl);
    void processInheritance(VariableTemplate& tmpl);
    
    bool checkCircularDependency(const std::string& templateName, TemplateType type, 
                                std::unordered_set<std::string>& visited);
};

// 全局模板管理器实例
extern std::shared_ptr<TemplateManager> g_templateManager;

} // namespace template_system
} // namespace chtl