// BatteryService.hpp
#pragma once

#include "ServiceBase.hpp"
#include <string>
#include <thread>
#include <atomic>

class CBatteryService : public CServiceBase {
public:
    CBatteryService(LPCTSTR serviceName);
protected:
    virtual void OnStart(DWORD dwArgc, LPTSTR* lpszArgv);
    virtual void OnStop();
private:
    void ServiceWorkerThread();
    std::thread m_worker;
    std::atomic<bool> m_stopRequested;
    int ReadBatInfo(std::string command, std::string& timeToEmpty);
};
