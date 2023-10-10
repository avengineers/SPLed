<#
.DESCRIPTION
    Deploy documentation to engweb.marquardt.de
#>

param(
    [Parameter(Mandatory = $true, Position = 0, HelpMessage = 'build folder containing reports')]
    [System.IO.FileInfo]$buildFolder,
    [Parameter(Mandatory = $true, Position = 1, HelpMessage = 'Target subdirectory on engweb.marquardt.de')]
    [String]$outputSubdir
)

## start of script
$ErrorActionPreference = "Stop"

if (Test-Path -Path $buildFolder) {
    # Create a redirecting html page for Jenkins
    $outputUrl = "https://engweb.marquardt.de/sple/$outputSubdir/"
    Write-Output "Making documentation available at $outputUrl ..."
    $htmlContent = @"
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="refresh" content="0; url=$outputUrl" />
</head>
<body>
<p>If you are not redirected automatically, please click <a href="$outputUrl">$outputUrl</a>.</p>
</body>
</html>
"@
    Set-Content -Path "$buildFolder\reports.html" -Value $htmlContent -Force

    # Mirror the reports to the SPLE webspace and engweb
    $outputPath = "\\engweb.marquardt.de\wwwsple$\$outputSubdir\"
    $fullBuildFolder = Resolve-Path -Path "$buildFolder"
    # Find all reports directories
    $directoriesToCopy = Get-ChildItem -Path $fullBuildFolder -Directory -Recurse -Filter "reports" -Depth 6
    foreach ($directory in $directoriesToCopy) {
        # We want to copy Sphinx and GCOVR output
        foreach ($subDirectory in "html", "coverage") {
            $sourePath = Join-Path $directory.FullName $subDirectory
            if (Test-Path -Path $sourePath) {
                # Calculate the target path and get rid of the extra sub dir reports
                $targetPath = $sourePath.Replace($fullBuildFolder, $outputPath).Replace("reports\", "")
                # Now really copy the stuff ...
                robocopy $sourePath $targetPath /MIR /TBD /NFL /NDL /NP /NS /NC /NJH
                if ($LASTEXITCODE -gt 7) {
                    Write-Error "Failed to deploy output."
                }
                else {
                    $LASTEXITCODE = 0
                }
            }
        }
    }
}
else {
    Write-Error "Given parameter docdir is a non-existent path"
}
## end of script
