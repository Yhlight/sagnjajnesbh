#!/usr/bin/env python3
"""
创建模拟的Windows静态库文件用于开发测试
这些文件具有正确的格式，但不包含实际的编译代码
"""

import os
import struct
import time

def create_coff_header():
    """创建COFF文件头"""
    # COFF文件签名
    header = b'!<arch>\n'
    return header

def create_member_header(name, size):
    """创建归档成员头"""
    # 格式：名称(16) 时间戳(12) 用户ID(6) 组ID(6) 模式(8) 大小(10) 结束标记(2)
    timestamp = str(int(time.time())).ljust(12)
    uid = '0'.ljust(6)
    gid = '0'.ljust(6) 
    mode = '100666'.ljust(8)
    size_str = str(size).ljust(10)
    
    header = (
        name.ljust(16).encode('ascii') +
        timestamp.encode('ascii') +
        uid.encode('ascii') +
        gid.encode('ascii') +
        mode.encode('ascii') +
        size_str.encode('ascii') +
        b'`\n'
    )
    return header

def create_first_linker_member():
    """创建第一个链接器成员（符号表）"""
    # 简化的符号表
    symbols = [
        b'_CSSCompiler_Compile',
        b'_CSSCompiler_GetOutput',
        b'_JavaScriptCompiler_Compile',
        b'_JavaScriptCompiler_GetOutput',
        b'_antlr4_runtime_version',
    ]
    
    # 符号数量（大端序）
    data = struct.pack('>I', len(symbols))
    
    # 符号偏移量
    offset = 100
    for _ in symbols:
        data += struct.pack('>I', offset)
        offset += 50
    
    # 符号字符串
    for symbol in symbols:
        data += symbol + b'\x00'
    
    # 对齐到偶数字节
    if len(data) % 2 != 0:
        data += b'\x00'
    
    return data

def create_second_linker_member():
    """创建第二个链接器成员"""
    # 成员数量
    data = struct.pack('<I', 3)
    # 成员偏移量
    data += struct.pack('<I', 100)
    data += struct.pack('<I', 200)
    data += struct.pack('<I', 300)
    # 符号数量
    data += struct.pack('<I', 5)
    # 符号索引
    for i in range(5):
        data += struct.pack('<H', i)
    # 符号字符串
    data += b'Symbol1\x00Symbol2\x00Symbol3\x00Symbol4\x00Symbol5\x00'
    
    return data

def create_object_file_stub(name):
    """创建目标文件存根"""
    # 简化的COFF目标文件
    # COFF头
    machine = 0x8664  # AMD64
    sections = 3
    timestamp = int(time.time())
    symbol_table = 0
    symbols = 0
    optional_header_size = 0
    characteristics = 0
    
    data = struct.pack('<HHIIIHH',
        machine, sections, timestamp, symbol_table,
        symbols, optional_header_size, characteristics)
    
    # 添加一些节数据
    data += b'.text\x00\x00\x00' * 10
    data += b'.data\x00\x00\x00' * 10
    data += b'.rdata\x00\x00' * 10
    
    # 填充到合理大小
    data += b'\x00' * (1024 - len(data))
    
    return data

def create_mock_lib(filename, lib_type):
    """创建模拟的.lib文件"""
    with open(filename, 'wb') as f:
        # 写入归档头
        f.write(create_coff_header())
        
        # 第一个链接器成员
        first_member = create_first_linker_member()
        f.write(create_member_header('/', len(first_member)))
        f.write(first_member)
        
        # 第二个链接器成员
        second_member = create_second_linker_member()
        f.write(create_member_header('/', len(second_member)))
        f.write(second_member)
        
        # 长名称成员
        longnames = b'//\x00' + b'verylongobjectfilename.obj/\x00'
        f.write(create_member_header('//', len(longnames)))
        f.write(longnames)
        
        # 添加一些目标文件
        for i in range(3):
            obj_name = f'{lib_type}_obj{i}.obj'
            obj_data = create_object_file_stub(obj_name)
            f.write(create_member_header(obj_name + '/', len(obj_data)))
            f.write(obj_data)

def main():
    """创建所有模拟库文件"""
    
    # 创建目录
    os.makedirs('prebuilt/windows/x64/Release', exist_ok=True)
    os.makedirs('prebuilt/windows/x64/Debug', exist_ok=True)
    
    # Release版本
    libs = {
        'chtl_css.lib': 'css',
        'chtl_javascript.lib': 'javascript',
        'antlr4-runtime.lib': 'antlr',
        'chtl_antlr_all.lib': 'all'
    }
    
    print("Creating Release libraries...")
    for lib_name, lib_type in libs.items():
        path = f'prebuilt/windows/x64/Release/{lib_name}'
        create_mock_lib(path, lib_type)
        size = os.path.getsize(path)
        print(f"  Created {lib_name} ({size:,} bytes)")
    
    # Debug版本
    print("\nCreating Debug libraries...")
    for lib_name, lib_type in libs.items():
        debug_name = lib_name.replace('.lib', '_d.lib')
        path = f'prebuilt/windows/x64/Debug/{debug_name}'
        create_mock_lib(path, lib_type)
        size = os.path.getsize(path)
        print(f"  Created {debug_name} ({size:,} bytes)")
    
    print("\nMock libraries created successfully!")
    print("\nNOTE: These are mock libraries for development only.")
    print("For production use, build real libraries using GitHub Actions.")

if __name__ == '__main__':
    main()