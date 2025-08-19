# VSCode插件开发指南

## 概述

CHTL VSCode插件为开发者提供完整的CHTL开发体验，包括语法高亮、智能补全、实时错误检测、模块管理等功能。本指南介绍如何开发和扩展CHTL VSCode插件。

---

## 插件架构

### 核心组件

```
vscode-chtl-extension/
├── src/
│   ├── extension.ts                # 主扩展入口
│   ├── completionProvider.ts       # 代码补全提供器
│   ├── diagnosticProvider.ts       # 错误诊断提供器
│   ├── hoverProvider.ts            # 悬停提示提供器
│   ├── definitionProvider.ts       # 定义跳转提供器
│   ├── formattingProvider.ts       # 代码格式化提供器
│   ├── moduleManager.ts            # 模块管理器
│   ├── compilerIntegration.ts      # 编译器集成
│   └── utils/
│       ├── syntaxValidator.ts       # 语法验证器
│       ├── moduleScanner.ts         # 模块扫描器
│       └── pathResolver.ts          # 路径解析器
├── syntaxes/
│   └── chtl.tmLanguage.json        # 语法高亮定义
├── snippets/
│   └── chtl.json                   # 代码片段定义
└── package.json                    # 插件配置
```

---

## 插件开发环境

### 环境设置

```bash
# 安装Node.js和npm
npm install -g @vscode/vsce typescript

# 进入插件目录
cd vscode-chtl-extension

# 安装依赖
npm install

# 编译TypeScript
npm run compile

# 打包插件
vsce package
```

### 开发依赖

```json
{
  "devDependencies": {
    "@types/vscode": "^1.74.0",
    "@types/node": "^18.0.0",
    "typescript": "^4.9.0",
    "@vscode/vsce": "^2.15.0",
    "esbuild": "^0.17.0"
  }
}
```

---

## 核心功能开发

### 1. 智能代码补全

```typescript
// completionProvider.ts
import * as vscode from 'vscode';

export class CHTLCompletionProvider implements vscode.CompletionItemProvider {
    
    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position
    ): vscode.CompletionItem[] {
        const completions: vscode.CompletionItem[] = [];
        
        // 获取当前上下文
        const context = this.detectContext(document, position);
        
        switch (context.type) {
            case 'import':
                completions.push(...this.getImportCompletions());
                break;
            case 'section':
                completions.push(...this.getSectionCompletions());
                break;
            case 'chtljs':
                completions.push(...this.getCHTLJSCompletions());
                break;
            case 'module':
                completions.push(...this.getModuleCompletions());
                break;
            default:
                completions.push(...this.getGeneralCompletions());
        }
        
        return completions;
    }
    
    private detectContext(document: vscode.TextDocument, position: vscode.Position): Context {
        const line = document.lineAt(position).text;
        
        if (line.includes('[Import]')) {
            return { type: 'import' };
        } else if (line.match(/\[(Template|Custom|Origin)\]/)) {
            return { type: 'section' };
        } else if (this.isInScriptBlock(document, position)) {
            return { type: 'chtljs' };
        }
        
        return { type: 'general' };
    }
    
    private getImportCompletions(): vscode.CompletionItem[] {
        const imports = [
            { label: '@Chtl', detail: 'CHTL模块导入' },
            { label: '@CJmod', detail: 'CJMOD扩展导入' },
            { label: '@Html', detail: 'HTML文件导入' },
            { label: '@Style', detail: 'CSS文件导入' },
            { label: '@JavaScript', detail: 'JS文件导入' }
        ];
        
        return imports.map(imp => {
            const item = new vscode.CompletionItem(imp.label, vscode.CompletionItemKind.Keyword);
            item.detail = imp.detail;
            item.insertText = new vscode.SnippetString(`${imp.label} from \${1:moduleName}`);
            return item;
        });
    }
    
    private getCHTLJSCompletions(): vscode.CompletionItem[] {
        const chtljsFeatures = [
            {
                label: '{{selector}}',
                detail: 'CHTL JS增强选择器',
                insertText: '{{${1|.class,#id,tag,&|}}}',
                documentation: '增强选择器语法\n\n{{.class}} - 类选择器\n{{#id}} - ID选择器\n{{tag}} - 标签选择器\n{{&}} - 当前元素'
            },
            {
                label: 'listen',
                detail: 'CHTL JS事件监听器',
                insertText: 'listen({\n    ${1|click,mouseenter,mouseleave,focus,blur|}: function(${2:e}) {\n        ${3:// 事件处理}\n    }\n})',
                documentation: 'listen函数支持所有82个JavaScript事件'
            },
            {
                label: 'delegate',
                detail: 'CHTL JS事件委托',
                insertText: 'delegate({\n    target: {{${1:.selector}}},\n    ${2|click,mouseenter,mouseleave|}: function(${3:e}) {\n        ${4:// 委托处理}\n    }\n})',
                documentation: '事件委托系统，支持全局事件管理'
            },
            {
                label: 'vir',
                detail: 'CHTL JS虚对象',
                insertText: 'vir ${1:ObjectName} = ${2|listen,delegate,animate|}({\n    ${3:method}: function(${4:params}) {\n        ${5:// 虚对象方法}\n    }\n});',
                documentation: 'vir虚对象是编译时语法糖，不创建运行时对象'
            }
        ];
        
        return chtljsFeatures.map(feature => {
            const item = new vscode.CompletionItem(feature.label, vscode.CompletionItemKind.Function);
            item.detail = feature.detail;
            item.insertText = new vscode.SnippetString(feature.insertText);
            item.documentation = new vscode.MarkdownString(feature.documentation);
            return item;
        });
    }
}
```

### 2. 实时语法验证

```typescript
// diagnosticProvider.ts
export class CHTLDiagnosticProvider {
    
    validateDocument(document: vscode.TextDocument): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');
        
        lines.forEach((line, lineIndex) => {
            // 验证delete语法
            this.validateDeleteSyntax(line, lineIndex, diagnostics);
            
            // 验证变量组语法
            this.validateVariableGroupSyntax(line, lineIndex, diagnostics);
            
            // 验证继承语法
            this.validateInheritSyntax(line, lineIndex, diagnostics);
            
            // 验证约束语法
            this.validateConstraintSyntax(line, lineIndex, diagnostics);
        });
        
        return diagnostics;
    }
    
    private validateDeleteSyntax(line: string, lineIndex: number, diagnostics: vscode.Diagnostic[]) {
        // 检查错误的CSS选择器delete
        const wrongDeleteMatch = line.match(/delete\s+[.#][\w-]+/);
        if (wrongDeleteMatch) {
            const startPos = line.indexOf(wrongDeleteMatch[0]);
            const range = new vscode.Range(
                lineIndex, startPos,
                lineIndex, startPos + wrongDeleteMatch[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                'delete不支持CSS选择器，只支持：属性名、元素类型、索引元素、继承元素',
                vscode.DiagnosticSeverity.Error
            ));
        }
    }
    
    private validateVariableGroupSyntax(line: string, lineIndex: number, diagnostics: vscode.Diagnostic[]) {
        // 检查错误的[Var]语法
        const wrongVarMatch = line.match(/\[Var\]\s+\w+/);
        if (wrongVarMatch) {
            const startPos = line.indexOf(wrongVarMatch[0]);
            const range = new vscode.Range(
                lineIndex, startPos,
                lineIndex, startPos + wrongVarMatch[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                '变量组定义应使用 [Template] @Var 或 [Custom] @Var 语法',
                vscode.DiagnosticSeverity.Error
            ));
        }
    }
}
```

### 3. 模块管理功能

```typescript
// moduleManager.ts
export class CHTLModuleManager {
    private moduleCache = new Map<string, ModuleInfo[]>();
    
    async scanModules(): Promise<ModuleInfo[]> {
        const modules: ModuleInfo[] = [];
        
        // 扫描用户模块目录（不包括src/Module）
        if (vscode.workspace.rootPath) {
            const userModuleDir = path.join(vscode.workspace.rootPath, 'module');
            if (fs.existsSync(userModuleDir)) {
                await this.scanModuleDirectory(userModuleDir, 'user', modules);
            }
        }
        
        return modules;
    }
    
    private async scanModuleDirectory(baseDir: string, category: string, modules: ModuleInfo[]) {
        // 扫描所有格式变体
        const variants = ['CMOD', 'Cmod', 'cmod', 'CJMOD', 'CJmod', 'cjmod'];
        
        for (const variant of variants) {
            const variantDir = path.join(baseDir, variant);
            if (fs.existsSync(variantDir)) {
                const items = fs.readdirSync(variantDir);
                
                for (const item of items) {
                    const itemPath = path.join(variantDir, item);
                    const stat = fs.statSync(itemPath);
                    
                    if (stat.isDirectory()) {
                        // 未打包的模块目录
                        const moduleInfo = await this.parseModuleDirectory(itemPath, variant, category);
                        if (moduleInfo) {
                            modules.push(moduleInfo);
                        }
                    } else if (item.endsWith('.cmod') || item.endsWith('.cjmod')) {
                        // 打包的模块文件
                        const moduleInfo = await this.parsePackedModule(itemPath, variant, category);
                        if (moduleInfo) {
                            modules.push(moduleInfo);
                        }
                    }
                }
            }
        }
    }
    
    async createModuleWizard(): Promise<void> {
        // 模块类型选择
        const moduleType = await vscode.window.showQuickPick([
            { label: 'CMOD', description: 'CHTL组件模块' },
            { label: 'CJMOD', description: 'C++语法扩展模块' }
        ], { placeHolder: '选择模块类型' });
        
        if (!moduleType) return;
        
        // 模块名称输入
        const moduleName = await vscode.window.showInputBox({
            prompt: '输入模块名称（PascalCase）',
            validateInput: (value) => {
                if (!/^[A-Z][a-zA-Z0-9]*$/.test(value)) {
                    return '模块名必须以大写字母开头，使用PascalCase命名';
                }
                return null;
            }
        });
        
        if (!moduleName) return;
        
        // 格式变体选择
        const format = await vscode.window.showQuickPick([
            { label: moduleType.label, description: '大写格式（推荐）' },
            { label: this.toTitleCase(moduleType.label), description: '首字母大写格式' },
            { label: moduleType.label.toLowerCase(), description: '小写格式' }
        ], { placeHolder: '选择格式变体' });
        
        if (!format) return;
        
        // 创建模块结构
        await this.createModuleStructure(moduleType.label, moduleName, format.label);
        
        vscode.window.showInformationMessage(
            `✅ ${moduleType.label}模块 "${moduleName}" 创建成功！`,
            '打开模块', '查看文档'
        );
    }
}
```

### 4. 编译器集成

```typescript
// compilerIntegration.ts
export class CHTLCompilerIntegration {
    
    async compileFile(filePath: string): Promise<CompilationResult> {
        const compilerPath = this.findCHTLCompiler();
        if (!compilerPath) {
            throw new Error('未找到CHTL编译器');
        }
        
        return new Promise((resolve, reject) => {
            const outputPath = filePath.replace('.chtl', '.html');
            
            exec(`"${compilerPath}" "${filePath}" -o "${outputPath}"`, (error, stdout, stderr) => {
                if (error) {
                    reject(new Error(`编译失败: ${error.message}`));
                } else {
                    resolve({
                        success: true,
                        outputPath: outputPath,
                        stdout: stdout,
                        stderr: stderr
                    });
                }
            });
        });
    }
    
    private findCHTLCompiler(): string | null {
        const possiblePaths = [
            path.join(vscode.workspace.rootPath || '', 'build', 'chtl_compiler'),
            '/usr/local/bin/chtl',
            '/usr/bin/chtl',
            process.env.CHTL_COMPILER_PATH || ''
        ];
        
        return possiblePaths.find(path => fs.existsSync(path)) || null;
    }
    
    async previewFile(document: vscode.TextDocument): Promise<void> {
        const result = await this.compileFile(document.uri.fsPath);
        
        if (result.success) {
            const panel = vscode.window.createWebviewPanel(
                'chtlPreview',
                'CHTL预览',
                vscode.ViewColumn.Beside,
                { enableScripts: true }
            );
            
            const htmlContent = fs.readFileSync(result.outputPath, 'utf-8');
            panel.webview.html = this.createPreviewHTML(htmlContent);
        }
    }
}
```

---

## 语法高亮开发

### 语法定义文件

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
        { "include": "#selectors" },
        { "include": "#chtljs" }
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
                    "end": "\\*/"
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
                        }
                    ]
                },
                {
                    "name": "keyword.operator.arrow.chtl",
                    "match": "->"
                }
            ]
        }
    }
}
```

### 主题适配

```json
// 为不同主题定制颜色
{
    "tokenColors": [
        {
            "scope": "keyword.control.section.chtl",
            "settings": {
                "foreground": "#C586C0",
                "fontStyle": "bold"
            }
        },
        {
            "scope": "support.function.chtljs.chtl",
            "settings": {
                "foreground": "#4EC9B0"
            }
        }
    ]
}
```

---

## 代码片段开发

### 片段定义

```json
// snippets/chtl.json
{
    "CMOD组件模板": {
        "prefix": "cmod-component",
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
    
    "变量组定义": {
        "prefix": "var-group",
        "body": [
            "[${1|Template,Custom|}] @Var ${2:VariableGroupName} {",
            "    ${3:propertyName}: \"${4:value}\";",
            "    ${5:anotherProperty}: \"${6:anotherValue}\";",
            "}"
        ],
        "description": "创建变量组定义"
    },
    
    "CHTL JS监听器": {
        "prefix": "listen",
        "body": [
            "{{${1:.selector}}}->listen({",
            "    ${2|click,mouseenter,mouseleave,focus,blur,input,change|}: function(${3:e}) {",
            "        ${4:// 事件处理逻辑}",
            "    }",
            "});"
        ],
        "description": "创建CHTL JS事件监听器"
    }
}
```

---

## 模块管理界面

### Web界面开发

```typescript
// 创建模块管理Web界面
export function createModuleManagerWebview(modules: ModuleInfo[]): string {
    return `
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <title>CHTL模块管理器</title>
            <style>
                body { 
                    font-family: 'Segoe UI', sans-serif; 
                    margin: 0; 
                    padding: 20px; 
                    background: #f6f8fa;
                }
                .header {
                    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                    color: white;
                    padding: 30px;
                    border-radius: 12px;
                    margin-bottom: 20px;
                    text-align: center;
                }
                .module-grid {
                    display: grid;
                    grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
                    gap: 20px;
                }
                .module-card {
                    background: white;
                    border: 1px solid #e1e4e8;
                    border-radius: 12px;
                    padding: 20px;
                    box-shadow: 0 2px 8px rgba(0,0,0,0.1);
                    transition: transform 0.2s ease;
                }
                .module-card:hover {
                    transform: translateY(-4px);
                }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>🗂️ CHTL模块管理器</h1>
                <p>发现 ${modules.length} 个可用模块</p>
            </div>
            
            <div class="module-grid">
                ${modules.map(module => `
                    <div class="module-card">
                        <h3>${module.name}</h3>
                        <p><strong>类型:</strong> ${module.type}</p>
                        <p><strong>版本:</strong> ${module.version}</p>
                        <p><strong>描述:</strong> ${module.description}</p>
                        <button onclick="useModule('${module.name}')">使用模块</button>
                    </div>
                `).join('')}
            </div>
            
            <script>
                const vscode = acquireVsCodeApi();
                
                function useModule(moduleName) {
                    vscode.postMessage({
                        command: 'useModule',
                        moduleName: moduleName
                    });
                }
            </script>
        </body>
        </html>
    `;
}
```

---

## 插件配置

### package.json配置

```json
{
    "name": "chtl",
    "displayName": "CHTL Language Support",
    "description": "完整的CHTL语言支持 - 智能提示、语法高亮、模块管理",
    "version": "1.0.0",
    "publisher": "chtl-dev",
    "engines": {
        "vscode": "^1.74.0"
    },
    "categories": [
        "Programming Languages",
        "Snippets",
        "Formatters"
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
                "configuration": "./language-configuration.json"
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
                "category": "CHTL"
            },
            {
                "command": "chtl.preview",
                "title": "预览CHTL文件",
                "category": "CHTL"
            },
            {
                "command": "chtl.moduleManager",
                "title": "模块管理器",
                "category": "CHTL"
            }
        ],
        "keybindings": [
            {
                "command": "chtl.compile",
                "key": "ctrl+shift+c",
                "when": "editorLangId == chtl"
            }
        ]
    },
    "scripts": {
        "compile": "tsc -p ./",
        "watch": "tsc -watch -p ./",
        "package": "vsce package"
    }
}
```

---

## 插件测试

### 单元测试

```typescript
// test/extension.test.ts
import * as assert from 'assert';
import * as vscode from 'vscode';
import { CHTLCompletionProvider } from '../src/completionProvider';

suite('CHTL Extension Tests', () => {
    
    test('代码补全测试', async () => {
        const provider = new CHTLCompletionProvider();
        
        // 创建测试文档
        const document = await vscode.workspace.openTextDocument({
            content: '[Import] @',
            language: 'chtl'
        });
        
        const position = new vscode.Position(0, 10);
        const completions = provider.provideCompletionItems(document, position);
        
        assert.ok(completions.length > 0);
        assert.ok(completions.some(item => item.label === '@Chtl'));
    });
    
    test('语法验证测试', () => {
        const diagnosticProvider = new CHTLDiagnosticProvider();
        
        const testCode = 'delete .class-name;'; // 错误语法
        const diagnostics = diagnosticProvider.validateLine(testCode, 0);
        
        assert.ok(diagnostics.length > 0);
        assert.ok(diagnostics[0].message.includes('delete不支持CSS选择器'));
    });
});
```

### 集成测试

```typescript
suite('CHTL Integration Tests', () => {
    
    test('模块扫描测试', async () => {
        const moduleManager = new CHTLModuleManager();
        const modules = await moduleManager.scanModules();
        
        // 验证模块扫描结果
        assert.ok(modules.length >= 0);
        
        // 验证珂朵莉模块
        const chthollyModule = modules.find(m => m.name === 'Chtholly');
        if (chthollyModule) {
            assert.equal(chthollyModule.type, 'CMOD');
        }
    });
    
    test('编译器集成测试', async () => {
        const integration = new CHTLCompilerIntegration();
        
        // 创建测试CHTL文件
        const testFile = path.join(__dirname, 'test.chtl');
        fs.writeFileSync(testFile, `
            div {
                class: test;
                text { 测试内容 }
            }
        `);
        
        try {
            const result = await integration.compileFile(testFile);
            assert.ok(result.success);
            assert.ok(fs.existsSync(result.outputPath));
        } finally {
            // 清理测试文件
            if (fs.existsSync(testFile)) fs.unlinkSync(testFile);
        }
    });
});
```

---

## 插件发布

### 构建和打包

```bash
# 编译TypeScript
npm run compile

# 运行测试
npm test

# 打包插件
vsce package

# 发布到市场
vsce publish
```

### 版本管理

```json
// 版本号管理
{
    "version": "1.0.0",
    "changelog": {
        "1.0.0": [
            "初始发布",
            "完整的CHTL语法支持",
            "智能代码补全",
            "模块管理功能"
        ]
    }
}
```

---

## 插件扩展

### 添加新的语言功能

1. **更新语法高亮**: 修改 `chtl.tmLanguage.json`
2. **添加代码补全**: 扩展 `CompletionProvider`
3. **增加语法验证**: 扩展 `DiagnosticProvider`
4. **更新代码片段**: 修改 `chtl.json`

### 集成新的编译器功能

```typescript
// 集成新的编译器功能
export class NewFeatureIntegration {
    
    async processNewSyntax(document: vscode.TextDocument): Promise<void> {
        // 检测新语法
        const newSyntax = this.detectNewSyntax(document.getText());
        
        if (newSyntax.length > 0) {
            // 提供特殊处理
            this.provideNewSyntaxSupport(newSyntax);
        }
    }
    
    private detectNewSyntax(text: string): string[] {
        // 实现新语法检测逻辑
        return [];
    }
}
```

---

## 性能优化

### 插件性能

1. **延迟加载**: 只在需要时加载功能
2. **缓存机制**: 缓存解析结果和模块信息
3. **增量更新**: 只更新变更的部分
4. **内存管理**: 及时清理不需要的资源

### 响应性优化

```typescript
// 使用Web Worker进行重计算
class BackgroundProcessor {
    private worker: Worker;
    
    constructor() {
        this.worker = new Worker('./worker.js');
    }
    
    async processInBackground<T>(data: any): Promise<T> {
        return new Promise((resolve, reject) => {
            this.worker.postMessage(data);
            this.worker.onmessage = (e) => resolve(e.data);
            this.worker.onerror = (e) => reject(e);
        });
    }
}
```

---

## 调试和诊断

### 插件调试

```typescript
// 启用调试模式
const debugMode = vscode.workspace.getConfiguration('chtl').get('debug', false);

if (debugMode) {
    console.log('CHTL插件调试模式已启用');
}

// 输出诊断信息
function logDiagnostic(message: string) {
    if (debugMode) {
        console.log(`[CHTL Debug] ${message}`);
    }
}
```

### 错误报告

```typescript
// 错误报告机制
class ErrorReporter {
    static reportError(error: Error, context: string) {
        vscode.window.showErrorMessage(
            `CHTL插件错误: ${error.message}`,
            '查看详情', '报告问题'
        ).then(selection => {
            if (selection === '查看详情') {
                console.error(`Context: ${context}`, error);
            } else if (selection === '报告问题') {
                vscode.env.openExternal(vscode.Uri.parse('https://github.com/chtl-lang/chtl/issues'));
            }
        });
    }
}
```

---

## 最佳实践

### 代码组织

1. **模块化**: 功能分离到不同模块
2. **类型安全**: 使用TypeScript类型系统
3. **异步处理**: 使用Promise和async/await
4. **错误处理**: 完善的错误处理机制

### 用户体验

1. **响应性**: 保持界面响应
2. **一致性**: 与VSCode界面风格一致
3. **可访问性**: 支持键盘导航和屏幕阅读器
4. **国际化**: 支持多语言

### 性能考虑

1. **懒加载**: 按需加载功能
2. **缓存**: 缓存计算结果
3. **防抖**: 避免频繁触发
4. **内存**: 及时清理资源

---

## 社区贡献

### 插件扩展开发

鼓励社区开发者贡献插件扩展：

1. **主题扩展**: 为CHTL开发专门的主题
2. **片段扩展**: 贡献有用的代码片段
3. **工具扩展**: 开发辅助开发工具
4. **集成扩展**: 与其他工具集成

### 质量标准

1. **功能完整**: 插件功能完整可用
2. **性能优秀**: 不影响VSCode性能
3. **用户友好**: 提供良好的用户体验
4. **文档完善**: 提供完整的使用文档

---

通过VSCode插件，CHTL为开发者提供了现代化的开发体验。插件的模块化架构和扩展机制确保了其持续发展和社区贡献的便利性。