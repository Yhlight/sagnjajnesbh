#include "CHTLGeneratorV3.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace chtl {

CHTLGeneratorV3::CHTLGeneratorV3() : m_IndentLevel(0) {
    // 设置默认配置
    m_Configuration["INDEX_INITIAL_COUNT"] = "0";
    m_Configuration["DEBUG_MODE"] = "false";
    m_Configuration["DISABLE_NAME_GROUP"] = "false";
    m_Configuration["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
}

std::string CHTLGeneratorV3::Generate(std::shared_ptr<ast::DocumentNode> document, 
                                      const GeneratorOptions& options) {
    m_Options = options;
    m_Output.str("");
    m_GlobalStyles.str("");
    m_GlobalScripts.str("");
    m_LocalStyleRules.clear();
    m_IndentLevel = 0;
    
    // 访问文档收集信息
    document->Accept(this);
    
    // 生成最终输出
    return GenerateFinalOutput();
}

std::string CHTLGeneratorV3::GenerateFinalOutput() {
    if (m_Options.fragmentOnly) {
        return m_Output.str();
    }
    
    std::stringstream final_output;
    
    // HTML5 文档类型
    final_output << "<!DOCTYPE html>\n";
    final_output << "<html>\n";
    final_output << "<head>\n";
    final_output << "    <meta charset=\"UTF-8\">\n";
    final_output << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    final_output << "    <title>CHTL Generated Page</title>\n";
    
    // 生成样式
    std::string styles = GenerateStyleContent();
    if (!styles.empty()) {
        final_output << "    <style>\n";
        if (!m_Options.minify) {
            std::istringstream styleStream(styles);
            std::string line;
            while (std::getline(styleStream, line)) {
                final_output << "        " << line << "\n";
            }
        } else {
            final_output << styles;
        }
        final_output << "    </style>\n";
    }
    
    final_output << "</head>\n";
    final_output << "<body>\n";
    
    // 主体内容
    std::istringstream contentStream(m_Output.str());
    std::string line;
    while (std::getline(contentStream, line)) {
        if (!line.empty() || !m_Options.minify) {
            final_output << "    " << line << "\n";
        }
    }
    
    // 生成脚本
    std::string scripts = GenerateScriptContent();
    if (!scripts.empty()) {
        final_output << "    <script>\n";
        if (!m_Options.minify) {
            std::istringstream scriptStream(scripts);
            while (std::getline(scriptStream, line)) {
                final_output << "        " << line << "\n";
            }
        } else {
            final_output << scripts;
        }
        final_output << "    </script>\n";
    }
    
    final_output << "</body>\n";
    final_output << "</html>\n";
    
    return final_output.str();
}

// 文档访问
void CHTLGeneratorV3::VisitDocument(ast::DocumentNode* node) {
    VisitChildren(node);
}

// 元素访问
void CHTLGeneratorV3::VisitElement(ast::ElementNode* node) {
    const std::string& tag = node->GetTagName();
    
    // 检查约束
    if (!CheckConstraints(tag)) {
        return;
    }
    
    // 是否是自闭合标签
    bool isSelfClosing = IsSelfClosingTag(tag);
    
    // 是否是内联元素
    bool isInline = IsInlineElement(tag);
    
    WriteIndent();
    Write("<" + tag);
    
    // 属性
    for (const auto& attr : node->GetAttributes()) {
        Write(" " + attr.first);
        if (!attr.second.empty()) {
            Write("=\"" + EscapeHTML(attr.second) + "\"");
        }
    }
    
    // 处理局部样式生成的内联样式
    std::unordered_map<std::string, std::string> mergedInlineProps;
    
    for (const auto& child : node->GetChildren()) {
        if (auto style = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            if (style->GetType() == ast::StyleNode::LOCAL) {
                ProcessStyleInheritance(style.get(), mergedInlineProps);
                
                // 收集本地内联属性
                for (const auto& prop : style->GetInlineProperties()) {
                    mergedInlineProps[prop.first] = prop.second;
                }
            }
        }
    }
    
    // 生成内联样式属性
    if (!mergedInlineProps.empty()) {
        Write(" style=\"");
        bool first = true;
        for (const auto& prop : mergedInlineProps) {
            if (!first) Write(" ");
            Write(prop.first + ": " + prop.second + ";");
            first = false;
        }
        Write("\"");
    }
    
    // 自闭合标签
    if (isSelfClosing && node->GetChildren().empty()) {
        Write(" />");
        if (m_Options.prettyPrint) WriteLine();
        return;
    }
    
    Write(">");
    
    // 检查是否有块级子元素
    bool hasBlockChildren = false;
    for (const auto& child : node->GetChildren()) {
        if (auto elem = std::dynamic_pointer_cast<ast::ElementNode>(child)) {
            if (!IsInlineElement(elem->GetTagName())) {
                hasBlockChildren = true;
                break;
            }
        }
    }
    
    if (m_Options.prettyPrint && (hasBlockChildren || !isInline)) {
        WriteLine();
        IncreaseIndent();
    }
    
    // 保存当前元素上下文
    auto oldElement = m_Context.currentElement;
    m_Context.currentElement = node;
    
    // 访问子节点
    for (const auto& child : node->GetChildren()) {
        // 跳过已处理的局部样式和脚本
        if (auto style = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            if (style->GetType() == ast::StyleNode::LOCAL) {
                ProcessLocalStyle(style.get());
                continue;
            }
        }
        if (auto script = std::dynamic_pointer_cast<ast::ScriptNode>(child)) {
            if (script->GetType() == ast::ScriptNode::LOCAL) {
                ProcessLocalScript(script.get());
                continue;
            }
        }
        
        child->Accept(this);
    }
    
    m_Context.currentElement = oldElement;
    
    if (m_Options.prettyPrint && (hasBlockChildren || !isInline)) {
        DecreaseIndent();
        WriteIndent();
    }
    
    // 结束标签
    Write("</" + tag + ">");
    
    if (m_Options.prettyPrint) {
        WriteLine();
    }
}

// 文本访问
void CHTLGeneratorV3::VisitText(ast::TextNode* node) {
    WriteIndent();
    Write(EscapeHTML(node->GetContent()));
    if (m_Options.prettyPrint) WriteLine();
}

// 样式访问
void CHTLGeneratorV3::VisitStyle(ast::StyleNode* node) {
    if (node->GetType() == ast::StyleNode::GLOBAL) {
        CollectGlobalStyle(node);
    }
    // 局部样式在元素访问时处理
}

// 脚本访问
void CHTLGeneratorV3::VisitScript(ast::ScriptNode* node) {
    if (node->GetType() == ast::ScriptNode::GLOBAL) {
        CollectGlobalScript(node);
    }
    // 局部脚本在元素访问时处理
}

// 注释访问
void CHTLGeneratorV3::VisitComment(ast::CommentNode* node) {
    if (node->GetType() == ast::CommentNode::HTML_COMMENT) {
        WriteIndent();
        Write("<!-- " + node->GetContent() + " -->");
        if (m_Options.prettyPrint) WriteLine();
    }
    // 其他类型的注释不输出到HTML
}

// 模板访问
void CHTLGeneratorV3::VisitTemplate(ast::TemplateNode* node) {
    RegisterTemplate(node);
    ProcessTemplateInheritance(node);
    // 模板定义不生成输出
}

// 自定义访问
void CHTLGeneratorV3::VisitCustom(ast::CustomNode* node) {
    if (node->IsSpecialization()) {
        // 特例化使用
        ProcessSpecialization(node);
    } else if (m_Context.currentElement != nullptr || 
               m_Context.currentStyle != nullptr) {
        // 在元素或样式中的使用
        ProcessCustomUsage(node);
    } else {
        // 定义
        ProcessCustomDefinition(node);
    }
}

// 变量访问
void CHTLGeneratorV3::VisitVar(ast::VarNode* node) {
    RegisterVariable(node);
    // 变量定义不生成输出
}

// 变量调用访问
void CHTLGeneratorV3::VisitVarCall(ast::VarCallNode* node) {
    // 变量调用在属性值处理时解析
}

// 导入访问
void CHTLGeneratorV3::VisitImport(ast::ImportNode* node) {
    ProcessImport(node);
    // 导入不直接生成输出
}

// 命名空间访问
void CHTLGeneratorV3::VisitNamespace(ast::NamespaceNode* node) {
    EnterNamespace(node->GetName());
    VisitChildren(node);
    ExitNamespace();
    // 命名空间不生成输出
}

// 配置访问
void CHTLGeneratorV3::VisitConfiguration(ast::ConfigurationNode* node) {
    ProcessConfiguration(node);
    // 配置不生成输出
}

// 原始嵌入访问
void CHTLGeneratorV3::VisitOrigin(ast::OriginNode* node) {
    // 如果有名称但无内容，则是引用
    if (!node->GetName().empty() && node->GetContent().empty()) {
        // TODO: 查找并输出已命名的原始嵌入
        return;
    }
    
    switch (node->GetType()) {
        case ast::OriginNode::HTML:
            // 直接输出HTML内容
            if (m_Options.prettyPrint) {
                WriteIndent();
            }
            Write(node->GetContent());
            if (m_Options.prettyPrint) {
                WriteLine();
            }
            break;
            
        case ast::OriginNode::STYLE:
            m_GlobalStyles << node->GetContent() << "\n";
            break;
            
        case ast::OriginNode::JAVASCRIPT:
            m_GlobalScripts << node->GetContent() << "\n";
            break;
            
        case ast::OriginNode::CUSTOM:
            // 自定义类型处理
            // TODO: 根据配置的OriginType处理
            break;
    }
}

// 继承访问
void CHTLGeneratorV3::VisitInherit(ast::InheritNode* node) {
    // 继承在模板和自定义处理中解析
}

// 删除访问
void CHTLGeneratorV3::VisitDelete(ast::DeleteNode* node) {
    ProcessDelete(node);
}

// 插入访问
void CHTLGeneratorV3::VisitInsert(ast::InsertNode* node) {
    ProcessInsert(node);
}

// 约束访问
void CHTLGeneratorV3::VisitExcept(ast::ExceptNode* node) {
    AddConstraint(node);
    // 约束不生成输出
}

// 属性访问
void CHTLGeneratorV3::VisitAttribute(ast::AttributeNode* node) {
    // 属性在元素中处理
}

// 样式处理
void CHTLGeneratorV3::ProcessLocalStyle(ast::StyleNode* style) {
    // 收集需要提升到全局的规则
    for (const auto& rule : style->GetRules()) {
        std::string selector = rule.first;
        
        // 替换 & 为实际的选择器
        if (!style->GetAutoClassName().empty()) {
            size_t pos = 0;
            while ((pos = selector.find("&", pos)) != std::string::npos) {
                selector.replace(pos, 1, "." + style->GetAutoClassName());
                pos += style->GetAutoClassName().length() + 1;
            }
        } else if (!style->GetAutoId().empty()) {
            size_t pos = 0;
            while ((pos = selector.find("&", pos)) != std::string::npos) {
                selector.replace(pos, 1, "#" + style->GetAutoId());
                pos += style->GetAutoId().length() + 1;
            }
        }
        
        m_LocalStyleRules.push_back(selector + " { " + rule.second + " }");
    }
}

void CHTLGeneratorV3::CollectGlobalStyle(ast::StyleNode* style) {
    // 处理全局样式中的继承
    std::unordered_map<std::string, std::string> inheritedProps;
    ProcessStyleInheritance(style, inheritedProps);
    
    // 输出继承的属性
    for (const auto& prop : inheritedProps) {
        m_GlobalStyles << prop.first << ": " << prop.second << ";\n";
    }
    
    // 输出规则
    for (const auto& rule : style->GetRules()) {
        m_GlobalStyles << rule.first << " { " << rule.second << " }\n";
    }
}

void CHTLGeneratorV3::ProcessStyleInheritance(ast::StyleNode* style,
                                              std::unordered_map<std::string, std::string>& properties) {
    // 处理样式中的继承
    for (const auto& child : style->GetChildren()) {
        if (auto inherit = std::dynamic_pointer_cast<ast::InheritNode>(child)) {
            // 收集继承的模板属性
            std::unordered_set<std::string> visited;
            CollectTemplateStyleProperties(inherit->GetBase(), properties, visited);
        } else if (auto custom = std::dynamic_pointer_cast<ast::CustomNode>(child)) {
            if (custom->GetType() == ast::CustomNode::STYLE && !custom->IsSpecialization()) {
                // 简单的样式模板使用
                std::unordered_set<std::string> visited;
                CollectTemplateStyleProperties(custom->GetName(), properties, visited);
            }
        }
    }
}

void CHTLGeneratorV3::CollectTemplateStyleProperties(const std::string& templateName,
                                                     std::unordered_map<std::string, std::string>& properties,
                                                     std::unordered_set<std::string>& visitedTemplates) {
    // 防止循环继承
    if (visitedTemplates.find(templateName) != visitedTemplates.end()) {
        return;
    }
    visitedTemplates.insert(templateName);
    
    // 解析全缀名
    std::string resolvedName = ResolveFullQualifiedName(templateName);
    
    // 查找模板
    auto tmplIt = m_Templates.find(resolvedName);
    if (tmplIt == m_Templates.end()) {
        // 尝试查找自定义
        auto customIt = m_Customs.find(resolvedName);
        if (customIt != m_Customs.end()) {
            auto custom = customIt->second;
            if (custom->GetType() == ast::CustomNode::STYLE) {
                // 处理自定义样式
                for (const auto& child : custom->GetChildren()) {
                    if (auto styleNode = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
                        // 无值属性处理
                        for (const auto& prop : styleNode->GetNoValueProperties()) {
                            // 无值属性需要在使用时提供值
                            if (properties.find(prop) == properties.end()) {
                                properties[prop] = ""; // 标记为需要值
                            }
                        }
                        
                        // 有值属性
                        for (const auto& prop : styleNode->GetInlineProperties()) {
                            properties[prop.first] = prop.second;
                        }
                    }
                }
            }
        }
        return;
    }
    
    auto tmpl = tmplIt->second;
    if (tmpl->GetType() != ast::TemplateNode::STYLE) {
        return;
    }
    
    // 遍历模板的子节点
    for (const auto& child : tmpl->GetChildren()) {
        if (auto styleNode = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            // 收集内联属性
            for (const auto& prop : styleNode->GetInlineProperties()) {
                properties[prop.first] = prop.second;
            }
        } else if (auto inheritNode = std::dynamic_pointer_cast<ast::InheritNode>(child)) {
            // 递归处理继承
            CollectTemplateStyleProperties(inheritNode->GetBase(), properties, visitedTemplates);
        }
    }
}

std::string CHTLGeneratorV3::GenerateStyleContent() {
    std::stringstream styles;
    
    // 全局样式
    if (m_GlobalStyles.str().length() > 0) {
        styles << m_GlobalStyles.str();
    }
    
    // 局部样式规则
    for (const auto& rule : m_LocalStyleRules) {
        styles << rule << "\n";
    }
    
    return styles.str();
}

// 脚本处理
void CHTLGeneratorV3::ProcessLocalScript(ast::ScriptNode* script) {
    // 局部脚本直接添加到全局脚本中
    m_GlobalScripts << script->GetContent() << "\n";
}

void CHTLGeneratorV3::CollectGlobalScript(ast::ScriptNode* script) {
    m_GlobalScripts << script->GetContent() << "\n";
}

std::string CHTLGeneratorV3::GenerateScriptContent() {
    return m_GlobalScripts.str();
}

// 模板处理
void CHTLGeneratorV3::RegisterTemplate(ast::TemplateNode* tmpl) {
    std::string fullName = m_CurrentNamespace.empty() ? 
        tmpl->GetName() : 
        m_CurrentNamespace + "." + tmpl->GetName();
    
    m_Templates[fullName] = tmpl;
}

void CHTLGeneratorV3::ProcessTemplateInheritance(ast::TemplateNode* tmpl) {
    // 模板继承已在使用时处理
}

void CHTLGeneratorV3::InstantiateTemplate(const std::string& templateName, ast::CustomNode* usage) {
    std::string resolvedName = ResolveFullQualifiedName(templateName);
    
    auto tmplIt = m_Templates.find(resolvedName);
    if (tmplIt == m_Templates.end()) {
        return;
    }
    
    auto tmpl = tmplIt->second;
    
    // 根据模板类型实例化
    if (tmpl->GetType() == ast::TemplateNode::ELEMENT) {
        // 元素模板实例化
        for (const auto& child : tmpl->GetChildren()) {
            child->Accept(this);
        }
    }
    // 样式和变量模板在其他地方处理
}

// 自定义处理
void CHTLGeneratorV3::ProcessCustomDefinition(ast::CustomNode* custom) {
    std::string fullName = m_CurrentNamespace.empty() ? 
        custom->GetName() : 
        m_CurrentNamespace + "." + custom->GetName();
    
    m_Customs[fullName] = custom;
    // 自定义定义不生成输出
}

void CHTLGeneratorV3::ProcessCustomUsage(ast::CustomNode* custom) {
    if (custom->GetType() == ast::CustomNode::ELEMENT) {
        InstantiateTemplate(custom->GetName(), custom);
    }
    // 样式和变量使用在其他地方处理
}

void CHTLGeneratorV3::ProcessSpecialization(ast::CustomNode* custom) {
    // 特例化处理
    auto oldCustom = m_Context.currentCustom;
    m_Context.currentCustom = custom;
    m_Context.inSpecialization = true;
    
    // 处理特例化内容
    VisitChildren(custom);
    
    m_Context.currentCustom = oldCustom;
    m_Context.inSpecialization = false;
}

// 变量处理
void CHTLGeneratorV3::RegisterVariable(ast::VarNode* var) {
    std::string group = m_CurrentNamespace.empty() ? 
        "global" : m_CurrentNamespace;
    
    m_Variables[group][var->GetName()] = var->GetValue();
}

std::string CHTLGeneratorV3::ResolveVariable(const std::string& varGroup, const std::string& varName) {
    // 解析变量组名
    std::string resolvedGroup = ResolveFullQualifiedName(varGroup);
    
    auto groupIt = m_Variables.find(resolvedGroup);
    if (groupIt != m_Variables.end()) {
        auto varIt = groupIt->second.find(varName);
        if (varIt != groupIt->second.end()) {
            return varIt->second;
        }
    }
    
    // 尝试从模板中查找
    auto tmplIt = m_Templates.find(resolvedGroup);
    if (tmplIt != m_Templates.end() && 
        tmplIt->second->GetType() == ast::TemplateNode::VAR) {
        for (const auto& child : tmplIt->second->GetChildren()) {
            if (auto varNode = std::dynamic_pointer_cast<ast::VarNode>(child)) {
                if (varNode->GetName() == varName) {
                    return varNode->GetValue();
                }
            }
        }
    }
    
    return "";
}

std::string CHTLGeneratorV3::ProcessVarCall(ast::VarCallNode* varCall) {
    if (varCall->HasOverride()) {
        return varCall->GetOverrideValue();
    }
    
    return ResolveVariable(varCall->GetVarGroup(), varCall->GetVarName());
}

// 导入处理
void CHTLGeneratorV3::ProcessImport(ast::ImportNode* import) {
    std::string resolvedPath = ResolvePath(import->GetPath(), import->GetType());
    
    if (m_ImportedFiles.find(resolvedPath) != m_ImportedFiles.end()) {
        // 已导入，跳过
        return;
    }
    
    m_ImportedFiles.insert(resolvedPath);
    ImportFile(resolvedPath, import);
}

std::string CHTLGeneratorV3::ResolvePath(const std::string& path, ast::ImportNode::ImportType type) {
    // 根据CHTL语法文档的路径解析规则
    std::filesystem::path resolvedPath;
    
    if (type == ast::ImportNode::CHTL || type == ast::ImportNode::AUTO) {
        // CHTL文件路径解析
        if (!path.ends_with(".chtl") && !path.ends_with(".cmod")) {
            // 只有名称，按优先级查找
            
            // 1. 官方模块目录
            std::filesystem::path officialModule = "module/" + path + ".cmod";
            if (std::filesystem::exists(officialModule)) {
                return officialModule.string();
            }
            
            officialModule = "module/" + path + ".chtl";
            if (std::filesystem::exists(officialModule)) {
                return officialModule.string();
            }
            
            // 2. 当前目录的module文件夹
            std::filesystem::path localModule = "./module/" + path + ".cmod";
            if (std::filesystem::exists(localModule)) {
                return localModule.string();
            }
            
            localModule = "./module/" + path + ".chtl";
            if (std::filesystem::exists(localModule)) {
                return localModule.string();
            }
            
            // 3. 当前目录
            std::filesystem::path currentDir = "./" + path + ".cmod";
            if (std::filesystem::exists(currentDir)) {
                return currentDir.string();
            }
            
            currentDir = "./" + path + ".chtl";
            if (std::filesystem::exists(currentDir)) {
                return currentDir.string();
            }
        }
    }
    
    // 默认返回原路径
    return path;
}

void CHTLGeneratorV3::ImportFile(const std::string& resolvedPath, ast::ImportNode* import) {
    // TODO: 实际导入文件并解析
    // 这里需要读取文件，解析，并根据导入类型处理内容
}

// 命名空间处理
void CHTLGeneratorV3::EnterNamespace(const std::string& name) {
    if (!m_CurrentNamespace.empty()) {
        m_NamespaceStack.push(m_CurrentNamespace);
        m_CurrentNamespace += "." + name;
    } else {
        m_CurrentNamespace = name;
    }
}

void CHTLGeneratorV3::ExitNamespace() {
    if (!m_NamespaceStack.empty()) {
        m_CurrentNamespace = m_NamespaceStack.top();
        m_NamespaceStack.pop();
    } else {
        m_CurrentNamespace.clear();
    }
}

std::string CHTLGeneratorV3::ResolveFullQualifiedName(const std::string& name) {
    // 如果已经是全缀名（包含.），直接返回
    if (name.find('.') != std::string::npos) {
        return name;
    }
    
    // 否则，根据当前命名空间解析
    if (!m_CurrentNamespace.empty()) {
        // 先尝试在当前命名空间中查找
        std::string fullName = m_CurrentNamespace + "." + name;
        if (m_Templates.find(fullName) != m_Templates.end() ||
            m_Customs.find(fullName) != m_Customs.end()) {
            return fullName;
        }
    }
    
    // 返回原名称
    return name;
}

// 配置处理
void CHTLGeneratorV3::ProcessConfiguration(ast::ConfigurationNode* config) {
    // 只处理未命名的配置（活动配置）
    if (!config->GetName().empty()) {
        return;
    }
    
    // 更新配置
    for (const auto& cfg : config->GetConfigs()) {
        m_Configuration[cfg.first] = cfg.second;
    }
    
    // 更新Name配置
    for (const auto& nameCfg : config->GetNameConfigs()) {
        m_NameConfiguration[nameCfg.first] = nameCfg.second;
    }
    
    // 更新OriginType配置
    for (const auto& originType : config->GetOriginTypes()) {
        m_OriginTypes[originType.first] = originType.second;
    }
}

std::string CHTLGeneratorV3::GetConfig(const std::string& key) {
    auto it = m_Configuration.find(key);
    if (it != m_Configuration.end()) {
        return it->second;
    }
    return "";
}

// 约束检查
bool CHTLGeneratorV3::CheckConstraints(const std::string& element) {
    // 检查所有活动约束
    for (const auto& constraint : m_Constraints) {
        for (const auto& target : constraint->GetTargets()) {
            // 精确匹配
            if (target == element) {
                return false;
            }
            
            // 类型匹配
            if (target == "@Html") {
                // 检查是否是HTML元素
                if (IsHtmlElement(element)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void CHTLGeneratorV3::AddConstraint(ast::ExceptNode* except) {
    m_Constraints.push_back(except);
}

// 特殊化处理
void CHTLGeneratorV3::ProcessDelete(ast::DeleteNode* deleteNode) {
    // 删除操作在特例化上下文中处理
    if (!m_Context.inSpecialization) {
        return;
    }
    
    // TODO: 实现具体的删除逻辑
}

void CHTLGeneratorV3::ProcessInsert(ast::InsertNode* insertNode) {
    // 插入操作在特例化上下文中处理
    if (!m_Context.inSpecialization) {
        return;
    }
    
    // 直接生成插入的内容
    for (const auto& child : insertNode->GetChildren()) {
        child->Accept(this);
    }
}

// 辅助方法
void CHTLGeneratorV3::Write(const std::string& text) {
    m_Output << text;
}

void CHTLGeneratorV3::WriteLine(const std::string& text) {
    m_Output << text << "\n";
}

void CHTLGeneratorV3::WriteIndent() {
    if (!m_Options.prettyPrint || m_Options.minify) return;
    for (int i = 0; i < m_IndentLevel; ++i) {
        m_Output << "    ";
    }
}

void CHTLGeneratorV3::IncreaseIndent() {
    m_IndentLevel++;
}

void CHTLGeneratorV3::DecreaseIndent() {
    if (m_IndentLevel > 0) m_IndentLevel--;
}

std::string CHTLGeneratorV3::EscapeHTML(const std::string& text) const {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c;
        }
    }
    return result;
}

bool CHTLGeneratorV3::IsInlineElement(const std::string& tag) const {
    static const std::unordered_set<std::string> inlineTags = {
        "span", "a", "strong", "em", "b", "i", "u", "code",
        "small", "sub", "sup", "mark", "del", "ins", "cite",
        "q", "abbr", "time", "var", "samp", "kbd"
    };
    return inlineTags.find(tag) != inlineTags.end();
}

bool CHTLGeneratorV3::IsSelfClosingTag(const std::string& tag) const {
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img",
        "input", "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags.find(tag) != selfClosingTags.end();
}

bool CHTLGeneratorV3::IsHtmlElement(const std::string& tag) const {
    // 简化的HTML元素检查
    // 如果不是以大写字母开头，通常是HTML元素
    return !tag.empty() && std::islower(tag[0]);
}

void CHTLGeneratorV3::VisitChildren(ast::ASTNode* node) {
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
}

} // namespace chtl