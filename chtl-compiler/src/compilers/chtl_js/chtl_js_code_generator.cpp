#include "compilers/chtl_js/chtl_js_code_generator.h"
#include <regex>
#include <algorithm>

namespace chtl {

CHTLJSCodeGenerator::CHTLJSCodeGenerator() : functionCounter_(0) {
}

CHTLJSCodeGenerator::~CHTLJSCodeGenerator() = default;

std::string CHTLJSCodeGenerator::generate(std::shared_ptr<CHTLJSASTNode> ast) {
    output_.str("");
    functionCounter_ = 0;
    
    if (ast) {
        ast->accept(this);
    }
    
    // 添加事件委托代码
    std::string delegationCode = generateEventDelegationCode();
    if (!delegationCode.empty()) {
        output_ << "\n" << delegationCode;
    }
    
    return output_.str();
}

void CHTLJSCodeGenerator::visitProgram(CHTLJSProgramNode* node) {
    for (const auto& child : node->getChildren()) {
        child->accept(this);
    }
}

void CHTLJSCodeGenerator::visitEnhancedSelector(EnhancedSelectorNode* node) {
    std::string selector = node->getSelector();
    output_ << processEnhancedSelector(selector);
}

void CHTLJSCodeGenerator::visitVirtualObject(VirtualObjectNode* node) {
    // 虚对象定义
    std::string name = node->getName();
    std::string definition = node->getDefinition();
    
    // 解析定义中的函数
    std::regex funcRegex(R"((\w+)\s*:\s*function\s*\([^)]*\)\s*\{[^}]*\})");
    std::smatch match;
    std::string remaining = definition;
    
    while (std::regex_search(remaining, match, funcRegex)) {
        std::string funcName = match[1].str();
        std::string funcBody = match[0].str();
        
        // 生成全局函数
        std::string globalFuncName = generateFunctionName(name + "_" + funcName);
        output_ << "function " << globalFuncName << funcBody.substr(funcBody.find("function") + 8) << "\n";
        
        // 注册虚对象方法
        virtualObjects_[name + "->" + funcName] = globalFuncName;
        
        remaining = match.suffix();
    }
}

void CHTLJSCodeGenerator::visitArrowCall(ArrowCallNode* node) {
    std::string object = node->getObject();
    std::string method = node->getMethod();
    
    // 检查是否是虚对象调用
    if (virtualObjects_.find(object) != virtualObjects_.end()) {
        std::string funcName = processVirtualObjectMethod(object, method);
        output_ << funcName;
    } else {
        // 普通箭头调用，转换为点操作符
        output_ << object << "." << method;
    }
}

void CHTLJSCodeGenerator::visitListenCall(ListenCallNode* node) {
    std::string target = node->getTarget();
    std::string events = node->getEvents();
    
    output_ << "(function() {\n";
    output_ << "  var target = " << processEnhancedSelector(target) << ";\n";
    output_ << "  var events = " << events << ";\n";
    output_ << "  for (var event in events) {\n";
    output_ << "    if (events.hasOwnProperty(event)) {\n";
    output_ << "      if (target.forEach) {\n";
    output_ << "        target.forEach(function(el) {\n";
    output_ << "          el.addEventListener(event, events[event]);\n";
    output_ << "        });\n";
    output_ << "      } else {\n";
    output_ << "        target.addEventListener(event, events[event]);\n";
    output_ << "      }\n";
    output_ << "    }\n";
    output_ << "  }\n";
    output_ << "})()";
}

void CHTLJSCodeGenerator::visitDelegateCall(DelegateCallNode* node) {
    std::string parent = node->getParent();
    std::string config = node->getConfig();
    
    // 生成事件委托代码
    std::string funcName = generateFunctionName("delegate");
    
    output_ << "(function() {\n";
    output_ << "  var parent = " << processEnhancedSelector(parent) << ";\n";
    output_ << "  var config = " << config << ";\n";
    output_ << "  var targets = config.target;\n";
    output_ << "  if (!Array.isArray(targets)) targets = [targets];\n";
    output_ << "  \n";
    output_ << "  for (var event in config) {\n";
    output_ << "    if (event !== 'target' && config.hasOwnProperty(event)) {\n";
    output_ << "      parent.addEventListener(event, function(e) {\n";
    output_ << "        targets.forEach(function(selector) {\n";
    output_ << "          if (e.target.matches(selector)) {\n";
    output_ << "            config[event].call(e.target, e);\n";
    output_ << "          }\n";
    output_ << "        });\n";
    output_ << "      });\n";
    output_ << "    }\n";
    output_ << "  }\n";
    output_ << "})()";
}

void CHTLJSCodeGenerator::visitAnimateCall(AnimateCallNode* node) {
    std::string config = node->getConfig();
    
    output_ << generateAnimationCode(config);
}

void CHTLJSCodeGenerator::visitRegularJS(RegularJSNode* node) {
    output_ << node->getCode();
}

void CHTLJSCodeGenerator::registerVirtualObject(const std::string& name, const std::string& type) {
    virtualObjects_[name] = type;
}

std::string CHTLJSCodeGenerator::generateFunctionName(const std::string& prefix) {
    return "_chtl_" + prefix + "_" + std::to_string(++functionCounter_);
}

std::string CHTLJSCodeGenerator::translateSelector(const std::string& selector) {
    // 清理选择器
    std::string clean = selector;
    
    // 移除空白
    clean.erase(std::remove_if(clean.begin(), clean.end(), ::isspace), clean.end());
    
    // 处理索引访问
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch match;
    if (std::regex_match(clean, match, indexRegex)) {
        return "'" + match[1].str() + "[" + match[2].str() + "]'";
    }
    
    return "'" + clean + "'";
}

std::string CHTLJSCodeGenerator::processEnhancedSelector(const std::string& selector, bool returnAll) {
    // 移除 {{ 和 }}
    std::string cleanSelector = selector;
    if (cleanSelector.substr(0, 2) == "{{" && cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    // 解析选择器类型
    if (cleanSelector[0] == '.') {
        // 类选择器
        return "document.querySelectorAll('" + cleanSelector + "')";
    } else if (cleanSelector[0] == '#') {
        // ID选择器
        return "document.querySelector('" + cleanSelector + "')";
    } else if (cleanSelector.find('[') != std::string::npos) {
        // 索引访问
        size_t bracketPos = cleanSelector.find('[');
        std::string tag = cleanSelector.substr(0, bracketPos);
        std::string index = cleanSelector.substr(bracketPos + 1, cleanSelector.find(']') - bracketPos - 1);
        return "document.getElementsByTagName('" + tag + "')[" + index + "]";
    } else {
        // 标签选择器或其他
        return "(function() {"
               "var el = document.getElementById('" + cleanSelector + "');"
               "if (el) return el;"
               "var els = document.getElementsByClassName('" + cleanSelector + "');"
               "if (els.length > 0) return " + (returnAll ? "els" : "els[0]") + ";"
               "return document.getElementsByTagName('" + cleanSelector + "');"
               "})()";
    }
}

std::string CHTLJSCodeGenerator::processVirtualObjectMethod(const std::string& virName, const std::string& method) {
    std::string key = virName + "->" + method;
    auto it = virtualObjects_.find(key);
    if (it != virtualObjects_.end()) {
        return it->second;
    }
    return virName + "." + method; // 降级到普通调用
}

std::string CHTLJSCodeGenerator::generateEventDelegationCode() {
    if (eventDelegations_.empty()) {
        return "";
    }
    
    std::stringstream code;
    code << "// Event Delegation Setup\n";
    code << "(function() {\n";
    
    for (const auto& [parent, configs] : eventDelegations_) {
        code << "  var parent = " << parent << ";\n";
        for (const auto& config : configs) {
            code << "  " << config << "\n";
        }
    }
    
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSCodeGenerator::generateAnimationCode(const std::string& config) {
    std::stringstream code;
    
    code << "(function() {\n";
    code << "  var config = " << config << ";\n";
    code << "  var target = config.target;\n";
    code << "  var duration = config.duration || 1000;\n";
    code << "  var startTime = null;\n";
    code << "  \n";
    code << "  function animate(timestamp) {\n";
    code << "    if (!startTime) startTime = timestamp;\n";
    code << "    var progress = (timestamp - startTime) / duration;\n";
    code << "    \n";
    code << "    if (progress >= 1) progress = 1;\n";
    code << "    \n";
    code << "    // Apply styles based on progress\n";
    code << "    if (config.when) {\n";
    code << "      config.when.forEach(function(keyframe) {\n";
    code << "        if (progress >= keyframe.at) {\n";
    code << "          Object.assign(target.style, keyframe);\n";
    code << "        }\n";
    code << "      });\n";
    code << "    }\n";
    code << "    \n";
    code << "    if (progress < 1) {\n";
    code << "      requestAnimationFrame(animate);\n";
    code << "    } else {\n";
    code << "      if (config.end) Object.assign(target.style, config.end);\n";
    code << "      if (config.callback) config.callback();\n";
    code << "    }\n";
    code << "  }\n";
    code << "  \n";
    code << "  if (config.begin) Object.assign(target.style, config.begin);\n";
    code << "  requestAnimationFrame(animate);\n";
    code << "})()";
    
    return code.str();
}

} // namespace chtl