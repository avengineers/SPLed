@echo off

pushd %~dp0
call build.bat --installOptional || exit /b 1
popd
pause
