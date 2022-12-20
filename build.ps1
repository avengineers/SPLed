$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot

Write-Output "Running in ${pwd}"

. .\modules\spl\powershell\include.ps1
Invoke-CommandLine -CommandLine "powershell .\modules\spl\powershell\spl-install.ps1 $args"

if($args -contains "-build" -or $args -contains "-import") {
    Invoke-CommandLine -CommandLine "python -m pipenv run powershell .\modules\spl\powershell\spl.ps1 $args"
}

Pop-Location
