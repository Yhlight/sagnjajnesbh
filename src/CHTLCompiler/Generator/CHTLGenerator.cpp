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
    writeLine("<!DOCTYPE html>");
    writeLine("<html>");
    writeLine("<head>");
    indent_level_++;
    writeLine("<meta charset=\"UTF-8\">");
    writeLine("<title>CHTL Generated Page</title>");
    indent_level_--;
    writeLine("</head>");
    writeLine("<body>");
    
    indent_level_++;
    
    // 访问所有子节点
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        if (auto child = node.getChild(i)) {
            child->accept(*this);
        }
    }
    
    indent_level_--;
    writeLine("</body>");
    writeLine("</html>");
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

} // namespace generator
} // namespace chtl