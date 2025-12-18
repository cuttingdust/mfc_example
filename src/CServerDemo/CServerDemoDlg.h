
// CServerDemoDlg.h: 头文件
//

#pragma once

#include "CServer.h"
#include "CListenSocket.h"

// CCServerDemoDlg 对话框
class CCServerDemoDlg : public CDialogEx
{
    // 构造
public:
    CCServerDemoDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_CSERVERDEMO_DIALOG
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
    CServer        m_serverSocket; /// 和客户端通信的socket
    CListenSocket  m_ListenSocket; /// 用来监听与客户端连接的socket
    afx_msg void   OnBnClickedBtnListen();
    CIPAddressCtrl m_serverIP;
    int            m_sPort;
    CListBox       m_listWords;
    afx_msg void   OnBnClickedBtnCloseListen();
};
