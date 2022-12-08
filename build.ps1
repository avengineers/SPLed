$ErrorActionPreference = "Stop"

Function Invoke-CommandLine {
    param (
        [string]$CommandLine,
        [bool]$StopAtError = $true,
        [bool]$Silent = $false
    )
    if (-Not $Silent) {
        Write-Host "Executing: $CommandLine"
    }
    Invoke-Expression $CommandLine
    if ($LASTEXITCODE -ne 0) {
        if ($StopAtError) {
            Write-Error "Command line call `"$CommandLine`" failed with exit code $LASTEXITCODE"
            exit 1
        }
        else {
            if (-Not $Silent) {
                Write-Host "Command line call `"$CommandLine`" failed with exit code $LASTEXITCODE, continuing ..."
            }
        }
    }
}

Push-Location $PSScriptRoot

Write-Output "Running in ${pwd}"

Invoke-CommandLine -CommandLine "pipenv install --deploy --dev"
Invoke-CommandLine -CommandLine "pipenv run powershell .\modules\spl\powershell\spl.ps1 $args"

Pop-Location
