$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host -ForegroundColor Black -BackgroundColor Yellow "Setting up git ..."

git config pull.rebase true
Write-Host "git config pull.rebase was set to 'true'"

$gitignorecontent = @'
# Ignore everything
*
'@

New-Item -ItemType Directory -Path "${PSScriptRoot}/modules" -Force
Set-Content "${PSScriptRoot}/modules/.gitignore" $gitignorecontent -Force

$postCheckoutContent = @'
#!/bin/sh

set -e

west_cmd="python -m pipenv run west"

if command -v $west_cmd &> /dev/null && [ -f .west/config ]
then
   export PIPENV_VERBOSITY=-1
   $west_cmd update
fi
'@

Set-Content "${PSScriptRoot}/.git/hooks/post-checkout" $postCheckoutContent -Force

Write-Host "Done."
