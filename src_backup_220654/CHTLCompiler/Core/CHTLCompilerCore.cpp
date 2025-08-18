#include "CHTLCompilerCore.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace chtl {

CHTLCompilerCore::CHTLCompilerCore() : debug_mode_(false) {
    initialize();
}

CHTLCompilerCore::~CHTLCompilerCore() {
    cleanup();
}

bool CHTLCompilerCore::initialize() {
    try {
        // 初始化各组件
        lexer_ = std::make_unique<CHTLLexer>();
        parser_ = std::make_unique<parser::CHTLParser>();
        generator_ = std::make_unique<generator::HTMLGenerator>();
        module_manager_ = std::make_unique<CmodManager>();
        
        if (debug_mode_) {
            std::cout << "CHTL编译器核心初始化成功" << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        addError("初始化失败: " + std::string(e.what()));
        return false;
    }
}

void CHTLCompilerCore::cleanup() {
    lexer_.reset();
    parser_.reset();
    generator_.reset();
    module_manager_.reset();
    
    if (debug_mode_) {
        std::cout << "CHTL编译器核心已清理" << std::endl;
    }
}

void CHTLCompilerCore::reset() {
    clearErrors();
    cleanup();
    initialize();
}

std::string CHTLCompilerCore::compile(const std::string& source_code) {
    if (!lexer_ || !parser_ || !generator_) {
        addError("编译器组件未初始化");
        return "";
    }
    
    try {
        // 1. 词法分析
        lexer_->setInput(source_code);
        auto tokens = lexer_->tokenize();
        
        if (debug_mode_) {
            std::cout << "词法分析完成，生成 " << tokens.size() << " 个token" << std::endl;
        }
        
        // 2. 语法分析
        parser_->setTokens(tokens);
        auto ast = parser_->parseProgram();
        
        if (!ast) {
            addError("语法分析失败");
            return "";
        }
        
        if (debug_mode_) {
            std::cout << "语法分析完成" << std::endl;
        }
        
        // 3. 代码生成
        std::string result = generator_->generateHTML(ast.get());
        
        if (debug_mode_) {
            std::cout << "代码生成完成" << std::endl;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        addError("编译异常: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLCompilerCore::compileFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        addError("无法打开文件: " + file_path);
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    return compile(content.str());
}

void CHTLCompilerCore::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "CHTL编译器错误: " << error << std::endl;
    }
}

void CHTLCompilerCore::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    if (debug_mode_) {
        std::cout << "CHTL编译器警告: " << warning << std::endl;
    }
}

} // namespace chtl