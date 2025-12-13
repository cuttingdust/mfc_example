
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
    virtual BOOL    PreCreateWindow(CREATESTRUCT& cs);
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnDestroy();
    afx_msg void    OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
    afx_msg void    OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL    OnEraseBkgnd(CDC* pDC);

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
private:
    // 添加以下成员变量
    static BOOL m_bHotkeyRegistered; // 热键是否注册
    static UINT m_nHotkeyId;         // 热键ID

    // 添加静态成员函数（用于热键回调）
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK            m_hKeyboardHook; // 键盘钩子
    static HWND             m_hWndInstance;  // ✅ 保存窗口句柄
private:
    NOTIFYICONDATA m_nid; // 托盘图标数据

    void CreateTrayIcon(); // 创建托盘图标
    void RemoveTrayIcon(); // 移除托盘图标
protected:
    afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam); // 托盘消息
    afx_msg void    OnTrayCapture();                          // 托盘菜单：截图
    afx_msg void    OnTraySettings();                         // 托盘菜单：设置
    afx_msg void    OnTrayAbout();                            // 托盘菜单：关于
    afx_msg void    OnTrayExit();                             // 托盘菜单：退出

    DECLARE_MESSAGE_MAP()
};
