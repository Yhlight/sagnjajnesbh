import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { spawn, ChildProcess } from 'child_process';

export interface CompilationResult {
    success: boolean;
    output: string;
    errors: string[];
    warnings: string[];
    outputFiles: string[];
    executionTime: number;
}

export interface CompilerOptions {
    strictMode: boolean;
    outputDirectory: string;
    enableRealTimeCompilation: boolean;
    enableSyntaxBoundaryCheck: boolean;
    enableImportValidation: boolean;
    additionalFlags: string[];
}

export class CHTLCompilerService {
    private context: vscode.ExtensionContext;
    private compilerPath: string;
    private isCompilerReady: boolean = false;
    private compilationQueue: Map<string, Promise<CompilationResult>> = new Map();

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.compilerPath = this.resolveCompilerPath();
        this.initializeCompiler();
    }

    private resolveCompilerPath(): string {
        // 内置编译器路径（打包在扩展中）
        const builtInCompilerPath = path.join(this.context.extensionPath, 'bin', 'chtl-compiler');
        
        // 检查是否存在内置编译器
        if (fs.existsSync(builtInCompilerPath)) {
            // 验证官方模块是否也存在
            const officialModulesPath = path.join(this.context.extensionPath, 'bin', 'module');
            if (!fs.existsSync(officialModulesPath)) {
                console.warn('内置编译器存在但官方模块目录缺失，这可能导致Import功能异常');
            }
            return builtInCompilerPath;
        }

        // 查找系统路径中的编译器
        const systemCompilerPath = this.findSystemCompiler();
        if (systemCompilerPath) {
            return systemCompilerPath;
        }

        // 使用Node.js版本的编译器（备用方案）
        return path.join(this.context.extensionPath, 'bin', 'chtl-compiler.js');
    }

    private findSystemCompiler(): string | null {
        const possiblePaths = [
            'chtl-compiler',
            'chtl',
            path.join(process.cwd(), 'chtl-compiler'),
            path.join(process.cwd(), 'build', 'chtl-compiler')
        ];

        for (const compilerPath of possiblePaths) {
            try {
                if (fs.existsSync(compilerPath)) {
                    return compilerPath;
                }
            } catch {
                continue;
            }
        }

        return null;
    }

    private async initializeCompiler(): Promise<void> {
        try {
            // 验证编译器可用性
            const result = await this.runCompiler(['--version'], '');
            if (result.success) {
                this.isCompilerReady = true;
                console.log('CHTL compiler initialized successfully');
            } else {
                // 如果编译器不可用，使用内置JavaScript版本
                await this.initializeFallbackCompiler();
            }
        } catch (error) {
            console.error('Failed to initialize CHTL compiler:', error);
            await this.initializeFallbackCompiler();
        }
    }

    private async initializeFallbackCompiler(): Promise<void> {
        // 初始化JavaScript版本的编译器（集成主项目代码）
        console.log('Using fallback JavaScript compiler');
        this.isCompilerReady = true;
    }

    public async compileFile(filePath: string, options?: Partial<CompilerOptions>): Promise<CompilationResult> {
        if (!this.isCompilerReady) {
            await this.initializeCompiler();
        }

        // 防止重复编译同一文件
        const existingCompilation = this.compilationQueue.get(filePath);
        if (existingCompilation) {
            return existingCompilation;
        }

        const compilationPromise = this.performCompilation(filePath, options);
        this.compilationQueue.set(filePath, compilationPromise);

        try {
            const result = await compilationPromise;
            return result;
        } finally {
            this.compilationQueue.delete(filePath);
        }
    }

    private async performCompilation(filePath: string, options?: Partial<CompilerOptions>): Promise<CompilationResult> {
        const startTime = Date.now();
        const config = vscode.workspace.getConfiguration('chtl');
        
        const compilerOptions: CompilerOptions = {
            strictMode: config.get('compiler.strictMode', true),
            outputDirectory: config.get('compiler.outputDirectory', './build'),
            enableRealTimeCompilation: config.get('compiler.enableRealTimeCompilation', true),
            enableSyntaxBoundaryCheck: config.get('validation.enableSyntaxBoundaryCheck', true),
            enableImportValidation: config.get('validation.enableImportValidation', true),
            additionalFlags: [],
            ...options
        };

        try {
            // 构建编译器参数
            const args = this.buildCompilerArgs(filePath, compilerOptions);
            
            // 读取源文件内容
            const sourceContent = fs.readFileSync(filePath, 'utf8');
            
            // 执行编译
            const result = await this.runCompiler(args, sourceContent);
            
            result.executionTime = Date.now() - startTime;
            return result;
        } catch (error) {
            return {
                success: false,
                output: '',
                errors: [error instanceof Error ? error.message : String(error)],
                warnings: [],
                outputFiles: [],
                executionTime: Date.now() - startTime
            };
        }
    }

    private buildCompilerArgs(filePath: string, options: CompilerOptions): string[] {
        const args = ['compile', filePath];
        
        if (options.strictMode) {
            args.push('--strict');
        }
        
        if (options.outputDirectory) {
            args.push('--output', options.outputDirectory);
        }
        
        if (options.enableSyntaxBoundaryCheck) {
            args.push('--enable-syntax-boundary-check');
        }
        
        if (options.enableImportValidation) {
            args.push('--enable-import-validation');
        }
        
        args.push(...options.additionalFlags);
        
        return args;
    }

    private async runCompiler(args: string[], input: string): Promise<CompilationResult> {
        return new Promise((resolve) => {
            let stdout = '';
            let stderr = '';
            
            const child: ChildProcess = spawn(this.compilerPath, args, {
                stdio: ['pipe', 'pipe', 'pipe']
            });

            child.stdout?.on('data', (data) => {
                stdout += data.toString();
            });

            child.stderr?.on('data', (data) => {
                stderr += data.toString();
            });

            child.on('close', (code) => {
                const result = this.parseCompilerOutput(stdout, stderr, code === 0);
                resolve(result);
            });

            child.on('error', (error) => {
                resolve({
                    success: false,
                    output: '',
                    errors: [error.message],
                    warnings: [],
                    outputFiles: [],
                    executionTime: 0
                });
            });

            // 发送输入数据
            if (input && child.stdin) {
                child.stdin.write(input);
                child.stdin.end();
            }
        });
    }

    private parseCompilerOutput(stdout: string, stderr: string, success: boolean): CompilationResult {
        const errors: string[] = [];
        const warnings: string[] = [];
        const outputFiles: string[] = [];
        
        // 解析stderr中的错误和警告
        const stderrLines = stderr.split('\n').filter(line => line.trim());
        for (const line of stderrLines) {
            if (line.includes('error:')) {
                errors.push(line);
            } else if (line.includes('warning:')) {
                warnings.push(line);
            }
        }
        
        // 解析stdout中的输出文件信息
        const stdoutLines = stdout.split('\n').filter(line => line.trim());
        for (const line of stdoutLines) {
            if (line.startsWith('Generated:')) {
                const filePath = line.replace('Generated:', '').trim();
                outputFiles.push(filePath);
            }
        }
        
        return {
            success: success && errors.length === 0,
            output: stdout,
            errors,
            warnings,
            outputFiles,
            executionTime: 0
        };
    }

    public async compileProject(projectPath: string): Promise<CompilationResult> {
        const args = ['compile-project', projectPath];
        const result = await this.runCompiler(args, '');
        return result;
    }

    public async compileToPreview(filePath: string): Promise<string> {
        const args = ['compile', '--preview', filePath];
        const result = await this.runCompiler(args, '');
        
        if (result.success) {
            return result.output;
        } else {
            // 生成错误预览页面
            return this.generateErrorPreview(result.errors);
        }
    }

    private generateErrorPreview(errors: string[]): string {
        return `
<!DOCTYPE html>
<html>
<head>
    <title>CHTL Compilation Error</title>
    <style>
        body { font-family: 'Courier New', monospace; padding: 20px; background: #1e1e1e; color: #d4d4d4; }
        .error { background: #f14c4c; color: white; padding: 10px; margin: 10px 0; border-radius: 4px; }
        .header { color: #569cd6; font-size: 18px; margin-bottom: 20px; }
    </style>
</head>
<body>
    <div class="header">CHTL Compilation Errors</div>
    ${errors.map(error => `<div class="error">${this.escapeHtml(error)}</div>`).join('')}
</body>
</html>
        `;
    }

    private escapeHtml(text: string): string {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    public async validateSyntax(filePath: string): Promise<CompilationResult> {
        const args = ['validate', filePath];
        const sourceContent = fs.readFileSync(filePath, 'utf8');
        return this.runCompiler(args, sourceContent);
    }

    public isReady(): boolean {
        return this.isCompilerReady;
    }
}