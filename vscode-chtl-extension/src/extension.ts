import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    console.log('🎉 CHTL IDE扩展已激活 - 基于最新语法文档');
    
    // 注册代码补全
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        {
            provideCompletionItems(document, position) {
                const completions: vscode.CompletionItem[] = [];
                
                // 正确的段落补全
                const sections = [
                    { name: 'Info', desc: '模块信息段' },
                    { name: 'Export', desc: '导出信息段' },
                    { name: 'Import', desc: '导入段' },
                    { name: 'Namespace', desc: '命名空间段' },
                    { name: 'Template', desc: '模板段' },
                    { name: 'Custom', desc: '自定义段' },
                    { name: 'Origin', desc: '原始嵌入段' },
                    { name: 'Configuration', desc: '配置段' }
                ];
                sections.forEach(section => {
                    const item = new vscode.CompletionItem(`[${section.name}]`, vscode.CompletionItemKind.Keyword);
                    item.detail = section.desc;
                    completions.push(item);
                });
                
                // 珂朵莉模块补全
                const chthollyModules = ['Accordion', 'Gallery', 'Memo', 'Notes', 'Sakura'];
                chthollyModules.forEach(module => {
                    const item = new vscode.CompletionItem(`Chtholly.${module}`, vscode.CompletionItemKind.Module);
                    item.detail = `珂朵莉${module}模块`;
                    completions.push(item);
                });
                
                // 由比滨结衣模块补全
                const yuigahamaModules = ['MusicPlayer', 'Accordion', 'Gallery', 'Memo'];
                yuigahamaModules.forEach(module => {
                    const item = new vscode.CompletionItem(`Yuigahama.${module}`, vscode.CompletionItemKind.Module);
                    item.detail = `由比滨结衣${module}模块`;
                    completions.push(item);
                });
                
                return completions;
            }
        },
        '{', '[', '@'
    );
    
    context.subscriptions.push(completionProvider);
    
    // 注册命令
    const createFileCommand = vscode.commands.registerCommand('chtl.createFile', async () => {
        const fileName = await vscode.window.showInputBox({
            prompt: '输入CHTL文件名',
            placeholder: 'MyComponent.chtl'
        });
        
        if (fileName) {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            if (workspaceFolder) {
                const filePath = vscode.Uri.joinPath(workspaceFolder.uri, fileName);
                const template = `[Style]
.component {
    /* 样式 */
}

[Component]
{{Component}}
    {{.component}}
        <!-- 内容 -->
    {{/.component}}
{{/Component}}

[Script]
function createComponent() {
    // 逻辑
}`;
                
                await vscode.workspace.fs.writeFile(filePath, Buffer.from(template, 'utf8'));
                const document = await vscode.workspace.openTextDocument(filePath);
                await vscode.window.showTextDocument(document);
            }
        }
    });
    
    context.subscriptions.push(createFileCommand);
    
    vscode.window.showInformationMessage('🌸 CHTL IDE已就绪！支持珂朵莉和由比滨结衣模块 💕');
}

export function deactivate() {
    console.log('👋 CHTL扩展已停用');
}