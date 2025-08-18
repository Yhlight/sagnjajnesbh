#include "LocalScriptProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

LocalScriptProcessor::LocalScriptProcessor() {
    // 初始化所有CHTL JS处理器
    selectorProcessor_ = std::make_shared<EnhancedSelectorProcessor>();
    listenProcessor_ = std::make_shared<ListenSystemProcessor>();
    delegateProcessor_ = std::make_shared<DelegateSystemProcessor>();
    animateProcessor_ = std::make_shared<AnimateSystemProcessor>();
    virtualProcessor_ = std::make_shared<VirtualObjectProcessor>();
    arrowProcessor_ = std::make_shared<ArrowOperatorProcessor>();
    
    initializeDefaultPriorities();
}

LocalScriptProcessor::~LocalScriptProcessor() = default;

// === 按语法文档：局部script处理 ===

std::string LocalScriptProcessor::processLocalScript(const std::string& scriptContent, const std::string& elementContext) {
    // 按语法文档：局部script会被添加到一个不会全局污染，具有高优先级的全局script块之中
    
    if (scriptContent.empty()) {
        return "";
    }
    
    // 创建script块
    ScriptBlock block;
    block.elementContext = elementContext;
    block.content = scriptContent;
    block.priority = calculateScriptPriority(elementContext);
    block.isProcessed = false;
    
    // 处理CHTL JS语法
    block.processedContent = processCHTLJSSyntax(scriptContent);
    block.isProcessed = true;
    
    // 包装防止全局污染
    block.processedContent = wrapScriptForIsolation(block.processedContent, elementContext);
    
    // 添加到全局注册表
    globalRegistry_.scriptBlocks.push_back(block);
    
    // 返回处理后的内容（用于即时使用）
    return block.processedContent;
}

std::string LocalScriptProcessor::generateGlobalScript() {
    // 按语法文档：生成具有高优先级的全局script块
    
    if (globalRegistry_.scriptBlocks.empty()) {
        return "";
    }
    
    // 按优先级排序
    sortScriptBlocksByPriority();
    
    std::string globalScript = "// 按CHTL语法文档：全局script块（由局部script合并生成）\n";
    globalScript += "(function() {\n";
    globalScript += "    'use strict';\n";
    globalScript += "    \n";
    globalScript += "    // CHTL局部script全局注册表\n";
    globalScript += "    if (!window.CHTL_SCRIPT_REGISTRY) {\n";
    globalScript += "        window.CHTL_SCRIPT_REGISTRY = {\n";
    globalScript += "            blocks: [],\n";
    globalScript += "            executed: new Set()\n";
    globalScript += "        };\n";
    globalScript += "    }\n";
    globalScript += "    \n";
    
    // 按优先级顺序添加script块
    for (const auto& block : globalRegistry_.scriptBlocks) {
        globalScript += "    // 来源: " + block.elementContext + " (优先级: " + std::to_string(block.priority) + ")\n";
        globalScript += "    (function() {\n";
        globalScript += "        " + block.processedContent + "\n";
        globalScript += "    })();\n";
        globalScript += "    \n";
    }
    
    globalScript += "})();\n";
    
    // 缓存合并结果
    globalRegistry_.combinedScript = globalScript;
    
    return globalScript;
}

std::string LocalScriptProcessor::processCHTLJSSyntax(const std::string& scriptContent) {
    // 按语法文档：处理script内容中的CHTL JS语法
    
    std::string processed = scriptContent;
    
    // 1. 处理vir虚对象定义（必须最先处理）
    processed = processVirtualObjectDefinitions(processed);
    
    // 2. 处理增强选择器
    processed = processEnhancedSelectors(processed);
    
    // 3. 处理箭头操作符
    processed = processArrowOperators(processed);
    
    // 4. 处理listen语句
    processed = processListenStatements(processed);
    
    // 5. 处理delegate语句
    processed = processDelegateStatements(processed);
    
    // 6. 处理animate语句
    processed = processAnimateStatements(processed);
    
    // 7. 处理vir虚对象调用（必须最后处理）
    processed = processVirtualObjectCalls(processed);
    
    return processed;
}

// === 优先级管理 ===

int LocalScriptProcessor::calculateScriptPriority(const std::string& elementContext) {
    // 按语法文档：计算局部script的优先级
    
    auto it = globalRegistry_.priorityMap.find(elementContext);
    if (it != globalRegistry_.priorityMap.end()) {
        return it->second;
    }
    
    // 默认优先级规则
    if (elementContext == "head") return 1000;      // 最高优先级
    if (elementContext == "body") return 900;       // 高优先级
    if (elementContext == "div") return 500;        // 中等优先级
    if (elementContext == "span") return 400;       // 较低优先级
    
    return 100; // 默认优先级
}

void LocalScriptProcessor::sortScriptBlocksByPriority() {
    std::sort(globalRegistry_.scriptBlocks.begin(), globalRegistry_.scriptBlocks.end(),
              [](const ScriptBlock& a, const ScriptBlock& b) {
                  return a.priority > b.priority; // 高优先级在前
              });
}

void LocalScriptProcessor::setElementPriority(const std::string& elementType, int priority) {
    globalRegistry_.priorityMap[elementType] = priority;
}

// === CHTL JS语法处理 ===

std::string LocalScriptProcessor::processEnhancedSelectors(const std::string& content) {
    std::string processed = content;
    
    // 找到所有增强选择器
    std::vector<std::string> selectors = extractAllSelectors(content);
    
    for (const auto& selector : selectors) {
        std::string jsCode = selectorProcessor_->processEnhancedSelector(selector);
        if (!jsCode.empty()) {
            // 替换选择器为生成的JavaScript代码
            size_t pos = processed.find(selector);
            if (pos != std::string::npos) {
                processed.replace(pos, selector.length(), "(" + jsCode + ")");
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processArrowOperators(const std::string& content) {
    std::string processed = content;
    
    // 找到所有箭头操作符表达式
    std::regex arrowRegex(R"([^-]+->[^-]+)"); // 避免匹配注释中的箭头
    std::sregex_iterator iter(content.begin(), content.end(), arrowRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string arrowExpr = (*iter)[0].str();
        std::string dotExpr = arrowProcessor_->processArrowExpression(arrowExpr);
        
        if (!dotExpr.empty()) {
            size_t pos = processed.find(arrowExpr);
            if (pos != std::string::npos) {
                processed.replace(pos, arrowExpr.length(), dotExpr);
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processListenStatements(const std::string& content) {
    std::string processed = content;
    
    // 找到所有listen语句
    std::regex listenRegex(R"(\{\{[^}]+\}\}->listen\s*\([^)]+\))");
    std::sregex_iterator iter(content.begin(), content.end(), listenRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string listenStmt = (*iter)[0].str();
        std::string jsCode = listenProcessor_->processListenStatement(listenStmt);
        
        if (!jsCode.empty()) {
            size_t pos = processed.find(listenStmt);
            if (pos != std::string::npos) {
                processed.replace(pos, listenStmt.length(), jsCode);
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processDelegateStatements(const std::string& content) {
    std::string processed = content;
    
    // 找到所有delegate语句
    std::regex delegateRegex(R"(\{\{[^}]+\}\}->delegate\s*\([^)]+\))");
    std::sregex_iterator iter(content.begin(), content.end(), delegateRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string delegateStmt = (*iter)[0].str();
        std::string jsCode = delegateProcessor_->processDelegateStatement(delegateStmt);
        
        if (!jsCode.empty()) {
            size_t pos = processed.find(delegateStmt);
            if (pos != std::string::npos) {
                processed.replace(pos, delegateStmt.length(), jsCode);
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processAnimateStatements(const std::string& content) {
    std::string processed = content;
    
    // 找到所有animate语句
    std::regex animateRegex(R"(animate\s*\([^)]+\))");
    std::sregex_iterator iter(content.begin(), content.end(), animateRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string animateStmt = (*iter)[0].str();
        std::string jsCode = animateProcessor_->processAnimateStatement(animateStmt);
        
        if (!jsCode.empty()) {
            size_t pos = processed.find(animateStmt);
            if (pos != std::string::npos) {
                processed.replace(pos, animateStmt.length(), jsCode);
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processVirtualObjectDefinitions(const std::string& content) {
    std::string processed = content;
    
    // 找到所有vir定义
    std::regex virRegex(R"(vir\s+\w+\s*=\s*\w+\s*\([^)]+\))");
    std::sregex_iterator iter(content.begin(), content.end(), virRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string virStmt = (*iter)[0].str();
        std::string jsCode = virtualProcessor_->processVirtualObjectDefinition(virStmt);
        
        if (!jsCode.empty()) {
            size_t pos = processed.find(virStmt);
            if (pos != std::string::npos) {
                processed.replace(pos, virStmt.length(), jsCode);
            }
        }
    }
    
    return processed;
}

std::string LocalScriptProcessor::processVirtualObjectCalls(const std::string& content) {
    std::string processed = content;
    
    // 找到所有vir调用
    std::regex virCallRegex(R"(\w+->[\w<>]+\(\))");
    std::sregex_iterator iter(content.begin(), content.end(), virCallRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string virCall = (*iter)[0].str();
        std::string jsCode = virtualProcessor_->processVirtualObjectCall(virCall);
        
        if (!jsCode.empty()) {
            size_t pos = processed.find(virCall);
            if (pos != std::string::npos) {
                processed.replace(pos, virCall.length(), jsCode);
            }
        }
    }
    
    return processed;
}

// === 全局污染防护 ===

std::string LocalScriptProcessor::wrapScriptForIsolation(const std::string& content, const std::string& context) {
    // 按语法文档：不会全局污染的script块
    
    std::string wrapped = "// 按CHTL语法文档：局部script块 (" + context + ")\n";
    wrapped += "(function() {\n";
    wrapped += "    'use strict';\n";
    wrapped += "    \n";
    wrapped += "    // 局部作用域，防止全局污染\n";
    wrapped += "    try {\n";
    
    // 添加内容（每行缩进）
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        wrapped += "        " + line + "\n";
    }
    
    wrapped += "    } catch (error) {\n";
    wrapped += "        console.error('CHTL局部script错误 (" + context + "):', error);\n";
    wrapped += "    }\n";
    wrapped += "})();\n";
    
    return wrapped;
}

std::string LocalScriptProcessor::generateNamespaceProtection(const std::string& content) {
    return "// 命名空间保护\n" + content;
}

std::string LocalScriptProcessor::processVariableScope(const std::string& content) {
    // 确保变量声明使用let/const而不是var
    std::string processed = content;
    
    // 替换var为let（简化实现）
    std::regex varRegex(R"(\bvar\b)");
    processed = std::regex_replace(processed, varRegex, "let");
    
    return processed;
}

// === 辅助方法 ===

bool LocalScriptProcessor::containsCHTLJSSyntax(const std::string& content) {
    // 检测是否包含CHTL JS特殊语法
    
    return content.find("{{") != std::string::npos ||    // 增强选择器
           content.find("->") != std::string::npos ||    // 箭头操作符
           content.find("listen") != std::string::npos || // listen函数
           content.find("delegate") != std::string::npos || // delegate函数
           content.find("animate") != std::string::npos || // animate函数
           content.find("vir ") != std::string::npos;     // 虚对象
}

std::vector<std::string> LocalScriptProcessor::extractAllSelectors(const std::string& content) {
    std::vector<std::string> selectors;
    
    // 查找所有 {{...}} 格式的选择器
    std::regex selectorRegex(R"(\{\{[^}]+\}\})");
    std::sregex_iterator iter(content.begin(), content.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        selectors.push_back((*iter)[0].str());
    }
    
    return selectors;
}

std::vector<std::string> LocalScriptProcessor::extractAllMethodCalls(const std::string& content) {
    std::vector<std::string> calls;
    
    // 查找所有 target->method() 格式的调用
    std::regex callRegex(R"([^-]+->[\w<>]+\([^)]*\))");
    std::sregex_iterator iter(content.begin(), content.end(), callRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        calls.push_back((*iter)[0].str());
    }
    
    return calls;
}

std::string LocalScriptProcessor::generateScriptBlockId(const std::string& elementContext) {
    static int counter = 0;
    return "chtl_script_" + elementContext + "_" + std::to_string(++counter);
}

std::string LocalScriptProcessor::cleanAndFormatScript(const std::string& content) {
    std::string cleaned = content;
    
    // 移除多余的空白行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    cleaned = std::regex_replace(cleaned, multipleNewlines, "\n\n");
    
    // 移除行首尾空白
    std::stringstream ss(cleaned);
    std::string line;
    std::string formatted;
    
    while (std::getline(ss, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        if (!line.empty()) {
            formatted += line + "\n";
        }
    }
    
    return formatted;
}

void LocalScriptProcessor::initializeDefaultPriorities() {
    // 按语法文档：初始化默认优先级
    globalRegistry_.priorityMap = {
        {"head", 1000},     // 最高优先级
        {"body", 900},      // 高优先级  
        {"main", 800},      // 较高优先级
        {"header", 700},    // 中高优先级
        {"nav", 650},       // 中高优先级
        {"section", 600},   // 中等优先级
        {"article", 550},   // 中等优先级
        {"div", 500},       // 中等优先级
        {"aside", 450},     // 较低优先级
        {"footer", 400},    // 较低优先级
        {"span", 300},      // 低优先级
        {"p", 250},         // 低优先级
        {"a", 200},         // 很低优先级
        {"button", 600},    // 交互元素较高优先级
        {"input", 580},     // 交互元素较高优先级
        {"form", 750}       // 表单元素高优先级
    };
}

// === 错误处理 ===

void LocalScriptProcessor::addError(const std::string& message) {
    errors_.push_back("LocalScript: " + message);
}

void LocalScriptProcessor::clearErrors() {
    errors_.clear();
}

bool LocalScriptProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> LocalScriptProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl