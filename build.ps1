$ErrorActionPreference = 'Stop'

. .\powershell\spl.ps1 -install -installMandatory

Push-Location powershell\test\
# TODO: ugly workaround to invoke tests twice, first time always fails.
try {
    Invoke-Pester spl-functions.Tests.ps1
}
catch {
    Invoke-Pester spl-functions.Tests.ps1
}
Pop-Location

if ($lastexitcode -ne 0) {
    throw ("Exec: " + $errorMessage)
}

# TODO: move these tests to python tests
Push-Location cmake\test\common.cmake\
if (Test-Path .cmaketest) {
    Remove-Item .cmaketest -Recurse -Force
}
cmake -B .cmaketest -G Ninja
if ($lastexitcode -ne 0) {
    throw ("Exec: " + $errorMessage)
}
Pop-Location

Push-Location cmake\test\spl.cmake\
if (Test-Path .cmaketest) {
    Remove-Item .cmaketest -Recurse -Force
}
cmake -B .cmaketest -G Ninja
if ($lastexitcode -ne 0) {
    throw ("Exec: " + $errorMessage)
}
Pop-Location

pipenv run pytest 
