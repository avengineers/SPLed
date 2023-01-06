$ErrorActionPreference = "Stop"
Push-Location $PSScriptRoot
if ($Env:SPLCORE_PATH) {
    $SPL_PS1 = "$Env:SPLCORE_PATH\powershell\spl.ps1"
}
else {
    $SPL_PS1 = ".\modules\spl\powershell\spl.ps1"
}
Invoke-Expression "$SPL_PS1 $args"
Pop-Location
