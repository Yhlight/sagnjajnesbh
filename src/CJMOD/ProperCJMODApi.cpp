#include "CJMOD/ProperCJMODApi.h"
#include "Utils/StringUtils.h"
#include <iostream>
#include <regex>
#include <sstream>

namespace CHTL {
namespace CJMOD {

// ============ 全局注册表实现 ============
std::unordered_map<std::string, std::string> g_virtualObjectRegistry;
std::unordered_map<std::string, std::string> g_globalFunctionRegistry;
std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

// ============ Arg类实现 ============

Arg::Arg() : name_(""), value_(""), type_("unknown") {}

Arg::Arg(const std::string& name) : name_(name), value_(""), type_("unknown") {}

void Arg::extractFromFragment(const Scanner::CodeFragment& fragment, int offset) {
    // 使用peekKeyword技术从CodeFragment中提取参数值
    value_ = peekKeyword(fragment, offset);
    if (!value_.empty()) {
        analyzeType(value_);
    }
}

std::string Arg::peekKeyword(const Scanner::CodeFragment& fragment, int offset) {
    // 核心peekKeyword实现 - 从CodeFragment中按偏移提取参数
    const std::string& content = fragment.content;
    
    // 查找关键字位置
    size_t keywordPos = content.find(fragment.content.substr(fragment.startPos, 
                                    fragment.endPos - fragment.startPos));
    
    if (keywordPos == std::string::npos) {
        return "";
    }
    
    // 根据offset方向查找参数
    if (offset < 0) {
        // 向前查找（offset为负数）
        return extractBackwardParameter(content, keywordPos, -offset);
    } else if (offset > 0) {
        // 向后查找（offset为正数）
        return extractForwardParameter(content, keywordPos, offset);
    }
    
    return "";
}

std::string Arg::extractBackwardParameter(const std::string& content, size_t pos, int steps) {
    // 向前提取参数（双指针扫描的前置截取机制）
    std::regex paramPattern(R"((\w+|\d+\.?\d*|"[^"]*"|'[^']*'|\{[^}]*\}))");
    std::sregex_iterator iter(content.begin(), content.begin() + pos, paramPattern);
    std::sregex_iterator end;
    
    std::vector<std::string> params;
    for (; iter != end; ++iter) {
        params.push_back(iter->str());
    }
    
    if (steps <= params.size()) {
        return params[params.size() - steps];
    }
    
    return "";
}

std::string Arg::extractForwardParameter(const std::string& content, size_t pos, int steps) {
    // 向后提取参数
    std::regex paramPattern(R"((\w+|\d+\.?\d*|"[^"]*"|'[^']*'|\{[^}]*\}))");
    std::sregex_iterator iter(content.begin() + pos, content.end(), paramPattern);
    std::sregex_iterator end;
    
    int count = 0;
    for (; iter != end; ++iter) {
        count++;
        if (count == steps) {
            return iter->str();
        }
    }
    
    return "";
}

void Arg::analyzeType(const std::string& value) {
    // 分析参数类型
    if (value.empty()) {
        type_ = "unknown";
    } else if (value.front() == '"' && value.back() == '"') {
        type_ = "string";
    } else if (value.front() == '\'' && value.back() == '\'') {
        type_ = "string";
    } else if (std::regex_match(value, std::regex(R"(\d+\.?\d*)"))) {
        type_ = "number";
    } else if (value.front() == '{' && value.back() == '}') {
        type_ = "object";
    } else if (value == "function" || value.find("function") != std::string::npos) {
        type_ = "function";
    } else {
        type_ = "identifier";
    }
}

// ============ Syntax类实现 ============

Syntax::Syntax() : pattern_("") {}

Syntax::Syntax(const std::string& pattern) : pattern_(pattern) {}

void Syntax::analyzeFromFragment(const Scanner::CodeFragment& fragment) {
    // 基于CodeFragment进行语法分析（不重新做词法分析）
    parseArgsFromFragment(fragment);
}

void Syntax::parseArgsFromFragment(const Scanner::CodeFragment& fragment) {
    // 从CodeFragment中解析参数
    args_.clear();
    
    // 使用正则表达式提取函数调用的参数
    std::regex functionCallPattern(R"((\w+)\s*\(\s*([^)]*)\s*\))");
    std::smatch match;
    
    if (std::regex_search(fragment.content, match, functionCallPattern)) {
        std::string functionName = match[1].str();
        std::string argsString = match[2].str();
        
        // 解析参数列表
        if (!argsString.empty()) {
            std::regex argPattern(R"((\w+)\s*:\s*([^,}]+))");
            std::sregex_iterator iter(argsString.begin(), argsString.end(), argPattern);
            std::sregex_iterator end;
            
            for (; iter != end; ++iter) {
                Arg arg(iter->str(1));
                arg.extractFromFragment(fragment, 0);
                args_.push_back(arg);
            }
        }
    }
}

void Syntax::bind(const std::string& paramName, std::function<std::string(const std::string&)> processor) {
    processors_[paramName] = processor;
}

std::string Syntax::transform(const std::string& targetTemplate) {
    std::string result = targetTemplate;
    
    // 替换模板中的参数占位符
    for (const auto& arg : args_) {
        std::string placeholder = "{" + arg.getName() + "}";
        std::string value = arg.getValue();
        
        // 应用处理器
        auto it = processors_.find(arg.getName());
        if (it != processors_.end()) {
            value = it->second(value);
        }
        
        // 替换占位符
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction() : functionName_(""), generatedCode_(""), processed_(false) {}

CHTLJSFunction::CHTLJSFunction(const std::string& functionName) 
    : functionName_(functionName), generatedCode_(""), processed_(false) {}

std::string CHTLJSFunction::processFragment(const Scanner::CodeFragment& fragment) {
    // 核心方法：处理来自统一扫描器的代码片段
    processWithUnifiedScanner(fragment);
    return generateCode();
}

void CHTLJSFunction::processWithUnifiedScanner(const Scanner::CodeFragment& fragment) {
    // 与统一扫描器协作的核心实现
    
    // 1. 自动进行语法分析
    syntax_ = syntaxAnalys(functionName_ + "({})");
    
    // 2. 从CodeFragment中分析语法
    syntax_->analyzeFromFragment(fragment);
    
    // 3. 应用已注册的处理器
    for (const auto& processor : processors_) {
        syntax_->bind(processor.first, processor.second);
    }
    
    processed_ = true;
}

void CHTLJSFunction::registerKeyword(const std::string& keyword, Scanner::CHTLUnifiedScanner& scanner) {
    // 注册关键字到统一扫描器
    scanner.RegisterKeyword(keyword);
    registeredKeywords_.push_back(keyword);
}

std::unique_ptr<Syntax> CHTLJSFunction::syntaxAnalys(const std::string& pattern) {
    return std::make_unique<Syntax>(pattern);
}

void CHTLJSFunction::bind(const std::string& paramName, std::function<std::string(const std::string&)> processor) {
    processors_[paramName] = processor;
}

std::string CHTLJSFunction::transform(const std::string& targetTemplate) {
    if (syntax_) {
        return syntax_->transform(targetTemplate);
    }
    return targetTemplate;
}

void CHTLJSFunction::scanKeyword(const std::string& keyword) {
    // 设置要扫描的关键字（统一扫描器会寻找这个关键字）
    registeredKeywords_.push_back(keyword);
}

void CHTLJSFunction::match(const Scanner::CodeFragment& fragment) {
    // 从CodeFragment中匹配参数值
    if (syntax_) {
        syntax_->analyzeFromFragment(fragment);
    }
}

std::string CHTLJSFunction::generateCode() {
    if (!processed_ || !syntax_) {
        return "";
    }
    
    // 生成标准JS代码
    std::ostringstream js;
    
    if (functionName_ == "printMylove") {
        // printMylove特殊处理
        js << "function printMylove(config) {\n";
        js << "    const canvas = document.createElement('canvas');\n";
        js << "    const ctx = canvas.getContext('2d');\n";
        js << "    const img = new Image();\n";
        js << "    \n";
        js << "    img.onload = function() {\n";
        js << "        canvas.width = img.width;\n";
        js << "        canvas.height = img.height;\n";
        js << "        ctx.drawImage(img, 0, 0);\n";
        js << "        \n";
        js << "        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);\n";
        js << "        const pixels = imageData.data;\n";
        js << "        \n";
        js << "        let result = '';\n";
        js << "        for (let y = 0; y < canvas.height; y += 2) {\n";
        js << "            for (let x = 0; x < canvas.width; x += 2) {\n";
        js << "                const i = (y * canvas.width + x) * 4;\n";
        js << "                const r = pixels[i];\n";
        js << "                const g = pixels[i + 1];\n";
        js << "                const b = pixels[i + 2];\n";
        js << "                const brightness = (r + g + b) / 3;\n";
        js << "                \n";
        js << "                if (config.mode === 'ASCII') {\n";
        js << "                    const chars = '@%#*+=-:. ';\n";
        js << "                    const charIndex = Math.floor(brightness / 255 * (chars.length - 1));\n";
        js << "                    result += chars[charIndex];\n";
        js << "                } else {\n";
        js << "                    result += brightness > 128 ? '█' : ' ';\n";
        js << "                }\n";
        js << "            }\n";
        js << "            result += '\\n';\n";
        js << "        }\n";
        js << "        \n";
        js << "        console.log(result);\n";
        js << "        return result;\n";
        js << "    };\n";
        js << "    \n";
        js << "    img.src = config.url;\n";
        js << "    return '';\n";
        js << "}\n";
    } else {
        // 通用JS函数生成
        js << "function " << functionName_ << "(";
        
        const auto& args = syntax_->getArgs();
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) js << ", ";
            js << args[i].getName();
        }
        
        js << ") {\n";
        js << "    // Generated by CJMOD\n";
        js << "    console.log('Calling " << functionName_ << "');\n";
        js << "}\n";
    }
    
    generatedCode_ = js.str();
    return generatedCode_;
}

void CHTLJSFunction::registerVirtualObject(const std::string& virName, const std::string& sourceFunction) {
    // 委托给CHTL JS的虚对象系统
    g_virtualObjectRegistry[virName] = sourceFunction;
}

std::string CHTLJSFunction::delegateVirtualCall(const std::string& virName, const std::string& methodName) {
    // 委托虚对象调用
    auto it = g_virtualObjectRegistry.find(virName);
    if (it != g_virtualObjectRegistry.end()) {
        return it->second + "_" + methodName + "()";
    }
    return "";
}

// ============ SimplifiedCHTLJSFunction类实现 ============

SimplifiedCHTLJSFunction::SimplifiedCHTLJSFunction(const std::string& functionName) 
    : CHTLJSFunction(functionName) {}

std::string SimplifiedCHTLJSFunction::autoProcess(const Scanner::CodeFragment& fragment, 
                                                 const std::string& pattern,
                                                 const std::unordered_map<std::string, std::function<std::string(const std::string&)>>& processors) {
    // 自动集成原始API的7步流程
    
    // 1. syntaxAnalys
    syntax_ = syntaxAnalys(pattern.empty() ? (getFunctionName() + "({})") : pattern);
    
    // 2. bind - 应用处理器
    for (const auto& processor : processors) {
        bind(processor.first, processor.second);
    }
    
    // 3. transform - 准备转换模板
    // 4. scanKeyword - 关键字已通过registerKeyword设置
    // 5. match - 从fragment中匹配参数
    match(fragment);
    
    // 6. result & 7. generateCode
    return generateCode();
}

// ============ 全局便利函数实现 ============

std::unique_ptr<CHTLJSFunction> createCJMODProcessor(const std::string& functionName) {
    return std::make_unique<CHTLJSFunction>(functionName);
}

std::unique_ptr<SimplifiedCHTLJSFunction> createCHTLJSFunction(const std::string& functionName) {
    return std::make_unique<SimplifiedCHTLJSFunction>(functionName);
}

void registerCJMODExtensions(Scanner::CHTLUnifiedScanner& scanner, const std::vector<std::string>& keywords) {
    for (const auto& keyword : keywords) {
        scanner.RegisterKeyword(keyword);
    }
}

std::string processCJMODFragment(const Scanner::CodeFragment& fragment, const std::string& functionName) {
    auto processor = createCJMODProcessor(functionName);
    return processor->processFragment(fragment);
}

// ============ 虚对象委托实现 ============

namespace VirtualObjectDelegate {
    void registerVirtualObject(const std::string& virName, const std::string& sourceFunction) {
        g_virtualObjectRegistry[virName] = sourceFunction;
    }
    
    std::string delegateVirtualCall(const std::string& virName, const std::string& methodName) {
        auto it = g_virtualObjectRegistry.find(virName);
        if (it != g_virtualObjectRegistry.end()) {
            return it->second + "_" + methodName + "()";
        }
        return "";
    }
    
    bool isVirtualObjectRegistered(const std::string& virName) {
        return g_virtualObjectRegistry.find(virName) != g_virtualObjectRegistry.end();
    }
    
    void clearVirtualObjectRegistry() {
        g_virtualObjectRegistry.clear();
    }
}

} // namespace CJMOD
} // namespace CHTL