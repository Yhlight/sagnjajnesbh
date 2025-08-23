import * as vscode from 'vscode';

export type ConflictResolutionStrategy = 'prefer-chtl' | 'prefer-vscode' | 'smart-detection';

export interface ConflictDetectionResult {
    isInCHTLContext: boolean;
    contextType: 'chtl-syntax' | 'css-in-style' | 'js-in-script' | 'html-element' | 'unknown';
    confidence: number; // 0-1，检测结果的置信度
    shouldBlockVSCodeCompletion: boolean;
    reason: string;
}

/**
 * CHTL语法冲突解决器
 * 智能检测当前位置是否应该使用CHTL语法提示还是VSCode默认提示
 */
export class CHTLConflictResolver {
    private config: vscode.WorkspaceConfiguration;
    private strategy: ConflictResolutionStrategy;
    
    // CHTL特有的语法模式
    private readonly CHTL_SYNTAX_PATTERNS = [
        /\[Template\]/,
        /\[Custom\]/,
        /\[Configuration\]/,
        /\[Namespace\]/,
        /\[Import\]/,
        /@(Style|Element|Var|Html|JavaScript|Chtl|CJmod)/,
        /\{\{[^}]*\}\}/, // CHTL JS语法
        /inherit\s+/,
        /delete\s+/,
        /\&(?!\w)/, // & 引用选择器（非实体引用）
        /\[Origin\]/
    ];

    // CSS语法模式（在style块中应该使用VSCode的CSS提示）
    private readonly CSS_SYNTAX_PATTERNS = [
        /[\w-]+\s*:\s*[^;]*;?/, // CSS属性
        /\.([\w-]+)/, // CSS类选择器
        /#([\w-]+)/, // CSS ID选择器
        /@media\s+/, // CSS媒体查询
        /@keyframes\s+/, // CSS关键帧
    ];

    // JavaScript语法模式（在script块中应该使用VSCode的JS提示）
    private readonly JS_SYNTAX_PATTERNS = [
        /function\s+\w+/, // 函数声明
        /var\s+\w+/, // 变量声明
        /let\s+\w+/, // let声明
        /const\s+\w+/, // const声明
        /if\s*\(/, // if语句
        /for\s*\(/, // for循环
        /while\s*\(/, // while循环
        /\.\w+\(/, // 方法调用
    ];

    // HTML元素模式
    private readonly HTML_ELEMENT_PATTERNS = [
        /<\w+/, // HTML标签开始
        /\w+\s*\{/, // 元素块开始
        /(div|span|button|input|form|img|a|p|h[1-6]|ul|ol|li|table|tr|td|th)\s*\{/, // 常见HTML元素
    ];

    constructor(config: vscode.WorkspaceConfiguration) {
        this.config = config;
        this.strategy = config.get<ConflictResolutionStrategy>('intellisense.conflictResolution', 'smart-detection');
    }

    public updateConfig(newConfig: vscode.WorkspaceConfiguration): void {
        this.config = newConfig;
        this.strategy = newConfig.get<ConflictResolutionStrategy>('intellisense.conflictResolution', 'smart-detection');
    }

    /**
     * 检测当前位置是否在CHTL上下文中
     */
    public async detectCHTLContext(document: vscode.TextDocument, position: vscode.Position): Promise<boolean> {
        const result = await this.analyzeContext(document, position);
        
        switch (this.strategy) {
            case 'prefer-chtl':
                return true; // 总是使用CHTL提示
            case 'prefer-vscode':
                return false; // 总是使用VSCode提示
            case 'smart-detection':
            default:
                return result.shouldBlockVSCodeCompletion;
        }
    }

    /**
     * 深度分析当前上下文
     */
    public async analyzeContext(document: vscode.TextDocument, position: vscode.Position): Promise<ConflictDetectionResult> {
        const line = document.lineAt(position).text;
        const beforeCursor = line.substring(0, position.character);
        const afterCursor = line.substring(position.character);
        
        // 1. 检查是否在注释中
        if (this.isInComment(document, position)) {
            return {
                isInCHTLContext: false,
                contextType: 'unknown',
                confidence: 0.9,
                shouldBlockVSCodeCompletion: false,
                reason: '在注释中，使用默认补全'
            };
        }

        // 2. 检查是否在字符串字面量中
        if (this.isInStringLiteral(beforeCursor)) {
            return {
                isInCHTLContext: false,
                contextType: 'unknown',
                confidence: 0.8,
                shouldBlockVSCodeCompletion: false,
                reason: '在字符串字面量中，使用默认补全'
            };
        }

        // 3. 分析当前所在的块类型
        const blockContext = this.analyzeBlockContext(document, position);
        
        // 4. 检查CHTL特有语法
        const chtlSyntaxMatch = this.matchesCHTLSyntax(line, beforeCursor);
        if (chtlSyntaxMatch.matches) {
            return {
                isInCHTLContext: true,
                contextType: 'chtl-syntax',
                confidence: chtlSyntaxMatch.confidence,
                shouldBlockVSCodeCompletion: true,
                reason: `检测到CHTL语法: ${chtlSyntaxMatch.pattern}`
            };
        }

        // 5. 根据块类型决定使用哪种补全
        switch (blockContext.blockType) {
            case 'style':
                return this.analyzeStyleBlockContext(line, beforeCursor, blockContext);
            case 'script':
                return this.analyzeScriptBlockContext(line, beforeCursor, blockContext);
            case 'template':
            case 'custom':
                return this.analyzeTemplateCustomContext(line, beforeCursor, blockContext);
            case 'configuration':
            case 'namespace':
            case 'import':
                return {
                    isInCHTLContext: true,
                    contextType: 'chtl-syntax',
                    confidence: 0.9,
                    shouldBlockVSCodeCompletion: true,
                    reason: `在${blockContext.blockType}块中，使用CHTL补全`
                };
            default:
                return this.analyzeRootContext(line, beforeCursor);
        }
    }

    private isInComment(document: vscode.TextDocument, position: vscode.Position): boolean {
        const line = document.lineAt(position).text;
        const beforeCursor = line.substring(0, position.character);
        
        // 单行注释
        const singleLineComment = line.indexOf('//');
        if (singleLineComment !== -1 && position.character > singleLineComment) {
            return true;
        }

        // 多行注释（简化检测）
        if (beforeCursor.includes('/*') && !beforeCursor.includes('*/')) {
            return true;
        }

        return false;
    }

    private isInStringLiteral(beforeCursor: string): boolean {
        // 检查是否在引号内
        const singleQuotes = (beforeCursor.match(/'/g) || []).length;
        const doubleQuotes = (beforeCursor.match(/"/g) || []).length;
        const backQuotes = (beforeCursor.match(/`/g) || []).length;
        
        return (singleQuotes % 2 !== 0) || (doubleQuotes % 2 !== 0) || (backQuotes % 2 !== 0);
    }

    private analyzeBlockContext(document: vscode.TextDocument, position: vscode.Position): {
        blockType: string;
        blockName: string;
        nestingLevel: number;
    } {
        let blockType = 'root';
        let blockName = '';
        let nestingLevel = 0;
        let braceCount = 0;

        // 向上搜索找到当前块
        for (let i = position.line; i >= 0; i--) {
            const currentLine = document.lineAt(i).text.trim();
            
            // 计算花括号层级
            braceCount += (currentLine.match(/\{/g) || []).length;
            braceCount -= (currentLine.match(/\}/g) || []).length;
            
            if (currentLine.includes('[Template]')) {
                blockType = 'template';
                const match = currentLine.match(/\[Template\]\s*@(\w+)\s+(\w+)/);
                blockName = match ? match[2] : '';
                nestingLevel = braceCount;
                break;
            } else if (currentLine.includes('[Custom]')) {
                blockType = 'custom';
                const match = currentLine.match(/\[Custom\]\s*@(\w+)\s+(\w+)/);
                blockName = match ? match[2] : '';
                nestingLevel = braceCount;
                break;
            } else if (currentLine.includes('[Configuration]')) {
                blockType = 'configuration';
                nestingLevel = braceCount;
                break;
            } else if (currentLine.includes('[Namespace]')) {
                blockType = 'namespace';
                nestingLevel = braceCount;
                break;
            } else if (currentLine.includes('[Import]')) {
                blockType = 'import';
                break;
            } else if (currentLine.includes('style {')) {
                blockType = 'style';
                nestingLevel = braceCount;
                break;
            } else if (currentLine.includes('script {')) {
                blockType = 'script';
                nestingLevel = braceCount;
                break;
            }
        }

        return { blockType, blockName, nestingLevel };
    }

    private matchesCHTLSyntax(line: string, beforeCursor: string): { matches: boolean; confidence: number; pattern: string } {
        for (const pattern of this.CHTL_SYNTAX_PATTERNS) {
            if (pattern.test(line) || pattern.test(beforeCursor)) {
                return {
                    matches: true,
                    confidence: 0.9,
                    pattern: pattern.source
                };
            }
        }
        
        return { matches: false, confidence: 0, pattern: '' };
    }

    private analyzeStyleBlockContext(line: string, beforeCursor: string, blockContext: any): ConflictDetectionResult {
        // 在style块中，需要区分CHTL语法和CSS语法
        
        // 检查CHTL特有的style语法
        if (beforeCursor.includes('&') || beforeCursor.includes('inherit') || beforeCursor.includes('delete')) {
            return {
                isInCHTLContext: true,
                contextType: 'chtl-syntax',
                confidence: 0.95,
                shouldBlockVSCodeCompletion: true,
                reason: '检测到CHTL样式块特有语法'
            };
        }

        // 检查是否是CSS语法
        for (const pattern of this.CSS_SYNTAX_PATTERNS) {
            if (pattern.test(line) || pattern.test(beforeCursor)) {
                return {
                    isInCHTLContext: false,
                    contextType: 'css-in-style',
                    confidence: 0.8,
                    shouldBlockVSCodeCompletion: false,
                    reason: '检测到CSS语法，使用VSCode CSS补全'
                };
            }
        }

        // 默认在style块中使用CSS补全
        return {
            isInCHTLContext: false,
            contextType: 'css-in-style',
            confidence: 0.6,
            shouldBlockVSCodeCompletion: false,
            reason: '在style块中，默认使用CSS补全'
        };
    }

    private analyzeScriptBlockContext(line: string, beforeCursor: string, blockContext: any): ConflictDetectionResult {
        // 在script块中，需要区分CHTL JS语法和普通JavaScript语法
        
        // 检查CHTL JS特有语法
        if (beforeCursor.includes('{{') || beforeCursor.includes('}}')) {
            return {
                isInCHTLContext: true,
                contextType: 'chtl-syntax',
                confidence: 0.95,
                shouldBlockVSCodeCompletion: true,
                reason: '检测到CHTL JS语法'
            };
        }

        // 检查是否是纯JavaScript语法
        for (const pattern of this.JS_SYNTAX_PATTERNS) {
            if (pattern.test(line) || pattern.test(beforeCursor)) {
                return {
                    isInCHTLContext: false,
                    contextType: 'js-in-script',
                    confidence: 0.8,
                    shouldBlockVSCodeCompletion: false,
                    reason: '检测到JavaScript语法，使用VSCode JS补全'
                };
            }
        }

        // 在script块中，如果不确定，优先使用CHTL补全
        return {
            isInCHTLContext: true,
            contextType: 'chtl-syntax',
            confidence: 0.7,
            shouldBlockVSCodeCompletion: true,
            reason: '在script块中，优先使用CHTL补全'
        };
    }

    private analyzeTemplateCustomContext(line: string, beforeCursor: string, blockContext: any): ConflictDetectionResult {
        // 在Template或Custom块中，主要是CHTL语法，但也可能包含HTML元素

        // 检查HTML元素语法
        for (const pattern of this.HTML_ELEMENT_PATTERNS) {
            if (pattern.test(line) || pattern.test(beforeCursor)) {
                // 如果是在元素定义中，仍然使用CHTL补全，因为CHTL的元素语法与HTML不同
                return {
                    isInCHTLContext: true,
                    contextType: 'html-element',
                    confidence: 0.8,
                    shouldBlockVSCodeCompletion: true,
                    reason: '在Template/Custom块中检测到HTML元素，使用CHTL补全'
                };
            }
        }

        // 默认使用CHTL补全
        return {
            isInCHTLContext: true,
            contextType: 'chtl-syntax',
            confidence: 0.9,
            shouldBlockVSCodeCompletion: true,
            reason: '在Template/Custom块中，使用CHTL补全'
        };
    }

    private analyzeRootContext(line: string, beforeCursor: string): ConflictDetectionResult {
        // 在根级别，主要是CHTL声明语法

        // 检查是否正在输入CHTL声明
        if (beforeCursor.includes('[') || line.includes('@')) {
            return {
                isInCHTLContext: true,
                contextType: 'chtl-syntax',
                confidence: 0.9,
                shouldBlockVSCodeCompletion: true,
                reason: '检测到CHTL声明语法'
            };
        }

        // 检查HTML元素
        for (const pattern of this.HTML_ELEMENT_PATTERNS) {
            if (pattern.test(line) || pattern.test(beforeCursor)) {
                return {
                    isInCHTLContext: true,
                    contextType: 'html-element',
                    confidence: 0.7,
                    shouldBlockVSCodeCompletion: true,
                    reason: '在根级别检测到HTML元素，使用CHTL补全'
                };
            }
        }

        // 默认在根级别使用CHTL补全
        return {
            isInCHTLContext: true,
            contextType: 'chtl-syntax',
            confidence: 0.6,
            shouldBlockVSCodeCompletion: true,
            reason: '在根级别，默认使用CHTL补全'
        };
    }

    /**
     * 获取冲突解决建议
     */
    public getConflictResolutionSuggestion(result: ConflictDetectionResult): string {
        if (result.confidence < 0.5) {
            return `检测置信度较低 (${Math.round(result.confidence * 100)}%)，建议手动切换补全模式`;
        }

        if (result.shouldBlockVSCodeCompletion) {
            return `建议使用CHTL补全: ${result.reason}`;
        } else {
            return `建议使用VSCode默认补全: ${result.reason}`;
        }
    }

    /**
     * 创建冲突解决状态栏项
     */
    public createStatusBarItem(): vscode.StatusBarItem {
        const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 90);
        statusBarItem.text = `$(symbol-method) CHTL-${this.strategy}`;
        statusBarItem.tooltip = '点击切换CHTL语法补全策略';
        statusBarItem.command = 'chtl.toggleConflictResolution';
        statusBarItem.show();

        return statusBarItem;
    }

    /**
     * 切换冲突解决策略
     */
    public toggleStrategy(): ConflictResolutionStrategy {
        const strategies: ConflictResolutionStrategy[] = ['smart-detection', 'prefer-chtl', 'prefer-vscode'];
        const currentIndex = strategies.indexOf(this.strategy);
        const nextIndex = (currentIndex + 1) % strategies.length;
        
        this.strategy = strategies[nextIndex];
        this.config.update('intellisense.conflictResolution', this.strategy, vscode.ConfigurationTarget.Workspace);
        
        return this.strategy;
    }

    /**
     * 获取当前策略
     */
    public getCurrentStrategy(): ConflictResolutionStrategy {
        return this.strategy;
    }
}