Write-Information -Tags "Info:" -MessageData "Install additional deps ..."

$filename = "scoopfile.json"
$foundfiles = Get-ChildItem . -name -Depth 2 $filename
Foreach ($file in $foundfiles) {
    if (-not ($file -match ".venv" -or $file -eq $filename)) {
        Install-Toolset -FilePath $file
    }
}

$filename = "requirements.txt"
$foundfiles = Get-ChildItem . -name -Depth 2 $filename
Foreach ($file in $foundfiles) {
    if (-not ($file -match ".venv" -or $file -eq $filename)) {
        Invoke-CommandLine -CommandLine "python -m pipenv install --requirements $file"
    }
}
