$ErrorActionPreference = "Stop"
$markerPath = '.git/git-settings.marker'

if (-not (Test-Path -Path $markerPath)) {
    Write-Host ""
    Write-Host -ForegroundColor Black -BackgroundColor Yellow "Setting up git ..."

    git config submodule.recurse true
    Write-Host "git config submodule.recurse was set to 'true'"
    git config pull.rebase true
    Write-Host "git config pull.rebase was set to 'true'"

    git submodule update --init
    Write-Host "Initialized submodules."

    Write-Host "Done."
    New-Item -Path "$markerPath" -ItemType File -Force
}
else {
    Write-Host -ForegroundColor Black -BackgroundColor Yellow "Skipping git settings setup; delete $markerPath to run it again."
}
