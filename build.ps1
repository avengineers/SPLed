$ErrorActionPreference = "Stop"
Push-Location $PSScriptRoot

New-Item -ItemType Directory '.bootstrap' -Force
(New-Object System.Net.WebClient).DownloadFile("https://raw.githubusercontent.com/avengineers/bootstrap/develop/bootstrap.ps1", ".\.bootstrap\bootstrap.ps1")
. .\.bootstrap\bootstrap.ps1

Invoke-Expression "& { $(Invoke-RestMethod https://raw.githubusercontent.com/avengineers/SPL/develop/install.ps1) } refactor-powershell-according-to-linter -skipInstall"

if ($Env:SPLCORE_PATH) {
    $SPL_PS1 = "$Env:SPLCORE_PATH\powershell\spl.ps1"
}
else {
    $SPL_PS1 = ".\build\spl-core\powershell\spl.ps1"
}

Invoke-Expression ". $SPL_PS1 $args"
Pop-Location
