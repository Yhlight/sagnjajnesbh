#include "../common/CompilerCore.h"
#include "../common/ContextManager.h"
#include "../ast/ASTStateManager.h"
#include "../ast/NodeStateTracker.h"
#include "../parser/ContextAwareParser.h"
#include <iostream>
#include <string>

using namespace chtl;
using namespace chtl::ast;
using namespace chtl::parser;

// 示例：基于RAII的状态机与上下文管理使用
void demonstrateRAIIStateManagement() {
    std::cout << "=== RAII状态机与上下文管理示例 ===\n\n";
    
    // 1. 创建编译器核心，自动集成状态管理
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    auto& ast_state_manager = context_manager.getASTStateManager();
    
    // 2. 创建状态跟踪器
    TrackingConfig tracking_config;
    tracking_config.enable_history = true;
    tracking_config.enable_snapshots = true;
    tracking_config.enable_rollback = true;
    
    auto state_tracker = std::make_shared<NodeStateTracker>(ast_state_manager, tracking_config);
    
    // 3. 示例CHTL代码（严格按照语法规范）
    std::string chtl_code = R"(
        // CHTL示例代码
        [Template] @Style DefaultButton
        {
            background-color: blue;
            color: white;
            padding: 10px;
        }
        
        [Custom] @Element CustomButton
        {
            button
            {
                style
                {
                    @Style DefaultButton;
                    border-radius: 5px;
                }
                
                text
                {
                    Click Me
                }
            }
        }
        
        html
        {
            head
            {
                // 页面头部
            }
            
            body
            {
                div
                {
                    id: container;
                    class: main-content;
                    
                    style
                    {
                        .container
                        {
                            width: 100%;
                            max-width: 1200px;
                        }
                        
                        &:hover
                        {
                            background-color: #f0f0f0;
                        }
                    }
                    
                    @Element CustomButton;
                    
                    script
                    {
                        {{.container}}->listen({
                            click: () => {
                                console.log("Container clicked");
                            }
                        });
                    }
                }
            }
        }
    )";
    
    std::cout << "4. 开始编译过程...\n";
    
    // 4. 使用RAII守护器管理编译过程
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::INITIALIZATION);
        std::cout << "   - 初始化阶段: " << context_manager.getPhaseDescription() << "\n";
        
        // 创建回滚点
        CHTL_CREATE_ROLLBACK_POINT(*state_tracker, "compilation_start");
    }
    
    // 5. 词法分析阶段
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::LEXICAL_SCANNING);
        std::cout << "   - 词法分析阶段: " << context_manager.getPhaseDescription() << "\n";
        
        if (!compiler->lexicalAnalysis(chtl_code)) {
            std::cout << "   错误：词法分析失败\n";
            return;
        }
    }
    
    // 6. 语法分析阶段
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::SYNTAX_PARSING);
        std::cout << "   - 语法分析阶段: " << context_manager.getPhaseDescription() << "\n";
        
        // 创建上下文感知解析器
        auto parser = ContextAwareParserFactory::createStandardParser(context_manager);
        parser->setStateTracker(state_tracker);
        
        // 使用RAII上下文守护器
        {
            CHTL_SCOPE_GUARD(context_manager, ContextScope::FILE, "example.chtl");
            
            if (!compiler->syntaxAnalysis()) {
                std::cout << "   错误：语法分析失败\n";
                
                // 演示错误恢复
                if (state_tracker->hasRollbackPoint("compilation_start")) {
                    std::cout << "   尝试回滚到编译开始点...\n";
                    state_tracker->rollbackToPoint("compilation_start");
                }
                return;
            }
        }
    }
    
    // 7. 语义分析阶段
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::SEMANTIC_ANALYSIS);
        std::cout << "   - 语义分析阶段: " << context_manager.getPhaseDescription() << "\n";
        
        if (!compiler->semanticAnalysis()) {
            std::cout << "   错误：语义分析失败\n";
            return;
        }
    }
    
    // 8. 代码生成阶段
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::CODE_GENERATION);
        std::cout << "   - 代码生成阶段: " << context_manager.getPhaseDescription() << "\n";
        
        if (!compiler->codeGeneration()) {
            std::cout << "   错误：代码生成失败\n";
            return;
        }
    }
    
    // 9. 完成阶段
    {
        CHTL_PHASE_GUARD(context_manager, CompilationPhase::FINALIZATION);
        std::cout << "   - 完成阶段: " << context_manager.getPhaseDescription() << "\n";
        
        compiler->finalize();
    }
    
    // 10. 获取编译结果
    auto result = compiler->getCompilationResult();
    
    std::cout << "\n=== 编译结果 ===\n";
    std::cout << "编译成功: " << (result.success ? "是" : "否") << "\n";
    std::cout << "编译时间: " << result.compilation_time_ms << " ms\n";
    std::cout << "错误数量: " << result.errors.size() << "\n";
    std::cout << "警告数量: " << result.warnings.size() << "\n";
    
    if (result.success) {
        std::cout << "\n生成的代码:\n";
        std::cout << result.generated_code << "\n";
    }
    
    // 11. 展示状态管理统计
    std::cout << "\n=== 状态管理统计 ===\n";
    std::cout << context_manager.getStatistics() << "\n";
    std::cout << state_tracker->getTrackingStatistics() << "\n";
    
    // 12. 展示AST节点状态
    if (result.ast) {
        std::cout << "\n=== AST节点状态 ===\n";
        ast_state_manager.printNodeStates();
        
        // 演示节点状态查询
        auto nodes_in_generated = ast_state_manager.getNodesInState(NodeState::GENERATED);
        std::cout << "已生成状态的节点数量: " << nodes_in_generated.size() << "\n";
    }
}

// 示例：演示错误恢复和状态回滚
void demonstrateErrorRecoveryAndRollback() {
    std::cout << "\n=== 错误恢复和状态回滚示例 ===\n\n";
    
    // 创建带有语法错误的CHTL代码
    std::string invalid_chtl_code = R"(
        // 故意包含语法错误的CHTL代码
        [Template] @Style InvalidStyle
        {
            // 缺少分号的CSS属性
            background-color: red
            color: white;
        }
        
        html
        {
            body
            {
                // 无效的元素名
                invalid-element
                {
                    // 错误的属性语法
                    invalid-attr = "value" without-colon;
                }
            }
        }
    )";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    auto& ast_state_manager = context_manager.getASTStateManager();
    
    auto state_tracker = std::make_shared<NodeStateTracker>(ast_state_manager);
    
    // 创建初始回滚点
    auto rollback_id = state_tracker->createRollbackPoint("before_error_parsing", "解析错误代码前的状态");
    
    std::cout << "1. 创建了回滚点 ID: " << rollback_id << "\n";
    
    // 尝试编译错误代码
    auto result = compiler->compileFromString(invalid_chtl_code);
    
    std::cout << "2. 编译结果: " << (result.success ? "成功" : "失败") << "\n";
    
    if (!result.success) {
        std::cout << "3. 编译错误:\n";
        for (const auto& error : result.errors) {
            std::cout << "   - " << error << "\n";
        }
        
        // 演示回滚操作
        std::cout << "4. 执行状态回滚...\n";
        if (state_tracker->rollbackToPoint(rollback_id)) {
            std::cout << "   回滚成功！\n";
        } else {
            std::cout << "   回滚失败！\n";
        }
        
        // 显示回滚后的状态
        std::cout << "5. 回滚后的状态:\n";
        std::cout << context_manager.getStatistics() << "\n";
    }
}

// 示例：演示上下文感知解析
void demonstrateContextAwareParsing() {
    std::cout << "\n=== 上下文感知解析示例 ===\n\n";
    
    // 创建复杂的CHTL代码，包含多种上下文
    std::string complex_chtl_code = R"(
        [Namespace] UI
        
        [Template] @Style BaseButton
        {
            padding: 8px 16px;
            border: none;
            cursor: pointer;
        }
        
        [Custom] @Element PrimaryButton
        {
            button
            {
                style
                {
                    @Style BaseButton;
                    background-color: #007bff;
                    color: white;
                    
                    &:hover
                    {
                        background-color: #0056b3;
                    }
                }
            }
        }
        
        html
        {
            body
            {
                div
                {
                    class: button-container;
                    
                    @Element PrimaryButton from UI;
                    
                    script
                    {
                        vir ButtonHandler = listen({
                            click: () => {
                                console.log("Button clicked!");
                            }
                        });
                        
                        {{.button-container}}->delegate({
                            target: {{button}},
                            click: ButtonHandler->click
                        });
                    }
                }
            }
        }
    )";
    
    auto compiler = CompilerFactory::createStandardCompiler();
    auto& context_manager = compiler->getContextManager();
    
    // 创建上下文感知解析器
    auto parser = ContextAwareParserFactory::createTemplateAwareParser(context_manager);
    parser->setParseStrategy(ParseStrategy::TEMPLATE_AWARE);
    
    std::cout << "1. 使用模板感知解析策略\n";
    
    // 编译复杂代码
    auto result = compiler->compileFromString(complex_chtl_code);
    
    std::cout << "2. 编译结果: " << (result.success ? "成功" : "失败") << "\n";
    std::cout << "3. 编译统计:\n";
    std::cout << compiler->getCompilationStatistics() << "\n";
    
    if (result.success) {
        std::cout << "4. 生成的代码:\n";
        std::cout << result.generated_code << "\n";
    }
}

int main() {
    try {
        // 运行所有示例
        demonstrateRAIIStateManagement();
        demonstrateErrorRecoveryAndRollback();
        demonstrateContextAwareParsing();
        
        std::cout << "\n=== 所有示例完成 ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "示例执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}