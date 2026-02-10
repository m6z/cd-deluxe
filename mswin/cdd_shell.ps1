# Cd Deluxe PowerShell Demo Shell
$env:Path = "$PSScriptRoot;$env:Path"

# Load the cd-deluxe PowerShell integration
. "$PSScriptRoot\cdd.ps1"

# Override the built-in 'cd' command with cdd
Set-Alias -Name cd -Value cdd -Option AllScope -Scope Global

Set-Location C:\
Push-Location $PSScriptRoot

Write-Host "Welcome to the Cd Deluxe PowerShell shell.  This is primarily for demonstrating the cdd command."
Write-Host ""
Write-Host "The Cd Deluxe command 'cdd' is typically a drop in replacement for the default 'cd' command."
Write-Host 'The "cd" command is aliased to "cdd" via: Set-Alias -Name cd -Value cdd'
Write-Host ""
Write-Host 'Typing "cd --help" will show you the available options for the cdd command.'
Write-Host ""
Write-Host 'Typing "cd -d+" shows the history of all directories visited in this session.'
Write-Host "Try changing directories and then issue the 'cd -d+' command again."
Write-Host ""

Write-Host "cd -d+"
cdd -d+
