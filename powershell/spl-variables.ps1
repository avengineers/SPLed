# Read build environment definitions from VSCode config
Function Read-Environment-Variables() {
    $settingsJSON = Get-Content -Raw -Path .vscode/settings.json | ConvertFrom-Json

    if ($settingsJSON.'cmake.environment') {
        $settingsJSON.'cmake.environment' | Get-Member -MemberType NoteProperty | ForEach-Object {
            $key = $_.Name
            [Environment]::SetEnvironmentVariable($key, $settingsJSON.'cmake.environment'.$key)
        }
    }
}

### Env Vars ###
# Env variables must be defined in settings.json and shall print an error if missing
Read-Environment-Variables

if ($Env:SPL_INSTALL_DEPENDENCY_JSON_FILE) {
    $SPL_INSTALL_DEPENDENCY_JSON_CONTENT = Get-Content -Raw -Path $Env:SPL_INSTALL_DEPENDENCY_JSON_FILE | ConvertFrom-Json
} elseif (Test-Path -Path dependencies.json) {
    $SPL_INSTALL_DEPENDENCY_JSON_CONTENT = Get-Content -Raw -Path dependencies.json | ConvertFrom-Json
}

$SPL_CORE_INSTALL_DEPENDENCY_JSON_CONTENT = Get-Content -Raw -Path $PSScriptRoot/../dependencies.json | ConvertFrom-Json

$SPL_EXTENSIONS_SETUP_SCRIPTS_PATH = $Env:SPL_EXTENSION_ROOT_DIR + $Env:SPL_EXTENSION_SETUP_SCRIPT_SUBDIR

# TODO: read proxy from a configuration file to make this script independent on network settings
$SPL_PROXY_HOST = $Env:SPL_PROXY_HOST
$SPL_PROXY_BYPASS_LIST = $Env:SPL_PROXY_BYPASS_LIST
