@echo off

pushd %~dp0
call build.bat --install --installMandatory || exit /b 1
popd
pause
