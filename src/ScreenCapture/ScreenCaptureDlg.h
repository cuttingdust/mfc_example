
// ScreenCaptureDlg.h: 头文件
//

#pragma once


// CScreenCaptureDlg 对话框
class CScreenCaptureDlg : public CDialogEx
{
    // 构造
public:
    CScreenCaptureDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_SCREENCAPTURE_DIALOG
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
    afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
    DECLARE_MESSAGE_MAP()

private:
    BOOL m_bCapture   = FALSE; /// 标记每次的鼠标抬起和释放， 意味着是否捕获
    BOOL m_bCapturing = FALSE;
    BOOL m_bMouseDown = FALSE;

public:
    POINT        m_ptStart;
    POINT        m_ptEnd;
    CRect        m_rcSelection;
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnCancel();
    void         CaptureSelection();

    BYTE     m_originalAlpha;    /// 保存原始透明度值
    COLORREF m_transparentColor; /// 透明色
    DWORD    m_transparentFlags; /// 透明标志
};
