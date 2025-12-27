// CWMPPlayer4.h  : 由 Microsoft Visual C++ 创建的 ActiveX 控件包装器类的声明

#pragma once
#include <afxdisp.h>
#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CWMPPlayer4

class CWMPPlayer4 : public CWnd
{
protected:
    DECLARE_DYNCREATE(CWMPPlayer4)
public:
    CLSID const& GetClsid()
    {
        static CLSID const clsid = { 0x6C497D62, 0x8919, 0x413C, { 0x82, 0xDB, 0xE9, 0x35, 0xFB, 0x3E, 0xC5, 0x84 } };
        return clsid;
    }
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,
                        CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr)
    {
        return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
    }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
                CFile* pPersist = nullptr, BOOL bStorage = FALSE, BSTR bstrLicKey = nullptr)
    {
        return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID, pPersist, bStorage,
                             bstrLicKey);
    }

    // 特性
public:
    // 操作
public:
    void close()
    {
        InvokeHelper(0x3, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
    }

    CString get_URL()
    {
        CString result;
        InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
        return result;
    }

    void put_URL(LPCTSTR newValue)
    {
        static BYTE parms[] = VTS_BSTR;
        InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    long get_openState()
    {
        long result;
        InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
        return result;
    }

    long get_playState()
    {
        long result;
        InvokeHelper(0xA, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_controls()
    {
        LPDISPATCH result;
        InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_settings()
    {
        LPDISPATCH result;
        InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_currentMedia()
    {
        LPDISPATCH result;
        InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    void put_currentMedia(LPDISPATCH newValue)
    {
        static BYTE parms[] = VTS_DISPATCH;
        InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    LPDISPATCH get_mediaCollection()
    {
        LPDISPATCH result;
        InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_playlistCollection()
    {
        LPDISPATCH result;
        InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    CString get_versionInfo()
    {
        CString result;
        InvokeHelper(0xB, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
        return result;
    }

    void launchURL(LPCTSTR bstrURL)
    {
        static BYTE parms[] = VTS_BSTR;
        InvokeHelper(0xC, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, bstrURL);
    }

    LPDISPATCH get_network()
    {
        LPDISPATCH result;
        InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_currentPlaylist()
    {
        LPDISPATCH result;
        InvokeHelper(0xD, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    void put_currentPlaylist(LPDISPATCH newValue)
    {
        static BYTE parms[] = VTS_DISPATCH;
        InvokeHelper(0xD, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    LPDISPATCH get_cdromCollection()
    {
        LPDISPATCH result;
        InvokeHelper(0xE, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_closedCaption()
    {
        LPDISPATCH result;
        InvokeHelper(0xF, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    BOOL get_isOnline()
    {
        BOOL result;
        InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_Error()
    {
        LPDISPATCH result;
        InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    CString get_status()
    {
        CString result;
        InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_dvd()
    {
        LPDISPATCH result;
        InvokeHelper(0x28, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH newPlaylist(LPCTSTR bstrName, LPCTSTR bstrURL)
    {
        LPDISPATCH  result;
        static BYTE parms[] = VTS_BSTR VTS_BSTR;
        InvokeHelper(0x29, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, bstrName, bstrURL);
        return result;
    }

    LPDISPATCH newMedia(LPCTSTR bstrURL)
    {
        LPDISPATCH  result;
        static BYTE parms[] = VTS_BSTR;
        InvokeHelper(0x2A, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, bstrURL);
        return result;
    }

    BOOL get_enabled()
    {
        BOOL result;
        InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    void put_enabled(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL;
        InvokeHelper(0x13, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    BOOL get_fullScreen()
    {
        BOOL result;
        InvokeHelper(0x15, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    void put_fullScreen(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL;
        InvokeHelper(0x15, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    BOOL get_enableContextMenu()
    {
        BOOL result;
        InvokeHelper(0x16, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    void put_enableContextMenu(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL;
        InvokeHelper(0x16, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    void put_uiMode(LPCTSTR newValue)
    {
        static BYTE parms[] = VTS_BSTR;
        InvokeHelper(0x17, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    CString get_uiMode()
    {
        CString result;
        InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
        return result;
    }

    BOOL get_stretchToFit()
    {
        BOOL result;
        InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    void put_stretchToFit(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL;
        InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    BOOL get_windowlessVideo()
    {
        BOOL result;
        InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    void put_windowlessVideo(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL;
        InvokeHelper(0x19, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms, newValue);
    }

    BOOL get_isRemote()
    {
        BOOL result;
        InvokeHelper(0x1A, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
        return result;
    }

    LPDISPATCH get_playerApplication()
    {
        LPDISPATCH result;
        InvokeHelper(0x1B, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
        return result;
    }

    void openPlayer(LPCTSTR bstrURL)
    {
        static BYTE parms[] = VTS_BSTR;
        InvokeHelper(0x1C, DISPATCH_METHOD, VT_EMPTY, nullptr, parms, bstrURL);
    }
};
