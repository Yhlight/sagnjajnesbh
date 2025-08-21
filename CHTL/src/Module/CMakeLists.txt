# 模块系统库
add_library(ModuleSystem STATIC
    # 暂时创建一个空文件
    dummy.cpp
)

target_include_directories(ModuleSystem
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# 创建dummy.cpp
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/dummy.cpp "// Placeholder for ModuleSystem\n")