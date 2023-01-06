. $PSScriptRoot\spl-variables.ps1
. $PSScriptRoot\spl-functions.ps1

if ($SPL_PROXY_HOST -and $SPL_PROXY_BYPASS_LIST) {
    Setup-Proxy -ProxyHost $SPL_PROXY_HOST -NoProxy $SPL_PROXY_BYPASS_LIST
}