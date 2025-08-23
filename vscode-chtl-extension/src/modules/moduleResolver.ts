import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import * as glob from 'glob';

export interface ModuleInfo {
    name: string;
    path: string;
    type: 'chtl' | 'cmod' | 'cjmod' | 'html' | 'css' | 'js' | 'vue' | 'react' | 'angular';
    exports: string[];
    imports: string[];
    version?: string;
    description?: string;
    isOfficial: boolean;
}

export interface ModuleSearchResult {
    found: boolean;
    module?: ModuleInfo;
    candidates: ModuleInfo[];
    searchPaths: string[];
}

export interface ImportSearchOptions {
    importType: string; // @Html, @Style, @JavaScript, @Chtl, @CJmod, etc.
    importPath: string;
    hasAsClause: boolean;
    isOriginImport: boolean;
    workspaceRoot: string;
}

export class ModuleResolver {
    private context: vscode.ExtensionContext;
    private config: vscode.WorkspaceConfiguration;
    private moduleCache: Map<string, ModuleInfo> = new Map();
    private searchPathCache: Map<string, string[]> = new Map();
    private lastCacheUpdate: number = 0;
    private readonly CACHE_TTL = 5 * 60 * 1000; // 5 minutes

    constructor(context: vscode.ExtensionContext, config: vscode.WorkspaceConfiguration) {
        this.context = context;
        this.config = config;
        this.initializeSearchPaths();
    }

    public updateConfig(newConfig: vscode.WorkspaceConfiguration): void {
        this.config = newConfig;
        this.invalidateCache();
        this.initializeSearchPaths();
    }

    private initializeSearchPaths(): void {
        const workspaceFolders = vscode.workspace.workspaceFolders;
        if (!workspaceFolders) return;

        for (const folder of workspaceFolders) {
            const searchPaths = this.buildSearchPaths(folder.uri.fsPath);
            this.searchPathCache.set(folder.uri.fsPath, searchPaths);
        }
    }

    private buildSearchPaths(workspaceRoot: string): string[] {
        const configuredPaths = this.config.get<string[]>('modules.searchPaths', []);
        const officialModulePath = this.config.get<string>('modules.officialModulePath', '');
        
        const searchPaths: string[] = [];

        // 1. 官方模块目录
        if (officialModulePath) {
            const resolvedOfficialPath = this.resolvePlaceholders(officialModulePath, workspaceRoot);
            this.addModulePaths(searchPaths, resolvedOfficialPath);
        }

        // 2. 编译器所在目录的module文件夹
        const compilerPath = this.config.get<string>('compiler.path', '');
        if (compilerPath && fs.existsSync(compilerPath)) {
            const compilerDir = path.dirname(path.resolve(compilerPath));
            this.addModulePaths(searchPaths, compilerDir);
        }

        // 3. 当前编译文件所在目录的module文件夹
        const activeEditor = vscode.window.activeTextEditor;
        if (activeEditor && (activeEditor.document.languageId === 'chtl' || activeEditor.document.languageId === 'chtl-js')) {
            const currentFileDir = path.dirname(activeEditor.document.uri.fsPath);
            this.addModulePaths(searchPaths, currentFileDir);
            
            // 编译文件所在目录本身（不递归）
            searchPaths.push(currentFileDir);
        }

        // 4. 源码目录检查
        const srcPaths = [
            path.join(workspaceRoot, 'src'),
            path.join(workspaceRoot, 'source'),
            path.join(workspaceRoot, 'lib'),
            path.join(workspaceRoot, 'app')
        ];
        
        for (const srcPath of srcPaths) {
            if (fs.existsSync(srcPath)) {
                this.addModulePaths(searchPaths, srcPath);
            }
        }

        // 5. 工作区根目录的module文件夹
        this.addModulePaths(searchPaths, workspaceRoot);

        // 6. 配置的搜索路径
        for (const configuredPath of configuredPaths) {
            const resolvedPath = this.resolvePlaceholders(configuredPath, workspaceRoot);
            this.addModulePaths(searchPaths, resolvedPath);
        }

        return [...new Set(searchPaths.filter(p => fs.existsSync(p)))]; // 去重并过滤存在的路径
    }

    /**
     * 为指定目录添加模块路径
     * 支持无序结构和有序结构
     */
    private addModulePaths(paths: string[], baseDir: string): void {
        if (!fs.existsSync(baseDir)) {
            return;
        }

        // 检查module/modules文件夹的各种变体
        const moduleVariants = ['module', 'modules', 'Module', 'Modules'];
        
        for (const variant of moduleVariants) {
            const modulePath = path.join(baseDir, variant);
            if (fs.existsSync(modulePath)) {
                // 添加module文件夹本身（无序结构：cmod,chtl,cjmod文件直接放在一起）
                paths.push(modulePath);
                
                // 检查有序结构的子文件夹
                this.addOrderedStructurePaths(paths, modulePath);
            }
        }
    }

    /**
     * 添加有序结构路径
     * 支持 CMOD/Cmod/cmod 和 CJMOD/CJmod/cjmod 分类
     */
    private addOrderedStructurePaths(paths: string[], moduleDir: string): void {
        const cmodVariants = ['CMOD', 'Cmod', 'cmod'];
        const cjmodVariants = ['CJMOD', 'CJmod', 'cjmod'];
        
        // 检查CMOD分类文件夹
        for (const variant of cmodVariants) {
            const cmodPath = path.join(moduleDir, variant);
            if (fs.existsSync(cmodPath)) {
                paths.push(cmodPath);
            }
        }
        
        // 检查CJMOD分类文件夹
        for (const variant of cjmodVariants) {
            const cjmodPath = path.join(moduleDir, variant);
            if (fs.existsSync(cjmodPath)) {
                paths.push(cjmodPath);
            }
        }
    }

    private resolvePlaceholders(pathTemplate: string, workspaceRoot: string): string {
        return pathTemplate
            .replace('${workspaceFolder}', workspaceRoot)
            .replace('${extensionPath}', this.context.extensionPath)
            .replace('${workspaceRoot}', workspaceRoot);
    }

    public async resolveImport(options: ImportSearchOptions): Promise<ModuleSearchResult> {
        await this.ensureCacheValid();

        const searchPaths = this.searchPathCache.get(options.workspaceRoot) || [];
        const candidates: ModuleInfo[] = [];
        let foundModule: ModuleInfo | undefined;

        // 根据导入类型执行不同的搜索策略
        switch (options.importType) {
            case '@Html':
            case '@Style':
            case '@JavaScript':
                foundModule = await this.resolveMediaImport(options, searchPaths, candidates);
                break;

            case '@Chtl':
                foundModule = await this.resolveCHTLImport(options, searchPaths, candidates);
                break;

            case '@CJmod':
                foundModule = await this.resolveCJMODImport(options, searchPaths, candidates);
                break;

            default:
                if (options.isOriginImport) {
                    foundModule = await this.resolveOriginImport(options, searchPaths, candidates);
                }
                break;
        }

        return {
            found: !!foundModule,
            module: foundModule,
            candidates,
            searchPaths
        };
    }

    private async resolveMediaImport(options: ImportSearchOptions, searchPaths: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // 媒体文件导入: @Html, @Style, @JavaScript
        // 如果没有as语法，则跳过；如果有as语法，则创建相应类型的带名原始嵌入节点
        // 媒体导入只在编译文件所在目录（非递归）搜索
        
        if (!options.hasAsClause) {
            return undefined; // 跳过没有as语法的媒体导入
        }

        const targetExtension = this.getExtensionForMediaType(options.importType);
        const importPath = options.importPath;

        // 获取当前编译文件所在目录
        const activeEditor = vscode.window.activeTextEditor;
        if (!activeEditor || (activeEditor.document.languageId !== 'chtl' && activeEditor.document.languageId !== 'chtl-js')) {
            // 如果没有活动的CHTL文件，使用工作区根目录
            return this.searchMediaInDirectory(importPath, options.workspaceRoot, targetExtension, candidates);
        }

        const currentFileDir = path.dirname(activeEditor.document.uri.fsPath);
        return this.searchMediaInDirectory(importPath, currentFileDir, targetExtension, candidates);
    }

    private async resolveCHTLImport(options: ImportSearchOptions, searchPaths: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // CHTL导入搜索策略：
        // 1. 名称（不带后缀）：优先搜索官方模块目录，其次搜索当前目录module文件夹，最后搜索当前目录
        // 2. 具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件
        // 3. 具体路径（含文件信息）：直接按路径查找
        // 4. 通配符支持：*.cmod, *.chtl, 具体路径.*

        const importPath = options.importPath;

        if (this.isWildcardPath(importPath)) {
            return this.searchWildcardFiles(importPath, searchPaths, ['cmod', 'chtl'], candidates);
        } else if (this.isAbsolutePath(importPath)) {
            return this.searchAbsolutePath(importPath, ['cmod', 'chtl'], candidates);
        } else if (this.isSpecificFile(importPath)) {
            return this.searchInOrderedPaths(importPath, searchPaths, candidates);
        } else {
            return this.searchModuleByName(importPath, searchPaths, ['cmod', 'chtl'], candidates);
        }
    }

    private async resolveCJMODImport(options: ImportSearchOptions, searchPaths: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // CJMOD导入搜索策略：与CHTL相同，但仅匹配cjmod文件
        const importPath = options.importPath;

        if (this.isWildcardPath(importPath)) {
            return this.searchWildcardFiles(importPath, searchPaths, ['cjmod'], candidates);
        } else if (this.isAbsolutePath(importPath)) {
            return this.searchAbsolutePath(importPath, ['cjmod'], candidates);
        } else if (this.isSpecificFile(importPath)) {
            return this.searchInOrderedPaths(importPath, searchPaths, candidates);
        } else {
            return this.searchModuleByName(importPath, searchPaths, ['cjmod'], candidates);
        }
    }

    private async resolveOriginImport(options: ImportSearchOptions, searchPaths: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // 原始嵌入导入：这是CHTL的语法标记，不是文件导入
        // [Origin] ORIGIN_VUE/REACT/ANGULAR/CUSTOM 是告诉编译器使用原始格式，不进行CHTL解析
        // 这些不对应实际文件，只是语法标识符
        
        const importType = options.importType;
        
        // 创建一个虚拟的模块信息，表示这是一个原始嵌入标记
        const moduleInfo: ModuleInfo = {
            name: importType.replace('@', '').toLowerCase(),
            path: `[Origin] ${importType}`, // 虚拟路径，表示这是语法标记
            type: 'html', // 占位类型
            exports: [],
            imports: [],
            isOfficial: true // 标记为官方语法
        };
        
        candidates.push(moduleInfo);
        return moduleInfo;
    }

    private async searchMediaInDirectory(importPath: string, baseDir: string, expectedExtension: string, candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // 媒体导入专用：只在指定目录（非递归）搜索
        
        if (this.isAbsolutePath(importPath)) {
            // 绝对路径：直接检查
            if (fs.existsSync(importPath)) {
                const actualExtension = path.extname(importPath).slice(1);
                if (!expectedExtension || actualExtension === expectedExtension) {
                    const moduleInfo = await this.createModuleInfo(importPath, this.getFileType(importPath), false);
                    candidates.push(moduleInfo);
                    return moduleInfo;
                }
            }
            return undefined;
        }

        // 相对路径：在当前目录查找
        const fullPath = path.join(baseDir, importPath);
        
        if (fs.existsSync(fullPath)) {
            const actualExtension = path.extname(fullPath).slice(1);
            if (!expectedExtension || actualExtension === expectedExtension) {
                const moduleInfo = await this.createModuleInfo(fullPath, this.getFileType(fullPath), false);
                candidates.push(moduleInfo);
                return moduleInfo;
            }
        }

        // 如果没有扩展名，尝试添加预期的扩展名
        if (!path.extname(importPath) && expectedExtension) {
            const fullPathWithExt = path.join(baseDir, `${importPath}.${expectedExtension}`);
            if (fs.existsSync(fullPathWithExt)) {
                const moduleInfo = await this.createModuleInfo(fullPathWithExt, expectedExtension as any, false);
                candidates.push(moduleInfo);
                return moduleInfo;
            }
        }

        return undefined;
    }

    private getExtensionForMediaType(importType: string): string {
        switch (importType) {
            case '@Html': return 'html';
            case '@Style': return 'css';
            case '@JavaScript': return 'js';
            default: return '';
        }
    }

    private isSpecificFile(path: string): boolean {
        return path.includes('.') && !path.endsWith('/') && !path.includes('*');
    }

    private isFileName(path: string): boolean {
        return !path.includes('/') && !path.includes('.') && !path.includes('*');
    }

    private isWildcardPath(path: string): boolean {
        return path.includes('*');
    }

    private isAbsolutePath(path: string): boolean {
        return path.startsWith('/') || path.includes(':');
    }



    private async searchModuleByName(moduleName: string, searchPaths: string[], allowedExtensions: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // 按优先级搜索：所有搜索路径（已经包含正确的目录结构）
        for (const searchPath of searchPaths) {
            if (!fs.existsSync(searchPath)) continue;

            for (const extension of allowedExtensions) {
                const targetFile = path.join(searchPath, `${moduleName}.${extension}`);
                
                if (fs.existsSync(targetFile)) {
                    const isOfficial = searchPath.includes('official') || searchPath.includes(this.context.extensionPath);
                    const moduleInfo = await this.createModuleInfo(targetFile, extension as any, isOfficial);
                    candidates.push(moduleInfo);
                    
                    // 返回第一个找到的模块（按优先级）
                    return moduleInfo;
                }
            }
        }

        return candidates.length > 0 ? candidates[0] : undefined;
    }

    private async searchWildcardFiles(wildcardPath: string, searchPaths: string[], allowedExtensions: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        // 处理通配符导入：具体路径.*、具体路径/*.cmod等
        const foundModules: ModuleInfo[] = [];

        for (const searchPath of searchPaths) {
            try {
                const pattern = path.join(searchPath, wildcardPath);
                const files = glob.sync(pattern);

                for (const file of files) {
                    const extension = path.extname(file).slice(1);
                    if (allowedExtensions.includes(extension)) {
                        const moduleInfo = await this.createModuleInfo(file, extension as any, false);
                        foundModules.push(moduleInfo);
                        candidates.push(moduleInfo);
                    }
                }
            } catch (error) {
                console.warn(`Error searching wildcard pattern ${wildcardPath} in ${searchPath}:`, error);
            }
        }

        return foundModules.length > 0 ? foundModules[0] : undefined;
    }

    private async searchAbsolutePath(absolutePath: string, allowedExtensions: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        if (fs.existsSync(absolutePath)) {
            const extension = path.extname(absolutePath).slice(1);
            if (allowedExtensions.includes(extension)) {
                const moduleInfo = await this.createModuleInfo(absolutePath, extension as any, false);
                candidates.push(moduleInfo);
                return moduleInfo;
            }
        }

        return undefined;
    }

    private async searchInOrderedPaths(fileName: string, searchPaths: string[], candidates: ModuleInfo[]): Promise<ModuleInfo | undefined> {
        for (const searchPath of searchPaths) {
            const fullPath = path.join(searchPath, fileName);
            
            if (fs.existsSync(fullPath)) {
                const extension = path.extname(fileName).slice(1);
                const moduleInfo = await this.createModuleInfo(fullPath, this.getFileType(fullPath), false);
                candidates.push(moduleInfo);
                return moduleInfo;
            }
        }

        return undefined;
    }

    private async createModuleInfo(filePath: string, type: ModuleInfo['type'], isOfficial: boolean): Promise<ModuleInfo> {
        const name = path.basename(filePath, path.extname(filePath));
        
        // 缓存检查
        const cacheKey = `${filePath}:${type}`;
        if (this.moduleCache.has(cacheKey)) {
            return this.moduleCache.get(cacheKey)!;
        }

        const moduleInfo: ModuleInfo = {
            name,
            path: filePath,
            type,
            exports: [],
            imports: [],
            isOfficial
        };

        // 解析模块内容获取导出和导入信息
        try {
            const content = fs.readFileSync(filePath, 'utf8');
            moduleInfo.exports = this.parseExports(content, type);
            moduleInfo.imports = this.parseImports(content, type);
        } catch (error) {
            console.warn(`Error parsing module ${filePath}:`, error);
        }

        this.moduleCache.set(cacheKey, moduleInfo);
        return moduleInfo;
    }

    private parseExports(content: string, type: ModuleInfo['type']): string[] {
        const exports: string[] = [];
        
        switch (type) {
            case 'chtl':
            case 'cmod':
                // 解析CHTL导出：[Template] @Style, [Custom] @Element等
                const chtlExportMatches = content.match(/\[(Template|Custom)\]\s*@(Style|Element|Var)\s+(\w+)/g);
                if (chtlExportMatches) {
                    for (const match of chtlExportMatches) {
                        const nameMatch = match.match(/(\w+)$/);
                        if (nameMatch) {
                            exports.push(nameMatch[1]);
                        }
                    }
                }
                break;

            case 'cjmod':
                // 解析CJMOD导出：function declarations等
                const functionMatches = content.match(/function\s+(\w+)/g);
                if (functionMatches) {
                    for (const match of functionMatches) {
                        const nameMatch = match.match(/function\s+(\w+)/);
                        if (nameMatch) {
                            exports.push(nameMatch[1]);
                        }
                    }
                }
                break;
        }

        return exports;
    }

    private parseImports(content: string, type: ModuleInfo['type']): string[] {
        const imports: string[] = [];
        
        // 解析导入语句
        const importMatches = content.match(/\[Import\]\s*@\w+\s+from\s+["`']([^"`']+)["`']/g);
        if (importMatches) {
            for (const match of importMatches) {
                const pathMatch = match.match(/from\s+["`']([^"`']+)["`']/);
                if (pathMatch) {
                    imports.push(pathMatch[1]);
                }
            }
        }

        return imports;
    }

    private getFileType(filePath: string): ModuleInfo['type'] {
        const extension = path.extname(filePath).slice(1).toLowerCase();
        
        switch (extension) {
            case 'chtl': return 'chtl';
            case 'cmod': return 'cmod';
            case 'cjmod': return 'cjmod';
            case 'html': return 'html';
            case 'css': return 'css';
            case 'js': return 'js';
            case 'vue': return 'vue';
            case 'jsx': return 'react';
            case 'tsx': return 'react';
            default: return 'chtl';
        }
    }

    public async getAllModules(workspaceRoot: string): Promise<ModuleInfo[]> {
        await this.ensureCacheValid();
        
        const searchPaths = this.searchPathCache.get(workspaceRoot) || [];
        const allModules: ModuleInfo[] = [];

        for (const searchPath of searchPaths) {
            try {
                const files = glob.sync('**/*.{chtl,cmod,cjmod}', { cwd: searchPath });
                
                for (const file of files) {
                    const fullPath = path.join(searchPath, file);
                    const type = this.getFileType(fullPath);
                    const isOfficial = searchPath.includes(this.context.extensionPath);
                    
                    const moduleInfo = await this.createModuleInfo(fullPath, type, isOfficial);
                    allModules.push(moduleInfo);
                }
            } catch (error) {
                console.warn(`Error scanning modules in ${searchPath}:`, error);
            }
        }

        return allModules;
    }

    public async refreshCache(): Promise<void> {
        this.moduleCache.clear();
        this.searchPathCache.clear();
        this.lastCacheUpdate = 0;
        this.initializeSearchPaths();
    }

    /**
     * 获取当前搜索路径（用于调试）
     */
    public getSearchPaths(workspaceRoot?: string): string[] {
        if (!workspaceRoot) {
            const workspaceFolders = vscode.workspace.workspaceFolders;
            if (!workspaceFolders || workspaceFolders.length === 0) {
                return [];
            }
            workspaceRoot = workspaceFolders[0].uri.fsPath;
        }
        
        return this.searchPathCache.get(workspaceRoot) || [];
    }

    /**
     * 显示搜索路径调试信息
     */
    public showSearchPathsDebugInfo(): void {
        const workspaceFolders = vscode.workspace.workspaceFolders;
        if (!workspaceFolders || workspaceFolders.length === 0) {
            vscode.window.showInformationMessage('没有打开的工作区');
            return;
        }

        let debugInfo = '### CHTL 模块搜索路径 ###\n\n';
        
        for (const folder of workspaceFolders) {
            const workspaceRoot = folder.uri.fsPath;
            const searchPaths = this.getSearchPaths(workspaceRoot);
            
            debugInfo += `工作区: ${folder.name}\n`;
            debugInfo += `路径: ${workspaceRoot}\n\n`;
            debugInfo += '搜索路径:\n';
            
            if (searchPaths.length === 0) {
                debugInfo += '  (无搜索路径)\n';
            } else {
                searchPaths.forEach((searchPath, index) => {
                    const exists = fs.existsSync(searchPath);
                    debugInfo += `  ${index + 1}. ${searchPath} ${exists ? '✓' : '✗'}\n`;
                });
            }
            debugInfo += '\n';
        }

        debugInfo += '搜索策略说明:\n';
        debugInfo += '1. 编译器所在目录的 module/modules 文件夹\n';
        debugInfo += '2. 当前编译文件所在目录的 module/modules 文件夹\n';
        debugInfo += '3. 当前编译文件所在目录（不递归）\n';
        debugInfo += '4. 源码目录（src, source, lib, app）的 module/modules 文件夹\n';
        debugInfo += '5. 工作区根目录的 module/modules 文件夹\n';
        debugInfo += '6. 配置的额外搜索路径\n\n';
        debugInfo += '支持的目录结构:\n';
        debugInfo += '- 无序结构：所有文件直接在 module 文件夹根目录\n';
        debugInfo += '- 有序结构：CMOD/Cmod/cmod 和 CJMOD/CJmod/cjmod 分类文件夹\n';

        // 创建并显示输出面板
        const outputChannel = vscode.window.createOutputChannel('CHTL Module Paths');
        outputChannel.clear();
        outputChannel.appendLine(debugInfo);
        outputChannel.show();
    }

    private async ensureCacheValid(): Promise<void> {
        const now = Date.now();
        if (now - this.lastCacheUpdate > this.CACHE_TTL) {
            await this.refreshCache();
            this.lastCacheUpdate = now;
        }
    }

    private invalidateCache(): void {
        this.lastCacheUpdate = 0;
    }
}