import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';

/**
 * CHTL模块提供器 - 支持完整的模块路径机制
 */
export class CHTLModuleProvider {
    private static instance: CHTLModuleProvider;
    private moduleCache = new Map<string, ModuleInfo[]>();
    private compilerPath: string = '';
    
    public static getInstance(): CHTLModuleProvider {
        if (!CHTLModuleProvider.instance) {
            CHTLModuleProvider.instance = new CHTLModuleProvider();
        }
        return CHTLModuleProvider.instance;
    }
    
    /**
     * 初始化模块提供器
     */
    public initialize(context: vscode.ExtensionContext) {
        // 检测编译器路径
        this.detectCompilerPath();
        
        // 扫描所有模块
        this.scanAllModules();
        
        // 监听文件变化
        this.setupFileWatcher(context);
    }
    
    /**
     * 获取模块补全项
     */
    public getModuleCompletions(document: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
        const completions: vscode.CompletionItem[] = [];
        
        // 获取当前行文本
        const line = document.lineAt(position).text;
        const beforeCursor = line.substring(0, position.character);
        
        // 检查是否在导入语句中
        if (this.isInImportStatement(beforeCursor)) {
            return this.getImportCompletions();
        }
        
        // 检查是否在使用模块组件
        if (this.isUsingModuleComponent(beforeCursor)) {
            return this.getComponentCompletions();
        }
        
        return completions;
    }
    
    /**
     * 获取导入补全
     */
    private getImportCompletions(): vscode.CompletionItem[] {
        const completions: vscode.CompletionItem[] = [];
        
        // 获取所有可用模块
        const allModules = this.getAllAvailableModules();
        
        allModules.forEach(module => {
            const item = new vscode.CompletionItem(module.name, vscode.CompletionItemKind.Module);
            item.detail = `${module.type} 模块 - ${module.description}`;
            item.documentation = new vscode.MarkdownString(
                `**${module.name}**\n\n` +
                `类型: ${module.type}\n` +
                `版本: ${module.version}\n` +
                `作者: ${module.author}\n` +
                `路径: ${module.path}\n\n` +
                `${module.description}`
            );
            
            // 根据模块类型设置不同图标
            if (module.type === 'CMOD') {
                item.kind = vscode.CompletionItemKind.Module;
            } else if (module.type === 'CJMOD') {
                item.kind = vscode.CompletionItemKind.Function;
            }
            
            completions.push(item);
        });
        
        return completions;
    }
    
    /**
     * 获取组件补全
     */
    private getComponentCompletions(): vscode.CompletionItem[] {
        const completions: vscode.CompletionItem[] = [];
        
        // 珂朵莉模块组件
        const chthollyComponents = [
            { name: 'ChthollyAccordion', desc: '手风琴组件' },
            { name: 'ChthollyGallery', desc: '四叶窗相册' },
            { name: 'ChthollyMemo', desc: '备忘录组件' },
            { name: 'ChthollyNote', desc: '暖色笔记' },
            { name: 'ChthollySakura', desc: '樱花雨效果' },
            { name: 'ChthollyMouseEffect', desc: '鼠标特效' },
            { name: 'ChthollyMouseTrail', desc: '鼠标拖尾' },
            { name: 'ChthollyParallax', desc: '视差滚动背景' },
            { name: 'ChthollyContextMenu', desc: '右键菜单栏' },
            { name: 'ChthollyProgressBar', desc: '进度条' }
        ];
        
        chthollyComponents.forEach(comp => {
            const item = new vscode.CompletionItem(comp.name, vscode.CompletionItemKind.Class);
            item.detail = `珂朵莉模块 - ${comp.desc}`;
            item.documentation = new vscode.MarkdownString(
                `**${comp.name}**\n\n` +
                `珂朵莉主题组件 - ${comp.desc}\n\n` +
                `使用方法:\n` +
                `\`\`\`chtl\n` +
                `[Import] @Chtl from Chtholly\n\n` +
                `${comp.name} {\n` +
                `    // 组件属性\n` +
                `}\n` +
                `\`\`\``
            );
            completions.push(item);
        });
        
        // 由比滨结衣模块组件
        const yuigahamaComponents = [
            { name: 'YuigahamaMusicPlayer', desc: '音乐播放器' },
            { name: 'YuigahamaAccordion', desc: '手风琴' },
            { name: 'YuigahamaGallery', desc: '四叶窗相册' },
            { name: 'YuigahamaMemo', desc: '备忘录' },
            { name: 'YuigahamaNote', desc: '暖色笔记' },
            { name: 'YuigahamaSakura', desc: '樱花雨' },
            { name: 'YuigahamaMouseEffect', desc: '鼠标特效' }
        ];
        
        yuigahamaComponents.forEach(comp => {
            const item = new vscode.CompletionItem(comp.name, vscode.CompletionItemKind.Class);
            item.detail = `由比滨结衣模块 - ${comp.desc}`;
            item.documentation = new vscode.MarkdownString(
                `**${comp.name}**\n\n` +
                `由比滨结衣主题组件 - ${comp.desc}\n\n` +
                `使用方法:\n` +
                `\`\`\`chtl\n` +
                `[Import] @Chtl from Yuigahama\n\n` +
                `${comp.name} {\n` +
                `    // 组件属性\n` +
                `}\n` +
                `\`\`\``
            );
            completions.push(item);
        });
        
        return completions;
    }
    
    /**
     * 检测编译器路径
     */
    private detectCompilerPath() {
        // 检查常见的编译器位置
        const possiblePaths = [
            '/usr/local/bin/chtl',
            '/usr/bin/chtl',
            '/opt/chtl/bin/chtl',
            process.env.CHTL_COMPILER_PATH || '',
            path.join(vscode.workspace.rootPath || '', 'build', 'chtl_compiler')
        ];
        
        for (const compilerPath of possiblePaths) {
            if (fs.existsSync(compilerPath)) {
                this.compilerPath = compilerPath;
                console.log(`🔍 找到CHTL编译器: ${compilerPath}`);
                return;
            }
        }
        
        console.warn('⚠️ 未找到CHTL编译器，模块功能可能受限');
    }
    
    /**
     * 扫描所有模块
     */
    private scanAllModules() {
        this.moduleCache.clear();
        
        // 1. 扫描官方模块目录
        if (this.compilerPath) {
            const compilerDir = path.dirname(this.compilerPath);
            const officialModuleDir = path.join(compilerDir, 'module');
            this.scanModuleDirectory(officialModuleDir, 'official');
        }
        
        // 2. 扫描用户模块目录
        if (vscode.workspace.rootPath) {
            const userModuleDir = path.join(vscode.workspace.rootPath, 'module');
            this.scanModuleDirectory(userModuleDir, 'user');
        }
        
        console.log(`📦 模块扫描完成，发现 ${this.getTotalModuleCount()} 个模块`);
    }
    
    /**
     * 扫描指定目录的模块
     */
    private scanModuleDirectory(baseDir: string, category: string) {
        if (!fs.existsSync(baseDir)) {
            return;
        }
        
        // 扫描所有格式变体
        const variants = ['CMOD', 'Cmod', 'cmod', 'CJMOD', 'CJmod', 'cjmod'];
        
        variants.forEach(variant => {
            const variantDir = path.join(baseDir, variant);
            if (fs.existsSync(variantDir)) {
                this.scanVariantDirectory(variantDir, variant, category);
            }
        });
    }
    
    /**
     * 扫描格式变体目录
     */
    private scanVariantDirectory(variantDir: string, variant: string, category: string) {
        try {
            const items = fs.readdirSync(variantDir);
            
            items.forEach(item => {
                const itemPath = path.join(variantDir, item);
                const stat = fs.statSync(itemPath);
                
                if (stat.isFile() && (item.endsWith('.cmod') || item.endsWith('.cjmod'))) {
                    // 打包的模块文件
                    const moduleName = path.basename(item, path.extname(item));
                    const moduleInfo = this.parsePackedModule(itemPath, variant, category);
                    if (moduleInfo) {
                        this.addModuleToCache(moduleName, moduleInfo);
                    }
                } else if (stat.isDirectory()) {
                    // 未打包的模块目录
                    const moduleInfo = this.parseModuleDirectory(itemPath, variant, category);
                    if (moduleInfo) {
                        this.addModuleToCache(item, moduleInfo);
                    }
                }
            });
        } catch (error) {
            console.error(`扫描目录失败 ${variantDir}:`, error);
        }
    }
    
    /**
     * 解析打包的模块
     */
    private parsePackedModule(filePath: string, variant: string, category: string): ModuleInfo | null {
        // TODO: 实现ZIP文件解析
        const moduleName = path.basename(filePath, path.extname(filePath));
        
        return {
            name: moduleName,
            type: variant.toUpperCase().includes('CMOD') ? 'CMOD' : 'CJMOD',
            version: '1.0.0',
            description: `${variant}格式模块`,
            author: 'Unknown',
            path: filePath,
            category: category,
            isPacked: true,
            exports: []
        };
    }
    
    /**
     * 解析模块目录
     */
    private parseModuleDirectory(dirPath: string, variant: string, category: string): ModuleInfo | null {
        const moduleName = path.basename(dirPath);
        const infoFile = path.join(dirPath, 'info', `${moduleName}.chtl`);
        
        if (!fs.existsSync(infoFile)) {
            return null;
        }
        
        try {
            const infoContent = fs.readFileSync(infoFile, 'utf-8');
            return this.parseInfoFile(infoContent, dirPath, variant, category);
        } catch (error) {
            console.error(`解析模块信息失败 ${infoFile}:`, error);
            return null;
        }
    }
    
    /**
     * 解析info文件
     */
    private parseInfoFile(content: string, dirPath: string, variant: string, category: string): ModuleInfo | null {
        const moduleName = path.basename(dirPath);
        
        // 解析[Info]段
        const infoMatch = content.match(/\[Info\]\s*\{([^}]*)\}/);
        if (!infoMatch) {
            return null;
        }
        
        const infoContent = infoMatch[1];
        const fields = this.parseInfoFields(infoContent);
        
        // 解析[Export]段
        const exports = this.parseExports(content);
        
        return {
            name: fields.name || moduleName,
            type: variant.toUpperCase().includes('CMOD') ? 'CMOD' : 'CJMOD',
            version: fields.version || '1.0.0',
            description: fields.description || '',
            author: fields.author || '',
            path: dirPath,
            category: category,
            isPacked: false,
            exports: exports
        };
    }
    
    /**
     * 解析Info字段
     */
    private parseInfoFields(content: string): any {
        const fields: any = {};
        
        const fieldRegex = /(\w+)\s*=\s*"([^"]*)"/g;
        let match;
        
        while ((match = fieldRegex.exec(content)) !== null) {
            fields[match[1]] = match[2];
        }
        
        return fields;
    }
    
    /**
     * 解析导出信息
     */
    private parseExports(content: string): ExportInfo[] {
        const exports: ExportInfo[] = [];
        
        const exportMatch = content.match(/\[Export\]\s*\{([^}]*)\}/);
        if (!exportMatch) {
            return exports;
        }
        
        const exportContent = exportMatch[1];
        const exportRegex = /\[(\w+)\]\s*@(\w+)\s+([^;]+);/g;
        let match;
        
        while ((match = exportRegex.exec(exportContent)) !== null) {
            exports.push({
                category: match[1],
                type: match[2],
                names: match[3].split(',').map(name => name.trim())
            });
        }
        
        return exports;
    }
    
    /**
     * 添加模块到缓存
     */
    private addModuleToCache(name: string, module: ModuleInfo) {
        if (!this.moduleCache.has(name)) {
            this.moduleCache.set(name, []);
        }
        this.moduleCache.get(name)!.push(module);
    }
    
    /**
     * 获取所有可用模块
     */
    private getAllAvailableModules(): ModuleInfo[] {
        const allModules: ModuleInfo[] = [];
        
        this.moduleCache.forEach(modules => {
            // 按优先级排序：官方 > 用户
            modules.sort((a, b) => {
                if (a.category === 'official' && b.category !== 'official') return -1;
                if (a.category !== 'official' && b.category === 'official') return 1;
                return 0;
            });
            
            allModules.push(modules[0]); // 取优先级最高的
        });
        
        return allModules;
    }
    
    /**
     * 检查是否在导入语句中
     */
    private isInImportStatement(text: string): boolean {
        return /\[Import\]\s*@(Chtl|CJmod)\s+from\s*$/.test(text);
    }
    
    /**
     * 检查是否在使用模块组件
     */
    private isUsingModuleComponent(text: string): boolean {
        return /^\s*[A-Z]\w*::\w*$/.test(text) || /^\s*[A-Z]\w*$/.test(text);
    }
    
    /**
     * 设置文件监听器
     */
    private setupFileWatcher(context: vscode.ExtensionContext) {
        // 监听模块目录变化
        const watcher = vscode.workspace.createFileSystemWatcher('**/module/**/*.{cmod,cjmod,chtl}');
        
        watcher.onDidCreate(() => this.scanAllModules());
        watcher.onDidDelete(() => this.scanAllModules());
        watcher.onDidChange(() => this.scanAllModules());
        
        context.subscriptions.push(watcher);
    }
    
    /**
     * 获取模块总数
     */
    private getTotalModuleCount(): number {
        let total = 0;
        this.moduleCache.forEach(modules => total += modules.length);
        return total;
    }
}

/**
 * 模块信息接口
 */
interface ModuleInfo {
    name: string;
    type: 'CMOD' | 'CJMOD';
    version: string;
    description: string;
    author: string;
    path: string;
    category: string;
    isPacked: boolean;
    exports: ExportInfo[];
}

/**
 * 导出信息接口
 */
interface ExportInfo {
    category: string;  // Custom, Template, Origin, Configuration
    type: string;      // Element, Style, Var, Html, JavaScript, Config
    names: string[];   // 导出的名称列表
}