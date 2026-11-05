@echo off
setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set WORKSPACE_DIR=%SCRIPT_DIR%..\..
set ARMGCC_NAME=arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi
set REPO_DIR=%SCRIPT_DIR%..

echo Bootstrapping MCUX SDK development environment...

cd /d "%WORKSPACE_DIR%"
if errorlevel 1 (
    echo Error: Could not change to workspace directory
    exit /b 1
)

set SKIP_WEST=0
if exist "%WORKSPACE_DIR%\.env_created" (
    echo West environment already initialized - skipping west setup
    set SKIP_WEST=1
)

if "%SKIP_WEST%"=="0" (
    REM BEGIN_INTERNAL
    REM Run internal setup
    if exist "%SCRIPT_DIR%\internal.bat" (
        call "%SCRIPT_DIR%\internal.bat"
        if errorlevel 1 (
            echo Error: Internal setup failed
            exit /b 1
        )
    )
    REM END_INTERNAL

    west update
    if errorlevel 1 (
        echo Error: Failed to update west workspace
        exit /b 1
    )

    REM Apply patches if patches directory exists
    if exist "%REPO_DIR%\patches" (
        echo Applying patches...

        west patch -b "%REPO_DIR%\patches" -l "%REPO_DIR%\patches\patches.yml" apply
        if errorlevel 1 (
            echo Error: Failed to apply patches
            echo If patch application failed, remove mcuxsdk-manifests directory and re-run
            exit /b 1
        )
    )

    type nul > "%WORKSPACE_DIR%\.env_created"
)

echo Setting up environment variables...
cd /d "%WORKSPACE_DIR%"

set FOUND_TOOLCHAIN=0
if defined ARMGCC_DIR (
    echo ARM GCC toolchain already set: !ARMGCC_DIR!
    set FOUND_TOOLCHAIN=1
) else (
    if exist "%MCUXPRESSOTOOLS%\%ARMGCC_NAME%" (
        set ARMGCC_DIR=%MCUXPRESSOTOOLS%\%ARMGCC_NAME%
        set FOUND_TOOLCHAIN=1
        echo Found ARM GCC toolchain: !ARMGCC_DIR!
    )
)

if !FOUND_TOOLCHAIN!==0 (
    echo Error: ARM GCC toolchain not found. Please set ARMGCC_DIR manually
    exit /b 1
)

cd %REPO_DIR%

echo.
echo ========================================
echo Environment setup completed successfully!
echo ========================================
echo Virtual environment: %VIRTUAL_ENV%
echo ARM GCC toolchain: %ARMGCC_DIR%
echo Current directory: %cd%
echo.
echo You are now ready to build. Example:
echo   west build -p always boards/src/demo_apps/avb_tsn/tsn_app --toolchain armgcc --config release -b evkbmimxrt1170 -Dcore_id=cm7

cmd /k "set ARMGCC_DIR=%ARMGCC_DIR%"
