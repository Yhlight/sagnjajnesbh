#include "real_cjmod.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <cctype>

using namespace CHTL::CJMOD;

// ============================================================================
// 真正的词法分析器实现
// ============================================================================

std::vector<Token> RealDualPointerScanner::tokenize(const std::string& code) {
    std::vector<Token> tokens;
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;
    
    while (pos < code.length()) {
        char ch = code[pos];
        
        // 跳过空白字符
        if (std::isspace(ch)) {
            if (ch == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
            continue;
        }
        
        Token token;
        token.position = pos;
        token.line = line;
        token.column = column;
        
        // 识别标识符和关键字
        if (std::isalpha(ch) || ch == '_') {
            size_t start = pos;
            while (pos < code.length() && (std::isalnum(code[pos]) || code[pos] == '_')) {
                pos++;
                column++;
            }
            token.type = Token::IDENTIFIER;
            token.value = code.substr(start, pos - start);
        }
        // 识别数字
        else if (std::isdigit(ch)) {
            size_t start = pos;
            while (pos < code.length() && (std::isdigit(code[pos]) || code[pos] == '.')) {
                pos++;
                column++;
            }
            token.type = Token::NUMBER;
            token.value = code.substr(start, pos - start);
        }
        // 识别字符串
        else if (ch == '"' || ch == '\'') {
            char quote = ch;
            size_t start = pos;
            pos++; // 跳过开始引号
            column++;
            
            while (pos < code.length() && code[pos] != quote) {
                if (code[pos] == '\\') {
                    pos += 2; // 跳过转义字符
                    column += 2;
                } else {
                    pos++;
                    column++;
                }
            }
            
            if (pos < code.length()) {
                pos++; // 跳过结束引号
                column++;
            }
            
            token.type = Token::STRING;
            token.value = code.substr(start, pos - start);
        }
        // 识别操作符和分隔符
        else {
            switch (ch) {
                case '{': token.type = Token::LBRACE; break;
                case '}': token.type = Token::RBRACE; break;
                case '(': token.type = Token::LPAREN; break;
                case ')': token.type = Token::RPAREN; break;
                case ',': token.type = Token::COMMA; break;
                case ':': token.type = Token::COLON; break;
                case ';': token.type = Token::SEMICOLON; break;
                default: token.type = Token::OPERATOR; break;
            }
            token.value = ch;
            pos++;
            column++;
        }
        
        tokens.push_back(token);
    }
    
    // 添加EOF标记
    Token eof;
    eof.type = Token::EOF_TOKEN;
    eof.position = pos;
    eof.line = line;
    eof.column = column;
    tokens.push_back(eof);
    
    return tokens;
}

// ============================================================================
// 双指针扫描器实现
// ============================================================================

RealDualPointerScanner::RealDualPointerScanner(const std::string& sourceCode) 
    : sourceCode_(sourceCode) {
    tokens_ = tokenize(sourceCode_);
}

void RealDualPointerScanner::setKeywords(const std::vector<std::string>& keywords) {
    keywords_ = keywords;
}

std::vector<RealCodeFragment> RealDualPointerScanner::scan() {
    std::vector<RealCodeFragment> fragments;
    
    // 双指针扫描算法
    size_t frontPointer = 0;
    size_t backPointer = 0;
    
    // 初始扫描范围确定
    while (frontPointer < tokens_.size()) {
        const Token& token = tokens_[frontPointer];
        
        // 检查是否为关键字
        bool isKeyword = false;
        std::string triggerKeyword;
        
        for (const auto& keyword : keywords_) {
            if (token.type == Token::IDENTIFIER && token.value == keyword) {
                isKeyword = true;
                triggerKeyword = keyword;
                break;
            }
        }
        
        if (isKeyword) {
            // 找到关键字，确定代码片段边界
            auto bounds = findFragmentBounds(frontPointer);
            
            RealCodeFragment fragment;
            fragment.sourceCode = sourceCode_;
            fragment.startPos = bounds.first;
            fragment.endPos = bounds.second;
            fragment.triggerKeyword = triggerKeyword;
            
            // 提取片段的tokens
            for (size_t i = 0; i < tokens_.size(); ++i) {
                if (tokens_[i].position >= bounds.first && tokens_[i].position < bounds.second) {
                    fragment.tokens.push_back(tokens_[i]);
                }
            }
            
            fragments.push_back(fragment);
            
            // 移动指针到片段结束位置
            frontPointer = std::distance(tokens_.begin(), 
                std::find_if(tokens_.begin(), tokens_.end(), 
                    [bounds](const Token& t) { return t.position >= bounds.second; }));
            backPointer = frontPointer;
        } else {
            frontPointer++;
        }
    }
    
    return fragments;
}

std::pair<size_t, size_t> RealDualPointerScanner::findFragmentBounds(size_t keywordPos) {
    size_t start = tokens_[keywordPos].position;
    size_t end = start;
    
    // 向前扫描找到完整的函数调用
    size_t pos = keywordPos + 1;
    int braceCount = 0;
    int parenCount = 0;
    
    while (pos < tokens_.size()) {
        const Token& token = tokens_[pos];
        
        if (token.type == Token::LPAREN) {
            parenCount++;
        } else if (token.type == Token::RPAREN) {
            parenCount--;
            if (parenCount == 0) {
                end = token.position + token.value.length();
                break;
            }
        } else if (token.type == Token::LBRACE) {
            braceCount++;
        } else if (token.type == Token::RBRACE) {
            braceCount--;
        } else if (token.type == Token::SEMICOLON && parenCount == 0 && braceCount == 0) {
            end = token.position + token.value.length();
            break;
        }
        
        pos++;
    }
    
    return {start, end};
}

RealCodeFragment RealDualPointerScanner::frontExtract(const RealCodeFragment& fragment, const std::string& pattern) {
    RealCodeFragment extracted = fragment;
    
    // 前置截取：查找模式并截取前面部分
    std::string fragmentCode = fragment.sourceCode.substr(fragment.startPos, 
                                                         fragment.endPos - fragment.startPos);
    
    size_t patternPos = fragmentCode.find(pattern);
    if (patternPos != std::string::npos) {
        // 截取模式之前的部分
        extracted.endPos = fragment.startPos + patternPos;
        
        // 重新分析tokens
        extracted.tokens.clear();
        for (const auto& token : fragment.tokens) {
            if (token.position < extracted.endPos) {
                extracted.tokens.push_back(token);
            }
        }
    }
    
    return extracted;
}

// ============================================================================
// 真正的参数处理实现
// ============================================================================

RealArg::RealArg(const std::string& name, const Token& token) 
    : name_(name), token_(token) {}

std::string RealArg::extractValue() const {
    switch (token_.type) {
        case Token::STRING:
            // 去掉引号
            return token_.value.substr(1, token_.value.length() - 2);
        case Token::NUMBER:
        case Token::IDENTIFIER:
            return token_.value;
        default:
            return "";
    }
}

std::string RealArg::process(const std::function<std::string(const std::string&)>& processor) {
    std::string value = extractValue();
    processedValue_ = processor(value);
    return processedValue_;
}

std::string RealArg::generateJS(const std::string& jsTemplate) const {
    std::string jsCode = jsTemplate;
    
    // 替换模板中的占位符
    size_t pos = jsCode.find("$");
    if (pos != std::string::npos) {
        jsCode.replace(pos, 1, processedValue_.empty() ? extractValue() : processedValue_);
    }
    
    return jsCode;
}

// ============================================================================
// 真正的语法分析实现
// ============================================================================

RealSyntax::RealSyntax(const RealCodeFragment& fragment) 
    : fragment_(fragment), parsed_(false) {}

bool RealSyntax::parse() {
    if (fragment_.tokens.empty()) {
        return false;
    }
    
    // 简化的语法分析：查找函数调用模式
    // functionName({ key: value, key2: value2 })
    
    size_t pos = 0;
    
    // 跳过函数名
    if (pos < fragment_.tokens.size() && fragment_.tokens[pos].type == Token::IDENTIFIER) {
        pos++;
    } else {
        return false;
    }
    
    // 期望左括号
    if (pos < fragment_.tokens.size() && fragment_.tokens[pos].type == Token::LPAREN) {
        pos++;
    } else {
        return false;
    }
    
    // 期望左大括号
    if (pos < fragment_.tokens.size() && fragment_.tokens[pos].type == Token::LBRACE) {
        pos++;
    } else {
        return false;
    }
    
    // 解析键值对
    while (pos < fragment_.tokens.size()) {
        if (fragment_.tokens[pos].type == Token::RBRACE) {
            break;
        }
        
        // 期望键名
        if (fragment_.tokens[pos].type == Token::IDENTIFIER) {
            std::string keyName = fragment_.tokens[pos].value;
            pos++;
            
            // 期望冒号
            if (pos < fragment_.tokens.size() && fragment_.tokens[pos].type == Token::COLON) {
                pos++;
                
                // 期望值
                if (pos < fragment_.tokens.size()) {
                    args_.emplace_back(keyName, fragment_.tokens[pos]);
                    pos++;
                }
            }
        }
        
        // 跳过逗号
        if (pos < fragment_.tokens.size() && fragment_.tokens[pos].type == Token::COMMA) {
            pos++;
        }
    }
    
    parsed_ = true;
    return true;
}

std::vector<RealArg> RealSyntax::extractArgs() {
    if (!parsed_) {
        parse();
    }
    return args_;
}

bool RealSyntax::validate() const {
    return parsed_ && !args_.empty();
}

// ============================================================================
// 真正的CJMOD处理器实现
// ============================================================================

RealCJMODProcessor::RealCJMODProcessor() {}

void RealCJMODProcessor::registerFunction(const std::string& name, 
                                        std::function<std::string(const RealCodeFragment&)> handler) {
    handlers_[name] = handler;
}

std::string RealCJMODProcessor::processFragment(const RealCodeFragment& fragment) {
    auto it = handlers_.find(fragment.triggerKeyword);
    if (it != handlers_.end()) {
        return it->second(fragment);
    }
    
    return "// Unknown CJMOD function: " + fragment.triggerKeyword;
}

void RealCJMODProcessor::setScanner(std::unique_ptr<RealDualPointerScanner> scanner) {
    scanner_ = std::move(scanner);
}

// ============================================================================
// 真正的testFunction实现
// ============================================================================

std::string ProcessTestFunction(const RealCodeFragment& fragment) {
    std::cout << "\n=== 真正的testFunction处理 ===\n";
    std::cout << "代码片段: " << fragment.sourceCode.substr(fragment.startPos, 
                                                       fragment.endPos - fragment.startPos) << "\n";
    
    // 1. 语法分析
    RealSyntax syntax(fragment);
    if (!syntax.parse()) {
        return "// 语法分析失败";
    }
    
    // 2. 提取参数
    auto args = syntax.extractArgs();
    std::cout << "提取到 " << args.size() << " 个参数:\n";
    
    std::string jsCode = "// Generated by testFunction\n";
    
    // 3. 处理每个参数
    for (auto& arg : args) {
        std::string value = arg.extractValue();
        std::cout << "  " << arg.name_ << ": " << value << "\n";
        
        // 4. 应用处理函数
        std::string processed = arg.process([](const std::string& val) -> std::string {
            // 根据参数类型进行处理
            if (val.find_first_not_of("0123456789.") == std::string::npos) {
                return val; // 数字保持原样
            } else {
                return "\"" + val + "\""; // 字符串加引号
            }
        });
        
        // 5. 生成JS代码
        std::string jsTemplate = "const test" + arg.name_ + " = $;\n";
        jsCode += arg.generateJS(jsTemplate);
    }
    
    return jsCode;
}

std::string ProcessSimpleTest(const RealCodeFragment& fragment) {
    std::cout << "\n=== 真正的simpleTest处理 ===\n";
    
    // 简单处理：直接生成console.log
    std::string fragmentCode = fragment.sourceCode.substr(fragment.startPos, 
                                                         fragment.endPos - fragment.startPos);
    
    return "console.log('SimpleTest processed: " + fragmentCode + "');";
}

// ============================================================================
// CJMOD扩展接口实现
// ============================================================================

static RealCJMODProcessor* g_processor = nullptr;

extern "C" {
    void InitializeCJMOD(RealCJMODProcessor* processor) {
        g_processor = processor;
        
        // 注册处理函数
        g_processor->registerFunction("testFunction", ProcessTestFunction);
        g_processor->registerFunction("simpleTest", ProcessSimpleTest);
        
        std::cout << "CJMOD扩展初始化完成\n";
    }
    
    const char* ProcessCJMODFunction(const char* functionName, 
                                   const char* sourceCode,
                                   size_t startPos, 
                                   size_t endPos) {
        if (!g_processor) {
            return "// CJMOD未初始化";
        }
        
        // 构建代码片段
        RealCodeFragment fragment;
        fragment.sourceCode = sourceCode;
        fragment.startPos = startPos;
        fragment.endPos = endPos;
        fragment.triggerKeyword = functionName;
        
        // 词法分析
        RealDualPointerScanner scanner(sourceCode);
        fragment.tokens = scanner.tokenize(std::string(sourceCode).substr(startPos, endPos - startPos));
        
        // 处理片段
        static std::string result;
        result = g_processor->processFragment(fragment);
        
        return result.c_str();
    }
    
    void CleanupCJMOD() {
        g_processor = nullptr;
        std::cout << "CJMOD扩展清理完成\n";
    }
}