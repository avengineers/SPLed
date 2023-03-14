<#
.DESCRIPTION
    Wrapper for installing dependencies, running and testing the project

.Notes
On Windows, it may be required to call this script with the proper execution policy.
You can do this by issuing the following PowerShell command:

PS C:\> powershell -ExecutionPolicy Bypass -File .\build.ps1

For more information on Execution Policies:
https://go.microsoft.com/fwlink/?LinkID=135170
#>

param(
    [Parameter(Mandatory = $false, HelpMessage = 'Clean build, wipe out all build artifacts. (Switch, default: false)')]
    [switch]$clean = $false,
    [Parameter(Mandatory = $false, HelpMessage = 'Install mandatory packages. (Switch, default: false)')]
    [switch]$install = $false,
    [Parameter(Mandatory = $false, HelpMessage = 'Target to be built. (String, default: "all")')]
    [string]$target = "all",
    [Parameter(Mandatory = $false, HelpMessage = 'Variants (projects) to be built (List of strings, leave empty to be asked or "all" for automatic build of all variants)')]
    [ValidateNotNullOrEmpty()]
    [string[]]$variants,
    [Parameter(Mandatory = $false, HelpMessage = 'filter for selftests; define in pytest syntax: https://docs.pytest.org/en/6.2.x/usage.html; e.g. "PYRO_C or test/test_unittests.py"')]
    [string]$filter = "",
    [Parameter(Mandatory = $false, HelpMessage = 'Additional build arguments for Ninja (e.g., "-d explain -d keepdepfile" for debugging purposes)')]
    [string]$ninjaArgs = "",
    [Parameter(Mandatory = $false, HelpMessage = 'Delete CMake cache and reconfigure')]
    [switch]$reconfigure
)

Function Invoke-CommandLine {
    [Diagnostics.CodeAnalysis.SuppressMessageAttribute('PSAvoidUsingInvokeExpression', '', Justification = 'Usually this statement must be avoided (https://learn.microsoft.com/en-us/powershell/scripting/learn/deep-dives/avoid-using-invoke-expression?view=powershell-7.3), here it is OK as it does not execute unknown code.')]
    param (
        [Parameter(Mandatory = $true, Position = 0)]
        [string]$CommandLine,
        [Parameter(Mandatory = $false, Position = 1)]
        [bool]$StopAtError = $true,
        [Parameter(Mandatory = $false, Position = 2)]
        [bool]$Silent = $false
    )
    if (-Not $Silent) {
        Write-Output "Executing: $CommandLine"
    }
    $global:LASTEXITCODE = 0
    Invoke-Expression $CommandLine
    if ($global:LASTEXITCODE -ne 0) {
        if ($StopAtError) {
            Write-Error "Command line call `"$CommandLine`" failed with exit code $global:LASTEXITCODE"
        }
        else {
            if (-Not $Silent) {
                Write-Output "Command line call `"$CommandLine`" failed with exit code $global:LASTEXITCODE, continuing ..."
            }
        }
    }
}

## start of script
# Always set the $InformationPreference variable to "Continue" globally, this way it gets printed on execution and continues execution afterwards.
$InformationPreference = "Continue"

# Stop on first PS error
$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot
Write-Output "Running in ${pwd}"

try {
    if ($install) {
        if (-Not (Test-Path -Path '.bootstrap')) {
            New-Item -ItemType Directory '.bootstrap'
        }
        # Installation of Scoop, Python and pipenv via bootstrap
        Invoke-RestMethod "https://raw.githubusercontent.com/avengineers/bootstrap/develop/bootstrap.ps1" -OutFile "$PSScriptRoot\.bootstrap\bootstrap.ps1"
        Invoke-CommandLine ". $PSScriptRoot\.bootstrap\bootstrap.ps1" -Silent $true
        Write-Output "For installation changes to take effect, please close and re-open your current shell."

        # Fetch spl-core bootstrap
        Invoke-RestMethod "https://raw.githubusercontent.com/avengineers/spl-core/41-cmake-module/powershell/spl-bootstrap.ps1" -OutFile "$PSScriptRoot\.bootstrap\spl-bootstrap.ps1"
    }
    else {
        if ($clean) {
            # Remove all build artifacts
            $buildDir = ".\build"
            if (Test-Path -Path $buildDir) {
                Remove-Item $buildDir -Force -Recurse
            }
        }
        # Call CMake
        . "$PSScriptRoot\.bootstrap\spl-bootstrap.ps1"
        Invoke-CMake-Build -Target $target -Variants $variants -Filter $filter -NinjaArgs $ninjaArgs -Clean $clean -Reconfigure $reconfigure
    }
}
finally {
    Pop-Location
    if ((-Not $Env:JENKINS_URL) -and (-Not $Env:PYTEST_CURRENT_TEST) -and (-Not $Env:GITHUB_ACTIONS)) {
        Read-Host -Prompt "Press Enter to continue ..."
    }
}
## end of script
