@echo off

pushd %~dp0
powershell -Command Set-ExecutionPolicy Bypass -scope CurrentUser -force || exit /b 1
IF DEFINED SPLCORE_PATH (
    echo WARNING: Fixed SPL core path defined. Skip updating SPL core.
    powershell -File %SPLCORE_PATH%\powershell\spl.ps1 %* || exit /b 1
) else (
    if not exist build\spl-core\ powershell -File install-spl.ps1 || exit /b 1
    powershell -File build\spl-core\powershell\spl.ps1 %* || exit /b 1
)
popd