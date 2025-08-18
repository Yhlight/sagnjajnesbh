#include "CHTLContext.h"
#include <iostream>
#include <sstream>

namespace chtl {

CHTLContext::CHTLContext() : debug_mode_(false) {
    // 初始化全局作用域
    scope_stack_.push("global");
}

void CHTLContext::enterScope(const std::string& scope_name) {
    std::string name = scope_name.empty() ? "anonymous_" + std::to_string(scope_stack_.size()) : scope_name;
    scope_stack_.push(name);
    
    if (debug_mode_) {
        std::cout << "进入作用域: " << name << std::endl;
    }
}

void CHTLContext::exitScope() {
    if (scope_stack_.size() > 1) { // 保留全局作用域
        std::string scope = scope_stack_.top();
        scope_stack_.pop();
        
        if (debug_mode_) {
            std::cout << "退出作用域: " << scope << std::endl;
        }
    }
}

std::string CHTLContext::getCurrentScope() const {
    return scope_stack_.empty() ? "" : scope_stack_.top();
}

void CHTLContext::addSymbol(const std::string& name, const std::string& type) {
    std::string full_name = buildFullName(name);
    symbols_[full_name] = type;
    
    if (debug_mode_) {
        std::cout << "添加符号: " << full_name << " : " << type << std::endl;
    }
}

bool CHTLContext::hasSymbol(const std::string& name) const {
    std::string full_name = buildFullName(name);
    return symbols_.find(full_name) != symbols_.end();
}

std::string CHTLContext::getSymbolType(const std::string& name) const {
    std::string full_name = buildFullName(name);
    auto it = symbols_.find(full_name);
    return it != symbols_.end() ? it->second : "";
}

void CHTLContext::addTemplate(const std::string& name, const std::string& definition) {
    std::string full_name = buildFullName(name);
    templates_[full_name] = definition;
    
    if (debug_mode_) {
        std::cout << "添加模板: " << full_name << std::endl;
    }
}

void CHTLContext::addCustom(const std::string& name, const std::string& definition) {
    std::string full_name = buildFullName(name);
    customs_[full_name] = definition;
    
    if (debug_mode_) {
        std::cout << "添加自定义: " << full_name << std::endl;
    }
}

bool CHTLContext::hasTemplate(const std::string& name) const {
    std::string full_name = buildFullName(name);
    return templates_.find(full_name) != templates_.end();
}

bool CHTLContext::hasCustom(const std::string& name) const {
    std::string full_name = buildFullName(name);
    return customs_.find(full_name) != customs_.end();
}

void CHTLContext::enterNamespace(const std::string& ns_name) {
    namespace_stack_.push(ns_name);
    
    if (debug_mode_) {
        std::cout << "进入命名空间: " << ns_name << std::endl;
    }
}

void CHTLContext::exitNamespace() {
    if (!namespace_stack_.empty()) {
        std::string ns = namespace_stack_.top();
        namespace_stack_.pop();
        
        if (debug_mode_) {
            std::cout << "退出命名空间: " << ns << std::endl;
        }
    }
}

std::string CHTLContext::getCurrentNamespace() const {
    return namespace_stack_.empty() ? "" : namespace_stack_.top();
}

void CHTLContext::addError(const std::string& error) {
    errors_.push_back(error);
    
    if (debug_mode_) {
        std::cerr << "错误: " << error << std::endl;
    }
}

void CHTLContext::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    
    if (debug_mode_) {
        std::cout << "警告: " << warning << std::endl;
    }
}

std::string CHTLContext::buildFullName(const std::string& name) const {
    std::ostringstream oss;
    
    // 添加命名空间
    if (!namespace_stack_.empty()) {
        oss << namespace_stack_.top() << "::";
    }
    
    // 添加作用域
    if (!scope_stack_.empty() && scope_stack_.top() != "global") {
        oss << scope_stack_.top() << ".";
    }
    
    oss << name;
    return oss.str();
}

} // namespace chtl