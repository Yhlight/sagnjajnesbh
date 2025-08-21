#include <iostream>
#include <cassert>
#include "chtl/helpers/CHTLStateContextHelper.h"
#include "chtl_js/helpers/CHTLJSStateContextHelper.h"
#include "chtl/state/CHTLState.h"
#include "chtl/context/CHTLContext.h"
#include "chtl_js/state/CHTLJSState.h"
#include "chtl_js/context/CHTLJSContext.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl_js/ast/EnhancedSelectorNode.h"
#include "chtl_js/ast/VirNodes.h"
#include "utils/Logger.h"

using namespace chtl;
using namespace chtl::compiler;
using namespace chtl::compiler::helpers;
using namespace chtl::chtljs;
using namespace chtl::chtljs::helpers;

void TestCHTLStateContextHelper() {
    std::cout << "测试CHTL状态上下文协助器..." << std::endl;
    
    // 创建状态机和上下文
    CHTLStateMachine stateMachine;
    CHTLContext context;
    CHTLStateContextHelper helper(stateMachine, context);
    
    // 测试AST节点状态管理
    {
        auto element = std::make_shared<ast::ElementNode>("div");
        
        // 创建节点守卫
        auto guard = helper.CreateNodeGuard(
            element.get(),
            CHTLCompilerState::IN_ELEMENT,
            CHTLScopeType::ELEMENT
        );
        
        // 更新节点信息
        guard->UpdateNodeInfo([](ASTNodeStateInfo& info) {
            info.identifier = "div#main";
            info.depth = 1;
        });
        
        // 验证节点
        assert(guard->Validate());
        
        std::cout << "  元素节点状态管理测试通过" << std::endl;
    }
    
    // 测试样式块约束
    {
        auto styleBlock = std::make_shared<ast::StyleBlockNode>();
        styleBlock->SetIsLocal(true);
        
        // 获取局部样式块的约束
        auto constraints = helper.GetNodeConstraints(
            ast::ASTNodeType::STYLE_BLOCK,
            CHTLCompilerState::IN_LOCAL_STYLE,
            CHTLScopeType::STYLE_BLOCK
        );
        
        assert(constraints.allowsCHTLSyntax == true);
        assert(constraints.allowsVariables == true);
        assert(constraints.allowsTemplates == true);
        assert(constraints.allowsCustomElements == false);
        assert(constraints.allowsOriginEmbedding == true);
        
        std::cout << "  样式块约束测试通过" << std::endl;
    }
    
    // 测试脚本块约束
    {
        // 局部脚本
        auto localConstraints = helper.GetNodeConstraints(
            ast::ASTNodeType::SCRIPT_BLOCK,
            CHTLCompilerState::IN_LOCAL_SCRIPT,
            CHTLScopeType::GLOBAL
        );
        
        assert(localConstraints.allowsCHTLSyntax == true);
        assert(localConstraints.allowsVariables == true);
        assert(localConstraints.allowsTemplates == false);
        
        // 全局脚本
        auto globalConstraints = helper.GetNodeConstraints(
            ast::ASTNodeType::SCRIPT_BLOCK,
            CHTLCompilerState::IN_GLOBAL_SCRIPT,
            CHTLScopeType::GLOBAL
        );
        
        assert(globalConstraints.allowsCHTLSyntax == false);
        assert(globalConstraints.allowsVariables == false);
        assert(globalConstraints.allowsOriginEmbedding == true);
        
        std::cout << "  脚本块约束测试通过" << std::endl;
    }
    
    // 测试批量操作
    helper.WithStateContext(
        CHTLCompilerState::IN_ELEMENT,
        CHTLScopeType::ELEMENT,
        "batch-test",
        [&]() {
            auto state = stateMachine.GetCurrentState();
            assert(state == CHTLCompilerState::IN_ELEMENT);
            
            auto scope = context.GetCurrentScope();
            assert(scope && scope->type == CHTLScopeType::ELEMENT);
        }
    );
    
    std::cout << "  批量操作测试通过" << std::endl;
    
    // 测试解析器状态协助器
    {
        ParserStateHelper parserHelper(helper);
        
        auto elementGuard = parserHelper.ParseElement("button");
        elementGuard->EnterAttribute("onclick");
        elementGuard->ExitAttribute();
        elementGuard->EnterBody();
        elementGuard->MarkComplete();
        
        std::cout << "  解析器状态协助器测试通过" << std::endl;
    }
    
    // 测试生成器状态协助器
    {
        GeneratorStateHelper generatorHelper(helper);
        
        auto element = std::make_shared<ast::ElementNode>("span");
        auto genGuard = generatorHelper.BeginGeneration(element.get(), "html");
        
        assert(genGuard->ShouldGenerate());
        genGuard->MarkGenerated();
        
        std::cout << "  生成器状态协助器测试通过" << std::endl;
    }
    
    std::cout << "CHTL状态上下文协助器测试完成!" << std::endl;
}

void TestCHTLJSStateContextHelper() {
    std::cout << "\n测试CHTL JS状态上下文协助器..." << std::endl;
    
    // 创建状态机和上下文
    CHTLJSStateMachine stateMachine;
    CHTLJSContext context;
    CHTLJSStateContextHelper helper(stateMachine, context);
    
    // 测试增强选择器状态管理
    {
        auto selector = std::make_shared<ast::EnhancedSelectorNode>("{{.box}}");
        
        auto guard = helper.CreateNodeGuard(
            selector.get(),
            CHTLJSStateType::ENHANCED_SELECTOR
        );
        
        guard->UpdateNodeInfo([](CHTLJSNodeStateInfo& info) {
            info.identifier = "{{.box}}";
        });
        
        assert(guard->Validate());
        
        // 注册选择器
        helper.RegisterSelector(".box");
        auto selectors = helper.GetUsedSelectors();
        assert(!selectors.empty());
        assert(selectors[0] == ".box");
        
        std::cout << "  增强选择器状态管理测试通过" << std::endl;
    }
    
    // 测试虚对象状态管理
    {
        auto virDecl = std::make_shared<ast::VirDeclarationNode>("EventHandler");
        
        auto guard = helper.CreateNodeGuard(
            virDecl.get(),
            CHTLJSStateType::VIR_DECLARATION
        );
        
        // 注册虚对象
        helper.RegisterVirObject("EventHandler", {"click", "submit"});
        
        assert(helper.IsVirObject("EventHandler"));
        auto keys = helper.GetVirFunctionKeys("EventHandler");
        assert(keys.size() == 2);
        assert(keys[0] == "click");
        assert(keys[1] == "submit");
        
        std::cout << "  虚对象状态管理测试通过" << std::endl;
    }
    
    // 测试CHTL JS约束
    {
        auto constraints = helper.GetCurrentConstraints();
        
        assert(constraints.allowsEnhancedSelector == true);
        assert(constraints.allowsArrowOperator == true);
        assert(constraints.allowsVirDeclaration == true);
        assert(constraints.allowsCHTLJSFunctions == true);
        assert(constraints.allowsJavaScriptCode == true);
        
        std::cout << "  CHTL JS约束测试通过" << std::endl;
    }
    
    // 测试解析器状态协助器
    {
        CHTLJSParserStateHelper parserHelper(helper);
        
        // 测试增强选择器解析
        parserHelper.EnterEnhancedSelector();
        assert(stateMachine.GetCurrentState() == CHTLJSStateType::ENHANCED_SELECTOR);
        parserHelper.ExitEnhancedSelector();
        
        // 测试虚对象解析
        parserHelper.EnterVirDeclaration("MyVir");
        assert(stateMachine.GetCurrentState() == CHTLJSStateType::VIR_DECLARATION);
        parserHelper.ExitVirDeclaration();
        
        // 测试CHTL JS函数解析
        parserHelper.EnterCHTLJSFunction("listen");
        assert(stateMachine.GetCurrentState() == CHTLJSStateType::CHTLJS_FUNCTION);
        parserHelper.ExitCHTLJSFunction();
        
        std::cout << "  CHTL JS解析器状态协助器测试通过" << std::endl;
    }
    
    // 测试生成器状态协助器
    {
        CHTLJSGeneratorStateHelper generatorHelper(helper);
        
        // 注册运行时函数
        generatorHelper.RegisterRuntimeFunction("__chtljs_select");
        generatorHelper.RegisterRuntimeFunction("__chtljs_listen");
        
        assert(generatorHelper.IsRuntimeFunctionRegistered("__chtljs_select"));
        assert(generatorHelper.IsRuntimeFunctionRegistered("__chtljs_listen"));
        
        auto runtimeFuncs = generatorHelper.GetRuntimeFunctions();
        assert(runtimeFuncs.size() == 2);
        
        // 测试虚对象函数名生成
        auto selector = std::make_shared<ast::EnhancedSelectorNode>("{{#app}}");
        auto genGuard = generatorHelper.BeginGeneration(selector.get(), "javascript");
        
        auto virFuncName = genGuard->GenerateVirFunctionName("Test", "click");
        assert(virFuncName == "__chtljs_vir_Test_click");
        
        std::cout << "  CHTL JS生成器状态协助器测试通过" << std::endl;
    }
    
    std::cout << "CHTL JS状态上下文协助器测试完成!" << std::endl;
}

void TestRAIIFeatures() {
    std::cout << "\n测试RAII特性..." << std::endl;
    
    // 测试CHTL状态守卫的RAII行为
    {
        CHTLStateMachine stateMachine;
        assert(stateMachine.GetCurrentState() == CHTLCompilerState::INITIAL);
        
        {
            CHTLStateGuard guard(stateMachine, CHTLCompilerState::IN_ELEMENT);
            assert(stateMachine.GetCurrentState() == CHTLCompilerState::IN_ELEMENT);
            
            {
                CHTLStateGuard innerGuard(stateMachine, CHTLCompilerState::IN_ATTRIBUTE);
                assert(stateMachine.GetCurrentState() == CHTLCompilerState::IN_ATTRIBUTE);
            }
            // innerGuard析构，自动退出IN_ATTRIBUTE状态
            assert(stateMachine.GetCurrentState() == CHTLCompilerState::IN_ELEMENT);
        }
        // guard析构，自动退出IN_ELEMENT状态
        assert(stateMachine.GetCurrentState() == CHTLCompilerState::INITIAL);
        
        std::cout << "  CHTL状态守卫RAII测试通过" << std::endl;
    }
    
    // 测试CHTL作用域守卫的RAII行为
    {
        CHTLContext context;
        
        {
            CHTLScopeGuard guard(context, CHTLScopeType::ELEMENT, "div");
            auto scope = context.GetCurrentScope();
            assert(scope && scope->type == CHTLScopeType::ELEMENT);
            assert(scope->name == "div");
        }
        // guard析构，自动退出作用域
        
        std::cout << "  CHTL作用域守卫RAII测试通过" << std::endl;
    }
    
    // 测试CHTL JS状态守卫的RAII行为
    {
        CHTLJSStateMachine stateMachine;
        assert(stateMachine.GetCurrentState() == CHTLJSStateType::INITIAL);
        
        {
            CHTLJSStateGuard guard(stateMachine, CHTLJSStateType::SCRIPT);
            assert(stateMachine.GetCurrentState() == CHTLJSStateType::SCRIPT);
            
            {
                CHTLJSStateGuard innerGuard(stateMachine, CHTLJSStateType::VIR_DECLARATION);
                assert(stateMachine.GetCurrentState() == CHTLJSStateType::VIR_DECLARATION);
            }
            assert(stateMachine.GetCurrentState() == CHTLJSStateType::SCRIPT);
        }
        assert(stateMachine.GetCurrentState() == CHTLJSStateType::INITIAL);
        
        std::cout << "  CHTL JS状态守卫RAII测试通过" << std::endl;
    }
    
    // 测试异常安全
    {
        CHTLStateMachine stateMachine;
        CHTLContext context;
        CHTLStateContextHelper helper(stateMachine, context);
        
        try {
            CHTLStateGuard guard(stateMachine, CHTLCompilerState::IN_ELEMENT);
            CHTLScopeGuard scopeGuard(context, CHTLScopeType::ELEMENT);
            
            // 模拟异常
            throw std::runtime_error("测试异常");
        } catch (...) {
            // 即使发生异常，守卫也会正确清理
            assert(stateMachine.GetCurrentState() == CHTLCompilerState::INITIAL);
        }
        
        std::cout << "  异常安全测试通过" << std::endl;
    }
    
    std::cout << "RAII特性测试完成!" << std::endl;
}

void TestConstraintSystem() {
    std::cout << "\n测试约束系统..." << std::endl;
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    CHTLStateContextHelper helper(stateMachine, context);
    
    // 根据目标规划文档测试约束
    
    // 测试全局样式块约束（第137行）
    {
        auto constraints = helper.GetNodeConstraints(
            ast::ASTNodeType::STYLE_BLOCK,
            CHTLCompilerState::IN_GLOBAL_STYLE,
            CHTLScopeType::GLOBAL
        );
        
        assert(constraints.allowsCHTLSyntax == true);
        assert(constraints.allowsVariables == true);      // 允许模板变量、自定义变量
        assert(constraints.allowsTemplates == true);      // 允许模板样式组、自定义样式组
        assert(constraints.allowsCustomElements == false);
        assert(constraints.allowsOriginEmbedding == true); // 原始嵌入任何地方都可以
        
        std::cout << "  全局样式块约束测试通过" << std::endl;
    }
    
    // 测试局部脚本约束（第143行）
    {
        auto constraints = helper.GetNodeConstraints(
            ast::ASTNodeType::SCRIPT_BLOCK,
            CHTLCompilerState::IN_LOCAL_SCRIPT,
            CHTLScopeType::GLOBAL
        );
        
        assert(constraints.allowsCHTLSyntax == true);
        assert(constraints.allowsVariables == true);      // 允许模板变量、自定义变量组
        assert(constraints.allowsTemplates == false);     // 不允许模板
        assert(constraints.allowsCustomElements == false);
        assert(constraints.allowsOriginEmbedding == true); // 允许--注释和原始嵌入
        
        std::cout << "  局部脚本约束测试通过" << std::endl;
    }
    
    // 测试全局脚本约束（第139行）
    {
        auto constraints = helper.GetNodeConstraints(
            ast::ASTNodeType::SCRIPT_BLOCK,
            CHTLCompilerState::IN_GLOBAL_SCRIPT,
            CHTLScopeType::GLOBAL
        );
        
        assert(constraints.allowsCHTLSyntax == false);    // 禁止任何CHTL语法
        assert(constraints.allowsVariables == false);     // 禁止模板变量
        assert(constraints.allowsTemplates == false);
        assert(constraints.allowsCustomElements == false);
        assert(constraints.allowsOriginEmbedding == true); // 只允许--注释和原始嵌入
        
        std::cout << "  全局脚本约束测试通过" << std::endl;
    }
    
    std::cout << "约束系统测试完成!" << std::endl;
}

int main() {
    // 设置日志级别
    utils::Logger::GetInstance().SetLevel(utils::LogLevel::INFO);
    
    std::cout << "运行RAII状态上下文协助器测试...\n" << std::endl;
    std::cout << "注意：CHTL和CHTL JS的状态管理完全独立\n" << std::endl;
    
    TestCHTLStateContextHelper();
    TestCHTLJSStateContextHelper();
    TestRAIIFeatures();
    TestConstraintSystem();
    
    std::cout << "\n所有测试完成!" << std::endl;
    std::cout << "RAII模式确保了状态和作用域的自动管理" << std::endl;
    std::cout << "为解析器和生成器提供了更精确的状态控制" << std::endl;
    
    return 0;
}