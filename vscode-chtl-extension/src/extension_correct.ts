import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';

/**
 * CHTL VSCode扩展 - 严格按照最新语法文档实现
 */

export function activate(context: vscode.ExtensionContext) {
    console.log('🎉 CHTL IDE扩展已激活 - 严格语法版');
    
    // ========================================
    // 1. 智能代码补全 - 基于最新语法文档
    // ========================================
    
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        {
            provideCompletionItems(document, position) {
                const completions: vscode.CompletionItem[] = [];
                
                const line = document.lineAt(position).text;
                const beforeCursor = line.substring(0, position.character);
                
                // 根据上下文提供正确的补全
                if (beforeCursor.includes('[Import]')) {
                    completions.push(...getCorrectImportCompletions());
                } else if (beforeCursor.includes('inherit')) {
                    completions.push(...getCorrectInheritCompletions());
                } else if (beforeCursor.includes('delete')) {
                    completions.push(...getCorrectDeleteCompletions());
                } else if (beforeCursor.includes('except')) {
                    completions.push(...getCorrectExceptCompletions());
                } else if (isInScriptBlock(document, position)) {
                    completions.push(...getCHTLJSCompletions());
                } else {
                    completions.push(...getGeneralCompletions());
                }
                
                return completions;
            }
        },
        '{', '[', '@', '.', '#', ','
    );
    
    // ========================================
    // 2. 语法验证 - 基于最新文档
    // ========================================
    
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    
    const validateDocument = (document: vscode.TextDocument) => {
        if (document.languageId !== 'chtl') return;
        
        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        
        // 验证变量组语法
        validateVariableGroupSyntax(text, diagnostics, document);
        
        // 验证继承语法
        validateInheritSyntax(text, diagnostics, document);
        
        // 验证约束语法
        validateConstraintSyntax(text, diagnostics, document);
        
        // 验证链式语法
        validateChainSyntax(text, diagnostics, document);
        
        diagnosticCollection.set(document.uri, diagnostics);
    };
    
    // ========================================
    // 3. 模块管理 - 正确的路径机制
    // ========================================
    
    const scanModulesCommand = vscode.commands.registerCommand('chtl.scanModules', async () => {
        const modules = await scanCorrectModulePaths();
        
        const panel = vscode.window.createWebviewPanel(
            'chtlModules',
            'CHTL模块管理',
            vscode.ViewColumn.One,
            { enableScripts: true }
        );
        
        panel.webview.html = createModuleManagementHTML(modules);
    });
    
    // ========================================
    // 4. 代码片段 - 正确的语法模板
    // ========================================
    
    const snippetProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        {
            provideCompletionItems() {
                return getCorrectSyntaxSnippets();
            }
        }
    );
    
    // ========================================
    // 5. 悬停提示 - 语法帮助
    // ========================================
    
    const hoverProvider = vscode.languages.registerHoverProvider('chtl', {
        provideHover(document, position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range) return;
            
            const word = document.getText(range);
            return getHoverHelp(word);
        }
    });
    
    // 注册所有功能
    context.subscriptions.push(
        completionProvider,
        snippetProvider,
        hoverProvider,
        scanModulesCommand,
        diagnosticCollection
    );
    
    // 文档变化时验证
    vscode.workspace.onDidChangeTextDocument(event => {
        validateDocument(event.document);
    });
    
    vscode.workspace.onDidOpenTextDocument(validateDocument);
}

// ========================================
// 正确的补全函数
// ========================================

function getCorrectImportCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // 基础导入类型
    const importTypes = [
        { type: '@Html', desc: 'HTML文件导入', example: '[Import] @Html from header as HeaderTemplate' },
        { type: '@Style', desc: 'CSS文件导入', example: '[Import] @Style from theme as ThemeStyle' },
        { type: '@JavaScript', desc: 'JS文件导入', example: '[Import] @JavaScript from utils as Utils' },
        { type: '@Chtl', desc: 'CHTL模块导入', example: '[Import] @Chtl from ModuleName' },
        { type: '@CJmod', desc: 'CJMOD扩展导入', example: '[Import] @CJmod from ExtensionName' }
    ];
    
    importTypes.forEach(imp => {
        const item = new vscode.CompletionItem(imp.type, vscode.CompletionItemKind.Keyword);
        item.detail = imp.desc;
        item.documentation = new vscode.MarkdownString(`**${imp.desc}**\n\n示例:\n\`\`\`chtl\n${imp.example}\n\`\`\``);
        completions.push(item);
    });
    
    // 特定导入类型
    const specificImports = [
        '[Custom] @Element',
        '[Custom] @Style', 
        '[Custom] @Var',
        '[Template] @Element',
        '[Template] @Style',
        '[Template] @Var',
        '[Origin] @Html',
        '[Origin] @Style',
        '[Origin] @JavaScript',
        '[Configuration] @Config'
    ];
    
    specificImports.forEach(imp => {
        const item = new vscode.CompletionItem(imp, vscode.CompletionItemKind.Keyword);
        item.detail = `特定${imp}导入`;
        completions.push(item);
    });
    
    return completions;
}

function getCorrectInheritCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // 正确的inherit语法
    const inheritItem = new vscode.CompletionItem('inherit @Style', vscode.CompletionItemKind.Keyword);
    inheritItem.insertText = new vscode.SnippetString('inherit @Style ${1:StyleName};');
    inheritItem.detail = '继承样式组';
    inheritItem.documentation = new vscode.MarkdownString(
        '**inherit语法**\n\n' +
        '正确语法:\n' +
        '```chtl\n' +
        'inherit @Style ThemeColor;\n' +
        '```\n\n' +
        '注意：inherit后面跟的是 @Style StyleName，不是函数调用'
    );
    completions.push(inheritItem);
    
    return completions;
}

function getCorrectDeleteCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // delete链式语法
    const deleteItem = new vscode.CompletionItem('delete (链式)', vscode.CompletionItemKind.Keyword);
    deleteItem.insertText = new vscode.SnippetString('delete ${1:property}${2:, ${3:property2}};');
    deleteItem.detail = '删除属性（支持链式）';
    deleteItem.documentation = new vscode.MarkdownString(
        '**delete链式语法**\n\n' +
        '```chtl\n' +
        'delete line-height, border;\n' +
        'delete .class1, .class2;\n' +
        '```'
    );
    completions.push(deleteItem);
    
    return completions;
}

function getCorrectExceptCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // except链式语法 - 只能约束特定类型
    const exceptItem = new vscode.CompletionItem('except (链式)', vscode.CompletionItemKind.Keyword);
    exceptItem.insertText = new vscode.SnippetString('except ${1|@Html,[Custom],[Template]|}${2:, ${3|@Html,[Custom],[Template]|}};');
    exceptItem.detail = '约束语法（支持链式）';
    exceptItem.documentation = new vscode.MarkdownString(
        '**except链式语法**\n\n' +
        '只能约束以下三种类型：\n' +
        '- `@Html` - HTML元素\n' +
        '- `[Custom]` - 自定义类型\n' +
        '- `[Template]` - 模板类型\n\n' +
        '```chtl\n' +
        'except span, [Custom] @Element Box;\n' +
        'except @Html, [Template];\n' +
        '```'
    );
    completions.push(exceptItem);
    
    return completions;
}

function getCHTLJSCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // CHTL JS增强选择器
    const selectorItem = new vscode.CompletionItem('{{selector}}', vscode.CompletionItemKind.Function);
    selectorItem.insertText = new vscode.SnippetString('{{${1:selector}}}');
    selectorItem.detail = 'CHTL JS增强选择器';
    selectorItem.documentation = new vscode.MarkdownString(
        '**CHTL JS增强选择器**\n\n' +
        '```chtl\n' +
        '{{.class}}     // 类选择器\n' +
        '{{#id}}        // ID选择器\n' +
        '{{tag}}        // 标签选择器\n' +
        '{{tag[0]}}     // 索引选择器\n' +
        '{{&}}          // 当前元素\n' +
        '```'
    );
    completions.push(selectorItem);
    
    // 箭头操作符
    const arrowItem = new vscode.CompletionItem('->', vscode.CompletionItemKind.Operator);
    arrowItem.detail = '箭头操作符（等价于.）';
    arrowItem.documentation = new vscode.MarkdownString(
        '**箭头操作符**\n\n' +
        '-> 与 . 完全等价，用于明确使用CHTL JS语法\n' +
        '```chtl\n' +
        '{{element}}->method();\n' +
        '{{.button}}->listen({ click: handler });\n' +
        '```'
    );
    completions.push(arrowItem);
    
    return completions;
}

function getGeneralCompletions(): vscode.CompletionItem[] {
    const completions: vscode.CompletionItem[] = [];
    
    // 正确的段落类型
    const sections = [
        { name: '[Info]', desc: '模块信息段', template: '[Info] {\n    name = "${1:ModuleName}";\n    version = "${2:1.0.0}";\n}' },
        { name: '[Export]', desc: '导出信息段', template: '[Export] {\n    [${1|Custom,Template,Origin|}] @${2|Element,Style,Var|} ${3:Name};\n}' },
        { name: '[Import]', desc: '导入段', template: '[Import] @${1|Chtl,CJmod,Html,Style,JavaScript|} from ${2:name}' },
        { name: '[Namespace]', desc: '命名空间段', template: '[Namespace] ${1:name} {\n    $0\n}' },
        { name: '[Template]', desc: '模板段', template: '[Template] @${1|Element,Style,Var|} ${2:Name} {\n    $0\n}' },
        { name: '[Custom]', desc: '自定义段', template: '[Custom] @${1|Element,Style,Var|} ${2:Name} {\n    $0\n}' },
        { name: '[Origin]', desc: '原始嵌入段', template: '[Origin] @${1|Html,Style,JavaScript|} ${2:Name} {\n    $0\n}' },
        { name: '[Configuration]', desc: '配置段', template: '[Configuration] @Config ${1:Name} {\n    $0\n}' }
    ];
    
    sections.forEach(section => {
        const item = new vscode.CompletionItem(section.name, vscode.CompletionItemKind.Keyword);
        item.detail = section.desc;
        item.insertText = new vscode.SnippetString(section.template);
        completions.push(item);
    });
    
    return completions;
}

function getCorrectSyntaxSnippets(): vscode.CompletionItem[] {
    const snippets: vscode.CompletionItem[] = [];
    
    // 正确的变量组模板
    const varTemplateSnippet = new vscode.CompletionItem('变量组模板', vscode.CompletionItemKind.Snippet);
    varTemplateSnippet.insertText = new vscode.SnippetString(
        '[Template] @Var ${1:VariableGroupName} {\n' +
        '    ${2:propertyName}: "${3:value}";\n' +
        '    ${4:anotherProperty}: "${5:anotherValue}";\n' +
        '}'
    );
    varTemplateSnippet.detail = '变量组模板定义';
    snippets.push(varTemplateSnippet);
    
    // 正确的自定义变量组
    const customVarSnippet = new vscode.CompletionItem('自定义变量组', vscode.CompletionItemKind.Snippet);
    customVarSnippet.insertText = new vscode.SnippetString(
        '[Custom] @Var ${1:VariableGroupName} {\n' +
        '    ${2:propertyName}: "${3:value}";\n' +
        '    ${4:anotherProperty}: "${5:anotherValue}";\n' +
        '}'
    );
    customVarSnippet.detail = '自定义变量组定义';
    snippets.push(customVarSnippet);
    
    // 正确的样式组继承
    const inheritSnippet = new vscode.CompletionItem('样式组继承', vscode.CompletionItemKind.Snippet);
    inheritSnippet.insertText = new vscode.SnippetString('inherit @Style ${1:StyleName};');
    inheritSnippet.detail = '继承样式组';
    snippets.push(inheritSnippet);
    
    // 变量组使用
    const varUsageSnippet = new vscode.CompletionItem('变量组使用', vscode.CompletionItemKind.Snippet);
    varUsageSnippet.insertText = new vscode.SnippetString('${1:VariableGroup}(${2:property})');
    varUsageSnippet.detail = '使用变量组';
    varUsageSnippet.documentation = new vscode.MarkdownString(
        '**变量组使用语法**\n\n' +
        '```chtl\n' +
        'color: ThemeColor(tableColor);\n' +
        'background: ThemeColor(tableColor = rgb(145, 155, 200));\n' +
        '```'
    );
    snippets.push(varUsageSnippet);
    
    // CMOD组件模板
    const cmodSnippet = new vscode.CompletionItem('CMOD组件模板', vscode.CompletionItemKind.Snippet);
    cmodSnippet.insertText = new vscode.SnippetString(
        '[Custom] @Element ${1:ComponentName} {\n' +
        '    ${2:div} {\n' +
        '        class: ${3:component-class};\n' +
        '        \n' +
        '        style {\n' +
        '            .${3:component-class} {\n' +
        '                ${4:color}: ${5:ThemeColor(primaryColor)};\n' +
        '            }\n' +
        '        }\n' +
        '        \n' +
        '        script {\n' +
        '            {{&}}->listen({\n' +
        '                ${6:click}: function(e) {\n' +
        '                    ${7:// 事件处理}\n' +
        '                }\n' +
        '            });\n' +
        '        }\n' +
        '        \n' +
        '        text { ${8:组件内容} }\n' +
        '    }\n' +
        '}'
    );
    cmodSnippet.detail = 'CMOD组件模板（正确语法）';
    snippets.push(cmodSnippet);
    
    return snippets;
}

// ========================================
// 语法验证函数
// ========================================

function validateVariableGroupSyntax(text: string, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
    const lines = text.split('\n');
    
    lines.forEach((line, lineIndex) => {
        // 检查错误的变量组定义
        const wrongVarMatch = line.match(/\[Var\]\s+\w+/);
        if (wrongVarMatch) {
            const range = new vscode.Range(
                lineIndex, wrongVarMatch.index || 0,
                lineIndex, (wrongVarMatch.index || 0) + wrongVarMatch[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                '变量组定义应使用 [Template] @Var 或 [Custom] @Var 语法',
                vscode.DiagnosticSeverity.Error
            ));
        }
        
        // 检查变量组使用中的等号语法错误
        const wrongVarUsage = line.match(/(\w+)\s*=\s*[^";]+;/);
        if (wrongVarUsage && line.includes('[') && line.includes('@Var')) {
            const range = new vscode.Range(
                lineIndex, wrongVarUsage.index || 0,
                lineIndex, (wrongVarUsage.index || 0) + wrongVarUsage[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                '变量组定义中应使用冒号(:)而不是等号(=)',
                vscode.DiagnosticSeverity.Error
            ));
        }
    });
}

function validateInheritSyntax(text: string, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
    const lines = text.split('\n');
    
    lines.forEach((line, lineIndex) => {
        // 检查错误的inherit语法（函数调用形式）
        const wrongInherit = line.match(/inherit\s+\w+\([^)]+\)/);
        if (wrongInherit) {
            const range = new vscode.Range(
                lineIndex, wrongInherit.index || 0,
                lineIndex, (wrongInherit.index || 0) + wrongInherit[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                'inherit语法应为 "inherit @Style StyleName;" 而不是函数调用形式',
                vscode.DiagnosticSeverity.Error
            ));
        }
    });
}

function validateConstraintSyntax(text: string, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
    const lines = text.split('\n');
    
    lines.forEach((line, lineIndex) => {
        // 检查错误的约束类型
        const wrongExcept = line.match(/except\s+\[Origin\]/);
        if (wrongExcept) {
            const range = new vscode.Range(
                lineIndex, wrongExcept.index || 0,
                lineIndex, (wrongExcept.index || 0) + wrongExcept[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                'except只能约束 @Html, [Custom], [Template] 这三种类型',
                vscode.DiagnosticSeverity.Error
            ));
        }
        
        // 检查不存在的[Constraint]语法
        const wrongConstraint = line.match(/\[Constraint\]/);
        if (wrongConstraint) {
            const range = new vscode.Range(
                lineIndex, wrongConstraint.index || 0,
                lineIndex, (wrongConstraint.index || 0) + wrongConstraint[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                'CHTL中没有[Constraint]语法，应直接在元素中使用except',
                vscode.DiagnosticSeverity.Error
            ));
        }
    });
}

function validateChainSyntax(text: string, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument) {
    const lines = text.split('\n');
    
    lines.forEach((line, lineIndex) => {
        // 检查insert的错误链式使用
        const wrongInsertChain = line.match(/insert\s+[^{]+,\s*[^{]+/);
        if (wrongInsertChain) {
            const range = new vscode.Range(
                lineIndex, wrongInsertChain.index || 0,
                lineIndex, (wrongInsertChain.index || 0) + wrongInsertChain[0].length
            );
            
            diagnostics.push(new vscode.Diagnostic(
                range,
                'insert操作不支持链式语法，每次只能插入一个位置',
                vscode.DiagnosticSeverity.Error
            ));
        }
    });
}

// ========================================
// 模块扫描 - 正确的路径机制
// ========================================

async function scanCorrectModulePaths(): Promise<ModuleInfo[]> {
    const modules: ModuleInfo[] = [];
    
    // 只扫描运行时可访问的目录，不包括src/Module
    
    // 1. 官方模块目录（编译器所在目录）
    // TODO: 检测编译器路径并扫描官方模块
    
    // 2. 用户模块目录（当前工作区）
    if (vscode.workspace.rootPath) {
        const userModuleDir = path.join(vscode.workspace.rootPath, 'module');
        if (fs.existsSync(userModuleDir)) {
            scanModuleDirectory(userModuleDir, 'user', modules);
        }
    }
    
    return modules;
}

function scanModuleDirectory(baseDir: string, category: string, modules: ModuleInfo[]) {
    // 支持的格式变体
    const variants = ['CMOD', 'Cmod', 'cmod', 'CJMOD', 'CJmod', 'cjmod'];
    
    variants.forEach(variant => {
        const variantDir = path.join(baseDir, variant);
        if (fs.existsSync(variantDir)) {
            try {
                const items = fs.readdirSync(variantDir);
                items.forEach(item => {
                    const itemPath = path.join(variantDir, item);
                    const stat = fs.statSync(itemPath);
                    
                    if (stat.isFile() && (item.endsWith('.cmod') || item.endsWith('.cjmod'))) {
                        // 打包的模块文件
                        modules.push({
                            name: path.basename(item, path.extname(item)),
                            type: variant.toUpperCase().includes('CMOD') ? 'CMOD' : 'CJMOD',
                            path: itemPath,
                            category: category,
                            version: '1.0.0',
                            description: `${variant}格式模块`,
                            format: variant
                        });
                    } else if (stat.isDirectory()) {
                        // 未打包的模块目录
                        const infoFile = path.join(itemPath, 'info', `${item}.chtl`);
                        if (fs.existsSync(infoFile)) {
                            modules.push({
                                name: item,
                                type: variant.toUpperCase().includes('CMOD') ? 'CMOD' : 'CJMOD',
                                path: itemPath,
                                category: category,
                                version: '1.0.0',
                                description: `${variant}格式模块源码`,
                                format: variant
                            });
                        }
                    }
                });
            } catch (error) {
                console.error(`扫描目录失败 ${variantDir}:`, error);
            }
        }
    });
}

function createModuleManagementHTML(modules: ModuleInfo[]): string {
    return `
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <title>CHTL模块管理</title>
            <style>
                body { 
                    font-family: 'Segoe UI', sans-serif; 
                    padding: 20px; 
                    background: #f6f8fa;
                }
                .header {
                    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                    color: white;
                    padding: 20px;
                    border-radius: 8px;
                    margin-bottom: 20px;
                    text-align: center;
                }
                .module-grid {
                    display: grid;
                    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                    gap: 16px;
                    margin-bottom: 20px;
                }
                .module-card { 
                    border: 1px solid #e1e4e8; 
                    border-radius: 8px; 
                    padding: 16px; 
                    background: white;
                    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                }
                .module-type { 
                    display: inline-block; 
                    padding: 4px 8px; 
                    border-radius: 4px; 
                    font-size: 12px; 
                    font-weight: bold;
                    margin-right: 8px;
                }
                .cmod { background: #e3f2fd; color: #1565c0; }
                .cjmod { background: #f3e5f5; color: #7b1fa2; }
                .official { background: #e8f5e8; color: #2e7d32; }
                .user { background: #fff3e0; color: #ef6c00; }
                .format-variant {
                    background: #f5f5f5;
                    color: #666;
                    font-size: 10px;
                    padding: 2px 6px;
                    border-radius: 3px;
                }
                .path-info {
                    background: #f8f9fa;
                    border: 1px solid #e9ecef;
                    border-radius: 4px;
                    padding: 12px;
                    margin: 16px 0;
                    font-family: monospace;
                    font-size: 12px;
                }
                .warning {
                    background: #fff3cd;
                    border: 1px solid #ffeaa7;
                    color: #856404;
                    padding: 12px;
                    border-radius: 4px;
                    margin: 16px 0;
                }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>🗂️ CHTL模块系统</h1>
                <p>发现 ${modules.length} 个可用模块</p>
            </div>
            
            <div class="warning">
                <strong>⚠️ 重要说明：</strong>
                源码目录 (<code>src/Module/</code>) 仅用于开发时分类，不参与运行时搜索。
                只有官方模块目录和用户模块目录中的模块才能被导入使用。
            </div>
            
            <h2>📦 可用模块列表</h2>
            <div class="module-grid">
                ${modules.map(module => `
                    <div class="module-card">
                        <h3>${module.name}</h3>
                        <div>
                            <span class="module-type ${module.type.toLowerCase()}">${module.type}</span>
                            <span class="module-type ${module.category}">${module.category}</span>
                            <span class="format-variant">${module.format}</span>
                        </div>
                        <p><strong>描述:</strong> ${module.description}</p>
                        <p><strong>版本:</strong> ${module.version}</p>
                        <div class="path-info">${module.path}</div>
                    </div>
                `).join('')}
            </div>
            
            <h2>📋 模块路径机制（修正版）</h2>
            <div class="path-info">
                <h3>🏛️ 官方模块目录（最高优先级）</h3>
                <code>&lt;编译器目录&gt;/module/</code>
                <p>包含预编译的官方模块，全局可用</p>
                
                <h3>👤 用户模块目录（中等优先级）</h3>
                <code>&lt;项目目录&gt;/module/</code>
                <p>项目特定模块，本地可用</p>
                
                <h3>🔧 源码模块目录（仅用于开发分类）</h3>
                <code>&lt;项目根目录&gt;/src/Module/</code>
                <p><strong>重要：此目录不参与运行时搜索，仅用于开发时分类和组织</strong></p>
            </div>
            
            <h2>🎨 支持的格式变体</h2>
            <div class="path-info">
                <p><strong>CMOD:</strong> CMOD/Cmod/cmod （完全兼容）</p>
                <p><strong>CJMOD:</strong> CJMOD/CJmod/cjmod （完全兼容）</p>
                <p>搜索优先级：大写 → 首字母大写 → 小写</p>
            </div>
        </body>
        </html>
    `;
}

function getHoverHelp(word: string): vscode.Hover | undefined {
    const helpTexts: { [key: string]: string } = {
        'inherit': '**inherit语法**\n\n正确用法: `inherit @Style StyleName;`\n\n用于继承样式组的所有属性',
        'delete': '**delete语法**\n\n支持链式: `delete property1, property2;`\n\n用于删除属性或选择器',
        'except': '**except语法**\n\n支持链式: `except @Html, [Custom];`\n\n只能约束 @Html, [Custom], [Template]',
        'ThemeColor': '**变量组使用**\n\n语法: `ThemeColor(propertyName)`\n\n可以特例化: `ThemeColor(property = value)`'
    };
    
    if (helpTexts[word]) {
        return new vscode.Hover(new vscode.MarkdownString(helpTexts[word]));
    }
    
    return undefined;
}

function isInScriptBlock(document: vscode.TextDocument, position: vscode.Position): boolean {
    for (let i = position.line; i >= 0; i--) {
        const line = document.lineAt(i).text.trim();
        if (line.includes('script {')) {
            return true;
        }
        if (line.includes('}') && i < position.line) {
            return false;
        }
    }
    return false;
}

interface ModuleInfo {
    name: string;
    type: 'CMOD' | 'CJMOD';
    path: string;
    category: string;
    version: string;
    description: string;
    format: string;
}

export function deactivate() {
    console.log('👋 CHTL IDE扩展已停用');
}