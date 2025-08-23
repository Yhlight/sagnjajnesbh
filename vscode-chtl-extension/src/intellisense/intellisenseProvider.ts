import * as vscode from 'vscode';
import { ModuleResolver, ModuleInfo } from '../modules/moduleResolver';
import { CHTLCompilerService } from '../compiler/compilerService';

export interface CHTLSuggestion {
    label: string;
    kind: vscode.CompletionItemKind;
    detail: string;
    documentation: string | vscode.MarkdownString;
    insertText: string | vscode.SnippetString;
    sortText?: string;
    filterText?: string;
    commitCharacters?: string[];
}

export interface ContextInfo {
    position: vscode.Position;
    line: string;
    beforeCursor: string;
    afterCursor: string;
    inBlock: 'template' | 'custom' | 'style' | 'script' | 'configuration' | 'namespace' | 'import' | 'none';
    blockType: string; // @Style, @Element, @Var等
    parentContext: string;
    isInQuotes: boolean;
    isInComment: boolean;
}

export class CHTLIntelliSenseProvider {
    private moduleResolver: ModuleResolver;
    private compilerService: CHTLCompilerService;
    private keywordSuggestions: Map<string, CHTLSuggestion[]> = new Map();
    private snippetSuggestions: Map<string, CHTLSuggestion[]> = new Map();

    constructor(moduleResolver: ModuleResolver, compilerService: CHTLCompilerService) {
        this.moduleResolver = moduleResolver;
        this.compilerService = compilerService;
        this.initializeSuggestions();
    }

    private initializeSuggestions(): void {
        this.initializeKeywordSuggestions();
        this.initializeSnippetSuggestions();
    }

    private initializeKeywordSuggestions(): void {
        // 基础CHTL关键字建议
        this.keywordSuggestions.set('root', [
            {
                label: '[Template]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'CHTL Template Declaration',
                documentation: new vscode.MarkdownString('定义一个CHTL模板，可以在其他地方引用和展开'),
                insertText: new vscode.SnippetString('[Template] @${1|Style,Element,Var|} ${2:name} {\n\t$0\n}'),
                sortText: '00'
            },
            {
                label: '[Custom]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'CHTL Custom Declaration',
                documentation: new vscode.MarkdownString('定义一个CHTL自定义节点，支持特例化和继承'),
                insertText: new vscode.SnippetString('[Custom] @${1|Style,Element,Var|} ${2:name} {\n\t$0\n}'),
                sortText: '01'
            },
            {
                label: '[Configuration]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'CHTL Configuration Block',
                documentation: new vscode.MarkdownString('配置CHTL编译器行为和项目设置'),
                insertText: new vscode.SnippetString('[Configuration] @Config ${1:name} {\n\t$0\n}'),
                sortText: '02'
            },
            {
                label: '[Namespace]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'CHTL Namespace Declaration',
                documentation: new vscode.MarkdownString('定义命名空间，避免名称冲突并组织代码'),
                insertText: new vscode.SnippetString('[Namespace] ${1:name} {\n\t$0\n}'),
                sortText: '03'
            },
            {
                label: '[Import]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'CHTL Import Statement',
                documentation: new vscode.MarkdownString('导入外部模块、文件或资源'),
                insertText: new vscode.SnippetString('[Import] @${1|Html,Style,JavaScript,Chtl,CJmod|} from ${2:path}'),
                sortText: '04'
            }
        ]);

        // 模板和自定义内部的类型标识符
        this.keywordSuggestions.set('template-custom', [
            {
                label: '@Style',
                kind: vscode.CompletionItemKind.Class,
                detail: 'Style Template/Custom',
                documentation: '样式模板或自定义，用于定义CSS样式',
                insertText: '@Style',
                sortText: '00'
            },
            {
                label: '@Element',
                kind: vscode.CompletionItemKind.Class,
                detail: 'Element Template/Custom',
                documentation: '元素模板或自定义，用于定义HTML元素',
                insertText: '@Element',
                sortText: '01'
            },
            {
                label: '@Var',
                kind: vscode.CompletionItemKind.Class,
                detail: 'Variable Template/Custom',
                documentation: '变量模板或自定义，用于定义可重用的变量',
                insertText: '@Var',
                sortText: '02'
            }
        ]);

        // 导入类型标识符
        this.keywordSuggestions.set('import-types', [
            {
                label: '@Html',
                kind: vscode.CompletionItemKind.File,
                detail: 'HTML File Import',
                documentation: '导入HTML文件。需要as语法创建带名原始嵌入节点，否则跳过。',
                insertText: '@Html',
                sortText: '00'
            },
            {
                label: '@Style',
                kind: vscode.CompletionItemKind.File,
                detail: 'CSS/Style File Import',
                documentation: '导入CSS样式文件。需要as语法创建带名原始嵌入节点，否则跳过。',
                insertText: '@Style',
                sortText: '01'
            },
            {
                label: '@JavaScript',
                kind: vscode.CompletionItemKind.File,
                detail: 'JavaScript File Import',
                documentation: '导入JavaScript文件。需要as语法创建带名原始嵌入节点，否则跳过。',
                insertText: '@JavaScript',
                sortText: '02'
            },
            {
                label: '@Chtl',
                kind: vscode.CompletionItemKind.Module,
                detail: 'CHTL Module Import',
                documentation: '导入CHTL模块，支持.cmod和.chtl文件',
                insertText: '@Chtl',
                sortText: '03'
            },
            {
                label: '@CJmod',
                kind: vscode.CompletionItemKind.Module,
                detail: 'CJMOD Module Import',
                documentation: '导入CJMOD模块，支持JavaScript扩展语法',
                insertText: '@CJmod',
                sortText: '04'
            },
            {
                label: '[Origin]',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'Origin Embedding Prefix',
                documentation: '原始嵌入前缀，用于导入自定义原始嵌入类型',
                insertText: '[Origin] @',
                sortText: '05'
            }
        ]);

        // 样式块内的特殊语法
        this.keywordSuggestions.set('style-block', [
            {
                label: '&',
                kind: vscode.CompletionItemKind.Operator,
                detail: 'Reference Selector (Style)',
                documentation: '引用选择器，在样式块中优先选择class',
                insertText: '&',
                sortText: '00'
            },
            {
                label: 'inherit',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'Inheritance Declaration',
                documentation: '继承声明，可以继承其他样式或模板',
                insertText: new vscode.SnippetString('inherit ${1:target};'),
                sortText: '01'
            },
            {
                label: 'delete',
                kind: vscode.CompletionItemKind.Keyword,
                detail: 'Deletion Declaration',
                documentation: '删除声明，用于移除继承的属性或元素',
                insertText: new vscode.SnippetString('delete ${1:target};'),
                sortText: '02'
            }
        ]);

        // 脚本块内的特殊语法
        this.keywordSuggestions.set('script-block', [
            {
                label: '{{&}}',
                kind: vscode.CompletionItemKind.Operator,
                detail: 'Reference Selector (Script)',
                documentation: '引用选择器，在脚本块中优先选择id',
                insertText: '{{&}}',
                sortText: '00'
            },
            {
                label: '{{.className}}',
                kind: vscode.CompletionItemKind.Operator,
                detail: 'Class Selector Reference',
                documentation: '类选择器引用，自动触发class自动化添加',
                insertText: new vscode.SnippetString('{{.${1:className}}}'),
                sortText: '01'
            },
            {
                label: '{{#idName}}',
                kind: vscode.CompletionItemKind.Operator,
                detail: 'ID Selector Reference',
                documentation: 'ID选择器引用，自动触发id自动化添加',
                insertText: new vscode.SnippetString('{{#${1:idName}}}'),
                sortText: '02'
            }
        ]);

        // 配置项建议
        this.keywordSuggestions.set('configuration', [
            {
                label: 'INDEX_INITIAL_COUNT',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Index Initial Count Configuration',
                documentation: '设置索引初始计数值',
                insertText: new vscode.SnippetString('INDEX_INITIAL_COUNT = ${1:0};'),
                sortText: '00'
            },
            {
                label: 'DISABLE_NAME_GROUP',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Name Group Configuration',
                documentation: '禁用名称组功能',
                insertText: new vscode.SnippetString('DISABLE_NAME_GROUP = ${1|true,false|};'),
                sortText: '01'
            },
            {
                label: 'DISABLE_STYLE_AUTO_ADD_CLASS',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Style Auto Add Class',
                documentation: '禁止局部样式块自动添加类选择器',
                insertText: new vscode.SnippetString('DISABLE_STYLE_AUTO_ADD_CLASS = ${1|true,false|};'),
                sortText: '02'
            },
            {
                label: 'DISABLE_STYLE_AUTO_ADD_ID',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Style Auto Add ID',
                documentation: '禁止局部样式块自动添加id选择器',
                insertText: new vscode.SnippetString('DISABLE_STYLE_AUTO_ADD_ID = ${1|true,false|};'),
                sortText: '03'
            },
            {
                label: 'DISABLE_SCRIPT_AUTO_ADD_CLASS',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Script Auto Add Class',
                documentation: '禁止局部脚本自动添加类选择器',
                insertText: new vscode.SnippetString('DISABLE_SCRIPT_AUTO_ADD_CLASS = ${1|true,false|};'),
                sortText: '04'
            },
            {
                label: 'DISABLE_SCRIPT_AUTO_ADD_ID',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Script Auto Add ID',
                documentation: '禁止局部脚本自动添加id选择器',
                insertText: new vscode.SnippetString('DISABLE_SCRIPT_AUTO_ADD_ID = ${1|true,false|};'),
                sortText: '05'
            },
            {
                label: 'DISABLE_DEFAULT_NAMESPACE',
                kind: vscode.CompletionItemKind.Property,
                detail: 'Disable Default Namespace',
                documentation: '禁用默认命名空间功能',
                insertText: new vscode.SnippetString('DISABLE_DEFAULT_NAMESPACE = ${1|true,false|};'),
                sortText: '06'
            }
        ]);
    }

    private initializeSnippetSuggestions(): void {
        // HTML元素片段
        this.snippetSuggestions.set('html-elements', [
            {
                label: 'div',
                kind: vscode.CompletionItemKind.Snippet,
                detail: 'HTML Div Element',
                documentation: '创建div元素',
                insertText: new vscode.SnippetString('div {\n\t$0\n}'),
                sortText: '00'
            },
            {
                label: 'span',
                kind: vscode.CompletionItemKind.Snippet,
                detail: 'HTML Span Element',
                documentation: '创建span元素',
                insertText: new vscode.SnippetString('span {\n\t$0\n}'),
                sortText: '01'
            },
            {
                label: 'button',
                kind: vscode.CompletionItemKind.Snippet,
                detail: 'HTML Button Element',
                documentation: '创建button元素',
                insertText: new vscode.SnippetString('button {\n\ttype: "${1|button,submit,reset|}";\n\t$0\n}'),
                sortText: '02'
            }
        ]);

        // CSS属性片段
        this.snippetSuggestions.set('css-properties', [
            {
                label: 'color',
                kind: vscode.CompletionItemKind.Property,
                detail: 'CSS Color Property',
                documentation: '设置文本颜色',
                insertText: new vscode.SnippetString('color: ${1:#000000};'),
                sortText: '00'
            },
            {
                label: 'background',
                kind: vscode.CompletionItemKind.Property,
                detail: 'CSS Background Property',
                documentation: '设置背景样式',
                insertText: new vscode.SnippetString('background: ${1:transparent};'),
                sortText: '01'
            },
            {
                label: 'display',
                kind: vscode.CompletionItemKind.Property,
                detail: 'CSS Display Property',
                documentation: '设置显示类型',
                insertText: new vscode.SnippetString('display: ${1|block,inline,inline-block,flex,grid,none|};'),
                sortText: '02'
            }
        ]);
    }

    public async provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): Promise<vscode.CompletionItem[]> {
        const contextInfo = this.analyzeContext(document, position);
        const suggestions: vscode.CompletionItem[] = [];

        // 根据上下文提供不同的建议
        switch (contextInfo.inBlock) {
            case 'none':
                suggestions.push(...this.getRootSuggestions(contextInfo));
                break;
            case 'template':
            case 'custom':
                suggestions.push(...this.getTemplateCustomSuggestions(contextInfo));
                break;
            case 'import':
                suggestions.push(...this.getImportSuggestions(contextInfo));
                break;
            case 'style':
                suggestions.push(...this.getStyleBlockSuggestions(contextInfo));
                break;
            case 'script':
                suggestions.push(...this.getScriptBlockSuggestions(contextInfo));
                break;
            case 'configuration':
                suggestions.push(...this.getConfigurationSuggestions(contextInfo));
                break;
            case 'namespace':
                suggestions.push(...this.getNamespaceSuggestions(contextInfo));
                break;
        }

        // 添加模块导入建议
        if (contextInfo.beforeCursor.includes('from')) {
            suggestions.push(...await this.getModuleSuggestions(document, contextInfo));
        }

        // 转换为VSCode补全项
        return suggestions.map(s => this.convertToCompletionItem(s));
    }

    private analyzeContext(document: vscode.TextDocument, position: vscode.Position): ContextInfo {
        const line = document.lineAt(position).text;
        const beforeCursor = line.substring(0, position.character);
        const afterCursor = line.substring(position.character);

        // 分析当前所在的块类型
        let inBlock: ContextInfo['inBlock'] = 'none';
        let blockType = '';
        let parentContext = '';

        // 向上搜索找到当前块
        for (let i = position.line; i >= 0; i--) {
            const currentLine = document.lineAt(i).text.trim();
            
            if (currentLine.includes('[Template]')) {
                inBlock = 'template';
                const match = currentLine.match(/@(\w+)/);
                blockType = match ? match[1] : '';
                break;
            } else if (currentLine.includes('[Custom]')) {
                inBlock = 'custom';
                const match = currentLine.match(/@(\w+)/);
                blockType = match ? match[1] : '';
                break;
            } else if (currentLine.includes('[Import]')) {
                inBlock = 'import';
                break;
            } else if (currentLine.includes('[Configuration]')) {
                inBlock = 'configuration';
                break;
            } else if (currentLine.includes('[Namespace]')) {
                inBlock = 'namespace';
                break;
            } else if (currentLine.includes('style {')) {
                inBlock = 'style';
                break;
            } else if (currentLine.includes('script {')) {
                inBlock = 'script';
                break;
            }
        }

        const isInQuotes = this.isPositionInQuotes(beforeCursor);
        const isInComment = this.isPositionInComment(document, position);

        return {
            position,
            line,
            beforeCursor,
            afterCursor,
            inBlock,
            blockType,
            parentContext,
            isInQuotes,
            isInComment
        };
    }

    private isPositionInQuotes(beforeCursor: string): boolean {
        const singleQuotes = (beforeCursor.match(/'/g) || []).length;
        const doubleQuotes = (beforeCursor.match(/"/g) || []).length;
        return (singleQuotes % 2 !== 0) || (doubleQuotes % 2 !== 0);
    }

    private isPositionInComment(document: vscode.TextDocument, position: vscode.Position): boolean {
        const line = document.lineAt(position).text;
        const beforeCursor = line.substring(0, position.character);
        
        // 检查单行注释
        const singleLineCommentIndex = line.indexOf('//');
        if (singleLineCommentIndex !== -1 && position.character > singleLineCommentIndex) {
            return true;
        }

        // 检查多行注释（简化实现）
        if (beforeCursor.includes('/*') && !beforeCursor.includes('*/')) {
            return true;
        }

        return false;
    }

    private getRootSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions = this.keywordSuggestions.get('root') || [];
        return suggestions.map(s => this.convertToCompletionItem(s));
    }

    private getTemplateCustomSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions: vscode.CompletionItem[] = [];
        
        // 如果在@类型标识符位置
        if (context.beforeCursor.includes('[Template]') || context.beforeCursor.includes('[Custom]')) {
            const typeSuggestions = this.keywordSuggestions.get('template-custom') || [];
            suggestions.push(...typeSuggestions.map(s => this.convertToCompletionItem(s)));
        }

        // 根据块类型提供相应建议
        if (context.blockType === 'Style') {
            const styleSuggestions = this.keywordSuggestions.get('style-block') || [];
            suggestions.push(...styleSuggestions.map(s => this.convertToCompletionItem(s)));
            
            const cssSuggestions = this.snippetSuggestions.get('css-properties') || [];
            suggestions.push(...cssSuggestions.map(s => this.convertToCompletionItem(s)));
        } else if (context.blockType === 'Element') {
            const htmlSuggestions = this.snippetSuggestions.get('html-elements') || [];
            suggestions.push(...htmlSuggestions.map(s => this.convertToCompletionItem(s)));
        }

        return suggestions;
    }

    private getImportSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions = this.keywordSuggestions.get('import-types') || [];
        return suggestions.map(s => this.convertToCompletionItem(s));
    }

    private getStyleBlockSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions: vscode.CompletionItem[] = [];
        
        const styleSuggestions = this.keywordSuggestions.get('style-block') || [];
        suggestions.push(...styleSuggestions.map(s => this.convertToCompletionItem(s)));
        
        const cssSuggestions = this.snippetSuggestions.get('css-properties') || [];
        suggestions.push(...cssSuggestions.map(s => this.convertToCompletionItem(s)));
        
        return suggestions;
    }

    private getScriptBlockSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions = this.keywordSuggestions.get('script-block') || [];
        return suggestions.map(s => this.convertToCompletionItem(s));
    }

    private getConfigurationSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        const suggestions = this.keywordSuggestions.get('configuration') || [];
        return suggestions.map(s => this.convertToCompletionItem(s));
    }

    private getNamespaceSuggestions(context: ContextInfo): vscode.CompletionItem[] {
        // 命名空间内可以包含所有根级别的语法
        return this.getRootSuggestions(context);
    }

    private async getModuleSuggestions(document: vscode.TextDocument, context: ContextInfo): Promise<vscode.CompletionItem[]> {
        const suggestions: vscode.CompletionItem[] = [];
        const workspaceRoot = vscode.workspace.getWorkspaceFolder(document.uri)?.uri.fsPath;
        
        if (!workspaceRoot) return suggestions;

        try {
            const modules = await this.moduleResolver.getAllModules(workspaceRoot);
            
            for (const module of modules) {
                suggestions.push({
                    label: module.name,
                    kind: vscode.CompletionItemKind.Module,
                    detail: `${module.type.toUpperCase()} Module${module.isOfficial ? ' (Official)' : ''}`,
                    documentation: new vscode.MarkdownString(
                        `**Path:** ${module.path}\n\n` +
                        `**Exports:** ${module.exports.join(', ') || 'None'}\n\n` +
                        `**Type:** ${module.type}`
                    ),
                    insertText: module.name,
                    sortText: module.isOfficial ? '00' + module.name : '01' + module.name
                });
            }
        } catch (error) {
            console.warn('Error getting module suggestions:', error);
        }

        return suggestions;
    }

    private convertToCompletionItem(suggestion: CHTLSuggestion): vscode.CompletionItem {
        const item = new vscode.CompletionItem(suggestion.label, suggestion.kind);
        item.detail = suggestion.detail;
        item.documentation = suggestion.documentation;
        item.insertText = suggestion.insertText;
        item.sortText = suggestion.sortText;
        item.filterText = suggestion.filterText;
        item.commitCharacters = suggestion.commitCharacters;
        
        return item;
    }

    public async provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Hover | undefined> {
        const wordRange = document.getWordRangeAtPosition(position);
        if (!wordRange) return undefined;

        const word = document.getText(wordRange);
        const context = this.analyzeContext(document, position);

        // 提供关键字和语法的详细说明
        const hoverContent = this.getHoverContent(word, context);
        if (hoverContent) {
            return new vscode.Hover(hoverContent, wordRange);
        }

        return undefined;
    }

    private getHoverContent(word: string, context: ContextInfo): vscode.MarkdownString | undefined {
        // 根据上下文和单词提供悬停信息
        if (word === 'inherit') {
            return new vscode.MarkdownString(
                '**inherit** - CHTL继承声明\n\n' +
                '用于继承其他模板、自定义或样式组的属性。支持样式组之间的继承。'
            );
        } else if (word === 'delete') {
            return new vscode.MarkdownString(
                '**delete** - CHTL删除声明\n\n' +
                '用于删除继承的属性或元素。可以删除属性、删除继承等。'
            );
        } else if (word.startsWith('@')) {
            return new vscode.MarkdownString(
                `**${word}** - CHTL类型标识符\n\n` +
                '用于标识模板、自定义或导入的类型。'
            );
        }

        return undefined;
    }

    public async provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): Promise<vscode.Definition | undefined> {
        const wordRange = document.getWordRangeAtPosition(position);
        if (!wordRange) return undefined;

        const word = document.getText(wordRange);
        const workspaceRoot = vscode.workspace.getWorkspaceFolder(document.uri)?.uri.fsPath;
        
        if (!workspaceRoot) return undefined;

        // 查找模块定义
        try {
            const modules = await this.moduleResolver.getAllModules(workspaceRoot);
            const module = modules.find(m => m.name === word || m.exports.includes(word));
            
            if (module) {
                return new vscode.Location(
                    vscode.Uri.file(module.path),
                    new vscode.Position(0, 0)
                );
            }
        } catch (error) {
            console.warn('Error finding definition:', error);
        }

        return undefined;
    }
}