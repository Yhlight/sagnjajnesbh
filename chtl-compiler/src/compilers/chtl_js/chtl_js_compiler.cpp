#include "compilers/chtl_js/chtl_js_compiler.h"
#include "compilers/chtl_js/chtl_js_parser.h"
#include "compilers/chtl_js/chtl_js_code_generator.h"
#include "compilers/chtl_js/chtl_js_ast.h"

namespace chtl {

CHTLJSCompiler::CHTLJSCompiler() 
    : parser_(std::make_unique<CHTLJSParser>()),
      codeGenerator_(std::make_unique<CHTLJSCodeGenerator>()) {
}

CHTLJSCompiler::~CHTLJSCompiler() = default;

std::string CHTLJSCompiler::compile(const std::string& source) {
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
    
    // 传递虚对象注册表到代码生成器
    for (const auto& [name, type] : virtualObjects_) {
        codeGenerator_->registerVirtualObject(name, type);
    }
    
    // 生成JavaScript代码
    std::string js = codeGenerator_->generate(ast);
    
    return js;
}

void CHTLJSCompiler::reset() {
    errors_.clear();
    warnings_.clear();
    virtualObjects_.clear();
    selectorCache_.clear();
    parser_->reset();
}

void CHTLJSCompiler::setOption(const std::string& key, const std::string& value) {
    options_[key] = value;
}

void CHTLJSCompiler::registerVirtualObject(const std::string& name, const std::string& type) {
    virtualObjects_[name] = type;
}

std::string CHTLJSCompiler::processEnhancedSelector(const std::string& selector) {
    // 检查缓存
    auto it = selectorCache_.find(selector);
    if (it != selectorCache_.end()) {
        return it->second;
    }
    
    std::string result;
    
    // 移除 {{ 和 }}
    std::string cleanSelector = selector.substr(2, selector.length() - 4);
    
    // 解析选择器类型
    if (cleanSelector[0] == '.') {
        // 类选择器
        result = "document.querySelectorAll('" + cleanSelector + "')";
    } else if (cleanSelector[0] == '#') {
        // ID选择器
        result = "document.querySelector('" + cleanSelector + "')";
    } else if (cleanSelector.find('[') != std::string::npos) {
        // 索引访问
        size_t bracketPos = cleanSelector.find('[');
        std::string tag = cleanSelector.substr(0, bracketPos);
        std::string index = cleanSelector.substr(bracketPos + 1, cleanSelector.find(']') - bracketPos - 1);
        result = "document.getElementsByTagName('" + tag + "')[" + index + "]";
    } else {
        // 标签选择器或其他
        // 首先检查是否是类名或ID（不带前缀）
        result = "(function() {";
        result += "var el = document.getElementById('" + cleanSelector + "');";
        result += "if (el) return el;";
        result += "var els = document.getElementsByClassName('" + cleanSelector + "');";
        result += "if (els.length > 0) return els[0];";
        result += "return document.getElementsByTagName('" + cleanSelector + "');";
        result += "})()";
    }
    
    // 缓存结果
    selectorCache_[selector] = result;
    return result;
}

std::string CHTLJSCompiler::processArrowOperator(const std::string& code) {
    // 将 -> 替换为 .
    std::string result = code;
    size_t pos = 0;
    while ((pos = result.find("->", pos)) != std::string::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    return result;
}

std::string CHTLJSCompiler::processVirtualObject(const std::string& virName, const std::string& method) {
    auto it = virtualObjects_.find(virName);
    if (it == virtualObjects_.end()) {
        errors_.push_back("Undefined virtual object: " + virName);
        return "";
    }
    
    // 生成函数名
    return generateFunctionName(virName + "_" + method);
}

std::string CHTLJSCompiler::generateFunctionName(const std::string& prefix) {
    static int counter = 0;
    return "_chtl_" + prefix + "_" + std::to_string(++counter);
}

void CHTLJSCompiler::generateEventDelegation(const std::string& parent, 
                                           const std::string& target, 
                                           const std::string& events) {
    // 事件委托的实现将在代码生成器中完成
}

} // namespace chtl