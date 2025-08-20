# CHTL 编译器

CHTL（Cascading HyperText Language）是基于 C++ 实现的超文本语言编译器，提供更符合开发者习惯的 HTML 编写方式。

## 特性

- **模块化架构**：CHTL 和 CHTL JS 完全独立的编译系统
- **精准代码切割**：智能识别代码片段边界
- **丰富的语法支持**：模板、自定义组件、命名空间等
- **模块系统**：支持 CMOD 和 CJMOD 模块化开发
- **约束系统**：确保语法正确性和边界控制

## 快速开始

### 编译项目

```bash
mkdir build && cd build
cmake ..
make -j4
```

### 基本使用

```bash
# 编译 CHTL 文件
./chtl input.chtl -o output.html

# 启用调试模式
./chtl input.chtl -o output.html -d

# 指定模块路径
./chtl input.chtl -o output.html -m /path/to/modules
```

### 模块打包

```bash
# 打包 CMOD 模块
./chtl --pack-cmod /path/to/module/folder

# 打包 CJMOD 模块
./chtl --pack-cjmod /path/to/module/folder
```

## 项目结构

详见 [项目架构说明](docs/项目架构说明.md)

## 开发计划

1. ✅ 项目基础结构和目录架构
2. ⏳ CHTLUnifiedScanner 统一扫描器
3. ⏳ CHTL 编译器基础架构
4. ⏳ CHTL JS 编译器基础架构
5. ⏳ CompilerDispatcher 编译器调度器
6. ⏳ AST 节点实现
7. ⏳ 解析器和生成器
8. ⏳ RAII 状态机和上下文管理
9. ⏳ Import 系统增强
10. ⏳ 命名空间系统
11. ⏳ CMOD/CJMOD 模块系统
12. ⏳ 约束器实现
13. ⏳ ANTLR4 集成
14. ⏳ 结果合并器
15. ⏳ 测试和优化

## 许可证

MIT License