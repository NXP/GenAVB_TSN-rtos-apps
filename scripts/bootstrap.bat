@echo off
setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set WORKSPACE_DIR=%SCRIPT_DIR%..\..
set ARMGCC_FOLDER=arm-gnu-toolchain-14.2.rel1-mingw-w64-i686-arm-none-eabi

echo Bootstrapping MCUX SDK 3.0 development environment...

echo Running initial workspace setup...
call "%SCRIPT_DIR%\setup_env.bat"
if errorlevel 1 (
    echo Error: Initial setup failed
    exit /b 1
)

echo Setting up environment variables...
cd /d "%WORKSPACE_DIR%"
call .venv\Scripts\activate.bat

set FOUND_TOOLCHAIN=0
if defined ARMGCC_DIR (
    echo ARM GCC toolchain already set: !ARMGCC_DIR!
    set FOUND_TOOLCHAIN=1
) else (
    if exist "%WORKSPACE_DIR%\tools\%ARMGCC_FOLDER%" (
        set ARMGCC_DIR=%WORKSPACE_DIR%\tools\%ARMGCC_FOLDER%
        set FOUND_TOOLCHAIN=1
        echo Found ARM GCC toolchain: !ARMGCC_DIR!
    )
)

if !FOUND_TOOLCHAIN!==0 (
    echo Error: ARM GCC toolchain not found. Please set ARMGCC_DIR manually
    echo or ensure toolchain is installed in tools directory
    exit /b 1
)

cd mcuxsdk

echo.
echo ========================================
echo Environment setup completed successfully!
echo ========================================
echo Virtual environment: %WORKSPACE_DIR%\.venv
echo ARM GCC toolchain: %ARMGCC_DIR%
echo Current directory: %cd%
echo.
echo You are now ready to build. Example:
echo   west build -p always examples/demo_apps/avb_tsn/tsn_app --toolchain armgcc --config release -b evkbmimxrt1170 -Dcore_id=cm7

cmd /k "set ARMGCC_DIR=%ARMGCC_DIR%"