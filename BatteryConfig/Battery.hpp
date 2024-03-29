#pragma once
#include <Windows.h>
#include <poclass.h>
#include "../simbatt/simbattdriverif.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <iostream>


/** Convenience function for getting the battery tag that's needed for some IOCTL calls. */
ULONG GetBatteryTag(HANDLE device) {
    // get battery tag (needed in later calls)
    ULONG battery_tag = 0;
    ULONG wait = 0;
    DWORD bytes_returned = 0;
    BOOL ok = DeviceIoControl(device, IOCTL_BATTERY_QUERY_TAG, &wait, sizeof(wait), &battery_tag, sizeof(battery_tag), &bytes_returned, nullptr);
    if (!ok) {
        DWORD err = GetLastError();
        wprintf(L"ERROR: IOCTL_BATTERY_QUERY_TAG (err=%i).\n", err);
        return -1;
    }
    return battery_tag;
}

/** Convenience C++ wrapper for BATTERY_STATUS. */
struct BatteryStausWrap : BATTERY_STATUS {
    BatteryStausWrap(HANDLE device = INVALID_HANDLE_VALUE) : BATTERY_STATUS{} {
        if (device != INVALID_HANDLE_VALUE)
            Get(device);
    }

    /** Standard getter. */
    void Get(HANDLE device) {
        // query BATTERY_STATUS status
        BATTERY_WAIT_STATUS wait_status = {};
        wait_status.BatteryTag = GetBatteryTag(device);
        DWORD bytes_returned = 0;
        BOOL ok = DeviceIoControl(device, IOCTL_BATTERY_QUERY_STATUS, &wait_status, sizeof(wait_status), this, sizeof(*this), &bytes_returned, nullptr);
        if (!ok) {
            //DWORD err = GetLastError();
            throw std::runtime_error("IOCTL_BATTERY_QUERY_STATUS error");
        }
    }

    /** SimBatt-specific setter. */
    void Set(HANDLE device) {
        BOOL ok = DeviceIoControl(device, IOCTL_SIMBATT_SET_STATUS, this, sizeof(*this), nullptr, 0, nullptr, nullptr);
        if (!ok) {
            //DWORD err = GetLastError();
            throw std::runtime_error("IOCTL_SIMBATT_SET_STATUS error");
        }
    }

    void Print() {
        wprintf(L"  Capacity=%i\n", Capacity);
        wprintf(L"  PowerState=%x\n", PowerState);
        wprintf(L"  Rate=%x\n", Rate);
        wprintf(L"  Voltage=%i\n", Voltage);
    }
};
static_assert(sizeof(BatteryStausWrap) == sizeof(BATTERY_STATUS));