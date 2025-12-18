
// CClientDemoDlg.h: 头文件
//

#pragma once

#include "CClientSocket.h"

// CCClientDemoDlg 对话框
class CCClientDemoDlg : public CDialogEx
{
    // 构造
public:
    CCClientDemoDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_CCLIENTDEMO_DIALOG
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
    CClientSocket  m_ClientSocket;
    int            m_sPort;
    CString        m_strWords;
    CIPAddressCtrl m_serverIP;
    CListBox       m_listWords;
    afx_msg void   OnBnClickedBtnConnect();
    afx_msg void   OnBnClickedBtnDisconnect();
    afx_msg void   OnBnClickedBtnSend();
};
