#include "compilers/chtl/chtl_compiler.h"
#include "compilers/chtl/chtl_parser.h"
#include "compilers/chtl/chtl_code_generator.h"
#include "compilers/chtl/chtl_ast.h"
#include <sstream>
#include <regex>

namespace chtl {

CHTLCompiler::CHTLCompiler() 
    : parser_(std::make_unique<CHTLParser>()),
      codeGenerator_(std::make_unique<CHTLCodeGenerator>()) {
}

CHTLCompiler::~CHTLCompiler() = default;

std::string CHTLCompiler::compile(const std::string& source) {
    // 清理之前的错误和警告
    errors_.clear();
    warnings_.clear();
    
    // 解析源代码
    auto ast = parser_->parse(source);
    
    // 收集解析错误
    for (const auto& error : parser_->getErrors()) {
        errors_.push_back(error);
    }
    
    if (!ast || !errors_.empty()) {
        return "";
    }
    
    // 生成HTML代码
    std::string html = codeGenerator_->generate(ast);
    
    return html;
}

std::pair<std::string, std::string> CHTLCompiler::compileLocalStyle(
    const std::string& styleContent, 
    const std::string& elementId) {
    
    std::string processedStyle;
    std::vector<std::string> generatedClasses;
    
    // 处理局部样式中的选择器
    processLocalStyleSelectors(styleContent, processedStyle, generatedClasses);
    
    // 生成唯一的类名
    std::string mainClassName = generateUniqueClassName();
    generatedClasses.push_back(mainClassName);
    
    // 返回处理后的样式和主类名
    return {processedStyle, mainClassName};
}

void CHTLCompiler::reset() {
    errors_.clear();
    warnings_.clear();
    templates_.clear();
    customElements_.clear();
    parser_->reset();
}

void CHTLCompiler::setOption(const std::string& key, const std::string& value) {
    options_[key] = value;
}

void CHTLCompiler::registerTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> node) {
    templates_[name] = node;
}

void CHTLCompiler::registerCustomElement(const std::string& name, std::shared_ptr<CHTLASTNode> node) {
    customElements_[name] = node;
}

std::string CHTLCompiler::generateUniqueClassName() {
    static int counter = 0;
    return "chtl-" + std::to_string(++counter);
}

void CHTLCompiler::processLocalStyleSelectors(
    const std::string& style, 
    std::string& processedStyle, 
    std::vector<std::string>& generatedClasses) {
    
    std::stringstream result;
    std::regex classSelector(R"(\.(\w+)\s*\{)");
    std::regex idSelector(R"(#(\w+)\s*\{)");
    std::regex ampersandSelector(R"(&([\s:]+))");
    
    std::string workingStyle = style;
    
    // 处理类选择器 - 自动添加类名
    std::smatch match;
    while (std::regex_search(workingStyle, match, classSelector)) {
        result << match.prefix();
        
        std::string className = match[1].str();
        generatedClasses.push_back(className);
        
        result << "." << className << " {";
        workingStyle = match.suffix();
    }
    result << workingStyle;
    
    workingStyle = result.str();
    result.str("");
    
    // 处理ID选择器
    while (std::regex_search(workingStyle, match, idSelector)) {
        result << match.prefix();
        
        std::string idName = match[1].str();
        generatedClasses.push_back(idName);
        
        result << "#" << idName << " {";
        workingStyle = match.suffix();
    }
    result << workingStyle;
    
    workingStyle = result.str();
    result.str("");
    
    // 处理 & 符号（上下文推导）
    if (!generatedClasses.empty()) {
        std::string mainClass = "." + generatedClasses[0];
        while (std::regex_search(workingStyle, match, ampersandSelector)) {
            result << match.prefix();
            result << mainClass << match[1].str();
            workingStyle = match.suffix();
        }
        result << workingStyle;
        processedStyle = result.str();
    } else {
        processedStyle = workingStyle;
    }
    
    // 处理内联样式（直接在style块中的属性）
    std::regex inlineStyle(R"(^\s*([a-zA-Z-]+)\s*:\s*([^;]+);\s*$)", std::regex::multiline);
    std::stringstream inlineStyles;
    std::string remaining = processedStyle;
    
    auto begin = std::sregex_iterator(processedStyle.begin(), processedStyle.end(), inlineStyle);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        inlineStyles << match[1].str() << ": " << match[2].str() << "; ";
    }
    
    // 如果有内联样式，需要特殊处理
    if (!inlineStyles.str().empty()) {
        // 这些内联样式会被添加到元素的style属性中
        // 这里暂时只返回非内联的样式
        processedStyle = std::regex_replace(processedStyle, inlineStyle, "");
    }
}

} // namespace chtl