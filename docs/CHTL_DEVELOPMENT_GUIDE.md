# CHTL开发指南

## 概述

本指南面向希望参与CHTL语言开发的开发者，介绍如何设置开发环境、理解项目架构、贡献代码等。

---

## 开发环境设置

### 系统要求

- **操作系统**: Linux (推荐)、macOS、Windows
- **编译器**: GCC 9+ 或 Clang 10+，支持C++17
- **构建工具**: CMake 3.16+
- **版本控制**: Git 2.20+
- **IDE**: VSCode (推荐) 或 CLion

### 依赖安装

#### Linux (Ubuntu/Debian)
```bash
# 安装基础开发工具
sudo apt update
sudo apt install build-essential cmake git

# 安装C++开发环境
sudo apt install g++ clang libc++-dev

# 安装Java（ANTLR需要）
sudo apt install openjdk-11-jdk

# 安装Node.js（VSCode插件开发）
sudo apt install nodejs npm
```

#### macOS
```bash
# 安装Xcode命令行工具
xcode-select --install

# 安装Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install cmake git java nodejs npm
```

#### Windows
```powershell
# 安装Visual Studio 2019+（包含MSVC编译器）
# 安装CMake
# 安装Git
# 安装Java JDK 11+
# 安装Node.js
```

### 克隆项目

```bash
git clone https://github.com/chtl-lang/chtl.git
cd chtl
```

---

## 项目架构

### 目录结构

```
chtl/
├── src/                    # 源代码
│   ├── CHTLCompiler/      # CHTL核心编译器
│   ├── CHTLJSCompiler/    # CHTL JS编译器
│   ├── CSSCompiler/       # CSS编译器
│   ├── JavaScriptCompiler/ # JavaScript编译器
│   ├── Scanner/           # 统一扫描器
│   ├── Dispatcher/        # 编译器调度器
│   ├── Common/            # 共享组件
│   └── Module/            # 模块源码（开发分类）
├── vscode-chtl-extension/ # VSCode插件
├── scripts/               # 构建和工具脚本
├── external/              # 外部依赖
└── docs/                  # 文档
```

### 核心组件

#### CHTL编译器 (`src/CHTLCompiler/`)
- **CHTLCompilerCore**: 核心编译器
- **ImportManager**: 导入系统管理
- **NamespaceManager**: 命名空间管理
- **CmodManager**: CMOD模块管理
- **TemplateManager**: 模板系统
- **CustomManager**: 自定义系统
- **OriginManager**: 原始嵌入系统
- **ConstraintManager**: 约束系统
- **ConfigurationManager**: 配置系统

#### CHTL JS编译器 (`src/CHTLJSCompiler/`)
- **ListenSystemProcessor**: listen函数处理
- **DelegateSystemProcessor**: delegate函数处理
- **EnhancedSelectorProcessor**: 增强选择器处理
- **VirtualObjectProcessor**: 虚对象处理
- **AnimateSystemProcessor**: 动画系统处理
- **ProfessionalCJMODInterface**: CJMOD扩展接口

#### 统一扫描器 (`src/Scanner/`)
- **CHTLUnifiedScanner**: 变量长度切片扫描器
- 基于上下文的智能扫描
- 不将无修饰字面量作为CHTL JS判断条件

---

## 构建系统

### 基础构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# 编译所有组件
make -j4

# 运行测试
ctest
```

### 完整构建（包含ANTLR）

```bash
# 使用完整构建脚本
./scripts/build/build_complete_with_antlr.sh
```

### 调试构建

```bash
# 调试模式构建
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4

# 使用调试器
gdb ./build/chtl_compiler
```

---

## 开发工作流

### 1. 功能开发

#### 添加新的语法特性

1. **更新词法分析器** (`src/CHTLCompiler/Lexer/CHTLLexer.cpp`)
2. **更新语法分析器** (`src/CHTLCompiler/Parser/CHTLParser.cpp`)
3. **更新AST节点** (`src/CHTLCompiler/AST/CHTLNodes.h`)
4. **更新代码生成器** (`src/CHTLCompiler/Generator/CHTLGenerator.cpp`)
5. **添加测试用例**
6. **更新文档**

#### 添加新的管理器

1. **创建管理器类** (`src/CHTLCompiler/NewManager/`)
2. **实现核心功能**
3. **集成到编译器核心** (`CHTLCompilerCore.cpp`)
4. **更新CMakeLists.txt**
5. **添加测试**

### 2. CHTL JS功能开发

#### 添加新的CHTL JS语法

1. **创建处理器** (`src/CHTLJSCompiler/NewFeature/`)
2. **实现语法解析**
3. **实现代码生成**
4. **集成到CHTL JS编译器**
5. **测试验证**

#### 开发CJMOD扩展

1. **使用专业级CJMOD接口**
2. **实现语法分析**: `syntaxAnalys`
3. **设置参数绑定**: `bind`
4. **配置转换规则**: `transform`
5. **注册扫描器**: `scanKeyword`
6. **参数匹配**: `match`
7. **代码生成**: `generateCode`

### 3. 代码提交规范

#### 提交消息格式

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**类型**:
- `feat`: 新功能
- `fix`: 错误修复
- `docs`: 文档更新
- `style`: 代码格式调整
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建和工具相关

**示例**:
```
feat(compiler): 添加新的模板继承功能

实现了样式组模板的多重继承，支持链式继承语法。

Closes #123
```

---

## 测试开发

### 测试框架

CHTL使用基于进程的单元测试框架：

```cpp
#include "src/Test/TestFramework.h"

class MyFeatureTest : public TestBase {
public:
    void setUp() override {
        // 测试初始化
    }
    
    void tearDown() override {
        // 测试清理
    }
    
    void testBasicFunctionality() {
        // 测试基础功能
        ASSERT_TRUE(condition);
        ASSERT_EQUALS(expected, actual);
    }
};

REGISTER_TEST(MyFeatureTest);
```

### 测试类型

1. **单元测试**: 测试单个组件功能
2. **集成测试**: 测试组件间协作
3. **语法合规测试**: 验证语法文档合规性
4. **性能测试**: 测试编译器性能
5. **模块测试**: 测试模块系统功能

### 运行测试

```bash
# 编译测试
make test_target

# 运行特定测试
./build/test_specific_feature

# 运行所有测试
./build/ComprehensiveTestRunner
```

---

## 调试技巧

### 编译器调试

1. **启用调试模式**:
```cpp
#define CHTL_DEBUG_MODE 1
```

2. **使用调试输出**:
```cpp
CHTL_DEBUG_LOG("Processing element: " + elementName);
```

3. **AST可视化**:
```cpp
astNode->printDebugInfo();
```

### 扫描器调试

```cpp
// 启用扫描器调试
scanner.enableDebugMode(true);

// 查看扫描器状态
std::cout << scanner.getDebugInfo() << std::endl;
```

### CJMOD调试

```cpp
// 启用CJMOD调试
enableCJMODDebug(true);

// 查看诊断信息
std::cout << getCJMODDiagnostics() << std::endl;
```

---

## 性能优化

### 编译器性能

1. **缓存机制**: 缓存解析结果和生成代码
2. **并行处理**: 多线程编译大型项目
3. **增量编译**: 只重新编译变更的文件
4. **内存管理**: 使用智能指针和RAII

### 扫描器性能

1. **变量长度切片**: 避免固定长度扫描的性能损失
2. **上下文缓存**: 缓存上下文信息
3. **智能预测**: 预测下一个可能的标记

---

## 贡献指南

### 贡献类型

1. **功能开发**: 实现新的语言特性
2. **错误修复**: 修复已知问题
3. **性能优化**: 提升编译器性能
4. **文档改进**: 完善文档和示例
5. **测试增强**: 增加测试覆盖

### 代码审查

1. **功能正确性**: 确保功能按预期工作
2. **语法合规性**: 严格按照CHTL语法文档
3. **代码质量**: 遵循项目编码规范
4. **测试覆盖**: 包含适当的测试
5. **文档更新**: 更新相关文档

### 发布流程

1. **版本规划**: 确定版本号和功能范围
2. **功能开发**: 实现计划功能
3. **测试验证**: 全面测试验证
4. **文档更新**: 更新用户和开发文档
5. **发布打包**: 创建发布包
6. **社区通知**: 通知社区新版本

---

## 开发工具

### VSCode插件开发

CHTL项目包含完整的VSCode插件，支持：
- 语法高亮
- 智能补全
- 实时错误检测
- 模块管理
- 编译预览

### 调试工具

1. **编译器调试器**: 调试编译过程
2. **AST查看器**: 可视化AST结构
3. **扫描器跟踪**: 跟踪扫描过程
4. **性能分析器**: 分析性能瓶颈

---

## 社区参与

### 交流渠道

1. **GitHub Issues**: 报告问题和功能请求
2. **GitHub Discussions**: 技术讨论和交流
3. **开发者邮件列表**: 深度技术讨论

### 贡献认可

CHTL项目认可所有形式的贡献：
- 代码贡献者将被列入贡献者名单
- 重要贡献者可以成为项目维护者
- 优秀贡献将在发布说明中特别提及

---

## 路线图

### 短期目标

1. **语法稳定**: 完善现有语法特性
2. **性能优化**: 提升编译器性能
3. **工具完善**: 改进开发工具
4. **文档完善**: 完善开发文档

### 长期目标

1. **生态建设**: 建设模块生态系统
2. **标准化**: 推动CHTL标准化
3. **社区发展**: 扩大开发者社区
4. **技术创新**: 持续技术创新

---

CHTL项目致力于推动编程界的发展，我们欢迎所有有志于改进Web开发体验的开发者加入我们的行列。让我们一起创造更美好的编程世界！