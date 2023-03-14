@echo off

pushd %~dp0
call build.bat -install %* || exit /b 1
popd
