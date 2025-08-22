# CHTL编译器项目

CHTL编译器是一个基于C++17实现的超文本语言编译器，严格按照CHTL语法文档规范开发，实现了CHTL和CHTL JS的完全分离架构。

## 项目特点

- **完全分离架构**: CHTL和CHTL JS拥有独立的文件体系，各自管理Token、GlobalMap、State、Context、Lexer等核心组件
- **统一扫描器**: 实现了CHTLUnifiedScanner，能够精准切割不同类型的代码片段
- **模块化设计**: 采用模块化开发方式，每个组件职责明确
- **高质量代码**: 使用大驼峰命名法，遵循C++17标准，支持UTF-8编码
- **完整测试**: 包含单元测试和集成测试，确保代码质量

## 架构设计

```
CHTL源代码
     ↓
CHTLUnifiedScanner (精准代码切割器)
     ↓
┌─────────┬─────────────┬─────────┬─────────────┐
│ CHTL片段 │ CHTL JS片段 │ CSS片段 │ JavaScript片段 │
└────┬────┴─────┬───────┴────┬────┴─────┬───────┘
     ▼          ▼            ▼          ▼
CompilerDispatcher (编译器调度器)
     ↓
┌─────────┬─────────────┬─────────┬─────────────┐
│  CHTL   │   CHTL JS   │   CSS   │ JavaScript  │
│Compiler │  Compiler   │Compiler │  Compiler   │
│ (手写)   │   (手写)     │(ANTLR4) │  (ANTLR4)   │
└─────────┴─────────────┴─────────┴─────────────┘
     ↓
编译结果合并 (HTML输出)
```

## 目录结构

```
CHTL_Compiler/
├── src/                    # 源代码目录
│   ├── CHTL/              # CHTL编译器核心
│   │   ├── Core/          # 核心组件
│   │   ├── Lexer/         # 词法分析器
│   │   ├── Parser/        # 语法分析器
│   │   ├── AST/           # 抽象语法树
│   │   └── Generator/     # 代码生成器
│   ├── CHTLJS/            # CHTL JS编译器核心
│   │   ├── Core/          # 核心组件（独立）
│   │   ├── Lexer/         # 词法分析器（独立）
│   │   ├── Parser/        # 语法分析器（独立）
│   │   ├── AST/           # 抽象语法树（独立）
│   │   └── Generator/     # 代码生成器（独立）
│   ├── Scanner/           # 统一扫描器
│   ├── Dispatcher/        # 编译器调度器
│   ├── CSS/               # CSS编译器
│   ├── JavaScript/        # JavaScript编译器
│   └── Utils/             # 工具类
├── include/               # 头文件目录
├── tests/                 # 测试目录
├── examples/              # 示例文件
├── ANTLR4/               # ANTLR4库
├── css/                  # CSS语法文件
├── js/                   # JavaScript语法文件
└── build/                # 构建目录
```

## 构建说明

### 系统要求

- C++17兼容编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- ANTLR4运行时库

### 构建步骤

```bash
# 克隆项目
git clone <repository-url>
cd CHTL_Compiler

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译项目
make

# 运行测试
make test
```

## 使用说明

### 基本用法

```bash
# 编译CHTL文件
./build/bin/chtl_compiler input.chtl

# 指定输出文件
./build/bin/chtl_compiler -o output.html input.chtl

# 启用详细输出
./build/bin/chtl_compiler -v input.chtl

# 仅进行扫描测试
./build/bin/chtl_compiler --scan-only --verbose input.chtl
```

### 命令行选项

- `-h, --help`: 显示帮助信息
- `-v, --verbose`: 启用详细输出模式
- `-o, --output <文件>`: 指定输出文件
- `--scan-only`: 仅进行扫描测试，不执行编译
- `--no-context-check`: 禁用上下文检查
- `--no-minimal-unit`: 禁用最小单元切割

## 核心功能

### 统一扫描器 (CHTLUnifiedScanner)

统一扫描器是CHTL编译器的核心组件，负责：

1. **精准代码切割**: 将CHTL源代码切割成不同类型的片段
2. **类型识别**: 自动识别CHTL、CHTL JS、CSS、JavaScript四种类型
3. **上下文检查**: 确保代码片段在合理的上下文中
4. **最小单元切割**: 对CHTL和CHTL JS进行最小单元级别的切割
5. **可变长度切片**: 支持基于可变长度切片的工作模式

### 支持的语法特征

#### CHTL语法
- 模板系统：`[Template] @Style`, `[Template] @Element`, `[Template] @Var`
- 自定义系统：`[Custom] @Style`, `[Custom] @Element`, `[Custom] @Var`
- 原始嵌入：`[Origin] @Html`, `[Origin] @Style`, `[Origin] @JavaScript`
- 导入系统：`[Import]`
- 命名空间：`[Namespace]`
- 配置系统：`[Configuration]`

#### CHTL JS语法
- 增强选择器：`{{box}}`, `{{.class}}`, `{{#id}}`
- 箭头操作符：`->`
- 监听器：`listen({})`
- 事件委托：`delegate({})`
- 动画系统：`animate({})`
- 虚对象：`vir objectName = ...`

## 测试

项目包含完整的测试套件：

```bash
# 运行所有测试
make test

# 运行特定测试
./build/bin/test_scanner      # 扫描器测试
./build/bin/test_chtl_core    # CHTL核心测试
./build/bin/test_chtljs_core  # CHTL JS核心测试
./build/bin/test_integration  # 集成测试
```

## 开发状态

### 已完成功能
- ✅ 项目架构设计和目录结构
- ✅ 统一扫描器实现
- ✅ CMake构建系统
- ✅ UTF-8编码支持
- ✅ ANTLR4集成
- ✅ 基础测试框架
- ✅ 工具类库（文件操作、字符串处理、错误处理）

### 待实现功能
- ⏳ CHTL编译器核心模块
- ⏳ CHTL JS编译器核心模块
- ⏳ AST节点体系
- ⏳ 编译器调度器
- ⏳ 完整的编译流程

## 示例

项目包含一个完整的CHTL示例文件 `examples/test.chtl`，展示了：

- 模板定义和使用
- 自定义元素
- 局部样式块
- CHTL JS增强功能
- 原始嵌入

可以使用以下命令测试扫描功能：

```bash
./build/bin/chtl_compiler --scan-only --verbose examples/test.chtl
```

## 许可证

本项目使用MIT开源协议。

## 贡献

欢迎贡献代码！请确保：

1. 遵循C++17标准
2. 使用大驼峰命名法
3. 添加相应的测试
4. 保持代码注释完整
5. 严格按照CHTL语法文档规范

## 联系方式

如有问题或建议，请通过GitHub Issues联系。