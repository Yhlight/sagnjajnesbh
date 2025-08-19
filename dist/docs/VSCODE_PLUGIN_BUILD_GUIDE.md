# VSCode插件构建指南

## 概述

本指南详细介绍如何构建、测试和发布CHTL VSCode插件，包括开发环境设置、构建流程和发布过程。

---

## 开发环境设置

### 前置要求

- **Node.js**: 16.0+ (推荐18.0+)
- **npm**: 8.0+ 或 **yarn**: 1.22+
- **VSCode**: 1.74.0+
- **TypeScript**: 4.9+

### 工具安装

```bash
# 安装全局工具
npm install -g @vscode/vsce typescript

# 验证安装
vsce --version
tsc --version
```

---

## 项目结构

### 插件目录结构

```
vscode-chtl-extension/
├── package.json                    # 插件配置文件
├── tsconfig.json                   # TypeScript配置
├── webpack.config.js               # Webpack配置
├── .vscodeignore                   # VSCode忽略文件
├── src/                            # 源代码
│   ├── extension.ts                # 主扩展文件
│   ├── providers/                  # 功能提供器
│   │   ├── completionProvider.ts   # 代码补全
│   │   ├── diagnosticProvider.ts   # 错误诊断
│   │   ├── hoverProvider.ts        # 悬停提示
│   │   ├── definitionProvider.ts   # 定义跳转
│   │   └── formattingProvider.ts   # 代码格式化
│   ├── managers/                   # 管理器
│   │   ├── moduleManager.ts        # 模块管理
│   │   ├── compilerManager.ts      # 编译器管理
│   │   └── configManager.ts        # 配置管理
│   ├── utils/                      # 工具函数
│   │   ├── syntaxValidator.ts      # 语法验证
│   │   ├── pathResolver.ts         # 路径解析
│   │   └── fileUtils.ts            # 文件工具
│   └── webviews/                   # Web视图
│       ├── moduleManager.html      # 模块管理界面
│       └── preview.html            # 预览界面
├── syntaxes/                       # 语法定义
│   ├── chtl.tmLanguage.json        # 语法高亮
│   └── chtl.configuration.json     # 语言配置
├── snippets/                       # 代码片段
│   └── chtl.json                   # CHTL代码片段
├── icons/                          # 图标资源
├── themes/                         # 主题定义
└── test/                           # 测试文件
    ├── suite/                      # 测试套件
    └── fixtures/                   # 测试数据
```

---

## 构建配置

### package.json配置

```json
{
    "name": "chtl",
    "displayName": "CHTL Language Support",
    "description": "完整的CHTL语言支持 - 智能提示、语法高亮、模块管理",
    "version": "1.0.0",
    "publisher": "chtl-dev",
    "license": "MIT",
    "repository": {
        "type": "git",
        "url": "https://github.com/chtl-lang/chtl.git"
    },
    "engines": {
        "vscode": "^1.74.0"
    },
    "categories": [
        "Programming Languages",
        "Snippets",
        "Formatters",
        "Linters"
    ],
    "keywords": [
        "chtl",
        "template",
        "web",
        "html",
        "css",
        "javascript"
    ],
    "activationEvents": [
        "onLanguage:chtl"
    ],
    "main": "./out/extension.js",
    "contributes": {
        "languages": [
            {
                "id": "chtl",
                "aliases": ["CHTL", "chtl"],
                "extensions": [".chtl"],
                "configuration": "./syntaxes/chtl.configuration.json"
            }
        ],
        "grammars": [
            {
                "language": "chtl",
                "scopeName": "source.chtl",
                "path": "./syntaxes/chtl.tmLanguage.json"
            }
        ],
        "snippets": [
            {
                "language": "chtl",
                "path": "./snippets/chtl.json"
            }
        ],
        "commands": [
            {
                "command": "chtl.compile",
                "title": "编译CHTL文件",
                "category": "CHTL",
                "icon": "$(play)"
            },
            {
                "command": "chtl.preview",
                "title": "预览CHTL文件",
                "category": "CHTL",
                "icon": "$(preview)"
            },
            {
                "command": "chtl.moduleManager",
                "title": "模块管理器",
                "category": "CHTL",
                "icon": "$(package)"
            }
        ],
        "keybindings": [
            {
                "command": "chtl.compile",
                "key": "ctrl+shift+c",
                "when": "editorLangId == chtl"
            },
            {
                "command": "chtl.preview",
                "key": "ctrl+shift+p",
                "when": "editorLangId == chtl"
            }
        ],
        "menus": {
            "editor/context": [
                {
                    "when": "editorLangId == chtl",
                    "command": "chtl.compile",
                    "group": "chtl"
                }
            ]
        }
    },
    "scripts": {
        "vscode:prepublish": "npm run compile",
        "compile": "tsc -p ./",
        "watch": "tsc -watch -p ./",
        "pretest": "npm run compile && npm run lint",
        "lint": "eslint src --ext ts",
        "test": "node ./out/test/runTest.js",
        "package": "vsce package",
        "publish": "vsce publish"
    },
    "devDependencies": {
        "@types/vscode": "^1.74.0",
        "@types/node": "^18.15.0",
        "@typescript-eslint/eslint-plugin": "^5.56.0",
        "@typescript-eslint/parser": "^5.56.0",
        "eslint": "^8.36.0",
        "typescript": "^4.9.5",
        "@vscode/test-electron": "^2.3.0",
        "@vscode/vsce": "^2.18.0"
    }
}
```

### TypeScript配置

```json
// tsconfig.json
{
    "compilerOptions": {
        "module": "commonjs",
        "target": "ES2020",
        "outDir": "out",
        "lib": ["ES2020"],
        "sourceMap": true,
        "rootDir": "src",
        "strict": true,
        "esModuleInterop": true,
        "skipLibCheck": true,
        "forceConsistentCasingInFileNames": true,
        "resolveJsonModule": true
    },
    "exclude": [
        "node_modules",
        ".vscode-test"
    ]
}
```

### Webpack配置 (可选)

```javascript
// webpack.config.js
const path = require('path');

module.exports = {
    target: 'node',
    mode: 'none',
    entry: './src/extension.ts',
    output: {
        path: path.resolve(__dirname, 'out'),
        filename: 'extension.js',
        libraryTarget: 'commonjs2'
    },
    externals: {
        vscode: 'commonjs vscode'
    },
    resolve: {
        extensions: ['.ts', '.js']
    },
    module: {
        rules: [
            {
                test: /\.ts$/,
                exclude: /node_modules/,
                use: [
                    {
                        loader: 'ts-loader'
                    }
                ]
            }
        ]
    },
    devtool: 'nosources-source-map',
    infrastructureLogging: {
        level: "log"
    }
};
```

---

## 构建流程

### 开发构建

```bash
# 进入插件目录
cd vscode-chtl-extension

# 安装依赖
npm install

# 编译TypeScript
npm run compile

# 监听文件变化
npm run watch
```

### 测试构建

```bash
# 运行Lint检查
npm run lint

# 运行单元测试
npm test

# 运行集成测试
npm run test:integration
```

### 生产构建

```bash
# 预发布构建
npm run vscode:prepublish

# 打包插件
vsce package

# 验证包内容
vsce ls
```

---

## 插件测试

### 单元测试框架

```typescript
// test/suite/extension.test.ts
import * as assert from 'assert';
import * as vscode from 'vscode';
import { CHTLCompletionProvider } from '../../src/providers/completionProvider';

suite('CHTL Extension Test Suite', () => {
    
    vscode.window.showInformationMessage('开始运行CHTL插件测试');
    
    test('代码补全功能测试', async () => {
        const provider = new CHTLCompletionProvider();
        
        // 创建测试文档
        const document = await vscode.workspace.openTextDocument({
            content: '[Import] @',
            language: 'chtl'
        });
        
        const position = new vscode.Position(0, 10);
        const completions = provider.provideCompletionItems(document, position);
        
        assert.ok(completions.length > 0, '应该提供补全项');
        assert.ok(
            completions.some(item => item.label === '@Chtl'),
            '应该包含@Chtl补全项'
        );
    });
    
    test('语法验证功能测试', () => {
        const diagnosticProvider = new CHTLDiagnosticProvider();
        
        const testCode = 'delete .class-name;'; // 错误语法
        const diagnostics = diagnosticProvider.validateLine(testCode, 0);
        
        assert.ok(diagnostics.length > 0, '应该检测到语法错误');
        assert.ok(
            diagnostics[0].message.includes('delete不支持CSS选择器'),
            '应该提供正确的错误信息'
        );
    });
    
    test('模块扫描功能测试', async () => {
        const moduleManager = new CHTLModuleManager();
        const modules = await moduleManager.scanModules();
        
        assert.ok(Array.isArray(modules), '应该返回模块数组');
        
        // 如果存在珂朵莉模块，验证其结构
        const chthollyModule = modules.find(m => m.name === 'Chtholly');
        if (chthollyModule) {
            assert.equal(chthollyModule.type, 'CMOD', '珂朵莉模块应该是CMOD类型');
        }
    });
});
```

### 集成测试

```typescript
// test/suite/integration.test.ts
suite('CHTL Integration Tests', () => {
    
    test('编译器集成测试', async () => {
        // 创建临时CHTL文件
        const tempFile = path.join(__dirname, 'temp.chtl');
        const testContent = `
            div {
                class: test;
                text { 测试内容 }
            }
        `;
        
        fs.writeFileSync(tempFile, testContent);
        
        try {
            const compilerIntegration = new CHTLCompilerIntegration();
            const result = await compilerIntegration.compileFile(tempFile);
            
            assert.ok(result.success, '编译应该成功');
            assert.ok(fs.existsSync(result.outputPath), '应该生成输出文件');
            
            const htmlContent = fs.readFileSync(result.outputPath, 'utf-8');
            assert.ok(htmlContent.includes('<div class="test">'), '应该包含正确的HTML');
            
        } finally {
            // 清理临时文件
            [tempFile, tempFile.replace('.chtl', '.html')].forEach(file => {
                if (fs.existsSync(file)) fs.unlinkSync(file);
            });
        }
    });
    
    test('模块管理界面测试', async () => {
        const moduleManager = new CHTLModuleManager();
        
        // 测试模块创建向导
        const mockInput = {
            moduleType: 'CMOD',
            moduleName: 'TestModule',
            format: 'CMOD'
        };
        
        await moduleManager.createModuleWithInput(mockInput);
        
        // 验证模块创建
        const expectedPath = path.join(
            vscode.workspace.rootPath || '',
            'src', 'Module', 'CMOD', 'TestModule'
        );
        
        assert.ok(fs.existsSync(expectedPath), '应该创建模块目录');
        assert.ok(
            fs.existsSync(path.join(expectedPath, 'src', 'TestModule.chtl')),
            '应该创建主模块文件'
        );
        assert.ok(
            fs.existsSync(path.join(expectedPath, 'info', 'TestModule.chtl')),
            '应该创建信息文件'
        );
    });
});
```

---

## 语法高亮构建

### TextMate语法定义

```json
// syntaxes/chtl.tmLanguage.json
{
    "name": "CHTL",
    "scopeName": "source.chtl",
    "fileTypes": ["chtl"],
    "patterns": [
        { "include": "#comments" },
        { "include": "#sections" },
        { "include": "#imports" },
        { "include": "#keywords" },
        { "include": "#strings" },
        { "include": "#chtljs" },
        { "include": "#css" }
    ],
    "repository": {
        "comments": {
            "patterns": [
                {
                    "name": "comment.line.double-slash.chtl",
                    "match": "//.*$"
                },
                {
                    "name": "comment.block.chtl",
                    "begin": "/\\*",
                    "end": "\\*/",
                    "name": "comment.block.chtl"
                },
                {
                    "name": "comment.generator.chtl",
                    "match": "--.*$"
                }
            ]
        },
        "sections": {
            "patterns": [
                {
                    "name": "keyword.control.section.chtl",
                    "match": "\\[(Info|Export|Import|Namespace|Template|Custom|Origin|Configuration)\\]"
                },
                {
                    "name": "keyword.control.type.chtl",
                    "match": "@(Element|Style|Var|Html|JavaScript|Config|Chtl|CJmod)"
                }
            ]
        },
        "chtljs": {
            "patterns": [
                {
                    "name": "support.function.chtljs.chtl",
                    "begin": "\\{\\{",
                    "end": "\\}\\}",
                    "patterns": [
                        {
                            "name": "entity.name.tag.css.chtl",
                            "match": "\\.[a-zA-Z][a-zA-Z0-9_-]*"
                        },
                        {
                            "name": "entity.other.attribute-name.id.css.chtl",
                            "match": "#[a-zA-Z][a-zA-Z0-9_-]*"
                        },
                        {
                            "name": "keyword.operator.chtl",
                            "match": "&"
                        }
                    ]
                },
                {
                    "name": "keyword.operator.arrow.chtl",
                    "match": "->"
                },
                {
                    "name": "keyword.control.chtljs.chtl",
                    "match": "\\b(listen|delegate|animate|vir)\\b"
                }
            ]
        }
    }
}
```

### 语言配置

```json
// syntaxes/chtl.configuration.json
{
    "comments": {
        "lineComment": "//",
        "blockComment": ["/*", "*/"]
    },
    "brackets": [
        ["{", "}"],
        ["[", "]"],
        ["(", ")"]
    ],
    "autoClosingPairs": [
        ["{", "}"],
        ["[", "]"],
        ["(", ")"],
        ["\"", "\""],
        ["'", "'"]
    ],
    "surroundingPairs": [
        ["{", "}"],
        ["[", "]"],
        ["(", ")"],
        ["\"", "\""],
        ["'", "'"]
    ],
    "folding": {
        "markers": {
            "start": "^\\s*//\\s*#region",
            "end": "^\\s*//\\s*#endregion"
        }
    },
    "wordPattern": "\\b[a-zA-Z_][a-zA-Z0-9_]*\\b",
    "indentationRules": {
        "increaseIndentPattern": "^.*\\{[^}\"']*$",
        "decreaseIndentPattern": "^\\s*\\}"
    }
}
```

---

## 代码片段构建

### 片段定义

```json
// snippets/chtl.json
{
    "CMOD组件": {
        "prefix": ["cmod", "component"],
        "body": [
            "[Custom] @Element ${1:ComponentName} {",
            "    ${2:div} {",
            "        class: ${3:component-class};",
            "        ",
            "        style {",
            "            .${3:component-class} {",
            "                ${4:padding}: ${5:16px};",
            "                ${6:border-radius}: ${7:8px};",
            "            }",
            "        }",
            "        ",
            "        script {",
            "            {{&}}->listen({",
            "                ${8:click}: function(${9:e}) {",
            "                    ${10:// 事件处理}",
            "                }",
            "            });",
            "        }",
            "        ",
            "        text { ${11:组件内容} }",
            "    }",
            "}"
        ],
        "description": "创建CMOD组件模板"
    },
    
    "CHTL JS监听器": {
        "prefix": ["listen", "event"],
        "body": [
            "{{${1:.selector}}}->listen({",
            "    ${2|click,mouseenter,mouseleave,focus,blur,input,change|}: function(${3:e}) {",
            "        ${4:// 事件处理逻辑}",
            "    }",
            "});"
        ],
        "description": "创建CHTL JS事件监听器"
    },
    
    "变量组定义": {
        "prefix": ["var", "variables"],
        "body": [
            "[${1|Template,Custom|}] @Var ${2:VariableGroupName} {",
            "    ${3:propertyName}: \"${4:value}\";",
            "    ${5:anotherProperty}: \"${6:anotherValue}\";",
            "}"
        ],
        "description": "创建变量组定义"
    },
    
    "模块导入": {
        "prefix": ["import", "module"],
        "body": [
            "[Import] @${1|Chtl,CJmod,Html,Style,JavaScript|} from ${2:moduleName}${3: as ${4:alias}}"
        ],
        "description": "导入模块或文件"
    }
}
```

---

## 构建自动化

### 构建脚本

```bash
#!/bin/bash
# scripts/build_vscode_extension.sh

set -e

echo "🚀 构建CHTL VSCode插件"

# 进入插件目录
cd vscode-chtl-extension

# 清理旧构建
rm -rf out/ node_modules/ *.vsix

# 安装依赖
echo "📦 安装依赖..."
npm install

# 运行Lint检查
echo "🔍 运行代码检查..."
npm run lint

# 编译TypeScript
echo "🔧 编译TypeScript..."
npm run compile

# 运行测试
echo "🧪 运行测试..."
npm test

# 打包插件
echo "📦 打包插件..."
vsce package

# 验证包
echo "✅ 验证插件包..."
vsce ls

echo "🎉 VSCode插件构建完成！"

# 显示构建结果
ls -la *.vsix
```

### CI/CD配置

```yaml
# .github/workflows/vscode-extension.yml
name: VSCode Extension CI

on:
  push:
    paths:
      - 'vscode-chtl-extension/**'
  pull_request:
    paths:
      - 'vscode-chtl-extension/**'

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout
      uses: actions/checkout@v3
    
    - name: Setup Node.js
      uses: actions/setup-node@v3
      with:
        node-version: '18'
        cache: 'npm'
        cache-dependency-path: vscode-chtl-extension/package-lock.json
    
    - name: Install dependencies
      run: |
        cd vscode-chtl-extension
        npm ci
    
    - name: Lint
      run: |
        cd vscode-chtl-extension
        npm run lint
    
    - name: Compile
      run: |
        cd vscode-chtl-extension
        npm run compile
    
    - name: Test
      run: |
        cd vscode-chtl-extension
        xvfb-run -a npm test
    
    - name: Package
      run: |
        cd vscode-chtl-extension
        npx vsce package
    
    - name: Upload artifact
      uses: actions/upload-artifact@v3
      with:
        name: chtl-vscode-extension
        path: vscode-chtl-extension/*.vsix
```

---

## 插件发布

### 发布准备

```bash
# 1. 更新版本号
npm version patch  # 或 minor, major

# 2. 更新CHANGELOG
echo "## [1.0.1] - $(date +%Y-%m-%d)" >> CHANGELOG.md
echo "### Fixed" >> CHANGELOG.md
echo "- 修复的问题列表" >> CHANGELOG.md

# 3. 提交版本更新
git add .
git commit -m "chore: release v1.0.1"
git tag v1.0.1
```

### 发布到市场

```bash
# 发布到VSCode市场
vsce publish

# 发布到Open VSX
npx ovsx publish chtl-1.0.0.vsix

# 发布到GitHub Releases
gh release create v1.0.1 chtl-1.0.0.vsix --title "CHTL VSCode Extension v1.0.1"
```

### 发布验证

```bash
# 验证发布成功
vsce show chtl-dev.chtl

# 下载并测试发布的版本
code --install-extension chtl-dev.chtl-1.0.0.vsix
```

---

## 插件维护

### 更新管理

```bash
# 更新依赖
npm update

# 检查过时的依赖
npm outdated

# 更新VSCode API
npm install @types/vscode@latest
```

### 兼容性测试

```bash
# 测试不同VSCode版本
npm run test:vscode-1.74
npm run test:vscode-1.75
npm run test:vscode-latest
```

---

## 性能优化

### 插件性能优化

```typescript
// 延迟加载
export async function activate(context: vscode.ExtensionContext) {
    // 只注册必要的功能
    registerBasicFeatures(context);
    
    // 延迟加载重型功能
    setTimeout(() => {
        registerAdvancedFeatures(context);
    }, 1000);
}

// 缓存优化
class CacheManager {
    private static cache = new Map<string, any>();
    
    static get<T>(key: string): T | undefined {
        return this.cache.get(key);
    }
    
    static set<T>(key: string, value: T): void {
        this.cache.set(key, value);
        
        // 限制缓存大小
        if (this.cache.size > 1000) {
            const firstKey = this.cache.keys().next().value;
            this.cache.delete(firstKey);
        }
    }
}
```

### 内存管理

```typescript
// 资源清理
export function deactivate(): Thenable<void> | undefined {
    // 清理事件监听器
    disposables.forEach(disposable => disposable.dispose());
    
    // 清理缓存
    CacheManager.clear();
    
    // 清理Web视图
    webviewPanels.forEach(panel => panel.dispose());
    
    return undefined;
}
```

---

## 调试技巧

### 插件调试

```typescript
// 启用调试输出
const outputChannel = vscode.window.createOutputChannel('CHTL');

function debugLog(message: string) {
    if (vscode.workspace.getConfiguration('chtl').get('debug')) {
        outputChannel.appendLine(`[${new Date().toISOString()}] ${message}`);
    }
}

// 性能监控
class PerformanceMonitor {
    private static timers = new Map<string, number>();
    
    static start(operation: string) {
        this.timers.set(operation, Date.now());
    }
    
    static end(operation: string) {
        const start = this.timers.get(operation);
        if (start) {
            const duration = Date.now() - start;
            debugLog(`${operation} 耗时: ${duration}ms`);
            this.timers.delete(operation);
        }
    }
}
```

---

## 最佳实践

### 代码组织

1. **模块化**: 功能分离到不同模块
2. **类型安全**: 充分利用TypeScript类型系统
3. **异步处理**: 正确使用Promise和async/await
4. **错误处理**: 完善的错误处理和用户反馈

### 用户体验

1. **响应性**: 保持界面响应，避免阻塞
2. **一致性**: 与VSCode界面风格保持一致
3. **可发现性**: 功能易于发现和使用
4. **反馈**: 提供及时的操作反馈

### 性能考虑

1. **懒加载**: 按需加载功能模块
2. **缓存**: 缓存计算结果和网络请求
3. **防抖**: 避免频繁的操作触发
4. **内存**: 及时清理不需要的资源

---

## 故障排除

### 常见构建问题

#### TypeScript编译错误

```bash
# 错误: TypeScript compilation failed
# 解决: 检查TypeScript版本和配置
npm list typescript
npm run compile -- --listFiles
```

#### 依赖版本冲突

```bash
# 错误: Dependency version conflicts
# 解决: 清理并重新安装依赖
rm -rf node_modules package-lock.json
npm install
```

#### VSCode API不兼容

```bash
# 错误: VSCode API compatibility issues
# 解决: 更新@types/vscode版本
npm install @types/vscode@latest
```

### 运行时问题

#### 插件激活失败

```typescript
// 添加激活事件日志
export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL插件开始激活');
    
    try {
        // 插件初始化代码
        initializeExtension(context);
        console.log('CHTL插件激活成功');
    } catch (error) {
        console.error('CHTL插件激活失败:', error);
        vscode.window.showErrorMessage(`CHTL插件激活失败: ${error.message}`);
    }
}
```

#### 功能不工作

```typescript
// 添加功能调试
function debugFeature(featureName: string, operation: () => any) {
    try {
        debugLog(`开始执行功能: ${featureName}`);
        const result = operation();
        debugLog(`功能执行成功: ${featureName}`);
        return result;
    } catch (error) {
        debugLog(`功能执行失败: ${featureName}, 错误: ${error.message}`);
        throw error;
    }
}
```

---

## 版本发布流程

### 发布检查清单

- [ ] 代码编译无错误
- [ ] 所有测试通过
- [ ] Lint检查通过
- [ ] 版本号已更新
- [ ] CHANGELOG已更新
- [ ] 文档已更新
- [ ] 功能测试完成
- [ ] 性能测试通过

### 自动化发布

```bash
#!/bin/bash
# scripts/release_vscode_extension.sh

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "❌ 请指定版本号: ./release_vscode_extension.sh 1.0.1"
    exit 1
fi

echo "🚀 发布CHTL VSCode插件 v$VERSION"

cd vscode-chtl-extension

# 更新版本
npm version $VERSION --no-git-tag-version

# 构建和测试
npm run vscode:prepublish
npm test

# 打包
vsce package

# 发布
vsce publish

# 创建Git标签
git add package.json
git commit -m "chore: release vscode extension v$VERSION"
git tag "vscode-v$VERSION"
git push origin "vscode-v$VERSION"

echo "✅ VSCode插件 v$VERSION 发布完成"
```

---

通过本指南，开发者可以完整地构建、测试和发布CHTL VSCode插件，为CHTL用户提供优秀的开发体验。插件的模块化架构确保了其可维护性和可扩展性。