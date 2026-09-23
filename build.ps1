<#
.DESCRIPTION
    Wrapper: every option prints and runs one pypeline command, listed in README.md.
    The logic lives in pipeline/*.yaml and pytest.ini. CI calls pytest directly and must keep doing so.
#>

# Without it a stray token binds to the next free string parameter, so `--marker x` lands in -filter and the run succeeds with the wrong options.
[CmdletBinding(PositionalBinding = $false)]
param(
    [switch]$install,
    [switch]$build,
    [switch]$startVSCode,
    [switch]$selftests,
    # [string[]], so both `-variants a,b` from PowerShell and `-variants "a,b"` through build.bat arrive as a list.
    [string[]]$variants = @(),
    [string]$buildKit = "",
    [string]$buildType = "",
    [string]$target = "",
    [switch]$reconfigure,
    [switch]$configureOnly,
    [string]$ninjaArgs = "",
    [string]$filter = "",
    [string]$marker = "gate_develop_push",
    [string]$pytestExtraArgs = "",
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$rest
)

$ErrorActionPreference = "Stop"

$usage = @"
Usage: .\build.ps1 -install | -build | -startVSCode | -selftests
  -build accepts -variants <name|a,b|all>, -buildKit <prod|test>, -buildType <name>, -target <name>,
         -reconfigure, -configureOnly and -ninjaArgs <args>;
  -selftests accepts -filter <expr>, -marker <expr> and -pytestExtraArgs <args>.
"@

if ($rest) {
    Write-Error "Unknown argument(s): $($rest -join ' ')`nOptions take a single dash: -marker, not --marker.`n$usage" -Category InvalidArgument -ErrorAction Continue
    exit 1
}

if (-not ($install -or $build -or $startVSCode -or $selftests)) {
    Write-Output $usage
    exit
}

# Quoted, so a value containing a space stays one argument.
function Add-Arg([string]$name, [string]$value) { if ($value) { " $name `"$value`"" } }
function Add-Input([string]$name, [string]$value) { if ($value) { " -i `"$name=$value`"" } }

function Invoke-CommandLine([string]$commandLine, [bool]$stopAtError = $true) {
    Write-Output "Executing: $commandLine"
    $global:LASTEXITCODE = 0
    Invoke-Expression $commandLine
    if ($global:LASTEXITCODE -eq 0) { return }
    if ($stopAtError) { throw "Command line call `"$commandLine`" failed with exit code $global:LASTEXITCODE" }
    Write-Output "Command line call `"$commandLine`" failed with exit code $global:LASTEXITCODE, continuing ..."
}

$variantList = $variants -join ","
# The leading '.\' is required: PowerShell reads '.venv\Scripts\pypeline' as module-qualified syntax.
$pypeline = ".\.venv\Scripts\pypeline run --config-file"

Push-Location $PSScriptRoot
try {
    if ($install) {
        # There is no pypeline before the virtual environment exists, so the bootstrap creates it first.
        Invoke-RestMethod -Uri https://raw.githubusercontent.com/avengineers/bootstrap-installer/v1.19.1/install.ps1 | Invoke-Expression
        . .\.bootstrap\bootstrap.ps1
        Invoke-CommandLine "$pypeline pipeline/bootstrap.yaml"
    }
    if ($startVSCode) { Invoke-CommandLine "$pypeline pipeline/bootstrap.yaml --application `"code .`"" }
    if ($selftests) {
        # A release branch names the variant it releases, so test only that one. Empty on every other branch.
        if (-not $filter) {
            $filter = (& .\.venv\Scripts\python pipeline/variants.py --release-filter)
            # A release branch naming no variant is a mistake; the error is already on screen.
            if ($LASTEXITCODE -ne 0) { exit 1 }
        }
        # The report file is the verdict, so a failing gate must not stop the wrapper.
        Invoke-CommandLine ".\.venv\Scripts\pytest$(Add-Arg '-m' $marker)$(Add-Arg '-k' $filter) $pytestExtraArgs" -stopAtError $false
    }
    if ($build) {
        $flags = "$(Add-Input 'reconfigure' $(if ($reconfigure) { 'true' }))$(Add-Input 'configure_only' $(if ($configureOnly) { 'true' }))"
        Invoke-CommandLine "$pypeline pipeline/variant_build.yaml$(Add-Input 'variant' $variantList)$(Add-Input 'build_kit' $buildKit)$(Add-Input 'build_type' $buildType)$(Add-Input 'target' $target)$(Add-Input 'ninja_args' $ninjaArgs)$flags"
    }
}
finally {
    Pop-Location
}
