
// MyPlayerDlg.h: 头文件
//

#pragma once
#include "CWMPPlayer4.h"
#include "COCX1.h"


// CMyPlayerDlg 对话框
class CMyPlayerDlg : public CDialogEx
{
    // 构造
public:
    CMyPlayerDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_MYPLAYER_DIALOG
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
    CRgn            m_rgn;
    COCX1           m_mp;
    BOOL            m_FirstDraw = FALSE;
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void    OnBnClickedBtnOpenAudio();
};
