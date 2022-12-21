@echo off

pushd %~dp0
call build.bat -install -installOptional || exit /b 1
popd
pause
