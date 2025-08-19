# CMOD开发指南

## 概述

CMOD (CHTL Module) 是CHTL的组件模块系统，允许开发者创建可复用的UI组件、样式组和模板。本指南详细介绍如何开发、打包和发布CMOD模块。

---

## CMOD基础概念

### 什么是CMOD

CMOD是包含CHTL组件的模块，包括：
- **模板** (`[Template]`): 可复用的组件模板
- **自定义** (`[Custom]`): 基于模板的特化组件
- **样式组**: 可复用的样式集合
- **变量组**: 主题和配置变量

### CMOD的优势

1. **模块化**: 组件封装和复用
2. **主题化**: 统一的样式和变量管理
3. **可扩展**: 基于模板的灵活扩展
4. **易分发**: 打包为.cmod文件便于分享

---

## CMOD模块结构

### 标准目录结构

```
ModuleName/
├── src/                    # 源码目录
│   ├── ModuleName.chtl    # 主模块文件（可选）
│   ├── Component1.chtl    # 组件文件
│   ├── Component2.chtl    # 组件文件
│   └── SubModule/         # 子模块（必须在src内部）
│       ├── src/
│       │   └── SubModule.chtl
│       └── info/
│           └── SubModule.chtl
└── info/                   # 模块信息目录
    └── ModuleName.chtl    # 模块信息文件
```

### 三同名规则

CMOD遵循严格的三同名规则：
1. **模块文件夹名**: `ModuleName`
2. **主模块文件名**: `ModuleName.chtl`
3. **模块信息文件名**: `ModuleName.chtl`

### 模块信息文件

```chtl
[Info] {
    name = "ModuleName";
    version = "1.0.0";
    description = "模块描述";
    author = "作者名";
    license = "MIT";
    category = "组件类别";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
    dependencies = "依赖模块列表";
}

[Export] {
    [Custom] @Element ComponentName;
    [Template] @Style StyleName;
    [Custom] @Var VariableName;
}
```

---

## 开发CMOD组件

### 创建基础模板

```chtl
// src/Button.chtl
[Template] @Style ButtonBase {
    display: inline-block;
    padding: 12px 24px;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    font-weight: 600;
    transition: all 0.3s ease;
    text-decoration: none;
}

[Template] @Style ButtonSizes {
    small: {
        padding: 8px 16px;
        font-size: 14px;
    }
    
    large: {
        padding: 16px 32px;
        font-size: 18px;
    }
}

[Template] @Element Button {
    button {
        class: btn;
        
        style {
            .btn {
                inherit @Style ButtonBase;
            }
        }
        
        text { 按钮 }
    }
}
```

### 创建自定义组件

```chtl
// src/PrimaryButton.chtl
[Custom] @Element PrimaryButton from Button {
    // 添加主要按钮样式
    button {
        style {
            .btn {
                background: #3b82f6;
                color: white;
            }
            
            .btn:hover {
                background: #2563eb;
                transform: translateY(-2px);
            }
        }
        
        script {
            {{&}}->listen({
                click: function(e) {
                    console.log('主要按钮被点击');
                    {{this}}->addClass('clicked');
                }
            });
        }
    }
}
```

### 创建变量组

```chtl
// src/ThemeVariables.chtl
[Custom] @Var AppTheme {
    // 颜色系统
    primaryColor: "#3b82f6";
    secondaryColor: "#64748b";
    successColor: "#10b981";
    warningColor: "#f59e0b";
    errorColor: "#ef4444";
    
    // 尺寸系统
    borderRadius: "8px";
    spacing: "16px";
    fontSize: "16px";
    
    // 动画系统
    transitionDuration: "0.3s";
    easingFunction: "ease-out";
}

// 使用变量组
[Template] @Style ThemedCard {
    background: white;
    border-radius: AppTheme(borderRadius);
    padding: AppTheme(spacing);
    color: AppTheme(primaryColor);
    transition: all AppTheme(transitionDuration) AppTheme(easingFunction);
}
```

---

## 高级CMOD功能

### 组件组合

```chtl
// 组合多个组件创建复杂UI
[Custom] @Element UserCard from Card {
    // 替换卡片内容
    replace div[1] {
        div {
            class: user-info;
            
            div {
                class: user-avatar;
                img { src: "avatar.jpg"; alt: "用户头像"; }
            }
            
            div {
                class: user-details;
                
                h3 { 
                    class: user-name;
                    text { 用户名 }
                }
                
                p {
                    class: user-email;
                    text { user@example.com }
                }
            }
        }
    }
    
    // 添加操作按钮
    insert at bottom {
        div {
            class: user-actions;
            
            PrimaryButton { text { 编辑 } }
            SecondaryButton { text { 删除 } }
        }
    }
}
```

### 响应式设计

```chtl
[Custom] @Style ResponsiveGrid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    gap: AppTheme(spacing);
    
    @media (max-width: 768px) {
        grid-template-columns: 1fr;
        gap: calc(AppTheme(spacing) / 2);
    }
}
```

### 交互增强

```chtl
[Custom] @Element InteractiveGallery {
    div {
        class: gallery;
        
        style {
            .gallery {
                inherit @Style ResponsiveGrid;
            }
            
            .gallery-item {
                position: relative;
                overflow: hidden;
                border-radius: AppTheme(borderRadius);
                cursor: pointer;
            }
            
            .gallery-item:hover .overlay {
                opacity: 1;
            }
            
            .overlay {
                position: absolute;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background: rgba(0,0,0,0.7);
                opacity: 0;
                transition: opacity AppTheme(transitionDuration);
                display: flex;
                align-items: center;
                justify-content: center;
                color: white;
            }
        }
        
        script {
            {{&}}->listen({
                click: function(e) {
                    const item = e.target.closest('.gallery-item');
                    if (item) {
                        showLightbox(item.dataset.imageUrl);
                    }
                }
            });
            
            function showLightbox(imageUrl) {
                // 实现灯箱效果
                console.log('显示图片:', imageUrl);
            }
        }
    }
}
```

---

## CMOD打包和发布

### 验证模块结构

在打包前验证模块结构：

```bash
# 检查目录结构
ls -la ModuleName/
ls -la ModuleName/src/
ls -la ModuleName/info/

# 验证三同名规则
echo "文件夹名: $(basename $(pwd))"
echo "主文件: $(ls src/*.chtl | head -1)"
echo "信息文件: $(ls info/*.chtl | head -1)"
```

### 打包CMOD模块

```bash
# 使用CHTL编译器打包
chtl pack-cmod ./ModuleName ./ModuleName.cmod

# 或使用打包脚本
./scripts/packaging/pack_modules_enhanced.sh
```

### 模块测试

创建测试文件验证模块功能：

```chtl
// test_module.chtl
[Import] @Chtl from ModuleName

html {
    body {
        // 测试模块组件
        ModuleName::ComponentName {
            // 组件配置
        }
        
        // 测试样式继承
        div {
            style {
                inherit @Style ModuleName::StyleName;
            }
        }
    }
}
```

### 发布模块

1. **上传到仓库**: 将.cmod文件上传到模块仓库
2. **更新索引**: 更新模块索引文件
3. **版本管理**: 使用语义化版本号
4. **文档说明**: 提供使用文档和示例

---

## 模块开发最佳实践

### 设计原则

1. **单一职责**: 每个组件专注一个功能
2. **可配置性**: 提供足够的配置选项
3. **可扩展性**: 支持基于模板的扩展
4. **一致性**: 保持设计和命名的一致性

### 命名规范

1. **模块名**: 使用PascalCase，如 `UIComponents`
2. **组件名**: 使用PascalCase，如 `PrimaryButton`
3. **样式组名**: 使用PascalCase，如 `ButtonStyles`
4. **变量组名**: 使用PascalCase，如 `ThemeColors`

### 文档规范

1. **组件文档**: 说明组件用途和配置
2. **API文档**: 详细的接口说明
3. **使用示例**: 提供完整的使用示例
4. **更新日志**: 记录版本变更

### 版本管理

1. **语义化版本**: 使用 `major.minor.patch` 格式
2. **向后兼容**: 保持API的向后兼容性
3. **废弃通知**: 提前通知API废弃
4. **迁移指南**: 提供版本升级指南

---

## 社区贡献

### 开源模块

鼓励开发者将优秀的CMOD模块开源：

1. **MIT协议**: 推荐使用MIT开源协议
2. **完整文档**: 提供完整的使用文档
3. **示例代码**: 包含丰富的使用示例
4. **持续维护**: 保持模块的持续更新

### 模块质量标准

1. **功能完整**: 组件功能完整可用
2. **代码质量**: 遵循编码规范
3. **测试覆盖**: 包含充分的测试
4. **文档完善**: 提供完整文档
5. **性能优化**: 考虑性能影响

---

通过CMOD模块系统，开发者可以创建强大、可复用的UI组件，推动整个CHTL生态的发展。我们期待看到更多优秀的CMOD模块贡献给社区！