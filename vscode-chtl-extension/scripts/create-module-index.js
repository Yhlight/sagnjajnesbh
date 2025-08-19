#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

/**
 * 模块索引创建脚本 - 为VSCode插件创建快速查找的JSON索引
 */

const COMPILER_SOURCE_DIR = path.resolve(__dirname, '../../');
const EXTENSION_DIR = path.resolve(__dirname, '..');
const MODULES_DIR = path.join(EXTENSION_DIR, 'modules');
const INDEX_FILE = path.join(MODULES_DIR, 'module-index.json');

console.log('📋 创建CHTL模块索引...');

function ensureDir(dir) {
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }
}

function calculateFileHash(filePath) {
    const fileBuffer = fs.readFileSync(filePath);
    const hashSum = crypto.createHash('md5');
    hashSum.update(fileBuffer);
    return hashSum.digest('hex');
}

function scanModuleDirectory(baseDir, moduleType) {
    const modules = [];
    
    // 支持的文件夹名称变体
    const folderVariants = [moduleType.toUpperCase(), capitalize(moduleType), moduleType.toLowerCase()];
    
    for (const variant of folderVariants) {
        const moduleDir = path.join(baseDir, variant);
        if (fs.existsSync(moduleDir)) {
            console.log(`📁 扫描${moduleType}模块目录: ${moduleDir}`);
            
            const items = fs.readdirSync(moduleDir);
            for (const item of items) {
                const itemPath = path.join(moduleDir, item);
                const stats = fs.statSync(itemPath);
                
                if (stats.isFile() && item.endsWith(`.${moduleType.toLowerCase()}`)) {
                    // 已打包的模块文件
                    const moduleInfo = analyzePackedModule(itemPath, moduleType);
                    if (moduleInfo) {
                        modules.push(moduleInfo);
                    }
                } else if (stats.isDirectory()) {
                    // 源码形式的模块目录
                    const moduleInfo = analyzeSourceModule(itemPath, moduleType, item);
                    if (moduleInfo) {
                        modules.push(moduleInfo);
                    }
                }
            }
        }
    }
    
    return modules;
}

function analyzePackedModule(filePath, moduleType) {
    try {
        const stats = fs.statSync(filePath);
        const fileName = path.basename(filePath, `.${moduleType.toLowerCase()}`);
        
        return {
            name: fileName,
            type: moduleType.toUpperCase(),
            source: 'packed',
            filePath: filePath,
            size: stats.size,
            hash: calculateFileHash(filePath),
            lastModified: stats.mtime.toISOString(),
            info: extractPackedModuleInfo(filePath, moduleType)
        };
    } catch (error) {
        console.warn(`⚠️  分析打包模块失败: ${filePath}`, error.message);
        return null;
    }
}

function analyzeSourceModule(dirPath, moduleType, moduleName) {
    try {
        const infoFile = path.join(dirPath, 'info', `${moduleName}.chtl`);
        const srcDir = path.join(dirPath, 'src');
        
        if (!fs.existsSync(infoFile)) {
            console.warn(`⚠️  模块缺少info文件: ${infoFile}`);
            return null;
        }
        
        const stats = fs.statSync(dirPath);
        const info = parseModuleInfo(infoFile, moduleType);
        
        return {
            name: moduleName,
            type: moduleType.toUpperCase(),
            source: 'directory',
            filePath: dirPath,
            size: calculateDirectorySize(dirPath),
            hash: calculateDirectoryHash(dirPath),
            lastModified: stats.mtime.toISOString(),
            info: info,
            structure: analyzeModuleStructure(dirPath, moduleType)
        };
    } catch (error) {
        console.warn(`⚠️  分析源码模块失败: ${dirPath}`, error.message);
        return null;
    }
}

function extractPackedModuleInfo(filePath, moduleType) {
    // 这里应该解压ZIP文件并读取info.chtl
    // 暂时返回基本信息
    const fileName = path.basename(filePath, `.${moduleType.toLowerCase()}`);
    return {
        name: fileName,
        version: '1.0.0',
        description: `${moduleType}模块: ${fileName}`,
        author: 'CHTL Official',
        exports: [],
        dependencies: []
    };
}

function parseModuleInfo(infoFilePath, moduleType) {
    try {
        const content = fs.readFileSync(infoFilePath, 'utf8');
        
        // 解析CHTL info文件格式
        const info = {
            name: '',
            version: '1.0.0',
            description: '',
            author: '',
            exports: [],
            dependencies: []
        };
        
        // 简化解析 - 提取基本信息
        const nameMatch = content.match(/Name:\s*(.+)/);
        if (nameMatch) info.name = nameMatch[1].trim();
        
        const versionMatch = content.match(/Version:\s*(.+)/);
        if (versionMatch) info.version = versionMatch[1].trim();
        
        const descMatch = content.match(/Description:\s*(.+)/);
        if (descMatch) info.description = descMatch[1].trim();
        
        const authorMatch = content.match(/Author:\s*(.+)/);
        if (authorMatch) info.author = authorMatch[1].trim();
        
        // 解析导出项 (仅CMOD有[Export]段)
        if (moduleType.toUpperCase() === 'CMOD') {
            const exportMatch = content.match(/\[Export\]\s*\{([^}]+)\}/s);
            if (exportMatch) {
                const exportContent = exportMatch[1];
                const exports = exportContent.split('\n')
                    .map(line => line.trim())
                    .filter(line => line && !line.startsWith('//'))
                    .map(line => {
                        const parts = line.split(':');
                        return {
                            name: parts[0]?.trim() || '',
                            type: parts[1]?.trim() || 'unknown'
                        };
                    });
                info.exports = exports;
            }
        }
        
        return info;
    } catch (error) {
        console.warn(`⚠️  解析模块info文件失败: ${infoFilePath}`, error.message);
        return {
            name: path.basename(path.dirname(path.dirname(infoFilePath))),
            version: '1.0.0',
            description: '',
            author: '',
            exports: [],
            dependencies: []
        };
    }
}

function analyzeModuleStructure(dirPath, moduleType) {
    const structure = {
        hasInfo: false,
        hasSrc: false,
        sourceFiles: [],
        totalFiles: 0
    };
    
    try {
        const infoDir = path.join(dirPath, 'info');
        const srcDir = path.join(dirPath, 'src');
        
        structure.hasInfo = fs.existsSync(infoDir);
        structure.hasSrc = fs.existsSync(srcDir);
        
        if (structure.hasSrc) {
            const srcFiles = fs.readdirSync(srcDir);
            structure.sourceFiles = srcFiles.filter(file => {
                const ext = path.extname(file);
                return moduleType.toUpperCase() === 'CMOD' ? 
                    ext === '.chtl' : 
                    ['.cpp', '.h'].includes(ext);
            });
        }
        
        structure.totalFiles = countFilesRecursive(dirPath);
        
    } catch (error) {
        console.warn(`⚠️  分析模块结构失败: ${dirPath}`, error.message);
    }
    
    return structure;
}

function calculateDirectorySize(dirPath) {
    let totalSize = 0;
    
    function walkDir(dir) {
        const files = fs.readdirSync(dir);
        for (const file of files) {
            const filePath = path.join(dir, file);
            const stats = fs.statSync(filePath);
            
            if (stats.isDirectory()) {
                walkDir(filePath);
            } else {
                totalSize += stats.size;
            }
        }
    }
    
    try {
        walkDir(dirPath);
    } catch (error) {
        console.warn(`⚠️  计算目录大小失败: ${dirPath}`, error.message);
    }
    
    return totalSize;
}

function calculateDirectoryHash(dirPath) {
    const hash = crypto.createHash('md5');
    
    function walkDir(dir) {
        const files = fs.readdirSync(dir).sort(); // 确保顺序一致
        for (const file of files) {
            const filePath = path.join(dir, file);
            const stats = fs.statSync(filePath);
            
            hash.update(file);
            
            if (stats.isDirectory()) {
                walkDir(filePath);
            } else {
                const content = fs.readFileSync(filePath);
                hash.update(content);
            }
        }
    }
    
    try {
        walkDir(dirPath);
    } catch (error) {
        console.warn(`⚠️  计算目录哈希失败: ${dirPath}`, error.message);
        return 'unknown';
    }
    
    return hash.digest('hex');
}

function countFilesRecursive(dirPath) {
    let count = 0;
    
    function walkDir(dir) {
        const files = fs.readdirSync(dir);
        for (const file of files) {
            const filePath = path.join(dir, file);
            const stats = fs.statSync(filePath);
            
            if (stats.isDirectory()) {
                walkDir(filePath);
            } else {
                count++;
            }
        }
    }
    
    try {
        walkDir(dirPath);
    } catch (error) {
        console.warn(`⚠️  统计文件数量失败: ${dirPath}`, error.message);
    }
    
    return count;
}

function capitalize(str) {
    return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();
}

function createModuleIndex() {
    console.log('🔍 扫描模块目录...');
    
    const allModules = [];
    
    // 扫描官方模块 (插件内置)
    const officialModulesDir = MODULES_DIR;
    if (fs.existsSync(officialModulesDir)) {
        console.log('📦 扫描官方模块...');
        
        // 扫描CMOD模块
        const cmodModules = scanModuleDirectory(officialModulesDir, 'cmod');
        allModules.push(...cmodModules.map(m => ({ ...m, category: 'official' })));
        
        // 扫描CJMOD模块
        const cjmodModules = scanModuleDirectory(officialModulesDir, 'cjmod');
        allModules.push(...cjmodModules.map(m => ({ ...m, category: 'official' })));
    }
    
    // 扫描用户模块 (如果在工作区中)
    const userModulesDir = path.join(COMPILER_SOURCE_DIR, 'src/Module');
    if (fs.existsSync(userModulesDir)) {
        console.log('👤 扫描用户模块...');
        
        // 扫描CMOD模块
        const userCmodModules = scanModuleDirectory(userModulesDir, 'cmod');
        allModules.push(...userCmodModules.map(m => ({ ...m, category: 'user' })));
        
        // 扫描CJMOD模块
        const userCjmodModules = scanModuleDirectory(userModulesDir, 'cjmod');
        allModules.push(...userCjmodModules.map(m => ({ ...m, category: 'user' })));
    }
    
    // 创建索引结构
    const index = {
        version: '1.0.0',
        buildDate: new Date().toISOString(),
        totalModules: allModules.length,
        
        // 按类型分类
        byType: {
            CMOD: allModules.filter(m => m.type === 'CMOD'),
            CJMOD: allModules.filter(m => m.type === 'CJMOD')
        },
        
        // 按来源分类
        byCategory: {
            official: allModules.filter(m => m.category === 'official'),
            user: allModules.filter(m => m.category === 'user')
        },
        
        // 按名称索引 (快速查找)
        byName: {},
        
        // 按导出项索引 (用于智能提示)
        byExport: {},
        
        // 统计信息
        statistics: {
            totalSize: allModules.reduce((sum, m) => sum + m.size, 0),
            cmodCount: allModules.filter(m => m.type === 'CMOD').length,
            cjmodCount: allModules.filter(m => m.type === 'CJMOD').length,
            officialCount: allModules.filter(m => m.category === 'official').length,
            userCount: allModules.filter(m => m.category === 'user').length,
            packedCount: allModules.filter(m => m.source === 'packed').length,
            sourceCount: allModules.filter(m => m.source === 'directory').length
        },
        
        // 完整模块列表
        modules: allModules
    };
    
    // 构建名称索引
    for (const module of allModules) {
        index.byName[module.name] = module;
    }
    
    // 构建导出项索引
    for (const module of allModules) {
        if (module.info && module.info.exports) {
            for (const exportItem of module.info.exports) {
                if (!index.byExport[exportItem.name]) {
                    index.byExport[exportItem.name] = [];
                }
                index.byExport[exportItem.name].push({
                    moduleName: module.name,
                    moduleType: module.type,
                    exportType: exportItem.type,
                    category: module.category
                });
            }
        }
    }
    
    return index;
}

function saveModuleIndex(index) {
    ensureDir(path.dirname(INDEX_FILE));
    
    try {
        fs.writeFileSync(INDEX_FILE, JSON.stringify(index, null, 2));
        console.log(`✅ 模块索引已保存: ${INDEX_FILE}`);
        
        // 创建压缩版本用于快速加载
        const compactIndex = {
            version: index.version,
            buildDate: index.buildDate,
            byName: index.byName,
            byExport: index.byExport,
            statistics: index.statistics
        };
        
        const compactFile = INDEX_FILE.replace('.json', '.compact.json');
        fs.writeFileSync(compactFile, JSON.stringify(compactIndex));
        console.log(`✅ 压缩索引已保存: ${compactFile}`);
        
        return true;
    } catch (error) {
        console.error(`❌ 保存模块索引失败:`, error.message);
        return false;
    }
}

function printIndexSummary(index) {
    console.log('');
    console.log('📊 模块索引摘要:');
    console.log(`   📦 总模块数: ${index.totalModules}`);
    console.log(`   🎨 CMOD模块: ${index.statistics.cmodCount}`);
    console.log(`   ⚙️  CJMOD模块: ${index.statistics.cjmodCount}`);
    console.log(`   🏢 官方模块: ${index.statistics.officialCount}`);
    console.log(`   👤 用户模块: ${index.statistics.userCount}`);
    console.log(`   📁 源码模块: ${index.statistics.sourceCount}`);
    console.log(`   📦 打包模块: ${index.statistics.packedCount}`);
    console.log(`   💾 总大小: ${Math.round(index.statistics.totalSize / 1024 / 1024 * 100) / 100}MB`);
    console.log(`   🕒 构建时间: ${new Date(index.buildDate).toLocaleString()}`);
    console.log('');
    
    // 显示模块列表
    if (index.modules.length > 0) {
        console.log('📋 发现的模块:');
        for (const module of index.modules) {
            const sizeKB = Math.round(module.size / 1024);
            const sourceIcon = module.source === 'packed' ? '📦' : '📁';
            const categoryIcon = module.category === 'official' ? '🏢' : '👤';
            console.log(`   ${sourceIcon}${categoryIcon} ${module.name} (${module.type}) - ${sizeKB}KB`);
        }
    }
}

function main() {
    console.log('🎯 CHTL模块索引创建脚本');
    console.log(`📂 编译器目录: ${COMPILER_SOURCE_DIR}`);
    console.log(`📂 插件目录: ${EXTENSION_DIR}`);
    console.log(`📂 模块目录: ${MODULES_DIR}`);
    console.log('');
    
    // 创建模块索引
    const index = createModuleIndex();
    
    if (index.totalModules === 0) {
        console.warn('⚠️  没有发现任何模块');
        return;
    }
    
    // 保存索引文件
    if (!saveModuleIndex(index)) {
        process.exit(1);
    }
    
    // 显示摘要
    printIndexSummary(index);
    
    console.log('🎉 模块索引创建完成！');
}

if (require.main === module) {
    main();
}