#include "ArrowOperatorProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

ArrowOperatorProcessor::ArrowOperatorProcessor() {
    initializeCHTLJSMethods();
    initializeDOMMethods();
}

ArrowOperatorProcessor::~ArrowOperatorProcessor() = default;

// === 按语法文档：箭头操作符处理 ===

std::string ArrowOperatorProcessor::processArrowExpression(const std::string& expression) {
    // 按语法文档：-> 与 . 是完全等价的
    
    if (!isValidArrowExpression(expression)) {
        addError("无效的箭头操作符表达式: " + expression);
        return "";
    }
    
    // 清理表达式
    std::string cleanedExpression = cleanExpression(expression);
    
    // 检查是否包含箭头操作符
    if (cleanedExpression.find("->") == std::string::npos) {
        // 没有箭头操作符，直接返回
        return cleanedExpression;
    }
    
    // 转换为等价的点操作符表达式
    std::string dotExpression = convertArrowToDot(cleanedExpression);
    
    // 生成等价的JavaScript代码
    return generateEquivalentCode(dotExpression);
}

std::string ArrowOperatorProcessor::convertArrowToDot(const std::string& expression) {
    // 按语法文档：{{box}}->textContent() 等价于 {{box}}.textContent()
    
    std::string result = expression;
    
    // 简单替换：-> 变为 .
    size_t pos = 0;
    while ((pos = result.find("->", pos)) != std::string::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    return result;
}

std::vector<ArrowOperatorProcessor::MethodCall> ArrowOperatorProcessor::parseMethodChain(const std::string& expression) {
    std::vector<MethodCall> methodChain;
    
    // 分割链式调用
    std::vector<std::string> calls = splitChainedCalls(expression);
    
    for (const auto& call : calls) {
        MethodCall methodCall = parseMethodCall(call);
        if (!methodCall.method.empty()) {
            methodChain.push_back(methodCall);
        }
    }
    
    return methodChain;
}

std::string ArrowOperatorProcessor::generateEquivalentCode(const std::string& expression) {
    // 按语法文档：确保箭头操作符和点操作符的完全等价性
    
    // 解析方法调用链
    std::vector<MethodCall> methodChain = parseMethodChain(expression);
    
    if (methodChain.empty()) {
        return expression;
    }
    
    // 检查第一个调用是否为CHTL JS特殊方法
    if (methodChain.size() > 0 && isCHTLJSMethod(methodChain[0].method)) {
        return processCHTLJSMethod(methodChain[0].target, methodChain[0].method, methodChain[0].parameters);
    }
    
    // 处理链式调用
    if (methodChain.size() > 1) {
        return processChainedCalls(methodChain);
    }
    
    // 单个方法调用
    return expression;
}

// === 链式调用处理 ===

ArrowOperatorProcessor::MethodCall ArrowOperatorProcessor::parseMethodCall(const std::string& callExpression) {
    MethodCall call;
    
    // 查找箭头或点操作符
    size_t arrowPos = callExpression.find("->");
    size_t dotPos = callExpression.find(".");
    
    size_t operatorPos = std::string::npos;
    if (arrowPos != std::string::npos && dotPos != std::string::npos) {
        operatorPos = std::min(arrowPos, dotPos);
    } else if (arrowPos != std::string::npos) {
        operatorPos = arrowPos;
    } else if (dotPos != std::string::npos) {
        operatorPos = dotPos;
    }
    
    if (operatorPos != std::string::npos) {
        call.target = callExpression.substr(0, operatorPos);
        
        // 提取方法部分
        size_t methodStart = operatorPos + (callExpression.substr(operatorPos, 2) == "->" ? 2 : 1);
        std::string methodPart = callExpression.substr(methodStart);
        
        // 分离方法名和参数
        size_t parenPos = methodPart.find('(');
        if (parenPos != std::string::npos) {
            call.method = methodPart.substr(0, parenPos);
            call.parameters = extractParameters(methodPart);
        } else {
            call.method = methodPart;
        }
    }
    
    return call;
}

std::string ArrowOperatorProcessor::processChainedCalls(const std::vector<MethodCall>& methodChain) {
    if (methodChain.empty()) {
        return "";
    }
    
    std::string code = "// 按CHTL语法文档：链式调用处理\n";
    code += "(function() {\n";
    code += "    let result = " + methodChain[0].target + ";\n";
    
    for (size_t i = 0; i < methodChain.size(); ++i) {
        const auto& call = methodChain[i];
        
        if (i == 0) {
            // 第一个调用
            if (call.parameters.empty()) {
                code += "    result = result." + call.method + ";\n";
            } else {
                code += "    result = result." + call.method + "(" + call.parameters + ");\n";
            }
        } else {
            // 后续调用
            if (call.parameters.empty()) {
                code += "    result = result." + call.method + ";\n";
            } else {
                code += "    result = result." + call.method + "(" + call.parameters + ");\n";
            }
        }
        
        // 添加空值检查
        if (i < methodChain.size() - 1) {
            code += "    if (!result) return null;\n";
        }
    }
    
    code += "    return result;\n";
    code += "})()";
    
    return code;
}

// === CHTL JS特殊方法处理 ===

bool ArrowOperatorProcessor::isCHTLJSMethod(const std::string& methodName) {
    return std::find(chtljsMethods_.begin(), chtljsMethods_.end(), methodName) != chtljsMethods_.end();
}

std::string ArrowOperatorProcessor::processCHTLJSMethod(const std::string& target, const std::string& method, const std::string& params) {
    // 按语法文档：处理CHTL JS特殊方法
    
    std::string code = "// 按CHTL语法文档：CHTL JS特殊方法 " + method + "\n";
    
    if (method == "listen") {
        // 委托给ListenSystemProcessor处理
        return "/* CHTL listen方法需要ListenSystemProcessor处理 */\n" + target + ".listen(" + params + ")";
    } else if (method == "delegate") {
        // 委托给DelegateSystemProcessor处理
        return "/* CHTL delegate方法需要DelegateSystemProcessor处理 */\n" + target + ".delegate(" + params + ")";
    } else if (method == "animate") {
        // 委托给AnimateSystemProcessor处理
        return "/* CHTL animate方法需要AnimateSystemProcessor处理 */\n" + target + ".animate(" + params + ")";
    } else {
        // 标准DOM方法，直接转换
        return target + "." + method + "(" + params + ")";
    }
}

// === 辅助方法 ===

std::string ArrowOperatorProcessor::extractTarget(const std::string& expression) {
    size_t arrowPos = expression.find("->");
    if (arrowPos != std::string::npos) {
        return expression.substr(0, arrowPos);
    }
    
    size_t dotPos = expression.find(".");
    if (dotPos != std::string::npos) {
        return expression.substr(0, dotPos);
    }
    
    return "";
}

std::string ArrowOperatorProcessor::extractMethod(const std::string& expression) {
    size_t arrowPos = expression.find("->");
    size_t dotPos = expression.find(".");
    
    size_t operatorPos = std::string::npos;
    size_t operatorLen = 0;
    
    if (arrowPos != std::string::npos && dotPos != std::string::npos) {
        if (arrowPos < dotPos) {
            operatorPos = arrowPos;
            operatorLen = 2;
        } else {
            operatorPos = dotPos;
            operatorLen = 1;
        }
    } else if (arrowPos != std::string::npos) {
        operatorPos = arrowPos;
        operatorLen = 2;
    } else if (dotPos != std::string::npos) {
        operatorPos = dotPos;
        operatorLen = 1;
    }
    
    if (operatorPos != std::string::npos) {
        std::string methodPart = expression.substr(operatorPos + operatorLen);
        size_t parenPos = methodPart.find('(');
        if (parenPos != std::string::npos) {
            return methodPart.substr(0, parenPos);
        } else {
            return methodPart;
        }
    }
    
    return "";
}

std::string ArrowOperatorProcessor::extractParameters(const std::string& methodCall) {
    size_t startPos = methodCall.find('(');
    size_t endPos = methodCall.rfind(')');
    
    if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) {
        return methodCall.substr(startPos + 1, endPos - startPos - 1);
    }
    
    return "";
}

std::vector<std::string> ArrowOperatorProcessor::splitChainedCalls(const std::string& expression) {
    std::vector<std::string> calls;
    
    // 简化实现：按 -> 和 . 分割
    std::string current = expression;
    std::string accumulated;
    
    size_t pos = 0;
    while (pos < current.length()) {
        if (pos + 1 < current.length() && current.substr(pos, 2) == "->") {
            // 找到箭头操作符
            if (!accumulated.empty()) {
                calls.push_back(accumulated);
                accumulated.clear();
            }
            pos += 2;
        } else if (current[pos] == '.' && !accumulated.empty()) {
            // 找到点操作符（且不是第一个字符）
            calls.push_back(accumulated);
            accumulated.clear();
            pos += 1;
        } else {
            accumulated += current[pos];
            pos++;
        }
    }
    
    if (!accumulated.empty()) {
        calls.push_back(accumulated);
    }
    
    return calls;
}

bool ArrowOperatorProcessor::isValidArrowExpression(const std::string& expression) {
    if (expression.empty()) return false;
    
    // 必须包含箭头操作符或点操作符
    return expression.find("->") != std::string::npos || 
           expression.find(".") != std::string::npos;
}

std::string ArrowOperatorProcessor::cleanExpression(const std::string& expression) {
    std::string cleaned = expression;
    
    // 移除多余的空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n") + 1);
    
    return cleaned;
}

bool ArrowOperatorProcessor::isValidMethodCall(const MethodCall& call) {
    return !call.target.empty() && !call.method.empty();
}

void ArrowOperatorProcessor::initializeCHTLJSMethods() {
    // 按语法文档：CHTL JS特殊方法
    chtljsMethods_ = {
        "listen",      // 增强监听器
        "delegate",    // 事件委托
        "animate"      // 动画系统
    };
}

void ArrowOperatorProcessor::initializeDOMMethods() {
    // 标准DOM方法（部分列表）
    domMethods_ = {
        "textContent", "innerHTML", "outerHTML", "innerText",
        "getAttribute", "setAttribute", "removeAttribute", "hasAttribute",
        "appendChild", "removeChild", "insertBefore", "replaceChild",
        "querySelector", "querySelectorAll", "getElementById",
        "getElementsByClassName", "getElementsByTagName",
        "addEventListener", "removeEventListener", "dispatchEvent",
        "focus", "blur", "click", "submit", "reset",
        "scrollIntoView", "scrollTo", "scrollBy",
        "getBoundingClientRect", "getClientRects",
        "matches", "closest", "contains"
    };
}

// === 错误处理 ===

void ArrowOperatorProcessor::addError(const std::string& message) {
    errors_.push_back("ArrowOperator: " + message);
}

void ArrowOperatorProcessor::clearErrors() {
    errors_.clear();
}

bool ArrowOperatorProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> ArrowOperatorProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl