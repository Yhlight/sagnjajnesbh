#include "../src/parser/Parser.h"
#include "../src/common/Context.h"
#include <iostream>
#include <cassert>

using namespace chtl;
using namespace chtl::parser;
using namespace chtl::ast;

void testBasicParsing() {
    std::cout << "=== 测试基础解析功能 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    // 测试简单的HTML元素解析
    std::string input = R"(
        div {
            id: "container";
            class: "main";
            
            text {
                "Hello World"
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->type == NodeType::PROGRAM);
    assert(program->getChildCount() == 1);
    
    auto element = dynamic_cast<ElementNode*>(program->getChild(0));
    assert(element != nullptr);
    assert(element->tag_name == "div");
    assert(element->getChildCount() == 3); // id, class, text
    
    std::cout << "基础解析功能测试通过!" << std::endl;
}

void testStyleBlockParsing() {
    std::cout << "=== 测试样式块解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        div {
            style {
                width: 100px;
                height: 200px;
                
                .box {
                    background-color: red;
                }
                
                &:hover {
                    opacity: 0.8;
                }
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    auto element = dynamic_cast<ElementNode*>(program->getChild(0));
    assert(element != nullptr);
    
    // 查找样式块
    StyleBlockNode* style_block = nullptr;
    for (size_t i = 0; i < element->getChildCount(); ++i) {
        if (element->getChild(i)->type == NodeType::STYLE_BLOCK) {
            style_block = dynamic_cast<StyleBlockNode*>(element->getChild(i));
            break;
        }
    }
    
    assert(style_block != nullptr);
    assert(style_block->is_local == true);
    assert(style_block->getChildCount() > 0);
    
    std::cout << "样式块解析测试通过!" << std::endl;
}

void testScriptBlockParsing() {
    std::cout << "=== 测试脚本块解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        div {
            script {
                {{box}}->listen({
                    click: () => {
                        console.log('clicked');
                    }
                });
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    auto element = dynamic_cast<ElementNode*>(program->getChild(0));
    assert(element != nullptr);
    
    // 查找脚本块
    ScriptBlockNode* script_block = nullptr;
    for (size_t i = 0; i < element->getChildCount(); ++i) {
        if (element->getChild(i)->type == NodeType::SCRIPT_BLOCK) {
            script_block = dynamic_cast<ScriptBlockNode*>(element->getChild(i));
            break;
        }
    }
    
    assert(script_block != nullptr);
    assert(script_block->is_local == true);
    
    std::cout << "脚本块解析测试通过!" << std::endl;
}

void testTemplateParsing() {
    std::cout << "=== 测试模板解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        [Template] @Style DefaultText {
            color: black;
            line-height: 1.6;
        }
        
        [Template] @Element Box {
            div {
                style {
                    width: 200px;
                    height: 200px;
                    background-color: red;
                }
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->getChildCount() == 2);
    
    auto template1 = dynamic_cast<TemplateDefinitionNode*>(program->getChild(0));
    assert(template1 != nullptr);
    assert(template1->template_type == "@Style");
    assert(template1->template_name == "DefaultText");
    
    auto template2 = dynamic_cast<TemplateDefinitionNode*>(program->getChild(1));
    assert(template2 != nullptr);
    assert(template2->template_type == "@Element");
    assert(template2->template_name == "Box");
    
    std::cout << "模板解析测试通过!" << std::endl;
}

void testCustomParsing() {
    std::cout << "=== 测试自定义解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        [Custom] @Style TextSet {
            color,
            font-size;
        }
        
        [Custom] @Element CustomBox {
            div {
                style {
                    @Style TextSet {
                        color: blue;
                        font-size: 16px;
                    }
                }
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->getChildCount() == 2);
    
    auto custom1 = dynamic_cast<CustomDefinitionNode*>(program->getChild(0));
    assert(custom1 != nullptr);
    assert(custom1->custom_type == "@Style");
    assert(custom1->custom_name == "TextSet");
    
    auto custom2 = dynamic_cast<CustomDefinitionNode*>(program->getChild(1));
    assert(custom2 != nullptr);
    assert(custom2->custom_type == "@Element");
    assert(custom2->custom_name == "CustomBox");
    
    std::cout << "自定义解析测试通过!" << std::endl;
}

void testOriginBlockParsing() {
    std::cout << "=== 测试原始嵌入解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        [Origin] @Html customBlock {
            <div class="custom">
                <p>This is raw HTML</p>
            </div>
        }
        
        [Origin] @JavaScript {
            function customFunction() {
                return "Hello from raw JS";
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->getChildCount() == 2);
    
    auto origin1 = dynamic_cast<OriginBlockNode*>(program->getChild(0));
    assert(origin1 != nullptr);
    assert(origin1->origin_type == "@Html");
    assert(origin1->block_name == "customBlock");
    assert(!origin1->raw_content.empty());
    
    auto origin2 = dynamic_cast<OriginBlockNode*>(program->getChild(1));
    assert(origin2 != nullptr);
    assert(origin2->origin_type == "@JavaScript");
    assert(!origin2->raw_content.empty());
    
    std::cout << "原始嵌入解析测试通过!" << std::endl;
}

void testImportParsing() {
    std::cout << "=== 测试导入语句解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        [Import] @Style from "styles.css" as MainStyles;
        [Import] @Chtl from "components.chtl";
        [Import] [Template] @Element Box from "templates.chtl";
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->getChildCount() == 3);
    
    auto import1 = dynamic_cast<ImportStatementNode*>(program->getChild(0));
    assert(import1 != nullptr);
    assert(import1->import_type == "@Style");
    assert(import1->source_path == "\"styles.css\"");
    assert(import1->alias == "MainStyles");
    
    auto import2 = dynamic_cast<ImportStatementNode*>(program->getChild(1));
    assert(import2 != nullptr);
    assert(import2->import_type == "@Chtl");
    assert(import2->source_path == "\"components.chtl\"");
    
    auto import3 = dynamic_cast<ImportStatementNode*>(program->getChild(2));
    assert(import3 != nullptr);
    assert(import3->import_type == "[Template] @Element");
    
    std::cout << "导入语句解析测试通过!" << std::endl;
}

void testNamespaceParsing() {
    std::cout << "=== 测试命名空间解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        [Namespace] TestSpace {
            [Template] @Style ThemeColor {
                color: blue;
            }
            
            [Namespace] InnerSpace {
                [Custom] @Element InnerBox {
                    div {
                        style {
                            border: 1px solid black;
                        }
                    }
                }
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->getChildCount() == 1);
    
    auto namespace_node = dynamic_cast<NamespaceDefinitionNode*>(program->getChild(0));
    assert(namespace_node != nullptr);
    assert(namespace_node->namespace_name == "TestSpace");
    assert(namespace_node->getChildCount() == 2); // Template + Namespace
    
    std::cout << "命名空间解析测试通过!" << std::endl;
}

void testCommentParsing() {
    std::cout << "=== 测试注释解析 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    std::string input = R"(
        // This is a single line comment
        /* This is a multi-line comment */
        -- This is a generator comment
        
        div {
            // Another comment
            id: "test";
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    // 查找注释节点
    int comment_count = 0;
    for (size_t i = 0; i < program->getChildCount(); ++i) {
        if (program->getChild(i)->type == NodeType::SINGLE_COMMENT ||
            program->getChild(i)->type == NodeType::MULTI_COMMENT ||
            program->getChild(i)->type == NodeType::GENERATOR_COMMENT) {
            comment_count++;
        }
    }
    
    // 应该找到一些注释节点
    assert(comment_count > 0);
    
    std::cout << "注释解析测试通过!" << std::endl;
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    // 测试语法错误
    std::string invalid_input = R"(
        div {
            id: "test"  // 缺少分号
            class: "container";
        }
        
        invalid_element_name_123 {
            // 无效的元素名
        }
    )";
    
    parser.setInput(invalid_input);
    auto program = parser.parseProgram();
    
    // 解析器应该能够处理错误并继续解析
    assert(program != nullptr);
    assert(parser.hasError() || context.hasErrors());
    
    if (parser.hasError()) {
        auto errors = parser.getErrors();
        std::cout << "捕获到 " << errors.size() << " 个解析错误" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  错误: " << error.message << " @" 
                      << error.position.line << ":" << error.position.column << std::endl;
        }
    }
    
    std::cout << "错误处理测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始CHTL解析器测试..." << std::endl;
        
        testBasicParsing();
        testStyleBlockParsing();
        testScriptBlockParsing();
        testTemplateParsing();
        testCustomParsing();
        testOriginBlockParsing();
        testImportParsing();
        testNamespaceParsing();
        testCommentParsing();
        testErrorHandling();
        
        std::cout << std::endl << "=== 所有解析器测试通过! ===" << std::endl;
        std::cout << "第三个Todo (开发CHTL与CHTL JS的解析器及生成器) 的解析器部分已完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}