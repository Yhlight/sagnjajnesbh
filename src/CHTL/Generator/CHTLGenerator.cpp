#include "CHTL/Generator/CHTLGenerator.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Generator {

CHTLGenerator::CHTLGenerator(Core::CHTLGlobalMap& globalMap, const GeneratorConfig& config)
    : config_(config), globalMap_(globalMap), currentIndent_(0),
      elementCount_(0), templateExpandCount_(0), customExpandCount_(0), variableSubstitutionCount_(0) {}

std::string CHTLGenerator::Generate(AST::ASTNodePtr ast) {
    if (!ast) {
        return "";
    }
    
    // 重置状态
    Reset();
    
    // 访问AST根节点
    ast->Accept(*this);
    
    // 生成完整的HTML文档
    std::string html = GenerateCompleteHTML();
    
    // 后处理
    if (config_.minify) {
        html = MinifyHTML(html);
    } else if (config_.prettyPrint) {
        html = FormatHTML(html);
    }
    
    // 验证输出
    if (config_.validateOutput) {
        if (!ValidateHTML(html)) {
            Utils::ErrorHandler::GetInstance().LogWarning("生成的HTML可能存在问题");
        }
    }
    
    return html;
}

void CHTLGenerator::VisitRootNode(AST::RootNode& node) {
    // 处理根节点的所有子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

void CHTLGenerator::VisitElementNode(AST::ElementNode& node) {
    elementCount_++;
    
    // 设置当前元素上下文
    context_.variables["__current_element__"] = node.GetTagName();
    
    // 检查是否需要自动添加类名或ID
    std::string autoClass = context_.variables["__auto_class__"];
    std::string autoId = context_.variables["__auto_id__"];
    
    // 创建元素副本以添加自动属性
    auto elementCopy = std::dynamic_pointer_cast<AST::ElementNode>(node.Clone());
    
    if (!autoClass.empty() && !elementCopy->HasAttribute("class")) {
        elementCopy->AddClass(autoClass);
        context_.variables.erase("__auto_class__"); // 清除已使用的自动类名
    }
    
    if (!autoId.empty() && !elementCopy->HasAttribute("id")) {
        elementCopy->SetId(autoId);
        context_.variables.erase("__auto_id__"); // 清除已使用的自动ID
    }
    
    // 生成开始标签
    if (config_.prettyPrint) {
        output_ << GetIndent();
    }
    
    output_ << GenerateStartTag(*elementCopy);
    
    if (config_.prettyPrint && !elementCopy->IsSelfClosing()) {
        output_ << "\n";
        AddIndent();
    }
    
    // 处理子节点
    if (!elementCopy->IsSelfClosing()) {
        for (const auto& child : node.GetChildren()) {
            child->Accept(*this);
        }
        
        // 生成结束标签
        if (config_.prettyPrint) {
            RemoveIndent();
            output_ << GetIndent();
        }
        
        output_ << GenerateEndTag(*elementCopy);
    }
    
    if (config_.prettyPrint) {
        output_ << "\n";
    }
    
    // 清除当前元素上下文
    context_.variables.erase("__current_element__");
}

void CHTLGenerator::VisitTextNode(AST::TextNode& node) {
    if (config_.prettyPrint) {
        output_ << GetIndent();
    }
    
    // 转义HTML特殊字符
    std::string content = EscapeHTML(node.GetContent());
    output_ << content;
    
    if (config_.prettyPrint) {
        output_ << "\n";
    }
}

void CHTLGenerator::VisitAttributeNode(AST::AttributeNode& node) {
    // 属性节点在元素节点中处理，这里不直接输出
}

void CHTLGenerator::VisitStyleBlockNode(AST::StyleBlockNode& node) {
    // 处理样式块
    std::ostringstream styleContent;
    
    // 如果是局部样式块，需要特殊处理
    if (node.IsLocalStyleBlock()) {
        // 收集内联样式和全局样式
        std::ostringstream inlineStyles;
        std::ostringstream globalStyles;
        
        for (const auto& child : node.GetChildren()) {
            if (child->GetType() == AST::NodeType::CSS_PROPERTY) {
                // 内联样式
                auto property = std::dynamic_pointer_cast<AST::CSSPropertyNode>(child);
                if (property) {
                    inlineStyles << property->GetProperty() << ": " << property->GetValue() << "; ";
                }
            } else if (child->GetType() == AST::NodeType::CSS_SELECTOR) {
                // 全局样式
                auto selector = std::dynamic_pointer_cast<AST::CSSSelectorNode>(child);
                if (selector && selector->IsAddedToGlobalStyle()) {
                    globalStyles << selector->GetSelector() << " { ";
                    
                    // 处理选择器的子属性
                    for (const auto& grandChild : selector->GetChildren()) {
                        if (grandChild->GetType() == AST::NodeType::CSS_PROPERTY) {
                            auto prop = std::dynamic_pointer_cast<AST::CSSPropertyNode>(grandChild);
                            if (prop) {
                                globalStyles << prop->GetProperty() << ": " << prop->GetValue() << "; ";
                            }
                        }
                    }
                    
                    globalStyles << "}\n";
                }
            }
        }
        
        // 添加内联样式到当前元素（如果有父元素）
        std::string inlineStyleStr = inlineStyles.str();
        if (!inlineStyleStr.empty()) {
            // 这里需要与父元素节点配合
            // 暂时存储到上下文中
            context_.variables["__inline_style__"] = inlineStyleStr;
        }
        
        // 添加全局样式
        std::string globalStyleStr = globalStyles.str();
        if (!globalStyleStr.empty()) {
            CollectGlobalStyle(globalStyleStr);
        }
    }
}

void CHTLGenerator::VisitScriptBlockNode(AST::ScriptBlockNode& node) {
    // 处理脚本块
    if (node.IsLocalScriptBlock()) {
        // 局部脚本块添加到全局脚本中
        CollectGlobalScript(node.GetScriptContent());
    } else {
        // 全局脚本块直接输出
        output_ << "<script>\n" << node.GetScriptContent() << "\n</script>\n";
    }
}

void CHTLGenerator::VisitCSSSelectorNode(AST::CSSSelectorNode& node) {
    // 处理CSS选择器，实现自动化类名/ID功能（语法文档第110行）
    
    if (node.IsAddedToGlobalStyle()) {
        std::ostringstream selectorStyle;
        std::string actualSelector = node.GetSelector();
        
        // 处理自动化类名/ID
        if (node.GetSelectorType() == AST::CSSSelectorNode::SelectorType::CLASS) {
            // 自动添加类名到当前元素（如果在元素上下文中）
            std::string className = actualSelector.substr(1); // 移除点号
            
            // 如果当前有元素上下文，自动添加类名
            if (!context_.variables["__current_element__"].empty()) {
                context_.variables["__auto_class__"] = className;
            }
            
            context_.generatedClasses.insert(className);
            
        } else if (node.GetSelectorType() == AST::CSSSelectorNode::SelectorType::ID) {
            // 自动添加ID到当前元素
            std::string idName = actualSelector.substr(1); // 移除#号
            
            if (!context_.variables["__current_element__"].empty()) {
                context_.variables["__auto_id__"] = idName;
            }
            
            context_.generatedIds.insert(idName);
            
        } else if (node.GetSelectorType() == AST::CSSSelectorNode::SelectorType::AMPERSAND) {
            // 上下文推导选择器，替换为实际的类名或ID
            std::string currentClass = context_.variables["__auto_class__"];
            std::string currentId = context_.variables["__auto_id__"];
            
            if (!currentClass.empty()) {
                actualSelector = Utils::StringUtils::ReplaceAll(actualSelector, "&", "." + currentClass);
            } else if (!currentId.empty()) {
                actualSelector = Utils::StringUtils::ReplaceAll(actualSelector, "&", "#" + currentId);
            } else {
                // 如果没有上下文，生成一个自动类名
                std::string autoClass = GenerateAutoClassName();
                actualSelector = Utils::StringUtils::ReplaceAll(actualSelector, "&", "." + autoClass);
                context_.variables["__auto_class__"] = autoClass;
            }
        }
        
        // 生成CSS规则
        selectorStyle << actualSelector << " {\n";
        
        // 处理选择器的子属性
        for (const auto& child : node.GetChildren()) {
            if (child->GetType() == AST::NodeType::CSS_PROPERTY) {
                auto property = std::dynamic_pointer_cast<AST::CSSPropertyNode>(child);
                if (property) {
                    selectorStyle << "  " << property->GetProperty() 
                                 << ": " << property->GetValue() << ";\n";
                }
            }
        }
        
        selectorStyle << "}\n";
        
        // 添加到全局样式
        CollectGlobalStyle(selectorStyle.str());
    }
}

void CHTLGenerator::VisitCSSPropertyNode(AST::CSSPropertyNode& node) {
    // CSS属性在样式块或选择器中处理
}

void CHTLGenerator::VisitTemplateNode(AST::TemplateNode& node) {
    // 模板定义不直接生成输出，存储在全局映射表中
    // 在模板引用时展开
}

void CHTLGenerator::VisitCustomNode(AST::CustomNode& node) {
    // 自定义定义不直接生成输出，存储在全局映射表中
    // 在自定义引用时展开
}

void CHTLGenerator::VisitOriginNode(AST::OriginNode& node) {
    // 原始嵌入直接输出内容
    switch (node.GetOriginType()) {
        case AST::OriginNode::OriginType::HTML:
            output_ << node.GetContent();
            break;
        case AST::OriginNode::OriginType::STYLE:
            CollectGlobalStyle(node.GetContent());
            break;
        case AST::OriginNode::OriginType::JAVASCRIPT:
            CollectGlobalScript(node.GetContent());
            break;
        case AST::OriginNode::OriginType::CUSTOM:
            // 自定义类型原始嵌入，根据类型处理
            if (node.GetCustomTypeName().find("Style") != std::string::npos ||
                node.GetCustomTypeName().find("CSS") != std::string::npos) {
                CollectGlobalStyle(node.GetContent());
            } else if (node.GetCustomTypeName().find("Script") != std::string::npos ||
                      node.GetCustomTypeName().find("JavaScript") != std::string::npos) {
                CollectGlobalScript(node.GetContent());
            } else {
                // 其他自定义类型直接输出
                output_ << node.GetContent();
            }
            break;
    }
}

void CHTLGenerator::VisitImportNode(AST::ImportNode& node) {
    // 导入节点在编译时处理，不生成运行时输出
}

void CHTLGenerator::VisitNamespaceNode(AST::NamespaceNode& node) {
    // 进入命名空间
    std::string previousNamespace = context_.currentNamespace;
    context_.currentNamespace = node.GetName();
    
    // 处理命名空间内容
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    
    // 恢复命名空间
    context_.currentNamespace = previousNamespace;
}

void CHTLGenerator::VisitConfigurationNode(AST::ConfigurationNode& node) {
    // 配置节点在编译时处理，不生成运行时输出
}

void CHTLGenerator::VisitTemplateReferenceNode(AST::TemplateReferenceNode& node) {
    templateExpandCount_++;
    
    // 查找模板定义
    Core::SymbolType symbolType = (node.GetTemplateType() == "@Style") ? 
                                 Core::SymbolType::TEMPLATE_STYLE :
                                 (node.GetTemplateType() == "@Element") ? 
                                 Core::SymbolType::TEMPLATE_ELEMENT :
                                 Core::SymbolType::TEMPLATE_VAR;
    
    const Core::SymbolInfo* symbol = globalMap_.FindSymbolByType(
        node.GetTemplateName(), symbolType, node.GetNamespace());
    
    if (!symbol) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到模板: " + node.GetTemplateType() + " " + node.GetTemplateName()
        );
        return;
    }
    
    // 根据模板类型进行展开
    if (node.GetTemplateType() == "@Style") {
        // 样式模板展开为内联样式
        std::string inlineStyle = context_.variables["__inline_style__"];
        
        for (const auto& prop : symbol->properties) {
            inlineStyle += prop.first + ": " + prop.second + "; ";
        }
        
        // 处理继承
        for (const auto& inherit : symbol->inherits) {
            // 递归展开继承的模板
            std::string inheritNamespace, inheritName;
            if (Core::CHTLGlobalMap::ParseFullName(inherit, inheritNamespace, inheritName)) {
                const Core::SymbolInfo* inheritSymbol = globalMap_.FindSymbolByType(
                    inheritName, symbolType, inheritNamespace);
                
                if (inheritSymbol) {
                    for (const auto& prop : inheritSymbol->properties) {
                        // 检查是否已存在（避免重复）
                        if (inlineStyle.find(prop.first + ":") == std::string::npos) {
                            inlineStyle += prop.first + ": " + prop.second + "; ";
                        }
                    }
                }
            }
        }
        
        context_.variables["__inline_style__"] = inlineStyle;
        
    } else if (node.GetTemplateType() == "@Element") {
        // 元素模板展开为实际元素
        auto expandedElements = ExpandTemplate(node);
        for (const auto& element : expandedElements) {
            element->Accept(*this);
        }
        
    } else if (node.GetTemplateType() == "@Var") {
        // 变量模板存储到上下文中
        for (const auto& prop : symbol->properties) {
            context_.variables[symbol->name + "." + prop.first] = prop.second;
        }
    }
}

void CHTLGenerator::VisitCustomReferenceNode(AST::CustomReferenceNode& node) {
    customExpandCount_++;
    
    // 查找自定义定义
    Core::SymbolType symbolType = (node.GetCustomType() == "@Style") ? 
                                 Core::SymbolType::CUSTOM_STYLE :
                                 (node.GetCustomType() == "@Element") ? 
                                 Core::SymbolType::CUSTOM_ELEMENT :
                                 Core::SymbolType::CUSTOM_VAR;
    
    const Core::SymbolInfo* symbol = globalMap_.FindSymbolByType(
        node.GetCustomName(), symbolType, node.GetNamespace());
    
    if (!symbol) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到自定义: " + node.GetCustomType() + " " + node.GetCustomName()
        );
        return;
    }
    
    // 根据自定义类型进行展开
    if (node.GetCustomType() == "@Style") {
        // 样式自定义展开
        std::string inlineStyle = context_.variables["__inline_style__"];
        
        // 基础属性
        for (const auto& prop : symbol->properties) {
            inlineStyle += prop.first + ": " + prop.second + "; ";
        }
        
        // 处理继承
        for (const auto& inherit : symbol->inherits) {
            std::string inheritNamespace, inheritName;
            if (Core::CHTLGlobalMap::ParseFullName(inherit, inheritNamespace, inheritName)) {
                const Core::SymbolInfo* inheritSymbol = globalMap_.FindSymbolByType(
                    inheritName, symbolType, inheritNamespace);
                
                if (inheritSymbol) {
                    for (const auto& prop : inheritSymbol->properties) {
                        if (inlineStyle.find(prop.first + ":") == std::string::npos) {
                            inlineStyle += prop.first + ": " + prop.second + "; ";
                        }
                    }
                }
            }
        }
        
        // 处理特例化
        if (node.HasSpecialization()) {
            for (const auto& specialization : node.GetSpecializations()) {
                auto specNode = std::dynamic_pointer_cast<AST::SpecializationNode>(specialization);
                if (specNode) {
                    // 应用属性覆盖
                    for (const auto& override : specNode->GetPropertyOverrides()) {
                        // 替换或添加属性
                        std::string propPattern = override.first + ":";
                        size_t pos = inlineStyle.find(propPattern);
                        if (pos != std::string::npos) {
                            // 查找属性值的结束位置
                            size_t endPos = inlineStyle.find(";", pos);
                            if (endPos != std::string::npos) {
                                // 替换现有属性
                                inlineStyle.replace(pos, endPos - pos + 1, 
                                                  override.first + ": " + override.second + "; ");
                            }
                        } else {
                            // 添加新属性
                            inlineStyle += override.first + ": " + override.second + "; ";
                        }
                    }
                    
                    // 处理删除操作
                    for (const auto& deletion : specNode->GetDeletions()) {
                        auto delNode = std::dynamic_pointer_cast<AST::DeletionNode>(deletion);
                        if (delNode && delNode->GetDeletionType() == AST::DeletionNode::DeletionType::PROPERTY) {
                            for (const auto& target : delNode->GetTargets()) {
                                // 从内联样式中删除指定属性
                                std::string propPattern = target + ":";
                                size_t pos = inlineStyle.find(propPattern);
                                if (pos != std::string::npos) {
                                    size_t endPos = inlineStyle.find(";", pos);
                                    if (endPos != std::string::npos) {
                                        inlineStyle.erase(pos, endPos - pos + 1);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        context_.variables["__inline_style__"] = inlineStyle;
        
    } else if (node.GetCustomType() == "@Element") {
        // 元素自定义展开
        auto expandedElements = ExpandCustom(node);
        for (const auto& element : expandedElements) {
            element->Accept(*this);
        }
        
    } else if (node.GetCustomType() == "@Var") {
        // 变量自定义处理
        if (symbol->properties.empty()) {
            // 无值样式组，需要在使用时提供值
            // 这里只是标记，实际值在变量引用时处理
            context_.variables["__valueless_group__" + symbol->name] = "true";
        } else {
            // 有值变量组
            for (const auto& prop : symbol->properties) {
                context_.variables[symbol->name + "." + prop.first] = prop.second;
            }
        }
    }
}

void CHTLGenerator::VisitVariableReferenceNode(AST::VariableReferenceNode& node) {
    variableSubstitutionCount_++;
    
    // 查找变量定义并替换
    std::string value = ExpandVariable(node);
    if (!value.empty()) {
        output_ << value;
    }
}

void CHTLGenerator::VisitCommentNode(AST::CommentNode& node) {
    if ((config_.includeComments && !node.IsGeneratorComment()) ||
        (config_.includeGeneratorComments && node.IsGeneratorComment())) {
        
        if (config_.prettyPrint) {
            output_ << GetIndent();
        }
        
        // 转换为HTML注释
        output_ << "<!-- " << node.GetContent() << " -->";
        
        if (config_.prettyPrint) {
            output_ << "\n";
        }
    }
}

// 辅助方法实现
std::string CHTLGenerator::GenerateStartTag(AST::ElementNode& element) {
    std::string tag = "<" + element.GetTagName();
    
    // 生成属性
    std::string attributes = GenerateAttributes(element);
    if (!attributes.empty()) {
        tag += " " + attributes;
    }
    
    // 检查内联样式
    auto inlineStyleIt = context_.variables.find("__inline_style__");
    if (inlineStyleIt != context_.variables.end() && !inlineStyleIt->second.empty()) {
        tag += " style=\"" + inlineStyleIt->second + "\"";
        context_.variables.erase("__inline_style__"); // 清除已使用的内联样式
    }
    
    tag += ">";
    return tag;
}

std::string CHTLGenerator::GenerateEndTag(AST::ElementNode& element) {
    return "</" + element.GetTagName() + ">";
}

std::string CHTLGenerator::GenerateAttributes(AST::ElementNode& element) {
    std::ostringstream attrs;
    bool first = true;
    
    const auto& attributes = element.GetAttributes();
    for (const auto& attr : attributes) {
        if (!first) {
            attrs << " ";
        }
        
        attrs << attr.first << "=\"" << EscapeHTML(attr.second) << "\"";
        first = false;
    }
    
    return attrs.str();
}

std::string CHTLGenerator::ExpandVariable(AST::VariableReferenceNode& varRef) {
    // 查找变量组
    const Core::SymbolInfo* symbol = globalMap_.FindSymbolByType(
        varRef.GetGroupName(), Core::SymbolType::TEMPLATE_VAR);
    
    if (!symbol) {
        symbol = globalMap_.FindSymbolByType(
            varRef.GetGroupName(), Core::SymbolType::CUSTOM_VAR);
    }
    
    if (!symbol) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到变量组: " + varRef.GetGroupName()
        );
        return "";
    }
    
    // 查找具体变量
    auto it = symbol->properties.find(varRef.GetVariableName());
    if (it == symbol->properties.end()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到变量: " + varRef.GetVariableName() + " 在组 " + varRef.GetGroupName() + " 中"
        );
        return "";
    }
    
    std::string value = it->second;
    
    // 处理特例化参数
    if (varRef.HasSpecialization()) {
        const auto& params = varRef.GetSpecializationParams();
        auto paramIt = params.find(varRef.GetVariableName());
        if (paramIt != params.end()) {
            value = paramIt->second;
        }
    }
    
    return value;
}

std::string CHTLGenerator::GenerateAutoClassName(const std::string& baseName) {
    std::string className;
    
    if (baseName.empty()) {
        className = "auto-class-" + std::to_string(context_.autoClassCounter++);
    } else {
        className = baseName + "-" + std::to_string(context_.autoClassCounter++);
    }
    
    context_.generatedClasses.insert(className);
    return className;
}

std::string CHTLGenerator::GenerateAutoId(const std::string& baseName) {
    std::string id;
    
    if (baseName.empty()) {
        id = "auto-id-" + std::to_string(context_.autoIdCounter++);
    } else {
        id = baseName + "-" + std::to_string(context_.autoIdCounter++);
    }
    
    context_.generatedIds.insert(id);
    return id;
}

void CHTLGenerator::ProcessGlobalStyleSelector(AST::CSSSelectorNode& selector) {
    if (selector.IsAddedToGlobalStyle()) {
        std::ostringstream globalStyle;
        globalStyle << selector.GetSelector() << " {\n";
        
        // 处理选择器的子属性
        for (const auto& child : selector.GetChildren()) {
            if (child->GetType() == AST::NodeType::CSS_PROPERTY) {
                auto property = std::dynamic_pointer_cast<AST::CSSPropertyNode>(child);
                if (property) {
                    globalStyle << "  " << property->GetProperty() 
                               << ": " << property->GetValue() << ";\n";
                }
            }
        }
        
        globalStyle << "}\n";
        CollectGlobalStyle(globalStyle.str());
    }
}

void CHTLGenerator::CollectGlobalStyle(const std::string& styleContent) {
    if (!styleContent.empty()) {
        context_.globalStyles.push_back(styleContent);
    }
}

void CHTLGenerator::CollectGlobalScript(const std::string& scriptContent) {
    if (!scriptContent.empty()) {
        context_.globalScripts.push_back(scriptContent);
    }
}

std::string CHTLGenerator::GenerateCompleteHTML() {
    std::ostringstream html;
    
    // 检查是否有完整的HTML结构
    std::string bodyContent = output_.str();
    bool hasHtmlTag = bodyContent.find("<html") != std::string::npos;
    bool hasHeadTag = bodyContent.find("<head") != std::string::npos;
    bool hasBodyTag = bodyContent.find("<body") != std::string::npos;
    
    if (!hasHtmlTag) {
        // 生成完整的HTML文档结构
        html << "<!DOCTYPE html>\n";
        html << "<html lang=\"zh-CN\">\n";
        
        // 生成头部
        html << GenerateHTMLHead();
        
        // 生成主体
        html << "<body>\n";
        html << bodyContent;
        html << "</body>\n";
        html << "</html>\n";
    } else {
        // 如果已有HTML结构，插入样式和脚本
        std::string result = bodyContent;
        
        // 在</head>前插入样式
        std::string globalStyles = GenerateGlobalStyles();
        if (!globalStyles.empty()) {
            size_t headEnd = result.find("</head>");
            if (headEnd != std::string::npos) {
                result.insert(headEnd, globalStyles);
            }
        }
        
        // 在</body>前插入脚本
        std::string globalScripts = GenerateGlobalScripts();
        if (!globalScripts.empty()) {
            size_t bodyEnd = result.find("</body>");
            if (bodyEnd != std::string::npos) {
                result.insert(bodyEnd, globalScripts);
            }
        }
        
        html << result;
    }
    
    return html.str();
}

std::string CHTLGenerator::GenerateHTMLHead() {
    std::ostringstream head;
    
    head << "<head>\n";
    head << "  <meta charset=\"UTF-8\">\n";
    head << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    head << "  <title>CHTL Generated Page</title>\n";
    
    // 添加全局样式
    std::string globalStyles = GenerateGlobalStyles();
    if (!globalStyles.empty()) {
        head << globalStyles;
    }
    
    head << "</head>\n";
    
    return head.str();
}

std::string CHTLGenerator::GenerateGlobalStyles() {
    if (context_.globalStyles.empty()) {
        return "";
    }
    
    std::ostringstream styles;
    styles << "  <style>\n";
    
    for (const auto& style : context_.globalStyles) {
        styles << "    " << style;
        if (!Utils::StringUtils::EndsWith(style, "\n")) {
            styles << "\n";
        }
    }
    
    styles << "  </style>\n";
    
    return styles.str();
}

std::string CHTLGenerator::GenerateGlobalScripts() {
    if (context_.globalScripts.empty()) {
        return "";
    }
    
    std::ostringstream scripts;
    scripts << "  <script>\n";
    
    for (const auto& script : context_.globalScripts) {
        scripts << "    " << script;
        if (!Utils::StringUtils::EndsWith(script, "\n")) {
            scripts << "\n";
        }
    }
    
    scripts << "  </script>\n";
    
    return scripts.str();
}

void CHTLGenerator::AddIndent() {
    currentIndent_++;
}

void CHTLGenerator::RemoveIndent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

std::string CHTLGenerator::GetIndent() const {
    std::string indent = "";
    for (int i = 0; i < currentIndent_ * config_.indentSize; ++i) {
        indent += config_.indentChar;
    }
    return indent;
}

std::string CHTLGenerator::EscapeHTML(const std::string& text) {
    return Utils::StringUtils::EscapeHtml(text);
}

std::string CHTLGenerator::EscapeCSS(const std::string& css) {
    return Utils::StringUtils::EscapeCss(css);
}

std::string CHTLGenerator::EscapeJavaScript(const std::string& js) {
    return Utils::StringUtils::EscapeJavaScript(js);
}

std::string CHTLGenerator::MinifyHTML(const std::string& html) {
    std::string minified = html;
    
    // 移除多余的空白字符
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除标签间的空白
    minified = std::regex_replace(minified, std::regex(R"(>\s+<)"), "><");
    
    // 移除行首行尾空白
    minified = Utils::StringUtils::Trim(minified);
    
    return minified;
}

bool CHTLGenerator::ValidateHTML(const std::string& html) {
    // 简单的HTML验证
    
    // 检查标签是否匹配
    std::vector<std::string> tagStack;
    std::regex tagRegex(R"(<(\/?)\s*([a-zA-Z][a-zA-Z0-9]*)\s*[^>]*>)");
    std::sregex_iterator iter(html.begin(), html.end(), tagRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        bool isClosing = !match[1].str().empty();
        std::string tagName = Utils::StringUtils::ToLower(match[2].str());
        
        // 跳过自闭合标签
        static const std::unordered_set<std::string> selfClosingTags = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };
        
        if (selfClosingTags.find(tagName) != selfClosingTags.end()) {
            continue;
        }
        
        if (isClosing) {
            if (tagStack.empty() || tagStack.back() != tagName) {
                Utils::ErrorHandler::GetInstance().LogWarning(
                    "HTML标签不匹配: </" + tagName + ">"
                );
                return false;
            }
            tagStack.pop_back();
        } else {
            tagStack.push_back(tagName);
        }
    }
    
    if (!tagStack.empty()) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "存在未闭合的HTML标签"
        );
        return false;
    }
    
    return true;
}

std::string CHTLGenerator::FormatHTML(const std::string& html) {
    // 简单的HTML格式化
    std::string formatted = html;
    
    // 在标签后添加换行
    formatted = std::regex_replace(formatted, std::regex(R"(>([^<\s]))"), ">\n$1");
    formatted = std::regex_replace(formatted, std::regex(R"(([^>\s])<)"), "$1\n<");
    
    return formatted;
}

void CHTLGenerator::Reset() {
    output_.str("");
    output_.clear();
    currentIndent_ = 0;
    context_ = GenerationContext();
    elementCount_ = 0;
    templateExpandCount_ = 0;
    customExpandCount_ = 0;
    variableSubstitutionCount_ = 0;
}

std::string CHTLGenerator::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL代码生成统计:\n";
    oss << "  元素数量: " << elementCount_ << "\n";
    oss << "  模板展开数量: " << templateExpandCount_ << "\n";
    oss << "  自定义展开数量: " << customExpandCount_ << "\n";
    oss << "  变量替换数量: " << variableSubstitutionCount_ << "\n";
    oss << "  生成的CSS类数量: " << context_.generatedClasses.size() << "\n";
    oss << "  生成的CSS ID数量: " << context_.generatedIds.size() << "\n";
    oss << "  全局样式数量: " << context_.globalStyles.size() << "\n";
    oss << "  全局脚本数量: " << context_.globalScripts.size() << "\n";
    
    return oss.str();
}

// 存根实现
AST::ASTNodeList CHTLGenerator::ExpandTemplate(AST::TemplateReferenceNode& templateRef) {
    return AST::ASTNodeList();
}

AST::ASTNodeList CHTLGenerator::ExpandCustom(AST::CustomReferenceNode& customRef) {
    return AST::ASTNodeList();
}

// GeneratorFactory 实现
std::unique_ptr<CHTLGenerator> GeneratorFactory::CreateDefaultGenerator(Core::CHTLGlobalMap& globalMap) {
    return std::make_unique<CHTLGenerator>(globalMap);
}

std::unique_ptr<CHTLGenerator> GeneratorFactory::CreateMinifyGenerator(Core::CHTLGlobalMap& globalMap) {
    GeneratorConfig config;
    config.prettyPrint = false;
    config.minify = true;
    config.includeComments = false;
    config.includeGeneratorComments = false;
    return std::make_unique<CHTLGenerator>(globalMap, config);
}

std::unique_ptr<CHTLGenerator> GeneratorFactory::CreateDebugGenerator(Core::CHTLGlobalMap& globalMap) {
    GeneratorConfig config;
    config.prettyPrint = true;
    config.minify = false;
    config.includeComments = true;
    config.includeGeneratorComments = true;
    config.validateOutput = true;
    return std::make_unique<CHTLGenerator>(globalMap, config);
}

} // namespace Generator
} // namespace CHTL
