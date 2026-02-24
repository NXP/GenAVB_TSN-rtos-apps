@echo off
setlocal enabledelayedexpansion

net session >nul 2>&1
if errorlevel 1 (
    echo Error: This script requires Administrator privileges
    echo Please run Command Prompt as Administrator
    pause
    exit /b 1
)

set SCRIPT_DIR=%~dp0
set REPO_DIR=%SCRIPT_DIR%..
set WORKSPACE_DIR=%SCRIPT_DIR%..\..
set TOOLS_DIR=%WORKSPACE_DIR%\tools
set ARMGCC_VERSION=14.2.rel1
set ARMGCC_NAME=arm-gnu-toolchain-%ARMGCC_VERSION%-mingw-w64-i686-arm-none-eabi
set ARMGCC_URL=https://developer.arm.com/-/media/Files/downloads/gnu/%ARMGCC_VERSION%/binrel/%ARMGCC_NAME%.zip

echo Setting up MCUX SDK 3.0 workspace environment...

cd /d "%WORKSPACE_DIR%"
if errorlevel 1 (
    echo Error: Could not change to workspace directory
    exit /b 1
)

set SKIP_WEST=0
if exist "%WORKSPACE_DIR%\mcuxsdk-manifests" (
    echo West environment already initialized - skipping west setup
    set SKIP_WEST=1
)

if "%SKIP_WEST%"=="0" (
    echo Creating Python virtual environment...
    if exist .venv (
        echo Removing existing virtual environment...
        rmdir /S /Q .venv
    )

    python -m venv .venv
    if errorlevel 1 (
        echo Error: Failed to create virtual environment
        exit /b 1
    )

    call .venv\Scripts\activate.bat
    if errorlevel 1 (
        echo Error: Failed to activate virtual environment
        exit /b 1
    )

    echo Installing Python requirements...
    pip install -r "%SCRIPT_DIR%\env_requirements.txt"
    if errorlevel 1 (
        echo Error: Failed to install Python requirements
        exit /b 1
    )

    set PATH=!WORKSPACE_DIR!\.venv\Scripts;!PATH!


    west update

    echo Installing MCUX SDK requirements...
    cd mcuxsdk
    pip install -r scripts\requirements.txt
    if errorlevel 1 (
        echo Error: Failed to install MCUX SDK requirements
        exit /b 1
    )

    cd ..

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
) else (
    echo Activating existing virtual environment...
    if not exist .venv (
        echo Creating Python virtual environment...
        python -m venv .venv
        if errorlevel 1 (
            echo Error: Failed to create virtual environment
            exit /b 1
        )
    )

    call .venv\Scripts\activate.bat
    if errorlevel 1 (
        echo Error: Failed to activate virtual environment
        exit /b 1
    )
)

set SKIP_TOOLCHAIN=0
if defined ARMGCC_DIR (
    echo ARM GCC toolchain path already set: %ARMGCC_DIR%
    set SKIP_TOOLCHAIN=1
) else (
    if exist "%TOOLS_DIR%\%ARMGCC_NAME%" (
        echo Found existing ARM GCC toolchain: %TOOLS_DIR%\%ARMGCC_NAME%
        set SKIP_TOOLCHAIN=1
    )
)

if !SKIP_TOOLCHAIN!==0 (
    echo Downloading ARM GCC toolchain...
    if not exist "%TOOLS_DIR%" mkdir "%TOOLS_DIR%"
    curl -k -L --ssl-no-revoke -o "%TOOLS_DIR%\arm-gcc.zip" "%ARMGCC_URL%"
    if errorlevel 1 (
        echo Error: Failed to download ARM GCC toolchain
        exit /b 1
    )

    echo Extracting ARM GCC toolchain...
    powershell -command "Expand-Archive -Path '%TOOLS_DIR%\arm-gcc.zip' -DestinationPath '%TOOLS_DIR%\%ARMGCC_NAME%' -Force"
    if errorlevel 1 (
        echo Error: Failed to extract ARM GCC toolchain
        exit /b 1
    )

    del "%TOOLS_DIR%\arm-gcc.zip"
) else (
    echo Skipping ARM GCC toolchain setup
)

echo Creating symbolic links...

if exist "%WORKSPACE_DIR%\mcuxsdk\examples\demo_apps\avb_tsn" rmdir "%WORKSPACE_DIR%\mcuxsdk\examples\demo_apps\avb_tsn" >nul 2>&1
mklink /D "%WORKSPACE_DIR%\mcuxsdk\examples\demo_apps\avb_tsn" "%REPO_DIR%\boards\src\demo_apps\avb_tsn"

for /d %%i in ("%REPO_DIR%\boards\*") do (
    set BOARD_NAME=%%~ni
    if not "!BOARD_NAME!"=="src" (
        echo Creating symlink for board: !BOARD_NAME!
        if exist "%WORKSPACE_DIR%\mcuxsdk\examples\_boards\!BOARD_NAME!\demo_apps\avb_tsn" rmdir "%WORKSPACE_DIR%\mcuxsdk\examples\_boards\!BOARD_NAME!\demo_apps\avb_tsn" >nul 2>&1
        mklink /D "%WORKSPACE_DIR%\mcuxsdk\examples\_boards\!BOARD_NAME!\demo_apps\avb_tsn" "%REPO_DIR%\boards\!BOARD_NAME!\demo_apps\avb_tsn"
    )
)

if exist "%WORKSPACE_DIR%\mcuxsdk\devices\RT\RT1180\MIMXRT118x" rmdir "%WORKSPACE_DIR%\mcuxsdk\devices\RT\RT1180\MIMXRT118x" >nul 2>&1
mklink /D "%WORKSPACE_DIR%\mcuxsdk\devices\RT\RT1180\MIMXRT118x" "%REPO_DIR%\devices\MIMXRT118x"