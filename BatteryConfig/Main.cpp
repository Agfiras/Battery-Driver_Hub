// main.cpp
#include <windows.h>
#include "BatteryService.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    CBatteryService service(TEXT("BatteryService"));
    if (!CServiceBase::Run(service)) {
        DWORD dwError = GetLastError();
        wchar_t errMsg[256];
        swprintf_s(errMsg, L"Service failed to run with error: %lu\n", dwError);
        MessageBox(NULL, errMsg, L"Error", MB_OK | MB_ICONERROR);
    }
    return 0;
} 
