# CHTL Compiler

CHTL (C++ HyperText Language) 编译器实现，基于C++17开发。

## 架构概述

```
CHTL源代码 → CHTLUnifiedScanner → 代码片段 → CompilerDispatcher → 各种编译器 → 结果合并 → HTML输出
```

### 核心组件

1. **CHTLUnifiedScanner** - 精准代码切割器
   - 将CHTL源代码切割成不同类型的片段
   - 识别CHTL、CHTL JS、CSS、JavaScript等代码块

2. **CompilerDispatcher** - 编译器调度器
   - 将代码片段分发到对应的编译器
   - 协调各编译器的工作

3. **编译器**
   - **CHTL Compiler** - 手写实现，处理CHTL核心语法和局部样式
   - **CHTL JS Compiler** - 手写实现，处理CHTL JS扩展语法
   - **CSS Compiler** - ANTLR集成（带备用实现），处理全局CSS
   - **JavaScript Compiler** - ANTLR集成（带备用实现），处理标准JavaScript

4. **ResultMerger** - 结果合并器
   - 将各编译器的输出合并成最终的HTML文档

## 构建要求

- C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- CMake 3.14+
- 可选：ANTLR4运行时库（用于CSS和JavaScript编译器的完整功能）

## 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make -j4

# 运行测试
make test
```

## 使用方法

```bash
# 基本用法
./chtlc input.chtl

# 指定输出文件
./chtlc -o output.html input.chtl

# 设置文档选项
./chtlc --title "My Page" --lang zh-CN input.chtl
```

## 示例

查看 `examples` 目录中的示例文件：
- `hello_world.chtl` - 基础示例
- `advanced.chtl` - 高级功能演示

## 开发状态

- ✅ 项目基础结构
- ✅ CHTLUnifiedScanner实现
- ✅ CompilerDispatcher实现
- ✅ CHTL编译器基础框架
- ✅ CHTL JS编译器基础框架
- ✅ CSS编译器接口（带备用实现）
- ✅ JavaScript编译器接口（带备用实现）
- ✅ 结果合并器和HTML输出
- ✅ 主程序和测试用例

## 许可证

MIT License