#define INITGUID
#include "Battery.hpp"
#include "DeviceInstance.hpp"
#include <Cgos.h>
#include <iostream>
#include <Windows.h>
#include <wrl/wrappers/corewrappers.h>

int ReadBatInfo(std::string command, std::string& timeToEmpty) 
{  
    unsigned int percentageRemaining = 0;   // Battery percentage remaining
    byte addr = 0x26; // Pic Address
    byte cmd = 0;     // Commande

    try {
        addr = std::stoi("26", nullptr, 16);    // Convert hex string to integer
        cmd = std::stoi(command, nullptr, 16);  // Convert hex string to integer
    }
    catch (const std::exception&) {
        std::cerr << "Invalid argument: " << command << std::endl; 
    }

    UINT16 data_u16 = 0;
    byte rData[2] = { 0 }; // Use stack-allocated array
    byte wData[1] = { cmd };  // Use stack-allocated array

    CgosLibInitialize();
    HCGOS hCgos = CgosBoardOpen(0, 0, 0, nullptr);  // Open the first CGOS board
    try {
        CgosI2CWriteReadCombined(hCgos, 0, addr, wData, 1, rData, 2);    // Write and read data
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl; 
    }

    if (command == "0D") {
        percentageRemaining = rData[0];     // Battery percentage remaining
    }
    else if (command == "12") {
        data_u16 = (UINT16)((rData[1] << 8) | rData[0]);  // Battery time to empty
        if (data_u16 == 0xFFFF) {
            return 1;
        }
        else 
        {
           int hours = data_u16 / 60; 
           int minutes = data_u16 % 60;
           timeToEmpty = std::to_string(hours) + " hours " + std::to_string(minutes) + " minutes"; 
           std::cout << "Time to empty: " << timeToEmpty << std::endl;  
          return 0;
        }
    }

    return percentageRemaining;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    while (true) {
    std::string timeToEmpty;    
    const unsigned int newCharge = ReadBatInfo("0D",timeToEmpty); // Read battery percentage remaining
    const unsigned int PowerInfo = ReadBatInfo("12",timeToEmpty); // Read battery time to empty
   
    // wprintf(L"Power state: Battery is %s\n", PowerInfo == 1 ? L"powered" : L"not powered"); // Check if battery is powered

    wchar_t deviceInstancePath[18] = L"ROOT\\BATTERY\\0000" ; // Device instance path

    //wprintf(L"DeviceInstancePath: %s\n", deviceInstancePath); // Print device instance path

    std::wstring pdoPath; // PDO path

    try {
        DeviceInstance dev(deviceInstancePath); // Device instance
        auto ver = dev.GetDriverVersion(); // Get driver version
        //wprintf(L"  Driver version: %s.\n", ver.c_str()); // Print driver version
        auto time = dev.GetDriverDate(); // Get driver date
        //wprintf(L"  Driver date: %s.\n", DeviceInstance::FileTimeToDateStr(time).c_str()); // Print driver date
        pdoPath = dev.GetPDOPath(); // Get PDO path
    }
    catch (std::exception& e) {
        wprintf(L"ERROR: Unable to locate battery %s\n", deviceInstancePath);
        wprintf(L"ERROR: what: %hs\n", e.what());
        return -1;
    }

    //wprintf(L"Opening %s\n", pdoPath.c_str());
    Microsoft::WRL::Wrappers::FileHandle battery(CreateFileW(pdoPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr)); // Open battery

    if (!battery.IsValid()) {
        DWORD err = GetLastError(); // Get last error
        //wprintf(L"ERROR: CreateFileW (err=%i).\n", err); // print Error     
        return -1;
    }


    BatteryStausWrap status(battery.Get()); // Battery status
    //wprintf(L"Battery status (before update):\n"); // Print battery status
    //status.Print(); // Print battery status
    //wprintf(L"\n"); // Print new line

    wprintf(L"New charge level: %u\n", newCharge);

    // Toggle between charge and discharge
    status.PowerState = (PowerInfo == 1) ? BATTERY_POWER_ON_LINE | BATTERY_CHARGING : BATTERY_DISCHARGING; // Set power state

    status.Capacity = newCharge;  // Set the new charge level

    if(PowerInfo == 1)       
	{
		status.Rate = 80; // 80W
	}
	else
	{
		status.Rate = -80; // -80W
	}

    status.Voltage = 14;   // 14V

    status.Set(battery.Get()); // Set the new status

    Sleep(2000); // Sleep for 2 seconds

    }
    return 0;
}
