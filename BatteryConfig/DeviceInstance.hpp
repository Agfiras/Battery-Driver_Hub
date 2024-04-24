#pragma once
#include <Cfgmgr32.h>
#pragma comment(lib, "Cfgmgr32.lib")
#include <Devpkey.h> // for DEVPKEY_Device_PDOName
#include <variant>
#include <vector>


class DeviceInstance {
public:
    DeviceInstance(wchar_t* deviceInstancePath) {
        CONFIGRET res = CM_Locate_DevNodeW(&m_devInst, deviceInstancePath, CM_LOCATE_DEVNODE_NORMAL); // CM_LOCATE_DEVNODE_NORMAL or CM_LOCATE_DEVNODE_PHANTOM
        if (res != CR_SUCCESS)
            throw std::runtime_error("ERROR: CM_Locate_DevNodeW"); // Print error
    }

    ~DeviceInstance() {
    }

    std::wstring GetDriverVersion() const {
        auto res = GetProperty(DEVPKEY_Device_DriverVersion); // Get driver version
        return std::get<std::wstring>(res); // Return driver version
    }

    FILETIME GetDriverDate() const {
        auto res = GetProperty(DEVPKEY_Device_DriverDate);  // Get driver date
        return std::get<FILETIME>(res);   // Return driver date
    }

    /** Get the virtual file physical device object (PDO) path of a device driver instance. */
    std::wstring GetPDOPath() const {
        auto res = GetProperty(DEVPKEY_Device_PDOName); // Get PDO name
        return L"\\\\?\\GLOBALROOT" + std::get<std::wstring>(res); // add prefix before PDO name
    }

    std::variant<std::wstring, FILETIME> GetProperty(const DEVPROPKEY& propertyKey) const {
        DEVPROPTYPE propertyType = 0; // Property type
        std::vector<BYTE> buffer(1024, 0); // Buffer
        ULONG buffer_size = (ULONG)buffer.size(); // Buffer size
        CONFIGRET res = CM_Get_DevNode_PropertyW(m_devInst, &propertyKey, &propertyType, buffer.data(), &buffer_size, 0); // Get device node property
        if (res != CR_SUCCESS) {
            wprintf(L"ERROR: CM_Get_DevNode_PropertyW (res=%i).\n", res); // Print error
            return {};
        }
        buffer.resize(buffer_size); // Resize buffer

        if (propertyType == DEVPROP_TYPE_STRING) {
            return reinterpret_cast<wchar_t*>(buffer.data()); // Return string
        } else if (propertyType == DEVPROP_TYPE_FILETIME) {
            return *reinterpret_cast<FILETIME*>(buffer.data()); // Return file time
        }

        throw std::runtime_error("Unsupported CM_Get_DevNode_PropertyW type"); // Print error
    }

    static std::wstring FileTimeToDateStr(FILETIME fileTime) {
        SYSTEMTIME time = {}; // System time
        BOOL ok = FileTimeToSystemTime(&fileTime, &time); // Convert file time to system time
        if (!ok) {
            DWORD err = GetLastError(); // Get last error
            wprintf(L"ERROR: FileTimeToSystemTime failure (res=%i).\n", err); // Print error
            return {}; // Return empty string
        }

        std::wstring dateString(128, L'\0'); // Date string
        int char_count = GetDateFormatW(LOCALE_SYSTEM_DEFAULT, NULL, &time, NULL, dateString.data(), (int)dateString.size()); // Get date format
        dateString.resize(char_count - 1); // exclude zero-termination
        return dateString; // Return date string
    }

private:
    DEVINST m_devInst = 0; // Device instance
};
