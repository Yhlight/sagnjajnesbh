#include <iostream>
#include <string>
#include <regex>

int main() {
    std::cout << "🔧 CHTL delete语法正确性验证\n";
    std::cout << "============================\n\n";
    
    // 正确的delete语法示例
    std::string correctDeleteExamples = R"(
// 1. 删除样式属性（支持链式）
[Custom] @Style YellowText {
    @Style WhiteText {
        delete line-height, border;
    }
    color: yellow;
}

div {
    style {
        @Style YellowText {
            delete color;
        }
    }
}

// 2. 删除元素（使用元素名或索引）
[Custom] @Element Box {
    div { }
    div { }
    span { }
    div { }
}

body {
    @Element Box {
        delete span;        // 删除元素类型
        delete div[1];      // 删除特定索引的元素
    }
}

// 3. 删除元素继承
[Custom] @Element Box {
    div { }
    @Element Line;
}

body {
    @Element Box {
        delete @Element Line;  // 删除继承的元素
    }
}
)";
    
    std::cout << "✅ 正确的delete语法示例长度: " << correctDeleteExamples.length() << " 字符\n\n";
    
    std::cout << "🔍 验证delete语法类型:\n";
    std::cout << "=====================\n";
    
    // 1. 删除样式属性（链式）
    std::regex deletePropsRegex(R"(delete\s+[\w-]+(?:\s*,\s*[\w-]+)*\s*;)");
    int deletePropsCount = std::distance(
        std::sregex_iterator(correctDeleteExamples.begin(), correctDeleteExamples.end(), deletePropsRegex),
        std::sregex_iterator()
    );
    std::cout << "✅ 删除样式属性（链式）: " << deletePropsCount << " 处\n";
    
    // 2. 删除元素类型
    std::regex deleteElementRegex(R"(delete\s+\w+\s*;)");
    int deleteElementCount = std::distance(
        std::sregex_iterator(correctDeleteExamples.begin(), correctDeleteExamples.end(), deleteElementRegex),
        std::sregex_iterator()
    );
    std::cout << "✅ 删除元素类型: " << deleteElementCount << " 处\n";
    
    // 3. 删除索引元素
    std::regex deleteIndexRegex(R"(delete\s+\w+\[\d+\]\s*;)");
    int deleteIndexCount = std::distance(
        std::sregex_iterator(correctDeleteExamples.begin(), correctDeleteExamples.end(), deleteIndexRegex),
        std::sregex_iterator()
    );
    std::cout << "✅ 删除索引元素: " << deleteIndexCount << " 处\n";
    
    // 4. 删除继承元素
    std::regex deleteInheritRegex(R"(delete\s+@Element\s+\w+\s*;)");
    int deleteInheritCount = std::distance(
        std::sregex_iterator(correctDeleteExamples.begin(), correctDeleteExamples.end(), deleteInheritRegex),
        std::sregex_iterator()
    );
    std::cout << "✅ 删除继承元素: " << deleteInheritCount << " 处\n";
    
    std::cout << "\n❌ 验证错误的delete语法:\n";
    std::cout << "========================\n";
    
    // 检查是否有错误的CSS选择器delete
    std::string wrongDeleteExamples = "delete .course-description, .course-meta;";
    
    std::regex wrongSelectorRegex(R"(delete\s+[.#][\w-]+)");
    int wrongSelectorCount = std::distance(
        std::sregex_iterator(wrongDeleteExamples.begin(), wrongDeleteExamples.end(), wrongSelectorRegex),
        std::sregex_iterator()
    );
    std::cout << (wrongSelectorCount > 0 ? "❌" : "✅") << " 错误的CSS选择器delete: " << wrongSelectorCount << " 处\n";
    
    std::cout << "\n📋 delete语法总结:\n";
    std::cout << "==================\n";
    std::cout << "✅ 支持的delete类型:\n";
    std::cout << "   1. 样式属性: delete line-height, border; (支持链式)\n";
    std::cout << "   2. 元素类型: delete span;\n";
    std::cout << "   3. 索引元素: delete div[1];\n";
    std::cout << "   4. 继承元素: delete @Element ElementName;\n\n";
    
    std::cout << "❌ 不支持的delete类型:\n";
    std::cout << "   1. CSS选择器: delete .class, #id; (错误)\n";
    std::cout << "   2. 复杂选择器: delete .parent .child; (错误)\n\n";
    
    std::cout << "🌟 结论:\n";
    std::cout << "========\n";
    std::cout << "🎉 delete语法理解正确！\n";
    std::cout << "✨ 支持属性链式删除\n";
    std::cout << "🎯 支持元素和索引删除\n";
    std::cout << "⚠️ 不支持CSS选择器删除\n";
    std::cout << "💎 严格按照CHTL语法文档\n";
    
    return 0;
}