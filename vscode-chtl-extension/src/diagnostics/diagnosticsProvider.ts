import * as vscode from 'vscode';
import { CHTLCompilerService } from '../compiler/compilerService';

export interface CHTLDiagnosticError {
    message: string;
    severity: vscode.DiagnosticSeverity;
    range: vscode.Range;
    code?: string | number;
    source: string;
    relatedInformation?: vscode.DiagnosticRelatedInformation[];
}

/**
 * CHTL诊断提供器
 * 提供语法错误检测、语义分析和代码验证功能
 */
export class CHTLDiagnosticsProvider {
    private diagnosticCollection?: vscode.DiagnosticCollection;
    private readonly compilerService: CHTLCompilerService;

    // CHTL语法规则检查器
    private readonly SYNTAX_RULES = {
        // 块声明规则
        BLOCK_DECLARATIONS: [
            {
                pattern: /\[(Template|Custom|Configuration|Namespace|Import)\]/g,
                validate: (match: RegExpMatchArray, line: string, lineNumber: number) => {
                    const blockType = match[1];
                    if (blockType === 'Template' || blockType === 'Custom') {
                        // 检查是否有类型标识符
                        const typePattern = /@(Style|Element|Var|Html|JavaScript|Chtl|CJmod)\s+\w+/;
                        if (!typePattern.test(line)) {
                            return {
                                message: `${blockType}块必须包含类型标识符，如 @Element myElement`,
                                severity: vscode.DiagnosticSeverity.Error,
                                code: 'missing-type-identifier'
                            };
                        }
                    }
                    return null;
                }
            }
        ],

        // 花括号匹配规则
        BRACE_MATCHING: [
            {
                pattern: /[{}]/g,
                validate: (document: vscode.TextDocument) => {
                    let braceCount = 0;
                    const errors: CHTLDiagnosticError[] = [];
                    
                    for (let i = 0; i < document.lineCount; i++) {
                        const line = document.lineAt(i).text;
                        const openBraces = (line.match(/\{/g) || []).length;
                        const closeBraces = (line.match(/\}/g) || []).length;
                        braceCount += openBraces - closeBraces;
                        
                        if (braceCount < 0) {
                            errors.push({
                                message: '多余的右花括号',
                                severity: vscode.DiagnosticSeverity.Error,
                                range: new vscode.Range(i, line.lastIndexOf('}'), i, line.lastIndexOf('}') + 1),
                                code: 'extra-close-brace',
                                source: 'chtl-syntax'
                            });
                            braceCount = 0; // 重置避免级联错误
                        }
                    }
                    
                    if (braceCount > 0) {
                        const lastLine = document.lineCount - 1;
                        errors.push({
                            message: `缺少 ${braceCount} 个右花括号`,
                            severity: vscode.DiagnosticSeverity.Error,
                            range: new vscode.Range(lastLine, 0, lastLine, document.lineAt(lastLine).text.length),
                            code: 'missing-close-brace',
                            source: 'chtl-syntax'
                        });
                    }
                    
                    return errors;
                }
            }
        ],

        // 导入语法规则
        IMPORT_SYNTAX: [
            {
                pattern: /\[Import\]/g,
                validate: (match: RegExpMatchArray, line: string, lineNumber: number) => {
                    const importPattern = /\[Import\]\s+(CHTL|CJMOD|MEDIA_HTML|MEDIA_STYLE|MEDIA_JAVASCRIPT|ORIGIN_VUE|ORIGIN_REACT|ORIGIN_ANGULAR|ORIGIN_CUSTOM)/;
                    if (!importPattern.test(line)) {
                        return {
                            message: '导入声明必须指定类型，如 [Import] CHTL',
                            severity: vscode.DiagnosticSeverity.Error,
                            code: 'missing-import-type'
                        };
                    }

                    // 检查媒体导入是否有as子句
                    const mediaImportPattern = /\[Import\]\s+(MEDIA_HTML|MEDIA_STYLE|MEDIA_JAVASCRIPT)/;
                    if (mediaImportPattern.test(line) && !line.includes(' as ')) {
                        return {
                            message: '媒体导入必须使用 as 子句指定别名',
                            severity: vscode.DiagnosticSeverity.Error,
                            code: 'missing-media-alias'
                        };
                    }

                    return null;
                }
            }
        ],

        // CHTL JS语法规则
        CHTL_JS_SYNTAX: [
            {
                pattern: /\{\{.*?\}\}/g,
                validate: (match: RegExpMatchArray, line: string, lineNumber: number) => {
                    const content = match[0].slice(2, -2).trim();
                    
                    // 检查嵌套花括号
                    const openBraces = (content.match(/\{/g) || []).length;
                    const closeBraces = (content.match(/\}/g) || []).length;
                    
                    if (openBraces !== closeBraces) {
                        return {
                            message: 'CHTL JS表达式中花括号不匹配',
                            severity: vscode.DiagnosticSeverity.Error,
                            code: 'chtl-js-brace-mismatch'
                        };
                    }

                    // 检查脚本引用语法
                    if (content.includes('&') && !content.match(/\{\{&\w+\}\}/)) {
                        return {
                            message: '脚本引用必须使用 {{&functionName}} 格式',
                            severity: vscode.DiagnosticSeverity.Warning,
                            code: 'invalid-script-reference'
                        };
                    }

                    return null;
                }
            }
        ],

        // 样式引用规则
        STYLE_REFERENCE: [
            {
                pattern: /&\w+/g,
                validate: (match: RegExpMatchArray, line: string, lineNumber: number, document: vscode.TextDocument) => {
                    // 检查是否在style块中
                    let inStyleBlock = false;
                    for (let i = lineNumber; i >= 0; i--) {
                        const currentLine = document.lineAt(i).text;
                        if (currentLine.includes('style {')) {
                            inStyleBlock = true;
                            break;
                        }
                        if (currentLine.includes('}') && !currentLine.includes('style {')) {
                            break;
                        }
                    }

                    if (!inStyleBlock) {
                        return {
                            message: '样式引用(&)只能在style块中使用',
                            severity: vscode.DiagnosticSeverity.Error,
                            code: 'style-reference-outside-block'
                        };
                    }

                    return null;
                }
            }
        ],

        // 配置项检查
        CONFIGURATION_ITEMS: [
            {
                pattern: /\b(INDEX_INITIAL_COUNT|DISABLE_NAME_GROUP|DISABLE_CUSTOM_ORIGIN_TYPE|DEBUG_MODE|OPTION_COUNT|DISABLE_STYLE_AUTO_ADD_CLASS|DISABLE_STYLE_AUTO_ADD_ID|DISABLE_SCRIPT_AUTO_ADD_CLASS|DISABLE_SCRIPT_AUTO_ADD_ID|DISABLE_DEFAULT_NAMESPACE)\b/g,
                validate: (match: RegExpMatchArray, line: string, lineNumber: number, document: vscode.TextDocument) => {
                    // 检查是否在配置块中
                    let inConfigBlock = false;
                    for (let i = lineNumber; i >= 0; i--) {
                        const currentLine = document.lineAt(i).text;
                        if (currentLine.includes('[Configuration]')) {
                            inConfigBlock = true;
                            break;
                        }
                        if (currentLine.includes('[') && !currentLine.includes('[Configuration]')) {
                            break;
                        }
                    }

                    if (!inConfigBlock) {
                        return {
                            message: '配置项只能在[Configuration]块中使用',
                            severity: vscode.DiagnosticSeverity.Warning,
                            code: 'config-item-outside-block'
                        };
                    }

                    return null;
                }
            }
        ]
    };

    constructor(compilerService: CHTLCompilerService) {
        this.compilerService = compilerService;
    }

    public setDiagnosticCollection(collection: vscode.DiagnosticCollection): void {
        this.diagnosticCollection = collection;
    }

    /**
     * 更新文档诊断信息
     */
    public async updateDiagnostics(document: vscode.TextDocument): Promise<void> {
        if (!this.diagnosticCollection) {
            return;
        }

        const diagnostics: vscode.Diagnostic[] = [];
        
        try {
            // 1. 语法检查
            const syntaxErrors = await this.performSyntaxCheck(document);
            diagnostics.push(...syntaxErrors);

            // 2. 语义检查
            const semanticErrors = await this.performSemanticCheck(document);
            diagnostics.push(...semanticErrors);

            // 3. 编译器验证
            const compilerErrors = await this.performCompilerValidation(document);
            diagnostics.push(...compilerErrors);

            // 4. 更新诊断集合
            this.diagnosticCollection.set(document.uri, diagnostics);

        } catch (error) {
            console.error('Error updating diagnostics:', error);
            
            // 添加内部错误诊断
            const internalError = new vscode.Diagnostic(
                new vscode.Range(0, 0, 0, 0),
                `诊断检查时出现内部错误: ${error}`,
                vscode.DiagnosticSeverity.Information
            );
            internalError.source = 'chtl-diagnostics';
            internalError.code = 'internal-error';
            
            this.diagnosticCollection.set(document.uri, [internalError]);
        }
    }

    /**
     * 验证单个文档语法
     */
    public async validateSyntax(document: vscode.TextDocument): Promise<void> {
        await this.updateDiagnostics(document);
    }

    /**
     * 刷新所有打开文档的诊断
     */
    public async refreshAll(): Promise<void> {
        if (!this.diagnosticCollection) {
            return;
        }

        for (const document of vscode.workspace.textDocuments) {
            if (document.languageId === 'chtl' || document.languageId === 'chtl-js') {
                await this.updateDiagnostics(document);
            }
        }
    }

    /**
     * 执行语法检查
     */
    private async performSyntaxCheck(document: vscode.TextDocument): Promise<vscode.Diagnostic[]> {
        const diagnostics: vscode.Diagnostic[] = [];

        // 执行所有语法规则检查
        for (const [categoryName, rules] of Object.entries(this.SYNTAX_RULES)) {
            for (const rule of rules) {
                if ('validate' in rule && typeof rule.validate === 'function') {
                    if (rule.validate.length > 1) {
                        // 需要文档参数的规则
                        const errors = (rule.validate as any)(document);
                        if (Array.isArray(errors)) {
                            diagnostics.push(...errors.map(error => this.convertToDiagnostic(error)));
                        }
                    } else {
                        // 基于行的规则
                        for (let i = 0; i < document.lineCount; i++) {
                            const line = document.lineAt(i).text;
                            const matches = Array.from(line.matchAll(rule.pattern));
                            
                            for (const match of matches) {
                                const error = (rule.validate as any)(match, line, i, document);
                                if (error) {
                                    const range = new vscode.Range(
                                        i, 
                                        match.index || 0, 
                                        i, 
                                        (match.index || 0) + match[0].length
                                    );
                                    
                                    const diagnostic = new vscode.Diagnostic(
                                        range,
                                        error.message,
                                        error.severity
                                    );
                                    diagnostic.source = 'chtl-syntax';
                                    diagnostic.code = error.code;
                                    
                                    diagnostics.push(diagnostic);
                                }
                            }
                        }
                    }
                }
            }
        }

        return diagnostics;
    }

    /**
     * 执行语义检查
     */
    private async performSemanticCheck(document: vscode.TextDocument): Promise<vscode.Diagnostic[]> {
        const diagnostics: vscode.Diagnostic[] = [];

        // 1. 检查未定义的引用
        const undefinedReferences = this.checkUndefinedReferences(document);
        diagnostics.push(...undefinedReferences);

        // 2. 检查重复定义
        const duplicateDefinitions = this.checkDuplicateDefinitions(document);
        diagnostics.push(...duplicateDefinitions);

        // 3. 检查循环引用
        const circularReferences = this.checkCircularReferences(document);
        diagnostics.push(...circularReferences);

        return diagnostics;
    }

    /**
     * 执行编译器验证
     */
    private async performCompilerValidation(document: vscode.TextDocument): Promise<vscode.Diagnostic[]> {
        const diagnostics: vscode.Diagnostic[] = [];

        try {
            const result = await this.compilerService.validateSyntax(document.uri.fsPath);
            
            if (!result.success) {
                for (const error of result.errors) {
                    const diagnostic = new vscode.Diagnostic(
                        new vscode.Range(
                            Math.max(0, (error.line || 1) - 1),
                            Math.max(0, (error.column || 1) - 1),
                            Math.max(0, (error.line || 1) - 1),
                            Math.max(0, (error.column || 1) - 1) + (error.length || 1)
                        ),
                        error.message,
                        this.mapSeverity(error.severity)
                    );
                    
                    diagnostic.source = 'chtl-compiler';
                    diagnostic.code = error.code;
                    diagnostics.push(diagnostic);
                }
            }
        } catch (error) {
            // 编译器验证失败，记录但不中断其他检查
            console.warn('Compiler validation failed:', error);
        }

        return diagnostics;
    }

    /**
     * 检查未定义的引用
     */
    private checkUndefinedReferences(document: vscode.TextDocument): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const definitions = new Set<string>();
        const references = new Map<string, vscode.Range[]>();

        // 收集定义和引用
        for (let i = 0; i < document.lineCount; i++) {
            const line = document.lineAt(i).text;
            
            // 收集模板/自定义节点定义
            const templateMatch = line.match(/\[(Template|Custom)\]\s*@\w+\s+(\w+)/);
            if (templateMatch) {
                definitions.add(templateMatch[2]);
            }

            // 收集引用
            const referenceMatches = Array.from(line.matchAll(/@\w+\s+(\w+)/g));
            for (const match of referenceMatches) {
                const refName = match[1];
                if (!references.has(refName)) {
                    references.set(refName, []);
                }
                const range = new vscode.Range(
                    i, 
                    match.index! + match[0].indexOf(refName), 
                    i, 
                    match.index! + match[0].indexOf(refName) + refName.length
                );
                references.get(refName)!.push(range);
            }
        }

        // 检查未定义的引用
        for (const [refName, ranges] of references) {
            if (!definitions.has(refName)) {
                for (const range of ranges) {
                    const diagnostic = new vscode.Diagnostic(
                        range,
                        `未定义的引用: ${refName}`,
                        vscode.DiagnosticSeverity.Warning
                    );
                    diagnostic.source = 'chtl-semantic';
                    diagnostic.code = 'undefined-reference';
                    diagnostics.push(diagnostic);
                }
            }
        }

        return diagnostics;
    }

    /**
     * 检查重复定义
     */
    private checkDuplicateDefinitions(document: vscode.TextDocument): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        const definitions = new Map<string, vscode.Range[]>();

        for (let i = 0; i < document.lineCount; i++) {
            const line = document.lineAt(i).text;
            const match = line.match(/\[(Template|Custom)\]\s*@\w+\s+(\w+)/);
            
            if (match) {
                const name = match[2];
                if (!definitions.has(name)) {
                    definitions.set(name, []);
                }
                
                const range = new vscode.Range(
                    i, 
                    match.index! + match[0].indexOf(name), 
                    i, 
                    match.index! + match[0].indexOf(name) + name.length
                );
                definitions.get(name)!.push(range);
            }
        }

        // 检查重复定义
        for (const [name, ranges] of definitions) {
            if (ranges.length > 1) {
                for (let i = 1; i < ranges.length; i++) {
                    const diagnostic = new vscode.Diagnostic(
                        ranges[i],
                        `重复定义: ${name}`,
                        vscode.DiagnosticSeverity.Error
                    );
                    diagnostic.source = 'chtl-semantic';
                    diagnostic.code = 'duplicate-definition';
                    
                    // 添加相关信息
                    diagnostic.relatedInformation = [
                        new vscode.DiagnosticRelatedInformation(
                            new vscode.Location(document.uri, ranges[0]),
                            `首次定义于此处`
                        )
                    ];
                    
                    diagnostics.push(diagnostic);
                }
            }
        }

        return diagnostics;
    }

    /**
     * 检查循环引用
     */
    private checkCircularReferences(document: vscode.TextDocument): vscode.Diagnostic[] {
        const diagnostics: vscode.Diagnostic[] = [];
        // 循环引用检查的实现比较复杂，这里提供一个简化版本
        // 实际项目中需要建立完整的依赖图进行检查

        return diagnostics;
    }

    /**
     * 映射错误严重性
     */
    private mapSeverity(severity?: string): vscode.DiagnosticSeverity {
        switch (severity?.toLowerCase()) {
            case 'error':
                return vscode.DiagnosticSeverity.Error;
            case 'warning':
                return vscode.DiagnosticSeverity.Warning;
            case 'info':
                return vscode.DiagnosticSeverity.Information;
            case 'hint':
                return vscode.DiagnosticSeverity.Hint;
            default:
                return vscode.DiagnosticSeverity.Error;
        }
    }

    /**
     * 转换错误对象为诊断对象
     */
    private convertToDiagnostic(error: CHTLDiagnosticError): vscode.Diagnostic {
        const diagnostic = new vscode.Diagnostic(
            error.range,
            error.message,
            error.severity
        );
        
        diagnostic.source = error.source;
        diagnostic.code = error.code;
        
        if (error.relatedInformation) {
            diagnostic.relatedInformation = error.relatedInformation;
        }
        
        return diagnostic;
    }
}