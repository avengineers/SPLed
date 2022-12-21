Push-Location $PSScriptRoot
Invoke-Expression ".\modules\spl\powershell\spl.ps1 $args"
Pop-Location
