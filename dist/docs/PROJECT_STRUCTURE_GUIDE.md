# CHTL项目结构指南

## 概述

本指南详细介绍CHTL项目的目录结构、组件关系和架构设计，帮助开发者快速理解项目组织方式。

---

## 项目根目录结构

```
chtl/
├── src/                    # 源代码目录
├── vscode-chtl-extension/  # VSCode插件
├── scripts/                # 构建和工具脚本
├── external/               # 外部依赖
├── docs/                   # 开发文档
├── module/                 # 官方模块目录
├── CMakeLists.txt          # CMake构建配置
├── CHTL语法文档.md         # 用户语法文档
├── LICENSE                 # MIT开源协议
└── README.md               # 项目说明
```

---

## 源代码架构 (`src/`)

### 核心编译器系统

```
src/
├── CHTLCompiler/           # CHTL核心编译器
│   ├── CHTLCompilerCore.h/cpp          # 编译器核心
│   ├── Import/                         # 导入系统
│   │   └── ImportManager.h/cpp         # 导入管理器 (871行)
│   ├── Namespace/                      # 命名空间系统
│   │   └── NamespaceManager.h/cpp      # 命名空间管理器 (798行)
│   ├── Module/                         # 模块系统
│   │   ├── CmodManager.h/cpp           # CMOD管理器 (827行)
│   │   └── ModulePackager.h/cpp        # 模块打包器
│   ├── Template/                       # 模板系统
│   │   └── TemplateManager.h/cpp       # 模板管理器 (307行)
│   ├── Custom/                         # 自定义系统
│   │   └── CustomManager.h/cpp         # 自定义管理器 (497行)
│   ├── Origin/                         # 原始嵌入系统
│   │   └── OriginManager.h/cpp         # 原始嵌入管理器 (423行)
│   ├── Constraint/                     # 约束系统
│   │   └── ConstraintManager.h/cpp     # 约束管理器
│   ├── Configuration/                  # 配置系统
│   │   └── ConfigurationManager.h/cpp  # 配置管理器
│   ├── AST/                            # 抽象语法树
│   │   └── CHTLNodes.h                 # AST节点定义
│   ├── Lexer/                          # 词法分析器
│   │   └── CHTLLexer.h/cpp             # CHTL词法分析器
│   ├── Parser/                         # 语法分析器
│   │   └── CHTLParser.h/cpp            # CHTL语法分析器
│   ├── Generator/                      # 代码生成器
│   │   └── CHTLGenerator.h/cpp         # CHTL代码生成器
│   └── State/                          # 状态管理
│       ├── CHTLState.h/cpp             # 编译器状态
│       └── CHTLGlobalMap.h/cpp         # 全局符号表
```

### CHTL JS编译器系统

```
├── CHTLJSCompiler/         # CHTL JS编译器
│   ├── CHTLJSCompiler.h/cpp            # CHTL JS编译器核心
│   ├── Event/                          # 事件系统
│   │   ├── ListenSystemProcessor.h/cpp     # listen函数处理器
│   │   └── DelegateSystemProcessor.h/cpp   # delegate函数处理器
│   ├── Selector/                       # 选择器系统
│   │   └── EnhancedSelectorProcessor.h/cpp # 增强选择器处理器
│   ├── Virtual/                        # 虚对象系统
│   │   ├── VirtualObjectProcessor.h/cpp    # 虚对象处理器
│   │   └── VirtualObjectManager.h/cpp      # 虚对象管理器
│   ├── Animation/                      # 动画系统
│   │   └── AnimateSystemProcessor.h/cpp    # 动画处理器
│   ├── Operator/                       # 操作符系统
│   │   └── ArrowOperatorProcessor.h/cpp    # 箭头操作符处理器
│   ├── Script/                         # 脚本系统
│   │   └── LocalScriptProcessor.h/cpp      # 局部脚本处理器
│   ├── Enhanced/                       # 增强功能
│   │   └── UnorderedOptionalProcessor.h/cpp # 无序可选处理器
│   ├── Core/                           # 核心组件
│   │   ├── AutoAdditionManager.h/cpp       # 自动添加管理器
│   │   └── JSEventRegistry.h/cpp           # JS事件注册表
│   ├── Parser/                         # 解析器
│   │   └── SelectorDetector.h/cpp          # 选择器检测器
│   └── Module/                         # 模块和扩展
│       ├── YourCJMODDesign.h/cpp           # 您的原创CJMOD设计
│       ├── ProfessionalCJMODInterface.h/cpp # 专业级CJMOD接口
│       └── CJmodManager.h/cpp              # CJMOD管理器
```

### CSS编译器系统

```
├── CSSCompiler/            # CSS编译器（ANTLR集成）
│   ├── Core/
│   │   └── CSSCompilerCore.h/cpp       # CSS编译器核心
│   └── Parser/                         # ANTLR生成的解析器
│       ├── css3Lexer.h/cpp             # CSS词法分析器
│       ├── css3Parser.h/cpp            # CSS语法分析器
│       ├── css3Lexer.g4                # CSS语法定义
│       └── css3Parser.g4               # CSS解析规则
```

### JavaScript编译器系统

```
├── JavaScriptCompiler/     # JavaScript编译器（ANTLR集成）
│   ├── Core/
│   │   └── JavaScriptCompilerCore.h/cpp # JS编译器核心
│   └── Parser/                         # ANTLR生成的解析器
│       ├── JavaScriptLexer_cpp.h/cpp   # JS词法分析器
│       ├── JavaScriptParser_cpp.h/cpp  # JS语法分析器
│       ├── JavaScriptLexer.g4          # JS语法定义
│       └── JavaScriptParser.g4         # JS解析规则
```

### 统一扫描器系统

```
├── Scanner/                # 统一扫描器（CHTL底层系统）
│   └── CHTLUnifiedScanner.h/cpp        # 统一扫描器
```

### 编译器调度器

```
├── Dispatcher/             # 编译器调度器
│   └── CompilerDispatcher.h/cpp        # 调度器核心
```

### 共享组件

```
└── Common/                 # 共享组件
    ├── Core/                           # 核心工具
    │   ├── ErrorHandler.h/cpp          # 错误处理器
    │   ├── UnifiedErrorHandler.h       # 统一错误处理接口
    │   └── NamespaceAlgorithms.h/cpp   # 命名空间算法
    ├── SimpleZip/                      # 自定义ZIP库
    │   └── SimpleZip.h/cpp             # ZIP文件处理
    ├── AST/                            # AST相关
    │   ├── CommentGenerator.h/cpp      # 注释生成器
    │   ├── CommentNode.h/cpp           # 注释节点
    │   └── OriginEmbedDetector.h/cpp   # 原始嵌入检测器
    ├── Utils/                          # 工具函数
    │   └── FileUtils.h/cpp             # 文件工具
    └── ModulePathManager.h/cpp         # 模块路径管理器
```

---

## 模块源码组织 (`src/Module/`)

### 模块分类结构

```
src/Module/                 # 模块源码（仅开发分类，不参与运行时搜索）
├── CMOD/                   # 大写格式CMOD模块
│   ├── Chtholly/          # 珂朵莉模块
│   │   ├── src/
│   │   │   ├── Chtholly.chtl           # 主模块文件
│   │   │   └── ChthollyEffects.chtl    # 效果组件
│   │   └── info/
│   │       └── Chtholly.chtl           # 模块信息
│   └── Yuigahama/         # 由比滨结衣模块
│       ├── src/
│       │   └── Yuigahama.chtl          # 主模块文件
│       └── info/
│           └── Yuigahama.chtl          # 模块信息
├── Cmod/                   # 首字母大写格式
│   └── TestModule/        # 测试模块
├── cmod/                   # 小写格式
│   └── AnotherTest/       # 另一个测试模块
├── CJMOD/                  # 大写格式CJMOD扩展
│   └── Chtholly/          # 珂朵莉CJMOD扩展
│       ├── src/
│       │   ├── chtholly_extensions.cpp     # 扩展实现
│       │   └── chtholly_extensions_updated.cpp # 新接口版本
│       └── info/
│           └── Chtholly.chtl               # 扩展信息
├── CJmod/                  # 驼峰格式
│   └── ExtensionTest/     # 扩展测试
└── cjmod/                  # 小写格式
    └── SimpleExt/         # 简单扩展
```

**重要说明**: `src/Module/` 目录仅用于开发时的模块分类和组织，不参与运行时的模块搜索。

---

## VSCode插件结构 (`vscode-chtl-extension/`)

```
vscode-chtl-extension/
├── package.json            # 插件配置文件
├── src/                    # 插件源码
│   ├── extension.ts        # 主扩展文件
│   ├── extension_complete.ts # 完整功能版本
│   ├── moduleProvider.ts   # 模块提供器
│   └── modules/            # 模块相关功能
├── syntaxes/               # 语法定义
│   └── chtl.tmLanguage.json # CHTL语法高亮配置
├── snippets/               # 代码片段
│   └── chtl.json           # CHTL代码片段
├── scripts/                # 插件脚本
└── compiler/               # 编译器集成
    └── module/             # 模块索引
```

---

## 脚本和工具 (`scripts/`)

### 构建脚本

```
scripts/
├── build/                  # 构建脚本
│   ├── build_complete_with_antlr.sh    # 完整构建（含ANTLR）
│   └── build_with_antlr.sh             # ANTLR构建脚本
├── packaging/              # 打包脚本
│   └── pack_modules_enhanced.sh        # 增强模块打包脚本
├── integration/            # 集成脚本
│   └── setup_antlr_complete.sh         # ANTLR完整设置
├── utils/                  # 工具脚本
└── README.md               # 脚本说明
```

### 工具脚本功能

1. **构建脚本**: 自动化编译和构建
2. **打包脚本**: 模块打包和分发
3. **集成脚本**: 外部依赖集成
4. **工具脚本**: 开发辅助工具

---

## 外部依赖 (`external/`)

### ANTLR 4集成

```
external/
├── antlr4/                 # ANTLR 4源码
├── antlr4-cross-platform/  # 跨平台ANTLR集成
│   ├── docs/               # ANTLR集成文档
│   ├── precompiled/        # 预编译库
│   └── scripts/            # ANTLR构建脚本
├── antlr4-install/         # ANTLR安装目录
├── grammars/               # 语法定义
├── grammars-v4/            # ANTLR官方语法库
└── antlr-4.13.2-complete.jar # ANTLR工具JAR
```

### 依赖管理

1. **ANTLR 4**: 用于CSS和JavaScript的原生解析
2. **语法库**: 官方维护的语法定义
3. **预编译库**: 跨平台的预编译ANTLR库

---

## 组件间关系

### 编译器核心关系

```
CHTLCompilerCore
├── 依赖 ImportManager        # 处理模块导入
├── 依赖 NamespaceManager     # 管理命名空间
├── 依赖 CmodManager          # 管理CMOD模块
├── 依赖 TemplateManager      # 处理模板
├── 依赖 CustomManager        # 处理自定义
├── 依赖 OriginManager        # 处理原始嵌入
├── 依赖 ConstraintManager    # 处理约束
├── 依赖 ConfigurationManager # 处理配置
└── 使用 CHTLUnifiedScanner   # 统一扫描器
```

### 编译器调度关系

```
CompilerDispatcher
├── 调度 CHTLCompiler         # CHTL语法编译
├── 调度 CHTLJSCompiler       # CHTL JS语法编译
├── 调度 CSSCompiler          # CSS编译（ANTLR）
└── 调度 JavaScriptCompiler   # JavaScript编译（ANTLR）
```

### CHTL JS组件关系

```
CHTLJSCompiler
├── 使用 ListenSystemProcessor      # 处理listen语法
├── 使用 DelegateSystemProcessor    # 处理delegate语法
├── 使用 EnhancedSelectorProcessor  # 处理增强选择器
├── 使用 VirtualObjectProcessor     # 处理虚对象
├── 使用 AnimateSystemProcessor     # 处理动画语法
├── 使用 AutoAdditionManager        # 自动添加管理
├── 使用 JSEventRegistry            # JS事件注册表
└── 集成 ProfessionalCJMODInterface # CJMOD扩展接口
```

---

## 数据流架构

### 编译数据流

```
源文件 (.chtl)
    ↓
CHTLUnifiedScanner (统一扫描)
    ↓
CompilerDispatcher (智能调度)
    ↓
┌─────────────────┬─────────────────┬─────────────────┐
│   CHTLCompiler  │ CHTLJSCompiler  │   CSSCompiler   │
│                 │                 │ JavaScriptCompiler │
└─────────────────┴─────────────────┴─────────────────┘
    ↓
代码生成器 (HTML/CSS/JS输出)
    ↓
输出文件 (.html, .css, .js)
```

### 模块加载流程

```
模块导入语句
    ↓
ImportManager (解析导入)
    ↓
ModulePathManager (路径解析)
    ↓
┌─────────────────┬─────────────────┐
│   CmodManager   │  CJmodManager   │
│  (CMOD模块)    │  (CJMOD扩展)   │
└─────────────────┴─────────────────┘
    ↓
模块内容集成到编译器
    ↓
正常编译流程
```

---

## 配置系统架构

### CMake构建配置

```cmake
# 核心库
add_library(chtl_common STATIC ...)      # 共享组件库
add_library(chtl_scanner STATIC ...)     # 扫描器库
add_library(chtl_compiler STATIC ...)    # CHTL编译器库
add_library(chtljs_compiler STATIC ...)  # CHTL JS编译器库

# 依赖关系
target_link_libraries(chtl_compiler chtl_common)
target_link_libraries(chtljs_compiler chtl_common)

# 测试目标
add_executable(test_compliance ...)      # 合规性测试
add_executable(test_features ...)        # 功能测试
```

### 模块路径配置

```
运行时模块搜索路径:
1. <编译器目录>/module/     # 官方模块（最高优先级）
2. <项目目录>/module/       # 用户模块（中等优先级）

开发时模块分类:
3. <项目根>/src/Module/     # 源码分类（不参与搜索）
```

---

## 错误处理架构

### 统一错误处理

```
所有管理器
    ↓
UnifiedErrorHandler (统一错误接口)
    ↓
ErrorHandler (核心错误处理器)
    ↓
错误报告和日志
```

### 错误类型分类

```cpp
enum class ErrorType {
    SYNTAX_ERROR,      // 语法错误
    SEMANTIC_ERROR,    // 语义错误
    IMPORT_ERROR,      // 导入错误
    NAMESPACE_ERROR,   // 命名空间错误
    MODULE_ERROR,      // 模块错误
    RUNTIME_ERROR,     // 运行时错误
    VALIDATION_ERROR   // 验证错误
};
```

---

## 性能架构

### 编译器性能优化

1. **缓存系统**: 
   - AST缓存
   - 解析结果缓存
   - 模块信息缓存

2. **并行处理**:
   - 多线程编译
   - 并行模块加载
   - 并行代码生成

3. **增量编译**:
   - 变更检测
   - 部分重编译
   - 依赖跟踪

### 内存管理

1. **智能指针**: 使用std::shared_ptr和std::unique_ptr
2. **RAII模式**: 自动资源管理
3. **对象池**: 复用常用对象
4. **垃圾回收**: 及时清理不需要的资源

---

## 扩展点架构

### 编译器扩展点

1. **词法分析器扩展**: 添加新的token类型
2. **语法分析器扩展**: 添加新的语法规则
3. **代码生成器扩展**: 自定义代码生成逻辑
4. **管理器扩展**: 添加新的功能管理器

### CJMOD扩展架构

```
CJMOD扩展
    ↓
ProfessionalCJMODInterface (专业级接口)
    ↓
CHTLJSCompiler (集成到编译器)
    ↓
JavaScript代码生成
```

---

## 开发工作流

### 添加新功能的标准流程

1. **需求分析**: 确定功能需求和设计
2. **架构设计**: 设计组件架构和接口
3. **实现开发**: 编写核心实现代码
4. **集成测试**: 集成到编译器系统
5. **单元测试**: 编写单元测试
6. **文档更新**: 更新相关文档
7. **性能测试**: 验证性能影响
8. **代码审查**: 进行代码审查
9. **集成验证**: 完整系统验证
10. **发布准备**: 准备发布版本

### 代码组织原则

1. **单一职责**: 每个类专注一个功能
2. **接口分离**: 定义清晰的接口
3. **依赖注入**: 使用依赖注入降低耦合
4. **模块化**: 功能模块化组织
5. **可测试性**: 代码易于测试

---

## 构建系统架构

### CMake组织

```
CMakeLists.txt (根)
├── 设置项目基本信息
├── 查找依赖 (ANTLR等)
├── 定义核心库目标
├── 定义可执行文件目标
├── 设置链接关系
└── 配置安装规则
```

### 构建目标

1. **库目标**:
   - `chtl_common`: 共享组件库
   - `chtl_scanner`: 扫描器库
   - `chtl_compiler`: CHTL编译器库
   - `chtljs_compiler`: CHTL JS编译器库

2. **可执行文件目标**:
   - `chtl`: 主编译器可执行文件
   - 各种测试可执行文件

3. **安装目标**:
   - 头文件安装
   - 库文件安装
   - 可执行文件安装

---

## 项目配置

### 编译器配置

```cpp
// 编译器选项
#define CHTL_VERSION_MAJOR 1
#define CHTL_VERSION_MINOR 0
#define CHTL_VERSION_PATCH 0

// 功能开关
#define CHTL_ENABLE_DEBUG 1
#define CHTL_ENABLE_ANTLR 1
#define CHTL_ENABLE_CJMOD 1
#define CHTL_ENABLE_MODULES 1
```

### 路径配置

```cpp
// 默认路径配置
const std::string DEFAULT_MODULE_PATH = "./module";
const std::string DEFAULT_OUTPUT_PATH = "./output";
const std::string DEFAULT_TEMP_PATH = "./temp";
```

---

## 维护和演进

### 版本管理

1. **语义化版本**: major.minor.patch
2. **API兼容性**: 保持向后兼容
3. **废弃策略**: 渐进式废弃旧API
4. **迁移指南**: 提供升级指南

### 代码质量

1. **代码规范**: 统一的编码风格
2. **静态分析**: 使用静态分析工具
3. **代码审查**: 所有代码都经过审查
4. **测试覆盖**: 保持高测试覆盖率

### 性能监控

1. **基准测试**: 定期性能基准测试
2. **性能回归**: 检测性能回归
3. **内存监控**: 监控内存使用
4. **性能优化**: 持续性能优化

---

CHTL项目采用现代C++架构设计，具有清晰的模块划分、良好的扩展性和高度的可维护性。这种架构设计确保了项目的长期发展和社区贡献的便利性。