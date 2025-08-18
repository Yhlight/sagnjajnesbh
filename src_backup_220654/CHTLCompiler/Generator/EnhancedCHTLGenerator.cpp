#include "CHTLGenerator.h"
#include "../Template/TemplateManager.h"
#include "../Custom/CustomManager.h"
#include "../Origin/OriginManager.h"
#include "../Constraint/ConstraintManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chtl {
namespace generator {

CHTLGenerator::CHTLGenerator() {
    // 获取全局管理器实例
    templateManager_ = template_system::g_templateManager;
    customManager_ = custom_system::g_customManager;
    originManager_ = origin_system::g_originManager;
    constraintManager_ = constraint_system::g_constraintManager;
}

CHTLGenerator::~CHTLGenerator() = default;

std::string CHTLGenerator::generate(const ast::ASTNode& root) {
    output_.clear();
    indentLevel_ = 0;
    
    // 开始生成
    const_cast<ast::ASTNode&>(root).accept(*this);
    
    return output_;
}

// === 访问者模式实现 ===

void CHTLGenerator::visit(ast::ProgramNode& node) {
    // 按语法文档：程序节点生成完整的HTML文档结构
    
    output_ += "<!DOCTYPE html>\n";
    output_ += "<html lang=\"zh-CN\">\n";
    output_ += "<head>\n";
    output_ += "    <meta charset=\"UTF-8\">\n";
    output_ += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    output_ += "    <title>CHTL Generated Page</title>\n";
    
    // 生成全局样式
    generateGlobalStyles();
    
    output_ += "</head>\n";
    output_ += "<body>\n";
    
    // 访问所有子节点
    for (auto& child : node.children) {
        indentLevel_ = 1;
        child->accept(*this);
    }
    
    // 生成全局脚本
    generateGlobalScripts();
    
    output_ += "</body>\n";
    output_ += "</html>\n";
}

void CHTLGenerator::visit(ast::ElementNode& node) {
    // 按语法文档：生成HTML元素
    
    addIndent();
    output_ += "<" + node.tag;
    
    // 生成属性
    for (const auto& [attrName, attrValue] : node.attributes) {
        output_ += " " + attrName + "=\"" + attrValue + "\"";
    }
    
    if (node.children.empty()) {
        // 自闭合标签
        if (isSelfClosingTag(node.tag)) {
            output_ += " />\n";
        } else {
            output_ += "></" + node.tag + ">\n";
        }
    } else {
        output_ += ">\n";
        
        // 生成子元素
        indentLevel_++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indentLevel_--;
        
        addIndent();
        output_ += "</" + node.tag + ">\n";
    }
}

void CHTLGenerator::visit(ast::TemplateNode& node) {
    // 按语法文档：模板节点生成对应的HTML结构
    
    if (node.type == "Style") {
        generateStyleTemplate(node);
    } else if (node.type == "Element") {
        generateElementTemplate(node);
    } else if (node.type == "Var") {
        generateVariableTemplate(node);
    } else {
        // 模板使用
        generateTemplateUsage(node);
    }
}

void CHTLGenerator::visit(ast::CustomNode& node) {
    // 按语法文档：自定义节点生成对应的HTML结构
    
    if (node.type == "Style" || node.customType == "Style") {
        generateCustomStyle(node);
    } else if (node.type == "Element" || node.customType == "Element") {
        generateCustomElement(node);
    } else if (node.type == "Var" || node.customType == "Var") {
        generateCustomVariable(node);
    }
}

void CHTLGenerator::visit(ast::OriginNode& node) {
    // 按语法文档：原始嵌入节点直接输出原始内容
    
    std::string originType = !node.originType.empty() ? node.originType : node.type;
    
    if (originType == "@Html") {
        // HTML原始嵌入：直接输出
        addIndent();
        output_ += "<!-- HTML Origin: " + node.name + " -->\n";
        output_ += node.content + "\n";
    } else if (originType == "@Style") {
        // CSS原始嵌入：包装在style标签中
        addIndent();
        output_ += "<style>\n";
        output_ += "/* CSS Origin: " + node.name + " */\n";
        output_ += node.content + "\n";
        output_ += "</style>\n";
    } else if (originType == "@JavaScript") {
        // JavaScript原始嵌入：包装在script标签中
        addIndent();
        output_ += "<script>\n";
        output_ += "// JavaScript Origin: " + node.name + "\n";
        output_ += node.content + "\n";
        output_ += "</script>\n";
    } else {
        // 自定义原始类型：生成注释和内容
        addIndent();
        output_ += "<!-- Custom Origin: " + originType + " " + node.name + " -->\n";
        output_ += node.content + "\n";
    }
}

void CHTLGenerator::visit(ast::ConfigurationNode& node) {
    // 按语法文档：配置组节点不直接生成HTML，但影响编译行为
    
    addIndent();
    output_ += "<!-- Configuration: " + (node.name.empty() ? "unnamed" : node.name) + " -->\n";
    
    // 配置组主要影响编译器行为，不直接生成HTML内容
    // 但可以在注释中显示配置信息用于调试
    if (!node.basicConfig.empty()) {
        addIndent();
        output_ += "<!-- Basic Config: ";
        for (const auto& [key, value] : node.basicConfig) {
            output_ += key + "=" + value + " ";
        }
        output_ += "-->\n";
    }
}

void CHTLGenerator::visit(ast::ConstraintNode& node) {
    // 按语法文档：约束节点不直接生成HTML，但影响编译验证
    
    addIndent();
    output_ += "<!-- Constraint: " + node.constraintType + " ";
    for (const auto& target : node.targets) {
        output_ += target + " ";
    }
    output_ += "-->\n";
    
    // 约束节点主要用于编译时验证，不生成实际HTML内容
}

void CHTLGenerator::visit(ast::SpecializationNode& node) {
    // 按语法文档：特例化操作节点生成相应的HTML修改
    
    addIndent();
    output_ += "<!-- Specialization: " + node.operation + " " + node.target + " -->\n";
    
    if (node.operation == "inherit") {
        generateInheritSpecialization(node);
    } else if (node.operation == "delete") {
        generateDeleteSpecialization(node);
    } else if (node.operation == "insert") {
        generateInsertSpecialization(node);
    } else if (node.operation == "replace") {
        generateReplaceSpecialization(node);
    }
}

void CHTLGenerator::visit(ast::IndexAccessNode& node) {
    // 按语法文档：索引访问节点生成对应索引的元素
    
    addIndent();
    output_ += "<!-- Index Access: " + node.elementType + "[" + std::to_string(node.index) + "] -->\n";
    
    // 索引访问通常用于特例化操作中，这里生成注释标记
    addIndent();
    output_ += "<div data-index-access=\"" + node.elementType + "[" + std::to_string(node.index) + "]\">\n";
    
    // 访问子节点
    indentLevel_++;
    for (auto& child : node.children) {
        child->accept(*this);
    }
    indentLevel_--;
    
    addIndent();
    output_ += "</div>\n";
}

void CHTLGenerator::visit(ast::VariableGroupNode& node) {
    // 按语法文档：变量组节点生成使用变量值的内容
    
    addIndent();
    output_ += "<!-- Variable Group: " + node.templateName + "(" + node.variableName + ") -->\n";
    
    // 从模板管理器获取变量值
    if (templateManager_->hasVariableTemplate(node.templateName)) {
        auto variables = templateManager_->getVariableValues(node.templateName);
        auto it = variables.find(node.variableName);
        if (it != variables.end()) {
            addIndent();
            output_ += "<span class=\"variable-value\" data-template=\"" + node.templateName + 
                      "\" data-variable=\"" + node.variableName + "\">" + it->second + "</span>\n";
        } else {
            addIndent();
            output_ += "<!-- Variable not found: " + node.variableName + " in " + node.templateName + " -->\n";
        }
    } else {
        addIndent();
        output_ += "<!-- Template not found: " + node.templateName + " -->\n";
    }
}

void CHTLGenerator::visit(ast::NamespaceNode& node) {
    // 按语法文档：命名空间节点生成带命名空间的容器
    
    addIndent();
    output_ += "<!-- Namespace: " + node.name + " -->\n";
    
    addIndent();
    output_ += "<div class=\"namespace\" data-namespace=\"" + node.name + "\">\n";
    
    // 访问子节点
    indentLevel_++;
    for (auto& child : node.children) {
        child->accept(*this);
    }
    indentLevel_--;
    
    addIndent();
    output_ += "</div>\n";
}

void CHTLGenerator::visit(ast::ImportNode& node) {
    // 按语法文档：导入节点生成导入标记和相关内容
    
    addIndent();
    output_ += "<!-- Import: " + node.importType + " from " + node.modulePath + " -->\n";
    
    // 根据导入类型生成相应的HTML结构
    if (node.importType == "@Style") {
        addIndent();
        output_ += "<link rel=\"stylesheet\" href=\"" + node.modulePath + "\">\n";
    } else if (node.importType == "@JavaScript") {
        addIndent();
        output_ += "<script src=\"" + node.modulePath + "\"></script>\n";
    } else if (node.importType == "@Html") {
        addIndent();
        output_ += "<div class=\"imported-html\" data-source=\"" + node.modulePath + "\"></div>\n";
    } else {
        // 其他导入类型生成数据属性
        addIndent();
        output_ += "<div class=\"import\" data-type=\"" + node.importType + 
                  "\" data-source=\"" + node.modulePath + "\"></div>\n";
    }
}

// === 模板生成方法 ===

void CHTLGenerator::generateStyleTemplate(const ast::TemplateNode& node) {
    // 生成样式组模板的HTML表示
    
    addIndent();
    output_ += "<style>\n";
    output_ += "/* Style Template: " + node.name + " */\n";
    
    // 从模板管理器获取样式属性
    if (templateManager_->hasStyleTemplate(node.name)) {
        auto styleTemplate = templateManager_->getStyleTemplate(node.name);
        auto resolvedProps = templateManager_->resolveStyleInheritance(node.name);
        
        output_ += "." + node.name + " {\n";
        for (const auto& [prop, value] : resolvedProps) {
            output_ += "    " + prop + ": " + value + ";\n";
        }
        output_ += "}\n";
    }
    
    output_ += "</style>\n";
}

void CHTLGenerator::generateElementTemplate(const ast::TemplateNode& node) {
    // 生成元素模板的HTML表示
    
    addIndent();
    output_ += "<!-- Element Template: " + node.name + " -->\n";
    
    // 元素模板生成对应的HTML结构
    if (templateManager_->hasElementTemplate(node.name)) {
        auto elements = templateManager_->instantiateElementTemplate(node.name);
        
        // 生成模板元素
        addIndent();
        output_ += "<div class=\"element-template\" data-template=\"" + node.name + "\">\n";
        
        indentLevel_++;
        for (auto& element : elements) {
            element->accept(*this);
        }
        indentLevel_--;
        
        addIndent();
        output_ += "</div>\n";
    }
}

void CHTLGenerator::generateVariableTemplate(const ast::TemplateNode& node) {
    // 生成变量组模板的HTML表示
    
    addIndent();
    output_ += "<!-- Variable Template: " + node.name + " -->\n";
    
    if (templateManager_->hasVariableTemplate(node.name)) {
        auto variables = templateManager_->getVariableValues(node.name);
        
        addIndent();
        output_ += "<div class=\"variable-template\" data-template=\"" + node.name + "\">\n";
        
        indentLevel_++;
        for (const auto& [varName, varValue] : variables) {
            addIndent();
            output_ += "<span class=\"variable\" data-name=\"" + varName + "\">" + varValue + "</span>\n";
        }
        indentLevel_--;
        
        addIndent();
        output_ += "</div>\n";
    }
}

void CHTLGenerator::generateTemplateUsage(const ast::TemplateNode& node) {
    // 生成模板使用的HTML
    
    addIndent();
    output_ += "<!-- Template Usage: " + node.type + " " + node.name + " -->\n";
    
    if (node.type == "Style") {
        // 应用样式模板
        std::unordered_map<std::string, std::string> targetProperties;
        if (templateManager_->applyStyleTemplate(node.name, targetProperties)) {
            addIndent();
            output_ += "<div class=\"" + node.name + "\" style=\"";
            for (const auto& [prop, value] : targetProperties) {
                output_ += prop + ": " + value + "; ";
            }
            output_ += "\">\n";
            
            // 访问子节点
            indentLevel_++;
            for (auto& child : node.children) {
                child->accept(*this);
            }
            indentLevel_--;
            
            addIndent();
            output_ += "</div>\n";
        }
    } else {
        // 其他类型的模板使用
        addIndent();
        output_ += "<div class=\"template-usage\" data-type=\"" + node.type + 
                  "\" data-name=\"" + node.name + "\"></div>\n";
    }
}

// === 自定义生成方法 ===

void CHTLGenerator::generateCustomStyle(const ast::CustomNode& node) {
    // 生成自定义样式组的HTML
    
    addIndent();
    output_ += "<!-- Custom Style: " + node.name + " -->\n";
    
    if (customManager_->hasCustomStyle(node.name)) {
        auto customStyle = customManager_->getCustomStyle(node.name);
        auto resolvedProps = customManager_->resolveCustomStyleInheritance(node.name);
        
        addIndent();
        output_ += "<style>\n";
        output_ += "/* Custom Style: " + node.name + " */\n";
        output_ += "." + node.name + " {\n";
        
        for (const auto& [prop, value] : resolvedProps) {
            output_ += "    " + prop + ": " + value + ";\n";
        }
        
        output_ += "}\n";
        output_ += "</style>\n";
    }
}

void CHTLGenerator::generateCustomElement(const ast::CustomNode& node) {
    // 生成自定义元素的HTML
    
    addIndent();
    output_ += "<!-- Custom Element: " + node.name + " -->\n";
    
    if (customManager_->hasCustomElement(node.name)) {
        addIndent();
        output_ += "<div class=\"custom-element\" data-custom=\"" + node.name + "\">\n";
        
        // 访问子节点
        indentLevel_++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indentLevel_--;
        
        addIndent();
        output_ += "</div>\n";
    }
}

void CHTLGenerator::generateCustomVariable(const ast::CustomNode& node) {
    // 生成自定义变量组的HTML
    
    addIndent();
    output_ += "<!-- Custom Variable: " + node.name + " -->\n";
    
    if (customManager_->hasCustomVariable(node.name)) {
        addIndent();
        output_ += "<div class=\"custom-variable\" data-custom=\"" + node.name + "\"></div>\n";
    }
}

// === 特例化操作生成方法 ===

void CHTLGenerator::generateInheritSpecialization(const ast::SpecializationNode& node) {
    // 生成继承特例化的HTML
    
    addIndent();
    output_ += "<!-- Inherit Specialization: " + node.target + " -->\n";
    
    // 继承操作主要在编译时处理，这里生成标记
    addIndent();
    output_ += "<div class=\"inherit-specialization\" data-target=\"" + node.target + "\"></div>\n";
}

void CHTLGenerator::generateDeleteSpecialization(const ast::SpecializationNode& node) {
    // 生成删除特例化的HTML
    
    addIndent();
    output_ += "<!-- Delete Specialization: " + node.target + " -->\n";
    
    // 删除操作在生成时跳过目标元素
}

void CHTLGenerator::generateInsertSpecialization(const ast::SpecializationNode& node) {
    // 生成插入特例化的HTML
    
    addIndent();
    output_ += "<!-- Insert Specialization: " + node.operation + " " + node.position + " " + node.target + " -->\n";
    
    addIndent();
    output_ += "<div class=\"insert-specialization\" data-position=\"" + node.position + 
              "\" data-target=\"" + node.target + "\">\n";
    
    // 生成插入的内容
    indentLevel_++;
    for (auto& child : node.content) {
        child->accept(*this);
    }
    indentLevel_--;
    
    addIndent();
    output_ += "</div>\n";
}

void CHTLGenerator::generateReplaceSpecialization(const ast::SpecializationNode& node) {
    // 生成替换特例化的HTML
    
    addIndent();
    output_ += "<!-- Replace Specialization: " + node.target + " -->\n";
    
    addIndent();
    output_ += "<div class=\"replace-specialization\" data-target=\"" + node.target + "\">\n";
    
    // 生成替换的内容
    indentLevel_++;
    for (auto& child : node.content) {
        child->accept(*this);
    }
    indentLevel_--;
    
    addIndent();
    output_ += "</div>\n";
}

// === 全局内容生成 ===

void CHTLGenerator::generateGlobalStyles() {
    // 生成全局样式
    
    output_ += "    <style>\n";
    output_ += "    /* CHTL Generated Global Styles */\n";
    output_ += "    .chtl-generated { font-family: Arial, sans-serif; }\n";
    output_ += "    .template-usage { border: 1px dashed #ccc; padding: 10px; margin: 5px 0; }\n";
    output_ += "    .custom-element { border: 1px solid #007acc; padding: 8px; margin: 4px 0; }\n";
    output_ += "    .variable-value { background: #f0f8ff; padding: 2px 4px; border-radius: 3px; }\n";
    output_ += "    .namespace { border-left: 3px solid #28a745; padding-left: 15px; }\n";
    output_ += "    </style>\n";
}

void CHTLGenerator::generateGlobalScripts() {
    // 生成全局脚本
    
    output_ += "    <script>\n";
    output_ += "    // CHTL Generated Global Scripts\n";
    output_ += "    console.log('CHTL page generated successfully');\n";
    output_ += "    \n";
    output_ += "    // Initialize CHTL components\n";
    output_ += "    document.addEventListener('DOMContentLoaded', function() {\n";
    output_ += "        const chtlElements = document.querySelectorAll('[data-chtl]');\n";
    output_ += "        chtlElements.forEach(element => {\n";
    output_ += "            element.classList.add('chtl-generated');\n";
    output_ += "        });\n";
    output_ += "    });\n";
    output_ += "    </script>\n";
}

// === 辅助方法 ===

void CHTLGenerator::addIndent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ += "    ";
    }
}

bool CHTLGenerator::isSelfClosingTag(const std::string& tag) {
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tag) != selfClosingTags.end();
}

std::string CHTLGenerator::escapeHtml(const std::string& input) {
    std::string output;
    output.reserve(input.size() * 2);
    
    for (char c : input) {
        switch (c) {
            case '<': output += "&lt;"; break;
            case '>': output += "&gt;"; break;
            case '&': output += "&amp;"; break;
            case '"': output += "&quot;"; break;
            case '\'': output += "&#39;"; break;
            default: output += c; break;
        }
    }
    
    return output;
}

std::string CHTLGenerator::generateUniqueId(const std::string& prefix) {
    static int counter = 0;
    return prefix + "_" + std::to_string(++counter);
}

} // namespace generator
} // namespace chtl