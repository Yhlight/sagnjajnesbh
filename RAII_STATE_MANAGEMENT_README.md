# 基于RAII自动化管理模式的状态机与上下文管理协助器

## 概述

本项目实现了一个基于RAII（Resource Acquisition Is Initialization）自动化管理模式的状态机与上下文管理协助器，专门用于CHTL编译器的AST节点状态标记和跟踪。该系统严格遵循《CHTL语法文档.md》中的语法规范，为解析器和生成器提供精确的状态支持，并与扫描器协同工作。

## 核心特性

### 1. RAII自动化管理
- **自动资源管理**：使用RAII模式确保状态和上下文的自动获取与释放
- **异常安全**：在异常情况下自动恢复状态，保证系统稳定性
- **零开销抽象**：编译时优化，运行时无额外开销

### 2. AST节点状态管理
- **状态跟踪**：完整跟踪AST节点的生命周期状态
- **状态转换验证**：确保状态转换的合法性
- **批量状态操作**：支持子树级别的状态批量设置
- **状态历史记录**：维护完整的状态变更历史

### 3. 上下文感知解析
- **多层次上下文**：支持全局、文件、命名空间、模板、自定义等多种上下文
- **上下文栈管理**：自动管理上下文的进入和退出
- **解析策略调整**：根据上下文动态调整解析策略
- **错误恢复机制**：智能错误恢复和状态回滚

### 4. 状态回滚和历史追踪
- **快照机制**：支持创建和恢复状态快照
- **回滚点管理**：命名回滚点，支持精确回滚
- **历史查询**：丰富的历史记录查询接口
- **性能优化**：自动清理过期数据，控制内存使用

## 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                    CompilerCore                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌──────────────┐ │
│  │   ContextManager │  │  ASTStateManager │  │ NodeStateTracker│ │
│  │                 │  │                 │  │              │ │
│  │ - PhaseGuard    │  │ - NodeStateGuard│  │ - Snapshots  │ │
│  │ - ScopeGuard    │  │ - FlagGuard     │  │ - Rollback   │ │
│  │ - CompoundGuard │  │ - StateHistory  │  │ - History    │ │
│  └─────────────────┘  └─────────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                 ContextAwareParser                          │
│  ┌─────────────────┐  ┌─────────────────┐                  │
│  │  ParseStrategy  │  │  ParseDecision  │                  │
│  │                 │  │                 │                  │
│  │ - STRICT        │  │ - shouldParse   │                  │
│  │ - TOLERANT      │  │ - strategy      │                  │
│  │ - TEMPLATE_AWARE│  │ - alternatives  │                  │
│  │ - CUSTOM_AWARE  │  │ - reason        │                  │
│  └─────────────────┘  └─────────────────┘                  │
└─────────────────────────────────────────────────────────────┘
```

## 核心组件

### 1. ASTStateManager
负责AST节点的状态管理，包括：
- 节点状态跟踪（UNINITIALIZED → CREATED → PARSING → PARSED → VALIDATED → GENERATED）
- 节点标记管理（TEMPLATE_NODE, CUSTOM_NODE, DYNAMIC_NODE等）
- 依赖关系管理和循环依赖检测
- 状态转换验证和历史记录

### 2. ContextManager
统一管理编译上下文，包括：
- 编译阶段管理（INITIALIZATION → LEXICAL_SCANNING → SYNTAX_PARSING → SEMANTIC_ANALYSIS → OPTIMIZATION → CODE_GENERATION → FINALIZATION）
- 上下文作用域管理（GLOBAL, FILE, NAMESPACE, TEMPLATE, CUSTOM等）
- 状态同步和一致性检查
- 错误处理和诊断信息

### 3. NodeStateTracker
提供状态历史追踪和回滚功能：
- 状态快照创建和恢复
- 命名回滚点管理
- 状态变更事件记录
- 统计分析和导出功能

### 4. ContextAwareParser
上下文感知的解析器：
- 根据上下文调整解析策略
- CHTL语法规范严格验证
- 智能错误恢复
- 模板、自定义、命名空间感知解析

## RAII守护器

### 1. NodeStateGuard
```cpp
{
    CHTL_NODE_STATE_GUARD(state_manager, node, NodeState::PARSING, ProcessingPhase::SYNTAX_ANALYSIS);
    // 节点状态自动设置为PARSING
    // ... 解析操作
} // 自动恢复到之前的状态
```

### 2. ContextScopeGuard
```cpp
{
    CHTL_SCOPE_GUARD(context_manager, ContextScope::TEMPLATE, "ButtonTemplate");
    // 自动进入模板上下文
    // ... 模板解析操作
} // 自动退出模板上下文
```

### 3. CompoundStateGuard
```cpp
{
    CHTL_COMPOUND_GUARD(context_manager, 
                       CompilationPhase::SYNTAX_PARSING,
                       CompilerState::PARSING_TEMPLATE,
                       SyntaxContext::TEMPLATE_DEFINITION,
                       ContextScope::TEMPLATE,
                       template_node,
                       "ButtonTemplate");
    // 同时管理多个状态
} // 自动恢复所有状态
```

## CHTL语法规范支持

### 1. 模板系统
```chtl
[Template] @Style DefaultButton
{
    background-color: blue;
    color: white;
    padding: 10px;
}
```

### 2. 自定义系统
```chtl
[Custom] @Element CustomButton
{
    button
    {
        style
        {
            @Style DefaultButton;
            border-radius: 5px;
        }
    }
}
```

### 3. 样式块增强
```chtl
div
{
    style
    {
        .container    // 自动添加类名
        {
            width: 100%;
        }
        
        &:hover      // 上下文推导
        {
            background-color: #f0f0f0;
        }
    }
}
```

### 4. CHTL JS支持
```chtl
script
{
    {{.container}}->listen({
        click: () => {
            console.log("Clicked");
        }
    });
    
    vir Handler = listen({
        click: () => { /* ... */ }
    });
}
```

## 使用示例

### 基础编译流程
```cpp
// 创建编译器核心
auto compiler = CompilerFactory::createStandardCompiler();
auto& context_manager = compiler->getContextManager();

// 创建状态跟踪器
auto state_tracker = std::make_shared<NodeStateTracker>(
    context_manager.getASTStateManager());

// 编译CHTL代码
std::string chtl_code = "...";
auto result = compiler->compileFromString(chtl_code);

// 检查结果
if (result.success) {
    std::cout << "Generated: " << result.generated_code << std::endl;
}
```

### 错误恢复和回滚
```cpp
// 创建回滚点
auto rollback_id = state_tracker->createRollbackPoint("before_parsing");

// 尝试解析
if (!parser->parse()) {
    // 解析失败，回滚状态
    state_tracker->rollbackToPoint(rollback_id);
}
```

### 上下文感知解析
```cpp
// 创建上下文感知解析器
auto parser = ContextAwareParserFactory::createTemplateAwareParser(context_manager);
parser->setParseStrategy(ParseStrategy::TEMPLATE_AWARE);

// 解析模板定义
auto template_node = parser->parseTemplateDefinition();
```

## 性能特性

- **零开销RAII**：编译时优化，运行时无额外开销
- **内存高效**：自动清理过期状态数据
- **线程安全**：支持多线程编译环境
- **可配置追踪**：可根据需求启用/禁用不同的追踪功能

## 调试和诊断

系统提供丰富的调试和诊断功能：
- 状态变更历史查看
- AST节点状态统计
- 编译过程跟踪
- 错误恢复路径分析
- 性能统计报告

## 扩展性

系统设计为高度可扩展：
- 可自定义状态转换规则
- 可添加新的节点标记类型
- 可扩展解析策略
- 可插入自定义监听器

## 总结

本基于RAII自动化管理模式的状态机与上下文管理协助器为CHTL编译器提供了：

1. **可靠的状态管理**：自动化的资源管理，确保状态一致性
2. **精确的AST标记**：完整的节点生命周期跟踪
3. **智能的错误恢复**：状态回滚和历史追踪
4. **上下文感知解析**：根据CHTL语法规范进行智能解析
5. **高性能实现**：零开销抽象和内存优化

该系统严格遵循CHTL语法文档规范，为编译器的各个阶段提供了强大而可靠的状态支持基础设施。