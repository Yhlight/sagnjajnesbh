#include "compilers/chtl/chtl_context.h"
#include <sstream>

namespace chtl {

CHTLContext::CHTLContext() {
    // 初始化全局作用域
    scopeStack_.push(Scope(ScopeType::GLOBAL));
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::initialize(const std::string& sourceFile, const std::string& sourceContent) {
    compileContext_.sourceFile = sourceFile;
    compileContext_.sourceContent = sourceContent;
    compileContext_.currentLine = 1;
    compileContext_.currentColumn = 1;
    compileContext_.currentPos = 0;
    compileContext_.errors.clear();
    compileContext_.warnings.clear();
    compileContext_.generatedCode.clear();
}

void CHTLContext::enterScope(ScopeType type, const std::string& name) {
    scopeStack_.push(Scope(type, name));
}

void CHTLContext::exitScope() {
    if (scopeStack_.size() > 1) {  // 保留全局作用域
        scopeStack_.pop();
    }
}

Scope* CHTLContext::getCurrentScope() {
    if (scopeStack_.empty()) {
        return nullptr;
    }
    return &scopeStack_.top();
}

const Scope* CHTLContext::getCurrentScope() const {
    if (scopeStack_.empty()) {
        return nullptr;
    }
    return &scopeStack_.top();
}

void CHTLContext::addError(const std::string& message, size_t line, size_t column) {
    size_t errorLine = line > 0 ? line : compileContext_.currentLine;
    size_t errorColumn = column > 0 ? column : compileContext_.currentColumn;
    
    std::string formattedError = formatError(message, errorLine, errorColumn);
    compileContext_.errors.push_back(formattedError);
}

void CHTLContext::addWarning(const std::string& message, size_t line, size_t column) {
    size_t warnLine = line > 0 ? line : compileContext_.currentLine;
    size_t warnColumn = column > 0 ? column : compileContext_.currentColumn;
    
    std::string formattedWarning = formatWarning(message, warnLine, warnColumn);
    compileContext_.warnings.push_back(formattedWarning);
}

void CHTLContext::updatePosition(const CHTLToken& token) {
    compileContext_.currentLine = token.line;
    compileContext_.currentColumn = token.column;
    compileContext_.currentPos = token.startPos;
}

void CHTLContext::updatePosition(size_t line, size_t column, size_t pos) {
    compileContext_.currentLine = line;
    compileContext_.currentColumn = column;
    compileContext_.currentPos = pos;
}

void CHTLContext::appendGeneratedCode(const std::string& code) {
    compileContext_.generatedCode += code;
}

void CHTLContext::reset() {
    stateManager_.reset();
    globalMap_.clear();
    compileContext_ = CompileContext();
    
    // 清空作用域栈并重新初始化全局作用域
    while (!scopeStack_.empty()) {
        scopeStack_.pop();
    }
    scopeStack_.push(Scope(ScopeType::GLOBAL));
    
    options_.clear();
}

void CHTLContext::setOption(const std::string& key, const std::string& value) {
    options_[key] = value;
    
    // 同步到全局映射的配置
    globalMap_.setConfiguration(key, value);
}

std::string CHTLContext::getOption(const std::string& key, const std::string& defaultValue) const {
    auto it = options_.find(key);
    if (it != options_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool CHTLContext::isInTemplateContext() const {
    auto state = stateManager_.getCurrentState();
    return state == CHTLCompilerState::IN_TEMPLATE;
}

bool CHTLContext::isInCustomContext() const {
    auto state = stateManager_.getCurrentState();
    return state == CHTLCompilerState::IN_CUSTOM;
}

bool CHTLContext::isInElementContext() const {
    return stateManager_.isInsideElement();
}

bool CHTLContext::isInStyleContext() const {
    auto state = stateManager_.getCurrentState();
    return state == CHTLCompilerState::IN_STYLE_BLOCK;
}

bool CHTLContext::isInScriptContext() const {
    auto state = stateManager_.getCurrentState();
    return state == CHTLCompilerState::IN_SCRIPT_BLOCK;
}

std::string CHTLContext::getCurrentElementName() const {
    // 遍历作用域栈查找最近的元素作用域
    std::stack<Scope> temp = scopeStack_;
    while (!temp.empty()) {
        if (temp.top().type == ScopeType::ELEMENT) {
            return temp.top().name;
        }
        temp.pop();
    }
    return "";
}

std::string CHTLContext::getCurrentElementId() const {
    if (auto scope = getCurrentScope()) {
        if (scope->type == ScopeType::ELEMENT) {
            auto it = scope->attributes.find("id");
            if (it != scope->attributes.end()) {
                return it->second;
            }
        }
    }
    return "";
}

std::vector<std::string> CHTLContext::getCurrentElementClasses() const {
    std::vector<std::string> classes;
    
    if (auto scope = getCurrentScope()) {
        if (scope->type == ScopeType::ELEMENT) {
            auto it = scope->attributes.find("class");
            if (it != scope->attributes.end()) {
                // 简单的类名分割
                std::stringstream ss(it->second);
                std::string className;
                while (ss >> className) {
                    classes.push_back(className);
                }
            }
        }
    }
    
    return classes;
}

std::string CHTLContext::formatError(const std::string& message, size_t line, size_t column) const {
    std::stringstream ss;
    if (!compileContext_.sourceFile.empty()) {
        ss << compileContext_.sourceFile << ":";
    }
    ss << line << ":" << column << ": error: " << message;
    return ss.str();
}

std::string CHTLContext::formatWarning(const std::string& message, size_t line, size_t column) const {
    std::stringstream ss;
    if (!compileContext_.sourceFile.empty()) {
        ss << compileContext_.sourceFile << ":";
    }
    ss << line << ":" << column << ": warning: " << message;
    return ss.str();
}

} // namespace chtl