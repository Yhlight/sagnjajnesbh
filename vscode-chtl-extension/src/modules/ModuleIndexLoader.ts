import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export interface ModuleIndexEntry {
    name: string;
    type: 'CMOD' | 'CJMOD';
    file: string;
    size: number;
    category: string;
    description: string;
    version: string;
    hash?: string;
    lastModified?: string;
    exports?: Array<{
        name: string;
        type: 'component' | 'function' | 'variable' | 'style';
        description?: string;
    }>;
    structure?: {
        hasInfo: boolean;
        hasSrc: boolean;
        sourceFiles: string[];
        totalFiles: number;
    };
}

export interface ModuleIndex {
    version: string;
    buildDate: string;
    totalModules: number;
    byType: {
        CMOD: ModuleIndexEntry[];
        CJMOD: ModuleIndexEntry[];
    };
    byCategory: {
        official: ModuleIndexEntry[];
        user: ModuleIndexEntry[];
    };
    byName: { [name: string]: ModuleIndexEntry };
    byExport: { 
        [exportName: string]: Array<{
            moduleName: string;
            moduleType: 'CMOD' | 'CJMOD';
            exportType: string;
            category: string;
        }>;
    };
    statistics: {
        totalSize: number;
        cmodCount: number;
        cjmodCount: number;
        officialCount: number;
        userCount: number;
        packedCount: number;
        sourceCount: number;
    };
    modules: ModuleIndexEntry[];
}

/**
 * 模块索引加载器 - 从JSON文件加载模块索引，替代手动内置模块
 */
export class ModuleIndexLoader {
    private moduleIndex: ModuleIndex | null = null;
    private compactIndex: Partial<ModuleIndex> | null = null;
    private indexPath: string;
    private compactIndexPath: string;
    
    constructor(private context: vscode.ExtensionContext) {
        this.indexPath = path.join(context.extensionPath, 'modules', 'module-index.json');
        this.compactIndexPath = path.join(context.extensionPath, 'modules', 'module-index.compact.json');
    }
    
    /**
     * 加载模块索引
     */
    async loadIndex(): Promise<boolean> {
        try {
            // 优先加载压缩索引用于快速启动
            if (await this.loadCompactIndex()) {
                // 后台加载完整索引
                setTimeout(() => this.loadFullIndex(), 100);
                return true;
            }
            
            // 如果压缩索引不存在，加载完整索引
            return await this.loadFullIndex();
            
        } catch (error) {
            console.error('Failed to load module index:', error);
            return false;
        }
    }
    
    /**
     * 加载压缩索引（快速启动）
     */
    private async loadCompactIndex(): Promise<boolean> {
        if (!fs.existsSync(this.compactIndexPath)) {
            return false;
        }
        
        try {
            const content = fs.readFileSync(this.compactIndexPath, 'utf8');
            this.compactIndex = JSON.parse(content);
            
            console.log(`✅ 压缩模块索引加载成功: ${this.compactIndex?.statistics?.totalModules || 0} 个模块`);
            return true;
        } catch (error) {
            console.warn('Failed to load compact module index:', error);
            return false;
        }
    }
    
    /**
     * 加载完整索引
     */
    private async loadFullIndex(): Promise<boolean> {
        if (!fs.existsSync(this.indexPath)) {
            console.error('Module index file not found:', this.indexPath);
            return false;
        }
        
        try {
            const content = fs.readFileSync(this.indexPath, 'utf8');
            this.moduleIndex = JSON.parse(content);
            
            console.log(`✅ 完整模块索引加载成功: ${this.moduleIndex.totalModules} 个模块`);
            console.log(`   📦 CMOD: ${this.moduleIndex.statistics.cmodCount}, CJMOD: ${this.moduleIndex.statistics.cjmodCount}`);
            console.log(`   🏢 官方: ${this.moduleIndex.statistics.officialCount}, 👤 用户: ${this.moduleIndex.statistics.userCount}`);
            
            return true;
        } catch (error) {
            console.error('Failed to load full module index:', error);
            return false;
        }
    }
    
    /**
     * 获取所有模块名称（用于智能提示）
     */
    getModuleNames(): string[] {
        if (this.compactIndex?.byName) {
            return Object.keys(this.compactIndex.byName);
        }
        if (this.moduleIndex?.byName) {
            return Object.keys(this.moduleIndex.byName);
        }
        return [];
    }
    
    /**
     * 根据名称查找模块
     */
    findModule(name: string): ModuleIndexEntry | null {
        const normalizedName = name.toLowerCase();
        
        // 先从压缩索引查找
        if (this.compactIndex?.byName) {
            const module = this.compactIndex.byName[normalizedName];
            if (module) return module as ModuleIndexEntry;
        }
        
        // 从完整索引查找
        if (this.moduleIndex?.byName) {
            const module = this.moduleIndex.byName[normalizedName];
            if (module) return module;
        }
        
        return null;
    }
    
    /**
     * 根据导出项查找模块
     */
    findModulesByExport(exportName: string): Array<{
        moduleName: string;
        moduleType: 'CMOD' | 'CJMOD';
        exportType: string;
        category: string;
    }> {
        const normalizedName = exportName.toLowerCase();
        
        if (this.compactIndex?.byExport) {
            return this.compactIndex.byExport[normalizedName] || [];
        }
        if (this.moduleIndex?.byExport) {
            return this.moduleIndex.byExport[normalizedName] || [];
        }
        
        return [];
    }
    
    /**
     * 获取指定类型的模块
     */
    getModulesByType(type: 'CMOD' | 'CJMOD'): ModuleIndexEntry[] {
        if (this.moduleIndex?.byType) {
            return this.moduleIndex.byType[type] || [];
        }
        return [];
    }
    
    /**
     * 获取指定分类的模块
     */
    getModulesByCategory(category: string): ModuleIndexEntry[] {
        if (this.moduleIndex?.byCategory) {
            return (this.moduleIndex.byCategory as any)[category] || [];
        }
        return [];
    }
    
    /**
     * 获取模块的完整路径
     */
    getModuleFilePath(moduleName: string): string | null {
        const module = this.findModule(moduleName);
        if (!module) return null;
        
        return path.join(this.context.extensionPath, 'modules', module.file);
    }
    
    /**
     * 检查模块是否存在
     */
    moduleExists(name: string): boolean {
        return this.findModule(name) !== null;
    }
    
    /**
     * 获取模块统计信息
     */
    getStatistics() {
        if (this.compactIndex?.statistics) {
            return this.compactIndex.statistics;
        }
        if (this.moduleIndex?.statistics) {
            return this.moduleIndex.statistics;
        }
        return null;
    }
    
    /**
     * 获取所有组件名称（用于智能提示）
     */
    getAllComponentNames(): string[] {
        const components: string[] = [];
        
        if (this.moduleIndex?.modules) {
            for (const module of this.moduleIndex.modules) {
                if (module.exports) {
                    for (const exp of module.exports) {
                        if (exp.type === 'component') {
                            components.push(exp.name);
                        }
                    }
                }
            }
        }
        
        return components;
    }
    
    /**
     * 获取所有函数名称（用于智能提示）
     */
    getAllFunctionNames(): string[] {
        const functions: string[] = [];
        
        if (this.moduleIndex?.modules) {
            for (const module of this.moduleIndex.modules) {
                if (module.exports) {
                    for (const exp of module.exports) {
                        if (exp.type === 'function') {
                            functions.push(exp.name);
                        }
                    }
                }
            }
        }
        
        return functions;
    }
    
    /**
     * 获取所有变量名称（用于智能提示）
     */
    getAllVariableNames(): string[] {
        const variables: string[] = [];
        
        if (this.moduleIndex?.modules) {
            for (const module of this.moduleIndex.modules) {
                if (module.exports) {
                    for (const exp of module.exports) {
                        if (exp.type === 'variable') {
                            variables.push(exp.name);
                        }
                    }
                }
            }
        }
        
        return variables;
    }
    
    /**
     * 刷新索引（重新加载）
     */
    async refresh(): Promise<boolean> {
        this.moduleIndex = null;
        this.compactIndex = null;
        return await this.loadIndex();
    }
    
    /**
     * 获取索引版本信息
     */
    getIndexVersion(): string {
        if (this.compactIndex?.version) {
            return this.compactIndex.version;
        }
        if (this.moduleIndex?.version) {
            return this.moduleIndex.version;
        }
        return 'unknown';
    }
    
    /**
     * 获取索引构建日期
     */
    getIndexBuildDate(): string {
        if (this.compactIndex?.buildDate) {
            return this.compactIndex.buildDate;
        }
        if (this.moduleIndex?.buildDate) {
            return this.moduleIndex.buildDate;
        }
        return 'unknown';
    }
}