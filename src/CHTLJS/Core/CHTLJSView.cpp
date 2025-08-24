#include "CHTLJS/Core/CHTLJSView.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace CHTLJS {
namespace Core {

// 静态成员初始化
int CHTLJSView::globalFunctionCounter_ = 0;

CHTLJSView::CHTLJSView() {
    Utils::ErrorHandler::GetInstance().LogInfo("初始化CHTL JS虚对象View管理器");
}

void CHTLJSView::RecordVirtualObjectName(const std::string& objectName) {
    if (objectName.empty()) {
        Utils::ErrorHandler::GetInstance().LogError("虚对象名称不能为空");
        return;
    }
    
    // 检查是否已存在
    auto it = std::find(virtualObjectNames_.begin(), virtualObjectNames_.end(), objectName);
    if (it != virtualObjectNames_.end()) {
        Utils::ErrorHandler::GetInstance().LogWarning("虚对象 '" + objectName + "' 已存在，将被覆盖");
        return;
    }
    
    virtualObjectNames_.push_back(objectName);
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "记录虚对象名称: " + objectName
    );
}

void CHTLJSView::ParseCHTLJSFunctionKeys(const std::string& objectName, 
                                       const std::string& chtlJSFunction,
                                       const std::vector<std::string>& functionKeys) {
    if (!IsValidCHTLJSFunction(chtlJSFunction)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的CHTL JS函数类型: " + chtlJSFunction + "，只支持 listen、delegate、animate"
        );
        return;
    }
    
    // 记录CHTL JS函数类型
    virtualObjectCHTLJSFunction_[objectName] = chtlJSFunction;
    
    // 记录函数键
    virtualObjectFunctionTable_[objectName] = functionKeys;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "为虚对象 '" + objectName + "' 解析 " + chtlJSFunction + " 函数，包含 " + 
        std::to_string(functionKeys.size()) + " 个函数键"
    );
    
    // 输出函数键详情
    for (const auto& key : functionKeys) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "  - 函数键: " + key
        );
    }
}

std::string CHTLJSView::ProcessVirtualMethodCall(const std::string& objectName, 
                                               const std::string& methodName) {
    // 1. 检查虚对象是否存在
    if (!HasVirtualObject(objectName)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "虚对象 '" + objectName + "' 不存在"
        );
        return "";
    }
    
    // 2. 检查函数键是否存在
    if (!HasFunctionKey(objectName, methodName)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "虚对象 '" + objectName + "' 没有函数键 '" + methodName + "'"
        );
        return "";
    }
    
    // 3. 检查全局函数注册表，防止重复创建
    std::string callKey = objectName + "->" + methodName;
    auto it = globalFunctionRegistry_.find(callKey);
    
    if (it != globalFunctionRegistry_.end()) {
        // 已存在，直接返回函数引用
        Utils::ErrorHandler::GetInstance().LogInfo(
            "使用已存在的全局函数: " + it->second + " (对应 " + callKey + ")"
        );
        return it->second;
    }
    
    // 4. 创建新的全局函数并加入注册表
    std::string globalFunctionName = GenerateGlobalFunctionName(objectName, methodName);
    globalFunctionRegistry_[callKey] = globalFunctionName;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "创建全局函数: " + globalFunctionName + " (对应 " + callKey + ")"
    );
    
    return globalFunctionName;
}

bool CHTLJSView::HasVirtualObject(const std::string& objectName) const {
    return std::find(virtualObjectNames_.begin(), virtualObjectNames_.end(), objectName) 
           != virtualObjectNames_.end();
}

bool CHTLJSView::HasFunctionKey(const std::string& objectName, const std::string& functionKey) const {
    auto it = virtualObjectFunctionTable_.find(objectName);
    if (it == virtualObjectFunctionTable_.end()) {
        return false;
    }
    
    const auto& functionKeys = it->second;
    return std::find(functionKeys.begin(), functionKeys.end(), functionKey) 
           != functionKeys.end();
}

std::vector<std::string> CHTLJSView::GetFunctionKeys(const std::string& objectName) const {
    auto it = virtualObjectFunctionTable_.find(objectName);
    if (it != virtualObjectFunctionTable_.end()) {
        return it->second;
    }
    return {};
}

const std::unordered_map<std::string, std::string>& CHTLJSView::GetGlobalFunctionRegistry() const {
    return globalFunctionRegistry_;
}

void CHTLJSView::Clear() {
    virtualObjectNames_.clear();
    virtualObjectFunctionTable_.clear();
    virtualObjectCHTLJSFunction_.clear();
    globalFunctionRegistry_.clear();
    globalFunctionCounter_ = 0;
    
    Utils::ErrorHandler::GetInstance().LogInfo("清空虚对象View管理器");
}

std::string CHTLJSView::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL JS虚对象View统计:\n";
    oss << "  虚对象数量: " << virtualObjectNames_.size() << "\n";
    oss << "  全局函数注册数量: " << globalFunctionRegistry_.size() << "\n";
    oss << "  下一个函数计数: " << globalFunctionCounter_ << "\n";
    
    for (const auto& objectName : virtualObjectNames_) {
        auto functionIt = virtualObjectFunctionTable_.find(objectName);
        auto typeIt = virtualObjectCHTLJSFunction_.find(objectName);
        
        std::string functionType = (typeIt != virtualObjectCHTLJSFunction_.end()) ? 
                                 typeIt->second : "unknown";
        int functionCount = (functionIt != virtualObjectFunctionTable_.end()) ? 
                          functionIt->second.size() : 0;
        
        oss << "  - " << objectName << " (" << functionType << "): " 
            << functionCount << " 个函数键\n";
    }
    
    return oss.str();
}

std::string CHTLJSView::GenerateGlobalFunctionName(const std::string& objectName, 
                                                 const std::string& methodName) {
    // 生成唯一的全局函数名
    // 格式: __chtljs_vir_{objectName}_{methodName}_{counter}
    std::string functionName = "__chtljs_vir_" + objectName + "_" + methodName + "_" + 
                              std::to_string(++globalFunctionCounter_);
    
    return functionName;
}

bool CHTLJSView::IsValidCHTLJSFunction(const std::string& functionType) const {
    return functionType == "listen" || 
           functionType == "delegate" || 
           functionType == "animate";
}

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL