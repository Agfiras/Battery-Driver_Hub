# Define the path to the INSTALL.BAT file
$installScriptPath = "C:\Users\Adok\source\repos\Agfiras\Battery-Driver_Hub\x64\Debug\simbatt\INSTALL.BAT"

# Check if the file exists
if (Test-Path $installScriptPath -PathType Leaf) {
    # Run the INSTALL.BAT file as administrator
    Start-Process -FilePath "cmd.exe" -ArgumentList "/c $installScriptPath" -Verb RunAs -Wait
} else {
    Write-Host "INSTALL.BAT file not found at $installScriptPath"
}
