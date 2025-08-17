#include "HTMLGenerator.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace chtl {
namespace generator {

HTMLGenerator::HTMLGenerator(CompilerContext& context, const GeneratorConfig& config)
    : context_(context), config_(config), current_indent_(0) {
}

std::string HTMLGenerator::generate(const ast::ASTNode* root) {
    if (!root) return "";
    
    // 重置输出流
    html_output_.str("");
    css_output_.str("");
    js_output_.str("");
    current_indent_ = 0;
    errors_.clear();
    
    // 生成HTML文档结构
    writeLine("<!DOCTYPE html>");
    writeLine("<html>");
    increaseIndent();
    
    generateDocumentHead();
    generateDocumentBody();
    
    // 访问AST根节点
    const_cast<ast::ASTNode*>(root)->accept(*this);
    
    decreaseIndent();
    writeLine("</html>");
    
    return html_output_.str();
}

std::string HTMLGenerator::generateToFile(const ast::ASTNode* root, const std::string& output_path) {
    std::string html_content = generate(root);
    
    std::ofstream output_file(output_path);
    if (!output_file.is_open()) {
        reportError("无法创建输出文件: " + output_path);
        return "";
    }
    
    output_file << html_content;
    output_file.close();
    
    return html_content;
}

void HTMLGenerator::visitProgram(ast::ProgramNode& node) {
    // 首先处理导入、命名空间、模板、自定义定义
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        auto child = node.getChild(i);
        
        if (child->type == ast::NodeType::IMPORT_STATEMENT) {
            child->accept(*this);
        } else if (child->type == ast::NodeType::NAMESPACE_DEFINITION) {
            child->accept(*this);
        } else if (child->type == ast::NodeType::TEMPLATE_DEFINITION) {
            child->accept(*this);
        } else if (child->type == ast::NodeType::CUSTOM_DEFINITION) {
            child->accept(*this);
        }
    }
    
    // 然后处理实际的HTML元素
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        auto child = node.getChild(i);
        
        if (child->type == ast::NodeType::ELEMENT) {
            child->accept(*this);
        } else if (child->type == ast::NodeType::ORIGIN_BLOCK) {
            child->accept(*this);
        } else if (child->type == ast::NodeType::SINGLE_COMMENT ||
                  child->type == ast::NodeType::MULTI_COMMENT ||
                  child->type == ast::NodeType::GENERATOR_COMMENT) {
            child->accept(*this);
        }
    }
}

void HTMLGenerator::visitElement(ast::ElementNode& node) {
    std::vector<std::pair<std::string, std::string>> attributes;
    std::vector<ast::ASTNode*> content_nodes;
    std::vector<ast::ASTNode*> style_blocks;
    std::vector<ast::ASTNode*> script_blocks;
    
    // 分类子节点
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        auto child = node.getChild(i);
        
        if (child->type == ast::NodeType::ATTRIBUTE) {
            auto attr = dynamic_cast<ast::AttributeNode*>(child);
            if (attr) {
                attributes.emplace_back(attr->name, unquoteString(attr->value));
            }
        } else if (child->type == ast::NodeType::STYLE_BLOCK) {
            style_blocks.push_back(child);
        } else if (child->type == ast::NodeType::SCRIPT_BLOCK) {
            script_blocks.push_back(child);
        } else {
            content_nodes.push_back(child);
        }
    }
    
    // 处理样式块，可能会自动生成类名
    for (auto style_block : style_blocks) {
        generateCSS(style_block);
    }
    
    // 处理脚本块
    for (auto script_block : script_blocks) {
        generateJS(script_block);
    }
    
    // 生成HTML元素
    if (isSelfClosingTag(node.tag_name)) {
        generateSelfClosingElement(node.tag_name, attributes);
    } else {
        generateElementStart(node.tag_name, attributes);
        
        if (!content_nodes.empty()) {
            increaseIndent();
            
            for (auto content_node : content_nodes) {
                content_node->accept(*this);
            }
            
            decreaseIndent();
        }
        
        generateElementEnd(node.tag_name);
    }
}

void HTMLGenerator::visitText(ast::TextNode& node) {
    std::string content = unquoteString(node.content);
    writeIndent();
    write(escapeHTML(content));
    writeLine();
}

void HTMLGenerator::visitAttribute(ast::AttributeNode& node) {
    // 属性在visitElement中处理，这里不需要额外操作
}

void HTMLGenerator::visitStyleBlock(ast::StyleBlockNode& node) {
    generateCSS(&node);
}

void HTMLGenerator::visitScriptBlock(ast::ScriptBlockNode& node) {
    generateJS(&node);
}

void HTMLGenerator::visitTemplateDefinition(ast::TemplateDefinitionNode& node) {
    // 存储模板定义以供后续使用
    templates_[node.template_name] = &node;
}

void HTMLGenerator::visitCustomDefinition(ast::CustomDefinitionNode& node) {
    // 存储自定义定义以供后续使用
    customs_[node.custom_name] = &node;
}

void HTMLGenerator::visitOriginBlock(ast::OriginBlockNode& node) {
    processOriginBlock(&node);
}

void HTMLGenerator::visitImportStatement(ast::ImportStatementNode& node) {
    // 导入语句在编译时处理，生成时通常不需要输出
    if (config_.generate_comments) {
        writeLine("<!-- Import: " + node.import_type + " from " + node.source_path + " -->");
    }
}

void HTMLGenerator::visitNamespaceDefinition(ast::NamespaceDefinitionNode& node) {
    // 命名空间定义，处理其内容
    for (size_t i = 0; i < node.getChildCount(); ++i) {
        node.getChild(i)->accept(*this);
    }
}

void HTMLGenerator::visitCHTLJSSelector(ast::CHTLJSSelectorNode& node) {
    generateCHTLJSSelector(&node);
}

void HTMLGenerator::visitComment(ast::CommentNode& node) {
    if (config_.generate_comments) {
        if (node.type == ast::NodeType::GENERATOR_COMMENT) {
            // 生成器注释直接输出到HTML
            writeLine("<!-- " + node.comment_content + " -->");
        }
        // 其他注释类型不输出到HTML
    }
}

void HTMLGenerator::visitLiteral(ast::LiteralNode& node) {
    // 字面量直接输出
    write(node.literal_value);
}

void HTMLGenerator::visitIdentifier(ast::IdentifierNode& node) {
    // 标识符直接输出
    write(node.identifier_name);
}

// 获取生成的CSS和JS
std::string HTMLGenerator::getGeneratedCSS() const {
    return css_output_.str();
}

std::string HTMLGenerator::getGeneratedJS() const {
    return js_output_.str();
}

// 错误处理
bool HTMLGenerator::hasError() const {
    return !errors_.empty();
}

std::vector<std::string> HTMLGenerator::getErrors() const {
    return errors_;
}

void HTMLGenerator::clearErrors() {
    errors_.clear();
}

// 输出辅助方法
void HTMLGenerator::writeIndent() {
    if (config_.pretty_print) {
        for (int i = 0; i < current_indent_ * config_.indent_size; ++i) {
            html_output_ << " ";
        }
    }
}

void HTMLGenerator::writeLine(const std::string& content) {
    writeIndent();
    html_output_ << content;
    if (config_.pretty_print) {
        html_output_ << "\n";
    }
}

void HTMLGenerator::write(const std::string& content) {
    html_output_ << content;
}

void HTMLGenerator::increaseIndent() {
    current_indent_++;
}

void HTMLGenerator::decreaseIndent() {
    if (current_indent_ > 0) {
        current_indent_--;
    }
}

// HTML生成辅助
void HTMLGenerator::generateElementStart(const std::string& tag_name, 
                                        const std::vector<std::pair<std::string, std::string>>& attributes) {
    writeIndent();
    write("<" + tag_name);
    
    for (const auto& attr : attributes) {
        write(" " + attr.first + "=\"" + attr.second + "\"");
    }
    
    write(">");
    if (config_.pretty_print) {
        html_output_ << "\n";
    }
}

void HTMLGenerator::generateElementEnd(const std::string& tag_name) {
    writeLine("</" + tag_name + ">");
}

void HTMLGenerator::generateSelfClosingElement(const std::string& tag_name, 
                                              const std::vector<std::pair<std::string, std::string>>& attributes) {
    writeIndent();
    write("<" + tag_name);
    
    for (const auto& attr : attributes) {
        write(" " + attr.first + "=\"" + attr.second + "\"");
    }
    
    write(" />");
    if (config_.pretty_print) {
        html_output_ << "\n";
    }
}

// CSS生成辅助
void HTMLGenerator::generateCSS(const ast::ASTNode* style_node) {
    for (size_t i = 0; i < style_node->getChildCount(); ++i) {
        auto child = style_node->getChild(i);
        
        if (child->type == ast::NodeType::CSS_PROPERTY) {
            generateCSSProperty(dynamic_cast<const ast::CSSPropertyNode*>(child));
        } else if (child->type == ast::NodeType::CSS_SELECTOR) {
            generateCSSSelector(dynamic_cast<const ast::CSSSelectorNode*>(child));
        } else if (child->type == ast::NodeType::STYLE_INHERITANCE) {
            processStyleInheritance(dynamic_cast<const ast::StyleInheritanceNode*>(child));
        } else if (child->type == ast::NodeType::STYLE_DELETE) {
            processStyleDelete(dynamic_cast<const ast::StyleDeleteNode*>(child));
        }
    }
}

void HTMLGenerator::generateCSSProperty(const ast::CSSPropertyNode* property_node) {
    if (!property_node) return;
    
    css_output_ << "  " << property_node->property_name << ": " << property_node->property_value;
    if (property_node->is_important) {
        css_output_ << " !important";
    }
    css_output_ << ";\n";
}

void HTMLGenerator::generateCSSSelector(const ast::CSSSelectorNode* selector_node) {
    if (!selector_node) return;
    
    std::string resolved_selector = resolveCSSSelector(selector_node->selector_text);
    css_output_ << resolved_selector << " {\n";
    
    // 生成选择器内的属性
    for (size_t i = 0; i < selector_node->getChildCount(); ++i) {
        auto child = selector_node->getChild(i);
        if (child->type == ast::NodeType::CSS_PROPERTY) {
            generateCSSProperty(dynamic_cast<const ast::CSSPropertyNode*>(child));
        }
    }
    
    css_output_ << "}\n";
}

// JS生成辅助
void HTMLGenerator::generateJS(const ast::ASTNode* script_node) {
    for (size_t i = 0; i < script_node->getChildCount(); ++i) {
        auto child = script_node->getChild(i);
        
        if (child->type == ast::NodeType::CHTL_JS_SELECTOR) {
            generateCHTLJSSelector(dynamic_cast<const ast::CHTLJSSelectorNode*>(child));
        } else if (child->type == ast::NodeType::CHTL_JS_LISTEN) {
            generateCHTLJSListen(dynamic_cast<const ast::CHTLJSListenNode*>(child));
        } else if (child->type == ast::NodeType::CHTL_JS_DELEGATE) {
            generateCHTLJSDelegate(dynamic_cast<const ast::CHTLJSDelegateNode*>(child));
        } else if (child->type == ast::NodeType::CHTL_JS_VIR) {
            generateCHTLJSVir(dynamic_cast<const ast::CHTLJSVirNode*>(child));
        } else if (child->type == ast::NodeType::STRING_LITERAL) {
            // 普通JavaScript代码
            auto literal = dynamic_cast<const ast::LiteralNode*>(child);
            if (literal) {
                js_output_ << literal->literal_value << "\n";
            }
        }
    }
}

void HTMLGenerator::generateCHTLJSSelector(const ast::CHTLJSSelectorNode* selector_node) {
    if (!selector_node) return;
    
    std::string selector = selector_node->selector_content;
    std::string js_selector;
    
    // 转换CHTL JS选择器为标准JS选择器
    if (selector.find('.') == 0) {
        // 类选择器
        js_selector = "document.querySelector('" + selector + "')";
    } else if (selector.find('#') == 0) {
        // ID选择器
        js_selector = "document.querySelector('" + selector + "')";
    } else if (selector.find('[') != std::string::npos) {
        // 索引选择器
        size_t bracket_pos = selector.find('[');
        std::string element_name = selector.substr(0, bracket_pos);
        std::string index_str = selector.substr(bracket_pos + 1);
        index_str = index_str.substr(0, index_str.find(']'));
        
        js_selector = "document.querySelectorAll('" + element_name + "')[" + index_str + "]";
    } else {
        // 元素选择器或复合选择器
        if (selector.find(' ') != std::string::npos) {
            js_selector = "document.querySelectorAll('" + selector + "')";
        } else {
            // 先检查是否为标签名，然后查找类名/ID
            js_selector = "(function() { "
                         "var elem = document.getElementById('" + selector + "'); "
                         "if (elem) return elem; "
                         "elem = document.querySelector('." + selector + "'); "
                         "if (elem) return elem; "
                         "return document.querySelector('" + selector + "'); "
                         "})()";
        }
    }
    
    chtl_js_selectors_[selector] = js_selector;
}

void HTMLGenerator::generateCHTLJSListen(const ast::CHTLJSListenNode* listen_node) {
    if (!listen_node) return;
    
    std::string selector = listen_node->target_selector;
    generateCHTLJSSelector(new ast::CHTLJSSelectorNode(selector));
    
    std::string js_selector = chtl_js_selectors_[selector];
    
    js_output_ << "if (" << js_selector << ") {\n";
    
    for (const auto& handler : listen_node->event_handlers) {
        js_output_ << "  " << js_selector << ".addEventListener('" 
                   << handler.first << "', " << handler.second << ");\n";
    }
    
    js_output_ << "}\n";
}

void HTMLGenerator::generateCHTLJSDelegate(const ast::CHTLJSDelegateNode* delegate_node) {
    if (!delegate_node) return;
    
    std::string parent_selector = delegate_node->parent_selector;
    generateCHTLJSSelector(new ast::CHTLJSSelectorNode(parent_selector));
    
    std::string js_parent_selector = chtl_js_selectors_[parent_selector];
    
    for (const auto& handler : delegate_node->event_handlers) {
        js_output_ << js_parent_selector << ".addEventListener('" 
                   << handler.first << "', function(event) {\n";
        
        for (const auto& target : delegate_node->target_selectors) {
            generateCHTLJSSelector(new ast::CHTLJSSelectorNode(target));
            std::string js_target_selector = chtl_js_selectors_[target];
            
            js_output_ << "  if (event.target.matches('" << target << "')) {\n";
            js_output_ << "    (" << handler.second << ")(event);\n";
            js_output_ << "  }\n";
        }
        
        js_output_ << "});\n";
    }
}

void HTMLGenerator::generateCHTLJSVir(const ast::CHTLJSVirNode* vir_node) {
    if (!vir_node) return;
    
    // 为虚对象的每个函数创建全局函数
    for (const auto& func : vir_node->function_map) {
        std::string func_name = vir_node->vir_name + "_" + func.first;
        js_output_ << "function " << func_name << "() {\n";
        js_output_ << "  " << func.second << "\n";
        js_output_ << "}\n";
        
        // 记录函数映射
        chtl_js_functions_.push_back(func_name);
    }
}

// 工具方法
std::string HTMLGenerator::escapeHTML(const std::string& text) {
    std::string escaped = text;
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('>', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = escaped.find('\'', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return escaped;
}

std::string HTMLGenerator::unquoteString(const std::string& quoted_string) {
    if (quoted_string.length() >= 2 && 
        ((quoted_string.front() == '"' && quoted_string.back() == '"') ||
         (quoted_string.front() == '\'' && quoted_string.back() == '\''))) {
        return quoted_string.substr(1, quoted_string.length() - 2);
    }
    return quoted_string;
}

bool HTMLGenerator::isSelfClosingTag(const std::string& tag_name) {
    static const std::unordered_set<std::string> self_closing_tags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return self_closing_tags.count(tag_name) > 0;
}

std::string HTMLGenerator::generateClassName(const std::string& base_name) {
    std::string class_name = config_.css_class_prefix + base_name;
    
    // 确保类名唯一
    int counter = 1;
    std::string unique_name = class_name;
    while (generated_classes_.count(unique_name)) {
        unique_name = class_name + "_" + std::to_string(counter++);
    }
    
    generated_classes_.insert(unique_name);
    return unique_name;
}

std::string HTMLGenerator::resolveCSSSelector(const std::string& selector) {
    if (selector.find('&') == 0) {
        // 上下文推导选择器，需要根据当前元素的类名/ID替换
        // 这里简化处理，实际应该根据上下文动态替换
        return selector;
    }
    
    return selector;
}

void HTMLGenerator::processOriginBlock(const ast::OriginBlockNode* origin_node) {
    if (!origin_node) return;
    
    if (origin_node->origin_type == "@Html") {
        // 直接输出HTML内容
        writeLine(origin_node->raw_content);
    } else if (origin_node->origin_type == "@Style") {
        // 添加到CSS输出
        css_output_ << origin_node->raw_content << "\n";
    } else if (origin_node->origin_type == "@JavaScript") {
        // 添加到JS输出
        js_output_ << origin_node->raw_content << "\n";
    } else {
        // 自定义类型，作为注释处理
        if (config_.generate_comments) {
            writeLine("<!-- Custom Origin Block: " + origin_node->origin_type + " -->");
            writeLine("<!-- " + origin_node->raw_content + " -->");
        }
    }
}

void HTMLGenerator::processStyleInheritance(const ast::StyleInheritanceNode* inherit_node) {
    if (!inherit_node) return;
    
    // 查找被继承的样式模板或自定义样式
    auto template_it = templates_.find(inherit_node->inherited_style);
    if (template_it != templates_.end()) {
        // 展开模板样式
        const auto* template_node = template_it->second;
        for (size_t i = 0; i < template_node->getChildCount(); ++i) {
            auto child = template_node->getChild(i);
            if (child->type == ast::NodeType::CSS_PROPERTY) {
                generateCSSProperty(dynamic_cast<const ast::CSSPropertyNode*>(child));
            }
        }
    }
    
    auto custom_it = customs_.find(inherit_node->inherited_style);
    if (custom_it != customs_.end()) {
        // 展开自定义样式
        const auto* custom_node = custom_it->second;
        for (size_t i = 0; i < custom_node->getChildCount(); ++i) {
            auto child = custom_node->getChild(i);
            if (child->type == ast::NodeType::CSS_PROPERTY) {
                generateCSSProperty(dynamic_cast<const ast::CSSPropertyNode*>(child));
            }
        }
    }
}

void HTMLGenerator::processStyleDelete(const ast::StyleDeleteNode* delete_node) {
    if (!delete_node) return;
    
    // 样式删除在生成阶段通过过滤实现
    // 这里记录要删除的属性和继承，在实际生成时跳过
    
    if (config_.generate_comments) {
        css_output_ << "/* Deleted properties: ";
        for (const auto& prop : delete_node->deleted_properties) {
            css_output_ << prop << " ";
        }
        css_output_ << "*/\n";
        
        css_output_ << "/* Deleted inheritance: ";
        for (const auto& inherit : delete_node->deleted_inheritance) {
            css_output_ << inherit << " ";
        }
        css_output_ << "*/\n";
    }
}

void HTMLGenerator::generateDocumentHead() {
    writeLine("<head>");
    increaseIndent();
    
    writeLine("<meta charset=\"UTF-8\">");
    writeLine("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    writeLine("<title>CHTL Generated Page</title>");
    
    // 生成内联样式
    generateInlineStyles();
    
    decreaseIndent();
    writeLine("</head>");
}

void HTMLGenerator::generateDocumentBody() {
    writeLine("<body>");
    increaseIndent();
}

void HTMLGenerator::generateInlineStyles() {
    std::string css_content = getGeneratedCSS();
    if (!css_content.empty()) {
        writeLine("<style>");
        increaseIndent();
        
        std::istringstream css_stream(css_content);
        std::string line;
        while (std::getline(css_stream, line)) {
            writeLine(line);
        }
        
        decreaseIndent();
        writeLine("</style>");
    }
}

void HTMLGenerator::generateInlineScripts() {
    std::string js_content = getGeneratedJS();
    if (!js_content.empty()) {
        writeLine("<script>");
        increaseIndent();
        
        std::istringstream js_stream(js_content);
        std::string line;
        while (std::getline(js_stream, line)) {
            writeLine(line);
        }
        
        decreaseIndent();
        writeLine("</script>");
    }
}

void HTMLGenerator::reportError(const std::string& message) {
    errors_.push_back(message);
    context_.reportError(message);
}

} // namespace generator
} // namespace chtl