@echo off

pushd %~dp0
powershell -ExecutionPolicy Bypass -File git-settings.ps1 || exit /b 1
powershell -ExecutionPolicy Bypass -File build.ps1 -install -installMandatory || exit /b 1
popd
pause
