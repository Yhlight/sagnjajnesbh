import * as vscode from 'vscode';
import { ModuleResolver } from '../modules/moduleResolver';

export class ModuleIndexCommands {
    private moduleResolver: ModuleResolver;

    constructor(moduleResolver: ModuleResolver) {
        this.moduleResolver = moduleResolver;
    }

    /**
     * 注册所有模块索引相关命令
     */
    public registerCommands(context: vscode.ExtensionContext): void {
        // 显示模块索引状态
        const showIndexStatusCommand = vscode.commands.registerCommand(
            'chtl.modules.showIndexStatus',
            () => this.showIndexStatus()
        );

        // 刷新项目模块索引
        const refreshProjectIndexCommand = vscode.commands.registerCommand(
            'chtl.modules.refreshProjectIndex',
            () => this.refreshProjectIndex()
        );

        // 搜索模块
        const searchModulesCommand = vscode.commands.registerCommand(
            'chtl.modules.search',
            () => this.searchModules()
        );

        // 显示模块详情
        const showModuleDetailsCommand = vscode.commands.registerCommand(
            'chtl.modules.showDetails',
            (moduleName?: string) => this.showModuleDetails(moduleName)
        );

        context.subscriptions.push(
            showIndexStatusCommand,
            refreshProjectIndexCommand,
            searchModulesCommand,
            showModuleDetailsCommand
        );
    }

    /**
     * 显示模块索引状态
     */
    private async showIndexStatus(): Promise<void> {
        const status = this.moduleResolver.getIndexStatus();
        
        const statusMessage = `
# CHTL 模块索引状态报告

## 📊 索引概览
- **官方模块索引**: ${status.official ? '✅ 已加载' : '❌ 未加载'} (${status.officialCount} 个模块)
- **项目模块索引**: ${status.project ? '✅ 已加载' : '❌ 未加载'} (${status.projectCount} 个模块)
- **总模块数**: ${status.officialCount + status.projectCount}

## 🔧 性能优化
${status.official ? 
    '✅ 官方模块使用预构建索引，无需运行时解包，性能最佳' : 
    '⚠️ 官方模块索引缺失，将使用实时扫描模式，性能较差'}

${status.project ? 
    '✅ 项目模块索引已缓存，智能提示响应快速' : 
    '⚠️ 项目模块索引缺失，可点击"刷新项目索引"按钮重建'}

## 📝 详细信息
- **索引格式**: JSON查询表，支持按名称、导出、类型快速搜索
- **CMOD模块**: 使用[Export]块信息，解析准确高效
- **CJMOD模块**: 手动解析extern "C"、class、namespace等导出
- **自动更新**: 项目模块在文件变更时自动重建索引

## 🚀 可用操作
- 刷新项目模块索引
- 搜索所有模块
- 查看模块详细信息
`;

        const panel = vscode.window.createWebviewPanel(
            'chtlModuleIndexStatus',
            'CHTL 模块索引状态',
            vscode.ViewColumn.One,
            {
                enableScripts: true,
                retainContextWhenHidden: true
            }
        );

        panel.webview.html = this.getStatusWebviewContent(statusMessage, status);

        // 处理webview消息
        panel.webview.onDidReceiveMessage(
            async message => {
                switch (message.command) {
                    case 'refreshProjectIndex':
                        await this.refreshProjectIndex();
                        // 更新状态显示
                        const newStatus = this.moduleResolver.getIndexStatus();
                        panel.webview.postMessage({ 
                            command: 'updateStatus', 
                            status: newStatus 
                        });
                        break;
                    case 'searchModules':
                        await this.searchModules();
                        break;
                }
            }
        );
    }

    /**
     * 刷新项目模块索引
     */
    private async refreshProjectIndex(): Promise<void> {
        const progress = await vscode.window.withProgress({
            location: vscode.ProgressLocation.Notification,
            title: '刷新项目模块索引',
            cancellable: false
        }, async (progress) => {
            progress.report({ increment: 0, message: '正在扫描项目模块...' });
            
            try {
                await this.moduleResolver.refreshProjectModuleIndex();
                progress.report({ increment: 100, message: '索引刷新完成' });
                
                vscode.window.showInformationMessage(
                    '✅ 项目模块索引刷新成功',
                    '查看状态'
                ).then(selection => {
                    if (selection === '查看状态') {
                        this.showIndexStatus();
                    }
                });
            } catch (error) {
                vscode.window.showErrorMessage(
                    `❌ 刷新项目模块索引失败: ${error}`
                );
            }
        });
    }

    /**
     * 搜索模块
     */
    private async searchModules(): Promise<void> {
        const query = await vscode.window.showInputBox({
            prompt: '输入模块名称或导出名称进行搜索',
            placeHolder: '例如: Button, createComponent, MyStyle...'
        });

        if (!query) return;

        try {
            const results = this.moduleResolver.searchModulesWithIndex(query);
            
            if (results.length === 0) {
                vscode.window.showInformationMessage(`未找到与 "${query}" 相关的模块`);
                return;
            }

            // 显示搜索结果
            const items = results.map(module => ({
                label: module.name,
                description: `${module.type} | ${module.isOfficial ? '官方' : '项目'}`,
                detail: module.description || `导出: ${module.exports.join(', ')}`,
                module: module
            }));

            const selected = await vscode.window.showQuickPick(items, {
                title: `搜索结果: "${query}"`,
                placeHolder: '选择模块查看详情',
                matchOnDescription: true,
                matchOnDetail: true
            });

            if (selected) {
                await this.showModuleDetails(selected.module.name);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`搜索模块失败: ${error}`);
        }
    }

    /**
     * 显示模块详情
     */
    private async showModuleDetails(moduleName?: string): Promise<void> {
        if (!moduleName) {
            // 让用户选择模块
            const allModules = this.moduleResolver.getAllModulesFromIndex();
            const items = allModules.map(module => ({
                label: module.name,
                description: `${module.type} | ${module.isOfficial ? '官方' : '项目'}`,
                detail: module.description,
                module: module
            }));

            const selected = await vscode.window.showQuickPick(items, {
                title: '选择模块',
                placeHolder: '选择要查看详情的模块',
                matchOnDescription: true,
                matchOnDetail: true
            });

            if (!selected) return;
            moduleName = selected.module.name;
        }

        // 获取模块详细信息
        const allModules = this.moduleResolver.getAllModulesFromIndex();
        const module = allModules.find(m => m.name === moduleName);
        
        if (!module) {
            vscode.window.showErrorMessage(`未找到模块: ${moduleName}`);
            return;
        }

        const exports = this.moduleResolver.getModuleExports(moduleName);
        
        let exportsInfo = '';
        if (exports.length > 0) {
            exportsInfo = exports.map(exp => 
                `- **${exp.name}** (${exp.type})${exp.signature ? `: \`${exp.signature}\`` : ''}${exp.description ? ` - ${exp.description}` : ''}`
            ).join('\n');
        } else {
            exportsInfo = '无导出信息';
        }

        const moduleDetails = `
# 📦 ${module.name}

## 基本信息
- **类型**: ${module.type}
- **来源**: ${module.isOfficial ? '官方模块' : '项目模块'}
- **路径**: \`${module.path}\`
${module.version ? `- **版本**: ${module.version}` : ''}
${module.description ? `- **描述**: ${module.description}` : ''}

## 导出内容
${exportsInfo}

${module.imports && module.imports.length > 0 ? `
## 依赖模块
${module.imports.map(imp => `- ${imp}`).join('\n')}
` : ''}

## 📝 使用示例
\`\`\`chtl
[Import] @Chtl from ${module.name}
// 或者
[Import] @CJmod from ${module.name}
\`\`\`
`;

        const panel = vscode.window.createWebviewPanel(
            'chtlModuleDetails',
            `模块详情: ${module.name}`,
            vscode.ViewColumn.One,
            {
                enableScripts: false,
                retainContextWhenHidden: false
            }
        );

        panel.webview.html = this.getDetailsWebviewContent(moduleDetails);
    }

    /**
     * 获取状态页面的HTML内容
     */
    private getStatusWebviewContent(statusMessage: string, status: any): string {
        return `
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL 模块索引状态</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
            margin: 20px;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
        }
        .header {
            border-bottom: 1px solid var(--vscode-panel-border);
            padding-bottom: 10px;
            margin-bottom: 20px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin: 20px 0;
        }
        .status-card {
            border: 1px solid var(--vscode-panel-border);
            border-radius: 6px;
            padding: 15px;
            background: var(--vscode-panel-background);
        }
        .actions {
            margin-top: 30px;
            display: flex;
            gap: 10px;
        }
        button {
            background: var(--vscode-button-background);
            color: var(--vscode-button-foreground);
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            cursor: pointer;
            font-size: 14px;
        }
        button:hover {
            background: var(--vscode-button-hoverBackground);
        }
        .success { color: var(--vscode-charts-green); }
        .warning { color: var(--vscode-charts-orange); }
        .error { color: var(--vscode-charts-red); }
    </style>
</head>
<body>
    <div class="header">
        <h1>🔍 CHTL 模块索引状态</h1>
    </div>
    
    <div class="status-grid">
        <div class="status-card">
            <h3>📚 官方模块</h3>
            <p class="${status.official ? 'success' : 'error'}">
                ${status.official ? '✅ 已加载' : '❌ 未加载'}
            </p>
            <p>模块数量: <strong>${status.officialCount}</strong></p>
            <p>状态: ${status.official ? '使用预构建索引，性能最佳' : '需要重新安装扩展'}</p>
        </div>
        
        <div class="status-card">
            <h3>🏗️ 项目模块</h3>
            <p class="${status.project ? 'success' : 'warning'}">
                ${status.project ? '✅ 已加载' : '⚠️ 未加载'}
            </p>
            <p>模块数量: <strong>${status.projectCount}</strong></p>
            <p>状态: ${status.project ? '索引已缓存，响应快速' : '可手动刷新构建索引'}</p>
        </div>
    </div>
    
    <div class="actions">
        <button onclick="refreshProjectIndex()">🔄 刷新项目索引</button>
        <button onclick="searchModules()">🔍 搜索模块</button>
    </div>
    
    <div style="margin-top: 30px; padding: 15px; background: var(--vscode-textCodeBlock-background); border-radius: 6px;">
        <h3>💡 性能提示</h3>
        <ul>
            <li>官方模块索引在扩展打包时预构建，无需运行时解包，性能最佳</li>
            <li>项目模块索引在首次使用时构建，后续自动检测变更并更新</li>
            <li>JSON查询表支持按名称、导出、类型进行O(1)快速搜索</li>
            <li>CMOD使用[Export]块信息，CJMOD手动解析导出函数和类</li>
        </ul>
    </div>
    
    <script>
        const vscode = acquireVsCodeApi();
        
        function refreshProjectIndex() {
            vscode.postMessage({ command: 'refreshProjectIndex' });
        }
        
        function searchModules() {
            vscode.postMessage({ command: 'searchModules' });
        }
        
        // 监听状态更新
        window.addEventListener('message', event => {
            const message = event.data;
            if (message.command === 'updateStatus') {
                location.reload();
            }
        });
    </script>
</body>
</html>`;
    }

    /**
     * 获取详情页面的HTML内容
     */
    private getDetailsWebviewContent(content: string): string {
        // 使用简单的Markdown渲染
        const htmlContent = content
            .replace(/^# (.+)$/gm, '<h1>$1</h1>')
            .replace(/^## (.+)$/gm, '<h2>$1</h2>')
            .replace(/^### (.+)$/gm, '<h3>$1</h3>')
            .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
            .replace(/`(.+?)`/g, '<code>$1</code>')
            .replace(/^- (.+)$/gm, '<li>$1</li>')
            .replace(/```(\w+)?\n([\s\S]*?)```/g, '<pre><code>$2</code></pre>')
            .split('\n').map(line => line.trim() ? `<p>${line}</p>` : '').join('');

        return `
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>模块详情</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            line-height: 1.6;
            margin: 20px;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
        }
        code {
            background: var(--vscode-textCodeBlock-background);
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Consolas', 'Monaco', monospace;
        }
        pre {
            background: var(--vscode-textCodeBlock-background);
            padding: 15px;
            border-radius: 6px;
            overflow-x: auto;
        }
        h1, h2, h3 {
            color: var(--vscode-textLink-foreground);
            margin-top: 30px;
            margin-bottom: 15px;
        }
        li {
            margin: 5px 0;
        }
    </style>
</head>
<body>
    ${htmlContent}
</body>
</html>`;
    }
}