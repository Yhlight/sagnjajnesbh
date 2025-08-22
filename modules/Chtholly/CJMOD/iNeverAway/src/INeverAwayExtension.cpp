#include "INeverAwayExtension.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace Chtholly {
namespace CJMOD {

// iNeverAway扩展实现 - 语法文档第1485-1520行
// 功能：创建一组标记函数，允许开发者定义键，使用状态区分同名键
// 通过CJMOD模块分发，不修改CHTL源码

// 全局状态管理 - 语法文档第1425行：可以编写全局变量管理状态
std::unordered_map<std::string, INeverAwayExtension::INeverAwayVirtualObject> INeverAwayExtension::globalVirtualObjects_;
int INeverAwayExtension::globalFunctionCounter_ = 0;

std::vector<std::string> INeverAwayExtension::GetSupportedSyntaxPatterns() const {
    return {
        "vir\\s+\\w+\\s*=\\s*iNeverAway\\({[^}]+}\\)",  // vir Test = iNeverAway({...})
        "\\w+->Void<\\w+>\\([^)]*\\)",                   // Test->Void<A>()
        "\\w+->[\\w<>]+\\([^)]*\\)"                      // Test->Ax()
    };
}

bool INeverAwayExtension::MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const {
    // 检查是否匹配iNeverAway语法
    std::regex iNeverAwayRegex(R"(vir\s+\w+\s*=\s*iNeverAway\s*\(\s*\{[^}]+\}\s*\))");
    std::regex virtualCallRegex(R"(\w+->\w+\([^)]*\))");
    
    return std::regex_search(syntaxPattern, iNeverAwayRegex) || 
           std::regex_search(syntaxPattern, virtualCallRegex);
}

std::string INeverAwayExtension::ParseSyntax(const std::string& input, const std::any& context) const {
    // 解析iNeverAway语法并返回生成的JavaScript代码
    
    if (input.find("vir") != std::string::npos && input.find("iNeverAway") != std::string::npos) {
        // 解析iNeverAway定义
        INeverAwayVirtualObject virtualObject = ParseINeverAwaySyntax(input);
        
        if (!virtualObject.objectName.empty()) {
            // 注册到全局虚对象表
            globalVirtualObjects_[virtualObject.objectName] = virtualObject;
            return GenerateINeverAwayJS(virtualObject);
        }
    } else if (input.find("->") != std::string::npos) {
        // 解析虚对象调用 Test->Void<A>()
        return HandleVirtualObjectCall(input);
    }
    
    return "";
}

std::string INeverAwayExtension::GenerateJavaScript(const INeverAwayVirtualObject& virtualObject) const {
    return GenerateINeverAwayJS(virtualObject);
}

bool INeverAwayExtension::Initialize() {
    globalVirtualObjects_.clear();
    globalFunctionCounter_ = 0;
    
    std::cout << "iNeverAway扩展初始化完成" << std::endl;
    return true;
}

void INeverAwayExtension::Cleanup() {
    globalVirtualObjects_.clear();
    globalFunctionCounter_ = 0;
    
    std::cout << "iNeverAway扩展清理完成" << std::endl;
}

INeverAwayExtension::INeverAwayVirtualObject INeverAwayExtension::ParseINeverAwaySyntax(const std::string& input) {
    INeverAwayVirtualObject virtualObject;
    
    // 提取虚对象名称
    std::regex virRegex(R"(vir\s+(\w+)\s*=\s*iNeverAway)");
    std::smatch virMatch;
    
    if (std::regex_search(input, virMatch, virRegex)) {
        virtualObject.objectName = virMatch[1].str();
    }
    
    // 提取方法定义块
    std::regex methodBlockRegex(R"(iNeverAway\s*\(\s*\{([^}]+)\}\s*\))");
    std::smatch blockMatch;
    
    if (std::regex_search(input, blockMatch, methodBlockRegex)) {
        std::string methodContent = blockMatch[1].str();
        
        // 解析各个方法
        std::regex methodRegex(R"(([^:]+):\s*([^,}]+))");
        std::sregex_iterator iter(methodContent.begin(), methodContent.end(), methodRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            const std::smatch& methodMatch = *iter;
            std::string methodName = methodMatch[1].str();
            std::string methodDef = methodMatch[2].str();
            
            // 清理空白
            methodName.erase(0, methodName.find_first_not_of(" \t"));
            methodName.erase(methodName.find_last_not_of(" \t") + 1);
            
            INeverAwayMethod method;
            method.name = methodName;
            method.stateType = DetermineVoidStateType(methodName);
            
            if (method.stateType == VoidStateType::TYPED_VOID) {
                method.stateIdentifier = ExtractStateIdentifier(methodName);
            }
            
            // 生成全局函数名称 - 语法文档第1514行：名称由CHTL编译器统一管理
            method.generatedFunctionName = GenerateGlobalFunctionName(virtualObject.objectName, method.name);
            
            virtualObject.methods.push_back(method);
            virtualObject.methodNameMap[method.name] = method.generatedFunctionName;
        }
    }
    
    return virtualObject;
}

std::string INeverAwayExtension::GenerateGlobalFunctionName(const std::string& objectName, const std::string& methodName) {
    // 生成全局函数名称 - 语法文档第1514行：名称由CHTL编译器统一管理
    return "_CHTL_iNeverAway_" + objectName + "_" + methodName + "_" + std::to_string(++globalFunctionCounter_);
}

std::string INeverAwayExtension::GenerateINeverAwayJS(const INeverAwayVirtualObject& virtualObject) {
    // 生成iNeverAway的JavaScript实现
    // 语法文档第1514行：创建一组JS全局函数，在调用时才生成对应的JS函数代码
    
    std::ostringstream js;
    
    js << "// iNeverAway扩展 - 虚对象: " << virtualObject.objectName << "\n";
    js << "// 语法文档第1514行：创建一组JS全局函数，名称由CHTL编译器统一管理\n\n";
    
    // 为每个方法生成全局函数
    for (const auto& method : virtualObject.methods) {
        js << "// 方法: " << method.name << "\n";
        js << "function " << method.generatedFunctionName << "() {\n";
        js << "    // " << method.name << " 实现\n";
        js << "    console.log('调用了 " << method.name << " 方法');\n";
        js << "}\n\n";
    }
    
    // 生成虚对象调用映射注释
    js << "// 虚对象调用映射:\n";
    for (const auto& mapping : virtualObject.methodNameMap) {
        js << "// " << virtualObject.objectName << "->" << mapping.first << "() => " << mapping.second << "()\n";
    }
    
    return js.str();
}

std::string INeverAwayExtension::HandleVirtualObjectCall(const std::string& input) {
    // 处理虚对象调用 - 语法文档第1509行：Test->Void<A>();
    // 语法文档第1515行：vir对象本身不存在，最终转变成相对应的函数的引用
    
    std::regex callRegex(R"((\w+)->([^(]+)\([^)]*\))");
    std::smatch match;
    
    if (std::regex_search(input, match, callRegex)) {
        std::string objectName = match[1].str();
        std::string methodCall = match[2].str();
        
        auto it = globalVirtualObjects_.find(objectName);
        if (it != globalVirtualObjects_.end()) {
            const INeverAwayVirtualObject& virtualObject = it->second;
            
            // 查找方法映射
            auto methodIt = virtualObject.methodNameMap.find(methodCall);
            if (methodIt != virtualObject.methodNameMap.end()) {
                // 返回全局函数调用
                return methodIt->second + "();";
            }
        }
        
        return "/* 错误：未找到虚对象方法 " + objectName + "->" + methodCall + " */";
    }
    
    return "";
}

INeverAwayExtension::VoidStateType INeverAwayExtension::DetermineVoidStateType(const std::string& methodName) {
    // 确定Void状态类型
    // 语法文档第1492-1500行：Void<A>、Void<B>是TYPED_VOID，Void是SIMPLE_VOID
    
    if (methodName.find("Void<") != std::string::npos && methodName.find(">") != std::string::npos) {
        return VoidStateType::TYPED_VOID;  // Void<A>、Void<B>
    } else if (methodName == "Void") {
        return VoidStateType::SIMPLE_VOID;  // Void
    } else {
        return VoidStateType::SIMPLE_VOID;  // 其他方法（如Ax）
    }
}

std::string INeverAwayExtension::ExtractStateIdentifier(const std::string& methodName) {
    // 提取状态标识符
    // 从Void<A>中提取A，从Void<B>中提取B
    
    std::regex stateRegex(R"(Void<(\w+)>)");
    std::smatch match;
    
    if (std::regex_search(methodName, match, stateRegex)) {
        return match[1].str();
    }
    
    return "";
}

} // namespace CJMOD
} // namespace Chtholly