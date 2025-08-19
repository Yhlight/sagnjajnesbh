#include "CHTLGenerator.h"
#include <iostream>

namespace chtl {
namespace generator {

HTMLGenerator::HTMLGenerator() 
    : indent_level_(0), indent_size_(2), pretty_print_(true) {
}

std::string HTMLGenerator::generateHTML(ast::ASTNode* root) {
    if (!root) {
        addError("根节点为空");
        return "";
    }
    
    output_.str("");
    output_.clear();
    errors_.clear();
    indent_level_ = 0;
    
    try {
        root->accept(*this);
    } catch (const std::exception& e) {
        addError("生成HTML时发生异常: " + std::string(e.what()));
    }
    
    return output_.str();
}

void HTMLGenerator::visit(ast::ProgramNode& node) {
    // CHTL纯粹转换：输入什么CHTL代码，就输出对应的HTML
    // 不自动添加文档结构，支持SPA、组件、片段等任意HTML内容
    
    // 直接访问所有子节点，按CHTL代码原样转换
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
}

void HTMLGenerator::visit(ast::TemplateNode& node) {
    writeLine("<!-- Template: " + node.name + " -->");
    
    // 访问子节点
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
}

void HTMLGenerator::visit(ast::CustomNode& node) {
    writeLine("<!-- Custom: " + node.name + " -->");
    
    // 访问子节点
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
}

void HTMLGenerator::visit(ast::ElementNode& node) {
    if (node.tag.empty()) {
        addError("元素标签为空");
        return;
    }
    
    writeIndent();
    write("<" + node.tag);
    
    // 添加属性
    for (const auto& attr : node.attributes) {
        write(" " + attr.first + "=\"" + attr.second + "\"");
    }
    
    if (node.getChildCount() == 0) {
        // 自闭合标签
        write(" />");
        if (pretty_print_) write("\n");
    } else {
        write(">");
        if (pretty_print_) write("\n");
        
        indent_level_++;
        
        // 访问子节点
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            if (auto child = node.getChild(i)) {
                child->accept(*this);
            }
        }
        
        indent_level_--;
        writeLine("</" + node.tag + ">");
    }
}

void HTMLGenerator::visit(ast::StyleBlockNode& node) {
    if (node.is_global) {
        writeLine("<style>");
    } else {
        writeLine("<style scoped>");
    }
    
    indent_level_++;
    
    // 访问子节点生成CSS
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
    
    indent_level_--;
    writeLine("</style>");
}

void HTMLGenerator::visit(ast::ScriptBlockNode& node) {
    writeLine("<script>");
    
    indent_level_++;
    
    // 访问子节点生成JavaScript
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
    
    indent_level_--;
    writeLine("</script>");
}

void HTMLGenerator::visit(ast::TextNode& node) {
    if (!node.content.empty()) {
        writeIndent();
        write(node.content);
        if (pretty_print_) write("\n");
    }
}

void HTMLGenerator::visit(ast::ImportNode& node) {
    writeLine("<!-- Import: " + node.type + " from " + node.path + " -->");
}

void HTMLGenerator::visit(ast::NamespaceNode& node) {
    writeLine("<!-- Namespace: " + node.name + " -->");
    
    // 访问子节点
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
}

void HTMLGenerator::writeIndent() {
    if (pretty_print_) {
        for (size_t i = 0; i < indent_level_ * indent_size_; ++i) {
            output_ << ' ';
        }
    }
}

void HTMLGenerator::writeLine(const std::string& line) {
    writeIndent();
    output_ << line;
    if (pretty_print_) {
        output_ << '\n';
    }
}

void HTMLGenerator::write(const std::string& text) {
    output_ << text;
}

void HTMLGenerator::addError(const std::string& error) {
    errors_.push_back(error);
    std::cerr << "HTML生成器错误: " << error << std::endl;
}

void HTMLGenerator::generateElement(const std::string& tag, const std::string& content) {
    writeIndent();
    if (content.empty()) {
        write("<" + tag + " />");
    } else {
        write("<" + tag + ">" + content + "</" + tag + ">");
    }
    if (pretty_print_) write("\n");
}

void HTMLGenerator::generateOpenTag(const std::string& tag) {
    writeIndent();
    write("<" + tag + ">");
    if (pretty_print_) write("\n");
    indent_level_++;
}

void HTMLGenerator::generateCloseTag(const std::string& tag) {
    indent_level_--;
    writeIndent();
    write("</" + tag + ">");
    if (pretty_print_) write("\n");
}

void HTMLGenerator::generateStyle(const std::string& css) {
    writeLine("<style>");
    indent_level_++;
    writeLine(css);
    indent_level_--;
    writeLine("</style>");
}

void HTMLGenerator::generateScript(const std::string& js) {
    writeLine("<script>");
    indent_level_++;
    writeLine(js);
    indent_level_--;
    writeLine("</script>");
}

// === 新增的AST节点visit方法实现 ===

void HTMLGenerator::visit(ast::OriginNode& node) {
    // 原始嵌入节点处理
    if (node.originType == "Html") {
        write(node.content);
    } else if (node.originType == "Style") {
        writeLine("<style>");
        writeLine(node.content);
        writeLine("</style>");
    } else if (node.originType == "JavaScript") {
        writeLine("<script>");
        writeLine(node.content);
        writeLine("</script>");
    } else {
        // 自定义类型的原始嵌入
        writeLine("<!-- Origin: " + node.originType + " -->");
        writeLine(node.content);
        writeLine("<!-- End Origin: " + node.originType + " -->");
    }
}

void HTMLGenerator::visit(ast::ConfigurationNode& node) {
    // 配置节点不生成HTML，只是编译时配置
    writeLine("<!-- Configuration: " + node.name + " -->");
}

void HTMLGenerator::visit(ast::ConstraintNode& node) {
    // 约束节点不生成HTML，只是编译时约束
    writeLine("<!-- Constraint: " + node.type + " -->");
}

void HTMLGenerator::visit(ast::SpecializationNode& node) {
    // 特例化节点处理
    writeLine("<!-- Specialization: " + node.operation + " -->");
}

void HTMLGenerator::visit(ast::IndexAccessNode& node) {
    // 索引访问节点处理
    writeLine("<!-- IndexAccess: " + std::to_string(node.index) + " -->");
}

void HTMLGenerator::visit(ast::VariableGroupNode& node) {
    // 变量组节点不直接生成HTML，而是在使用时替换
    writeLine("<!-- VariableGroup: " + node.name + " -->");
}

} // namespace generator
} // namespace chtl