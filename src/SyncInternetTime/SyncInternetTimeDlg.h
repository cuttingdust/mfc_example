
// SyncInternetTimeDlg.h: 头文件
//

#pragma once


// CSyncInternetTimeDlg 对话框
class CSyncInternetTimeDlg : public CDialogEx
{
    // 构造
public:
    CSyncInternetTimeDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_SYNCINTERNETTIME_DIALOG
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持


    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL    OnInitDialog();
    afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CComboBox    m_cmbServer;
    BOOL         GetInternetTime(CTime* pTime, CString strServer);
    CString      m_info;
    DWORD        dwDely = 0;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedSyncTime();
    BOOL         SyncSystemClock(CTime tmServer);
};
