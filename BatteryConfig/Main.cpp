#define INITGUID
#include "Battery.hpp"
#include "DeviceInstance.hpp"
#include <wrl/wrappers/corewrappers.h> // for FileHandle
#include <cassert>
#include <Cgos.h>
#include <iostream> 
#include <cstdlib>
#include <cstdlib>
#include <cctype>
#include <Windows.h>
#include "Main.h"
#include <iostream>
#include <sstream>
#include <iomanip>

int wmain(int argc, wchar_t* argv[]) {
  HCGOS hCgos = 0;
  unsigned int cntCongatecI2C = 0;
  unsigned int i2cCongatecI2CIndex = 0;
  int picAddress = 0x26; // Pic Address;
  unsigned long dwUnit = 0 ;
  unsigned char bAddr = 0;
  unsigned char *pDatabyte = 0;
  unsigned long wReg =


  // Get library and driver versions
  unsigned long dwLibVersion = CgosLibGetVersion();
  unsigned long dwDrvVersion = CgosLibGetDrvVersion();

  // Check library initialization
  if (!CgosLibInitialize()) {
    // Handle failed initialization (e.g., log error)
    std::cerr << "Error: CgosLibInitialize failed" << std::endl;
    return -1;
  }

  // Driver installation check (optional)
  if (!CgosLibInitialize()) { // Assuming this function exists
    if (!CgosLibInstall(1)) {
      std::cerr << "Error: driver installation failed" << std::endl;
      CgosLibUninitialize(); // Cleanup even if install fails
      return -1;
    }
  }

  // Open the board
  hCgos = CgosBoardOpen(0, 0, 0, &hCgos);
  if (!hCgos) {
    std::cerr << "Could not open a board" << std::endl;
  } else {
    std::cerr << "Board opened successfully" << std::endl;
    // Your code to interact with the board using hCgos goes here
    if (!CgosI2CIsAvailable(hCgos, dwUnit)) {
        std::cerr << "no available I2C communication" << std::endl;
    }
    else {
        std::cerr << "Availble I2C communications" << std::endl;
        if(!CgosI2CReadRegister(hCgos,dwUnit,bAddr,wReg,*pDatabyte)
    }

  }

  // Close the board and cleanup
  if (hCgos) {
    CgosBoardClose(hCgos);
  }
  CgosLibUninitialize();
  std::cout << "checked and closed" << std::endl;
  return 0;

    const unsigned int batteryIdx = _wtoi(argv[1]); // 0 is first battery
    const unsigned int newCharge = _wtoi(argv[2]);

    wchar_t deviceInstancePath[18] = {};
    swprintf_s(deviceInstancePath, L"SWD\\DEVGEN\\%i", batteryIdx); // add device index suffix
    wprintf(L"DeviceInstancePath: %s\n", deviceInstancePath);

    std::wstring pdoPath;
    try {
        DeviceInstance dev(deviceInstancePath);

        auto ver = dev.GetDriverVersion();
        wprintf(L"  Driver version: %s.\n", ver.c_str());
        auto time = dev.GetDriverDate();
        wprintf(L"  Driver date: %s.\n", DeviceInstance::FileTimeToDateStr(time).c_str());

        pdoPath = dev.GetPDOPath();
    } catch (std::exception& e) {
        wprintf(L"ERROR: Unable to locate battery %s\n", deviceInstancePath);
        wprintf(L"ERROR: what: %hs\n", e.what());
        return -1;
    }

    wprintf(L"Opening %s\n", pdoPath.c_str());
    Microsoft::WRL::Wrappers::FileHandle battery(CreateFileW(pdoPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL));
    if (!battery.IsValid()) {
        DWORD err = GetLastError();
        wprintf(L"ERROR: CreateFileW (err=%i).\n", err);
        return -1;
    }

    BatteryInformationWrap info(battery.Get());
    wprintf(L"\n"); 
    wprintf(L"Battery information:\n");
    info.Print();
    wprintf(L"\n");

    BatteryStausWrap status(battery.Get());
    wprintf(L"Battery status (before update):\n");
    status.Print();
    wprintf(L"\n");

#if 0
    // update battery information
    info.Set(battery.Get());
#endif

    // update battery charge level
    {
        // toggle between charge and dischage
        if (newCharge > status.Capacity)
            status.PowerState = BATTERY_POWER_ON_LINE | BATTERY_CHARGING; // charging while on AC power
        else if (newCharge < status.Capacity)
            status.PowerState = BATTERY_DISCHARGING; // discharging
        else
            status.PowerState = 0; // same charge as before

        // update charge level
        status.Capacity = newCharge;

        status.Rate = BATTERY_UNKNOWN_RATE; // was 0
        status.Voltage = BATTERY_UNKNOWN_VOLTAGE; // was -1

        status.Set(battery.Get());
    }

    wprintf(L"Battery status (after update):\n");
    status.Print();

    return 0; 
}