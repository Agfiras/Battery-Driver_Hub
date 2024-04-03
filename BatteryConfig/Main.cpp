#define INITGUID
#include "Battery.hpp"
#include "DeviceInstance.hpp"
#include <Cgos.h>
#include <iostream>
#include <Windows.h>
#include <wrl/wrappers/corewrappers.h>

int ReadBatInfo(std::string command, std::string& timeToEmpty) 
{
    unsigned int percentageRemaining = 0;   
    byte addr = 0x26; // Pic Address
    byte cmd = 0;     // Command

    try {
        addr = std::stoi("26", nullptr, 16);    // Convert hex string to integer
        cmd = std::stoi(command, nullptr, 16);  // Convert hex string to integer
    }
    catch (const std::exception&) {
        std::cerr << "Invalid argument: " << command << std::endl;
    }

    UINT16 data_u16 = 0;
    byte rData[2] = { 0 }; // Use stack-allocated array
    byte wData[1] = { cmd }; 

    CgosLibInitialize();
    HCGOS hCgos = CgosBoardOpen(0, 0, 0, nullptr);  // Open the first CGOS board
    try {
        CgosI2CWriteReadCombined(hCgos, 0, addr, wData, 1, rData, 2);   
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    if (command == "0D") {
        percentageRemaining = rData[0];     // Battery percentage remaining
    }
    else if (command == "12") {
        data_u16 = (UINT16)((rData[1] << 8) | rData[0]);
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
int wmain() {
    while (true) {
    std::string timeToEmpty;
    const unsigned int newCharge = ReadBatInfo("0D",timeToEmpty);
    const unsigned int PowerInfo = ReadBatInfo("12",timeToEmpty);
   
   
    wprintf(L"Power state: Battery is %s\n", PowerInfo == 1 ? L"powered" : L"not powered");

    wchar_t deviceInstancePath[18] = L"SWD\\DEVGEN\\1";
    wprintf(L"DeviceInstancePath: %s\n", deviceInstancePath);

    std::wstring pdoPath;

    try {
        DeviceInstance dev(deviceInstancePath);
        auto ver = dev.GetDriverVersion();
        wprintf(L"  Driver version: %s.\n", ver.c_str());
        auto time = dev.GetDriverDate();
        wprintf(L"  Driver date: %s.\n", DeviceInstance::FileTimeToDateStr(time).c_str());
        pdoPath = dev.GetPDOPath();
    }
    catch (std::exception& e) {
        wprintf(L"ERROR: Unable to locate battery %s\n", deviceInstancePath);
        wprintf(L"ERROR: what: %hs\n", e.what());
        return -1;
    }

    wprintf(L"Opening %s\n", pdoPath.c_str());
    Microsoft::WRL::Wrappers::FileHandle battery(CreateFileW(pdoPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr));

    if (!battery.IsValid()) {
        DWORD err = GetLastError();
        wprintf(L"ERROR: CreateFileW (err=%i).\n", err);
        return -1;
    }


    BatteryInformationWrap RateWrap(battery.Get());
    wprintf(L"Battery information:\n");
    RateWrap.Print();
    wprintf(L"\n");


    BatteryStausWrap status(battery.Get());
    wprintf(L"Battery status (before update):\n");
    status.Print();
    wprintf(L"\n");

    wprintf(L"New charge level: %u\n", newCharge);

    // Toggle between charge and discharge
    status.PowerState = (PowerInfo == 1) ? BATTERY_POWER_ON_LINE | BATTERY_CHARGING : BATTERY_DISCHARGING;

    RateWrap.AtRate = -6; // 0 minutes
    //RateWrap.Set(battery.Get()); // Set the new rate)
    status.Capacity = newCharge; 
    status.Rate = -30; // 95W
    status.Voltage = 14;    // 19V
    status.Set(battery.Get()); // Set the new status

    Sleep(5000);

    }
    return 0;
}
