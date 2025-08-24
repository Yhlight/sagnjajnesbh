#include "CHTL/Enhanced/OriginAnywhereSystem.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace Enhanced {

// ============ EnhancedOriginManager 实现 ============

EnhancedOriginManager::EnhancedOriginManager() {
    std::cout << "🚀 初始化增强的Origin任意位置管理器" << std::endl;
    std::cout << "核心理念：[Origin]原始嵌入节点任意地方都可以使用，不应该被束缚" << std::endl;
    std::cout << "问题由开发者自己控制" << std::endl;
}

bool EnhancedOriginManager::canUseOriginAnywhere(const OriginPosition& position, 
                                               OriginType originType) const {
    // 核心理念：[Origin]原始嵌入节点任意地方都可以使用，不应该被束缚
    std::cout << "✅ [Origin]可以在任意位置使用：" 
              << "行 " << position.line 
              << ", 列 " << position.column 
              << ", 上下文: " << getContextName(position.context) 
              << ", 类型: " << getOriginTypeName(originType) << std::endl;
    
    // 始终返回true，因为Origin设计理念就是可以在任意位置使用
    return true;
}

std::string EnhancedOriginManager::getOriginUsageAdvice(const OriginPosition& position, 
                                                       OriginType originType) const {
    std::ostringstream advice;
    
    advice << "[Origin]使用建议：\n";
    advice << "📍 当前位置：" << getContextName(position.context) << "\n";
    advice << "🎯 Origin类型：" << getOriginTypeName(originType) << "\n";
    advice << "📝 嵌套层级：" << position.nestingLevel << "\n\n";
    
    // 根据上下文提供具体建议
    switch (position.context) {
        case ContextType::CHTL_ROOT:
            advice << "💡 在CHTL根级别使用Origin是一个很好的选择！\n";
            advice << "   - 可以定义全局的HTML、CSS、JS代码\n";
            advice << "   - 适合放置全局配置和初始化代码\n";
            break;
            
        case ContextType::CHTL_ELEMENT:
            advice << "💡 在CHTL元素内部使用Origin非常灵活！\n";
            advice << "   - 可以嵌入特定的HTML片段\n";
            advice << "   - 可以添加元素特定的样式和脚本\n";
            advice << "   - 适合创建复杂的交互组件\n";
            break;
            
        case ContextType::CHTL_STYLE:
            advice << "💡 在样式块中使用Origin允许精确控制CSS！\n";
            advice << "   - 可以嵌入原生CSS代码\n";
            advice << "   - 可以使用CSS预处理器代码\n";
            advice << "   - 适合复杂的样式定义\n";
            break;
            
        case ContextType::CHTL_SCRIPT:
            advice << "💡 在脚本块中使用Origin提供JavaScript灵活性！\n";
            advice << "   - 可以嵌入原生JavaScript代码\n";
            advice << "   - 可以使用第三方库代码\n";
            advice << "   - 适合复杂的逻辑实现\n";
            break;
            
        case ContextType::ORIGIN_BLOCK:
            advice << "💡 在Origin块内部再次使用Origin是完全可行的！\n";
            advice << "   - 支持嵌套原始嵌入\n";
            advice << "   - 可以创建复杂的代码结构\n";
            advice << "   - 开发者需要确保语法正确性\n";
            break;
            
        default:
            advice << "💡 Origin可以在任意位置使用！\n";
            advice << "   - CHTL设计理念支持最大化的灵活性\n";
            advice << "   - 开发者拥有完全的控制权\n";
            break;
    }
    
    // 语义合理性检查（仅建议，不阻止）
    auto [isReasonable, semanticAdvice] = validateOriginSemantics(position, originType);
    if (!isReasonable) {
        advice << "\n⚠️  语义提醒：" << semanticAdvice << "\n";
        advice << "   （这不会阻止使用，只是提供最佳实践建议）\n";
    }
    
    advice << "\n🎯 记住：问题由开发者自己控制！";
    
    return advice.str();
}

std::pair<bool, std::string> EnhancedOriginManager::validateOriginSemantics(
    const OriginPosition& position, OriginType originType) const {
    
    bool isReasonable = isSemanticallySensible(position.context, originType);
    std::string suggestion;
    
    if (!isReasonable) {
        suggestion = "在" + getContextName(position.context) + "中使用" + 
                    getOriginTypeName(originType) + "可能不太常见。";
        
        // 提供具体的语义建议
        switch (originType) {
            case OriginType::HTML:
                if (position.context == ContextType::CSS_BLOCK) {
                    suggestion += "在CSS块中嵌入HTML可能会导致语法错误。";
                } else if (position.context == ContextType::JAVASCRIPT_BLOCK) {
                    suggestion += "在JS块中嵌入HTML需要使用字符串形式。";
                }
                break;
                
            case OriginType::STYLE:
                if (position.context == ContextType::HTML_ELEMENT) {
                    suggestion += "建议使用style属性或<style>标签。";
                }
                break;
                
            case OriginType::JAVASCRIPT:
                if (position.context == ContextType::CSS_BLOCK) {
                    suggestion += "在CSS中嵌入JS可能需要特殊处理。";
                }
                break;
                
            default:
                suggestion += "请确认这符合您的预期。";
                break;
        }
        
        suggestion += "（Origin仍然可以使用，这只是最佳实践建议）";
    } else {
        suggestion = "在" + getContextName(position.context) + "中使用" + 
                    getOriginTypeName(originType) + "是合理的选择。";
    }
    
    return {isReasonable, suggestion};
}

std::vector<std::string> EnhancedOriginManager::getBestPractices(ContextType context) const {
    std::vector<std::string> practices;
    
    switch (context) {
        case ContextType::CHTL_ROOT:
            practices = {
                "在根级别定义全局[Origin] @Style用于全局样式",
                "使用[Origin] @JavaScript进行全局脚本初始化",
                "利用[Origin] @Html定义可重用的HTML模板",
                "创建[Origin] @CustomType用于自定义类型定义"
            };
            break;
            
        case ContextType::CHTL_ELEMENT:
            practices = {
                "使用[Origin] @Html嵌入复杂的HTML结构",
                "通过[Origin] @Style添加元素特定样式",
                "利用[Origin] @JavaScript实现交互逻辑",
                "结合多种Origin类型创建完整组件"
            };
            break;
            
        case ContextType::CHTL_STYLE:
            practices = {
                "使用[Origin] @Style嵌入原生CSS代码",
                "通过[Origin] @CustomCSS使用预处理器语法",
                "利用Origin实现CSS-in-JS模式",
                "嵌入第三方CSS库代码"
            };
            break;
            
        case ContextType::CHTL_SCRIPT:
            practices = {
                "使用[Origin] @JavaScript嵌入原生JS代码",
                "通过[Origin] @TypeScript使用TypeScript",
                "利用Origin嵌入第三方库代码",
                "实现复杂的业务逻辑"
            };
            break;
            
        default:
            practices = {
                "Origin可以在任意位置使用",
                "开发者拥有完全的控制权",
                "根据实际需求选择合适的Origin类型",
                "确保代码的可读性和维护性"
            };
            break;
    }
    
    return practices;
}

void EnhancedOriginManager::registerCustomOriginType(const std::string& typeName,
                                                    const std::string& description,
                                                    const std::vector<ContextType>& compatibleContexts) {
    customOriginTypes_[typeName] = description;
    customTypeContexts_[typeName] = compatibleContexts;
    
    std::cout << "✨ 注册自定义Origin类型：" << typeName << " - " << description << std::endl;
}

std::unordered_map<std::string, std::string> EnhancedOriginManager::getSupportedOriginTypes() const {
    std::unordered_map<std::string, std::string> types;
    
    // 官方预设的三种基本类型（根据CHTL语法文档）
    types["@Html"] = "HTML代码嵌入（官方预设）";
    types["@Style"] = "CSS样式嵌入（官方预设）";
    types["@JavaScript"] = "JavaScript代码嵌入（官方预设）";
    
    // 添加开发者自定义类型
    for (const auto& [typeName, description] : customOriginTypes_) {
        types[typeName] = description;
    }
    
    return types;
}

std::string EnhancedOriginManager::getContextName(ContextType context) const {
    switch (context) {
        case ContextType::CHTL_ROOT: return "CHTL根上下文";
        case ContextType::CHTL_ELEMENT: return "CHTL元素内部";
        case ContextType::CHTL_STYLE: return "CHTL样式块";
        case ContextType::CHTL_SCRIPT: return "CHTL脚本块";
        case ContextType::HTML_ELEMENT: return "HTML元素";
        case ContextType::CSS_BLOCK: return "CSS代码块";
        case ContextType::JAVASCRIPT_BLOCK: return "JavaScript代码块";
        case ContextType::CHTL_JS_BLOCK: return "CHTL JS代码块";
        case ContextType::CUSTOM_BLOCK: return "自定义代码块";
        case ContextType::TEMPLATE_BLOCK: return "模板块";
        case ContextType::CONFIGURATION_BLOCK: return "配置块";
        case ContextType::IMPORT_BLOCK: return "导入块";
        case ContextType::NAMESPACE_BLOCK: return "命名空间块";
        case ContextType::ORIGIN_BLOCK: return "Origin原始嵌入块";
        case ContextType::MIXED_CONTEXT: return "混合上下文";
        default: return "未知上下文";
    }
}

std::string EnhancedOriginManager::getOriginTypeName(OriginType type) const {
    switch (type) {
        case OriginType::HTML: return "@Html";
        case OriginType::STYLE: return "@Style";
        case OriginType::JAVASCRIPT: return "@JavaScript";
        case OriginType::VUE: return "@Vue";
        case OriginType::REACT: return "@React";
        case OriginType::SVELTE: return "@Svelte";
        case OriginType::MARKDOWN: return "@Markdown";
        case OriginType::XML: return "@XML";
        case OriginType::JSON: return "@JSON";
        case OriginType::YAML: return "@YAML";
        default: return "@开发者自定义类型";
    }
}

bool EnhancedOriginManager::isSemanticallySensible(ContextType context, OriginType originType) const {
    // 定义语义上比较合理的组合
    switch (context) {
        case ContextType::CHTL_ROOT:
        case ContextType::CHTL_ELEMENT:
        case ContextType::ORIGIN_BLOCK:
            return true; // 在这些上下文中，任何Origin类型都是合理的
            
        case ContextType::CHTL_STYLE:
        case ContextType::CSS_BLOCK:
            return originType == OriginType::STYLE || originType == OriginType::CUSTOM_TYPE;
            
        case ContextType::CHTL_SCRIPT:
        case ContextType::JAVASCRIPT_BLOCK:
        case ContextType::CHTL_JS_BLOCK:
            return originType == OriginType::JAVASCRIPT || 
                   originType == OriginType::VUE || 
                   originType == OriginType::REACT ||
                   originType == OriginType::CUSTOM_TYPE;
            
        case ContextType::HTML_ELEMENT:
            return originType == OriginType::HTML || originType == OriginType::CUSTOM_TYPE;
            
        default:
            return true; // 默认认为合理，因为Origin可以在任意位置使用
    }
}

// ============ EnhancedContextualCommentGenerator 实现 ============

EnhancedContextualCommentGenerator::EnhancedContextualCommentGenerator() {
    std::cout << "🚀 初始化增强的上下文注释生成器" << std::endl;
    std::cout << "核心理念：\"--\"注释会根据上下文生成不同编程语言的注释" << std::endl;
}

ContextType EnhancedContextualCommentGenerator::detectCurrentContext(
    const std::string& surroundingCode, size_t cursorPosition) const {
    
    std::cout << "🔍 检测上下文，光标位置: " << cursorPosition << std::endl;
    
    // 获取光标前后的代码片段
    std::string beforeCursor = surroundingCode.substr(0, cursorPosition);
    std::string afterCursor = surroundingCode.substr(cursorPosition);
    
    // 检查是否在Origin块内部
    if (isInsideOriginBlock(surroundingCode, cursorPosition)) {
        std::cout << "📍 检测到：Origin块内部" << std::endl;
        
        // 进一步检测Origin类型
        std::regex originTypeRegex(R"(\[Origin\]\s*@(\w+))");
        std::smatch match;
        if (std::regex_search(beforeCursor, match, originTypeRegex)) {
            std::string originType = match[1].str();
            if (originType == "Html") return ContextType::HTML_ELEMENT;
            if (originType == "Style") return ContextType::CSS_BLOCK;
            if (originType == "JavaScript") return ContextType::JAVASCRIPT_BLOCK;
        }
        return ContextType::ORIGIN_BLOCK;
    }
    
    // 检查是否在CHTL样式块内部
    if (isInsideStyleBlock(surroundingCode, cursorPosition)) {
        std::cout << "📍 检测到：CHTL样式块" << std::endl;
        return ContextType::CHTL_STYLE;
    }
    
    // 检查是否在CHTL脚本块内部
    if (isInsideScriptBlock(surroundingCode, cursorPosition)) {
        std::cout << "📍 检测到：CHTL脚本块" << std::endl;
        return ContextType::CHTL_SCRIPT;
    }
    
    // 检查是否在CHTL JS块内部
    if (isInsideCHTLJSBlock(surroundingCode, cursorPosition)) {
        std::cout << "📍 检测到：CHTL JS块" << std::endl;
        return ContextType::CHTL_JS_BLOCK;
    }
    
    // 检查是否在HTML元素内部
    if (isInsideHTMLElement(surroundingCode, cursorPosition)) {
        std::cout << "📍 检测到：HTML元素" << std::endl;
        return ContextType::HTML_ELEMENT;
    }
    
    // 检查特殊的CHTL结构
    std::regex templateRegex(R"(\[Template\])");
    if (std::regex_search(beforeCursor, templateRegex)) {
        std::cout << "📍 检测到：模板块" << std::endl;
        return ContextType::TEMPLATE_BLOCK;
    }
    
    std::regex configRegex(R"(\[Configuration\])");
    if (std::regex_search(beforeCursor, configRegex)) {
        std::cout << "📍 检测到：配置块" << std::endl;
        return ContextType::CONFIGURATION_BLOCK;
    }
    
    std::regex importRegex(R"(\[Import\])");
    if (std::regex_search(beforeCursor, importRegex)) {
        std::cout << "📍 检测到：导入块" << std::endl;
        return ContextType::IMPORT_BLOCK;
    }
    
    std::regex namespaceRegex(R"(\[Namespace\])");
    if (std::regex_search(beforeCursor, namespaceRegex)) {
        std::cout << "📍 检测到：命名空间块" << std::endl;
        return ContextType::NAMESPACE_BLOCK;
    }
    
    // 检查是否在CHTL元素内部
    std::regex elementRegex(R"(\b(\w+)\s*\{[^}]*$)");
    if (std::regex_search(beforeCursor, elementRegex)) {
        std::cout << "📍 检测到：CHTL元素内部" << std::endl;
        return ContextType::CHTL_ELEMENT;
    }
    
    // 默认为CHTL根上下文
    std::cout << "📍 检测到：CHTL根上下文" << std::endl;
    return ContextType::CHTL_ROOT;
}

std::string EnhancedContextualCommentGenerator::processDashComment(
    const std::string& commentContent, ContextType context, bool isMultiLine) const {
    
    std::cout << "💬 处理\"--\"注释（CHTL原生语法），上下文: " << static_cast<int>(context) 
              << ", 多行: " << (isMultiLine ? "是" : "否") << std::endl;
    
    switch (context) {
        case ContextType::CSS_BLOCK:
        case ContextType::CHTL_STYLE:
            return generateCSSComment(commentContent, isMultiLine);
            
        case ContextType::JAVASCRIPT_BLOCK:
            return generateJavaScriptComment(commentContent, isMultiLine);
            
        case ContextType::CHTL_JS_BLOCK:
        case ContextType::CHTL_SCRIPT:
            return generateCHTLJSComment(commentContent, isMultiLine);
            
        // "--"是CHTL的原生语法，在CHTL上下文中应该转换为HTML注释
        case ContextType::CHTL_ROOT:
        case ContextType::CHTL_ELEMENT:
        case ContextType::HTML_ELEMENT:
        case ContextType::ORIGIN_BLOCK:
        case ContextType::TEMPLATE_BLOCK:
        case ContextType::CONFIGURATION_BLOCK:
        case ContextType::IMPORT_BLOCK:
        case ContextType::NAMESPACE_BLOCK:
            return generateHTMLComment(commentContent, isMultiLine);
            
        case ContextType::CUSTOM_BLOCK:
            return generateCustomComment(commentContent, context, isMultiLine);
            
        default:
            // 默认情况下，"--"作为CHTL原生语法，转换为HTML注释
            return generateHTMLComment(commentContent, isMultiLine);
    }
}

std::string EnhancedContextualCommentGenerator::smartProcessDashComment(
    const std::string& commentContent, const std::string& surroundingCode, 
    size_t cursorPosition) const {
    
    ContextType context = detectCurrentContext(surroundingCode, cursorPosition);
    bool isMultiLine = commentContent.find('\n') != std::string::npos;
    
    return processDashComment(commentContent, context, isMultiLine);
}

std::string EnhancedContextualCommentGenerator::getCommentExample(ContextType context) const {
    switch (context) {
        case ContextType::CSS_BLOCK:
        case ContextType::CHTL_STYLE:
            return "/* 这是CSS注释 */";
        case ContextType::JAVASCRIPT_BLOCK:
            return "// 这是JavaScript注释";
        case ContextType::CHTL_JS_BLOCK:
            return "// 这是CHTL JS注释";
        // "--"是CHTL原生语法，默认转换为HTML注释
        default:
            return "<!-- 这是HTML注释（CHTL \"--\"语法的默认转换） -->";
    }
}

std::unordered_map<std::string, std::pair<std::string, std::string>> 
EnhancedContextualCommentGenerator::getSupportedCommentFormats() const {
    return {
        {"CSS", {"/* {content} */", "/* {content} */"}},
        {"JavaScript", {"// {content}", "/* {content} */"}},
        {"CHTL JS", {"// {content}", "/* {content} */"}},
        {"CHTL (默认HTML)", {"<!-- {content} -->", "<!-- {content} -->"}}
    };
}

std::string EnhancedContextualCommentGenerator::generateHTMLComment(
    const std::string& content, bool isMultiLine) const {
    return "<!-- " + content + " -->";
}

std::string EnhancedContextualCommentGenerator::generateCSSComment(
    const std::string& content, bool isMultiLine) const {
    return "/* " + content + " */";
}

std::string EnhancedContextualCommentGenerator::generateJavaScriptComment(
    const std::string& content, bool isMultiLine) const {
    if (isMultiLine) {
        return "/* " + content + " */";
    } else {
        return "// " + content;
    }
}

std::string EnhancedContextualCommentGenerator::generateCHTLComment(
    const std::string& content, bool isMultiLine) const {
    if (isMultiLine) {
        return "/* " + content + " */";
    } else {
        return "// " + content;
    }
}

std::string EnhancedContextualCommentGenerator::generateCHTLJSComment(
    const std::string& content, bool isMultiLine) const {
    return generateJavaScriptComment(content, isMultiLine);
}

std::string EnhancedContextualCommentGenerator::generateCustomComment(
    const std::string& content, ContextType context, bool isMultiLine) const {
    // 对于自定义上下文，使用默认的CHTL注释格式
    return generateCHTLComment(content, isMultiLine);
}

bool EnhancedContextualCommentGenerator::isInsideOriginBlock(
    const std::string& surroundingCode, size_t position) const {
    
    std::string beforeCursor = surroundingCode.substr(0, position);
    
    // 查找最近的[Origin]标记
    std::regex originStartRegex(R"(\[Origin\]\s*@\w+[^{]*\{)");
    std::regex originEndRegex(R"(\})");
    
    std::sregex_iterator originStart(beforeCursor.begin(), beforeCursor.end(), originStartRegex);
    std::sregex_iterator originEnd(beforeCursor.begin(), beforeCursor.end(), originEndRegex);
    std::sregex_iterator end;
    
    int originStartCount = std::distance(originStart, end);
    int originEndCount = std::distance(originEnd, end);
    
    return originStartCount > originEndCount;
}

bool EnhancedContextualCommentGenerator::isInsideStyleBlock(
    const std::string& surroundingCode, size_t position) const {
    
    std::string beforeCursor = surroundingCode.substr(0, position);
    
    std::regex styleStartRegex(R"(\bstyle\s*\{)");
    std::regex styleEndRegex(R"(\})");
    
    std::sregex_iterator styleStart(beforeCursor.begin(), beforeCursor.end(), styleStartRegex);
    std::sregex_iterator styleEnd(beforeCursor.begin(), beforeCursor.end(), styleEndRegex);
    std::sregex_iterator end;
    
    int styleStartCount = std::distance(styleStart, end);
    int styleEndCount = std::distance(styleEnd, end);
    
    return styleStartCount > styleEndCount;
}

bool EnhancedContextualCommentGenerator::isInsideScriptBlock(
    const std::string& surroundingCode, size_t position) const {
    
    std::string beforeCursor = surroundingCode.substr(0, position);
    
    std::regex scriptStartRegex(R"(\bscript\s*\{)");
    std::regex scriptEndRegex(R"(\})");
    
    std::sregex_iterator scriptStart(beforeCursor.begin(), beforeCursor.end(), scriptStartRegex);
    std::sregex_iterator scriptEnd(beforeCursor.begin(), beforeCursor.end(), scriptEndRegex);
    std::sregex_iterator end;
    
    int scriptStartCount = std::distance(scriptStart, end);
    int scriptEndCount = std::distance(scriptEnd, end);
    
    return scriptStartCount > scriptEndCount;
}

bool EnhancedContextualCommentGenerator::isInsideHTMLElement(
    const std::string& surroundingCode, size_t position) const {
    
    std::string beforeCursor = surroundingCode.substr(0, position);
    
    // 检查是否在HTML标签内部
    std::regex htmlTagRegex(R"(<[^>]*$)");
    return std::regex_search(beforeCursor, htmlTagRegex);
}

bool EnhancedContextualCommentGenerator::isInsideCHTLJSBlock(
    const std::string& surroundingCode, size_t position) const {
    
    std::string beforeCursor = surroundingCode.substr(0, position);
    
    // 检查是否在CHTL JS特有的语法中
    std::regex chtlJSRegex(R"((\{\{[^}]*|\{\&\}|printMylove|iNeverAway|vir\s+\w+|listen\s*\(|delegate\s*\())");
    return std::regex_search(beforeCursor, chtlJSRegex);
}

// ============ EnhancedFeatureManager 实现 ============

EnhancedFeatureManager::EnhancedFeatureManager() {
    originManager_ = std::make_unique<EnhancedOriginManager>();
    commentGenerator_ = std::make_unique<EnhancedContextualCommentGenerator>();
    
    std::cout << "🎯 增强功能管理器初始化完成" << std::endl;
}

std::string EnhancedFeatureManager::processCode(const std::string& code) const {
    std::cout << "\n🔄 处理CHTL代码，长度: " << code.length() << std::endl;
    
    // 1. 转换"--"注释
    std::string processedCode = transformDashComments(code);
    
    // 2. 验证Origin使用（不阻止，只提供建议）
    auto validationReport = validateOriginUsage(processedCode);
    
    if (!validationReport.empty()) {
        std::cout << "\n📋 Origin使用验证报告:" << std::endl;
        for (const auto& report : validationReport) {
            std::cout << "  " << report << std::endl;
        }
    }
    
    return processedCode;
}

std::vector<std::string> EnhancedFeatureManager::validateOriginUsage(const std::string& code) const {
    std::vector<std::string> reports;
    
    std::regex originRegex(R"(\[Origin\]\s*@(\w+))");
    std::sregex_iterator iter(code.begin(), code.end(), originRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string originType = iter->str(1);
        size_t position = iter->position();
        
        // 创建位置信息
        OriginPosition pos;
        pos.line = std::count(code.begin(), code.begin() + position, '\n') + 1;
        pos.column = position - code.rfind('\n', position);
        pos.context = commentGenerator_->detectCurrentContext(code, position);
        
        // 确定Origin类型
        OriginType type = OriginType::CUSTOM_TYPE;
        if (originType == "Html") type = OriginType::HTML;
        else if (originType == "Style") type = OriginType::STYLE;
        else if (originType == "JavaScript") type = OriginType::JAVASCRIPT;
        else if (originType == "Vue") type = OriginType::VUE;
        else if (originType == "React") type = OriginType::REACT;
        
        // 获取使用建议
        std::string advice = originManager_->getOriginUsageAdvice(pos, type);
        reports.push_back("行 " + std::to_string(pos.line) + ": " + originType + " - " + advice);
    }
    
    return reports;
}

std::string EnhancedFeatureManager::transformDashComments(const std::string& code) const {
    std::string result = code;
    
    std::regex dashCommentRegex(R"(--(.*?)(?=\n|$))");
    std::sregex_iterator iter(code.begin(), code.end(), dashCommentRegex);
    std::sregex_iterator end;
    
    // 从后往前替换，避免位置偏移
    std::vector<std::pair<size_t, std::pair<size_t, std::string>>> replacements;
    
    for (; iter != end; ++iter) {
        std::string commentContent = iter->str(1);
        size_t position = iter->position();
        size_t length = iter->length();
        
        // 检测上下文并生成对应的注释
        std::string transformedComment = commentGenerator_->smartProcessDashComment(
            commentContent, code, position
        );
        
        replacements.emplace_back(position, std::make_pair(length, transformedComment));
    }
    
    // 从后往前替换
    std::sort(replacements.begin(), replacements.end(), std::greater<>());
    for (const auto& [pos, replacement] : replacements) {
        result.replace(pos, replacement.first, replacement.second);
    }
    
    return result;
}

// 全局实例
static std::unique_ptr<EnhancedFeatureManager> globalManager = nullptr;

EnhancedFeatureManager& getEnhancedFeatureManager() {
    if (!globalManager) {
        globalManager = std::make_unique<EnhancedFeatureManager>();
    }
    return *globalManager;
}

} // namespace Enhanced
} // namespace CHTL