import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import * as glob from 'glob';
import { ModuleIndex, ModuleIndexEntry, ModuleIndexBuilder } from './moduleIndexBuilder';

export interface ModuleInfo {
    name: string;
    path: string;
    type: 'chtl' | 'cmod' | 'cjmod' | 'hybrid' | 'html' | 'css' | 'js' | 'vue' | 'react' | 'angular';
    exports: string[];
    imports: string[];
    version?: string;
    description?: string;
    isOfficial: boolean;
    // 混合模块特有属性
    subModules?: {
        cmod: string[];  // CMOD子模块列表
        cjmod: string[]; // CJMOD子模块列表
    };
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
    
    // 新增：索引相关
    private officialModuleIndex: ModuleIndex | null = null;
    private projectModuleIndex: ModuleIndex | null = null;
    private indexBuilder: ModuleIndexBuilder;

    constructor(context: vscode.ExtensionContext, config: vscode.WorkspaceConfiguration) {
        this.context = context;
        this.config = config;
        this.indexBuilder = new ModuleIndexBuilder(context.extensionPath);
        this.initializeSearchPaths();
        this.loadOfficialModuleIndex();
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

        // 1. 官方模块目录（内置编译器同级的module目录）
        // 这是最重要的搜索路径，因为官方模块与编译器一起打包
        const builtInOfficialPath = path.join(this.context.extensionPath, 'bin', 'module');
        this.addModulePaths(searchPaths, builtInOfficialPath);
        
        // 验证官方模块目录是否存在
        if (!fs.existsSync(builtInOfficialPath)) {
            console.warn(`内置官方模块目录不存在: ${builtInOfficialPath}`);
            console.warn('这可能是因为扩展打包时未包含内置编译器和官方模块');
        }
        
        // 兼容配置的官方模块路径（用于开发测试）
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
        // 原始嵌入导入：[Import] [Origin] @Html box from "path/to/file.chtl"
        // 这是导入其他CHTL文件中定义的带名原始嵌入块
        
        const { importPath, importType, elementName, alias } = options;
        
        if (!importPath) {
            console.error('原始嵌入导入必须指定源文件路径');
            return undefined;
        }
        
        if (!elementName) {
            console.error('原始嵌入导入必须指定原始嵌入块名称');
            return undefined;
        }

        // 解析目标CHTL文件路径
        const targetFilePath = await this.resolveFilePath(importPath, options.currentFileDir, searchPaths);
        if (!targetFilePath) {
            console.error(`找不到原始嵌入源文件: ${importPath}`);
            return undefined;
        }

        // 检查文件是否存在且为CHTL文件
        if (!fs.existsSync(targetFilePath)) {
            console.error(`原始嵌入源文件不存在: ${targetFilePath}`);
            return undefined;
        }

        if (!targetFilePath.endsWith('.chtl')) {
            console.error(`原始嵌入只能从CHTL文件导入: ${targetFilePath}`);
            return undefined;
        }

        try {
            // 读取并解析CHTL文件内容，查找指定的带名原始嵌入
            const fileContent = fs.readFileSync(targetFilePath, 'utf-8');
            const originContent = this.extractNamedOriginEmbedding(fileContent, importType, elementName);
            
            if (!originContent) {
                console.error(`在文件 ${targetFilePath} 中找不到原始嵌入: [Origin] ${importType} ${elementName}`);
                return undefined;
            }

            const moduleInfo: ModuleInfo = {
                name: alias || elementName,
                path: targetFilePath,
                type: this.getOriginType(importType),
                exports: [{
                    name: elementName,
                    type: 'origin'
                }],
                imports: [],
                isOfficial: false,
                content: originContent,
                lastModified: fs.statSync(targetFilePath).mtime.getTime(),
                size: originContent.length
            };

            candidates.push(moduleInfo);
            console.log(`成功解析原始嵌入: [Origin] ${importType} ${elementName} from ${targetFilePath}`);
            return moduleInfo;
        } catch (error) {
            console.error(`解析原始嵌入导入失败: ${error}`);
                    return undefined;
    }

    /**
     * 加载官方模块索引
     */
    private async loadOfficialModuleIndex(): Promise<void> {
        try {
            const indexPath = path.join(this.context.extensionPath, 'bin', 'module-index.json');
            this.officialModuleIndex = await this.indexBuilder.loadIndexFromFile(indexPath);
            
            if (this.officialModuleIndex) {
                console.log(`✅ 官方模块索引已加载: ${this.officialModuleIndex.modules.length} 个模块`);
                this.populateCacheFromIndex(this.officialModuleIndex);
            } else {
                console.warn('⚠️ 官方模块索引未找到，将回退到实时扫描模式');
            }
        } catch (error) {
            console.error('❌ 加载官方模块索引失败:', error);
        }
    }

    /**
     * 加载项目模块索引
     */
    private async loadProjectModuleIndex(projectPath: string): Promise<void> {
        try {
            const indexPath = path.join(projectPath, 'module-index.json');
            
            // 检查是否需要重建索引
            const modulesPath = path.join(projectPath, 'module');
            const needsUpdate = await this.indexBuilder.needsUpdate(indexPath, modulesPath);
            
            if (needsUpdate) {
                console.log('🔄 项目模块索引需要更新，正在重建...');
                this.projectModuleIndex = await this.indexBuilder.buildProjectModuleIndex(projectPath);
                await this.indexBuilder.saveIndexToFile(this.projectModuleIndex, indexPath);
            } else {
                this.projectModuleIndex = await this.indexBuilder.loadIndexFromFile(indexPath);
            }
            
            if (this.projectModuleIndex) {
                console.log(`✅ 项目模块索引已加载: ${this.projectModuleIndex.modules.length} 个模块`);
                this.populateCacheFromIndex(this.projectModuleIndex);
            }
        } catch (error) {
            console.error('❌ 加载项目模块索引失败:', error);
        }
    }

    /**
     * 从索引填充缓存
     */
    private populateCacheFromIndex(index: ModuleIndex): void {
        for (const moduleEntry of index.modules) {
            const moduleInfo: ModuleInfo = {
                name: moduleEntry.moduleName,
                path: moduleEntry.filePath,
                type: this.mapModuleType(moduleEntry.moduleType),
                exports: moduleEntry.exports.map(exp => exp.name),
                imports: moduleEntry.dependencies,
                isOfficial: moduleEntry.isOfficial,
                version: moduleEntry.version,
                description: moduleEntry.description
            };
            
            this.moduleCache.set(moduleEntry.moduleName, moduleInfo);
        }
    }

    /**
     * 映射模块类型
     */
    private mapModuleType(moduleType: 'chtl' | 'cmod' | 'cjmod'): ModuleInfo['type'] {
        const typeMap: { [key: string]: ModuleInfo['type'] } = {
            'chtl': 'chtl',
            'cmod': 'cmod', 
            'cjmod': 'cjmod'
        };
        return typeMap[moduleType] || 'chtl';
    }

    /**
     * 快速搜索模块（使用索引）
     */
    public searchModulesWithIndex(query: string, type?: 'chtl' | 'cmod' | 'cjmod'): ModuleInfo[] {
        const results: ModuleInfo[] = [];
        const allIndices = [this.officialModuleIndex, this.projectModuleIndex].filter(Boolean) as ModuleIndex[];
        
        for (const index of allIndices) {
            // 按模块名搜索
            const moduleIndex = index.searchMap.byName[query];
            if (moduleIndex !== undefined) {
                const moduleEntry = index.modules[moduleIndex];
                if (!type || moduleEntry.moduleType === type) {
                    results.push(this.convertToModuleInfo(moduleEntry));
                }
            }

            // 按导出名搜索
            const exportIndices = index.searchMap.byExport[query] || [];
            for (const idx of exportIndices) {
                const moduleEntry = index.modules[idx];
                if (!type || moduleEntry.moduleType === type) {
                    const moduleInfo = this.convertToModuleInfo(moduleEntry);
                    if (!results.some(r => r.name === moduleInfo.name)) {
                        results.push(moduleInfo);
                    }
                }
            }
        }
        
        return results;
    }

    /**
     * 获取模块的详细导出信息
     */
    public getModuleExports(moduleName: string): { name: string; type: string; signature?: string; description?: string; }[] {
        const allIndices = [this.officialModuleIndex, this.projectModuleIndex].filter(Boolean) as ModuleIndex[];
        
        for (const index of allIndices) {
            const moduleIndex = index.searchMap.byName[moduleName];
            if (moduleIndex !== undefined) {
                const moduleEntry = index.modules[moduleIndex];
                return moduleEntry.exports.map(exp => ({
                    name: exp.name,
                    type: exp.type,
                    signature: exp.signature,
                    description: exp.description
                }));
            }
        }
        
        return [];
    }

    /**
     * 转换为ModuleInfo格式
     */
    private convertToModuleInfo(moduleEntry: ModuleIndexEntry): ModuleInfo {
        return {
            name: moduleEntry.moduleName,
            path: moduleEntry.filePath,
            type: this.mapModuleType(moduleEntry.moduleType),
            exports: moduleEntry.exports.map(exp => exp.name),
            imports: moduleEntry.dependencies,
            isOfficial: moduleEntry.isOfficial,
            version: moduleEntry.version,
            description: moduleEntry.description
        };
    }

    /**
     * 获取所有已缓存的模块（包括索引中的）
     */
    public getAllModulesFromIndex(): ModuleInfo[] {
        const allModules: ModuleInfo[] = [];
        const allIndices = [this.officialModuleIndex, this.projectModuleIndex].filter(Boolean) as ModuleIndex[];
        
        for (const index of allIndices) {
            for (const moduleEntry of index.modules) {
                allModules.push(this.convertToModuleInfo(moduleEntry));
            }
        }
        
        return allModules;
    }

    /**
     * 获取官方模块列表（使用索引）
     */
    public getOfficialModulesFromIndex(): ModuleInfo[] {
        if (!this.officialModuleIndex) return [];
        
        return this.officialModuleIndex.modules
            .filter(module => module.isOfficial)
            .map(module => this.convertToModuleInfo(module));
    }

    /**
     * 刷新项目模块索引
     */
    public async refreshProjectModuleIndex(): Promise<void> {
        const workspaceFolders = vscode.workspace.workspaceFolders;
        if (workspaceFolders && workspaceFolders.length > 0) {
            await this.loadProjectModuleIndex(workspaceFolders[0].uri.fsPath);
        }
    }

    /**
     * 检查模块索引状态
     */
    public getIndexStatus(): { official: boolean; project: boolean; officialCount: number; projectCount: number } {
        return {
            official: this.officialModuleIndex !== null,
            project: this.projectModuleIndex !== null,
            officialCount: this.officialModuleIndex?.modules.length || 0,
            projectCount: this.projectModuleIndex?.modules.length || 0
        };
    }
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

    /**
     * 从CHTL文件内容中提取指定的带名原始嵌入
     */
    private extractNamedOriginEmbedding(fileContent: string, originType: string, originName: string): string | null {
        try {
            // 构建正则表达式来匹配带名原始嵌入块
            // 例如：[Origin] @Html box { ... }
            const escapedType = originType.replace('@', '\\@');
            const originRegex = new RegExp(
                `\\[Origin\\]\\s*${escapedType}\\s+${originName}\\s*\\{([\\s\\S]*?)\\}`,
                'i'
            );

            const match = fileContent.match(originRegex);
            if (match && match[1]) {
                return match[1].trim();
            }

            return null;
        } catch (error) {
            console.error(`提取原始嵌入内容失败: ${error}`);
            return null;
        }
    }

    /**
     * 根据原始嵌入类型返回对应的模块类型
     */
    private getOriginType(originType: string): 'html' | 'css' | 'javascript' | 'chtl' | 'cjmod' {
        switch (originType) {
            case '@Html': return 'html';
            case '@Style': return 'css';
            case '@JavaScript': return 'javascript';
            default: return 'html'; // 自定义类型默认当作HTML处理
        }
    }

    /**
     * 解析文件路径，支持相对路径和绝对路径
     */
    private async resolveFilePath(importPath: string, currentFileDir?: string, searchPaths?: string[]): Promise<string | null> {
        // 处理绝对路径
        if (this.isAbsolutePath(importPath)) {
            return fs.existsSync(importPath) ? importPath : null;
        }

        // 处理相对路径
        const baseDirs = [
            currentFileDir || '',
            ...(searchPaths || []),
            this.workspaceRoot
        ].filter(Boolean);

        for (const baseDir of baseDirs) {
            const fullPath = path.join(baseDir, importPath);
            if (fs.existsSync(fullPath)) {
                return fullPath;
            }

            // 尝试添加.chtl扩展名
            const fullPathWithExt = `${fullPath}.chtl`;
            if (fs.existsSync(fullPathWithExt)) {
                return fullPathWithExt;
            }
        }

        return null;
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

        // 处理混合模块
        if (type === 'hybrid') {
            const modulePath = path.dirname(filePath);
            moduleInfo.subModules = this.getHybridSubModules(modulePath);
        }

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

            case 'hybrid':
                // 混合模块：同时支持CHTL和CJMOD导出格式
                // 解析CHTL导出
                const hybridChtlMatches = content.match(/\[(Template|Custom)\]\s*@(Style|Element|Var)\s+(\w+)/g);
                if (hybridChtlMatches) {
                    for (const match of hybridChtlMatches) {
                        const nameMatch = match.match(/(\w+)$/);
                        if (nameMatch) {
                            exports.push(`${nameMatch[1]} (CMOD)`);
                        }
                    }
                }
                
                // 解析CJMOD导出
                const hybridFunctionMatches = content.match(/function\s+(\w+)/g);
                if (hybridFunctionMatches) {
                    for (const match of hybridFunctionMatches) {
                        const nameMatch = match.match(/function\s+(\w+)/);
                        if (nameMatch) {
                            exports.push(`${nameMatch[1]} (CJMOD)`);
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
            case 'cmod': 
                // 检查是否是混合模块
                const dirPath = path.dirname(filePath);
                if (this.isHybridModule(dirPath)) {
                    return 'hybrid';
                }
                return 'cmod';
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

    /**
     * 检测是否为混合模块结构
     * 混合模块应该包含 CMOD 和 CJMOD 两个子目录
     */
    private isHybridModule(modulePath: string): boolean {
        if (!fs.existsSync(modulePath)) {
            return false;
        }

        const cmodVariants = ['CMOD', 'Cmod', 'cmod'];
        const cjmodVariants = ['CJMOD', 'CJmod', 'cjmod'];
        
        let hasCMOD = false;
        let hasCJMOD = false;

        // 检查CMOD目录
        for (const variant of cmodVariants) {
            const cmodPath = path.join(modulePath, variant);
            if (fs.existsSync(cmodPath) && fs.lstatSync(cmodPath).isDirectory()) {
                hasCMOD = true;
                break;
            }
        }

        // 检查CJMOD目录
        for (const variant of cjmodVariants) {
            const cjmodPath = path.join(modulePath, variant);
            if (fs.existsSync(cjmodPath) && fs.lstatSync(cjmodPath).isDirectory()) {
                hasCJMOD = true;
                break;
            }
        }

        return hasCMOD && hasCJMOD;
    }

    /**
     * 获取混合模块的子模块信息
     */
    private getHybridSubModules(modulePath: string): { cmod: string[], cjmod: string[] } {
        const result = { cmod: [] as string[], cjmod: [] as string[] };
        
        const cmodVariants = ['CMOD', 'Cmod', 'cmod'];
        const cjmodVariants = ['CJMOD', 'CJmod', 'cjmod'];

        // 获取CMOD子模块
        for (const variant of cmodVariants) {
            const cmodPath = path.join(modulePath, variant);
            if (fs.existsSync(cmodPath)) {
                try {
                    const subDirs = fs.readdirSync(cmodPath).filter(item => {
                        const itemPath = path.join(cmodPath, item);
                        return fs.lstatSync(itemPath).isDirectory();
                    });
                    result.cmod.push(...subDirs);
                } catch (error) {
                    console.warn(`读取CMOD子模块失败: ${cmodPath}`, error);
                }
                break;
            }
        }

        // 获取CJMOD子模块
        for (const variant of cjmodVariants) {
            const cjmodPath = path.join(modulePath, variant);
            if (fs.existsSync(cjmodPath)) {
                try {
                    const subDirs = fs.readdirSync(cjmodPath).filter(item => {
                        const itemPath = path.join(cjmodPath, item);
                        return fs.lstatSync(itemPath).isDirectory();
                    });
                    result.cjmod.push(...subDirs);
                } catch (error) {
                    console.warn(`读取CJMOD子模块失败: ${cjmodPath}`, error);
                }
                break;
            }
        }

        return result;
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
        debugInfo += '- 混合结构：同时包含 CMOD 和 CJMOD 子目录的混合模块\n';

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