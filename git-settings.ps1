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
# But descend into directories
!*/
# !*/ by itself will only allow descent into the full directory tree; it won’t actually allow anything into the repo; so include .gitignore of subdirectories
# it is important! that <subdirectory>/.gitignore files contain "!*" as first line to allow adding new files in subdirectories
!*/**/.gitignore
"@

 Set-Content "${PSScriptRoot}/modules/.gitignore" $gitignorecontent
}
else {
    Write-Host -ForegroundColor Black -BackgroundColor Yellow "Skipping git settings setup; delete $markerPath to run it again."
}
