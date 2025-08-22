#include "iNeverAway.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace CJMOD {

// iNeverAway扩展实现 - 语法文档第1485-1520行
// 功能：创建一组标记函数，允许开发者定义键，使用状态区分同名键

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

CHTLJS::AST::ASTNodePtr INeverAwayExtension::ParseSyntax(const std::string& input, const std::any& context) const {
    // 解析iNeverAway语法
    // 语法文档第1491-1507行：
    // vir Test = iNeverAway({
    //     Void<A>: function(int, int) { },
    //     Void<B>: function(int, int) { },
    //     Void: { },
    //     Ax: { }
    // });
    
    if (input.find("vir") != std::string::npos && input.find("iNeverAway") != std::string::npos) {
        // 解析iNeverAway定义
        INeverAwayVirtualObject virtualObject = ParseINeverAwaySyntax(input);
        
        if (!virtualObject.objectName.empty()) {
            // 注册到全局虚对象表
            globalVirtualObjects_[virtualObject.objectName] = virtualObject;
            
            Utils::ErrorHandler::GetInstance().LogInfo(
                "iNeverAway虚对象解析完成: " + virtualObject.objectName + 
                "，方法数量: " + std::to_string(virtualObject.methods.size())
            );
        }
    } else if (input.find("->") != std::string::npos) {
        // 解析虚对象调用 Test->Void<A>()
        Utils::ErrorHandler::GetInstance().LogInfo(
            "iNeverAway虚对象调用解析: " + input
        );
    }
    
    // 创建标识符节点表示iNeverAway语法
    auto node = std::make_shared<CHTLJS::AST::IdentifierNode>("iNeverAway_syntax", 
                                                              CHTLJS::Core::CHTLJSToken());
    
    return node;
}

std::string INeverAwayExtension::GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const {
    // 生成iNeverAway的JavaScript实现
    // 语法文档第1513-1515行：
    // iNeverAway -> 创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码
    // vir对象本身不存在，最终转变成相对应的函数的引用
    
    std::string input;
    try {
        input = std::any_cast<std::string>(context);
    } catch (const std::bad_any_cast&) {
        return "";
    }
    
    if (input.find("vir") != std::string::npos && input.find("iNeverAway") != std::string::npos) {
        // 生成iNeverAway定义的JavaScript
        INeverAwayVirtualObject virtualObject = ParseINeverAwaySyntax(input);
        return GenerateINeverAwayJS(virtualObject);
    } else if (input.find("->") != std::string::npos) {
        // 生成虚对象调用的JavaScript
        std::regex callRegex(R"((\w+)->([^(]+)\([^)]*\))");
        std::smatch match;
        
        if (std::regex_search(input, match, callRegex)) {
            std::string objectName = match[1].str();
            std::string methodCall = match[2].str();
            
            return HandleVirtualObjectCall(objectName, methodCall);
        }
    }
    
    return "";
}

bool INeverAwayExtension::Initialize() {
    // 初始化iNeverAway扩展
    globalVirtualObjects_.clear();
    globalFunctionCounter_ = 0;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "iNeverAway扩展初始化完成"
    );
    return true;
}

void INeverAwayExtension::Cleanup() {
    // 清理iNeverAway扩展
    globalVirtualObjects_.clear();
    globalFunctionCounter_ = 0;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "iNeverAway扩展清理完成"
    );
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
            std::string methodName = Utils::StringUtils::Trim(methodMatch[1].str());
            std::string methodDef = Utils::StringUtils::Trim(methodMatch[2].str());
            
            INeverAwayMethod method = ParseMethod(methodName + ": " + methodDef);
            if (!method.name.empty()) {
                virtualObject.methods.push_back(method);
                
                // 生成全局函数名称 - 语法文档第1514行：名称由CHTL编译器统一管理
                std::string globalFuncName = GenerateGlobalFunctionName(virtualObject.objectName, method.name);
                method.generatedFunctionName = globalFuncName;
                virtualObject.methodNameMap[method.name] = globalFuncName;
            }
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
        js << "function " << method.generatedFunctionName << "(";
        
        // 生成参数列表
        for (size_t i = 0; i < method.parameters.size(); ++i) {
            if (i > 0) js << ", ";
            js << "param" << i;
        }
        
        js << ") {\n";
        js << "    // " << method.name << " 实现\n";
        js << "    " << method.functionBody << "\n";
        js << "}\n\n";
    }
    
    // 生成虚对象调用映射注释
    js << "// 虚对象调用映射:\n";
    for (const auto& mapping : virtualObject.methodNameMap) {
        js << "// " << virtualObject.objectName << "->" << mapping.first << "() => " << mapping.second << "()\n";
    }
    
    return js.str();
}

std::string INeverAwayExtension::HandleVirtualObjectCall(const std::string& objectName, const std::string& methodCall) {
    // 处理虚对象调用 - 语法文档第1509行：Test->Void<A>();
    // 语法文档第1515行：vir对象本身不存在，最终转变成相对应的函数的引用
    
    auto it = globalVirtualObjects_.find(objectName);
    if (it == globalVirtualObjects_.end()) {
        Utils::ErrorHandler::GetInstance().LogError(
            "未找到虚对象: " + objectName
        );
        return "/* 错误：未找到虚对象 " + objectName + " */";
    }
    
    const INeverAwayVirtualObject& virtualObject = it->second;
    
    // 查找方法映射
    auto methodIt = virtualObject.methodNameMap.find(methodCall);
    if (methodIt != virtualObject.methodNameMap.end()) {
        // 返回全局函数调用
        return methodIt->second + "()";
    }
    
    Utils::ErrorHandler::GetInstance().LogError(
        "虚对象 " + objectName + " 中未找到方法: " + methodCall
    );
    
    return "/* 错误：未找到方法 " + methodCall + " */";
}

INeverAwayExtension::INeverAwayMethod INeverAwayExtension::ParseMethod(const std::string& methodDef) {
    INeverAwayMethod method;
    
    // 解析方法定义
    // 语法文档第1492-1506行的格式：
    // Void<A>: function(int, int) { }
    // Void<B>: function(int, int) { }
    // Void: { }
    // Ax: { }
    
    std::regex methodRegex(R"(([^:]+):\s*(.+))");
    std::smatch match;
    
    if (std::regex_search(methodDef, match, methodRegex)) {
        method.name = Utils::StringUtils::Trim(match[1].str());
        std::string definition = Utils::StringUtils::Trim(match[2].str());
        
        // 确定状态类型
        method.stateType = DetermineVoidStateType(method.name);
        
        // 提取状态标识符
        if (method.stateType == VoidStateType::TYPED_VOID) {
            method.stateIdentifier = ExtractStateIdentifier(method.name);
        }
        
        // 解析参数和函数体
        if (definition.find("function") != std::string::npos) {
            // function(int, int) { ... } 格式
            std::regex funcRegex(R"(function\s*\(([^)]*)\)\s*\{([^}]*)\})");
            std::smatch funcMatch;
            
            if (std::regex_search(definition, funcMatch, funcRegex)) {
                std::string paramStr = funcMatch[1].str();
                method.functionBody = Utils::StringUtils::Trim(funcMatch[2].str());
                method.parameters = ParseParameterTypes(paramStr);
            }
        } else {
            // { ... } 格式
            std::regex blockRegex(R"(\{([^}]*)\})");
            std::smatch blockMatch;
            
            if (std::regex_search(definition, blockMatch, blockRegex)) {
                method.functionBody = Utils::StringUtils::Trim(blockMatch[1].str());
            }
        }
    }
    
    return method;
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

std::vector<std::string> INeverAwayExtension::ParseParameterTypes(const std::string& paramStr) {
    // 解析参数类型
    // 语法文档第1492行：function(int, int)
    
    if (paramStr.empty()) {
        return {};
    }
    
    std::vector<std::string> parameters = Utils::StringUtils::Split(paramStr, ",");
    
    // 清理参数类型
    for (auto& param : parameters) {
        param = Utils::StringUtils::Trim(param);
    }
    
    return parameters;
}

} // namespace CJMOD
} // namespace CHTL