// ServiceBase.cpp
#include "ServiceBase.hpp"

CServiceBase* CServiceBase::s_service = NULL;

BOOL CServiceBase::Run(CServiceBase& service) {
    s_service = &service;

    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { service.m_name, ServiceMain },
        { NULL, NULL }
    };

    return StartServiceCtrlDispatcher(serviceTable);
}

CServiceBase::CServiceBase(LPCTSTR serviceName, BOOL canStop, BOOL canShutdown, BOOL canPauseContinue) {
    m_name = (LPTSTR)serviceName;
    m_statusHandle = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_START_PENDING;

    DWORD dwControlsAccepted = 0;
    if (canStop) dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    if (canShutdown) dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    if (canPauseContinue) dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    m_status.dwControlsAccepted = dwControlsAccepted;
}

CServiceBase::~CServiceBase(void) {}

void CServiceBase::Stop() {
    SetServiceStatus(SERVICE_STOP_PENDING);
    OnStop();
    SetServiceStatus(SERVICE_STOPPED);
}

void CServiceBase::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    s_service->m_statusHandle = RegisterServiceCtrlHandler(s_service->m_name, ServiceCtrlHandler);
    if (s_service->m_statusHandle == NULL) {
        s_service->WriteErrorLogEntry(TEXT("RegisterServiceCtrlHandler"));
        return;
    }
    s_service->SetServiceStatus(SERVICE_START_PENDING);
    s_service->OnStart(dwArgc, lpszArgv);
    s_service->SetServiceStatus(SERVICE_RUNNING);
}

void CServiceBase::ServiceCtrlHandler(DWORD dwCtrl) {
    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        s_service->SetServiceStatus(SERVICE_STOP_PENDING);
        s_service->OnStop();
        s_service->SetServiceStatus(SERVICE_STOPPED);
        break;
    case SERVICE_CONTROL_PAUSE:
        s_service->OnPause();
        break;
    case SERVICE_CONTROL_CONTINUE:
        s_service->OnContinue();
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        s_service->OnShutdown();
        break;
    default:
        break;
    }
}

void CServiceBase::SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    static DWORD dwCheckPoint = 1;

    m_status.dwCurrentState = dwCurrentState;
    m_status.dwWin32ExitCode = dwWin32ExitCode;
    m_status.dwWaitHint = dwWaitHint;
    m_status.dwCheckPoint = ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) ? 0 : dwCheckPoint++;

    ::SetServiceStatus(m_statusHandle, &m_status);
}

void CServiceBase::OnStart(DWORD dwArgc, LPTSTR* lpszArgv) {}

void CServiceBase::OnStop() {}

void CServiceBase::OnPause() {}

void CServiceBase::OnContinue() {}

void CServiceBase::OnShutdown() {}

void CServiceBase::WriteEventLogEntry(PCTSTR pszMessage, WORD wType) {
    HANDLE hEventSource = NULL;
    LPCWSTR lpszStrings[2] = { NULL, NULL };

    hEventSource = RegisterEventSource(NULL, m_name);
    if (hEventSource != NULL) {
        lpszStrings[0] = m_name;
        lpszStrings[1] = pszMessage;

        ReportEvent(hEventSource, wType, 0, 0, NULL, 2, 0, lpszStrings, NULL);
        DeregisterEventSource(hEventSource);
    }
}

void CServiceBase::WriteErrorLogEntry(PCTSTR pszFunction, DWORD dwError) {
    TCHAR szMessage[260];
    StringCchPrintf(szMessage, ARRAYSIZE(szMessage), TEXT("%s failed w/err 0x%08lx"), pszFunction, dwError);
    WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}
