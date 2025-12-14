#include "CServConfig.h"

CServItem *CServConfig::EnumServList()
{
    SC_HANDLE hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hScm)
        return NULL;

    CServItem            *pServerHeader = NULL, *pServePre = NULL, *pServeNext = NULL;
    LPENUM_SERVICE_STATUS pServStatus   = NULL;
    DWORD                 dwBytesNeeded = 0, dwServCound = 0, dwResume = 0, dwRealBytes = 0;
    BOOL bRet = ::EnumServicesStatus(hScm, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &dwBytesNeeded, &dwServCound,
                                     &dwResume);

    if (!bRet && GetLastError() == ERROR_MORE_DATA)
    {
        dwRealBytes = dwBytesNeeded;
        pServStatus = new ENUM_SERVICE_STATUS[dwRealBytes + 1];
        ::ZeroMemory(pServStatus, dwRealBytes + 1);
        bRet = ::EnumServicesStatus(hScm, SERVICE_WIN32, SERVICE_STATE_ALL, pServStatus, dwRealBytes, &dwBytesNeeded,
                                    &dwServCound, &dwResume);

        if (!bRet)
        {
            ::CloseServiceHandle(hScm);
        }
    }
    else
    {
        ::CloseServiceHandle(hScm);
        return NULL;
    }

    pServePre = pServeNext;
    for (DWORD dwIdx = 0; dwIdx < dwServCound; ++dwIdx)
    {
        pServeNext                    = new CServItem;
        pServeNext->m_strServName     = pServStatus[dwIdx].lpServiceName;
        pServeNext->m_strServDispName = pServStatus[dwIdx].lpDisplayName;
        pServeNext->m_dwServStatus    = pServStatus[dwIdx].ServiceStatus.dwCurrentState;
        (pServerHeader == NULL) ? (pServerHeader = pServeNext) : pServerHeader;
        (pServePre == NULL) ? (pServePre = pServeNext) : (pServePre->m_pNext = pServeNext, pServePre = pServeNext);
    }

    ::CloseServiceHandle(hScm);
    delete[] pServStatus;
    return pServerHeader;
}

CString CServConfig::GetStateString(DWORD dwCurrState)
{
    CString strResult;
    switch (dwCurrState)
    {
        case SERVICE_START_PENDING:
            strResult = L"正在启动";
            break;
        case SERVICE_STOP_PENDING:
            strResult = L"正在停止";
            break;
        case SERVICE_RUNNING:
            strResult = L"已启动";
            break;
        case SERVICE_CONTINUE_PENDING:
            strResult = L"继续中";
            break;
        case SERVICE_PAUSE_PENDING:
            strResult = L"暂停中";
            break;
        case SERVICE_PAUSED:
            strResult = L"暂停";
            break;
        case SERVICE_STOPPED:
            strResult = L"已关闭";
            break;
        default:
            break;
    }

    return strResult;
}
