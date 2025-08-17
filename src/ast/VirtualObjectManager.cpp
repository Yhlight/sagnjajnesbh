#include "VirtualObjectManager.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace ast {

VirtualObjectManager::VirtualObjectManager() = default;

VirtualObjectManager::~VirtualObjectManager() = default;

bool VirtualObjectManager::registerVirtualObject(const std::string& vir_name, const std::string& source_function) {
    if (vir_name.empty() || hasVirtualObject(vir_name)) {
        setError("虚对象名称为空或已存在: " + vir_name);
        return false;
    }
    
    auto vir_info = std::make_unique<VirtualObjectInfo>();
    vir_info->name = vir_name;
    vir_info->source_function = source_function;
    vir_info->is_processed = false;
    
    virtual_objects_[vir_name] = std::move(vir_info);
    return true;
}

bool VirtualObjectManager::addFunctionToVirtualObject(const std::string& vir_name, const std::string& key, 
                                                     const std::string& function_content) {
    auto vir_obj = getVirtualObject(vir_name);
    if (!vir_obj) {
        setError("虚对象不存在: " + vir_name);
        return false;
    }
    
    FunctionInfo func_info;
    func_info.name = key;
    func_info.type = detectFunctionType(function_content);
    func_info.content = function_content;
    func_info.is_generated = false;
    
    // 处理不同类型的函数
    switch (func_info.type) {
        case FunctionType::INLINE_ARROW:
        case FunctionType::INLINE_FUNCTION:
            // 内联函数：需要生成新的全局函数
            func_info.generated_name = generateUniqueFunctionName(vir_name, key);
            break;
            
        case FunctionType::REFERENCE:
            // 函数引用：检查函数是否存在
            func_info.reference_name = function_content;
            if (!isFunctionExists(function_content)) {
                std::cout << "警告: 引用的函数可能不存在: " << function_content << std::endl;
            }
            break;
            
        default:
            setError("无法识别的函数类型: " + function_content);
            return false;
    }
    
    vir_obj->functions[key] = func_info;
    return true;
}

FunctionType VirtualObjectManager::detectFunctionType(const std::string& function_content) {
    // 去除前后空白
    std::string trimmed = function_content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    // 检查是否是箭头函数
    if (isArrowFunction(trimmed)) {
        return FunctionType::INLINE_ARROW;
    }
    
    // 检查是否是普通函数
    if (isRegularFunction(trimmed)) {
        return FunctionType::INLINE_FUNCTION;
    }
    
    // 检查是否是简单标识符（函数引用）
    if (isSimpleIdentifier(trimmed)) {
        return FunctionType::REFERENCE;
    }
    
    return FunctionType::UNKNOWN;
}

bool VirtualObjectManager::isInlineFunction(const std::string& content) {
    FunctionType type = detectFunctionType(content);
    return type == FunctionType::INLINE_ARROW || type == FunctionType::INLINE_FUNCTION;
}

bool VirtualObjectManager::isFunctionReference(const std::string& content) {
    return detectFunctionType(content) == FunctionType::REFERENCE;
}

bool VirtualObjectManager::registerExistingFunction(const std::string& function_name) {
    if (function_name.empty()) return false;
    
    existing_functions_.insert(function_name);
    return true;
}

bool VirtualObjectManager::isFunctionExists(const std::string& function_name) {
    return existing_functions_.find(function_name) != existing_functions_.end();
}

std::vector<std::string> VirtualObjectManager::getExistingFunctions() const {
    return std::vector<std::string>(existing_functions_.begin(), existing_functions_.end());
}

std::string VirtualObjectManager::generateVirtualObjectCode(const std::string& vir_name) {
    auto vir_obj = getVirtualObject(vir_name);
    if (!vir_obj) {
        setError("虚对象不存在: " + vir_name);
        return "";
    }
    
    std::ostringstream code;
    
    // 生成虚对象的函数包装器
    for (const auto& [key, func_info] : vir_obj->functions) {
        // 为所有函数生成包装器（内联函数生成新代码，引用函数生成引用）
        std::string wrapper_code = generateFunctionWrapper(*vir_obj, key);
        code << wrapper_code << "\n";
    }
    
    vir_obj->is_processed = true;
    return code.str();
}

std::string VirtualObjectManager::generateFunctionWrapper(const VirtualObjectInfo& vir_info, const std::string& key) {
    auto it = vir_info.functions.find(key);
    if (it == vir_info.functions.end()) {
        return "";
    }
    
    const FunctionInfo& func_info = it->second;
    std::ostringstream wrapper;
    
    switch (func_info.type) {
        case FunctionType::INLINE_ARROW:
        case FunctionType::INLINE_FUNCTION:
            // 生成新的全局函数
            wrapper << "function " << func_info.generated_name << "() {\n";
            wrapper << "    return (" << func_info.content << ")();\n";  // 正确调用内联函数
            wrapper << "}\n";
            
            // 生成虚对象访问器
            wrapper << "const " << vir_info.name << "_" << key << " = " << func_info.generated_name << ";\n";
            break;
            
        case FunctionType::REFERENCE:
            // 只生成引用，不重复生成函数体
            wrapper << "const " << vir_info.name << "_" << key << " = " << func_info.reference_name << ";\n";
            break;
            
        default:
            break;
    }
    
    return wrapper.str();
}

bool VirtualObjectManager::shouldGenerateFunction(const FunctionInfo& func_info) {
    // 只有内联函数需要生成新代码
    return func_info.type == FunctionType::INLINE_ARROW || 
           func_info.type == FunctionType::INLINE_FUNCTION;
}

std::string VirtualObjectManager::generateUniqueFunctionName(const std::string& vir_name, const std::string& key) {
    std::string base_name = sanitizeFunctionName(vir_name + "_" + key);
    std::string unique_name = generateSafeFunctionName(base_name);
    
    markGlobalFunctionNameUsed(unique_name);
    return unique_name;
}

bool VirtualObjectManager::isGlobalFunctionNameUsed(const std::string& name) {
    return generated_function_names_.find(name) != generated_function_names_.end();
}

void VirtualObjectManager::markGlobalFunctionNameUsed(const std::string& name) {
    generated_function_names_.insert(name);
}

VirtualObjectInfo* VirtualObjectManager::getVirtualObject(const std::string& vir_name) {
    auto it = virtual_objects_.find(vir_name);
    return it != virtual_objects_.end() ? it->second.get() : nullptr;
}

std::vector<std::string> VirtualObjectManager::getVirtualObjectNames() const {
    std::vector<std::string> names;
    names.reserve(virtual_objects_.size());
    
    for (const auto& [name, vir_obj] : virtual_objects_) {
        names.push_back(name);
    }
    
    return names;
}

bool VirtualObjectManager::hasVirtualObject(const std::string& vir_name) {
    return virtual_objects_.find(vir_name) != virtual_objects_.end();
}

VirtualObjectManager::VirtualCall VirtualObjectManager::parseVirtualCall(const std::string& call_expression) {
    VirtualCall call;
    call.is_valid = false;
    
    // 解析 vir_name->function_key() 格式
    size_t arrow_pos = call_expression.find("->");
    if (arrow_pos == std::string::npos) {
        return call;
    }
    
    call.vir_name = call_expression.substr(0, arrow_pos);
    std::string function_part = call_expression.substr(arrow_pos + 2);
    
    // 提取函数名（去除括号）
    size_t paren_pos = function_part.find('(');
    if (paren_pos != std::string::npos) {
        call.function_key = function_part.substr(0, paren_pos);
    } else {
        call.function_key = function_part;
    }
    
    // 解析虚对象调用
    call.resolved_function_name = resolveVirtualCall(call.vir_name, call.function_key);
    call.is_valid = !call.resolved_function_name.empty();
    
    return call;
}

std::string VirtualObjectManager::resolveVirtualCall(const std::string& vir_name, const std::string& function_key) {
    auto vir_obj = getVirtualObject(vir_name);
    if (!vir_obj) {
        return "";
    }
    
    auto it = vir_obj->functions.find(function_key);
    if (it == vir_obj->functions.end()) {
        return "";
    }
    
    const FunctionInfo& func_info = it->second;
    
    switch (func_info.type) {
        case FunctionType::INLINE_ARROW:
        case FunctionType::INLINE_FUNCTION:
            return func_info.generated_name;
            
        case FunctionType::REFERENCE:
            return func_info.reference_name;
            
        default:
            return "";
    }
}

std::vector<std::string> VirtualObjectManager::analyzeFunctionDependencies(const std::string& function_content) {
    return VirtualObjectUtils::extractFunctionCalls(function_content);
}

bool VirtualObjectManager::hasFunctionDependency(const std::string& function_name) {
    return isFunctionExists(function_name);
}

void VirtualObjectManager::clear() {
    virtual_objects_.clear();
    existing_functions_.clear();
    generated_function_names_.clear();
    function_name_mapping_.clear();
}

void VirtualObjectManager::clearVirtualObject(const std::string& vir_name) {
    virtual_objects_.erase(vir_name);
}

void VirtualObjectManager::printVirtualObjects() const {
    std::cout << "=== 虚对象列表 ===" << std::endl;
    
    for (const auto& [name, vir_obj] : virtual_objects_) {
        std::cout << "虚对象: " << name << " (源函数: " << vir_obj->source_function << ")" << std::endl;
        
        for (const auto& [key, func_info] : vir_obj->functions) {
            std::cout << "  函数: " << key << " (类型: ";
            switch (func_info.type) {
                case FunctionType::INLINE_ARROW: std::cout << "内联箭头函数"; break;
                case FunctionType::INLINE_FUNCTION: std::cout << "内联普通函数"; break;
                case FunctionType::REFERENCE: std::cout << "函数引用"; break;
                default: std::cout << "未知"; break;
            }
            std::cout << ")" << std::endl;
            
            if (func_info.type == FunctionType::REFERENCE) {
                std::cout << "    引用: " << func_info.reference_name << std::endl;
            } else if (!func_info.generated_name.empty()) {
                std::cout << "    生成名: " << func_info.generated_name << std::endl;
            }
        }
    }
}

std::string VirtualObjectManager::getStatistics() const {
    std::ostringstream stats;
    
    stats << "虚对象总数: " << virtual_objects_.size() << std::endl;
    stats << "已注册函数总数: " << existing_functions_.size() << std::endl;
    stats << "已生成函数名总数: " << generated_function_names_.size() << std::endl;
    
    int inline_functions = 0;
    int reference_functions = 0;
    
    for (const auto& [name, vir_obj] : virtual_objects_) {
        for (const auto& [key, func_info] : vir_obj->functions) {
            if (func_info.type == FunctionType::REFERENCE) {
                reference_functions++;
            } else {
                inline_functions++;
            }
        }
    }
    
    stats << "内联函数总数: " << inline_functions << std::endl;
    stats << "引用函数总数: " << reference_functions << std::endl;
    
    return stats.str();
}

// 私有方法实现
std::string VirtualObjectManager::extractFunctionName(const std::string& content) {
    // 简化实现：提取函数名
    std::string trimmed = content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    
    if (isSimpleIdentifier(trimmed)) {
        return trimmed;
    }
    
    return "";
}

bool VirtualObjectManager::isArrowFunction(const std::string& content) {
    return content.find("=>") != std::string::npos;
}

bool VirtualObjectManager::isRegularFunction(const std::string& content) {
    return content.find("function") == 0;
}

bool VirtualObjectManager::isSimpleIdentifier(const std::string& content) {
    if (content.empty()) return false;
    
    // 检查是否是简单的标识符（函数名）
    for (char c : content) {
        if (!std::isalnum(c) && c != '_' && c != '$') {
            return false;
        }
    }
    
    // 不应该是数字开头
    return !std::isdigit(content[0]);
}

std::string VirtualObjectManager::sanitizeFunctionName(const std::string& name) {
    std::string sanitized;
    
    for (char c : name) {
        if (std::isalnum(c) || c == '_') {
            sanitized += c;
        } else {
            sanitized += '_';
        }
    }
    
    // 确保不以数字开头
    if (!sanitized.empty() && std::isdigit(sanitized[0])) {
        sanitized = "_" + sanitized;
    }
    
    return sanitized;
}

std::string VirtualObjectManager::generateSafeFunctionName(const std::string& base_name) {
    std::string name = base_name;
    int counter = 0;
    
    while (isGlobalFunctionNameUsed(name)) {
        name = base_name + "_" + std::to_string(++counter);
    }
    
    return name;
}

void VirtualObjectManager::setError(const std::string& error) {
    last_error_ = error;
    std::cerr << "VirtualObjectManager错误: " << error << std::endl;
}

// VirtualObjectUtils命名空间实现
namespace VirtualObjectUtils {

std::pair<std::string, std::string> parseFunctionSignature(const std::string& content) {
    // 简化实现：解析函数签名
    if (content.find("=>") != std::string::npos) {
        // 箭头函数
        size_t arrow_pos = content.find("=>");
        std::string params = content.substr(0, arrow_pos);
        std::string body = content.substr(arrow_pos + 2);
        
        // 去除括号
        if (params.front() == '(' && params.back() == ')') {
            params = params.substr(1, params.length() - 2);
        }
        
        return {params, body};
    }
    
    if (content.find("function") == 0) {
        // 普通函数
        size_t paren_start = content.find('(');
        size_t paren_end = content.find(')', paren_start);
        size_t brace_start = content.find('{', paren_end);
        
        if (paren_start != std::string::npos && paren_end != std::string::npos && brace_start != std::string::npos) {
            std::string params = content.substr(paren_start + 1, paren_end - paren_start - 1);
            std::string body = content.substr(brace_start);
            return {params, body};
        }
    }
    
    return {"", ""};
}

std::string extractFunctionBody(const std::string& content) {
    auto [params, body] = parseFunctionSignature(content);
    return body;
}

std::string generateFunctionWrapper(const std::string& original_name, 
                                   const std::string& wrapper_name,
                                   const std::string& parameters) {
    std::ostringstream wrapper;
    
    wrapper << "function " << wrapper_name << "(" << parameters << ") {\n";
    wrapper << "    return " << original_name << ".apply(this, arguments);\n";
    wrapper << "}\n";
    
    return wrapper.str();
}

bool isValidJavaScriptIdentifier(const std::string& name) {
    if (name.empty()) return false;
    
    // 检查首字符
    if (!std::isalpha(name[0]) && name[0] != '_' && name[0] != '$') {
        return false;
    }
    
    // 检查其余字符
    for (size_t i = 1; i < name.length(); ++i) {
        char c = name[i];
        if (!std::isalnum(c) && c != '_' && c != '$') {
            return false;
        }
    }
    
    return !isReservedKeyword(name);
}

bool isReservedKeyword(const std::string& name) {
    static const std::unordered_set<std::string> keywords = {
        "break", "case", "catch", "class", "const", "continue", "debugger", "default",
        "delete", "do", "else", "enum", "export", "extends", "false", "finally",
        "for", "function", "if", "import", "in", "instanceof", "let", "new",
        "null", "return", "super", "switch", "this", "throw", "true", "try",
        "typeof", "var", "void", "while", "with", "yield"
    };
    
    return keywords.find(name) != keywords.end();
}

std::vector<std::string> extractFunctionCalls(const std::string& code) {
    std::vector<std::string> calls;
    
    // 简化实现：查找函数调用模式
    std::regex call_pattern(R"(\b([a-zA-Z_$][a-zA-Z0-9_$]*)\s*\()");
    std::sregex_iterator iter(code.begin(), code.end(), call_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        calls.push_back((*iter)[1].str());
    }
    
    return calls;
}

std::vector<std::string> extractVariableReferences(const std::string& code) {
    std::vector<std::string> vars;
    
    // 简化实现：查找变量引用模式
    std::regex var_pattern(R"(\b([a-zA-Z_$][a-zA-Z0-9_$]*)\b)");
    std::sregex_iterator iter(code.begin(), code.end(), var_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string var_name = (*iter)[1].str();
        if (!isReservedKeyword(var_name)) {
            vars.push_back(var_name);
        }
    }
    
    return vars;
}

} // namespace VirtualObjectUtils

} // namespace ast
} // namespace chtl