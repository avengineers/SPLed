<#
.DESCRIPTION
    Wrapper for installing dependencies, running and testing the project

.Notes
On Windows, it may be required to enable this script by setting the execution
policy for the user. You can do this by issuing the following PowerShell command:

PS C:\> Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

For more information on Execution Policies: 
https://go.microsoft.com/fwlink/?LinkID=135170
#>

[CmdletBinding(DefaultParameterSetName = 'Build')]
param(
    [Parameter(
        Mandatory = $true,
        ParameterSetName = 'Build',
        Position = 0
    )]
    [switch]$build ## Select for building the software
    , [Parameter(
        Mandatory = $true,
        ParameterSetName = 'Build'
    )]
    [ValidateNotNullOrEmpty()]
    [string]$target = "" ## Target to be built
    , [Parameter(ParameterSetName = 'Build')]
    [string]$filter = "" ## filter for selftests; define in pytest syntax: https://docs.pytest.org/en/6.2.x/usage.html; e.g. "PYRO_C or test/test_unittests.py"
    , [Parameter(ParameterSetName = 'Build')]
    [string[]]$variants = "" ## Variants (projects) to be built ('all' for automatic build of all variants)
    , [Parameter(ParameterSetName = 'Build')]
    [string]$ninjaArgs = "" ## Additional build arguments for Ninja (e.g., "-d explain -d keepdepfile" for debugging purposes)
    , [Parameter(ParameterSetName = 'Build')]
    [switch]$clean ## Delete build directory
    , [Parameter(ParameterSetName = 'Build')]
    [switch]$reconfigure ## Delete CMake cache and reconfigure
    , [Parameter(
        Mandatory = $true,
        ParameterSetName = 'Import',
        Position = 0
    )]
    [switch]$import ## Select for importing legacy code from Dimensions repo
    , [Parameter(
        Mandatory = $true,
        ParameterSetName = 'Import'
    )]
    [ValidateNotNullOrEmpty()]
    [string]$source ## Location of Dimensions project containing an 'Impl' directory
    , [Parameter(
        Mandatory = $true,
        ParameterSetName = 'Import'
    )]
    [ValidateNotNullOrEmpty()]
    [string]$variant ## Configuration name (<platform>/<subsystem>, e.g., VW_PPE_Porsche_983/BMS_HV_PYRO)
    , [switch]$installMandatory ## install mandatory packages (e.g., CMake, Ninja, ...)
    , [switch]$installOptional ## install optional packages (e.g., VS Code)
)


$ErrorActionPreference = "Stop"
Write-Output "Running in ${pwd}"

# load spl scripts
. $PSScriptRoot\spl-variables.ps1
. $PSScriptRoot\spl-functions.ps1

if ($SPL_PROXY_HOST -and $SPL_PROXY_BYPASS_LIST) {
    Setup-Proxy -ProxyHost $SPL_PROXY_HOST -NoProxy $SPL_PROXY_BYPASS_LIST
}

if ($installMandatory -or $installOptional) {
    Install-Basic-Tools
}

if ($installMandatory) {
    Install-Mandatory-Tools -JsonDependencies $SPL_CORE_INSTALL_DEPENDENCY_JSON_CONTENT
    Install-Mandatory-Tools -JsonDependencies $SPL_INSTALL_DEPENDENCY_JSON_CONTENT
    Run-Setup-Scripts -Location "$SPL_EXTENSIONS_SETUP_SCRIPTS_PATH\mandatory"
}

if ($installOptional) {
    Install-Optional-Tools -JsonDependencies $SPL_INSTALL_DEPENDENCY_JSON_CONTENT
    Run-Setup-Scripts -Location "$SPL_EXTENSIONS_SETUP_SCRIPTS_PATH\optional"
}

if ($build) {
    Run-CMake-Build -Target $target -Variants $variants -Filter $filter -NinjaArgs $ninjaArgs -Clean $clean -Reconfigure $reconfigure
}

if ($import) {
    Run-Transformer -Source $source -Variant $variant -Clean $clean
}
