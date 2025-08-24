#!/usr/bin/env node

/**
 * CHTL VSCode扩展内置资源打包脚本
 * 
 * 此脚本负责将CHTL编译器和官方模块一起打包到VSCode扩展中
 * 确保：
 * 1. 内置编译器位于 bin/chtl-compiler
 * 2. 官方模块位于 bin/module/ 
 * 3. 支持分类结构和混合结构
 */

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

class CHTLAssetPackager {
    constructor() {
        this.extensionRoot = path.resolve(__dirname, '..');
        this.binDir = path.join(this.extensionRoot, 'bin');
        this.sourceCompilerPath = this.findSourceCompiler();
        this.sourceModulesPath = this.findSourceModules();
    }

    /**
     * 查找源码中的编译器
     */
    findSourceCompiler() {
        const possiblePaths = [
            // 相对于workspace的路径
            path.resolve(this.extensionRoot, '../../build/chtl-compiler'),
            path.resolve(this.extensionRoot, '../../bin/chtl-compiler'),
            path.resolve(this.extensionRoot, '../../chtl-compiler'),
            // 系统路径
            '/usr/local/bin/chtl-compiler',
            '/usr/bin/chtl-compiler'
        ];

        for (const compilerPath of possiblePaths) {
            if (fs.existsSync(compilerPath)) {
                console.log(`✅ 找到编译器: ${compilerPath}`);
                return compilerPath;
            }
        }

        throw new Error('❌ 未找到CHTL编译器，请确保编译器已构建');
    }

    /**
     * 查找源码中的官方模块
     */
    findSourceModules() {
        const possiblePaths = [
            // 相对于workspace的路径 - 编译输出的模块
            path.resolve(this.extensionRoot, '../../build/module'),
            path.resolve(this.extensionRoot, '../../module'),
            // 相对于workspace的路径 - 源码模块（需要编译）
            path.resolve(this.extensionRoot, '../../src/Modules'),
            path.resolve(this.extensionRoot, '../../Modules')
        ];

        for (const modulesPath of possiblePaths) {
            if (fs.existsSync(modulesPath)) {
                console.log(`✅ 找到模块目录: ${modulesPath}`);
                return modulesPath;
            }
        }

        throw new Error('❌ 未找到官方模块目录');
    }

    /**
     * 准备目标目录
     */
    prepareBinDirectory() {
        console.log('📁 准备bin目录...');
        
        // 清理并创建bin目录
        if (fs.existsSync(this.binDir)) {
            this.removeDirectory(this.binDir);
        }
        fs.mkdirSync(this.binDir, { recursive: true });
        
        console.log(`✅ bin目录已准备: ${this.binDir}`);
    }

    /**
     * 复制编译器
     */
    copyCompiler() {
        console.log('🔧 复制CHTL编译器...');
        
        const targetCompilerPath = path.join(this.binDir, 'chtl-compiler');
        
        // 复制编译器文件
        fs.copyFileSync(this.sourceCompilerPath, targetCompilerPath);
        
        // 确保编译器可执行
        if (process.platform !== 'win32') {
            fs.chmodSync(targetCompilerPath, '755');
        }
        
        console.log(`✅ 编译器已复制到: ${targetCompilerPath}`);
    }

    /**
     * 复制官方模块
     */
    copyOfficialModules() {
        console.log('📦 复制官方模块...');
        
        const targetModulesPath = path.join(this.binDir, 'module');
        
        // 检查源模块结构
        const isSourceCompiled = this.isCompiledModulesDirectory(this.sourceModulesPath);
        
        if (isSourceCompiled) {
            // 直接复制已编译的模块
            this.copyDirectory(this.sourceModulesPath, targetModulesPath);
            console.log(`✅ 已编译模块已复制到: ${targetModulesPath}`);
        } else {
            // 需要编译源码模块
            console.log('🔄 检测到源码模块，正在编译...');
            this.compileSourceModules(targetModulesPath);
        }
        
        // 验证模块完整性
        this.validateOfficialModules(targetModulesPath);
    }

    /**
     * 检查是否为已编译的模块目录
     */
    isCompiledModulesDirectory(modulesPath) {
        // 检查是否包含.chtl、.cmod、.cjmod文件
        const hasCompiledFiles = this.findFiles(modulesPath, ['.chtl', '.cmod', '.cjmod']).length > 0;
        
        // 检查是否包含分类结构
        const cmodDirs = ['CMOD', 'Cmod', 'cmod'];
        const cjmodDirs = ['CJMOD', 'CJmod', 'cjmod'];
        
        const hasClassifiedStructure = cmodDirs.some(dir => 
            fs.existsSync(path.join(modulesPath, dir))
        ) && cjmodDirs.some(dir => 
            fs.existsSync(path.join(modulesPath, dir))
        );
        
        return hasCompiledFiles || hasClassifiedStructure;
    }

    /**
     * 编译源码模块
     */
    compileSourceModules(targetPath) {
        // 创建临时构建目录
        const tempBuildDir = path.join(this.extensionRoot, '.temp-build');
        if (fs.existsSync(tempBuildDir)) {
            this.removeDirectory(tempBuildDir);
        }
        fs.mkdirSync(tempBuildDir, { recursive: true });

        try {
            // 使用编译器编译源码模块
            const compileCommand = `${this.sourceCompilerPath} --build-modules "${this.sourceModulesPath}" --output "${tempBuildDir}"`;
            console.log(`执行编译命令: ${compileCommand}`);
            
            execSync(compileCommand, { stdio: 'inherit' });
            
            // 复制编译结果
            const compiledModulesPath = path.join(tempBuildDir, 'module');
            if (fs.existsSync(compiledModulesPath)) {
                this.copyDirectory(compiledModulesPath, targetPath);
                console.log(`✅ 源码模块编译完成并复制到: ${targetPath}`);
            } else {
                throw new Error('编译器未生成预期的module目录');
            }
        } catch (error) {
            console.error('❌ 编译源码模块失败:', error.message);
            // 作为备选，直接复制源码（开发模式）
            console.log('⚠️ 回退到直接复制源码模式');
            this.copyDirectory(this.sourceModulesPath, targetPath);
        } finally {
            // 清理临时目录
            if (fs.existsSync(tempBuildDir)) {
                this.removeDirectory(tempBuildDir);
            }
        }
    }

    /**
     * 验证官方模块完整性
     */
    validateOfficialModules(modulesPath) {
        console.log('🔍 验证官方模块完整性...');
        
        const requiredCoreModules = [
            'Core', 'Base', 'Utils', 'Components', 'UI'
        ];
        
        const foundModules = [];
        const allFiles = this.findFiles(modulesPath, ['.chtl', '.cmod', '.cjmod']);
        
        for (const file of allFiles) {
            const moduleName = path.basename(file, path.extname(file));
            foundModules.push(moduleName);
        }
        
        console.log(`📊 找到 ${foundModules.length} 个模块:`, foundModules);
        
        const missingCore = requiredCoreModules.filter(core => 
            !foundModules.some(found => found.toLowerCase().includes(core.toLowerCase()))
        );
        
        if (missingCore.length > 0) {
            console.warn(`⚠️ 缺少核心模块: ${missingCore.join(', ')}`);
            console.warn('这可能影响CHTL功能的完整性');
        } else {
            console.log('✅ 核心模块验证通过');
        }
        
        // 检查结构类型
        const isClassified = this.isClassifiedStructure(modulesPath);
        console.log(`📁 模块结构: ${isClassified ? '分类模式' : '混合模式'}`);
    }

    /**
     * 检查是否为分类结构
     */
    isClassifiedStructure(directory) {
        const cmodDirs = ['CMOD', 'Cmod', 'cmod'];
        const cjmodDirs = ['CJMOD', 'CJmod', 'cjmod'];
        
        const hasCmod = cmodDirs.some(dir => fs.existsSync(path.join(directory, dir)));
        const hasCjmod = cjmodDirs.some(dir => fs.existsSync(path.join(directory, dir)));
        
        return hasCmod && hasCjmod;
    }

    /**
     * 查找指定扩展名的文件
     */
    findFiles(directory, extensions, recursive = true) {
        const files = [];
        
        if (!fs.existsSync(directory)) return files;
        
        const items = fs.readdirSync(directory);
        
        for (const item of items) {
            const fullPath = path.join(directory, item);
            const stat = fs.statSync(fullPath);
            
            if (stat.isFile()) {
                const ext = path.extname(item);
                if (extensions.includes(ext)) {
                    files.push(fullPath);
                }
            } else if (stat.isDirectory() && recursive) {
                files.push(...this.findFiles(fullPath, extensions, true));
            }
        }
        
        return files;
    }

    /**
     * 复制目录
     */
    copyDirectory(source, target) {
        if (!fs.existsSync(source)) return;
        
        fs.mkdirSync(target, { recursive: true });
        
        const items = fs.readdirSync(source);
        
        for (const item of items) {
            const sourcePath = path.join(source, item);
            const targetPath = path.join(target, item);
            const stat = fs.statSync(sourcePath);
            
            if (stat.isDirectory()) {
                this.copyDirectory(sourcePath, targetPath);
            } else {
                fs.copyFileSync(sourcePath, targetPath);
            }
        }
    }

    /**
     * 删除目录
     */
    removeDirectory(directory) {
        if (!fs.existsSync(directory)) return;
        
        const items = fs.readdirSync(directory);
        
        for (const item of items) {
            const fullPath = path.join(directory, item);
            const stat = fs.statSync(fullPath);
            
            if (stat.isDirectory()) {
                this.removeDirectory(fullPath);
            } else {
                fs.unlinkSync(fullPath);
            }
        }
        
        fs.rmdirSync(directory);
    }

    /**
     * 生成模块索引
     */
    generateModuleIndex() {
        console.log('📊 生成模块索引...');
        
        const modulesPath = path.join(this.binDir, 'module');
        if (!fs.existsSync(modulesPath)) {
            console.warn('模块目录不存在，跳过索引生成');
            return null;
        }

        // 简化的模块索引构建（避免依赖TypeScript）
        const index = this.buildSimpleModuleIndex(modulesPath);
        
        const indexPath = path.join(this.binDir, 'module-index.json');
        fs.writeFileSync(indexPath, JSON.stringify(index, null, 2));
        
        console.log(`✅ 模块索引已生成: ${indexPath}`);
        console.log(`📊 索引包含 ${index.modules.length} 个模块`);
        
        return index;
    }

    /**
     * 构建简化的模块索引
     */
    buildSimpleModuleIndex(modulesPath) {
        const modules = [];
        const structure = this.isClassifiedStructure(modulesPath) ? 'classified' : 'mixed';
        
        if (structure === 'classified') {
            this.scanClassifiedModulesForIndex(modulesPath, modules);
        } else {
            this.scanMixedModulesForIndex(modulesPath, modules);
        }

        // 构建搜索映射
        const searchMap = this.buildSearchMapSimple(modules);

        return {
            version: '1.0.0',
            timestamp: new Date().toISOString(),
            structure,
            modules,
            searchMap
        };
    }

    /**
     * 扫描分类结构模块（用于索引）
     */
    scanClassifiedModulesForIndex(modulesPath, modules) {
        // 扫描CMOD目录
        const cmodDirs = ['CMOD', 'Cmod', 'cmod'];
        for (const cmodDir of cmodDirs) {
            const cmodPath = path.join(modulesPath, cmodDir);
            if (fs.existsSync(cmodPath)) {
                this.scanDirectoryForIndex(cmodPath, modules, ['chtl', 'cmod']);
                break;
            }
        }

        // 扫描CJMOD目录
        const cjmodDirs = ['CJMOD', 'CJmod', 'cjmod'];
        for (const cjmodDir of cjmodDirs) {
            const cjmodPath = path.join(modulesPath, cjmodDir);
            if (fs.existsSync(cjmodPath)) {
                this.scanDirectoryForIndex(cjmodPath, modules, ['cjmod']);
                break;
            }
        }
    }

    /**
     * 扫描混合结构模块（用于索引）
     */
    scanMixedModulesForIndex(modulesPath, modules) {
        this.scanDirectoryForIndex(modulesPath, modules, ['chtl', 'cmod', 'cjmod']);
    }

    /**
     * 扫描目录生成索引
     */
    scanDirectoryForIndex(directory, modules, extensions) {
        if (!fs.existsSync(directory)) return;

        const files = fs.readdirSync(directory);
        
        for (const file of files) {
            const filePath = path.join(directory, file);
            const stat = fs.statSync(filePath);
            
            if (stat.isFile()) {
                const ext = path.extname(file).substring(1);
                if (extensions.includes(ext)) {
                    try {
                        const moduleEntry = this.parseModuleForIndex(filePath);
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
     * 解析模块文件（用于索引）
     */
    parseModuleForIndex(filePath) {
        const content = fs.readFileSync(filePath, 'utf8');
        const stat = fs.statSync(filePath);
        const ext = path.extname(filePath).substring(1);
        const baseName = path.basename(filePath, path.extname(filePath));

        let exports = [];
        let dependencies = [];

        if (ext === 'cmod' || ext === 'chtl') {
            exports = this.extractCMODExportsSimple(content);
            dependencies = this.extractDependenciesSimple(content);
        } else if (ext === 'cjmod') {
            exports = this.extractCJMODExportsSimple(content);
            dependencies = this.extractDependenciesSimple(content);
        }

        return {
            moduleName: baseName,
            moduleType: ext,
            filePath,
            isOfficial: true, // 打包时都是官方模块
            dependencies,
            exports,
            lastModified: stat.mtime.getTime(),
            fileSize: stat.size
        };
    }

    /**
     * 简化的CMOD导出提取
     */
    extractCMODExportsSimple(content) {
        const exports = [];
        
        // 提取[Export]块
        const exportBlockRegex = /\[Export\]\s*\{([^}]*)\}/gs;
        const exportMatches = content.matchAll(exportBlockRegex);
        
        for (const match of exportMatches) {
            const exportContent = match[1];
            const exportLines = exportContent.split('\n')
                .map(line => line.trim())
                .filter(line => line && !line.startsWith('//'));
            
            for (const line of exportLines) {
                const exportInfo = this.parseExportLineSimple(line);
                if (exportInfo) {
                    exports.push(exportInfo);
                }
            }
        }

        // 如果没有[Export]块，查找隐式导出
        if (exports.length === 0) {
            const patterns = [
                /\[Template\]\s*@(\w+)\s+(\w+)/g,
                /\[Custom\]\s*@(\w+)\s+(\w+)/g,
                /\[Style\]\s*@(\w+)\s+(\w+)/g,
                /\[Script\]\s*@(\w+)\s+(\w+)/g
            ];

            for (const pattern of patterns) {
                const matches = content.matchAll(pattern);
                for (const match of matches) {
                    const [, type, name] = match;
                    exports.push({
                        name,
                        type: type.toLowerCase(),
                        signature: `${type} ${name}`
                    });
                }
            }
        }

        return exports;
    }

    /**
     * 简化的CJMOD导出提取
     */
    extractCJMODExportsSimple(content) {
        const exports = [];
        
        // 提取extern "C"函数
        const externCRegex = /extern\s+"C"\s*\{([^}]*)\}/gs;
        const externMatches = content.matchAll(externCRegex);
        
        for (const match of externMatches) {
            const externContent = match[1];
            const functionRegex = /(\w+\s*\*?\s*)\s+(\w+)\s*\([^)]*\)/g;
            const functionMatches = externContent.matchAll(functionRegex);
            
            for (const funcMatch of functionMatches) {
                const [, returnType, funcName] = funcMatch;
                exports.push({
                    name: funcName.trim(),
                    type: 'function',
                    signature: `${returnType.trim()} ${funcName.trim()}(...)`
                });
            }
        }

        // 提取类定义
        const classRegex = /class\s+(\w+)/g;
        const classMatches = content.matchAll(classRegex);
        
        for (const match of classMatches) {
            const [, className] = match;
            // 简单检查是否可能是导出的类
            if (content.includes(`create${className}`) || content.includes(`new${className}`)) {
                exports.push({
                    name: className,
                    type: 'class',
                    signature: `class ${className}`
                });
            }
        }

        return exports;
    }

    /**
     * 简化的依赖提取
     */
    extractDependenciesSimple(content) {
        const dependencies = [];
        const importRegex = /\[Import\]\s*@\w+\s+from\s+['""]([^'""]+)['"]/g;
        const importMatches = content.matchAll(importRegex);
        
        for (const match of importMatches) {
            dependencies.push(match[1]);
        }
        
        return [...new Set(dependencies)];
    }

    /**
     * 简化的导出行解析
     */
    parseExportLineSimple(line) {
        const patterns = [
            /^@(\w+)\s+(\w+)\s*:\s*(.+)$/,
            /^(\w+)\s*:\s*(.+)$/
        ];

        for (const pattern of patterns) {
            const match = line.match(pattern);
            if (match) {
                if (match.length === 4) {
                    // @Type Name: signature
                    const [, type, name, signature] = match;
                    return {
                        name,
                        type: type.toLowerCase(),
                        signature
                    };
                } else {
                    // Name: signature
                    const [, name, signature] = match;
                    return {
                        name,
                        type: 'var',
                        signature
                    };
                }
            }
        }

        return null;
    }

    /**
     * 构建简化的搜索映射
     */
    buildSearchMapSimple(modules) {
        const byName = {};
        const byExport = {};
        const byType = {};
        const byOfficial = { official: [], project: [] };
        
        modules.forEach((module, index) => {
            byName[module.moduleName] = index;
            
            module.exports.forEach(exp => {
                if (!byExport[exp.name]) {
                    byExport[exp.name] = [];
                }
                byExport[exp.name].push(index);
            });
            
            if (!byType[module.moduleType]) {
                byType[module.moduleType] = [];
            }
            byType[module.moduleType].push(index);
            
            if (module.isOfficial) {
                byOfficial.official.push(index);
            } else {
                byOfficial.project.push(index);
            }
        });
        
        return { byName, byExport, byType, byOfficial };
    }

    /**
     * 生成资源清单
     */
    generateManifest() {
        console.log('📝 生成资源清单...');
        
        const moduleIndex = this.generateModuleIndex();
        
        const manifest = {
            version: '1.0.0',
            timestamp: new Date().toISOString(),
            compiler: {
                path: 'bin/chtl-compiler',
                version: this.getCompilerVersion()
            },
            modules: {
                path: 'bin/module',
                indexPath: 'bin/module-index.json',
                structure: this.isClassifiedStructure(path.join(this.binDir, 'module')) ? 'classified' : 'mixed',
                count: moduleIndex ? moduleIndex.modules.length : 0
            }
        };
        
        const manifestPath = path.join(this.binDir, 'manifest.json');
        fs.writeFileSync(manifestPath, JSON.stringify(manifest, null, 2));
        
        console.log(`✅ 资源清单已生成: ${manifestPath}`);
        return manifest;
    }

    /**
     * 获取编译器版本
     */
    getCompilerVersion() {
        try {
            const versionOutput = execSync(`${this.sourceCompilerPath} --version`, { encoding: 'utf8' });
            return versionOutput.trim();
        } catch (error) {
            return 'unknown';
        }
    }

    /**
     * 执行完整打包流程
     */
    async package() {
        console.log('🚀 开始打包CHTL扩展内置资源...\n');
        
        try {
            this.prepareBinDirectory();
            this.copyCompiler();
            this.copyOfficialModules();
            const manifest = this.generateManifest();
            
            console.log('\n🎉 打包完成！');
            console.log('📊 打包摘要:');
            console.log(`   编译器: ${manifest.compiler.path} (版本: ${manifest.compiler.version})`);
            console.log(`   模块: ${manifest.modules.count} 个 (结构: ${manifest.modules.structure})`);
            console.log(`   总大小: ${this.getDirectorySize(this.binDir)} KB`);
            
        } catch (error) {
            console.error('\n❌ 打包失败:', error.message);
            process.exit(1);
        }
    }

    /**
     * 获取目录大小
     */
    getDirectorySize(directory) {
        let size = 0;
        
        if (!fs.existsSync(directory)) return size;
        
        const items = fs.readdirSync(directory);
        
        for (const item of items) {
            const fullPath = path.join(directory, item);
            const stat = fs.statSync(fullPath);
            
            if (stat.isDirectory()) {
                size += this.getDirectorySize(fullPath);
            } else {
                size += stat.size;
            }
        }
        
        return Math.round(size / 1024);
    }
}

// 执行打包
if (require.main === module) {
    const packager = new CHTLAssetPackager();
    packager.package();
}

module.exports = CHTLAssetPackager;