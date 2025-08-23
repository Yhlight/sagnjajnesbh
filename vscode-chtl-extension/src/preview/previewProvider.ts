import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { CHTLCompilerService, CompilationResult } from '../compiler/compilerService';

export class CHTLPreviewProvider implements vscode.WebviewViewProvider {
    public static readonly viewType = 'chtl.preview';
    private _view?: vscode.WebviewView;
    private _compiler: CHTLCompilerService;
    private _currentFile?: string;
    private _updateTimer?: NodeJS.Timeout;

    constructor(
        private readonly _extensionUri: vscode.Uri,
        compiler: CHTLCompilerService
    ) {
        this._compiler = compiler;
    }

    public resolveWebviewView(
        webviewView: vscode.WebviewView,
        context: vscode.WebviewViewResolveContext,
        _token: vscode.CancellationToken,
    ) {
        this._view = webviewView;

        webviewView.webview.options = {
            enableScripts: true,
            localResourceRoots: [
                this._extensionUri,
                vscode.Uri.joinPath(this._extensionUri, 'media'),
                vscode.workspace.workspaceFolders?.[0]?.uri || this._extensionUri
            ]
        };

        this.updatePreview();
        this.setupFileWatcher();
    }

    public async updatePreview(document?: vscode.TextDocument) {
        if (!this._view) return;

        const activeEditor = vscode.window.activeTextEditor;
        if (!activeEditor && !document) return;

        const currentDoc = document || activeEditor?.document;
        if (!currentDoc || !this.isCHTLFile(currentDoc)) return;

        this._currentFile = currentDoc.fileName;

        // 清除之前的定时器
        if (this._updateTimer) {
            clearTimeout(this._updateTimer);
        }

        // 防抖更新
        this._updateTimer = setTimeout(async () => {
            try {
                const compilationResult = await this._compiler.compileFile(currentDoc.fileName);
                await this.renderPreview(compilationResult, currentDoc);
            } catch (error) {
                this.showError(`预览更新失败: ${error}`);
            }
        }, 300);
    }

    private async renderPreview(result: CompilationResult, document: vscode.TextDocument) {
        if (!this._view) return;

        const webview = this._view.webview;
        
        if (result.success && result.outputFiles.length > 0) {
            // 找到HTML输出文件
            const htmlFile = result.outputFiles.find(file => file.endsWith('.html'));
            if (htmlFile && fs.existsSync(htmlFile)) {
                let htmlContent = fs.readFileSync(htmlFile, 'utf8');
                
                // 处理相对路径，使其在webview中正确工作
                htmlContent = this.processResourcePaths(htmlContent, path.dirname(htmlFile));
                
                // 注入实时更新脚本
                htmlContent = this.injectLiveReloadScript(htmlContent);
                
                webview.html = this.getPreviewHTML(htmlContent, document.fileName);
            } else {
                this.showCompilationOutput(result);
            }
        } else {
            this.showError(`编译失败:\n${result.errors.join('\n')}`);
        }
    }

    private processResourcePaths(html: string, baseDir: string): string {
        const webview = this._view!.webview;
        
        // 处理CSS文件引用
        html = html.replace(/href="([^"]+\.css)"/g, (match, cssPath) => {
            if (!path.isAbsolute(cssPath)) {
                const fullPath = path.join(baseDir, cssPath);
                const cssUri = webview.asWebviewUri(vscode.Uri.file(fullPath));
                return `href="${cssUri}"`;
            }
            return match;
        });

        // 处理JavaScript文件引用
        html = html.replace(/src="([^"]+\.js)"/g, (match, jsPath) => {
            if (!path.isAbsolute(jsPath)) {
                const fullPath = path.join(baseDir, jsPath);
                const jsUri = webview.asWebviewUri(vscode.Uri.file(fullPath));
                return `src="${jsUri}"`;
            }
            return match;
        });

        // 处理图片资源
        html = html.replace(/src="([^"]+\.(png|jpg|jpeg|gif|svg))"/g, (match, imgPath) => {
            if (!path.isAbsolute(imgPath)) {
                const fullPath = path.join(baseDir, imgPath);
                const imgUri = webview.asWebviewUri(vscode.Uri.file(fullPath));
                return `src="${imgUri}"`;
            }
            return match;
        });

        return html;
    }

    private injectLiveReloadScript(html: string): string {
        const liveReloadScript = `
            <script>
                (function() {
                    const vscode = acquireVsCodeApi();
                    
                    // 监听来自扩展的消息
                    window.addEventListener('message', event => {
                        const message = event.data;
                        switch (message.command) {
                            case 'refresh':
                                location.reload();
                                break;
                            case 'updateContent':
                                document.documentElement.innerHTML = message.content;
                                break;
                        }
                    });
                    
                    // 页面加载完成后通知扩展
                    window.addEventListener('load', () => {
                        vscode.postMessage({
                            command: 'previewLoaded',
                            timestamp: Date.now()
                        });
                    });
                    
                    // 处理页面中的错误
                    window.addEventListener('error', (error) => {
                        vscode.postMessage({
                            command: 'previewError',
                            error: error.message,
                            stack: error.error?.stack
                        });
                    });
                })();
            </script>
        `;

        // 在</head>之前插入脚本
        if (html.includes('</head>')) {
            return html.replace('</head>', `${liveReloadScript}</head>`);
        } else {
            // 如果没有head标签，在body开始处插入
            return html.replace('<body>', `<body>${liveReloadScript}`);
        }
    }

    private getPreviewHTML(content: string, fileName: string): string {
        return `
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>CHTL Preview - ${path.basename(fileName)}</title>
                <style>
                    body {
                        margin: 0;
                        padding: 0;
                        font-family: var(--vscode-font-family);
                        background-color: var(--vscode-editor-background);
                        color: var(--vscode-editor-foreground);
                    }
                    .preview-header {
                        background: var(--vscode-titleBar-activeBackground);
                        color: var(--vscode-titleBar-activeForeground);
                        padding: 8px 16px;
                        font-size: 12px;
                        border-bottom: 1px solid var(--vscode-panel-border);
                        display: flex;
                        justify-content: space-between;
                        align-items: center;
                    }
                    .preview-content {
                        height: calc(100vh - 40px);
                        overflow: auto;
                    }
                    .preview-iframe {
                        width: 100%;
                        height: 100%;
                        border: none;
                    }
                    .refresh-button {
                        background: var(--vscode-button-background);
                        color: var(--vscode-button-foreground);
                        border: none;
                        padding: 4px 8px;
                        border-radius: 2px;
                        cursor: pointer;
                        font-size: 11px;
                    }
                    .refresh-button:hover {
                        background: var(--vscode-button-hoverBackground);
                    }
                </style>
            </head>
            <body>
                <div class="preview-header">
                    <span>预览: ${path.basename(fileName)}</span>
                    <button class="refresh-button" onclick="refreshPreview()">刷新</button>
                </div>
                <div class="preview-content">
                    ${content}
                </div>
                
                <script>
                    function refreshPreview() {
                        const vscode = acquireVsCodeApi();
                        vscode.postMessage({ command: 'refreshPreview' });
                    }
                </script>
            </body>
            </html>
        `;
    }

    private showError(message: string) {
        if (!this._view) return;

        this._view.webview.html = `
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="UTF-8">
                <style>
                    body {
                        font-family: var(--vscode-font-family);
                        background-color: var(--vscode-editor-background);
                        color: var(--vscode-editor-foreground);
                        padding: 20px;
                    }
                    .error {
                        background: var(--vscode-inputValidation-errorBackground);
                        color: var(--vscode-inputValidation-errorForeground);
                        border: 1px solid var(--vscode-inputValidation-errorBorder);
                        padding: 16px;
                        border-radius: 4px;
                        font-family: var(--vscode-editor-font-family);
                        white-space: pre-wrap;
                    }
                </style>
            </head>
            <body>
                <h3>预览错误</h3>
                <div class="error">${message}</div>
            </body>
            </html>
        `;
    }

    private showCompilationOutput(result: CompilationResult) {
        if (!this._view) return;

        this._view.webview.html = `
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="UTF-8">
                <style>
                    body {
                        font-family: var(--vscode-font-family);
                        background-color: var(--vscode-editor-background);
                        color: var(--vscode-editor-foreground);
                        padding: 20px;
                    }
                    .output {
                        background: var(--vscode-terminal-background);
                        color: var(--vscode-terminal-foreground);
                        border: 1px solid var(--vscode-panel-border);
                        padding: 16px;
                        border-radius: 4px;
                        font-family: var(--vscode-editor-font-family);
                        white-space: pre-wrap;
                    }
                </style>
            </head>
            <body>
                <h3>编译输出</h3>
                <div class="output">${result.output}</div>
            </body>
            </html>
        `;
    }

    private setupFileWatcher() {
        // 监听活动编辑器变化
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && this.isCHTLFile(editor.document)) {
                this.updatePreview(editor.document);
            }
        });

        // 监听文档内容变化
        vscode.workspace.onDidChangeTextDocument((event) => {
            if (this.isCHTLFile(event.document)) {
                this.updatePreview(event.document);
            }
        });

        // 监听文档保存
        vscode.workspace.onDidSaveTextDocument((document) => {
            if (this.isCHTLFile(document)) {
                this.updatePreview(document);
            }
        });
    }

    private isCHTLFile(document: vscode.TextDocument): boolean {
        const languageId = document.languageId;
        const fileName = document.fileName;
        
        return languageId === 'chtl' || 
               fileName.endsWith('.chtl') || 
               fileName.endsWith('.cmod');
    }

    public refresh() {
        const activeEditor = vscode.window.activeTextEditor;
        if (activeEditor && this.isCHTLFile(activeEditor.document)) {
            this.updatePreview(activeEditor.document);
        }
    }

    public dispose() {
        if (this._updateTimer) {
            clearTimeout(this._updateTimer);
        }
    }
}