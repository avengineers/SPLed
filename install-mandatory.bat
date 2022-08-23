@echo off

pushd %~dp0
call build.bat --installMandatory || exit /b 1
popd
pause
