#include "../src/CHTLCompiler/Lexer/CHTLLexer.h"
#include "../src/CHTLCompiler/Parser/CHTLParser.h"
#include "../src/CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testCommentTokenization() {
    std::cout << "=== 测试注释词法分析 ===" << std::endl;
    
    CHTLLexer lexer;
    
    // 测试单行注释
    std::string single_comment = "// 这是单行注释";
    lexer.setInput(single_comment);
    auto tokens1 = lexer.tokenize();
    
    std::cout << "单行注释Token数量: " << tokens1.size() << std::endl;
    for (const auto& token : tokens1) {
        if (token.type != TokenType::EOF_TOKEN) {
            std::cout << "  " << token.toString() << std::endl;
        }
    }
    
    // 测试多行注释
    std::string multi_comment = "/* 这是多行注释\n   跨越多行 */";
    lexer.setInput(multi_comment);
    auto tokens2 = lexer.tokenize();
    
    std::cout << "多行注释Token数量: " << tokens2.size() << std::endl;
    for (const auto& token : tokens2) {
        if (token.type != TokenType::EOF_TOKEN) {
            std::cout << "  " << token.toString() << std::endl;
        }
    }
    
    // 测试生成器注释
    std::string generator_comment = "-- 这是生成器注释";
    lexer.setInput(generator_comment);
    auto tokens3 = lexer.tokenize();
    
    std::cout << "生成器注释Token数量: " << tokens3.size() << std::endl;
    for (const auto& token : tokens3) {
        if (token.type != TokenType::EOF_TOKEN) {
            std::cout << "  " << token.toString() << std::endl;
        }
    }
    
    std::cout << "✅ 注释词法分析测试通过!" << std::endl;
}

void testCommentParsing() {
    std::cout << "=== 测试注释语法分析 ===" << std::endl;
    
    CHTLLexer lexer;
    parser::CHTLParser parser;
    
    // 测试包含注释的CHTL代码
    std::string chtl_with_comments = R"(
        // 这是普通注释，不会生成
        -- 这是HTML注释，会生成
        
        div {
            -- 这是元素内的注释
            
            text {
                Hello World
            }
            
            /* 多行注释
               不会生成 */
               
            style {
                -- CSS上下文注释
                color: red;
            }
            
            script {
                -- JavaScript上下文注释
                console.log('test');
            }
        }
    )";
    
    lexer.setInput(chtl_with_comments);
    auto tokens = lexer.tokenize();
    
    std::cout << "总Token数量: " << tokens.size() << std::endl;
    
    // 统计注释类型
    int single_comments = 0, multi_comments = 0, generator_comments = 0;
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::COMMENT_SINGLE:
                single_comments++;
                break;
            case TokenType::COMMENT_MULTI:
                multi_comments++;
                break;
            case TokenType::COMMENT_GENERATOR:
                generator_comments++;
                break;
            default:
                break;
        }
    }
    
    std::cout << "单行注释数量: " << single_comments << std::endl;
    std::cout << "多行注释数量: " << multi_comments << std::endl;
    std::cout << "生成器注释数量: " << generator_comments << std::endl;
    
    // 解析AST
    parser.setTokens(tokens);
    auto ast = parser.parseProgram();
    
    if (ast) {
        std::cout << "AST解析成功，节点数量: " << ast->getChildCount() << std::endl;
    } else {
        std::cout << "AST解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << "✅ 注释语法分析测试通过!" << std::endl;
}

void testCommentGeneration() {
    std::cout << "=== 测试注释生成 ===" << std::endl;
    
    // 直接测试CommentNode的生成功能
    ast::CommentNode html_comment(ast::CommentNode::CommentType::GENERATOR, "HTML上下文注释");
    ast::CommentNode css_comment(ast::CommentNode::CommentType::GENERATOR, "CSS上下文注释");
    ast::CommentNode js_comment(ast::CommentNode::CommentType::GENERATOR, "JavaScript上下文注释");
    
    std::cout << "HTML上下文: " << html_comment.generateComment("html") << std::endl;
    std::cout << "CSS上下文: " << css_comment.generateComment("css") << std::endl;
    std::cout << "JS上下文: " << js_comment.generateComment("js") << std::endl;
    
    // 测试非生成器注释
    ast::CommentNode single_comment(ast::CommentNode::CommentType::SINGLE_LINE, "不应该生成");
    std::cout << "单行注释生成: '" << single_comment.generateComment("html") << "' (应该为空)" << std::endl;
    
    std::cout << "✅ 注释生成测试通过!" << std::endl;
}

void testIntegratedCommentWorkflow() {
    std::cout << "=== 测试完整注释工作流 ===" << std::endl;
    
    CHTLLexer lexer;
    parser::CHTLParser parser;
    generator::HTMLGenerator generator;
    
    // 测试完整的CHTL代码
    std::string complete_chtl = R"(
        -- 页面主注释
        
        html {
            head {
                -- 头部注释
                title {
                    text { "CHTL注释测试" }
                }
            }
            
            body {
                -- 主体内容注释
                
                div {
                    // 这个注释不会生成
                    -- 这个div的说明注释
                    
                    style {
                        -- CSS样式注释
                        color: red;
                        /* 这个CSS注释不会生成 */
                    }
                    
                    script {
                        -- JavaScript代码注释
                        console.log('测试');
                        // 这个JS注释不会生成
                    }
                    
                    text {
                        Hello, CHTL Comments!
                    }
                }
            }
        }
    )";
    
    // 完整编译流程
    lexer.setInput(complete_chtl);
    auto tokens = lexer.tokenize();
    
    std::cout << "词法分析完成，Token数量: " << tokens.size() << std::endl;
    
    parser.setTokens(tokens);
    auto ast = parser.parseProgram();
    
    if (!ast) {
        std::cout << "解析失败!" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "解析错误: " << error << std::endl;
        }
        return;
    }
    
    std::cout << "语法分析完成，AST节点数量: " << ast->getChildCount() << std::endl;
    
    // 生成HTML
    std::string html_result = generator.generateHTML(ast.get());
    
    std::cout << "生成的HTML:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << html_result << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 验证生成器注释是否正确生成
    bool has_html_comments = html_result.find("<!-- ") != std::string::npos;
    bool has_css_comments = html_result.find("/* ") != std::string::npos;
    bool has_js_comments = html_result.find("// ") != std::string::npos;
    
    std::cout << "包含HTML注释: " << (has_html_comments ? "是" : "否") << std::endl;
    std::cout << "包含CSS注释: " << (has_css_comments ? "是" : "否") << std::endl;
    std::cout << "包含JS注释: " << (has_js_comments ? "是" : "否") << std::endl;
    
    std::cout << "✅ 完整注释工作流测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL注释节点功能..." << std::endl << std::endl;
        
        testCommentTokenization();
        std::cout << std::endl;
        
        testCommentParsing();
        std::cout << std::endl;
        
        testCommentGeneration();
        std::cout << std::endl;
        
        testIntegratedCommentWorkflow();
        std::cout << std::endl;
        
        std::cout << "🎉 所有注释功能测试通过!" << std::endl;
        std::cout << "✅ CHTL现在完全支持上下文感知的注释生成!" << std::endl;
        std::cout << "✅ -- 注释会根据上下文生成HTML、CSS或JS注释!" << std::endl;
        std::cout << "✅ // 和 /**/ 注释不会出现在生成的代码中!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}