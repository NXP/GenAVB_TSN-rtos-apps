@echo off

REM Internal-only setup steps for bifrost and mcuxsdk-manifests

git config --global url.ssh://git@bitbucket.sw.nxp.com/genavbtsn/bifrost.git.insteadOf https://github.com/nxp-zephyr/bifrost

west update bifrost
if errorlevel 1 (
    echo Error: west update bifrost failed
    exit /b 1
)

python "%WORKSPACE_DIR%\bifrost\scripts\init_env.py"
if errorlevel 1 (
    echo Error: west sdk_init failed
    exit /b 1
)
