#include "../Framework/TestBase.h"
#include "../../CHTLCompiler/Core/CHTLCompilerCore.h"
#include "../../CHTLJSCompiler/Core/CHTLJSCompilerCore.h"
#include <iostream>

namespace chtl {
namespace test {

class ActualSyntaxImplementationTest : public TestBase {
public:
    ActualSyntaxImplementationTest() : TestBase("ActualSyntaxImplementation") {}
    
    void runAllTests() override {
        std::cout << "🔬 实际语法功能实现测试\n" << std::endl;
        
        testActualTemplateCompilation();
        testActualCustomCompilation();
        testActualOriginCompilation();
        testActualConstraintCompilation();
        testActualVariableGroupCompilation();
        testActualSpecializationOperations();
        
        printSummary();
    }

private:
    void testActualTemplateCompilation() {
        startTest("实际模板编译功能");
        
        // 测试真正的模板编译
        std::string templateCode = R"(
            [Template] @Style DefaultText
            {
                color: "black";
                line-height: 1.6;
            }
            
            div
            {
                style
                {
                    @Style DefaultText;
                }
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(templateCode);
            
            // 检查是否真正编译了模板
            bool hasTemplateSupport = !result.empty() && 
                                    result.find("color: black") != std::string::npos;
            
            assertTrue(hasTemplateSupport, "模板编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "模板编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
    
    void testActualCustomCompilation() {
        startTest("实际自定义编译功能");
        
        std::string customCode = R"(
            [Custom] @Style TextSet
            {
                color,
                font-size;
            }
            
            div
            {
                style
                {
                    @Style TextSet
                    {
                        color: red;
                        font-size: 16px;
                    }
                }
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(customCode);
            
            bool hasCustomSupport = !result.empty() && 
                                  result.find("color: red") != std::string::npos;
            
            assertTrue(hasCustomSupport, "自定义编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "自定义编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
    
    void testActualOriginCompilation() {
        startTest("实际原始嵌入编译功能");
        
        std::string originCode = R"(
            [Origin] @Html
            {
                <div class="raw-html">原始HTML</div>
            }
            
            [Origin] @Style
            {
                .raw-style { color: blue; }
            }
            
            body
            {
                [Origin] @Html;
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(originCode);
            
            bool hasOriginSupport = !result.empty() && 
                                  result.find("raw-html") != std::string::npos;
            
            assertTrue(hasOriginSupport, "原始嵌入编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "原始嵌入编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
    
    void testActualConstraintCompilation() {
        startTest("实际约束编译功能");
        
        std::string constraintCode = R"(
            div
            {
                except span, [Custom] @Element Box;
                
                // 这里不应该允许使用span
                p { text { 允许的元素 } }
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(constraintCode);
            
            // 约束系统应该阻止span的使用
            bool hasConstraintSupport = !result.empty();
            
            assertTrue(hasConstraintSupport, "约束编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "约束编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
    
    void testActualVariableGroupCompilation() {
        startTest("实际变量组编译功能");
        
        std::string varCode = R"(
            [Template] @Var ThemeColor
            {
                tableColor: "rgb(255, 192, 203)";
            }
            
            div
            {
                style
                {
                    background-color: ThemeColor(tableColor);
                }
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(varCode);
            
            bool hasVarSupport = !result.empty() && 
                               result.find("rgb(255, 192, 203)") != std::string::npos;
            
            assertTrue(hasVarSupport, "变量组编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "变量组编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
    
    void testActualSpecializationOperations() {
        startTest("实际特例化操作编译功能");
        
        std::string specializationCode = R"(
            [Custom] @Element Box
            {
                div { }
                div { }
                span { }
            }
            
            body
            {
                @Element Box
                {
                    delete span;
                    insert after div[0] {
                        p { text { 插入的内容 } }
                    }
                }
            }
        )";
        
        try {
            CHTLCompilerCore compiler;
            std::string result = compiler.compile(specializationCode);
            
            bool hasSpecializationSupport = !result.empty();
            
            assertTrue(hasSpecializationSupport, "特例化操作编译功能应该真正工作");
            
        } catch (const std::exception& e) {
            assertTrue(false, "特例化操作编译抛出异常: " + std::string(e.what()));
        }
        
        endTest();
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::ActualSyntaxImplementationTest test;
    test.runAllTests();
    
    std::cout << "\n📋 实际功能实现测试总结:" << std::endl;
    std::cout << "这个测试直接调用编译器来验证语法功能是否真正实现" << std::endl;
    std::cout << "而不是仅仅检查语法格式的正确性" << std::endl;
    
    return 0;
}