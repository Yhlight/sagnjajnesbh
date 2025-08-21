#include <iostream>
#include <cassert>
#include <memory>
#include "chtl_js/ast/CHTLJSDocumentNode.h"
#include "chtl_js/ast/ScriptBlockNode.h"
#include "chtl_js/ast/EnhancedSelectorNode.h"
#include "chtl_js/ast/OperatorNodes.h"
#include "chtl_js/ast/SpecialFunctionNodes.h"
#include "chtl_js/ast/VirNodes.h"
#include "chtl_js/ast/StateTagNode.h"
#include "chtl_js/ast/ObjectLiteralNode.h"
#include "chtl_js/ast/LiteralNodes.h"
#include "chtl_js/ast/FunctionNodes.h"
#include "chtl_js/ast/JSCodeFragmentNode.h"
#include "chtl_js/ast/CHTLJSASTBuilder.h"
#include "utils/Logger.h"

using namespace chtl::chtljs::ast;

void TestBasicNodes() {
    std::cout << "测试基本CHTL JS AST节点..." << std::endl;
    
    // 创建文档节点
    auto doc = std::make_shared<CHTLJSDocumentNode>();
    doc->SetSource("test.chtl");
    doc->SetIsLocal(true);
    assert(doc->GetSource() == "test.chtl");
    assert(doc->IsLocal());
    assert(doc->GetType() == CHTLJSNodeType::DOCUMENT);
    
    // 创建script块节点
    auto scriptBlock = std::make_shared<ScriptBlockNode>();
    scriptBlock->SetIsLocal(true);
    scriptBlock->SetOwnerElement("div");
    assert(scriptBlock->IsLocal());
    assert(scriptBlock->GetOwnerElement() == "div");
    
    doc->AddChild(scriptBlock);
    assert(doc->GetChildCount() == 1);
    
    std::cout << "基本CHTL JS节点测试通过!" << std::endl;
}

void TestEnhancedSelector() {
    std::cout << "测试增强选择器节点..." << std::endl;
    
    // 测试各种选择器类型
    auto sel1 = std::make_shared<EnhancedSelectorNode>("box");
    assert(sel1->GetSelectorType() == EnhancedSelectorNode::TAG);
    assert(sel1->GetSelector() == "box");
    
    auto sel2 = std::make_shared<EnhancedSelectorNode>(".box");
    assert(sel2->GetSelectorType() == EnhancedSelectorNode::CLASS);
    
    auto sel3 = std::make_shared<EnhancedSelectorNode>("#box");
    assert(sel3->GetSelectorType() == EnhancedSelectorNode::ID);
    
    auto sel4 = std::make_shared<EnhancedSelectorNode>("button[0]");
    assert(sel4->GetSelectorType() == EnhancedSelectorNode::INDEXED);
    assert(sel4->HasIndex());
    
    auto sel5 = std::make_shared<EnhancedSelectorNode>(".box button");
    assert(sel5->GetSelectorType() == EnhancedSelectorNode::DESCENDANT);
    assert(sel5->IsCompoundSelector());
    
    // 测试选择器访问
    auto access = std::make_shared<SelectorAccessNode>();
    access->SetSelector(sel1);
    access->SetIndex(2);
    assert(access->GetIndex() == 2);
    
    std::cout << "增强选择器测试通过!" << std::endl;
}

void TestOperators() {
    std::cout << "测试操作符节点..." << std::endl;
    
    // 测试箭头操作符
    auto arrow = std::make_shared<ArrowOperatorNode>();
    auto left = std::make_shared<EnhancedSelectorNode>("{{box}}");
    auto right = std::make_shared<IdentifierNode>("textContent");
    
    arrow->SetLeft(left);
    arrow->SetRight(right);
    arrow->SetIsMethodCall(true);
    
    assert(arrow->GetLeft() == left);
    assert(arrow->GetRight() == right);
    assert(arrow->IsMethodCall());
    
    // 测试点操作符
    auto dot = std::make_shared<DotOperatorNode>();
    dot->SetLeft(left);
    dot->SetRight(right);
    
    assert(dot->GetLeft() == left);
    
    std::cout << "操作符节点测试通过!" << std::endl;
}

void TestSpecialFunctions() {
    std::cout << "测试CHTL JS特殊函数..." << std::endl;
    
    // 测试listen调用
    auto listen = std::make_shared<ListenCallNode>();
    auto config = std::make_shared<ObjectLiteralNode>();
    
    // 添加事件处理器
    auto clickProp = std::make_shared<PropertyNode>("click");
    auto clickHandler = std::make_shared<ArrowFunctionNode>();
    clickProp->SetValue(clickHandler);
    config->AddProperty(clickProp);
    
    listen->SetConfig(config);
    assert(listen->GetConfig() == config);
    assert(config->HasProperty("click"));
    
    // 测试delegate调用
    auto delegate = std::make_shared<DelegateCallNode>();
    auto delegateConfig = std::make_shared<ObjectLiteralNode>();
    
    auto targetProp = std::make_shared<PropertyNode>("target");
    auto targetValue = std::make_shared<EnhancedSelectorNode>("{{.item}}");
    targetProp->SetValue(targetValue);
    delegateConfig->AddProperty(targetProp);
    
    delegate->SetConfig(delegateConfig);
    assert(delegateConfig->HasTarget());
    
    // 测试animate调用
    auto animate = std::make_shared<AnimateCallNode>();
    auto animConfig = std::make_shared<ObjectLiteralNode>();
    
    auto durationProp = std::make_shared<PropertyNode>("duration");
    auto durationValue = std::make_shared<NumberLiteralNode>(1000);
    durationProp->SetValue(durationValue);
    animConfig->AddProperty(durationProp);
    
    animate->SetConfig(animConfig);
    assert(animConfig->HasDuration());
    
    // 测试iNeverAway调用
    auto iNeverAway = std::make_shared<INeverAwayCallNode>();
    auto inaConfig = std::make_shared<ObjectLiteralNode>();
    
    // 添加带状态标签的函数
    auto funcProp = std::make_shared<PropertyNode>("Void");
    auto stateTag = std::make_shared<StateTagNode>("A");
    funcProp->SetStateTag(stateTag);
    inaConfig->AddProperty(funcProp);
    
    iNeverAway->SetConfig(inaConfig);
    assert(funcProp->HasStateTag());
    
    // 测试printMylove调用
    auto printMylove = std::make_shared<PrintMyLoveCallNode>();
    auto pmlConfig = std::make_shared<ObjectLiteralNode>();
    
    auto srcProp = std::make_shared<PropertyNode>("src");
    auto srcValue = std::make_shared<StringLiteralNode>("image.png");
    srcProp->SetValue(srcValue);
    pmlConfig->AddProperty(srcProp);
    
    printMylove->SetConfig(pmlConfig);
    assert(pmlConfig->HasSrc());
    
    std::cout << "CHTL JS特殊函数测试通过!" << std::endl;
}

void TestVirNodes() {
    std::cout << "测试虚对象节点..." << std::endl;
    
    // 测试虚对象声明
    auto virDecl = std::make_shared<VirDeclarationNode>("Test");
    auto listenCall = std::make_shared<ListenCallNode>();
    virDecl->SetInitializer(listenCall);
    virDecl->AddFunctionKey("click");
    virDecl->AddFunctionKey("mouseenter");
    
    assert(virDecl->GetVirName() == "Test");
    assert(virDecl->GetInitializer() == listenCall);
    assert(virDecl->GetFunctionKeys().size() == 2);
    
    // 测试虚对象访问
    auto virAccess = std::make_shared<VirAccessNode>();
    virAccess->SetVirObject("Test");
    virAccess->SetMember("click");
    virAccess->SetUseArrow(true);
    virAccess->SetIsMethodCall(true);
    
    assert(virAccess->GetVirObject() == "Test");
    assert(virAccess->GetMember() == "click");
    assert(virAccess->UseArrow());
    assert(virAccess->IsMethodCall());
    
    std::cout << "虚对象节点测试通过!" << std::endl;
}

void TestStateTag() {
    std::cout << "测试状态标签节点..." << std::endl;
    
    // 测试状态标签
    auto stateTag = std::make_shared<StateTagNode>("Active");
    stateTag->SetIsForDeclaration(true);
    assert(stateTag->GetStateName() == "Active");
    assert(stateTag->IsForDeclaration());
    
    // 测试带状态的函数
    auto funcWithState = std::make_shared<FunctionWithStateNode>("Void");
    funcWithState->SetStateTag(stateTag);
    funcWithState->SetIsDeclaration(true);
    
    auto params = std::make_shared<ParameterListNode>();
    params->AddParameter("x");
    params->AddParameter("y");
    funcWithState->SetParameters(params);
    
    assert(funcWithState->GetFunctionName() == "Void");
    assert(funcWithState->GetStateTag() == stateTag);
    assert(funcWithState->IsDeclaration());
    
    std::cout << "状态标签测试通过!" << std::endl;
}

void TestObjectLiteral() {
    std::cout << "测试对象字面量节点..." << std::endl;
    
    auto obj = std::make_shared<ObjectLiteralNode>();
    
    // 添加各种类型的属性
    auto strProp = std::make_shared<PropertyNode>("name");
    strProp->SetValue(std::make_shared<StringLiteralNode>("CHTL JS"));
    obj->AddProperty(strProp);
    
    auto numProp = std::make_shared<PropertyNode>("version");
    numProp->SetValue(std::make_shared<NumberLiteralNode>(1.0));
    obj->AddProperty(numProp);
    
    auto boolProp = std::make_shared<PropertyNode>("enabled");
    boolProp->SetValue(std::make_shared<BooleanLiteralNode>(true));
    obj->AddProperty(boolProp);
    
    auto funcProp = std::make_shared<PropertyNode>("onClick");
    funcProp->SetValue(std::make_shared<ArrowFunctionNode>());
    obj->AddProperty(funcProp);
    
    assert(obj->GetProperties().size() == 4);
    assert(obj->HasProperty("name"));
    assert(obj->GetProperty("version") != nullptr);
    assert(funcProp->IsFunctionProperty());
    
    std::cout << "对象字面量测试通过!" << std::endl;
}

void TestJSCodeFragment() {
    std::cout << "测试JavaScript代码片段..." << std::endl;
    
    auto jsCode = std::make_shared<JSCodeFragmentNode>("console.log('Hello');");
    jsCode->SetContext(JSCodeFragmentNode::FUNCTION_BODY);
    jsCode->SetNeedsSemicolon(true);
    
    assert(jsCode->GetCode() == "console.log('Hello');");
    assert(jsCode->GetContext() == JSCodeFragmentNode::FUNCTION_BODY);
    assert(jsCode->NeedsSemicolon());
    
    std::cout << "JavaScript代码片段测试通过!" << std::endl;
}

void TestCompleteStructure() {
    std::cout << "测试完整的CHTL JS AST结构..." << std::endl;
    
    // 构建一个完整的CHTL JS文档
    auto doc = std::make_shared<CHTLJSDocumentNode>();
    auto scriptBlock = std::make_shared<ScriptBlockNode>();
    
    // 虚对象声明
    auto virDecl = std::make_shared<VirDeclarationNode>("EventHandler");
    auto listenConfig = std::make_shared<ObjectLiteralNode>();
    
    auto clickProp = std::make_shared<PropertyNode>("click");
    auto clickHandler = std::make_shared<ArrowFunctionNode>();
    clickHandler->SetIsExpression(false);
    clickProp->SetValue(clickHandler);
    listenConfig->AddProperty(clickProp);
    
    auto listenCall = std::make_shared<ListenCallNode>();
    listenCall->SetConfig(listenConfig);
    virDecl->SetInitializer(listenCall);
    
    scriptBlock->AddChild(virDecl);
    
    // 增强选择器使用
    auto selector = std::make_shared<EnhancedSelectorNode>("{{.box}}");
    auto arrow = std::make_shared<ArrowOperatorNode>();
    arrow->SetLeft(selector);
    
    auto delegateCall = std::make_shared<DelegateCallNode>();
    auto delegateConfig = std::make_shared<ObjectLiteralNode>();
    delegateCall->SetConfig(delegateConfig);
    arrow->SetRight(delegateCall);
    
    scriptBlock->AddChild(arrow);
    
    // 虚对象访问
    auto virAccess = std::make_shared<VirAccessNode>();
    virAccess->SetVirObject("EventHandler");
    virAccess->SetMember("click");
    virAccess->SetIsMethodCall(true);
    
    scriptBlock->AddChild(virAccess);
    
    doc->AddChild(scriptBlock);
    
    // 验证结构
    assert(doc->GetChildCount() == 1);
    assert(scriptBlock->GetChildCount() == 3);
    
    std::cout << "完整CHTL JS AST结构测试通过!" << std::endl;
}

int main() {
    // 设置日志级别
    chtl::utils::Logger::GetInstance().SetLevel(chtl::utils::LogLevel::INFO);
    
    std::cout << "运行CHTL JS AST节点测试..." << std::endl;
    std::cout << "注意：CHTL JS AST完全独立于CHTL AST" << std::endl << std::endl;
    
    TestBasicNodes();
    TestEnhancedSelector();
    TestOperators();
    TestSpecialFunctions();
    TestVirNodes();
    TestStateTag();
    TestObjectLiteral();
    TestJSCodeFragment();
    TestCompleteStructure();
    
    std::cout << "\n所有CHTL JS AST测试通过!" << std::endl;
    std::cout << "CHTL JS不是JavaScript，是CHTL的扩展语法" << std::endl;
    return 0;
}