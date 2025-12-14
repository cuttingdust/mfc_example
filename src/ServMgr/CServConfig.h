#pragma once

#include <afxwin.h>

class CServItem
{
public:
    CString    m_strServName;
    CString    m_strServDispName;
    DWORD      m_dwServStatus;
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
};
