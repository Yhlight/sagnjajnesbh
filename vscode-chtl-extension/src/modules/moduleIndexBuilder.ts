import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export interface ModuleExportInfo {
    name: string;
    type: 'template' | 'custom' | 'element' | 'style' | 'script' | 'var' | 'function' | 'class' | 'interface';
    signature?: string;
    description?: string;
    parameters?: string[];
    returnType?: string;
}

export interface ModuleIndexEntry {
    moduleName: string;
    moduleType: 'chtl' | 'cmod' | 'cjmod';
    filePath: string;
    isOfficial: boolean;
    version?: string;
    description?: string;
    dependencies: string[];
    exports: ModuleExportInfo[];
    lastModified: number;
    fileSize: number;
}

export interface ModuleIndex {
    version: string;
    timestamp: string;
    structure: 'classified' | 'mixed';
    modules: ModuleIndexEntry[];
    searchMap: {
        byName: { [moduleName: string]: number }; // 指向modules数组的索引
        byExport: { [exportName: string]: number[] }; // 可能多个模块导出同名内容
        byType: { [moduleType: string]: number[] };
        byOfficial: { official: number[], project: number[] };
    };
}

export class ModuleIndexBuilder {
    private extensionPath: string;
    
    constructor(extensionPath: string) {
        this.extensionPath = extensionPath;
    }

    /**
     * 构建官方模块索引
     */
    public async buildOfficialModuleIndex(): Promise<ModuleIndex> {
        const officialModulesPath = path.join(this.extensionPath, 'bin', 'module');
        return this.buildModuleIndex(officialModulesPath, true);
    }

    /**
     * 构建项目模块索引
     */
    public async buildProjectModuleIndex(projectPath: string): Promise<ModuleIndex> {
        const projectModulesPath = path.join(projectPath, 'module');
        return this.buildModuleIndex(projectModulesPath, false);
    }

    /**
     * 构建完整的模块索引
     */
    private async buildModuleIndex(modulesPath: string, isOfficial: boolean): Promise<ModuleIndex> {
        console.log(`📊 开始构建模块索引: ${modulesPath}`);
        
        if (!fs.existsSync(modulesPath)) {
            console.warn(`模块路径不存在: ${modulesPath}`);
            return this.createEmptyIndex();
        }

        const structure = this.detectStructure(modulesPath);
        const modules: ModuleIndexEntry[] = [];

        if (structure === 'classified') {
            await this.scanClassifiedModules(modulesPath, modules, isOfficial);
        } else {
            await this.scanMixedModules(modulesPath, modules, isOfficial);
        }

        const searchMap = this.buildSearchMap(modules);

        const index: ModuleIndex = {
            version: '1.0.0',
            timestamp: new Date().toISOString(),
            structure,
            modules,
            searchMap
        };

        console.log(`✅ 模块索引构建完成: ${modules.length} 个模块`);
        return index;
    }

    /**
     * 检测模块目录结构
     */
    private detectStructure(modulesPath: string): 'classified' | 'mixed' {
        const cmodDirs = ['CMOD', 'Cmod', 'cmod'];
        const cjmodDirs = ['CJMOD', 'CJmod', 'cjmod'];
        
        const hasCmod = cmodDirs.some(dir => fs.existsSync(path.join(modulesPath, dir)));
        const hasCjmod = cjmodDirs.some(dir => fs.existsSync(path.join(modulesPath, dir)));
        
        return (hasCmod && hasCjmod) ? 'classified' : 'mixed';
    }

    /**
     * 扫描分类结构的模块
     */
    private async scanClassifiedModules(modulesPath: string, modules: ModuleIndexEntry[], isOfficial: boolean): Promise<void> {
        // 扫描CMOD目录
        const cmodDirs = ['CMOD', 'Cmod', 'cmod'];
        for (const cmodDir of cmodDirs) {
            const cmodPath = path.join(modulesPath, cmodDir);
            if (fs.existsSync(cmodPath)) {
                await this.scanDirectory(cmodPath, modules, isOfficial, ['chtl', 'cmod']);
                break;
            }
        }

        // 扫描CJMOD目录
        const cjmodDirs = ['CJMOD', 'CJmod', 'cjmod'];
        for (const cjmodDir of cjmodDirs) {
            const cjmodPath = path.join(modulesPath, cjmodDir);
            if (fs.existsSync(cjmodPath)) {
                await this.scanDirectory(cjmodPath, modules, isOfficial, ['cjmod']);
                break;
            }
        }
    }

    /**
     * 扫描混合结构的模块
     */
    private async scanMixedModules(modulesPath: string, modules: ModuleIndexEntry[], isOfficial: boolean): Promise<void> {
        await this.scanDirectory(modulesPath, modules, isOfficial, ['chtl', 'cmod', 'cjmod']);
    }

    /**
     * 扫描指定目录
     */
    private async scanDirectory(directory: string, modules: ModuleIndexEntry[], isOfficial: boolean, allowedExtensions: string[]): Promise<void> {
        if (!fs.existsSync(directory)) return;

        const files = fs.readdirSync(directory);
        
        for (const file of files) {
            const filePath = path.join(directory, file);
            const stat = fs.statSync(filePath);
            
            if (stat.isFile()) {
                const ext = path.extname(file).substring(1);
                if (allowedExtensions.includes(ext)) {
                    try {
                        const moduleEntry = await this.parseModuleFile(filePath, isOfficial);
                        if (moduleEntry) {
                            modules.push(moduleEntry);
                        }
                    } catch (error) {
                        console.error(`解析模块文件失败: ${filePath}`, error);
                    }
                }
            }
        }
    }

    /**
     * 解析模块文件
     */
    private async parseModuleFile(filePath: string, isOfficial: boolean): Promise<ModuleIndexEntry | null> {
        const content = fs.readFileSync(filePath, 'utf8');
        const stat = fs.statSync(filePath);
        const ext = path.extname(filePath).substring(1) as 'chtl' | 'cmod' | 'cjmod';
        const baseName = path.basename(filePath, path.extname(filePath));

        let exports: ModuleExportInfo[] = [];
        let dependencies: string[] = [];
        let version: string | undefined;
        let description: string | undefined;

        if (ext === 'cmod' || ext === 'chtl') {
            // CMOD/CHTL使用[Export]块
            const exportData = this.extractCMODExports(content);
            exports = exportData.exports;
            dependencies = this.extractDependencies(content);
            version = this.extractVersion(content);
            description = this.extractDescription(content);
        } else if (ext === 'cjmod') {
            // CJMOD需要手动查找导出
            const exportData = this.extractCJMODExports(content);
            exports = exportData.exports;
            dependencies = this.extractDependencies(content);
            version = this.extractVersion(content);
            description = this.extractDescription(content);
        }

        return {
            moduleName: baseName,
            moduleType: ext,
            filePath,
            isOfficial,
            version,
            description,
            dependencies,
            exports,
            lastModified: stat.mtime.getTime(),
            fileSize: stat.size
        };
    }

    /**
     * 提取CMOD/CHTL的导出信息（使用[Export]块）
     */
    private extractCMODExports(content: string): { exports: ModuleExportInfo[] } {
        const exports: ModuleExportInfo[] = [];
        
        // 匹配[Export]块
        const exportBlockRegex = /\[Export\]\s*\{([^}]*)\}/gs;
        const exportMatches = content.matchAll(exportBlockRegex);
        
        for (const match of exportMatches) {
            const exportContent = match[1];
            
            // 解析导出项
            const exportLines = exportContent.split('\n')
                .map(line => line.trim())
                .filter(line => line && !line.startsWith('//'));
            
            for (const line of exportLines) {
                const exportInfo = this.parseExportLine(line);
                if (exportInfo) {
                    exports.push(exportInfo);
                }
            }
        }

        // 如果没有[Export]块，尝试查找常见的定义
        if (exports.length === 0) {
            exports.push(...this.extractImplicitCMODExports(content));
        }

        return { exports };
    }

    /**
     * 解析导出行
     */
    private parseExportLine(line: string): ModuleExportInfo | null {
        // 匹配各种导出格式
        // 例如：@Template MyComponent: (props: any) => HTMLElement
        // 例如：@Style button-primary: .btn-primary { ... }
        // 例如：@Var primary-color: #007bff
        
        const patterns = [
            // @Template Name: signature
            /^@Template\s+(\w+)\s*:\s*(.+)$/,
            // @Custom Name: signature  
            /^@Custom\s+(\w+)\s*:\s*(.+)$/,
            // @Element Name: signature
            /^@Element\s+(\w+)\s*:\s*(.+)$/,
            // @Style Name: selector
            /^@Style\s+(\w+)\s*:\s*(.+)$/,
            // @Script Name: function
            /^@Script\s+(\w+)\s*:\s*(.+)$/,
            // @Var Name: value
            /^@Var\s+(\w+)\s*:\s*(.+)$/,
            // 简单格式：Name: signature
            /^(\w+)\s*:\s*(.+)$/
        ];

        for (const pattern of patterns) {
            const match = line.match(pattern);
            if (match) {
                const [, name, signature] = match;
                const type = this.inferExportType(line, signature);
                
                return {
                    name,
                    type,
                    signature,
                    description: this.extractInlineDescription(line)
                };
            }
        }

        return null;
    }

    /**
     * 推断导出类型
     */
    private inferExportType(line: string, signature: string): ModuleExportInfo['type'] {
        if (line.includes('@Template')) return 'template';
        if (line.includes('@Custom')) return 'custom';
        if (line.includes('@Element')) return 'element';
        if (line.includes('@Style')) return 'style';
        if (line.includes('@Script')) return 'script';
        if (line.includes('@Var')) return 'var';
        
        // 根据签名推断
        if (signature.includes('=>') || signature.includes('function')) return 'function';
        if (signature.includes('class')) return 'class';
        if (signature.includes('interface')) return 'interface';
        if (signature.includes('{') && signature.includes('}')) return 'style';
        
        return 'var';
    }

    /**
     * 提取隐式的CMOD导出（没有[Export]块时）
     */
    private extractImplicitCMODExports(content: string): ModuleExportInfo[] {
        const exports: ModuleExportInfo[] = [];
        
        // 匹配各种定义模式
        const patterns = [
            // [Template] @Type Name { ... }
            /\[Template\]\s*@(\w+)\s+(\w+)\s*\{/g,
            // [Custom] @Type Name { ... }
            /\[Custom\]\s*@(\w+)\s+(\w+)\s*\{/g,
            // [Style] @Type Name { ... }
            /\[Style\]\s*@(\w+)\s+(\w+)\s*\{/g,
            // [Script] @Type Name { ... }
            /\[Script\]\s*@(\w+)\s+(\w+)\s*\{/g
        ];

        for (const pattern of patterns) {
            const matches = content.matchAll(pattern);
            for (const match of matches) {
                const [, typeHint, name] = match;
                exports.push({
                    name,
                    type: this.mapTypeHint(typeHint),
                    signature: `${typeHint} ${name}`,
                    description: `隐式导出的${typeHint}`
                });
            }
        }

        return exports;
    }

    /**
     * 提取CJMOD的导出信息（手动查找）
     */
    private extractCJMODExports(content: string): { exports: ModuleExportInfo[] } {
        const exports: ModuleExportInfo[] = [];
        
        // 1. 查找extern "C"函数导出
        const externCRegex = /extern\s+"C"\s*\{([^}]*)\}/gs;
        const externMatches = content.matchAll(externCRegex);
        
        for (const match of externMatches) {
            const externContent = match[1];
            
            // 匹配函数声明
            const functionRegex = /(\w+\s*\*?\s*)\s+(\w+)\s*\(([^)]*)\)\s*;?/g;
            const functionMatches = externContent.matchAll(functionRegex);
            
            for (const funcMatch of functionMatches) {
                const [, returnType, funcName, params] = funcMatch;
                exports.push({
                    name: funcName.trim(),
                    type: 'function',
                    signature: `${returnType.trim()} ${funcName.trim()}(${params.trim()})`,
                    description: 'CJMOD导出函数',
                    parameters: this.parseParameters(params),
                    returnType: returnType.trim()
                });
            }
        }

        // 2. 查找类定义
        const classRegex = /class\s+(\w+)[\s\S]*?\{/g;
        const classMatches = content.matchAll(classRegex);
        
        for (const match of classMatches) {
            const [, className] = match;
            // 检查是否为导出的类（通常在extern "C"中有对应的工厂函数）
            if (this.isExportedClass(content, className)) {
                exports.push({
                    name: className,
                    type: 'class',
                    signature: `class ${className}`,
                    description: 'CJMOD导出类'
                });
            }
        }

        // 3. 查找命名空间导出
        const namespaceRegex = /namespace\s+(\w+)\s*\{/g;
        const namespaceMatches = content.matchAll(namespaceRegex);
        
        for (const match of namespaceMatches) {
            const [, namespaceName] = match;
            exports.push({
                name: namespaceName,
                type: 'interface', // 命名空间作为接口处理
                signature: `namespace ${namespaceName}`,
                description: 'CJMOD命名空间'
            });
        }

        // 4. 查找宏定义导出
        const macroRegex = /#define\s+(\w+)(?:\([^)]*\))?\s+/g;
        const macroMatches = content.matchAll(macroRegex);
        
        for (const match of macroMatches) {
            const [, macroName] = match;
            // 只导出看起来像公共API的宏
            if (macroName.toUpperCase() === macroName && !macroName.startsWith('_')) {
                exports.push({
                    name: macroName,
                    type: 'var',
                    signature: `#define ${macroName}`,
                    description: 'CJMOD宏定义'
                });
            }
        }

        return { exports };
    }

    /**
     * 检查类是否被导出
     */
    private isExportedClass(content: string, className: string): boolean {
        // 检查是否有对应的工厂函数或创建函数
        const factoryPatterns = [
            new RegExp(`create${className}`, 'i'),
            new RegExp(`new${className}`, 'i'),
            new RegExp(`${className}\\s*\\*\\s*\\w+\\s*\\(`, 'i')
        ];
        
        return factoryPatterns.some(pattern => pattern.test(content));
    }

    /**
     * 解析函数参数
     */
    private parseParameters(paramsStr: string): string[] {
        if (!paramsStr.trim()) return [];
        
        return paramsStr.split(',')
            .map(param => param.trim())
            .filter(param => param && param !== 'void');
    }

    /**
     * 映射类型提示
     */
    private mapTypeHint(typeHint: string): ModuleExportInfo['type'] {
        const typeMap: { [key: string]: ModuleExportInfo['type'] } = {
            'Element': 'element',
            'Template': 'template',
            'Custom': 'custom',
            'Style': 'style',
            'Script': 'script',
            'Var': 'var',
            'Function': 'function',
            'Class': 'class',
            'Interface': 'interface'
        };
        
        return typeMap[typeHint] || 'var';
    }

    /**
     * 提取依赖信息
     */
    private extractDependencies(content: string): string[] {
        const dependencies: string[] = [];
        
        // 匹配Import语句
        const importRegex = /\[Import\]\s*@\w+\s+from\s+['""]([^'""]+)['"]/g;
        const importMatches = content.matchAll(importRegex);
        
        for (const match of importMatches) {
            dependencies.push(match[1]);
        }
        
        return [...new Set(dependencies)]; // 去重
    }

    /**
     * 提取版本信息
     */
    private extractVersion(content: string): string | undefined {
        const patterns = [
            /version\s*[:=]\s*['""]([^'""]+)['"]/i,
            /VERSION\s*[:=]\s*['""]([^'""]+)['"]/i,
            /@version\s+([^\s]+)/i
        ];
        
        for (const pattern of patterns) {
            const match = content.match(pattern);
            if (match) return match[1];
        }
        
        return undefined;
    }

    /**
     * 提取描述信息
     */
    private extractDescription(content: string): string | undefined {
        const patterns = [
            /description\s*[:=]\s*['""]([^'""]+)['"]/i,
            /@description\s+(.+)/i,
            /\/\*\*\s*\n\s*\*\s*(.+?)\s*\n\s*\*/
        ];
        
        for (const pattern of patterns) {
            const match = content.match(pattern);
            if (match) return match[1].trim();
        }
        
        return undefined;
    }

    /**
     * 提取行内描述
     */
    private extractInlineDescription(line: string): string | undefined {
        const commentMatch = line.match(/\/\/\s*(.+)$/);
        return commentMatch ? commentMatch[1].trim() : undefined;
    }

    /**
     * 构建搜索映射
     */
    private buildSearchMap(modules: ModuleIndexEntry[]): ModuleIndex['searchMap'] {
        const byName: { [moduleName: string]: number } = {};
        const byExport: { [exportName: string]: number[] } = {};
        const byType: { [moduleType: string]: number[] } = {};
        const byOfficial = { official: [] as number[], project: [] as number[] };
        
        modules.forEach((module, index) => {
            // 按模块名索引
            byName[module.moduleName] = index;
            
            // 按导出名索引
            module.exports.forEach(exp => {
                if (!byExport[exp.name]) {
                    byExport[exp.name] = [];
                }
                byExport[exp.name].push(index);
            });
            
            // 按模块类型索引
            if (!byType[module.moduleType]) {
                byType[module.moduleType] = [];
            }
            byType[module.moduleType].push(index);
            
            // 按官方/项目分类
            if (module.isOfficial) {
                byOfficial.official.push(index);
            } else {
                byOfficial.project.push(index);
            }
        });
        
        return { byName, byExport, byType, byOfficial };
    }

    /**
     * 创建空索引
     */
    private createEmptyIndex(): ModuleIndex {
        return {
            version: '1.0.0',
            timestamp: new Date().toISOString(),
            structure: 'mixed',
            modules: [],
            searchMap: {
                byName: {},
                byExport: {},
                byType: {},
                byOfficial: { official: [], project: [] }
            }
        };
    }

    /**
     * 保存索引到文件
     */
    public async saveIndexToFile(index: ModuleIndex, outputPath: string): Promise<void> {
        const indexContent = JSON.stringify(index, null, 2);
        await fs.promises.writeFile(outputPath, indexContent, 'utf8');
        console.log(`📝 模块索引已保存到: ${outputPath}`);
    }

    /**
     * 从文件加载索引
     */
    public async loadIndexFromFile(indexPath: string): Promise<ModuleIndex | null> {
        try {
            if (!fs.existsSync(indexPath)) {
                return null;
            }
            
            const content = await fs.promises.readFile(indexPath, 'utf8');
            const index = JSON.parse(content) as ModuleIndex;
            
            console.log(`📖 从文件加载模块索引: ${index.modules.length} 个模块`);
            return index;
        } catch (error) {
            console.error('加载模块索引失败:', error);
            return null;
        }
    }

    /**
     * 检查索引是否需要更新
     */
    public async needsUpdate(indexPath: string, modulesPath: string): Promise<boolean> {
        if (!fs.existsSync(indexPath)) {
            return true;
        }
        
        const indexStat = fs.statSync(indexPath);
        const modulesStat = fs.statSync(modulesPath);
        
        // 如果modules目录比索引文件新，则需要更新
        return modulesStat.mtime > indexStat.mtime;
    }
}