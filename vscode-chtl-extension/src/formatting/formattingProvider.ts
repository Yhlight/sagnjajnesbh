import * as vscode from 'vscode';

export interface CHTLFormattingOptions {
    insertSpaces: boolean;
    tabSize: number;
    indentSize: number;
    maxLineLength: number;
    insertFinalNewline: boolean;
    trimTrailingWhitespace: boolean;
    preserveEmptyLines: boolean;
    alignKeyValuePairs: boolean;
    sortImports: boolean;
    compactArrays: boolean;
}

/**
 * CHTL格式化提供器
 * 提供CHTL和CHTL JS代码的自动格式化功能
 */
export class CHTLFormattingProvider implements vscode.DocumentFormattingEditProvider, vscode.DocumentRangeFormattingEditProvider {
    
    /**
     * 格式化整个文档
     */
    public provideDocumentFormattingEdits(
        document: vscode.TextDocument,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): vscode.TextEdit[] {
        const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(document.getText().length)
        );
        
        return this.provideDocumentRangeFormattingEdits(document, fullRange, options, token);
    }

    /**
     * 格式化文档范围
     */
    public provideDocumentRangeFormattingEdits(
        document: vscode.TextDocument,
        range: vscode.Range,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): vscode.TextEdit[] {
        const edits: vscode.TextEdit[] = [];
        const config = this.getFormattingOptions(options);
        
        try {
            const text = document.getText(range);
            const lines = text.split('\n');
            const formattedLines = this.formatLines(lines, config, document.languageId);
            const formattedText = formattedLines.join('\n');
            
            if (text !== formattedText) {
                edits.push(vscode.TextEdit.replace(range, formattedText));
            }
            
        } catch (error) {
            console.error('CHTL formatting error:', error);
        }
        
        return edits;
    }

    /**
     * 获取格式化选项
     */
    private getFormattingOptions(options: vscode.FormattingOptions): CHTLFormattingOptions {
        const config = vscode.workspace.getConfiguration('chtl.format');
        
        return {
            insertSpaces: options.insertSpaces,
            tabSize: options.tabSize,
            indentSize: config.get<number>('indentSize', options.tabSize),
            maxLineLength: config.get<number>('maxLineLength', 120),
            insertFinalNewline: config.get<boolean>('insertFinalNewline', true),
            trimTrailingWhitespace: config.get<boolean>('trimTrailingWhitespace', true),
            preserveEmptyLines: config.get<boolean>('preserveEmptyLines', true),
            alignKeyValuePairs: config.get<boolean>('alignKeyValuePairs', true),
            sortImports: config.get<boolean>('sortImports', false),
            compactArrays: config.get<boolean>('compactArrays', false)
        };
    }

    /**
     * 格式化行数组
     */
    private formatLines(lines: string[], config: CHTLFormattingOptions, languageId: string): string[] {
        const formatter = languageId === 'chtl-js' ? 
            new CHTLJSFormatter(config) : 
            new CHTLFormatter(config);
        
        return formatter.format(lines);
    }
}

/**
 * CHTL格式化器
 */
class CHTLFormatter {
    private config: CHTLFormattingOptions;
    private indentLevel: number = 0;
    private inMultiLineComment: boolean = false;
    private currentContext: 'root' | 'template' | 'custom' | 'style' | 'script' | 'config' | 'namespace' | 'import' = 'root';

    constructor(config: CHTLFormattingOptions) {
        this.config = config;
    }

    public format(lines: string[]): string[] {
        const formattedLines: string[] = [];
        let pendingEmptyLines = 0;

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const trimmedLine = line.trim();

            // 处理空行
            if (trimmedLine === '') {
                if (this.config.preserveEmptyLines) {
                    pendingEmptyLines++;
                }
                continue;
            }

            // 添加待处理的空行（最多保留一个）
            if (pendingEmptyLines > 0 && this.config.preserveEmptyLines) {
                formattedLines.push('');
                pendingEmptyLines = 0;
            }

            // 格式化当前行
            const formattedLine = this.formatLine(trimmedLine, i, lines);
            formattedLines.push(formattedLine);
        }

        // 处理最终换行
        if (this.config.insertFinalNewline && formattedLines.length > 0) {
            const lastLine = formattedLines[formattedLines.length - 1];
            if (lastLine.trim() !== '') {
                formattedLines.push('');
            }
        }

        return formattedLines;
    }

    private formatLine(line: string, lineIndex: number, allLines: string[]): string {
        // 处理多行注释
        if (this.inMultiLineComment) {
            if (line.includes('*/')) {
                this.inMultiLineComment = false;
            }
            return this.indent() + line;
        }

        if (line.includes('/*')) {
            this.inMultiLineComment = true;
            if (!line.includes('*/')) {
                return this.indent() + line;
            }
        }

        // 处理单行注释
        if (line.startsWith('//')) {
            return this.indent() + line;
        }

        // 检测上下文变化
        this.updateContext(line);

        // 处理特殊语法结构
        if (this.isBlockDeclaration(line)) {
            return this.formatBlockDeclaration(line);
        }

        if (this.isStyleBlock(line)) {
            return this.formatStyleBlock(line);
        }

        if (this.isScriptBlock(line)) {
            return this.formatScriptBlock(line);
        }

        if (this.isKeyValuePair(line)) {
            return this.formatKeyValuePair(line);
        }

        // 处理花括号
        if (line.endsWith('{')) {
            const result = this.indent() + line;
            this.indentLevel++;
            return result;
        }

        if (line === '}') {
            this.indentLevel = Math.max(0, this.indentLevel - 1);
            return this.indent() + line;
        }

        if (line.startsWith('}')) {
            this.indentLevel = Math.max(0, this.indentLevel - 1);
            const result = this.indent() + line;
            if (line.length > 1) {
                this.indentLevel++;
            }
            return result;
        }

        // 默认缩进
        return this.indent() + line;
    }

    private updateContext(line: string): void {
        if (line.includes('[Template]')) {
            this.currentContext = 'template';
        } else if (line.includes('[Custom]')) {
            this.currentContext = 'custom';
        } else if (line.includes('[Configuration]')) {
            this.currentContext = 'config';
        } else if (line.includes('[Namespace]')) {
            this.currentContext = 'namespace';
        } else if (line.includes('[Import]')) {
            this.currentContext = 'import';
        } else if (line.includes('style {')) {
            this.currentContext = 'style';
        } else if (line.includes('script {')) {
            this.currentContext = 'script';
        }
    }

    private isBlockDeclaration(line: string): boolean {
        return /\[(Template|Custom|Configuration|Namespace|Import)\]/.test(line);
    }

    private isStyleBlock(line: string): boolean {
        return line.includes('style {');
    }

    private isScriptBlock(line: string): boolean {
        return line.includes('script {');
    }

    private isKeyValuePair(line: string): boolean {
        return /\w+\s*[=:]\s*.+/.test(line) && !line.includes('{') && !line.includes('}');
    }

    private formatBlockDeclaration(line: string): string {
        // 格式化块声明，确保正确的间距
        let formatted = line.replace(/\s+/g, ' ');
        
        // 处理类型标识符的格式
        formatted = formatted.replace(/@(\w+)\s+(\w+)/, '@$1 $2');
        
        return this.indent() + formatted;
    }

    private formatStyleBlock(line: string): string {
        const result = this.indent() + 'style {';
        this.indentLevel++;
        return result;
    }

    private formatScriptBlock(line: string): string {
        const result = this.indent() + 'script {';
        this.indentLevel++;
        return result;
    }

    private formatKeyValuePair(line: string): string {
        if (!this.config.alignKeyValuePairs) {
            return this.indent() + line;
        }

        // 对齐键值对
        const match = line.match(/^(\w+)\s*([=:])\s*(.+)$/);
        if (match) {
            const [, key, operator, value] = match;
            return this.indent() + `${key} ${operator} ${value}`;
        }

        return this.indent() + line;
    }

    private indent(): string {
        const indentChar = this.config.insertSpaces ? ' ' : '\t';
        const indentSize = this.config.insertSpaces ? this.config.indentSize : 1;
        return indentChar.repeat(this.indentLevel * indentSize);
    }
}

/**
 * CHTL JS格式化器
 */
class CHTLJSFormatter {
    private config: CHTLFormattingOptions;
    private indentLevel: number = 0;
    private inCHTLJSBlock: boolean = false;

    constructor(config: CHTLFormattingOptions) {
        this.config = config;
    }

    public format(lines: string[]): string[] {
        const formattedLines: string[] = [];
        let pendingEmptyLines = 0;

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const trimmedLine = line.trim();

            // 处理空行
            if (trimmedLine === '') {
                if (this.config.preserveEmptyLines) {
                    pendingEmptyLines++;
                }
                continue;
            }

            // 添加待处理的空行
            if (pendingEmptyLines > 0 && this.config.preserveEmptyLines) {
                formattedLines.push('');
                pendingEmptyLines = 0;
            }

            // 格式化当前行
            const formattedLine = this.formatLine(trimmedLine, i, lines);
            formattedLines.push(formattedLine);
        }

        // 处理最终换行
        if (this.config.insertFinalNewline && formattedLines.length > 0) {
            const lastLine = formattedLines[formattedLines.length - 1];
            if (lastLine.trim() !== '') {
                formattedLines.push('');
            }
        }

        return formattedLines;
    }

    private formatLine(line: string, lineIndex: number, allLines: string[]): string {
        // 处理CHTL JS表达式
        if (line.includes('{{') || line.includes('}}')) {
            return this.formatCHTLJSExpression(line);
        }

        // 处理CHTL JS函数
        if (this.isCHTLJSFunction(line)) {
            return this.formatCHTLJSFunction(line);
        }

        // 处理虚对象
        if (line.includes('vir {') || this.isInVirObject(line)) {
            return this.formatVirtualObject(line);
        }

        // 处理键值对
        if (this.isKeyValuePair(line)) {
            return this.formatKeyValuePair(line);
        }

        // 处理花括号
        if (line.endsWith('{')) {
            const result = this.indent() + line;
            this.indentLevel++;
            return result;
        }

        if (line === '}') {
            this.indentLevel = Math.max(0, this.indentLevel - 1);
            return this.indent() + line;
        }

        if (line.startsWith('}')) {
            this.indentLevel = Math.max(0, this.indentLevel - 1);
            const result = this.indent() + line;
            if (line.length > 1) {
                this.indentLevel++;
            }
            return result;
        }

        // 处理JavaScript代码
        return this.formatJavaScript(line);
    }

    private formatCHTLJSExpression(line: string): string {
        // 格式化{{...}}表达式
        let formatted = line;
        
        // 确保{{和}}周围有适当的空格
        formatted = formatted.replace(/\{\{([^}]+)\}\}/g, (match, content) => {
            const trimmedContent = content.trim();
            return `{{ ${trimmedContent} }}`;
        });

        return this.indent() + formatted;
    }

    private formatCHTLJSFunction(line: string): string {
        // 格式化CHTL JS函数调用
        const match = line.match(/^(\w+)\s*\{/);
        if (match) {
            const functionName = match[1];
            const result = this.indent() + `${functionName} {`;
            this.indentLevel++;
            return result;
        }

        return this.indent() + line;
    }

    private formatVirtualObject(line: string): string {
        if (line.includes('vir {')) {
            const result = this.indent() + 'vir {';
            this.indentLevel++;
            return result;
        }

        return this.indent() + line;
    }

    private formatKeyValuePair(line: string): string {
        if (!this.config.alignKeyValuePairs) {
            return this.indent() + line;
        }

        // 处理CHTL JS的键值对格式
        const match = line.match(/^(\w+)\s*:\s*(.+)$/);
        if (match) {
            const [, key, value] = match;
            return this.indent() + `${key}: ${value}`;
        }

        return this.indent() + line;
    }

    private formatJavaScript(line: string): string {
        // 简单的JavaScript格式化
        let formatted = line;

        // 处理操作符周围的空格
        formatted = formatted.replace(/([+\-*/%=!<>]+)/g, ' $1 ');
        formatted = formatted.replace(/\s+/g, ' ');
        formatted = formatted.trim();

        // 处理逗号后的空格
        formatted = formatted.replace(/,(?!\s)/g, ', ');

        // 处理分号
        if (formatted.endsWith(',') || formatted.endsWith(';')) {
            // 保持末尾标点符号
        } else if (this.needsSemicolon(formatted)) {
            formatted += ';';
        }

        return this.indent() + formatted;
    }

    private isCHTLJSFunction(line: string): boolean {
        return /^\w+\s*\{/.test(line) && 
               (line.includes('iNeverAway') || 
                line.includes('printMylove') || 
                /^[a-zA-Z_$][a-zA-Z0-9_$]*\s*\{/.test(line));
    }

    private isInVirObject(line: string): boolean {
        // 简化的检测逻辑，实际实现中需要维护状态
        return this.inCHTLJSBlock && line.includes(':');
    }

    private isKeyValuePair(line: string): boolean {
        return /\w+\s*:\s*.+/.test(line) && !line.includes('{') && !line.includes('}');
    }

    private needsSemicolon(line: string): boolean {
        // 检查是否需要分号
        const noSemicolonPatterns = [
            /^\s*\/\//, // 注释
            /^\s*\/\*/, // 多行注释开始
            /\*\/\s*$/, // 多行注释结束
            /[{}]\s*$/, // 花括号结尾
            /:\s*$/, // 冒号结尾（键值对）
            /,\s*$/, // 逗号结尾
        ];

        return !noSemicolonPatterns.some(pattern => pattern.test(line));
    }

    private indent(): string {
        const indentChar = this.config.insertSpaces ? ' ' : '\t';
        const indentSize = this.config.insertSpaces ? this.config.indentSize : 1;
        return indentChar.repeat(this.indentLevel * indentSize);
    }
}