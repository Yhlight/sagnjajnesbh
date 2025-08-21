# Pull Request: CHTL JS 编译器基础架构实现

## 概述

本PR实现了CHTL JS编译器的基础架构，严格按照CHTL语法文档的要求，创建了完全独立于CHTL编译器的系统。

## 主要变更

### 1. CHTL JS Token系统 (`chtl_js/token/`)
- **CHTLJSToken.h/cpp**: 定义了CHTL JS专用的词法单元
  - 增强选择器相关：`ENHANCED_SELECTOR_START/END`, `SELECTOR_CLASS/ID/INDEX/AMPERSAND`
  - 虚对象关键字：`KEYWORD_VIR`
  - CHTL JS函数：`FUNCTION_LISTEN/DELEGATE/ANIMATE/INEVERAWAY/PRINTMYLOVE`
  - 特殊操作符：`ARROW` (->)
  - 状态标记：`STATE_MARKER` (<A>, <B>等)

### 2. CHTL JS GlobalMap (`chtl_js/core/`)
- **CHTLJSGlobalMap.h/cpp**: 独立的全局映射表
  - 虚对象管理：注册和查询虚对象
  - 增强选择器管理：存储和检索选择器信息
  - 事件委托管理：管理父选择器的事件委托
  - 动画管理：存储动画配置
  - CHTL JS函数管理：内置函数注册
  - 函数调用链管理：追踪->调用链
  - 状态函数映射：支持iNeverAway的状态函数

### 3. CHTL JS State系统 (`chtl_js/state/`)
- **CHTLJSState.h/cpp**: 独立的状态机系统
  - 定义了CHTL JS特有的编译器状态
  - 增强选择器状态：`IN_ENHANCED_SELECTOR`, `IN_SELECTOR_*`
  - 虚对象状态：`IN_VIRTUAL_OBJECT_DECLARATION/INIT`
  - 函数调用状态：`IN_*_CALL` (各种CHTL JS函数)
  - RAII状态管理：`CHTLJSStateGuard`

### 4. CHTL JS Context系统 (`chtl_js/context/`)
- **CHTLJSContext.h/cpp**: 独立的上下文管理
  - 作用域管理：script块、虚对象、函数调用等作用域
  - 虚对象定义和查询
  - 选择器上下文管理
  - 函数调用链追踪
  - 对象字面量上下文
  - 事件处理器注册
  - 动画上下文管理

### 5. CHTL JS Lexer (`chtl_js/lexer/`)
- **CHTLJSLexer.h/cpp**: 专门的词法分析器
  - 增强选择器扫描：特殊处理{{}}内的内容
  - 虚对象识别：vir关键字
  - CHTL JS函数识别
  - 箭头操作符处理
  - 状态标记扫描：<State>语法

## 关键设计决策

### 1. 完全独立的架构
- CHTL JS的所有组件都在`chtl_js/`目录下
- 使用独立的命名空间`chtl::chtljs`
- 不与CHTL编译器共享任何状态机、上下文或全局映射表

### 2. 专注于CHTL JS语法
- CHTL JS不是JavaScript，只处理语法文档中定义的CHTL JS特性
- 不包含JavaScript的语法解析
- 严格实现文档中的功能：增强选择器、虚对象、特殊函数等

### 3. 模块化设计
- Token、State、Context、GlobalMap、Lexer各司其职
- 清晰的接口定义
- RAII模式的资源管理

## 测试覆盖

创建了全面的测试文件`TestCHTLJSCompilerBase.cpp`，包括：
- Token类型测试
- 词法分析器测试（增强选择器、虚对象、CHTL JS函数）
- 状态机测试
- 上下文管理测试
- 全局映射表测试
- 集成测试

## 符合语法文档的实现

严格按照CHTL语法文档实现了以下CHTL JS特性的基础支持：
1. **增强选择器**：`{{selector}}`语法
2. **虚对象**：`vir`关键字
3. **箭头操作符**：`->`推荐用法
4. **CHTL JS函数**：
   - `listen` - 事件监听
   - `delegate` - 事件委托
   - `animate` - 动画
   - `iNeverAway` - 状态函数
   - `printMylove` - 特殊输出函数

## 后续工作

- TODO 7: 实现CHTL JS AST节点
- TODO 9: 创建CHTL JS解析器和生成器
- 与统一扫描器集成，处理局部script块中的CHTL JS代码

## 编译和测试

```bash
cd /workspace/chtl-compiler/build
cmake ..
make -j4
./tests/TestCHTLJSCompilerBase
```

所有测试均已通过，证明基础架构的正确性。