powershell -ExecutionPolicy Bypass -File %~dp0build.ps1 -waitForKey %* || exit /b 1
