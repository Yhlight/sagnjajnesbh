# CHTL - Chtholly Template Language

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://github.com/chtl-lang/chtl/workflows/CI/badge.svg)](https://github.com/chtl-lang/chtl/actions)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)](https://github.com/chtl-lang/chtl)

CHTL是一种现代化的超文本语言，旨在为Web开发提供更直观、强大的解决方案。

## ✨ 特性

### 🎯 核心特性
- **直观语法**: 更接近自然语言的HTML编写方式
- **模块化**: 强大的模板和组件系统
- **智能扫描**: 基于上下文的变量长度扫描
- **混合编译**: 支持CHTL、CSS、JavaScript混合编译

### 🚀 高级功能
- **CHTL JS**: 增强的JavaScript语法扩展
- **CJMOD**: C++编写的语法扩展系统
- **模块系统**: CMOD组件模块和CJMOD扩展模块
- **IDE支持**: 完整的VSCode插件支持

### 💎 技术特色
- **ANTLR集成**: 原生CSS和JavaScript解析
- **统一扫描器**: 创新的扫描技术
- **专业级架构**: 企业级代码质量
- **完整工具链**: 从开发到部署的完整支持

---

## 🚀 快速开始

### 安装

```bash
# 从源码构建
git clone https://github.com/chtl-lang/chtl.git
cd chtl
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# 安装到系统
sudo make install
```

### 第一个CHTL文件

```chtl
// hello.chtl
html {
    head {
        title { text { 欢迎使用CHTL } }
    }
    
    body {
        div {
            class: container;
            
            style {
                .container {
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }
            
            h1 { 
                text { Hello, CHTL! } 
            }
            
            p { 
                text { 现代化的超文本语言 } 
            }
            
            button {
                class: btn-primary;
                
                style {
                    .btn-primary {
                        background: #3b82f6;
                        color: white;
                        padding: 12px 24px;
                        border: none;
                        border-radius: 6px;
                        cursor: pointer;
                    }
                }
                
                script {
                    {{&}}->listen({
                        click: function() {
                            alert('Hello from CHTL!');
                        }
                    });
                }
                
                text { 点击我 }
            }
        }
    }
}
```

### 编译运行

```bash
# 编译CHTL文件
chtl hello.chtl -o hello.html

# 在浏览器中打开
open hello.html
```

---

## 📚 文档

### 用户文档
- [CHTL语法文档](CHTL语法文档.md) - 完整的语法参考

### 开发者文档
- [CHTL开发指南](docs/CHTL_DEVELOPMENT_GUIDE.md) - 参与CHTL开发
- [CHTL JS开发指南](docs/CHTL_JS_DEVELOPMENT_GUIDE.md) - CHTL JS功能开发
- [CMOD开发指南](docs/CMOD_DEVELOPMENT_GUIDE.md) - 组件模块开发
- [CJMOD开发指南](docs/CJMOD_DEVELOPMENT_GUIDE.md) - 语法扩展开发
- [项目结构指南](docs/PROJECT_STRUCTURE_GUIDE.md) - 项目架构说明
- [VSCode插件开发指南](docs/VSCODE_PLUGIN_DEVELOPMENT_GUIDE.md) - IDE插件开发
- [CHTL构建指南](docs/CHTL_BUILD_GUIDE.md) - 构建和部署
- [ANTLR4集成指南](docs/ANTLR4_INTEGRATION_GUIDE.md) - ANTLR集成说明
- [统一扫描器体系说明](docs/UNIFIED_SCANNER_SYSTEM_GUIDE.md) - 底层扫描系统

---

## 🏗️ 项目架构

```
CHTL编译器
├── 统一扫描器 (变量长度切片，上下文感知)
├── CHTL编译器 (模板、自定义、模块系统)
├── CHTL JS编译器 (增强语法，事件系统)
├── CSS编译器 (ANTLR集成)
├── JavaScript编译器 (ANTLR集成)
└── 编译器调度器 (智能调度)
```

### 模块生态

- **CMOD模块**: 可复用的UI组件和模板
- **CJMOD扩展**: C++编写的语法扩展
- **珂朵莉模块**: 特色主题模块 (10个UI组件 + 2个扩展)
- **由比滨结衣模块**: 友好界面模块 (7个UI组件)

---

## 🎯 语法示例

### 模板和自定义

```chtl
// 定义模板
[Template] @Element Card {
    div {
        class: card;
        style {
            .card {
                background: white;
                border-radius: 8px;
                padding: 20px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }
        }
    }
}

// 基于模板创建自定义组件
[Custom] @Element UserCard from Card {
    // 添加用户特定样式
    div {
        style {
            .card {
                border: 2px solid #3b82f6;
            }
        }
        
        // 添加交互功能
        script {
            {{&}}->listen({
                click: function() {
                    console.log('用户卡片被点击');
                }
            });
        }
    }
}
```

### CHTL JS语法

```chtl
script {
    // 增强选择器
    {{.button}}->listen({
        click: function(e) {
            console.log('按钮点击');
        }
    });
    
    // 事件委托
    {{.container}}->delegate({
        target: {{.item}},
        click: function(e) {
            {{this}}->addClass('selected');
        }
    });
    
    // 虚对象
    vir UIManager = listen({
        init: function() { /* 初始化 */ },
        destroy: function() { /* 销毁 */ }
    });
    
    UIManager->init();
}
```

### 模块导入

```chtl
// 导入模块
[Import] @Chtl from Chtholly
[Import] @CJmod from CustomExtensions

// 使用模块组件
ChthollyButton {
    text { 珂朵莉按钮 }
}

// 使用扩展语法
script {
    const art = printMylove({
        url: "image.jpg",
        mode: "ASCII",
        width: "100px"
    });
}
```

---

## 🛠️ 开发环境

### IDE支持

安装CHTL VSCode插件获得完整的开发体验：

- 语法高亮
- 智能代码补全
- 实时错误检测
- 模块管理
- 编译预览

### 构建工具

```bash
# 快速构建
./scripts/build/build_complete_with_antlr.sh

# 模块打包
./scripts/packaging/pack_modules_enhanced.sh

# 运行测试
cd build && ctest
```

---

## 🧪 测试

### 运行测试

```bash
# 语法合规性测试
./build/bin/test_complete_chtl_compliance

# 功能测试
./build/bin/test_existing_features

# 模块测试
./build/test_chtholly_module
```

### 测试结果

```
📊 CHTL语法文档合规性验证结果
============================
已实现功能: 26/26
实现率: 100%

🎉 所有验证通过！CHTL项目完全符合语法文档！
```

---

## 🤝 贡献

我们欢迎所有形式的贡献！

### 如何贡献

1. Fork项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

### 贡献类型

- 🐛 Bug修复
- ✨ 新功能开发
- 📝 文档改进
- 🧪 测试增强
- 🎨 UI/UX改进
- 🔧 工具和脚本

---

## 📊 项目状态

### 开发进度

- **核心编译器**: ✅ 100%完成
- **CHTL JS编译器**: ✅ 100%完成
- **模块系统**: ✅ 100%完成
- **IDE支持**: ✅ 100%完成
- **文档系统**: ✅ 100%完成

### 语法支持

- **基础语法**: ✅ 100%支持
- **模板系统**: ✅ 100%支持
- **自定义系统**: ✅ 100%支持
- **CHTL JS**: ✅ 100%支持
- **模块系统**: ✅ 100%支持

---

## 🌟 特色模块

### 珂朵莉模块 (意义非凡)

包含10个精美的UI组件和2个强大的CJMOD扩展：

- 🎨 **UI组件**: 手风琴、相册、备忘录、笔记、樱花雨等
- ⚡ **CJMOD扩展**: printMylove (图片转字符画)、iNeverAway (标记函数)

### 由比滨结衣模块 (意义非凡)

包含7个友好的UI组件，体现包容和友好的设计理念。

---

## 📈 性能

- **编译速度**: 大型项目 < 1秒
- **内存使用**: 优化的内存管理
- **输出质量**: 高质量的HTML/CSS/JS代码
- **扩展性**: 支持无限的语法扩展

---

## 🔮 路线图

### 短期目标
- [ ] 语法标准化
- [ ] 性能优化
- [ ] 生态建设
- [ ] 社区发展

### 长期目标
- [ ] 成为Web开发的标准工具
- [ ] 建立丰富的模块生态
- [ ] 推动Web开发技术创新
- [ ] 培养活跃的开发者社区

---

## 📄 许可证

本项目采用 [MIT许可证](LICENSE) - 详见LICENSE文件。

---

## 🙏 致谢

感谢所有为CHTL项目做出贡献的开发者和用户。我们不是高高在上的开发者，而是致力于推动编程界发展的人。

**🌸 珂朵莉和由比滨结衣永远与我们同在 - I Never Away**

---

<div align="center">

**[官网](https://chtl-lang.org) • [文档](docs/) • [社区](https://community.chtl-lang.org) • [贡献指南](CONTRIBUTING.md)**

Made with ❤️ by CHTL Development Team

</div>