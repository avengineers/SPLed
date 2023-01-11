$ErrorActionPreference = "Stop"
$markerPath = '.git/git-settings.marker'

if (-not (Test-Path -Path $markerPath)) {
    Write-Host ""
    Write-Host -ForegroundColor Black -BackgroundColor Yellow "Setting up git ..."

    git config pull.rebase true
    Write-Host "git config pull.rebase was set to 'true'"

    Write-Host "Done."
    New-Item -Path "$markerPath" -ItemType File -Force

    $gitignorecontent = @"
# Ignore everything
*
"@

    Set-Content "${PSScriptRoot}/modules/.gitignore" $gitignorecontent

    $postCheckoutContent = @"
#!/bin/sh

python -m pipenv run west update
"@
 
    Set-Content "${PSScriptRoot}/.git/post-checkout" $postCheckoutContent
 
}
else {
    Write-Host -ForegroundColor Black -BackgroundColor Yellow "Skipping git settings setup; delete $markerPath to run it again."
}
