@echo off
:: Goto current directory
cd /d "%~dp0"

:: Delete simulated battery
devgen /remove "SWD\DEVGEN\1"

:: Uninstall driver
pnputil.exe /delete-driver simbatt.inf /uninstall

pause
