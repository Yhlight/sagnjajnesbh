import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { CHTLLanguageServer } from './languageServer';
import { CHTLCompilerService } from './compiler/compilerService';
import { ModuleResolver } from './modules/moduleResolver';
import { CHTLDiagnosticsProvider } from './diagnostics/diagnosticsProvider';
import { CHTLIntelliSenseProvider } from './intellisense/intellisenseProvider';
import { CHTLConflictResolver } from './intellisense/conflictResolver';
import { CHTLFormattingProvider } from './formatting/formattingProvider';
import { CHTLDebugAdapter } from './debug/debugAdapter';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL Language Support is now active!');

    // 初始化配置
    const config = vscode.workspace.getConfiguration('chtl');
    
    // 创建核心服务实例
    const compilerService = new CHTLCompilerService(context);
    const moduleResolver = new ModuleResolver(context, config);
    const diagnosticsProvider = new CHTLDiagnosticsProvider(compilerService);
    const languageServer = new CHTLLanguageServer(context, compilerService, moduleResolver);
    const intellisenseProvider = new CHTLIntelliSenseProvider(moduleResolver, compilerService);
    const conflictResolver = new CHTLConflictResolver(config);
    const formattingProvider = new CHTLFormattingProvider();
    const debugAdapter = new CHTLDebugAdapter(compilerService);

    // 注册语言服务器
    languageServer.start();

    // 注册智能提示提供器
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        ['chtl', 'chtl-js'],
        {
            async provideCompletionItems(document, position, token, context) {
                // 检查冲突解决策略
                const conflictStrategy = config.get<string>('intellisense.conflictResolution', 'smart-detection');
                
                if (conflictStrategy === 'smart-detection') {
                    const isInCHTLContext = await conflictResolver.detectCHTLContext(document, position);
                    if (!isInCHTLContext) {
                        return undefined; // 让VSCode默认补全接管
                    }
                }
                
                return intellisenseProvider.provideCompletionItems(document, position, token, context);
            }
        },
        '.',  // 触发字符：点号用于模块导入
        '@',  // 触发字符：@符号用于类型标识
        '{',  // 触发字符：花括号用于语法块
        '&',  // 触发字符：引用选择器
        '#',  // 触发字符：ID选择器
        '['   // 触发字符：方括号用于配置、命名空间等
    );

    // 注册悬停提示提供器
    const hoverProvider = vscode.languages.registerHoverProvider(
        ['chtl', 'chtl-js'],
        {
            async provideHover(document, position, token) {
                return intellisenseProvider.provideHover(document, position, token);
            }
        }
    );

    // 注册定义跳转提供器
    const definitionProvider = vscode.languages.registerDefinitionProvider(
        ['chtl', 'chtl-js'],
        {
            async provideDefinition(document, position, token) {
                return intellisenseProvider.provideDefinition(document, position, token);
            }
        }
    );

    // 注册格式化提供器
    const documentFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider(
        ['chtl', 'chtl-js'],
        formattingProvider
    );

    // 注册诊断提供器
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    diagnosticsProvider.setDiagnosticCollection(diagnosticCollection);

    // 监听文档变化
    const documentChangeListener = vscode.workspace.onDidChangeTextDocument(async (event) => {
        if (event.document.languageId === 'chtl' || event.document.languageId === 'chtl-js') {
            // 实时编译和诊断
            if (config.get<boolean>('compiler.enableRealTimeCompilation', true)) {
                await diagnosticsProvider.updateDiagnostics(event.document);
            }
        }
    });

    // 监听文档保存
    const documentSaveListener = vscode.workspace.onDidSaveTextDocument(async (document) => {
        if (document.languageId === 'chtl' || document.languageId === 'chtl-js') {
            // 保存时编译
            await compilerService.compileFile(document.uri.fsPath);
        }
    });

    // 注册命令
    const commands = [
        vscode.commands.registerCommand('chtl.compile.current', async () => {
            const activeEditor = vscode.window.activeTextEditor;
            if (activeEditor && (activeEditor.document.languageId === 'chtl' || activeEditor.document.languageId === 'chtl-js')) {
                await compilerService.compileFile(activeEditor.document.uri.fsPath);
                vscode.window.showInformationMessage('CHTL file compiled successfully!');
            } else {
                vscode.window.showErrorMessage('No active CHTL file to compile.');
            }
        }),

        vscode.commands.registerCommand('chtl.compile.project', async () => {
            const workspaceFolders = vscode.workspace.workspaceFolders;
            if (workspaceFolders) {
                await compilerService.compileProject(workspaceFolders[0].uri.fsPath);
                vscode.window.showInformationMessage('CHTL project compiled successfully!');
            } else {
                vscode.window.showErrorMessage('No workspace folder found.');
            }
        }),

        vscode.commands.registerCommand('chtl.validate.syntax', async () => {
            const activeEditor = vscode.window.activeTextEditor;
            if (activeEditor && (activeEditor.document.languageId === 'chtl' || activeEditor.document.languageId === 'chtl-js')) {
                await diagnosticsProvider.validateSyntax(activeEditor.document);
                vscode.window.showInformationMessage('Syntax validation completed.');
            }
        }),

        vscode.commands.registerCommand('chtl.modules.refresh', async () => {
            await moduleResolver.refreshCache();
            vscode.window.showInformationMessage('Module cache refreshed.');
        }),

        vscode.commands.registerCommand('chtl.preview.open', async () => {
            const activeEditor = vscode.window.activeTextEditor;
            if (activeEditor && activeEditor.document.languageId === 'chtl') {
                // 创建预览面板
                const panel = vscode.window.createWebviewPanel(
                    'chtlPreview',
                    'CHTL Preview',
                    vscode.ViewColumn.Beside,
                    {
                        enableScripts: true,
                        retainContextWhenHidden: true
                    }
                );

                // 编译并显示预览
                const compiledOutput = await compilerService.compileToPreview(activeEditor.document.uri.fsPath);
                panel.webview.html = compiledOutput;
            }
        }),

        vscode.commands.registerCommand('chtl.debug.toggleLogs', () => {
            const currentState = config.get<boolean>('debug.enableDetailedLogs', false);
            config.update('debug.enableDetailedLogs', !currentState, vscode.ConfigurationTarget.Workspace);
            vscode.window.showInformationMessage(`Debug logs ${!currentState ? 'enabled' : 'disabled'}.`);
        }),

        vscode.commands.registerCommand('chtl.toggleConflictResolution', () => {
            const newStrategy = conflictResolver.toggleStrategy();
            vscode.window.showInformationMessage(`冲突解决策略已切换为: ${newStrategy}`);
            // 更新状态栏
            conflictStatusBarItem.text = `$(symbol-method) CHTL-${newStrategy}`;
        })
    ];

    // 注册配置变化监听
    const configChangeListener = vscode.workspace.onDidChangeConfiguration((event) => {
        if (event.affectsConfiguration('chtl')) {
            // 重新加载配置
            const newConfig = vscode.workspace.getConfiguration('chtl');
            moduleResolver.updateConfig(newConfig);
            conflictResolver.updateConfig(newConfig);
            
            // 重新初始化诊断
            if (event.affectsConfiguration('chtl.validation')) {
                diagnosticsProvider.refreshAll();
            }
        }
    });

    // 注册调试配置提供器
    const debugConfigProvider = vscode.debug.registerDebugConfigurationProvider(
        'chtl',
        {
            resolveDebugConfiguration(folder, config, token) {
                // 如果没有配置，提供默认配置
                if (!config.type && !config.request && !config.name) {
                    const editor = vscode.window.activeTextEditor;
                    if (editor && editor.document.languageId === 'chtl') {
                        config.type = 'chtl';
                        config.name = 'Debug CHTL';
                        config.request = 'launch';
                        config.program = editor.document.fileName;
                    }
                }
                return config;
            }
        }
    );

    // 注册调试适配器
    const debugAdapterDescriptor = vscode.debug.registerDebugAdapterDescriptorFactory('chtl', {
        createDebugAdapterDescriptor(session) {
            return new vscode.DebugAdapterInlineImplementation(debugAdapter);
        }
    });

    // 创建状态栏项
    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
    statusBarItem.text = "$(gear) CHTL";
    statusBarItem.tooltip = "CHTL Language Support";
    statusBarItem.command = 'chtl.compile.current';
    statusBarItem.show();

    // 创建冲突解决器状态栏项
    const conflictStatusBarItem = conflictResolver.createStatusBarItem();

    // 添加所有订阅到上下文
    context.subscriptions.push(
        completionProvider,
        hoverProvider,
        definitionProvider,
        documentFormattingProvider,
        documentChangeListener,
        documentSaveListener,
        configChangeListener,
        debugConfigProvider,
        debugAdapterDescriptor,
        diagnosticCollection,
        statusBarItem,
        conflictStatusBarItem,
        ...commands
    );

    // 显示激活消息
    vscode.window.showInformationMessage('CHTL Language Support activated! Ready for modern development.');
}

export function deactivate() {
    console.log('CHTL Language Support is now deactivated.');
}