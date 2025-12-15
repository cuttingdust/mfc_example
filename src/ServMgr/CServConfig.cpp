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
        GetServPathAndStartType(pServeNext->m_strServName, *pServeNext);
        pServeNext->m_strDescription = GetServDescription(pServeNext->m_strServName);
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

CString CServConfig::GetStartTypeString(DWORD dwCurrStartType)
{
    CString strResult;
    switch (dwCurrStartType)
    {
        case SERVICE_AUTO_START:
            strResult = L"自动";
            break;
        case SERVICE_DEMAND_START:
            strResult = L"手动";
            break;
        case SERVICE_DISABLED:
            strResult = L"已禁用";
            break;

        default:
            strResult = L"未知";
            break;
    }

    return strResult;
}

BOOL CServConfig::GetServPathAndStartType(LPCTSTR lpszServName, CServItem &tItem)
{
    BOOL      bRet = FALSE;
    SC_HANDLE hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hScm)
        return bRet;

    SC_HANDLE hSvc = ::OpenService(hScm, lpszServName, SERVICE_QUERY_CONFIG);
    if (!hSvc)
    {
        ::CloseServiceHandle(hScm);
        return bRet;
    }

    QUERY_SERVICE_CONFIG *pServCfg      = NULL;
    DWORD                 cbBytesNeeded = 0, cbBufferSize = 0;
    if (!::QueryServiceConfig(hSvc, NULL, 0, &cbBytesNeeded))
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            pServCfg = new QUERY_SERVICE_CONFIG[cbBytesNeeded + 1];
            ::ZeroMemory(pServCfg, cbBytesNeeded + 1);
            cbBufferSize  = cbBytesNeeded;
            cbBytesNeeded = 0;
        }
        else
        {
            goto __Error_End;
        }
    }

    if (!::QueryServiceConfig(hSvc, pServCfg, cbBufferSize, &cbBytesNeeded))
    {
        goto __Error_End;
    }

    tItem.m_strBinPath  = pServCfg->lpBinaryPathName;
    tItem.m_dwStartType = pServCfg->dwStartType;
    bRet                = TRUE;

__Error_End:
    ::CloseServiceHandle(hSvc);
    ::CloseServiceHandle(hScm);
    return bRet;
}

CString CServConfig::GetServDescription(LPCTSTR lpszServName)
{
    CString   strResult;
    SC_HANDLE hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hScm)
        return strResult;

    SC_HANDLE hSvc = ::OpenService(hScm, lpszServName, SERVICE_QUERY_CONFIG);
    if (!hSvc)
    {
        ::CloseServiceHandle(hScm);
        return strResult;
    }

    DWORD                 dwNeeded = 0, dwLen = 0;
    LPSERVICE_DESCRIPTION pDescription = NULL;
    if (!::QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwNeeded))
    {
        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            dwLen        = dwNeeded + 1;
            pDescription = new SERVICE_DESCRIPTION[dwLen];
            ::ZeroMemory(pDescription, dwLen);
            if (::QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)pDescription, dwLen, &dwNeeded))
            {
                strResult = pDescription->lpDescription;
            }
        }
    }
    ::CloseServiceHandle(hSvc);
    ::CloseServiceHandle(hScm);

    return strResult;
}

DWORD CServConfig::GetServCtrlAccepted(LPCTSTR lpszServName, DWORD *pDwCurrentStatus)
{
    DWORD     dwCtrlAccepted = -1;
    SC_HANDLE hScm           = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hScm)
        return dwCtrlAccepted;

    SC_HANDLE hSvc = ::OpenService(hScm, lpszServName, SC_MANAGER_ALL_ACCESS);
    if (!hSvc)
    {
        ::CloseServiceHandle(hScm);
        return dwCtrlAccepted;
    }

    SERVICE_STATUS sTs = { 0 };
    if (!::QueryServiceStatus(hSvc, &sTs))
    {
        goto __ERROR_END;
    }

    dwCtrlAccepted = sTs.dwControlsAccepted;
    if (pDwCurrentStatus)
    {
        *pDwCurrentStatus = sTs.dwCurrentState;
    }

__ERROR_END:
    ::CloseServiceHandle(hSvc);
    ::CloseServiceHandle(hScm);
    return dwCtrlAccepted;
}

BOOL CServConfig::CtrlServStatus(LPCTSTR lpszServName, DWORD dwNewStatus)
{
    BOOL      bRet = FALSE;
    SC_HANDLE hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hScm)
        return bRet;

    SC_HANDLE hSvc = ::OpenService(hScm, lpszServName, SC_MANAGER_ALL_ACCESS);
    if (!hSvc)
    {
        ::CloseServiceHandle(hScm);
        return bRet;
    }
    SERVICE_STATUS sTs = { 0 };
    if (!::QueryServiceStatus(hSvc, &sTs))
    {
        goto __ERROR_END;
    }

    /// 由于服务是系统比较高级的操作 并不是所有状态都可以切换
    if (sTs.dwCurrentState == dwNewStatus)
    {
        bRet = TRUE;
    }
    else if (sTs.dwCurrentState == SERVICE_START_PENDING || sTs.dwCurrentState == SERVICE_STOP_PENDING ||
             sTs.dwCurrentState == SERVICE_CONTINUE_PENDING || sTs.dwCurrentState == SERVICE_PAUSE_PENDING)
    {
        return FALSE;
    }
    else if (sTs.dwCurrentState == SERVICE_STOPPED && dwNewStatus == SERVICE_RUNNING)
    {
        bRet = ::StartService(hSvc, NULL, NULL);
    }
    else if ((sTs.dwCurrentState == SERVICE_RUNNING || sTs.dwCurrentState == SERVICE_PAUSED) &&
             dwNewStatus == SERVICE_STOPPED)
    {
        bRet = ::ControlService(hSvc, SERVICE_CONTROL_STOP, &sTs);
    }
    else if ((sTs.dwCurrentState == SERVICE_PAUSED) && dwNewStatus == SERVICE_RUNNING)
    {
        bRet = ::ControlService(hSvc, SERVICE_CONTROL_CONTINUE, &sTs);
    }
    else if ((sTs.dwCurrentState == SERVICE_RUNNING) && dwNewStatus == SERVICE_PAUSED)
    {
        bRet = ::ControlService(hSvc, SERVICE_CONTROL_PAUSE, &sTs);
    }


__ERROR_END:
    ::CloseServiceHandle(hSvc);
    ::CloseServiceHandle(hScm);
    return bRet;
}
