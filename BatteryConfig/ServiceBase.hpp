// ServiceBase.hpp
#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

class CServiceBase {
public:
    static BOOL Run(CServiceBase& service);
    CServiceBase(LPCTSTR serviceName, BOOL canStop = TRUE, BOOL canShutdown = TRUE, BOOL canPauseContinue = FALSE);
    virtual ~CServiceBase(void);

    void Stop();

protected:
    virtual void OnStart(DWORD dwArgc, LPTSTR* lpszArgv);
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();

private:
    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);
    void SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0);
    void WriteEventLogEntry(PCTSTR pszMessage, WORD wType);
    void WriteErrorLogEntry(PCTSTR pszFunction, DWORD dwError = GetLastError());

    static CServiceBase* s_service;
    SERVICE_STATUS m_status;
    SERVICE_STATUS_HANDLE m_statusHandle;
    LPTSTR m_name;
};
