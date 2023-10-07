<#
.SYNOPSIS
    MQ Proxy settings for PowerShell
.DESCRIPTION
    MQ Proxy settings for PowerShell
#>

Param(
    [switch]$askForCreds ## ask for username and password (encrypted)
)

# TODO: hardcoded proxy shall be replaced by pac url used in Windows proxy settings
$ProxyHost = 'osde01proxy02.marquardt.de:8080'

[string]$username = ''
[string]$usermail = ''
[string]$userpwd = ''

if ($askForCreds) {
    $username = Read-Host "Please enter your Windows user name for proxy authentication"
    $usermail = Read-Host "Please enter your mail address for proxy authentication"
    $userpwd_sec = Read-Host "Please enter your Windows password for proxy authentication" -AsSecureString
    $bstr = [System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($userpwd_sec)
    $userpwd = [System.Runtime.InteropServices.Marshal]::PtrToStringAuto($bstr)
    $Env:HTTP_PROXY = "http://" + $username + ':' + $userpwd + '@' + $ProxyHost
}
else {
    $Env:HTTP_PROXY = "http://$ProxyHost"
}

$Env:HTTPS_PROXY = $Env:HTTP_PROXY
$Env:NO_PROXY = "localhost,.marquardt.de,.marquardt.com"

[net.webrequest]::DefaultWebProxy = New-Object System.Net.WebProxy("http://$ProxyHost", $true, ($Env:NO_PROXY).split(','))

if ($askForCreds) {
    # Powershell / System.Net needs the mail address (don't ask me why)
    [net.webrequest]::DefaultWebProxy.Credentials = New-Object System.Net.NetworkCredential($usermail, $userpwd)
}
else {
    [net.webrequest]::DefaultWebProxy.Credentials = [System.Net.CredentialCache]::DefaultNetworkCredentials
}
