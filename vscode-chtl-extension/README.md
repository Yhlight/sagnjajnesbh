# CHTL Language Support for VSCode

一个功能强大的 VSCode 扩展，为 CHTL (C++ HyperText Language) 和 CHTL JS 提供完整的现代化 IDE 支持。

## ✨ 主要特性

### 🎯 智能代码补全
- **上下文感知补全**：根据当前代码上下文提供精准的建议
- **语法结构智能提示**：支持 Template、Custom、Configuration、Namespace、Import 等语法块
- **CHTL JS 语法支持**：完整支持 `{{}}` 表达式、虚对象 `vir`、以及 `iNeverAway`、`printMylove` 等函数
- **模块导入智能提示**：自动识别和提示可用的 CJMOD 模块

### 🎨 语法高亮
- **CHTL 语法高亮**：完整支持所有 CHTL 语法元素的高亮显示
- **CHTL JS 语法高亮**：专门针对 `.cjmod` 文件的语法高亮
- **嵌套语法支持**：在 style 块中高亮 CSS，在 script 块中高亮 JavaScript 和 CHTL JS

### 🔍 智能错误检测
- **实时语法检查**：编辑时即时检测语法错误
- **语义分析**：检测未定义引用、重复定义、循环引用等问题
- **编译器集成验证**：结合内置编译器进行深度代码验证
- **详细错误信息**：提供准确的错误位置和修复建议

### 🚫 自动补全冲突解决
- **智能上下文检测**：自动识别当前位置应使用 CHTL 还是 VSCode 默认补全
- **三种解决策略**：智能检测、偏向 CHTL、偏向 VSCode
- **状态栏控制**：一键切换补全策略
- **置信度评估**：显示检测结果的可信度

### 🔧 内置编译器
- **即时编译**：保存时自动编译 CHTL 文件
- **实时预览**：支持编译结果的实时预览
- **多编译器支持**：内置编译器、系统编译器、JavaScript 后备编译器
- **错误解析**：智能解析编译错误并在编辑器中标注

### 🔄 模块系统
- **自动解包**：按路径搜索自动解包导入的模块
- **路径解析**：支持相对路径、绝对路径、通配符路径
- **缓存管理**：智能缓存模块信息，提升性能
- **分类目录支持**：支持 CMOD/CJMOD 分类目录结构

### 🐛 调试支持
- **断点调试**：支持在 CHTL 代码中设置断点
- **堆栈跟踪**：查看详细的调用堆栈信息
- **变量监控**：监控局部变量、全局变量和 CHTL 上下文
- **编译监控**：实时监控文件变化和重新编译

### 📐 代码格式化
- **自动格式化**：支持整个文档或选定范围的格式化
- **可配置选项**：缩进、换行、对齐等多种格式化选项
- **语法感知**：根据不同语法上下文应用不同的格式化规则
- **CHTL JS 专项格式化**：针对 CHTL JS 语法的特殊格式化支持

## 🚀 快速开始

### 安装
1. 在 VSCode 扩展市场搜索 "CHTL Language Support"
2. 点击安装并重启 VSCode
3. 打开 `.chtl` 或 `.cjmod` 文件即可开始使用

### 基本配置
```json
{
  "chtl.compiler.path": "path/to/chtl-compiler",
  "chtl.intellisense.conflictResolution": "smart-detection",
  "chtl.modules.searchPaths": [
    "./modules",
    "./cmod",
    "./cjmod"
  ],
  "chtl.validation.enableRealTime": true,
  "chtl.format.alignKeyValuePairs": true
}
```

## 📋 支持的文件类型

| 扩展名 | 语言 | 描述 |
|--------|------|------|
| `.chtl` | CHTL | CHTL 主文件 |
| `.cjmod` | CHTL JS | CHTL JS 模块文件 |
| `.cmod` | CMOD | CMOD 模块文件 |

## ⌨️ 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+Shift+P` → `CHTL: Compile Current` | 编译当前文件 |
| `Ctrl+Shift+P` → `CHTL: Compile Project` | 编译整个项目 |
| `Ctrl+Shift+P` → `CHTL: Open Preview` | 打开编译预览 |
| `Ctrl+Shift+P` → `CHTL: Refresh Modules` | 刷新模块缓存 |
| `Ctrl+Shift+P` → `CHTL: Toggle Conflict Resolution` | 切换补全策略 |

## 🎛️ 配置选项

### 编译器配置
```json
{
  "chtl.compiler.path": "",                    // 编译器路径（留空使用内置）
  "chtl.compiler.enableRealTimeCompilation": true,  // 启用实时编译
  "chtl.compiler.outputDirectory": "./dist",   // 输出目录
  "chtl.compiler.generateSourceMaps": true     // 生成源映射
}
```

### 智能提示配置
```json
{
  "chtl.intellisense.conflictResolution": "smart-detection",  // 冲突解决策略
  "chtl.intellisense.enableAutoComplete": true,               // 启用自动补全
  "chtl.intellisense.enableHover": true,                      // 启用悬停提示
  "chtl.intellisense.enableDefinition": true                  // 启用定义跳转
}
```

### 模块配置
```json
{
  "chtl.modules.searchPaths": [               // 模块搜索路径
    "./modules",
    "./cmod", 
    "./cjmod"
  ],
  "chtl.modules.enableCache": true,           // 启用模块缓存
  "chtl.modules.autoResolution": true         // 自动模块解析
}
```

### 验证配置
```json
{
  "chtl.validation.enableRealTime": true,     // 实时验证
  "chtl.validation.enableSyntaxCheck": true,  // 语法检查
  "chtl.validation.enableSemanticCheck": true, // 语义检查
  "chtl.validation.enableCompilerCheck": true  // 编译器检查
}
```

### 调试配置
```json
{
  "chtl.debug.enableDetailedLogs": false,     // 详细日志
  "chtl.debug.enableBreakpoints": true,       // 断点支持
  "chtl.debug.enableVariableInspection": true // 变量检查
}
```

### 格式化配置
```json
{
  "chtl.format.indentSize": 4,                // 缩进大小
  "chtl.format.maxLineLength": 120,           // 最大行长度
  "chtl.format.alignKeyValuePairs": true,     // 对齐键值对
  "chtl.format.insertFinalNewline": true,     // 插入最终换行
  "chtl.format.trimTrailingWhitespace": true  // 修剪尾随空格
}
```

## 🎯 CHTL 语法支持

### 模板语法
```chtl
[Template] @Element myButton {
    button {
        class: btn-primary
        style {
            background-color: blue
            &:hover {
                background-color: darkblue
            }
        }
        script {
            {{ 
                function onClick() {
                    console.log('Button clicked');
                }
            }}
        }
    }
}
```

### 配置语法
```chtl
[Configuration] @Config {
    DEBUG_MODE = true
    INDEX_INITIAL_COUNT = 1
    [Name] {
        defaultButton = "Submit"
    }
}
```

### 导入语法
```chtl
[Import] CHTL "components/button.chtl"
[Import] CJMOD "utils/helpers.cjmod"
[Import] MEDIA_STYLE "styles/main.css" as mainStyles
```

## 🔧 CHTL JS 语法支持

### 虚对象
```javascript
vir {
    clickHandler: function() { /* ... */ },
    data: someValue,
    customKey
}
```

### CHTL JS 函数
```javascript
iNeverAway {
    onInit: function() { /* 初始化 */ },
    onDestroy: function() { /* 清理 */ },
    customMethod: myFunction
}

printMylove {
    imageUrl: "./image.jpg",
    charSet: "ascii",
    outputFormat: "html"
}
```

## 🐛 故障排除

### 常见问题

**Q: 自动补全不工作**
A: 检查 `chtl.intellisense.conflictResolution` 设置，尝试切换到 "prefer-chtl" 模式。

**Q: 编译错误不显示**
A: 确保 `chtl.validation.enableCompilerCheck` 设置为 true，并检查编译器路径配置。

**Q: 模块导入失败**
A: 检查 `chtl.modules.searchPaths` 配置，确保包含正确的模块路径。

**Q: 语法高亮不正确**
A: 确认文件扩展名正确（`.chtl` 或 `.cjmod`），重启 VSCode 后重试。

### 诊断命令
- `CHTL: Validate Syntax` - 验证当前文件语法
- `CHTL: Refresh Modules` - 刷新模块缓存
- `CHTL: Toggle Debug Logs` - 切换调试日志

## 🤝 贡献

欢迎贡献代码、报告问题或提出功能建议！

### 开发环境设置
```bash
git clone <repository-url>
cd vscode-chtl-extension
npm install
code .
```

按 `F5` 启动扩展开发主机进行测试。

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🔗 相关链接

- [CHTL 语法文档](./docs/chtl-grammar.md)
- [CHTL JS 语法文档](./docs/chtl-js-grammar.md)
- [API 参考](./docs/api-reference.md)
- [问题反馈](https://github.com/your-repo/issues)

## 📊 版本历史

### v1.0.0
- ✨ 初始版本发布
- 🎯 完整的 CHTL 语法支持
- 🎨 语法高亮和智能补全
- 🔍 实时错误检测
- 🐛 调试支持
- 📐 代码格式化

---

**享受使用 CHTL Language Support！** 🚀

如有任何问题或建议，请随时联系我们。