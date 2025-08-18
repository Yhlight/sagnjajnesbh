#include "CJMODFramework.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chtl {
namespace cjmod {

std::unique_ptr<CJMODRegistry> CJMODRegistry::instance_ = nullptr;

// JSValue 实现
JSValue::JSValue() : type_(UNDEFINED) {}

JSValue::JSValue(bool value) : type_(BOOLEAN), value_(value) {}

JSValue::JSValue(int value) : type_(NUMBER), value_(static_cast<double>(value)) {}

JSValue::JSValue(double value) : type_(NUMBER), value_(value) {}

JSValue::JSValue(const std::string& value) : type_(STRING), value_(value) {}

JSValue::JSValue(const char* value) : type_(STRING), value_(std::string(value)) {}

JSValue::JSValue(const std::vector<JSValue>& array) : type_(ARRAY), value_(array) {}

JSValue::JSValue(const std::unordered_map<std::string, JSValue>& object) : type_(OBJECT), value_(object) {}

bool JSValue::toBool() const {
    switch (type_) {
        case BOOLEAN:
            return std::any_cast<bool>(value_);
        case NUMBER:
            return std::any_cast<double>(value_) != 0.0;
        case STRING:
            return !std::any_cast<std::string>(value_).empty();
        case ARRAY:
            return !std::any_cast<std::vector<JSValue>>(value_).empty();
        case OBJECT:
            return !std::any_cast<std::unordered_map<std::string, JSValue>>(value_).empty();
        default:
            return false;
    }
}

int JSValue::toInt() const {
    switch (type_) {
        case BOOLEAN:
            return std::any_cast<bool>(value_) ? 1 : 0;
        case NUMBER:
            return static_cast<int>(std::any_cast<double>(value_));
        case STRING:
            try {
                return std::stoi(std::any_cast<std::string>(value_));
            } catch (...) {
                return 0;
            }
        default:
            return 0;
    }
}

double JSValue::toDouble() const {
    switch (type_) {
        case BOOLEAN:
            return std::any_cast<bool>(value_) ? 1.0 : 0.0;
        case NUMBER:
            return std::any_cast<double>(value_);
        case STRING:
            try {
                return std::stod(std::any_cast<std::string>(value_));
            } catch (...) {
                return 0.0;
            }
        default:
            return 0.0;
    }
}

std::string JSValue::toString() const {
    switch (type_) {
        case UNDEFINED:
            return "undefined";
        case NULL_VALUE:
            return "null";
        case BOOLEAN:
            return std::any_cast<bool>(value_) ? "true" : "false";
        case NUMBER: {
            double num = std::any_cast<double>(value_);
            if (num == static_cast<int>(num)) {
                return std::to_string(static_cast<int>(num));
            } else {
                return std::to_string(num);
            }
        }
        case STRING:
            return std::any_cast<std::string>(value_);
        case ARRAY:
            return "[Array]";
        case OBJECT:
            return "[Object]";
        case FUNCTION:
            return "[Function]";
        default:
            return "unknown";
    }
}

std::vector<JSValue> JSValue::toArray() const {
    if (type_ == ARRAY) {
        return std::any_cast<std::vector<JSValue>>(value_);
    }
    return {};
}

std::unordered_map<std::string, JSValue> JSValue::toObject() const {
    if (type_ == OBJECT) {
        return std::any_cast<std::unordered_map<std::string, JSValue>>(value_);
    }
    return {};
}

JSValue& JSValue::operator[](const std::string& key) {
    if (type_ != OBJECT) {
        type_ = OBJECT;
        value_ = std::unordered_map<std::string, JSValue>();
    }
    
    auto& obj = std::any_cast<std::unordered_map<std::string, JSValue>&>(value_);
    return obj[key];
}

const JSValue& JSValue::operator[](const std::string& key) const {
    if (type_ == OBJECT) {
        const auto& obj = std::any_cast<const std::unordered_map<std::string, JSValue>&>(value_);
        auto it = obj.find(key);
        if (it != obj.end()) {
            return it->second;
        }
    }
    
    static JSValue undefined_value;
    return undefined_value;
}

JSValue& JSValue::operator[](size_t index) {
    if (type_ != ARRAY) {
        type_ = ARRAY;
        value_ = std::vector<JSValue>();
    }
    
    auto& arr = std::any_cast<std::vector<JSValue>&>(value_);
    if (index >= arr.size()) {
        arr.resize(index + 1);
    }
    return arr[index];
}

const JSValue& JSValue::operator[](size_t index) const {
    if (type_ == ARRAY) {
        const auto& arr = std::any_cast<const std::vector<JSValue>&>(value_);
        if (index < arr.size()) {
            return arr[index];
        }
    }
    
    static JSValue undefined_value;
    return undefined_value;
}

std::string JSValue::toDebugString() const {
    std::ostringstream oss;
    
    switch (type_) {
        case UNDEFINED:
            oss << "undefined";
            break;
        case NULL_VALUE:
            oss << "null";
            break;
        case BOOLEAN:
            oss << (std::any_cast<bool>(value_) ? "true" : "false");
            break;
        case NUMBER:
            oss << std::any_cast<double>(value_);
            break;
        case STRING:
            oss << "\"" << std::any_cast<std::string>(value_) << "\"";
            break;
        case ARRAY: {
            const auto& arr = std::any_cast<const std::vector<JSValue>&>(value_);
            oss << "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i].toDebugString();
            }
            oss << "]";
            break;
        }
        case OBJECT: {
            const auto& obj = std::any_cast<const std::unordered_map<std::string, JSValue>&>(value_);
            oss << "{";
            bool first = true;
            for (const auto& [key, val] : obj) {
                if (!first) oss << ", ";
                oss << "\"" << key << "\": " << val.toDebugString();
                first = false;
            }
            oss << "}";
            break;
        }
        case FUNCTION:
            oss << "[Function]";
            break;
    }
    
    return oss.str();
}

void JSValue::setUndefined() {
    type_ = UNDEFINED;
    value_ = std::any();
}

// JSContext 实现
JSValue JSContext::getArg(size_t index) const {
    if (index < arguments.size()) {
        return arguments[index];
    }
    return JSValue(); // undefined
}

JSValue JSContext::getArg(size_t index, const JSValue& default_value) const {
    if (index < arguments.size()) {
        return arguments[index];
    }
    return default_value;
}

void JSContext::throwError(const std::string& message) const {
    throw std::runtime_error("[" + module_name + "::" + function_name + "] " + message);
}

void JSContext::logWarning(const std::string& message) const {
    std::cout << "[WARNING] [" << module_name << "::" << function_name << "] " << message << std::endl;
}

void JSContext::logInfo(const std::string& message) const {
    std::cout << "[INFO] [" << module_name << "::" << function_name << "] " << message << std::endl;
}

// CJMODFunction 实现
CJMODFunction::CJMODFunction(const std::string& name, CJMODFunctionType func)
    : name_(name), function_(func) {}

CJMODFunction::CJMODFunction(const std::string& name, CJMODFunctionType func, const std::string& description)
    : name_(name), description_(description), function_(func) {}

CJMODFunction& CJMODFunction::setDescription(const std::string& desc) {
    description_ = desc;
    return *this;
}

CJMODFunction& CJMODFunction::addParameter(const std::string& name, const std::string& type, const std::string& desc) {
    parameters_.push_back({name, type, desc});
    return *this;
}

CJMODFunction& CJMODFunction::setReturnType(const std::string& type, const std::string& desc) {
    return_type_ = type;
    return_description_ = desc;
    return *this;
}

CJMODFunction& CJMODFunction::addExample(const std::string& code, const std::string& desc) {
    examples_.push_back({code, desc});
    return *this;
}

JSValue CJMODFunction::call(const JSContext& context) const {
    try {
        return function_(context);
    } catch (const std::exception& e) {
        helpers::logError("Function '" + name_ + "' failed: " + e.what());
        return helpers::makeError(e.what());
    }
}

std::string CJMODFunction::generateDocumentation() const {
    std::ostringstream doc;
    
    doc << "## " << name_ << "\n\n";
    
    if (!description_.empty()) {
        doc << description_ << "\n\n";
    }
    
    if (!parameters_.empty()) {
        doc << "### 参数\n\n";
        for (const auto& param : parameters_) {
            doc << "- **" << param.name << "** (`" << param.type << "`): " << param.description << "\n";
        }
        doc << "\n";
    }
    
    if (!return_type_.empty()) {
        doc << "### 返回值\n\n";
        doc << "- **类型**: `" << return_type_ << "`\n";
        if (!return_description_.empty()) {
            doc << "- **描述**: " << return_description_ << "\n";
        }
        doc << "\n";
    }
    
    if (!examples_.empty()) {
        doc << "### 示例\n\n";
        for (const auto& example : examples_) {
            if (!example.description.empty()) {
                doc << example.description << "\n\n";
            }
            doc << "```chtl\n" << example.code << "\n```\n\n";
        }
    }
    
    return doc.str();
}

// CJMODModule 实现
CJMODModule::CJMODModule(const std::string& name) : name_(name), version_("1.0.0") {}

CJMODModule::CJMODModule(const std::string& name, const std::string& version) 
    : name_(name), version_(version) {}

CJMODModule& CJMODModule::setDescription(const std::string& desc) {
    description_ = desc;
    return *this;
}

CJMODModule& CJMODModule::setAuthor(const std::string& author) {
    author_ = author;
    return *this;
}

CJMODModule& CJMODModule::setVersion(const std::string& version) {
    version_ = version;
    return *this;
}

CJMODModule& CJMODModule::setHomepage(const std::string& url) {
    homepage_ = url;
    return *this;
}

CJMODModule& CJMODModule::addFunction(const std::string& name, CJMODFunctionType func) {
    functions_[name] = std::make_unique<CJMODFunction>(name, func);
    return *this;
}

CJMODModule& CJMODModule::addFunction(const std::string& name, CJMODFunctionType func, const std::string& description) {
    functions_[name] = std::make_unique<CJMODFunction>(name, func, description);
    return *this;
}

CJMODModule& CJMODModule::functions(std::initializer_list<std::pair<std::string, CJMODFunctionType>> funcs) {
    for (const auto& [name, func] : funcs) {
        addFunction(name, func);
    }
    return *this;
}

bool CJMODModule::hasFunction(const std::string& name) const {
    return functions_.find(name) != functions_.end();
}

CJMODFunction* CJMODModule::getFunction(const std::string& name) const {
    auto it = functions_.find(name);
    return (it != functions_.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> CJMODModule::getFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& [name, func] : functions_) {
        names.push_back(name);
    }
    return names;
}

std::string CJMODModule::generateJavaScript() const {
    std::ostringstream js;
    
    js << "// CJMOD Module: " << name_ << " v" << version_ << "\n";
    if (!description_.empty()) {
        js << "// " << description_ << "\n";
    }
    js << "// Generated by CHTL CJMOD Framework\n\n";
    
    js << "(function() {\n";
    js << "    'use strict';\n\n";
    
    js << "    // Module namespace\n";
    js << "    if (typeof window.CJMOD === 'undefined') {\n";
    js << "        window.CJMOD = {};\n";
    js << "    }\n";
    js << "    window.CJMOD['" << name_ << "'] = {};\n\n";
    
    for (const auto& [func_name, func] : functions_) {
        js << "    // Function: " << func_name << "\n";
        if (!func->getDescription().empty()) {
            js << "    // " << func->getDescription() << "\n";
        }
        js << "    window.CJMOD['" << name_ << "']['" << func_name << "'] = function() {\n";
        js << "        return window.__CJMOD_CALL('" << name_ << "', '" << func_name << "', Array.from(arguments));\n";
        js << "    };\n\n";
    }
    
    js << "})();\n";
    
    return js.str();
}

std::string CJMODModule::generateDocumentation() const {
    std::ostringstream doc;
    
    doc << "# " << name_ << " v" << version_ << "\n\n";
    
    if (!description_.empty()) {
        doc << description_ << "\n\n";
    }
    
    if (!author_.empty()) {
        doc << "**作者**: " << author_ << "\n\n";
    }
    
    if (!homepage_.empty()) {
        doc << "**主页**: " << homepage_ << "\n\n";
    }
    
    doc << "## 函数列表\n\n";
    
    for (const auto& [func_name, func] : functions_) {
        doc << func->generateDocumentation();
    }
    
    return doc.str();
}

// CJMODRegistry 实现
CJMODRegistry& CJMODRegistry::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<CJMODRegistry>(new CJMODRegistry());
    }
    return *instance_;
}

void CJMODRegistry::registerModule(std::unique_ptr<CJMODModule> module) {
    std::string name = module->getName();
    modules_[name] = std::move(module);
    std::cout << "[CJMOD] 注册模块: " << name << std::endl;
}

bool CJMODRegistry::hasModule(const std::string& name) const {
    return modules_.find(name) != modules_.end();
}

CJMODModule* CJMODRegistry::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> CJMODRegistry::getModuleNames() const {
    std::vector<std::string> names;
    for (const auto& [name, module] : modules_) {
        names.push_back(name);
    }
    return names;
}

JSValue CJMODRegistry::callFunction(const std::string& module_name, const std::string& function_name, 
                                   const std::vector<JSValue>& args) const {
    auto module = getModule(module_name);
    if (!module) {
        return helpers::makeError("模块 '" + module_name + "' 未找到");
    }
    
    auto function = module->getFunction(function_name);
    if (!function) {
        return helpers::makeError("函数 '" + function_name + "' 在模块 '" + module_name + "' 中未找到");
    }
    
    JSContext context;
    context.module_name = module_name;
    context.function_name = function_name;
    context.arguments = args;
    
    return function->call(context);
}

std::string CJMODRegistry::generateAllJavaScript() const {
    std::ostringstream js;
    
    js << "// CHTL CJMOD Framework - All Modules\n";
    js << "// Generated automatically\n\n";
    
    // 添加运行时支持
    js << "(function() {\n";
    js << "    'use strict';\n\n";
    js << "    // CJMOD Runtime Support\n";
    js << "    window.__CJMOD_CALL = function(moduleName, functionName, args) {\n";
    js << "        // This will be implemented by the CHTL JS compiler\n";
    js << "        console.log('CJMOD Call:', moduleName, functionName, args);\n";
    js << "        return undefined;\n";
    js << "    };\n\n";
    js << "})();\n\n";
    
    for (const auto& [name, module] : modules_) {
        js << module->generateJavaScript() << "\n";
    }
    
    return js.str();
}

std::string CJMODRegistry::generateModuleJavaScript(const std::string& module_name) const {
    auto module = getModule(module_name);
    if (!module) {
        return "// 模块 '" + module_name + "' 未找到\n";
    }
    
    return module->generateJavaScript();
}

std::string CJMODRegistry::generateAllDocumentation() const {
    std::ostringstream doc;
    
    doc << "# CHTL CJMOD 模块文档\n\n";
    doc << "本文档包含所有已注册的CJMOD模块。\n\n";
    
    for (const auto& [name, module] : modules_) {
        doc << "---\n\n";
        doc << module->generateDocumentation();
    }
    
    return doc.str();
}

std::string CJMODRegistry::generateModuleDocumentation(const std::string& module_name) const {
    auto module = getModule(module_name);
    if (!module) {
        return "# 错误\n\n模块 '" + module_name + "' 未找到。\n";
    }
    
    return module->generateDocumentation();
}

void CJMODRegistry::printRegistryInfo() const {
    std::cout << "=== CJMOD 注册表信息 ===" << std::endl;
    std::cout << "已注册模块数: " << modules_.size() << std::endl;
    std::cout << "总函数数: " << getTotalFunctionCount() << std::endl;
    std::cout << std::endl;
    
    for (const auto& [name, module] : modules_) {
        std::cout << "模块: " << name << " v" << module->getVersion() << std::endl;
        std::cout << "  描述: " << module->getDescription() << std::endl;
        std::cout << "  函数数: " << module->getFunctionNames().size() << std::endl;
        
        auto func_names = module->getFunctionNames();
        if (!func_names.empty()) {
            std::cout << "  函数: ";
            for (size_t i = 0; i < func_names.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << func_names[i];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

size_t CJMODRegistry::getTotalFunctionCount() const {
    size_t total = 0;
    for (const auto& [name, module] : modules_) {
        total += module->getFunctionNames().size();
    }
    return total;
}

// 辅助函数实现
namespace helpers {

bool validateArgCount(const JSContext& ctx, size_t expected) {
    return ctx.getArgCount() == expected;
}

bool validateArgCount(const JSContext& ctx, size_t min, size_t max) {
    size_t count = ctx.getArgCount();
    return count >= min && count <= max;
}

bool validateArgType(const JSContext& ctx, size_t index, JSValue::Type expected) {
    if (index >= ctx.getArgCount()) {
        return false;
    }
    return ctx.getArg(index).getType() == expected;
}

JSValue makeError(const std::string& message) {
    JSValue error;
    error["error"] = JSValue(true);
    error["message"] = JSValue(message);
    return error;
}

JSValue makeSuccess(const JSValue& result) {
    JSValue success;
    success["error"] = JSValue(false);
    success["result"] = result;
    return success;
}

void logDebug(const std::string& message) {
    std::cout << "[DEBUG] " << message << std::endl;
}

void logInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void logWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

void logError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

} // namespace helpers

} // namespace cjmod
} // namespace chtl