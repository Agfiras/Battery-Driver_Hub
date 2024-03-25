

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cgos.h>
#include <stdexcept>
#include <vector>
#include <rpcndr.h>
#include <Windows.h>
int ReadBatInfo(std::string command) {

    unsigned int percentageRemaining = 0;
    HCGOS hCgos = 0;    // Handle to the CGOS library
    unsigned int i2cCongatecI2CIndex = 0; // I2C index
    bool powered = false;
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
        CgosI2CWriteReadCombined(hCgos, i2cCongatecI2CIndex, addr, &wData[0], 1, &rData[0], 2);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    if (command == "0D")
    {
        if (rData[0] <= 100)
        {
            percentageRemaining = rData[0];
        }
    }
    else if (command == "12")
    {
        data_u16 = (UINT16)((rData[1] << 8) | rData[0]);
        if (data_u16 == 0xFFFF)
        {
            powered = true;
            std::cout << "powered " << std::endl;
        }
        else
        {
            powered = false;
        }
    }
    delete[] rData;
    delete[] wData;
    return percentageRemaining;
    return powered;
};


void main()
{
    std::string command = "0D";
    ReadBatInfo(command);
    std::string command1 = "12";
    ReadBatInfo(command1);
    return;
};