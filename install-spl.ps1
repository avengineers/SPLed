$dependency_json_content = Get-Content -Raw -Path $PSScriptRoot\dependencies.json | ConvertFrom-Json
if ($dependency_json_content.mandatory.spl_core.version) {
    $spl_version = $dependency_json_content.mandatory.spl_core.version
} else {
    Write-Error "SPL core version not defined in the dependencies.json. Please define 'mandatory.spl-core.version' value."
    exit 1
}
if ($dependency_json_content.mandatory.spl_core.installer) {
    $spl_installer = $dependency_json_content.mandatory.spl_core.installer
} else {
    Write-Error "SPL core installer not defined in the dependencies.json. Please define 'mandatory.spl-core.installer' value."
    exit 1
}
if ($dependency_json_content.mandatory.spl_core.url) {
    $additional_args = $dependency_json_content.mandatory.spl_core.url
} else {
    Write-Warning "SPL core repository url not defined in the dependencies.json. The SPL Github repository will be used by default. Please define 'mandatory.spl-core.url' in case you want to use an SPL mirror repository."
}

Push-Location $PSScriptRoot
Invoke-Expression "& { $(Invoke-RestMethod $spl_installer) } $spl_version $additional_args -skipInstall"
Pop-Location
