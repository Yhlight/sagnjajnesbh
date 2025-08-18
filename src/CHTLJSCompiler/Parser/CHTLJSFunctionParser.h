#pragma once
#include "../Lexer/CHTLJSToken.h"
#include "../Core/JSEventRegistry.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS函数解析器
 * 支持无序键值对和可选键值对的解析
 */
class CHTLJSFunctionParser {
public:
    // 解析结果
    struct ParsedFunction {
        std::string function_name;      // listen, delegate, animate等
        std::vector<CHTLJSKeyValueManager::KeyValueInfo> key_value_pairs;
        bool is_valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ParsedFunction() : is_valid(false) {}
    };
    
    CHTLJSFunctionParser();
    ~CHTLJSFunctionParser() = default;
    
    // 主解析接口
    ParsedFunction parseFunction(const std::string& function_call);
    ParsedFunction parseFunctionFromTokens(const std::vector<Token>& tokens);
    
    // 键值对解析
    std::vector<CHTLJSKeyValueManager::KeyValueInfo> parseKeyValueBlock(const std::string& block_content);
    bool validateKeyValuePairs(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name);
    
    // 无序键值对支持
    std::vector<CHTLJSKeyValueManager::KeyValueInfo> normalizeKeyOrder(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, 
                                                                       const std::string& function_name);
    
    // 可选键值对支持
    std::vector<CHTLJSKeyValueManager::KeyValueInfo> validateOptionalKeys(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs,
                                                                          const std::string& function_name);
    bool checkRequiredKeys(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name);
    
    // 代码生成
    std::string generateOptimizedCode(const ParsedFunction& parsed_function);
    std::string generateListenCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs);
    std::string generateDelegateCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs);
    std::string generateAnimateCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs);
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearErrors() { errors_.clear(); warnings_.clear(); }
    
    // 配置
    void setStrictMode(bool strict) { strict_mode_ = strict; }
    bool isStrictMode() const { return strict_mode_; }
    
private:
    std::unique_ptr<CHTLJSKeyValueManager> kv_manager_;
    JSEventRegistry* event_registry_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    bool strict_mode_;
    
    // 内部解析方法
    std::string extractFunctionName(const std::string& function_call);
    std::string extractKeyValueBlock(const std::string& function_call);
    bool isValidFunctionName(const std::string& name);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 验证方法
    bool validateEventKey(const std::string& key);
    bool validateFunctionValue(const std::string& value);
    
    // 代码生成辅助
    std::string generateEventBinding(const std::string& event, const std::string& handler);
    std::string optimizeFunctionReference(const std::string& function_ref);
};

} // namespace chtljs
} // namespace chtl