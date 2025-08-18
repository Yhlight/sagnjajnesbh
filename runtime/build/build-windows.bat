@echo off
setlocal enabledelayedexpansion

REM CHTL ANTLR运行时 - Windows构建脚本

set "SCRIPT_DIR=%~dp0"
set "RUNTIME_DIR=%SCRIPT_DIR%\.."
set "PROJECT_ROOT=%RUNTIME_DIR%\.."

REM 默认参数
set "ARCH="
set "BUILD_TYPE=Release"
set "PARALLEL_JOBS=%NUMBER_OF_PROCESSORS%"
set "CLEAN_BUILD=false"
set "VERBOSE=false"
set "STATIC_ONLY=false"
set "SHARED_ONLY=false"
set "NO_TEST=false"
set "INSTALL_DEPS=false"
set "USE_MINGW=false"

REM 颜色定义（Windows 10+）
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM 打印函数
:print_info
echo %BLUE%[INFO]%NC% %~1
exit /b

:print_success
echo %GREEN%[SUCCESS]%NC% %~1
exit /b

:print_warning
echo %YELLOW%[WARNING]%NC% %~1
exit /b

:print_error
echo %RED%[ERROR]%NC% %~1
exit /b

REM 显示帮助信息
:show_help
echo CHTL ANTLR运行时 Windows构建脚本
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo     -h, --help          显示此帮助信息
echo     -a, --arch          目标架构 [x64^|x86] (默认: 自动检测)
echo     -t, --type          构建类型 [Release^|Debug^|RelWithDebInfo] (默认: Release)
echo     -j, --jobs          并行作业数 (默认: CPU核心数)
echo     -c, --clean         清理之前的构建
echo     -v, --verbose       详细输出
echo     --static-only       仅构建静态库
echo     --shared-only       仅构建动态库
echo     --no-test           跳过测试
echo     --install-deps      安装构建依赖
echo     --use-mingw         使用MinGW而不是Visual Studio
echo.
echo 示例:
echo     %~nx0                       # 构建当前架构
echo     %~nx0 -a x86                # 构建32位版本
echo     %~nx0 -t Debug -j 4         # Debug构建，4个并行作业
echo     %~nx0 --install-deps        # 安装依赖并构建
echo.
exit /b

REM 解析命令行参数
:parse_args
if "%~1"=="" goto :end_parse_args
if "%~1"=="-h" goto :show_help_and_exit
if "%~1"=="--help" goto :show_help_and_exit
if "%~1"=="-a" (
    set "ARCH=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="--arch" (
    set "ARCH=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="-t" (
    set "BUILD_TYPE=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="--type" (
    set "BUILD_TYPE=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="-j" (
    set "PARALLEL_JOBS=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="--jobs" (
    set "PARALLEL_JOBS=%~2"
    shift
    shift
    goto :parse_args
)
if "%~1"=="-c" (
    set "CLEAN_BUILD=true"
    shift
    goto :parse_args
)
if "%~1"=="--clean" (
    set "CLEAN_BUILD=true"
    shift
    goto :parse_args
)
if "%~1"=="-v" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)
if "%~1"=="--verbose" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)
if "%~1"=="--static-only" (
    set "STATIC_ONLY=true"
    shift
    goto :parse_args
)
if "%~1"=="--shared-only" (
    set "SHARED_ONLY=true"
    shift
    goto :parse_args
)
if "%~1"=="--no-test" (
    set "NO_TEST=true"
    shift
    goto :parse_args
)
if "%~1"=="--install-deps" (
    set "INSTALL_DEPS=true"
    shift
    goto :parse_args
)
if "%~1"=="--use-mingw" (
    set "USE_MINGW=true"
    shift
    goto :parse_args
)
call :print_error "未知选项: %~1"
goto :show_help_and_exit

:show_help_and_exit
call :show_help
exit /b 1

:end_parse_args

REM 检测架构
if "%ARCH%"=="" (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        set "ARCH=x64"
    ) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (
        set "ARCH=x86"
    ) else (
        call :print_error "不支持的架构: %PROCESSOR_ARCHITECTURE%"
        exit /b 1
    )
)

REM 验证架构
if not "%ARCH%"=="x64" if not "%ARCH%"=="x86" (
    call :print_error "不支持的架构: %ARCH%"
    exit /b 1
)

set "PLATFORM_ID=windows-%ARCH%"
set "BUILD_DIR=%RUNTIME_DIR%\build\temp\%PLATFORM_ID%"
set "INSTALL_DIR=%RUNTIME_DIR%\prebuilt\%PLATFORM_ID%"

call :print_info "Windows ANTLR运行时构建"
call :print_info "平台: %PLATFORM_ID%"
call :print_info "构建类型: %BUILD_TYPE%"
call :print_info "并行作业: %PARALLEL_JOBS%"

REM 安装依赖
:install_dependencies
if "%INSTALL_DEPS%"=="false" goto :skip_install_deps

call :print_info "检查构建依赖..."

REM 检查Visual Studio或MinGW
if "%USE_MINGW%"=="true" (
    where gcc >nul 2>&1
    if errorlevel 1 (
        call :print_error "MinGW GCC未安装"
        call :print_info "请安装MinGW-w64: https://www.mingw-w64.org/"
        exit /b 1
    )
    call :print_success "找到MinGW GCC"
) else (
    where cl >nul 2>&1
    if errorlevel 1 (
        call :print_error "Visual Studio编译器未找到"
        call :print_info "请安装Visual Studio 2019或更新版本"
        call :print_info "或使用 --use-mingw 选项"
        exit /b 1
    )
    call :print_success "找到Visual Studio编译器"
)

REM 检查CMake
where cmake >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake未安装"
    call :print_info "请从 https://cmake.org/download/ 下载安装CMake"
    exit /b 1
)
call :print_success "找到CMake"

REM 检查Git
where git >nul 2>&1
if errorlevel 1 (
    call :print_warning "Git未安装，如果需要下载源码将失败"
) else (
    call :print_success "找到Git"
)

:skip_install_deps

REM 准备ANTLR源码
:prepare_source
set "SRC_DIR=%RUNTIME_DIR%\src\antlr4-runtime"

if not exist "%SRC_DIR%" (
    call :print_info "下载ANTLR 4源码..."
    
    if not exist "%RUNTIME_DIR%\src" mkdir "%RUNTIME_DIR%\src"
    cd /d "%RUNTIME_DIR%\src"
    
    REM 从GitHub下载ANTLR 4.13.2源码
    git clone --depth 1 --branch 4.13.2 https://github.com/antlr/antlr4.git antlr4-full
    if errorlevel 1 (
        call :print_error "下载ANTLR源码失败"
        exit /b 1
    )
    
    REM 只需要C++运行时部分
    move antlr4-full\runtime\Cpp antlr4-runtime
    rmdir /s /q antlr4-full
    
    call :print_success "ANTLR源码准备完成"
)

REM 应用Windows特定补丁
:apply_patches
set "PATCHES_DIR=%RUNTIME_DIR%\src\patches\windows"

if exist "%PATCHES_DIR%" (
    call :print_info "应用Windows特定补丁..."
    
    for %%f in ("%PATCHES_DIR%\*.patch") do (
        call :print_info "应用补丁: %%~nxf"
        cd /d "%SRC_DIR%"
        git apply "%%f"
        if errorlevel 1 (
            call :print_warning "补丁应用失败: %%~nxf"
        )
    )
)

REM 构建ANTLR运行时
:build_runtime
call :print_info "开始构建ANTLR运行时..."

REM 创建构建目录
if "%CLEAN_BUILD%"=="true" (
    if exist "%BUILD_DIR%" (
        call :print_info "清理构建目录..."
        rmdir /s /q "%BUILD_DIR%"
    )
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

cd /d "%BUILD_DIR%"

REM 准备CMake参数
set "CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%"
set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR%"
set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_STANDARD=17"
set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_STANDARD_REQUIRED=ON"
set "CMAKE_ARGS=%CMAKE_ARGS% -DANTLR4_INSTALL=ON"
set "CMAKE_ARGS=%CMAKE_ARGS% -DANTLR4_USE_THREAD_LOCAL_CACHE=ON"

REM 生成器选择
if "%USE_MINGW%"=="true" (
    set "CMAKE_GENERATOR=MinGW Makefiles"
    set "CMAKE_ARGS=%CMAKE_ARGS% -G \"MinGW Makefiles\""
) else (
    set "CMAKE_GENERATOR=Visual Studio 16 2019"
    set "CMAKE_ARGS=%CMAKE_ARGS% -G \"Visual Studio 16 2019\""
    if "%ARCH%"=="x64" (
        set "CMAKE_ARGS=%CMAKE_ARGS% -A x64"
    ) else (
        set "CMAKE_ARGS=%CMAKE_ARGS% -A Win32"
    )
)

REM 库类型设置
if "%STATIC_ONLY%"=="true" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_SHARED_LIBS=OFF"
) else if "%SHARED_ONLY%"=="true" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_SHARED_LIBS=ON"
) else (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_SHARED_LIBS=ON"
)

REM 测试设置
if "%NO_TEST%"=="true" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_TESTING=OFF"
) else (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_TESTING=ON"
)

REM 详细输出
if "%VERBOSE%"=="true" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_VERBOSE_MAKEFILE=ON"
)

call :print_info "配置CMake..."
cmake %CMAKE_ARGS% "%SRC_DIR%"
if errorlevel 1 (
    call :print_error "CMake配置失败"
    exit /b 1
)

call :print_info "开始编译 (使用 %PARALLEL_JOBS% 个并行作业)..."
if "%USE_MINGW%"=="true" (
    mingw32-make -j%PARALLEL_JOBS%
) else (
    cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
)
if errorlevel 1 (
    call :print_error "编译失败"
    exit /b 1
)

call :print_info "安装到: %INSTALL_DIR%"
if "%USE_MINGW%"=="true" (
    mingw32-make install
) else (
    cmake --build . --config %BUILD_TYPE% --target install
)
if errorlevel 1 (
    call :print_error "安装失败"
    exit /b 1
)

REM 构建静态库版本（如果需要）
if not "%STATIC_ONLY%"=="true" if not "%SHARED_ONLY%"=="true" (
    call :print_info "构建静态库版本..."
    
    set "STATIC_BUILD_DIR=%BUILD_DIR%-static"
    if not exist "!STATIC_BUILD_DIR!" mkdir "!STATIC_BUILD_DIR!"
    cd /d "!STATIC_BUILD_DIR!"
    
    set "STATIC_CMAKE_ARGS=%CMAKE_ARGS% -DBUILD_SHARED_LIBS=OFF"
    cmake !STATIC_CMAKE_ARGS! "%SRC_DIR%"
    if errorlevel 1 (
        call :print_warning "静态库配置失败"
        goto :skip_static_build
    )
    
    if "%USE_MINGW%"=="true" (
        mingw32-make -j%PARALLEL_JOBS%
        mingw32-make install
    ) else (
        cmake --build . --config %BUILD_TYPE% --parallel %PARALLEL_JOBS%
        cmake --build . --config %BUILD_TYPE% --target install
    )
    if errorlevel 1 (
        call :print_warning "静态库构建失败"
    )
    
    :skip_static_build
    cd /d "%BUILD_DIR%"
)

REM 运行测试
:run_tests
if "%NO_TEST%"=="true" goto :skip_tests

call :print_info "运行测试..."
cd /d "%BUILD_DIR%"

if "%USE_MINGW%"=="true" (
    ctest --output-on-failure -j%PARALLEL_JOBS%
) else (
    ctest -C %BUILD_TYPE% --output-on-failure -j%PARALLEL_JOBS%
)
if errorlevel 1 (
    call :print_warning "部分测试失败，但继续构建过程"
) else (
    call :print_success "所有测试通过"
)

:skip_tests

REM 验证构建结果
:verify_build
call :print_info "验证构建结果..."

set "LIB_DIR=%INSTALL_DIR%\lib"
set "INCLUDE_DIR=%INSTALL_DIR%\include"

REM 检查头文件
if not exist "%INCLUDE_DIR%\antlr4-runtime" (
    call :print_error "头文件目录不存在: %INCLUDE_DIR%\antlr4-runtime"
    exit /b 1
)

REM 检查库文件
set "EXPECTED_LIBS="
if not "%SHARED_ONLY%"=="true" (
    set "EXPECTED_LIBS=%EXPECTED_LIBS% antlr4-runtime.lib"
)
if not "%STATIC_ONLY%"=="true" (
    set "EXPECTED_LIBS=%EXPECTED_LIBS% antlr4-runtime.dll antlr4-runtime.lib"
)

for %%L in (%EXPECTED_LIBS%) do (
    if not exist "%LIB_DIR%\%%L" (
        if not exist "%INSTALL_DIR%\bin\%%L" (
            call :print_error "库文件不存在: %%L"
            exit /b 1
        )
    )
    call :print_success "找到库文件: %%L"
)

call :print_success "构建验证通过"

REM 创建平台信息文件
:create_platform_info
set "INFO_FILE=%INSTALL_DIR%\platform-info.txt"

echo CHTL ANTLR Runtime Build Information > "%INFO_FILE%"
echo ==================================== >> "%INFO_FILE%"
echo. >> "%INFO_FILE%"
echo Platform: %PLATFORM_ID% >> "%INFO_FILE%"
echo Build Type: %BUILD_TYPE% >> "%INFO_FILE%"
echo Build Date: %DATE% %TIME% >> "%INFO_FILE%"
echo Build Host: %COMPUTERNAME% >> "%INFO_FILE%"
echo. >> "%INFO_FILE%"
echo System Information: >> "%INFO_FILE%"
echo - OS: %OS% >> "%INFO_FILE%"
echo - Architecture: %PROCESSOR_ARCHITECTURE% >> "%INFO_FILE%"
if "%USE_MINGW%"=="true" (
    echo - Compiler: MinGW GCC >> "%INFO_FILE%"
) else (
    echo - Compiler: Microsoft Visual C++ >> "%INFO_FILE%"
)
echo - CMake: >> "%INFO_FILE%"
cmake --version | findstr /C:"cmake version" >> "%INFO_FILE%"
echo. >> "%INFO_FILE%"
echo Build Configuration: >> "%INFO_FILE%"
if not "%SHARED_ONLY%"=="true" (
    echo - Static Library: Yes >> "%INFO_FILE%"
) else (
    echo - Static Library: No >> "%INFO_FILE%"
)
if not "%STATIC_ONLY%"=="true" (
    echo - Shared Library: Yes >> "%INFO_FILE%"
) else (
    echo - Shared Library: No >> "%INFO_FILE%"
)
if "%NO_TEST%"=="true" (
    echo - Tests Built: No >> "%INFO_FILE%"
) else (
    echo - Tests Built: Yes >> "%INFO_FILE%"
)
echo - Parallel Jobs: %PARALLEL_JOBS% >> "%INFO_FILE%"
echo. >> "%INFO_FILE%"
echo ANTLR Version: 4.13.2 >> "%INFO_FILE%"
echo C++ Standard: 17 >> "%INFO_FILE%"

call :print_info "平台信息已保存到: %INFO_FILE%"

REM 主函数完成
call :print_success "Windows ANTLR运行时构建完成！"
call :print_info "安装位置: %INSTALL_DIR%"

echo.
echo 使用说明:
echo 1. 在CMakeLists.txt中添加:
echo    set(ANTLR4_ROOT %INSTALL_DIR%)
echo    find_package(antlr4-runtime REQUIRED)
echo.
echo 2. 链接库文件:
echo    target_link_libraries(your_target antlr4-runtime)
echo.

exit /b 0

REM 解析参数时使用的shift函数
:shift
shift
goto :eof