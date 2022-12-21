$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot

Write-Output "Running in ${pwd}"

. .\modules\spl\powershell\include.ps1

Invoke-CommandLine -CommandLine ".\build.ps1 -installMandatory"
Invoke-CommandLine -CommandLine ".\build.ps1 -build -target selftests"

Pop-Location
