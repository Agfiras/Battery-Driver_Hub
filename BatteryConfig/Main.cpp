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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector> 

UINT16 percentageRemaining = 100;

 void ReadBatInfo(std::string command) {

    unsigned int i2cCongatecI2CIndex = 0; // I2C index
    std::string timeToEmpty;      
    std::string timeToFull;          
    HCGOS hCgos = 0;    // Handle to the CGOS library
    byte addr = 26; // Pic Address;
    byte cmd = 0; // Command
    
    try   
    {
        //convert addr and cmd to byte
        addr = std::stoi("26", nullptr, 16);
        cmd = std::stoi(command, nullptr, 16);
    }
    catch (const std::exception&) {
        std::cerr << "Invalid argument: " << command << std::endl; 
    }

    UINT16 data_u16 = 0;
    byte* rData = new byte[2]();
    byte* wData = new byte[1]();
    wData[0] = cmd;

    CgosLibInitialize();
    hCgos = CgosBoardOpen(0, 0, 0, &hCgos);
    try
    {
        CgosI2CWriteReadCombined(hCgos, i2cCongatecI2CIndex, addr, &wData[0], 1,  &rData[0], 2);
	}   
	catch (const std::exception& e) 
    {
		std::cerr << "Error: " << e.what() << std::endl;
	}


    if(command == "0D")
        {
        if (rData[0] <= 100)
        {
            percentageRemaining = rData[0];
            std::cout << "Percentage remaining: " << percentageRemaining << "%" << std::endl;
        }
        else if (command == "12")
        {data_u16 = (UINT16)((rData[1] << 8) | rData[0]);
            if (data_u16 == 0xFFFF)
            {
                timeToEmpty = "Powered.";
                std::cout << timeToEmpty<< std::endl;
            }
            else
            {
                int hours = data_u16 / 60;
                int minutes = data_u16 % 60;
                std::stringstream ss;
                ss << hours << "h" << minutes << "m";
                timeToEmpty = ss.str();
                std::cout << timeToEmpty<< std::endl;
            }
        }
        else if (command == "13")
        {
            data_u16 = (UINT16)((rData[1] << 8) | rData[0]);
			if (data_u16 == 0xFFFF)
			{
				timeToFull = "Not charging";
                std::cout << timeToFull<< std::endl;
			}
			else
			{
                if (data_u16 <= 300)
                {
                    if (data_u16 == 0)
                    {
                        timeToFull = "Charged.";
                        std::cout << timeToFull<< std::endl;
                    }
                    else
                    {
                        int hours = data_u16 / 60;
                        int minutes = data_u16 % 60;
                        std::stringstream ss;
                        ss << hours << "h" << minutes << "m";
                        timeToEmpty = ss.str();
                        std::cout << timeToEmpty<< std::endl;
                    }
                }

			}
		}
    }
};
int wmain(int argc, wchar_t* argv[]) {
  HCGOS hCgos = 0;
  unsigned int cntCongatecI2C = 0;
  unsigned int i2cCongatecI2CIndex = 0;
  int picAddress = 0x26; // Pic Address;
  unsigned long dwUnit = 0 ;
  unsigned char bAddr = 0;
  unsigned char pDatabyte = 0;
  unsigned long wReg = 0;
  // Get library and driver versions
  unsigned long dwLibVersion = CgosLibGetVersion();
  unsigned long dwDrvVersion = CgosLibGetDrvVersion();
  std::cout << "Library version: " << dwLibVersion << std::endl;
  // Check library initialization
  if (!CgosLibInitialize()) {
    // Handle failed initialization (e.g., log error)
    std::cerr << "Error: CgosLibInitialize failed" << std::endl;
    return -1;
    {

  // Driver installation check (optional)
  if (!CgosLibInitialize()) { // Assuming this function exists
    if (!CgosLibInstall(1)) {
      std::cerr << "Error: driver installation failed" << std::endl;
      CgosLibUninitialize(); // Cleanup even if install fails
      return -1;
    }
   }
  }
  }

  ReadBatInfo("0D");
  ReadBatInfo("12");
  ReadBatInfo("13");

  // Open the board
  hCgos = CgosBoardOpen(0, 0, 0, &hCgos);


  if (!hCgos) 
    {
    std::cerr << "Could not open a board" << std::endl;
    } 
  else 
  {
    std::cerr << "Board opened successfully" << std::endl;
  // Close the board and cleanup
  if (hCgos) {
    CgosBoardClose(hCgos);
  }
  CgosLibUninitialize();
  std::cout << "checked and closed" << std::endl;
  return 0;
  }

    const unsigned int batteryIdx = _wtoi(argv[1]); // 0 is first battery
    unsigned int newCharge = percentageRemaining ;

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
};
