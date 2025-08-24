#include "CHTLJS/Compiler/Context.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {
namespace JSCompiler {

class CompileContext::Impl {
public:
    ContextConfig config;
    GlobalMap globalMap;
    StateManager stateManager;
    
    std::string currentFile;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::stringstream outputBuffer;
    
    size_t functionCounter = 0;
    
    SelectorInfo ParseSelector(const std::string& selector) {
        SelectorInfo info;
        std::string s = selector;
        
        // 去除前后空白
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);
        
        // 提取索引 [n]
        std::regex indexRegex(R"(\[(\d+)\]$)");
        std::smatch match;
        if (std::regex_search(s, match, indexRegex)) {
            info.index = match[1];
            s = s.substr(0, match.position());
        }
        
        // 判断选择器类型
        if (s.empty()) {
            info.type = SelectorInfo::TAG;
            info.value = "";
        } else if (s[0] == '.') {
            info.type = SelectorInfo::CLASS;
            info.value = s.substr(1);
        } else if (s[0] == '#') {
            info.type = SelectorInfo::ID;
            info.value = s.substr(1);
        } else if (s.find(' ') != std::string::npos || 
                   s.find('>') != std::string::npos ||
                   s.find('+') != std::string::npos ||
                   s.find('~') != std::string::npos) {
            info.type = SelectorInfo::COMPLEX;
            info.value = s;
        } else {
            // 可能是标签名或标识符
            info.type = SelectorInfo::TAG;
            info.value = s;
        }
        
        return info;
    }
    
    std::string GenerateQuerySelector(const SelectorInfo& info) {
        std::stringstream js;
        std::string selector;
        
        // 构建CSS选择器
        switch (info.type) {
            case SelectorInfo::TAG:
                selector = info.value.empty() ? "*" : info.value;
                break;
            case SelectorInfo::CLASS:
                selector = "." + info.value;
                break;
            case SelectorInfo::ID:
                selector = "#" + info.value;
                break;
            case SelectorInfo::COMPLEX:
                selector = info.value;
                break;
        }
        
        // 生成查询代码
        if (!info.index.empty()) {
            // 有索引，使用querySelectorAll
            js << "document.querySelectorAll('" << selector << "')[" << info.index << "]";
        } else if (info.type == SelectorInfo::ID) {
            // ID选择器，使用getElementById
            js << "document.getElementById('" << info.value << "')";
        } else {
            // 其他情况，返回NodeList
            js << "document.querySelectorAll('" << selector << "')";
        }
        
        return js.str();
    }
    
    bool RegisterCurrentVirObject(const std::string& name) {
        GlobalObject obj;
        obj.type = GlobalObjectType::VIR_OBJECT;
        obj.name = name;
        obj.sourceFile = currentFile;
        obj.line = 0;  // TODO: 获取实际行号
        obj.column = 0;
        
        bool result = globalMap.RegisterVirObject(name, obj);
        if (result) {
            stateManager.SetCurrentVirObject(name);
        }
        return result;
    }
    
    std::string GenerateUniqueFunctionName(const std::string& prefix) {
        std::stringstream ss;
        ss << (prefix.empty() ? "chtljs_func" : prefix) << "_" << ++functionCounter;
        return ss.str();
    }
    
    void RegisterEventDelegate(const std::string& parent, 
                              const std::string& target,
                              const std::string& event) {
        globalMap.RegisterDelegate(parent, target, event);
    }
    
    std::string GenerateDelegateCode() {
        std::stringstream code;
        auto delegates = globalMap.GetAllDelegates();
        
        if (!delegates.empty()) {
            code << "\n// CHTL JS 事件委托\n";
            code << "(function() {\n";
            
            // 按父元素分组
            std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> grouped;
            for (const auto& [parent, target, event] : delegates) {
                grouped[parent].emplace_back(target, event);
            }
            
            // 生成委托代码
            for (const auto& [parent, targets] : grouped) {
                SelectorInfo parentInfo = ParseSelector(parent);
                std::string parentQuery = GenerateQuerySelector(parentInfo);
                
                code << "  const parent = " << parentQuery << ";\n";
                code << "  if (parent) {\n";
                
                for (const auto& [target, event] : targets) {
                    SelectorInfo targetInfo = ParseSelector(target);
                    
                    code << "    parent.addEventListener('" << event << "', function(e) {\n";
                    code << "      const target = e.target.closest('" 
                         << (targetInfo.type == SelectorInfo::CLASS ? "." + targetInfo.value :
                             targetInfo.type == SelectorInfo::ID ? "#" + targetInfo.value :
                             targetInfo.value) << "');\n";
                    code << "      if (target) {\n";
                    code << "        // 委托事件处理\n";
                    code << "        console.log('Delegated " << event << " on " << target << "');\n";
                    code << "      }\n";
                    code << "    });\n";
                }
                
                code << "  }\n";
            }
            
            code << "})();\n";
        }
        
        return code.str();
    }
    
    void AddError(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << currentFile << ":" << line << ":" << column << " 错误: " << message;
        errors.push_back(ss.str());
    }
    
    void AddWarning(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << currentFile << ":" << line << ":" << column << " 警告: " << message;
        warnings.push_back(ss.str());
    }
    
    void DumpContext() const {
        std::cout << "=== CHTL JS编译上下文 ===" << std::endl;
        std::cout << "当前文件: " << currentFile << std::endl;
        std::cout << "配置:" << std::endl;
        std::cout << "  虚对象: " << (config.enableVirObjects ? "启用" : "禁用") << std::endl;
        std::cout << "  事件委托: " << (config.enableEventDelegation ? "启用" : "禁用") << std::endl;
        std::cout << "  动画API: " << (config.enableAnimations ? "启用" : "禁用") << std::endl;
        std::cout << "  调试模式: " << (config.debugMode ? "开启" : "关闭") << std::endl;
        
        if (!errors.empty()) {
            std::cout << "\n错误:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  " << error << std::endl;
            }
        }
        
        if (!warnings.empty()) {
            std::cout << "\n警告:" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  " << warning << std::endl;
            }
        }
        
        std::cout << "\n全局映射表:" << std::endl;
        globalMap.Dump();
        
        std::cout << "\n状态栈:" << std::endl;
        stateManager.DumpStateStack();
        
        std::cout << "===================" << std::endl;
    }
};

// CompileContext 实现
CompileContext::CompileContext() : pImpl(std::make_unique<Impl>()) {}
CompileContext::~CompileContext() = default;

void CompileContext::SetConfig(const ContextConfig& config) {
    pImpl->config = config;
}

const ContextConfig& CompileContext::GetConfig() const {
    return pImpl->config;
}

GlobalMap& CompileContext::GetGlobalMap() {
    return pImpl->globalMap;
}

const GlobalMap& CompileContext::GetGlobalMap() const {
    return pImpl->globalMap;
}

StateManager& CompileContext::GetStateManager() {
    return pImpl->stateManager;
}

const StateManager& CompileContext::GetStateManager() const {
    return pImpl->stateManager;
}

SelectorInfo CompileContext::ParseSelector(const std::string& selector) {
    return pImpl->ParseSelector(selector);
}

std::string CompileContext::GenerateQuerySelector(const SelectorInfo& info) {
    return pImpl->GenerateQuerySelector(info);
}

bool CompileContext::RegisterCurrentVirObject(const std::string& name) {
    return pImpl->RegisterCurrentVirObject(name);
}

std::string CompileContext::GetCurrentVirObject() const {
    return pImpl->stateManager.GetCurrentVirObject();
}

std::string CompileContext::GenerateUniqueFunctionName(const std::string& prefix) {
    return pImpl->GenerateUniqueFunctionName(prefix);
}

void CompileContext::RegisterEventDelegate(const std::string& parent, 
                                          const std::string& target,
                                          const std::string& event) {
    pImpl->RegisterEventDelegate(parent, target, event);
}

std::string CompileContext::GenerateDelegateCode() {
    return pImpl->GenerateDelegateCode();
}

void CompileContext::AddError(const std::string& message, size_t line, size_t column) {
    pImpl->AddError(message, line, column);
}

void CompileContext::AddWarning(const std::string& message, size_t line, size_t column) {
    pImpl->AddWarning(message, line, column);
}

bool CompileContext::HasErrors() const {
    return !pImpl->errors.empty();
}

const std::vector<std::string>& CompileContext::GetErrors() const {
    return pImpl->errors;
}

const std::vector<std::string>& CompileContext::GetWarnings() const {
    return pImpl->warnings;
}

void CompileContext::SetCurrentFile(const std::string& filePath) {
    pImpl->currentFile = filePath;
}

std::string CompileContext::GetCurrentFile() const {
    return pImpl->currentFile;
}

void CompileContext::AppendOutput(const std::string& code) {
    pImpl->outputBuffer << code;
}

std::string CompileContext::GetOutput() const {
    return pImpl->outputBuffer.str();
}

void CompileContext::ClearOutput() {
    pImpl->outputBuffer.str("");
    pImpl->outputBuffer.clear();
}

void CompileContext::DumpContext() const {
    pImpl->DumpContext();
}

} // namespace JSCompiler
} // namespace CHTL