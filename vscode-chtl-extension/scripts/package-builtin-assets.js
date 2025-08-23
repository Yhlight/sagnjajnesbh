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
     * 生成资源清单
     */
    generateManifest() {
        console.log('📝 生成资源清单...');
        
        const manifest = {
            version: '1.0.0',
            timestamp: new Date().toISOString(),
            compiler: {
                path: 'bin/chtl-compiler',
                version: this.getCompilerVersion()
            },
            modules: {
                path: 'bin/module',
                structure: this.isClassifiedStructure(path.join(this.binDir, 'module')) ? 'classified' : 'mixed',
                count: this.findFiles(path.join(this.binDir, 'module'), ['.chtl', '.cmod', '.cjmod']).length
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