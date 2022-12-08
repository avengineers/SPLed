@echo off

pushd %~dp0
IF DEFINED SPLCORE_PATH (
    echo WARNING: Fixed SPL core path defined.
    powershell -ExecutionPolicy Bypass -File %SPLCORE_PATH%\powershell\spl.ps1 %* || exit /b 1
) else (
    powershell -ExecutionPolicy Bypass -File modules\spl\powershell\spl.ps1 %* || exit /b 1
)
popd
