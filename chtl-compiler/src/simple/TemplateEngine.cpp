#include "TemplateEngine.h"
#include <regex>

namespace chtl {
namespace simple {

void TemplateEngine::RegisterTemplate(const std::string& name, std::shared_ptr<TemplateDefinition> definition) {
    m_Templates[name] = definition;
}

bool TemplateEngine::HasTemplate(const std::string& name) const {
    return m_Templates.find(name) != m_Templates.end();
}

std::shared_ptr<ASTNode> TemplateEngine::InstantiateTemplate(
    const std::string& name,
    const std::unordered_map<std::string, std::string>& arguments) {
    
    auto it = m_Templates.find(name);
    if (it == m_Templates.end()) {
        return nullptr;
    }
    
    auto definition = it->second;
    
    // 克隆模板内容
    auto instance = CloneNode(definition->body);
    
    // 替换参数
    return SubstituteParameters(instance, arguments);
}

std::vector<std::string> TemplateEngine::GetStyleTemplates() const {
    std::vector<std::string> styles;
    
    for (const auto& pair : m_Templates) {
        if (pair.second->type == TemplateDefinition::STYLE) {
            // 生成 CSS 类定义
            styles.push_back("." + pair.first + " { " + 
                           /* 这里应该生成实际的CSS */ "}");
        }
    }
    
    return styles;
}

void TemplateEngine::Clear() {
    m_Templates.clear();
}

std::shared_ptr<ASTNode> TemplateEngine::SubstituteParameters(
    std::shared_ptr<ASTNode> node,
    const std::unordered_map<std::string, std::string>& arguments) {
    
    // 如果是文本节点，替换参数
    if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        std::string text = textNode->GetText();
        
        // 替换 $paramName 形式的参数
        for (const auto& arg : arguments) {
            std::string param = "$" + arg.first;
            size_t pos = 0;
            while ((pos = text.find(param, pos)) != std::string::npos) {
                text.replace(pos, param.length(), arg.second);
                pos += arg.second.length();
            }
        }
        
        return std::make_shared<TextNode>(text);
    }
    
    // 对于其他节点，递归处理子节点
    auto cloned = CloneNode(node);
    cloned->GetChildren().clear();
    
    for (const auto& child : node->GetChildren()) {
        cloned->AddChild(SubstituteParameters(child, arguments));
    }
    
    return cloned;
}

std::shared_ptr<ASTNode> TemplateEngine::CloneNode(std::shared_ptr<ASTNode> node) {
    // 简化的克隆实现
    // 在实际实现中，应该为每种节点类型实现深克隆
    
    if (auto elem = std::dynamic_pointer_cast<ElementNode>(node)) {
        auto clone = std::make_shared<ElementNode>(elem->GetTagName());
        for (const auto& attr : elem->GetAttributes()) {
            clone->AddAttribute(attr.first, attr.second);
        }
        return clone;
    }
    
    if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        return std::make_shared<TextNode>(text->GetText());
    }
    
    // 其他节点类型...
    return nullptr;
}

// TemplateProcessor 实现

TemplateProcessor::TemplateProcessor(TemplateEngine* engine) 
    : m_Engine(engine), m_InTemplateDefinition(false) {
}

std::shared_ptr<ASTNode> TemplateProcessor::Process(std::shared_ptr<ASTNode> root) {
    m_Result = std::make_shared<DocumentNode>();
    m_InTemplateDefinition = false;
    
    // 第一遍：收集模板定义
    root->Accept(this);
    
    // 第二遍：处理模板使用
    // （简化实现，实际应该再次遍历）
    
    return m_Result;
}

void TemplateProcessor::VisitTemplate(TemplateNode* node) {
    // 处理模板定义
    m_InTemplateDefinition = true;
    
    auto definition = std::make_shared<TemplateDefinition>();
    definition->name = node->GetName();
    
    switch (node->GetType()) {
        case TemplateNode::STYLE_TEMPLATE:
            definition->type = TemplateDefinition::STYLE;
            break;
        case TemplateNode::ELEMENT_TEMPLATE:
            definition->type = TemplateDefinition::ELEMENT;
            break;
        case TemplateNode::VAR_TEMPLATE:
            definition->type = TemplateDefinition::VARIABLE;
            break;
    }
    
    definition->parameters = node->GetParameters();
    
    // 假设第一个子节点是模板体
    if (!node->GetChildren().empty()) {
        definition->body = node->GetChildren()[0];
    }
    
    m_Engine->RegisterTemplate(definition->name, definition);
    
    m_InTemplateDefinition = false;
}

void TemplateProcessor::VisitCustom(CustomNode* node) {
    if (m_InTemplateDefinition) {
        // 在模板定义中，保留原样
        return;
    }
    
    // 检查是否是模板实例
    if (m_Engine->HasTemplate(node->GetName())) {
        // 实例化模板
        auto instance = m_Engine->InstantiateTemplate(
            node->GetName(), 
            node->GetProperties()
        );
        
        if (instance) {
            // 将实例添加到结果中
            m_Result->AddChild(instance);
        }
    } else {
        // 不是模板，作为普通自定义元素处理
        m_Result->AddChild(std::make_shared<CustomNode>(*node));
    }
}

} // namespace simple
} // namespace chtl