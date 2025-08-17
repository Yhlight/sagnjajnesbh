# CHTL 编译器

基于C++17实现的超文本语言(CHTL)编译器，提供了一种更符合开发者编写HTML代码的方式。

## 项目特性

- **精准代码切割**: CHTLUnifiedScanner 提供基于可变长度切片的精准代码片段识别
- **编译器调度**: CompilerDispatcher 协调不同类型编译器处理相应代码片段
- **模块化架构**: 支持CHTL、CHTL JS、CSS、JavaScript的独立编译
- **模板系统**: 支持样式组模板、元素模板、变量组模板
- **自定义系统**: 提供灵活的自定义内容和特例化操作
- **模块管理**: CMOD和CJMOD模块系统支持
- **命名空间**: 防止模块污染的命名空间机制

## 编译器架构

```
┌─────────────────────────────────────────────────────────────────┐
│                         CHTL源代码                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CHTLUnifiedScanner                           │
│                   (精准代码切割器)                               │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
        ┌──────────────┬──────────────┬──────────────┬────────────┐
        │   CHTL片段   │ CHTL JS片段  │   CSS片段    │   JS片段   │
        └──────┬───────┴──────┬───────┴──────┬───────┴──────┬─────┘
               │              │              │              │
               ▼              ▼              ▼              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CompilerDispatcher                           │
│                   (编译器调度器)                                 │
└──────┬──────────────┬──────────────┬──────────────┬────────────┘
       │              │              │              │
       ▼              ▼              ▼              ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│    CHTL     │ │   CHTL JS   │ │     CSS     │ │JavaScript   │
│  Compiler   │ │  Compiler   │ │  Compiler   │ │  Compiler   │
│  (手写)     │ │  (手写)     │ │  (ANTLR)    │ │  (ANTLR)    │
└─────────────┘ └─────────────┘ └─────────────┘ └─────────────┘
       │              │              │              │
       └──────────────┴──────────────┴──────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      编译结果合并                                │
│                     (HTML输出)                                   │
└─────────────────────────────────────────────────────────────────┘
```

## 构建说明

### 系统要求

- C++17 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16 或更高版本
- Linux、macOS 或 Windows 系统

### 编译步骤

```bash
# 克隆仓库
git clone <repository-url>
cd chtl-compiler

# 创建构建目录
mkdir build
cd build

# 配置和编译
cmake ..
make

# 或者在Windows上使用
cmake --build .
```

## 使用方法

### 基本用法

```bash
# 编译CHTL文件
./chtl-compiler input.chtl

# 指定输出文件
./chtl-compiler -o output.html input.chtl

# 启用调试模式
./chtl-compiler -d input.chtl

# 添加模块搜索路径
./chtl-compiler -m ./modules input.chtl
```

### 命令行选项

- `-o, --output <file>`: 指定输出文件 (默认: output.html)
- `-d, --debug`: 启用调试模式
- `-m, --module-path <dir>`: 添加模块搜索路径
- `-h, --help`: 显示帮助信息
- `-v, --version`: 显示版本信息

## CHTL 语法示例

### 基本元素

```chtl
html
{
    head
    {
        text
        {
            "页面标题"
        }
    }

    body
    {
        div
        {
            id: container;
            class: main-content;
            
            text
            {
                "Hello, CHTL!"
            }
        }
    }
}
```

### 局部样式块

```chtl
div
{
    style
    {
        width: 100%;
        height: 200px;
        
        .container
        {
            padding: 20px;
        }
        
        &:hover
        {
            background-color: #f0f0f0;
        }
    }
}
```

### CHTL JS 增强语法

```chtl
div
{
    script
    {
        {{container}}->addEventListener('click', () => {
            console.log('Clicked!');
        });
        
        {{button}}->listen({
            click: handleClick,
            mouseenter: handleMouseEnter
        });
    }
}
```

### 模板系统

```chtl
[Template] @Style DefaultButton
{
    padding: 10px 20px;
    border: 1px solid #ccc;
    background-color: #f8f8f8;
}

[Template] @Element Card
{
    div
    {
        style
        {
            @Style DefaultButton;
            border-radius: 4px;
        }
    }
}
```

## 项目状态

### 已完成
- ✅ 项目基础结构和CMake构建系统
- ✅ CHTLUnifiedScanner (精准代码切割器)
- ✅ CompilerDispatcher (编译器调度器)
- ✅ 基本的编译流程和HTML输出

### 开发中
- 🚧 CHTL编译器 (词法分析器和解析器)
- 🚧 CHTL JS编译器 (词法分析器和解析器)
- 🚧 CSS和JavaScript编译器 (ANTLR集成)
- 🚧 CMOD和CJMOD模块系统
- 🚧 完整的AST节点和代码生成器

## 开发计划

1. **词法分析和语法解析**: 实现完整的CHTL和CHTL JS语法解析
2. **AST构建**: 创建抽象语法树节点和遍历机制
3. **代码生成**: 实现HTML、CSS、JavaScript代码生成器
4. **模块系统**: 完成CMOD和CJMOD模块打包和加载
5. **错误处理**: 增强错误报告和调试信息
6. **优化**: 性能优化和内存管理改进

## 贡献指南

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 许可证

本项目使用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有问题或建议，请创建 Issue 或联系开发团队。