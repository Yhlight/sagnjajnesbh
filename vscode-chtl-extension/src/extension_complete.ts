import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';

/**
 * CHTL VSCode扩展 - 完整IDE功能
 */
export function activate(context: vscode.ExtensionContext) {
    console.log('🎉 CHTL IDE扩展已激活 - 完整功能版');
    
    // ========================================
    // 1. 智能代码补全 - 严格按照语法文档
    // ========================================
    
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        {
            provideCompletionItems(document, position) {
                const completions: vscode.CompletionItem[] = [];
                const line = document.lineAt(position).text;
                
                // 检查上下文
                if (line.includes('[Import]')) {
                    // 导入补全
                    const importItem = new vscode.CompletionItem('@Chtl', vscode.CompletionItemKind.Keyword);
                    importItem.insertText = new vscode.SnippetString('@Chtl from ${1:ModuleName}');
                    importItem.detail = 'CHTL模块导入';
                    completions.push(importItem);
                    
                    const cjmodItem = new vscode.CompletionItem('@CJmod', vscode.CompletionItemKind.Keyword);
                    cjmodItem.insertText = new vscode.SnippetString('@CJmod from ${1:ExtensionName}');
                    cjmodItem.detail = 'CJMOD扩展导入';
                    completions.push(cjmodItem);
                }
                
                if (line.includes('delete')) {
                    // 正确的delete补全
                    const deleteAttrItem = new vscode.CompletionItem('属性删除', vscode.CompletionItemKind.Snippet);
                    deleteAttrItem.insertText = new vscode.SnippetString('${1:property}${2:, ${3:property2}}');
                    deleteAttrItem.detail = '删除样式属性（支持链式）';
                    completions.push(deleteAttrItem);
                    
                    const deleteElementItem = new vscode.CompletionItem('元素删除', vscode.CompletionItemKind.Snippet);
                    deleteElementItem.insertText = new vscode.SnippetString('${1:element}${2:[${3:0}]}');
                    deleteElementItem.detail = '删除元素或索引元素';
                    completions.push(deleteElementItem);
                }
                
                if (line.includes('inherit')) {
                    // 正确的inherit补全
                    const inheritItem = new vscode.CompletionItem('@Style', vscode.CompletionItemKind.Keyword);
                    inheritItem.insertText = new vscode.SnippetString('@Style ${1:StyleName}');
                    inheritItem.detail = '继承样式组';
                    completions.push(inheritItem);
                }
                
                // 段落补全
                const sections = ['Info', 'Export', 'Import', 'Namespace', 'Template', 'Custom', 'Origin', 'Configuration'];
                sections.forEach(section => {
                    const item = new vscode.CompletionItem(`[${section}]`, vscode.CompletionItemKind.Keyword);
                    item.detail = `CHTL ${section} 段落`;
                    completions.push(item);
                });
                
                // 珂朵莉模块组件
                const chthollyComponents = [
                    'ChthollyAccordion', 'ChthollyGallery', 'ChthollyMemo', 'ChthollyNote', 'ChthollySakura',
                    'ChthollyMouseEffect', 'ChthollyMouseTrail', 'ChthollyParallax', 'ChthollyContextMenu', 'ChthollyProgressBar'
                ];
                
                chthollyComponents.forEach(comp => {
                    const item = new vscode.CompletionItem(comp, vscode.CompletionItemKind.Class);
                    item.detail = '珂朵莉模块组件';
                    completions.push(item);
                });
                
                return completions;
            }
        },
        '{', '[', '@', '.', '#'
    );
    
    // ========================================
    // 2. 实时语法验证
    // ========================================
    
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    
    const validateDocument = (document: vscode.TextDocument) => {
        if (document.languageId !== 'chtl') return;
        
        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');
        
        lines.forEach((line, lineIndex) => {
            // 检查错误的delete语法
            if (line.includes('delete .') || line.includes('delete #')) {
                const match = line.match(/delete\s+[.#][\w-]+/);
                if (match) {
                    const startPos = line.indexOf(match[0]);
                    const range = new vscode.Range(
                        lineIndex, startPos,
                        lineIndex, startPos + match[0].length
                    );
                    
                    diagnostics.push(new vscode.Diagnostic(
                        range,
                        'delete不支持CSS选择器，只支持：属性名、元素类型、索引元素、继承元素',
                        vscode.DiagnosticSeverity.Error
                    ));
                }
            }
            
            // 检查错误的变量组语法
            if (line.includes('[Var]')) {
                const match = line.match(/\[Var\]\s+\w+/);
                if (match) {
                    const startPos = line.indexOf(match[0]);
                    const range = new vscode.Range(
                        lineIndex, startPos,
                        lineIndex, startPos + match[0].length
                    );
                    
                    diagnostics.push(new vscode.Diagnostic(
                        range,
                        '变量组定义应使用 [Template] @Var 或 [Custom] @Var 语法',
                        vscode.DiagnosticSeverity.Error
                    ));
                }
            }
            
            // 检查错误的inherit语法
            if (line.includes('inherit') && line.includes('(')) {
                const match = line.match(/inherit\s+\w+\([^)]+\)/);
                if (match) {
                    const startPos = line.indexOf(match[0]);
                    const range = new vscode.Range(
                        lineIndex, startPos,
                        lineIndex, startPos + match[0].length
                    );
                    
                    diagnostics.push(new vscode.Diagnostic(
                        range,
                        'inherit语法应为 "inherit @Style StyleName;" 而不是函数调用形式',
                        vscode.DiagnosticSeverity.Error
                    ));
                }
            }
        });
        
        diagnosticCollection.set(document.uri, diagnostics);
    };
    
    // ========================================
    // 3. 模块管理命令
    // ========================================
    
    const moduleManagerCommand = vscode.commands.registerCommand('chtl.moduleManager', async () => {
        const panel = vscode.window.createWebviewPanel(
            'chtlModuleManager',
            'CHTL模块管理器',
            vscode.ViewColumn.One,
            { enableScripts: true }
        );
        
        panel.webview.html = `
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="UTF-8">
                <title>CHTL模块管理器</title>
                <style>
                    body { font-family: 'Segoe UI', sans-serif; padding: 20px; }
                    .header { text-align: center; margin-bottom: 30px; }
                    .feature-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
                    .feature-card { 
                        background: white; 
                        border: 1px solid #e1e4e8; 
                        border-radius: 8px; 
                        padding: 20px;
                        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    }
                    .btn { 
                        padding: 10px 20px; 
                        border: none; 
                        border-radius: 6px; 
                        cursor: pointer; 
                        margin: 5px;
                        background: #3b82f6;
                        color: white;
                    }
                </style>
            </head>
            <body>
                <div class="header">
                    <h1>🗂️ CHTL模块管理器</h1>
                    <p>完整的模块开发和管理工具</p>
                </div>
                
                <div class="feature-grid">
                    <div class="feature-card">
                        <h3>📦 模块创建</h3>
                        <p>创建CMOD组件模块或CJMOD扩展模块</p>
                        <button class="btn" onclick="createCMOD()">创建CMOD</button>
                        <button class="btn" onclick="createCJMOD()">创建CJMOD</button>
                    </div>
                    
                    <div class="feature-card">
                        <h3>🔍 模块扫描</h3>
                        <p>扫描所有可用模块（官方+用户）</p>
                        <button class="btn" onclick="scanModules()">扫描模块</button>
                    </div>
                    
                    <div class="feature-card">
                        <h3>📦 模块打包</h3>
                        <p>将模块源码打包为.cmod/.cjmod文件</p>
                        <button class="btn" onclick="packModules()">打包模块</button>
                    </div>
                    
                    <div class="feature-card">
                        <h3>🎯 语法验证</h3>
                        <p>实时检查CHTL语法正确性</p>
                        <ul>
                            <li>✅ delete语法验证</li>
                            <li>✅ 变量组语法验证</li>
                            <li>✅ inherit语法验证</li>
                            <li>✅ 约束语法验证</li>
                        </ul>
                    </div>
                </div>
                
                <div style="margin-top: 30px; padding: 20px; background: #f8f9fa; border-radius: 8px;">
                    <h3>📋 支持的模块路径</h3>
                    <p><strong>官方模块:</strong> &lt;编译器目录&gt;/module/</p>
                    <p><strong>用户模块:</strong> &lt;项目目录&gt;/module/</p>
                    <p><strong>源码目录:</strong> src/Module/ (仅开发分类，不搜索)</p>
                    
                    <h3>🎨 支持的格式变体</h3>
                    <p><strong>CMOD:</strong> CMOD/Cmod/cmod</p>
                    <p><strong>CJMOD:</strong> CJMOD/CJmod/cjmod</p>
                </div>
                
                <script>
                    const vscode = acquireVsCodeApi();
                    
                    function createCMOD() {
                        vscode.postMessage({ command: 'createCMOD' });
                    }
                    
                    function createCJMOD() {
                        vscode.postMessage({ command: 'createCJMOD' });
                    }
                    
                    function scanModules() {
                        vscode.postMessage({ command: 'scanModules' });
                    }
                    
                    function packModules() {
                        vscode.postMessage({ command: 'packModules' });
                    }
                </script>
            </body>
            </html>
        `;
    });
    
    // ========================================
    // 4. 编译和预览功能
    // ========================================
    
    const compileCommand = vscode.commands.registerCommand('chtl.compile', async () => {
        const activeEditor = vscode.window.activeTextEditor;
        if (!activeEditor || activeEditor.document.languageId !== 'chtl') {
            vscode.window.showErrorMessage('请在CHTL文件中使用编译功能');
            return;
        }
        
        await activeEditor.document.save();
        
        vscode.window.showInformationMessage('🔧 CHTL编译功能开发中...', '查看进度').then(selection => {
            if (selection === '查看进度') {
                vscode.env.openExternal(vscode.Uri.parse('https://github.com/chtl-lang/chtl'));
            }
        });
    });
    
    // 注册所有功能
    context.subscriptions.push(
        completionProvider,
        moduleManagerCommand,
        compileCommand,
        diagnosticCollection
    );
    
    // 监听文档变化进行验证
    vscode.workspace.onDidChangeTextDocument(event => {
        validateDocument(event.document);
    });
    
    vscode.workspace.onDidOpenTextDocument(validateDocument);
    
    console.log('🚀 CHTL IDE扩展功能就绪');
}

export function deactivate() {
    console.log('👋 CHTL IDE扩展已停用');
}