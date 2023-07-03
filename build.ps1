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

# Call a command and handle exit code
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

# Update/Reload current environment variable PATH with settings from registry
Function Initialize-EnvPath {
    # workaround for system-wide installations
    if ($Env:USER_PATH_FIRST) {
        $Env:Path = [System.Environment]::GetEnvironmentVariable("Path", "User") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "Machine")
    }
    else {
        $Env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
    }
}

function Test-RunningInCIorTestEnvironment {
    return [Boolean]($Env:JENKINS_URL -or $Env:PYTEST_CURRENT_TEST -or $Env:GITHUB_ACTIONS)
}

# start CMake with given targets
Function Invoke-CMake-Build([String] $Target, [String] $Variants, [String] $Filter, [String] $NinjaArgs, [bool] $Clean, [bool] $Reconfigure) {
    if ("selftests" -eq $Target) {
        $buildFolder = "build"
        # fresh and clean build
        if ($Clean) {
            if (Test-Path -Path $buildFolder) {
                Remove-Item $buildFolder -Force -Recurse
            }
        }

        # Run test cases to be found in folder test/
        # consider Filter if given
        $filterCmd = ''
        if ($Filter) {
            $filterCmd = "-k '$Filter'"
        }

        Invoke-CommandLine -CommandLine "python -m pipenv run python -m pytest test --capture=tee-sys --junitxml=test/output/test-report.xml -o junit_logging=all $filterCmd"
    }
    else {
        if ((-Not $Variants) -or ($Variants -eq 'all')) {
            $dirs = Get-Childitem -Include config.cmake -Path variants -Recurse | Resolve-Path -Relative
            $variantsList = @()
            Foreach ($dir in $dirs) {
                $variant = (get-item $dir).Directory.Parent.BaseName + "/" + (get-item $dir).Directory.BaseName
                $variantsList += $variant
            }
            $variantsSelected = @()
            if (-Not $Variants) {
                # variant selection if not specified
                Write-Information -Tags "Info:" -MessageData "no '--variant <variant>' was given, please select from list:"
                Foreach ($variant in $variantsList) {
                    Write-Information -Tags "Info:" -MessageData ("(" + [array]::IndexOf($variantsList, $variant) + ") " + $variant)
                }
                $variantsSelected += $variantsList[[int](Read-Host "Please enter selected variant number")]
                Write-Information -Tags "Info:" -MessageData "Selected variant is: $variantsSelected"
            }
            else {
                $variantsSelected = $variantsList
            }
        }
        else {
            $variantsSelected = $Variants.Replace('\', '/').Replace('./variant/', '').Replace('./variants/', '').Split(',')
        }

        Foreach ($variant in $variantsSelected) {
            $buildKit = "prod"
            if ($Target.Contains("unittests")) {
                $buildKit = "test"
            }
            $buildFolder = "build/$variant/$buildKit"
            # fresh and clean build
            if ($Clean) {
                if (Test-Path -Path $buildFolder) {
                    Remove-Item $buildFolder -Force -Recurse
                }
            }

            # delete CMake cache and reconfigure
            if ($Reconfigure) {
                if (Test-Path -Path "$buildFolder/CMakeCache.txt") {
                    Remove-Item "$buildFolder/CMakeCache.txt" -Force
                }
                if (Test-Path -Path "$buildFolder/CMakeFiles") {
                    Remove-Item "$buildFolder/CMakeFiles" -Force -Recurse
                }
            }

            # CMake configure and generate
            $variantDetails = $variant.Split('/')
            $platform = $variantDetails[0]
            $subsystem = $variantDetails[1]
            $additionalConfig = "-DBUILD_KIT=`"$buildKit`""
            if ($buildKit -eq "test") {
                $additionalConfig += " -DCMAKE_TOOLCHAIN_FILE=`"tools/toolchains/gcc/toolchain.cmake`""
            }
            Invoke-CommandLine -CommandLine "python -m pipenv run cmake -B '$buildFolder' -G Ninja -DFLAVOR=`"$platform`" -DSUBSYSTEM=`"$subsystem`" $additionalConfig"

            # CMake clean all dead artifacts. Required when running incremented builds to delete obsolete artifacts.
            Invoke-CommandLine -CommandLine "python -m pipenv run cmake --build '$buildFolder' --target $Target -- -t cleandead"
            # CMake build
            Invoke-CommandLine -CommandLine "python -m pipenv run cmake --build '$buildFolder' --target $Target -- $NinjaArgs"
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
        Invoke-RestMethod "https://raw.githubusercontent.com/avengineers/bootstrap/v1.0.0/bootstrap.ps1" -OutFile ".\.bootstrap\bootstrap.ps1"
        Invoke-CommandLine ". .\.bootstrap\bootstrap.ps1" -Silent $true
        Write-Output "For installation changes to take effect, please close and re-open your current shell."
    }
    else {
        if ($clean) {
            # Remove all build artifacts
            $buildDir = ".\build"
            if (Test-Path -Path $buildDir) {
                Remove-Item $buildDir -Force -Recurse
            }
        }
        if (Test-RunningInCIorTestEnvironment -or $Env:USER_PATH_FIRST) {
            Initialize-EnvPath
        }
        # Call CMake
        Invoke-CMake-Build -Target $target -Variants $variants -Filter $filter -NinjaArgs $ninjaArgs -Clean $clean -Reconfigure $reconfigure
    }
}
finally {
    Pop-Location
    if (-Not (Test-RunningInCIorTestEnvironment)) {
        Read-Host -Prompt "Press Enter to continue ..."
    }
}
## end of script
