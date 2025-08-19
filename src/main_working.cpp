#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTLCompiler/Template/TemplateManager.h"
#include "CHTLCompiler/Custom/CustomManager.h"
#include "CHTLCompiler/Origin/OriginManager.h"
#include "CHTLCompiler/Constraint/ConstraintManager.h"
#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace chtl;

// 简单的CHTL编译器实现
class SimpleCHTLCompiler {
public:
    std::string compile(const std::string& chtl_code) {
        try {
            // 1. 词法分析
            CHTLLexer lexer;
            lexer.setInput(chtl_code);
            auto tokens = lexer.tokenize();
            
            if (lexer.getErrors().size() > 0) {
                std::cerr << "词法分析错误:" << std::endl;
                for (const auto& error : lexer.getErrors()) {
                    std::cerr << "  " << error << std::endl;
                }
                return "";
            }
            
            std::cout << "词法分析完成，生成 " << tokens.size() << " 个token" << std::endl;
            
            // 2. 简单的HTML生成
            std::string html = generateBasicHTML(tokens);
            
            return html;
            
        } catch (const std::exception& e) {
            std::cerr << "编译错误: " << e.what() << std::endl;
            return "";
        }
    }
    
private:
    std::string generateBasicHTML(const std::vector<Token>& tokens) {
        std::string html = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL Generated</title>\n</head>\n<body>\n";
        
        // 简单的token到HTML转换
        for (size_t i = 0; i < tokens.size(); i++) {
            const auto& token = tokens[i];
            
            if (token.type == TokenType::IDENTIFIER) {
                // 检查是否是HTML元素
                if (isHTMLElement(token.value)) {
                    html += "<" + token.value;
                    
                    // 查找属性
                    i++;
                    if (i < tokens.size() && tokens[i].type == TokenType::LEFT_BRACE) {
                        i++;
                        while (i < tokens.size() && tokens[i].type != TokenType::RIGHT_BRACE) {
                            if (tokens[i].type == TokenType::IDENTIFIER && 
                                i + 2 < tokens.size() && 
                                (tokens[i+1].type == TokenType::COLON || tokens[i+1].type == TokenType::ASSIGN)) {
                                
                                std::string attr = tokens[i].value;
                                std::string value = tokens[i+2].value;
                                html += " " + attr + "=\"" + value + "\"";
                                i += 3;
                            } else if (tokens[i].value == "text" && 
                                      i + 1 < tokens.size() && 
                                      tokens[i+1].type == TokenType::LEFT_BRACE) {
                                // 处理文本内容
                                i += 2;
                                html += ">";
                                while (i < tokens.size() && tokens[i].type != TokenType::RIGHT_BRACE) {
                                    if (tokens[i].type == TokenType::IDENTIFIER || 
                                        tokens[i].type == TokenType::STRING) {
                                        html += tokens[i].value + " ";
                                    }
                                    i++;
                                }
                                html += "</" + token.value + ">\n";
                                break;
                            } else {
                                i++;
                            }
                        }
                        
                        if (html.back() != '\n') {
                            html += "></" + token.value + ">\n";
                        }
                    }
                }
            }
        }
        
        html += "</body>\n</html>";
        return html;
    }
    
    bool isHTMLElement(const std::string& name) {
        static const std::vector<std::string> html_elements = {
            "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
            "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button",
            "section", "article", "header", "footer", "nav", "main", "aside"
        };
        
        return std::find(html_elements.begin(), html_elements.end(), name) != html_elements.end();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "CHTL编译器 v1.0.0 - 工作版本" << std::endl;
    std::cout << "=============================" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <input.chtl> [output.html]" << std::endl;
        std::cout << "或者: " << argv[0] << " --test" << std::endl;
        return 1;
    }
    
    try {
        SimpleCHTLCompiler compiler;
        
        if (std::string(argv[1]) == "--test") {
            // 测试模式
            std::cout << "\n🧪 测试模式" << std::endl;
            
            std::string test_code = R"(
                html {
                    head {
                        title { text { CHTL Test Page } }
                    }
                    body {
                        div {
                            class: container;
                            h1 { text { Welcome to CHTL } }
                            p { text { This is a test page } }
                        }
                    }
                }
            )";
            
            std::cout << "编译测试代码..." << std::endl;
            std::string result = compiler.compile(test_code);
            
            if (!result.empty()) {
                std::cout << "\n✅ 编译成功！" << std::endl;
                std::cout << "\n生成的HTML:" << std::endl;
                std::cout << "================================" << std::endl;
                std::cout << result << std::endl;
                std::cout << "================================" << std::endl;
                
                // 保存到文件
                std::ofstream out("test_output.html");
                out << result;
                out.close();
                std::cout << "\n📁 输出已保存到: test_output.html" << std::endl;
            } else {
                std::cout << "❌ 编译失败" << std::endl;
                return 1;
            }
            
        } else {
            // 文件编译模式
            std::string input_file = argv[1];
            std::string output_file = (argc > 2) ? argv[2] : (input_file.substr(0, input_file.find_last_of('.')) + ".html");
            
            std::cout << "\n📁 编译文件: " << input_file << std::endl;
            std::cout << "📁 输出文件: " << output_file << std::endl;
            
            // 读取输入文件
            std::ifstream in(input_file);
            if (!in.is_open()) {
                std::cerr << "❌ 无法打开输入文件: " << input_file << std::endl;
                return 1;
            }
            
            std::string chtl_code((std::istreambuf_iterator<char>(in)),
                                  std::istreambuf_iterator<char>());
            in.close();
            
            // 编译
            std::string result = compiler.compile(chtl_code);
            
            if (!result.empty()) {
                // 写入输出文件
                std::ofstream out(output_file);
                out << result;
                out.close();
                
                std::cout << "\n✅ 编译成功！" << std::endl;
                std::cout << "📁 输出已保存到: " << output_file << std::endl;
            } else {
                std::cout << "❌ 编译失败" << std::endl;
                return 1;
            }
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "运行时错误: " << e.what() << std::endl;
        return 1;
    }
}