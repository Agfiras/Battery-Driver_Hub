# Check Secure Boot status
$secureBoot = Get-WmiObject -Namespace root\cimv2\Security\MicrosoftTpm -Class Win32_Tpm

if ($secureBoot.IsEndorsementKeyPairPresent -eq $true) {
    Write-Output "Secure Boot is enabled."
} else {
    Write-Output "Secure Boot is not enabled."
}

# Pause execution so you can see the output
pause