#include "../src/Shared/AST/CommentNode.h"
#include "../src/Shared/AST/CommentGenerator.h"
#include "../src/Shared/AST/OriginEmbedDetector.h"
#include <iostream>
#include <cassert>

using namespace chtl::ast;

void testOriginEmbedDetection() {
    std::cout << "Testing OriginEmbed detection..." << std::endl;
    
    OriginEmbedDetector detector;
    
    // 测试HTML原始嵌入
    std::string code_with_html = R"(
@Html {
    text { Hello World }
}

[Origin] @Html
{
    <!-- HTML注释，应该保留 -->
    -- 这不是CHTL生成器注释，是HTML内容
    <div>内容</div>
}

// 这是CHTL注释，应该被处理
)";
    
    auto blocks = detector.detectOriginEmbeds(code_with_html);
    assert(!blocks.empty());
    
    // 应该检测到一个原始嵌入块
    assert(blocks.size() == 1);
    assert(blocks[0].type == "@Html");
    assert(!blocks[0].is_reference);
    
    // 测试位置检查
    size_t html_comment_pos = code_with_html.find("<!-- HTML注释");
    size_t generator_comment_pos = code_with_html.find("-- 这不是CHTL");
    size_t chtl_comment_pos = code_with_html.find("// 这是CHTL注释");
    
    assert(detector.isInsideOriginEmbed(code_with_html, html_comment_pos));
    assert(detector.isInsideOriginEmbed(code_with_html, generator_comment_pos));
    assert(!detector.isInsideOriginEmbed(code_with_html, chtl_comment_pos));
    
    std::cout << "OriginEmbed detection tests passed!" << std::endl;
}

void testCommentParsingWithOriginEmbeds() {
    std::cout << "Testing comment parsing with OriginEmbeds..." << std::endl;
    
    CommentParser parser;
    
    std::string mixed_code = R"(
// CHTL注释1 - 应该被解析
@Html {
    text { Hello }
}

[Origin] @Html
{
    <!-- HTML注释，不应该被CHTL处理 -->
    -- 这不是CHTL生成器注释
    // 这不是CHTL JS注释
    /* 这不是CHTL多行注释 */
    <div>内容</div>
}

// CHTL注释2 - 应该被解析
-- CHTL生成器注释 - 应该被解析

[Origin] @JavaScript
{
    // JS注释，不应该被CHTL处理
    function test() {
        console.log('test');
    }
}
)";
    
    auto comments = parser.extractComments(mixed_code);
    
    // 应该只解析到CHTL区域的注释，不包括原始嵌入内部的注释
    std::cout << "Found " << comments.size() << " comments" << std::endl;
    
    bool found_chtl_comment1 = false;
    bool found_chtl_comment2 = false;
    bool found_generator_comment = false;
    bool found_origin_html_comment = false;
    bool found_origin_generator_comment = false;
    bool found_origin_js_comment = false;
    
    for (const auto& comment : comments) {
        std::string content = comment->getContent();
        std::cout << "Comment: '" << content << "' Type: " << comment->getTypeString() << std::endl;
        
        if (content.find("CHTL注释1") != std::string::npos) {
            found_chtl_comment1 = true;
        } else if (content.find("CHTL注释2") != std::string::npos) {
            found_chtl_comment2 = true;
        } else if (content.find("CHTL生成器注释") != std::string::npos) {
            found_generator_comment = true;
        } else if (content.find("HTML注释") != std::string::npos) {
            found_origin_html_comment = true;
        } else if (content.find("这不是CHTL生成器注释") != std::string::npos) {
            found_origin_generator_comment = true;
        } else if (content.find("JS注释") != std::string::npos) {
            found_origin_js_comment = true;
        }
    }
    
    // 验证结果
    assert(found_chtl_comment1);      // 应该找到
    assert(found_chtl_comment2);      // 应该找到  
    assert(found_generator_comment);  // 应该找到
    assert(!found_origin_html_comment);     // 不应该找到（在原始嵌入内）
    assert(!found_origin_generator_comment); // 不应该找到（在原始嵌入内）
    assert(!found_origin_js_comment);       // 不应该找到（在原始嵌入内）
    
    std::cout << "Comment parsing with OriginEmbeds tests passed!" << std::endl;
}

void testOriginEmbedReference() {
    std::cout << "Testing OriginEmbed references..." << std::endl;
    
    OriginEmbedDetector detector;
    
    std::string code_with_ref = R"(
[Origin] @Html box
{
    <div>原始HTML内容</div>
    -- 不是CHTL注释
}

body {
    [Origin] @Html box;  // 这是引用，不是定义
    // 这是CHTL注释，应该被处理
}
)";
    
    auto blocks = detector.detectOriginEmbeds(code_with_ref);
    assert(blocks.size() == 2); // 一个定义，一个引用
    
    // 第一个应该是定义
    assert(!blocks[0].is_reference);
    assert(blocks[0].type == "@Html");
    assert(blocks[0].name == "box");
    
    // 第二个应该是引用
    assert(blocks[1].is_reference);
    assert(blocks[1].type == "@Html");
    assert(blocks[1].name == "box");
    
    std::cout << "OriginEmbed reference tests passed!" << std::endl;
}

void testComplexOriginEmbedScenario() {
    std::cout << "Testing complex OriginEmbed scenario..." << std::endl;
    
    CommentParser parser;
    
    std::string complex_code = R"(
-- 全局生成器注释（应该被处理）

@Html {
    // CHTL JS注释（应该被处理）
    text { 
        {{button}}->click({ 
            action: function() {
                // 内联JS注释（应该被处理，因为在CHTL JS中）
                alert('test');
            }
        })
    }
}

[Origin] @Style
{
    /* CSS注释，不应该被CHTL处理 */
    .button {
        color: red;
        -- 这看起来像生成器注释，但实际上是CSS内容
    }
}

[Origin] @JavaScript
{
    // JS注释，不应该被CHTL处理
    function test() {
        /* 多行JS注释，不应该被CHTL处理 */
        console.log('test');
    }
}

-- 另一个全局生成器注释（应该被处理）
)";
    
    auto comments = parser.extractComments(complex_code);
    
    std::cout << "Found " << comments.size() << " comments in complex scenario:" << std::endl;
    
    int chtl_comments = 0;
    int origin_comments = 0;
    
    for (const auto& comment : comments) {
        std::string content = comment->getContent();
        std::cout << "  - '" << content << "' (" << comment->getTypeString() << ")" << std::endl;
        
        if (content.find("全局生成器注释") != std::string::npos ||
            content.find("CHTL JS注释") != std::string::npos ||
            content.find("内联JS注释") != std::string::npos) {
            chtl_comments++;
        } else if (content.find("CSS注释") != std::string::npos ||
                   content.find("这看起来像生成器注释") != std::string::npos ||
                   content.find("JS注释") != std::string::npos ||
                   content.find("多行JS注释") != std::string::npos) {
            origin_comments++;
        }
    }
    
    std::cout << "CHTL comments: " << chtl_comments << std::endl;
    std::cout << "Origin comments (should be 0): " << origin_comments << std::endl;
    
    assert(chtl_comments >= 3);  // 应该找到至少3个CHTL注释
    assert(origin_comments == 0); // 不应该找到任何原始嵌入内的注释
    
    std::cout << "Complex OriginEmbed scenario tests passed!" << std::endl;
}

int main() {
    try {
        std::cout << "=== OriginEmbed Comment Handling Tests ===" << std::endl;
        
        testOriginEmbedDetection();
        testOriginEmbedReference();
        testCommentParsingWithOriginEmbeds();
        testComplexOriginEmbedScenario();
        
        std::cout << "\n=== All OriginEmbed tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}