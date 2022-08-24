$dependency_json_content = Get-Content -Raw -Path $PSScriptRoot\dependencies.json | ConvertFrom-Json
if ($dependency_json_content.mandatory.spl_core_version) {
    $spl_version = $dependency_json_content.mandatory.spl_core_version
} else {
    Write-Error "SPL core version not defined in the dependencies.json. Please define 'spl-core-version' in the 'mandatory' dependencies."
    exit 1
}
Invoke-Expression "& { $(Invoke-RestMethod https://raw.githubusercontent.com/avengineers/SPL/develop/install.ps1) } $spl_version"
