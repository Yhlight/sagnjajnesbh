#include <iostream>
#include <string>

// 简单测试程序，模拟编译器的关键逻辑
int main() {
    std::cout << "🔍 简单调试：检查生成器逻辑问题" << std::endl;
    
    // 模拟ElementNode的情况
    struct MockElementNode {
        std::string tag;
        int childCount;
    };
    
    // 测试不同的情况
    MockElementNode cases[] = {
        {"title", 0},  // 没有子节点的情况
        {"title", 1},  // 有一个TextNode子节点的情况
        {"div", 0},    // 没有子节点的div
        {"div", 1}     // 有一个TextNode子节点的div
    };
    
    for (auto& testCase : cases) {
        std::cout << "\n测试元素: <" << testCase.tag << "> 子节点数: " << testCase.childCount << std::endl;
        
        // 当前生成器逻辑
        if (testCase.childCount == 0) {
            std::cout << "  当前输出: <" << testCase.tag << " />" << std::endl;
            std::cout << "  ❌ 问题: 生成了自闭合标签" << std::endl;
        } else {
            std::cout << "  当前输出: <" << testCase.tag << ">内容</" << testCase.tag << ">" << std::endl;
            std::cout << "  ✅ 正确: 生成了完整标签" << std::endl;
        }
        
        // 修复后的逻辑
        static const std::string self_closing[] = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
        bool is_self_closing = false;
        for (const auto& tag : self_closing) {
            if (testCase.tag == tag) {
                is_self_closing = true;
                break;
            }
        }
        
        if (testCase.childCount == 0 && is_self_closing) {
            std::cout << "  修复后: <" << testCase.tag << " />" << std::endl;
        } else {
            std::cout << "  修复后: <" << testCase.tag << "></" << testCase.tag << ">" << std::endl;
        }
    }
    
    std::cout << "\n🎯 结论: 问题在于生成器将所有无子节点的元素都生成为自闭合标签" << std::endl;
    std::cout << "应该只有特定的HTML元素（如img、br、meta等）才能自闭合" << std::endl;
    
    return 0;
}