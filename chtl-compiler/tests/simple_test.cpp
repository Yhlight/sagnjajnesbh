#include "../src/simple/CHTLCompiler.h"
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

using namespace chtl::simple;

// 简单的测试框架
class TestRunner {
public:
    void Run(const std::string& name, std::function<void()> test) {
        std::cout << "Running: " << name << " ... ";
        try {
            test();
            std::cout << "PASSED\n";
            m_Passed++;
        } catch (const std::exception& e) {
            std::cout << "FAILED: " << e.what() << "\n";
            m_Failed++;
        }
    }
    
    void Summary() {
        std::cout << "\n=== Test Summary ===\n";
        std::cout << "Passed: " << m_Passed << "\n";
        std::cout << "Failed: " << m_Failed << "\n";
        std::cout << "Total:  " << (m_Passed + m_Failed) << "\n";
    }
    
private:
    int m_Passed = 0;
    int m_Failed = 0;
};

// 测试辅助函数
void AssertEquals(const std::string& expected, const std::string& actual, const std::string& message = "") {
    if (expected != actual) {
        throw std::runtime_error(message + "\nExpected: " + expected + "\nActual: " + actual);
    }
}

void AssertContains(const std::string& haystack, const std::string& needle, const std::string& message = "") {
    if (haystack.find(needle) == std::string::npos) {
        throw std::runtime_error(message + "\nString not found: " + needle);
    }
}

// 测试用例
void TestBasicElement() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(false);
    
    std::string input = "div { }";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "<div>", "Should generate opening div tag");
    AssertContains(output, "</div>", "Should generate closing div tag");
}

void TestElementWithText() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(false);
    
    std::string input = "p { text { Hello World } }";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "<p>", "Should have p tag");
    AssertContains(output, "Hello World", "Should contain text");
    AssertContains(output, "</p>", "Should close p tag");
}

void TestElementWithAttributes() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(false);
    
    std::string input = "div { class: \"container\"; id: \"main\"; }";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "class=\"container\"", "Should have class attribute");
    AssertContains(output, "id=\"main\"", "Should have id attribute");
}

void TestNestedElements() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(false);
    
    std::string input = R"(
        div {
            h1 { text { Title } }
            p { text { Content } }
        }
    )";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "<div>", "Should have outer div");
    AssertContains(output, "<h1>", "Should have h1");
    AssertContains(output, "Title", "Should have title text");
    AssertContains(output, "<p>", "Should have p");
    AssertContains(output, "Content", "Should have content text");
}

void TestStyleBlock() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(true);
    
    std::string input = R"(
        style {
            body { margin: 0; }
        }
        div { text { Hello } }
    )";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "<style>", "Should have style tag");
    AssertContains(output, "body", "Should contain CSS selector");
    AssertContains(output, "margin: 0;", "Should contain CSS rule");
    AssertContains(output, "</style>", "Should close style tag");
}

void TestScriptBlock() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(true);
    
    std::string input = R"(
        div { text { Hello } }
        script {
            console.log("Hello from CHTL!");
        }
    )";
    std::string output = compiler.Compile(input);
    
    AssertContains(output, "<script>", "Should have script tag");
    AssertContains(output, "log", "Should contain JavaScript code");
    AssertContains(output, "</script>", "Should close script tag");
}

void TestMinifiedOutput() {
    CHTLCompiler compiler;
    compiler.SetGenerateFullDocument(false);
    compiler.SetPrettyPrint(false);
    
    std::string input = "div { p { text { Test } } }";
    std::string output = compiler.Compile(input);
    
    // Should not contain newlines in minified mode
    if (output.find('\n') != std::string::npos) {
        throw std::runtime_error("Minified output should not contain newlines");
    }
}

int main() {
    TestRunner runner;
    
    std::cout << "=== CHTL Compiler Tests ===\n\n";
    
    runner.Run("Basic Element", TestBasicElement);
    runner.Run("Element with Text", TestElementWithText);
    runner.Run("Element with Attributes", TestElementWithAttributes);
    runner.Run("Nested Elements", TestNestedElements);
    runner.Run("Style Block", TestStyleBlock);
    runner.Run("Script Block", TestScriptBlock);
    runner.Run("Minified Output", TestMinifiedOutput);
    
    runner.Summary();
    
    return 0;
}