# to execute tests you have to
# 1. Update 'Pester': "Install-Module -Name Pester -Force -SkipPublisherCheck"
# 2. call "Invoke-Pester spl-functions.Tests.ps1" from within the test directory
# Note: I noticed that sometimes after a test was changed it will fail with a overloading problem; retry helps

BeforeAll {
  . .\..\spl-functions.ps1
}

Describe "invoking command line calls" {
  BeforeEach{
    Mock -CommandName Write-Host -MockWith {}
  }

  It "shall not write the command to console if silent" {
    Invoke-CommandLine -CommandLine "dir" -Silent $true -StopAtError $true
    Should -Invoke -CommandName Write-Host -Times 0
  }

  It "shall write the command to console as default" {
    Invoke-CommandLine -CommandLine "dir"
    Should -Invoke -CommandName Write-Host -Times 1
  }

  It "shall print an error on failure" {
    $ErrorActionPreference = "Stop"
    Mock -CommandName Invoke-Expression -MockWith {$global:LASTEXITCODE = 1}

    Invoke-CommandLine -CommandLine "dir" -Silent $false -StopAtError $false
    Should -Invoke -CommandName Write-Host -Times 2
  }
}

Describe "scoop installation" {
  BeforeEach{
    Mock -CommandName Invoke-CommandLine -MockWith {}
  }

  It "shall not call Invoke-CommandLine if no package is given" {
    [String[]]$package = @() # empty array
    ScoopInstall($package)
    Should -Invoke -CommandName Invoke-CommandLine -Times 0
  }

  It "shall call Invoke-CommandLine once for all tools specified in package" {
    [String[]]$package = "PowerShell", "MinGW", "MSys"
    ScoopInstall($package)
    Should -Invoke -CommandName Invoke-CommandLine -Times 1
  }
}

Describe "scoop mandatory installation" {
  BeforeEach{
    Mock -CommandName ScoopInstall -MockWith {}
  }

  It "shall add all buckets from dependencies.json" {
    Mock -CommandName ScoopInstall -MockWith {}
    Mock -CommandName PythonInstall -MockWith {}
    Mock -CommandName Invoke-CommandLine -MockWith {}

    $SPL_INSTALL_DEPENDENCY_JSON_FILE_CONTENT = Get-Content -Raw -Path "../../dependencies.json" | ConvertFrom-Json
    Install-Mandatory-Tools($SPL_INSTALL_DEPENDENCY_JSON_FILE_CONTENT)

    Should -Invoke -CommandName Invoke-CommandLine -Times 2
  }
}

Describe "scoop optional installation" {
  BeforeEach{
    Mock -CommandName ScoopInstall -MockWith {}
  }

  It "shall fail if neither 'y' or 'n' was given" {
    Mock -CommandName Read-Host -MockWith {'x'}

    [String[]]$package = "PowerShell", "MinGW", "MSys"
    $failed = $false
    try {
      ScoopInstallOptional($package)
    } catch {
      $failed = $true
    }

    $failed | Should -Be $true
  }

  It "shall work but not install anything if 'n' was given" {
    Mock -CommandName Read-Host -MockWith {'n'}
    
    [String[]]$package = "PowerShell", "MinGW", "MSys"
    $failed = $false
    try {
      ScoopInstallOptional($package)
    } catch {
      $failed = $true
    }

    $failed | Should -Be $false
    Should -Invoke -CommandName ScoopInstall -Times 0
  }

  It "shall work and install all 3 tools if 'y' was given" {
    Mock -CommandName Read-Host -MockWith {'y'}
    
    [String[]]$package = "PowerShell", "MinGW", "MSys"
    $failed = $false
    try {
      ScoopInstallOptional($package)
    } catch {
      $failed = $true
    }

    $failed | Should -Be $false
    Should -Invoke -CommandName ScoopInstall -Times 3
  }
}

Describe "python installation" {
  BeforeEach{
    Mock -CommandName Invoke-CommandLine -MockWith {}
  }

  It "shall not call Invoke-CommandLine if no package is given" {
   
    [String[]]$package = @() # empty array
    PythonInstall -Package $package
    Should -Invoke -CommandName Invoke-CommandLine -Times 0
  }

  It "shall call Invoke-CommandLine twice, once for installing packages and once for upgrading" {
    [String[]]$package = "PowerShell", "MinGW", "MSys"
    PythonInstall -Package $package
    Should -Invoke -CommandName Invoke-CommandLine -Times 2
  }
}

Describe "python installation advanced" {
  It "shall add new trusted hosts to call" {
    [String[]]$package = "PowerShell", "MinGW", "MSys"
    [String[]]$hosts = "pypi.org", "files.pythonhosted.org"
    $global:counter = 0
    Mock -CommandName Invoke-CommandLine -MockWith {$global:counter++} -ParameterFilter { $CommandLine -eq "python -m pip install  --trusted-host pypi.org --trusted-host files.pythonhosted.org PowerShell MinGW MSys" }
    PythonInstall -Packages $package -TrustedHosts $hosts
    $global:counter | Should -Be 1
  }
}

Describe "running setup scripts" {
  It "shall not search for files if directory does not exist" {
    Mock -CommandName Test-Path -MockWith {$false}
    Mock -CommandName Get-ChildItem -MockWith {}
    
    Run-Setup-Scripts('mypath')
    Should -Invoke -CommandName Get-ChildItem -Times 0
  }

  It "shall print every file it finds and call it" {
    Mock -CommandName ForEach-Object -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true}
    Mock -CommandName Get-ChildItem -MockWith {
      @(
        @{"FullName" = "file1.txt"}
        @{"FullName" = "file2.txt"}
      )
    }
    
    Run-Setup-Scripts('mypath')
    Should -Invoke -CommandName ForEach-Object -Times 2
  }
}

Describe "install basic tools" {
  BeforeEach{
    Mock -CommandName ScoopInstall -MockWith {}
    Mock -CommandName invoke-expression -MockWith {}
    Mock -CommandName Invoke-CommandLine -MockWith {}
  }

  It "shall install scoop if it does not find it and afterwards install tools using scoop" {
    Mock -CommandName Get-Command -MockWith {$false} # does not find scoop
    
    Install-Basic-Tools
    Should -Invoke -CommandName ScoopInstall -Times 2
    # Should -Invoke -CommandName invoke-expression -Times 1  # complicated to count with different handling for admin and non-admin
    Should -Invoke -CommandName Invoke-CommandLine -Times 3
  }

  It "shall skip install scoop if it finds it and afterwards install tools using scoop" {
    Mock -CommandName Get-Command -MockWith {$true} # finds scoop
    
    Install-Basic-Tools
    Should -Invoke -CommandName ScoopInstall -Times 2
    Should -Invoke -CommandName invoke-expression -Times 0
    Should -Invoke -CommandName Invoke-CommandLine -Times 1
  }
}

Describe "import project with transformer" {
  BeforeEach{
    Mock -CommandName Remove-Item -MockWith {}
  }

  It "shall cleanup existing directories if clean is true" {
    Mock -CommandName New-Item -MockWith {}
    Mock -CommandName Push-Location -MockWith {}
    Mock -CommandName git -MockWith {}
    Mock -CommandName Invoke-CommandLine -MockWith {}
    Mock -CommandName Pop-Location -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true}

    Run-Transformer -Source "mysource" -Variant "myvariant" -Clean $true
    Should -Invoke -CommandName Remove-Item -Times 1
  }

  It "shall keep all files and directories if clean is false, but create a new dir and update git repo" {
    Mock -CommandName New-Item -MockWith {}
    Mock -CommandName Push-Location -MockWith {}
    Mock -CommandName git -MockWith {}
    Mock -CommandName Invoke-CommandLine -MockWith {}
    Mock -CommandName Pop-Location -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true}

    Run-Transformer -Source "mysource" -Variant "myvariant" -Clean $false
    Should -Invoke -CommandName Remove-Item -Times 0
    Should -Invoke -CommandName git -Times 3
    Should -Invoke -CommandName New-Item -Times 1
  }

  It "shall shall clone instead of pull when clean is active" {
    Mock -CommandName New-Item -MockWith {}
    Mock -CommandName Push-Location -MockWith {}
    Mock -CommandName git -MockWith {}
    Mock -CommandName Invoke-CommandLine -MockWith {}
    Mock -CommandName Pop-Location -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true} -ParameterFilter { $Path -eq "build/import" }
    Mock -CommandName Test-Path -MockWith {$false} -ParameterFilter { $Path -eq ".git" }

    Run-Transformer -Source "mysource" -Variant "myvariant" -Clean $true
    Should -Invoke -CommandName Remove-Item -Times 1
    Should -Invoke -CommandName git -Times 1
    Should -Invoke -CommandName New-Item -Times 1
  }
}

Describe "running CMake" {
  BeforeEach{
    Mock -CommandName Invoke-CommandLine -MockWith {}
  }

  It "shall run target selftests" {
    Run-CMake-Build -Target "selftests" -Variants "myvariant" -Filter "" -NinjaArgs "" -Clean $false -Reconfigure $false
    Should -Invoke -CommandName Invoke-CommandLine -Times 1
  }

  It "shall run target selftests and clean before and filter" {
    Mock -CommandName Remove-Item -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true}

    Run-CMake-Build -Target "selftests" -Variants "myvariant" -Filter "abc" -NinjaArgs "" -Clean $true -Reconfigure $false
    Should -Invoke -CommandName Invoke-CommandLine -Times 1
    Should -Invoke -CommandName Remove-Item -Times 1
  }

  It "shall run multiple CLI commands for other targets and also delete files to reconfigure" {
    Mock -CommandName Remove-Item -MockWith {}
    Mock -CommandName Test-Path -MockWith {$true}

    Run-CMake-Build -Target "mytarget" -Variants "myvariant" -Filter "abc" -NinjaArgs "" -Clean $false -Reconfigure $true
    Should -Invoke -CommandName Invoke-CommandLine -Times 3
    Should -Invoke -CommandName Remove-Item -Times 2
  }
}
