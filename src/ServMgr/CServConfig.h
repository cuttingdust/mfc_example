#pragma once

#include <afxwin.h>
#include <winsvc.h>

class CServItem
{
public:
    CString    m_strServName;
    CString    m_strServDispName;
    DWORD      m_dwServStatus;
    DWORD      m_dwStartType;
    CString    m_strBinPath;
    CString    m_strDescription;
    CServItem *m_pNext;
    CServItem()
    {
        m_dwServStatus = 0;
        m_pNext        = NULL;
    }
};

class CServConfig
{
public:
    CServItem *EnumServList();
    CString    GetStateString(DWORD dwCurrState);
    CString    GetStartTypeString(DWORD dwCurrStartType);
    BOOL       GetServPathAndStartType(LPCTSTR lpszServName, CServItem &tItem);
    CString    GetServDescription(LPCTSTR lpszServName);
    DWORD      GetServCtrlAccepted(LPCTSTR lpszServName, DWORD *pDwCurrentStatus = NULL);
    BOOL       CtrlServStatus(LPCTSTR lpszServName, DWORD dwNewStatus);
};
