import * as vscode from 'vscode';
import { CHTLCompilerService } from '../compiler/compilerService';

export interface CHTLDebugSession {
    id: string;
    name: string;
    workspaceFolder: string;
    configuration: vscode.DebugConfiguration;
    started: Date;
    status: 'running' | 'stopped' | 'paused' | 'error';
}

export interface CHTLBreakpoint {
    id: number;
    file: string;
    line: number;
    column?: number;
    condition?: string;
    hitCondition?: string;
    enabled: boolean;
}

export interface CHTLStackFrame {
    id: number;
    name: string;
    file: string;
    line: number;
    column: number;
    context: 'chtl' | 'css' | 'javascript' | 'template';
}

/**
 * CHTL调试适配器
 * 提供CHTL代码的调试、断点管理和编译输出监控功能
 */
export class CHTLDebugAdapter implements vscode.DebugAdapter {
    private readonly _onDidSendMessage = new vscode.EventEmitter<vscode.DebugProtocolMessage>();
    private sequence: number = 1;
    private readonly compilerService: CHTLCompilerService;
    private debugSession?: CHTLDebugSession;
    private breakpoints: Map<string, CHTLBreakpoint[]> = new Map();
    private stackFrames: CHTLStackFrame[] = [];
    private outputChannel: vscode.OutputChannel;
    private compilationWatcher?: vscode.FileSystemWatcher;

    readonly onDidSendMessage: vscode.Event<vscode.DebugProtocolMessage> = this._onDidSendMessage.event;

    constructor(compilerService: CHTLCompilerService) {
        this.compilerService = compilerService;
        this.outputChannel = vscode.window.createOutputChannel('CHTL Debug');
    }

    handleMessage(message: vscode.DebugProtocolMessage): void {
        switch (message.type) {
            case 'request':
                this.handleRequest(message as vscode.DebugProtocolRequest);
                break;
            case 'response':
                // 处理响应消息
                break;
            case 'event':
                // 处理事件消息
                break;
        }
    }

    dispose(): void {
        this.stopDebugging();
        this.outputChannel.dispose();
        if (this.compilationWatcher) {
            this.compilationWatcher.dispose();
        }
    }

    /**
     * 处理调试请求
     */
    private handleRequest(request: vscode.DebugProtocolRequest): void {
        try {
            switch (request.command) {
                case 'initialize':
                    this.handleInitializeRequest(request);
                    break;
                case 'launch':
                    this.handleLaunchRequest(request);
                    break;
                case 'attach':
                    this.handleAttachRequest(request);
                    break;
                case 'setBreakpoints':
                    this.handleSetBreakpointsRequest(request);
                    break;
                case 'continue':
                    this.handleContinueRequest(request);
                    break;
                case 'next':
                    this.handleNextRequest(request);
                    break;
                case 'stepIn':
                    this.handleStepInRequest(request);
                    break;
                case 'stepOut':
                    this.handleStepOutRequest(request);
                    break;
                case 'pause':
                    this.handlePauseRequest(request);
                    break;
                case 'stackTrace':
                    this.handleStackTraceRequest(request);
                    break;
                case 'scopes':
                    this.handleScopesRequest(request);
                    break;
                case 'variables':
                    this.handleVariablesRequest(request);
                    break;
                case 'evaluate':
                    this.handleEvaluateRequest(request);
                    break;
                case 'disconnect':
                    this.handleDisconnectRequest(request);
                    break;
                default:
                    this.sendErrorResponse(request, `未知的调试命令: ${request.command}`);
                    break;
            }
        } catch (error) {
            this.sendErrorResponse(request, `处理调试请求时出错: ${error}`);
        }
    }

    /**
     * 处理初始化请求
     */
    private handleInitializeRequest(request: vscode.DebugProtocolRequest): void {
        this.log('初始化CHTL调试适配器');
        
        const response: any = {
            type: 'response',
            request_seq: request.seq,
            command: request.command,
            success: true,
            body: {
                supportsConfigurationDoneRequest: true,
                supportsEvaluateForHovers: true,
                supportsStepBack: false,
                supportsSetVariable: true,
                supportsRestartFrame: false,
                supportsGotoTargetsRequest: false,
                supportsStepInTargetsRequest: false,
                supportsCompletionsRequest: true,
                supportsModulesRequest: false,
                supportsExceptionOptions: false,
                supportsValueFormattingOptions: true,
                supportsExceptionInfoRequest: false,
                supportTerminateDebuggee: true,
                supportSuspendDebuggee: false,
                supportsDelayedStackTraceLoading: false,
                supportsLoadedSourcesRequest: false,
                supportsLogPoints: true,
                supportsTerminateThreadsRequest: false,
                supportsSetExpression: false,
                supportsTerminateRequest: true,
                supportsDataBreakpoints: false,
                supportsReadMemoryRequest: false,
                supportsWriteMemoryRequest: false,
                supportsDisassembleRequest: false,
                supportsCancelRequest: false
            }
        };

        this.sendMessage(response);
        this.sendEvent('initialized');
    }

    /**
     * 处理启动请求
     */
    private async handleLaunchRequest(request: vscode.DebugProtocolRequest): Promise<void> {
        const args = request.arguments;
        
        this.log(`启动CHTL调试会话: ${args.program}`);
        
        // 创建调试会话
        this.debugSession = {
            id: `chtl-debug-${Date.now()}`,
            name: args.name || 'CHTL Debug',
            workspaceFolder: args.workspaceFolder || '',
            configuration: args,
            started: new Date(),
            status: 'running'
        };

        try {
            // 预编译程序
            await this.compileProgram(args.program);
            
            // 设置文件监听
            this.setupCompilationWatcher(args.program);
            
            // 启动调试监控
            this.startDebugging();
            
            this.sendResponse(request, true);
            this.sendEvent('process', {
                name: this.debugSession.name,
                systemProcessId: process.pid,
                isLocalProcess: true,
                startMethod: 'launch'
            });
            
        } catch (error) {
            this.sendErrorResponse(request, `启动调试会话失败: ${error}`);
        }
    }

    /**
     * 处理附加请求
     */
    private handleAttachRequest(request: vscode.DebugProtocolRequest): void {
        this.log('附加到CHTL进程');
        this.sendErrorResponse(request, 'CHTL调试适配器暂不支持附加模式');
    }

    /**
     * 处理设置断点请求
     */
    private handleSetBreakpointsRequest(request: vscode.DebugProtocolRequest): void {
        const args = request.arguments;
        const path = args.source.path;
        const requestedBreakpoints = args.breakpoints || [];
        
        this.log(`设置断点: ${path}, 数量: ${requestedBreakpoints.length}`);
        
        const breakpoints: CHTLBreakpoint[] = [];
        
        for (let i = 0; i < requestedBreakpoints.length; i++) {
            const requestedBreakpoint = requestedBreakpoints[i];
            const breakpoint: CHTLBreakpoint = {
                id: this.sequence++,
                file: path,
                line: requestedBreakpoint.line,
                column: requestedBreakpoint.column,
                condition: requestedBreakpoint.condition,
                hitCondition: requestedBreakpoint.hitCondition,
                enabled: true
            };
            
            breakpoints.push(breakpoint);
        }
        
        // 存储断点
        this.breakpoints.set(path, breakpoints);
        
        // 验证断点位置
        const verifiedBreakpoints = breakpoints.map(bp => ({
            verified: this.verifyBreakpoint(bp),
            line: bp.line,
            column: bp.column,
            message: this.verifyBreakpoint(bp) ? undefined : '无法在此位置设置断点'
        }));
        
        this.sendResponse(request, true, {
            breakpoints: verifiedBreakpoints
        });
    }

    /**
     * 处理继续执行请求
     */
    private handleContinueRequest(request: vscode.DebugProtocolRequest): void {
        this.log('继续执行');
        
        if (this.debugSession) {
            this.debugSession.status = 'running';
        }
        
        this.sendResponse(request, true);
        this.sendEvent('continued', {
            threadId: 1
        });
    }

    /**
     * 处理单步执行请求
     */
    private handleNextRequest(request: vscode.DebugProtocolRequest): void {
        this.log('单步执行');
        this.sendResponse(request, true);
        // 在实际实现中，这里会执行到下一个可调试的语句
        this.sendEvent('stopped', {
            reason: 'step',
            threadId: 1
        });
    }

    /**
     * 处理步进请求
     */
    private handleStepInRequest(request: vscode.DebugProtocolRequest): void {
        this.log('步进');
        this.sendResponse(request, true);
        this.sendEvent('stopped', {
            reason: 'step',
            threadId: 1
        });
    }

    /**
     * 处理步出请求
     */
    private handleStepOutRequest(request: vscode.DebugProtocolRequest): void {
        this.log('步出');
        this.sendResponse(request, true);
        this.sendEvent('stopped', {
            reason: 'step',
            threadId: 1
        });
    }

    /**
     * 处理暂停请求
     */
    private handlePauseRequest(request: vscode.DebugProtocolRequest): void {
        this.log('暂停执行');
        
        if (this.debugSession) {
            this.debugSession.status = 'paused';
        }
        
        this.sendResponse(request, true);
        this.sendEvent('stopped', {
            reason: 'pause',
            threadId: 1
        });
    }

    /**
     * 处理堆栈跟踪请求
     */
    private handleStackTraceRequest(request: vscode.DebugProtocolRequest): void {
        this.log('获取堆栈跟踪');
        
        const stackFrames = this.stackFrames.map(frame => ({
            id: frame.id,
            name: frame.name,
            source: {
                name: require('path').basename(frame.file),
                path: frame.file
            },
            line: frame.line,
            column: frame.column,
            presentationHint: this.getFramePresentationHint(frame.context)
        }));
        
        this.sendResponse(request, true, {
            stackFrames: stackFrames,
            totalFrames: stackFrames.length
        });
    }

    /**
     * 处理作用域请求
     */
    private handleScopesRequest(request: vscode.DebugProtocolRequest): void {
        this.log('获取作用域');
        
        const scopes = [
            {
                name: 'Local',
                variablesReference: 1,
                expensive: false
            },
            {
                name: 'Global',
                variablesReference: 2,
                expensive: false
            },
            {
                name: 'CHTL Context',
                variablesReference: 3,
                expensive: false
            }
        ];
        
        this.sendResponse(request, true, {
            scopes: scopes
        });
    }

    /**
     * 处理变量请求
     */
    private handleVariablesRequest(request: vscode.DebugProtocolRequest): void {
        const variablesReference = request.arguments.variablesReference;
        this.log(`获取变量: ${variablesReference}`);
        
        let variables: any[] = [];
        
        switch (variablesReference) {
            case 1: // Local
                variables = this.getLocalVariables();
                break;
            case 2: // Global
                variables = this.getGlobalVariables();
                break;
            case 3: // CHTL Context
                variables = this.getCHTLContextVariables();
                break;
        }
        
        this.sendResponse(request, true, {
            variables: variables
        });
    }

    /**
     * 处理表达式求值请求
     */
    private handleEvaluateRequest(request: vscode.DebugProtocolRequest): void {
        const expression = request.arguments.expression;
        this.log(`求值表达式: ${expression}`);
        
        try {
            const result = this.evaluateExpression(expression);
            this.sendResponse(request, true, {
                result: result.value,
                type: result.type,
                variablesReference: 0
            });
        } catch (error) {
            this.sendErrorResponse(request, `表达式求值失败: ${error}`);
        }
    }

    /**
     * 处理断开连接请求
     */
    private handleDisconnectRequest(request: vscode.DebugProtocolRequest): void {
        this.log('断开调试连接');
        this.stopDebugging();
        this.sendResponse(request, true);
        this.sendEvent('terminated');
    }

    /**
     * 编译程序
     */
    private async compileProgram(program: string): Promise<void> {
        this.log(`编译程序: ${program}`);
        
        try {
            const result = await this.compilerService.compileFile(program);
            
            if (result.success) {
                this.log('编译成功');
                this.outputChannel.appendLine(`✓ 编译成功: ${program}`);
            } else {
                this.log(`编译失败: ${result.errors.map(e => e.message).join(', ')}`);
                this.outputChannel.appendLine(`✗ 编译失败: ${program}`);
                
                for (const error of result.errors) {
                    this.outputChannel.appendLine(`  ${error.message} (${error.line}:${error.column})`);
                }
                
                throw new Error('编译失败');
            }
        } catch (error) {
            this.outputChannel.appendLine(`✗ 编译异常: ${error}`);
            throw error;
        }
    }

    /**
     * 设置编译监听器
     */
    private setupCompilationWatcher(program: string): void {
        if (this.compilationWatcher) {
            this.compilationWatcher.dispose();
        }
        
        const workspaceFolder = vscode.workspace.getWorkspaceFolder(vscode.Uri.file(program));
        if (!workspaceFolder) {
            return;
        }
        
        this.compilationWatcher = vscode.workspace.createFileSystemWatcher(
            new vscode.RelativePattern(workspaceFolder, '**/*.{chtl,cjmod}')
        );
        
        this.compilationWatcher.onDidChange(async (uri) => {
            this.log(`文件变化: ${uri.fsPath}`);
            this.outputChannel.appendLine(`🔄 检测到文件变化: ${uri.fsPath}`);
            
            try {
                await this.compileProgram(uri.fsPath);
                this.sendEvent('output', {
                    category: 'console',
                    output: `重新编译完成: ${require('path').basename(uri.fsPath)}\n`
                });
            } catch (error) {
                this.sendEvent('output', {
                    category: 'stderr',
                    output: `重新编译失败: ${error}\n`
                });
            }
        });
    }

    /**
     * 启动调试
     */
    private startDebugging(): void {
        this.log('启动调试监控');
        
        // 初始化堆栈帧
        this.stackFrames = [
            {
                id: 1,
                name: 'main',
                file: this.debugSession?.configuration.program || '',
                line: 1,
                column: 1,
                context: 'chtl'
            }
        ];
        
        // 显示输出面板
        this.outputChannel.show(true);
        this.outputChannel.appendLine('=== CHTL Debug Session Started ===');
        this.outputChannel.appendLine(`Program: ${this.debugSession?.configuration.program}`);
        this.outputChannel.appendLine(`Time: ${new Date().toLocaleString()}`);
        this.outputChannel.appendLine('');
    }

    /**
     * 停止调试
     */
    private stopDebugging(): void {
        this.log('停止调试会话');
        
        if (this.debugSession) {
            this.debugSession.status = 'stopped';
            this.outputChannel.appendLine('=== CHTL Debug Session Ended ===');
            this.outputChannel.appendLine(`Duration: ${Date.now() - this.debugSession.started.getTime()}ms`);
            this.outputChannel.appendLine('');
        }
        
        this.debugSession = undefined;
        this.breakpoints.clear();
        this.stackFrames = [];
        
        if (this.compilationWatcher) {
            this.compilationWatcher.dispose();
            this.compilationWatcher = undefined;
        }
    }

    /**
     * 验证断点
     */
    private verifyBreakpoint(breakpoint: CHTLBreakpoint): boolean {
        // 简化的断点验证逻辑
        // 实际实现中需要检查断点位置是否有效
        return breakpoint.line > 0;
    }

    /**
     * 获取帧展示提示
     */
    private getFramePresentationHint(context: string): string {
        switch (context) {
            case 'chtl':
                return 'normal';
            case 'css':
                return 'subtle';
            case 'javascript':
                return 'emphasis';
            case 'template':
                return 'normal';
            default:
                return 'normal';
        }
    }

    /**
     * 获取局部变量
     */
    private getLocalVariables(): any[] {
        return [
            {
                name: 'current_template',
                value: 'myTemplate',
                type: 'string',
                variablesReference: 0
            },
            {
                name: 'current_element',
                value: 'div',
                type: 'string',
                variablesReference: 0
            }
        ];
    }

    /**
     * 获取全局变量
     */
    private getGlobalVariables(): any[] {
        return [
            {
                name: 'CHTL_VERSION',
                value: '1.0.0',
                type: 'string',
                variablesReference: 0
            },
            {
                name: 'DEBUG_MODE',
                value: 'true',
                type: 'boolean',
                variablesReference: 0
            }
        ];
    }

    /**
     * 获取CHTL上下文变量
     */
    private getCHTLContextVariables(): any[] {
        return [
            {
                name: 'templates',
                value: '{}',
                type: 'object',
                variablesReference: 4
            },
            {
                name: 'imports',
                value: '[]',
                type: 'array',
                variablesReference: 5
            }
        ];
    }

    /**
     * 表达式求值
     */
    private evaluateExpression(expression: string): { value: string; type: string } {
        // 简化的表达式求值
        // 实际实现中需要解析和执行CHTL表达式
        
        if (expression === 'current_template') {
            return { value: 'myTemplate', type: 'string' };
        } else if (expression === 'DEBUG_MODE') {
            return { value: 'true', type: 'boolean' };
        } else {
            return { value: `Unknown: ${expression}`, type: 'string' };
        }
    }

    /**
     * 发送消息
     */
    private sendMessage(message: vscode.DebugProtocolMessage): void {
        this._onDidSendMessage.fire(message);
    }

    /**
     * 发送响应
     */
    private sendResponse(request: vscode.DebugProtocolRequest, success: boolean, body?: any): void {
        const response: vscode.DebugProtocolResponse = {
            type: 'response',
            request_seq: request.seq,
            seq: this.sequence++,
            command: request.command,
            success: success,
            body: body
        };
        
        this.sendMessage(response);
    }

    /**
     * 发送错误响应
     */
    private sendErrorResponse(request: vscode.DebugProtocolRequest, errorMessage: string): void {
        const response: vscode.DebugProtocolResponse = {
            type: 'response',
            request_seq: request.seq,
            seq: this.sequence++,
            command: request.command,
            success: false,
            message: errorMessage
        };
        
        this.sendMessage(response);
        this.log(`错误: ${errorMessage}`);
    }

    /**
     * 发送事件
     */
    private sendEvent(event: string, body?: any): void {
        const message: vscode.DebugProtocolEvent = {
            type: 'event',
            seq: this.sequence++,
            event: event,
            body: body
        };
        
        this.sendMessage(message);
    }

    /**
     * 记录日志
     */
    private log(message: string): void {
        const timestamp = new Date().toLocaleTimeString();
        console.log(`[${timestamp}] CHTL Debug: ${message}`);
    }
}