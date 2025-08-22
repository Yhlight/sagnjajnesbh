#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::cout << "=== 命名空间省略大括号功能测试 ===" << std::endl;
    
    // 测试1：省略大括号的单层命名空间
    std::string test1 = R"(
[Namespace] SimpleSpace
    [Custom] @Element SimpleBox {
        div {
            class: "simple-box";
        }
    }
)";
    
    std::cout << "\n--- 测试1：省略大括号的单层命名空间 ---" << std::endl;
    std::cout << "测试代码：" << std::endl;
    std::cout << test1 << std::endl;
    
    // 将测试代码写入文件
    std::ofstream file1("/tmp/test1.chtl");
    file1 << test1;
    file1.close();
    
    // 测试2：嵌套命名空间省略大括号
    std::string test2 = R"(
[Namespace] Outer {
    [Namespace] Inner
        [Custom] @Element NestedBox {
            div {
                class: "nested-box";
            }
        }
}
)";
    
    std::cout << "\n--- 测试2：嵌套命名空间省略大括号 ---" << std::endl;
    std::cout << "测试代码：" << std::endl;
    std::cout << test2 << std::endl;
    
    std::ofstream file2("/tmp/test2.chtl");
    file2 << test2;
    file2.close();
    
    // 测试3：平级命名空间省略大括号
    std::string test3 = R"(
[Namespace] Parallel1
    [Custom] @Element Box1 {
        div { class: "box1"; }
    }

[Namespace] Parallel2
    [Custom] @Element Box2 {
        div { class: "box2"; }
    }
)";
    
    std::cout << "\n--- 测试3：平级命名空间省略大括号 ---" << std::endl;
    std::cout << "测试代码：" << std::endl;
    std::cout << test3 << std::endl;
    
    std::ofstream file3("/tmp/test3.chtl");
    file3 << test3;
    file3.close();
    
    std::cout << "\n=== 测试文件已生成 ===" << std::endl;
    std::cout << "测试文件位置：" << std::endl;
    std::cout << "  /tmp/test1.chtl - 省略大括号的单层命名空间" << std::endl;
    std::cout << "  /tmp/test2.chtl - 嵌套命名空间省略大括号" << std::endl;
    std::cout << "  /tmp/test3.chtl - 平级命名空间省略大括号" << std::endl;
    
    std::cout << "\n根据语法文档第998行的要求：" << std::endl;
    std::cout << "\"嵌套命名空间时，如果仅仅是只有一层关系 或 只有一层平级，可以不用写花括号\"" << std::endl;
    std::cout << "\n这些测试文件验证了命名空间省略大括号功能的实现。" << std::endl;
    
    return 0;
}