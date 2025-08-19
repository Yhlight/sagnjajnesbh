#include "CHTLJSCompilerCore.h"
#include "../../Common/Core/ErrorHandler.h"
#include <iostream>

namespace chtl {

CHTLJSCompilerCore::CHTLJSCompilerCore() 
    : debug_mode_(false) {
}

CHTLJSCompilerCore::~CHTLJSCompilerCore() = default;

bool CHTLJSCompilerCore::initialize() {
    try {
        // 初始化CHTL JS词法分析器
        lexer_ = std::make_unique<chtljs::CHTLJSLexer>();
        
        // 初始化虚对象管理器
        virtual_object_manager_ = std::make_unique<ast::VirtualObjectManager>();
        
        // 初始化CJMOD管理器
        module_manager_ = std::make_unique<CJmodManager>();
        
        if (debug_mode_) {
            std::cout << "[CHTLJSCompilerCore] 初始化完成" << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("CHTLJSCompilerCore初始化失败: " + std::string(e.what()));
        return false;
    }
}

void CHTLJSCompilerCore::cleanup() {
    lexer_.reset();
    virtual_object_manager_.reset();
    module_manager_.reset();
    
    errors_.clear();
    warnings_.clear();
    
    if (debug_mode_) {
        std::cout << "[CHTLJSCompilerCore] 清理完成" << std::endl;
    }
}

std::string CHTLJSCompilerCore::compile(const std::string& source_code) {
    try {
        if (!lexer_) {
            addError("CHTLJSCompilerCore未初始化");
            return "";
        }
        
        if (debug_mode_) {
            std::cout << "[CHTLJSCompilerCore] 开始编译CHTL JS代码" << std::endl;
        }
        
        // 设置输入
        lexer_->setInput(source_code);
        
        // 词法分析
        auto tokens = lexer_->tokenize();
        
        if (lexer_->getErrors().size() > 0) {
            for (const auto& error : lexer_->getErrors()) {
                addError("CHTL JS词法分析错误: " + error);
            }
            return "";
        }
        
        if (debug_mode_) {
            std::cout << "[CHTLJSCompilerCore] 词法分析完成，生成 " << tokens.size() << " 个token" << std::endl;
        }
        
        // 简单的CHTL JS到JavaScript转换
        std::string javascript_code = generateJavaScript(tokens);
        
        if (debug_mode_) {
            std::cout << "[CHTLJSCompilerCore] JavaScript生成完成" << std::endl;
        }
        
        return javascript_code;
        
    } catch (const std::exception& e) {
        addError("CHTL JS编译错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLJSCompilerCore::generateJavaScript(const std::vector<chtljs::Token>& tokens) {
    std::string js_code;
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        
        // 处理CHTL JS特有语法
        if (token.type == chtljs::TokenType::CHTLJS_SELECTOR) {
            // 处理 {{selector}} 语法
            std::string selector = token.value;
            if (selector.substr(0, 2) == "{{" && selector.substr(selector.length()-2) == "}}") {
                std::string inner = selector.substr(2, selector.length() - 4);
                
                if (inner.length() > 0 && inner[0] == '.') {
                    // 类选择器
                    js_code += "document.querySelectorAll('" + inner + "')";
                } else if (inner.length() > 0 && inner[0] == '#') {
                    // ID选择器
                    js_code += "document.getElementById('" + inner.substr(1) + "')";
                } else if (inner == "&") {
                    // 当前元素
                    js_code += "this";
                } else {
                    // 标签选择器
                    js_code += "document.querySelectorAll('" + inner + "')";
                }
            }
        } else if (token.type == chtljs::TokenType::ARROW_OPERATOR) {
            // 处理 -> 操作符
            js_code += ".";
        } else if (token.value == "listen") {
            // 处理listen函数
            js_code += "addEventListener";
        } else {
            // 其他token直接输出
            js_code += token.value;
            if (token.type == chtljs::TokenType::IDENTIFIER || 
                token.type == chtljs::TokenType::STRING ||
                token.type == chtljs::TokenType::NUMBER) {
                js_code += " ";
            }
        }
    }
    
    return js_code;
}

void CHTLJSCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLJSCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

} // namespace chtl