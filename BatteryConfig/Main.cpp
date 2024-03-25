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
#include <string>

int ReadBatInfo(std::string command) {

    unsigned int percentageRemaining = 0;
    HCGOS hCgos = 0;    // Handle to the CGOS library
    unsigned int i2cCongatecI2CIndex = 0; // I2C index
    std::string timeToEmpty;      
    std::string timeToFull;          
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
            //std::cout << "Percentage remaining: " << percentageRemaining << "%" << std::endl;
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
    delete[] rData;
    delete[] wData;
    return percentageRemaining;
};

int wmain() {

  const unsigned int newCharge = ReadBatInfo("0D") ;  // read battery charge level
  wprintf(L"Initial charge level: %u\n", newCharge);    // print battery charge level

	// get battery device instance path
  wchar_t deviceInstancePath[18] = L"ROOT\\BATTERY\\0000";

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
                

    	wprintf(L"New charge level: %u\n", newCharge);
        // toggle between charge and dischage
        if (newCharge > status.Capacity)
            status.PowerState = BATTERY_POWER_ON_LINE | BATTERY_CHARGING; // charging while on AC power
        else if (newCharge < status.Capacity)
            status.PowerState = BATTERY_DISCHARGING; // discharging
        else
            status.PowerState = 0; // same charge as before

        // update charge level
        status.Capacity =newCharge;

        status.Rate = BATTERY_UNKNOWN_RATE; // was 0
        status.Voltage = BATTERY_UNKNOWN_VOLTAGE; // was -1

        status.Set(battery.Get());
    

    wprintf(L"Battery status (after update):\n");
    status.Print();

    return 0; 
};
