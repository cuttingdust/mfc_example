
// ScreenCaptureDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ScreenCapture.h"
#include "ScreenCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_ABOUTBOX
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持

    // 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CScreenCaptureDlg 对话框


CScreenCaptureDlg::CScreenCaptureDlg(CWnd* pParent /*=nullptr*/) :
    CDialogEx(IDD_SCREENCAPTURE_DIALOG, pParent), m_bCapturing(FALSE), m_bMouseDown(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CScreenCaptureDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CScreenCaptureDlg 消息处理程序

BOOL CScreenCaptureDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL    bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);  // 设置大图标
    SetIcon(m_hIcon, FALSE); // 设置小图标

    /// 设置为全屏无边框窗口
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    /// 移除标题栏和边框
    long lStyle = ::GetWindowLongPtr(GetSafeHwnd(), GWL_STYLE);
    lStyle &= ~WS_CAPTION;
    lStyle &= ~WS_BORDER;
    lStyle &= ~WS_SYSMENU;
    lStyle &= ~WS_THICKFRAME;
    ::SetWindowLongPtr(GetSafeHwnd(), GWL_STYLE, lStyle);

    int iWidth  = ::GetSystemMetrics(SM_CXSCREEN);
    int iHeight = ::GetSystemMetrics(SM_CYSCREEN);

    ::SetWindowPos(GetSafeHwnd(), wndTopMost, 0, 0, iWidth, iHeight,
                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOOWNERZORDER);


    /// 第二步： 透明效果
    ModifyStyleEx(0, WS_EX_LAYERED);
    SetLayeredWindowAttributes(0,  /// 窗口的句柄
                               90, /// 需要透明的颜色
                               LWA_ALPHA);
    GetLayeredWindowAttributes(&m_transparentColor, &m_originalAlpha, &m_transparentFlags);

    /// 初始化矩形
    m_rcSelection.SetRectEmpty();
    m_ptStart = CPoint(0, 0);
    m_ptEnd   = CPoint(0, 0);


    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CScreenCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CScreenCaptureDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int   cxIcon = GetSystemMetrics(SM_CXICON);
        int   cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        // ============ 双缓冲绘制开始 ============
        CPaintDC dc(this);
        CRect    rect;
        GetClientRect(&rect);

        // 创建内存DC
        CDC      memDC;
        CBitmap  memBitmap;
        CBitmap* pOldBitmap = NULL;

        memDC.CreateCompatibleDC(&dc);
        memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
        pOldBitmap = memDC.SelectObject(&memBitmap);

        // 先绘制到内存DC - 黑色半透明背景
        CBrush brushBackground(RGB(0, 0, 0));
        memDC.FillRect(&rect, &brushBackground);

        /// 如果正在截图，绘制选区
        if (m_bMouseDown && !m_rcSelection.IsRectNull())
        {
            CRect rcNormalized = m_rcSelection;
            rcNormalized.NormalizeRect();

            // ✅ 关键修改：在选中区域显示原始屏幕内容
            HDC hScreenDC = ::GetDC(NULL);

            // 将选中区域的屏幕内容复制到内存DC
            BitBlt(memDC, rcNormalized.left, rcNormalized.top, rcNormalized.Width(), rcNormalized.Height(), hScreenDC,
                   rcNormalized.left, rcNormalized.top, SRCCOPY);

            ::ReleaseDC(NULL, hScreenDC);

            /// 绘制选区边框（白色）
            CPen  penWhite(PS_SOLID, 2, RGB(255, 255, 255));
            CPen* pOldPen = memDC.SelectObject(&penWhite);
            memDC.SelectStockObject(NULL_BRUSH); /// 透明填充

            memDC.Rectangle(&rcNormalized);
            memDC.SelectObject(pOldPen);
            penWhite.DeleteObject();

            /// 绘制选区尺寸信息
            CString strSize;
            strSize.Format(L"%d × %d", rcNormalized.Width(), rcNormalized.Height());

            CFont font;
            font.CreatePointFont(90, L"Arial");
            CFont* pOldFont = memDC.SelectObject(&font);

            /// 在选区右下角显示尺寸
            CSize  textSize = memDC.GetTextExtent(strSize);
            CPoint textPos(rcNormalized.right - textSize.cx - 5, rcNormalized.bottom - textSize.cy - 5);

            /// 绘制文字背景
            CRect textRect(textPos.x, textPos.y, textPos.x + textSize.cx + 4, textPos.y + textSize.cy + 4);
            memDC.FillSolidRect(&textRect, RGB(0, 0, 0));

            /// 绘制文字
            memDC.SetTextColor(RGB(255, 255, 255));
            memDC.SetBkColor(RGB(0, 0, 0));
            memDC.TextOut(textPos.x + 2, textPos.y + 2, strSize);

            memDC.SelectObject(pOldFont);
            font.DeleteObject();
        }

        // 将内存DC内容复制到屏幕DC
        dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

        // 清理
        memDC.SelectObject(pOldBitmap);
        memBitmap.DeleteObject();
        memDC.DeleteDC();
        brushBackground.DeleteObject();
        // ============ 双缓冲绘制结束 ============
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CScreenCaptureDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

BOOL CScreenCaptureDlg::OnEraseBkgnd(CDC* pDC)
{
    // 防止背景闪烁
    return TRUE;
}

void CScreenCaptureDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CDialogEx::OnLButtonDown(nFlags, point);

    /// 开始截图
    m_bMouseDown = TRUE;
    m_bCapturing = TRUE;

    /// 记录起点
    m_ptStart = point;
    m_ptEnd   = point;
    m_rcSelection.SetRect(point.x, point.y, point.x, point.y);

    /// 捕获鼠标，确保在窗口外也能接收消息
    SetCapture();

    /// 更新显示
    Invalidate(FALSE);
}

void CScreenCaptureDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bMouseDown)
    {
        // 更新终点
        m_ptEnd = point;

        // 更新选区矩形
        m_rcSelection.SetRect(m_ptStart.x, m_ptStart.y, m_ptEnd.x, m_ptEnd.y);

        // 强制重绘
        Invalidate(FALSE);
        UpdateWindow();
    }

    CDialogEx::OnMouseMove(nFlags, point);
}

void CScreenCaptureDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CDialogEx::OnLButtonUp(nFlags, point);

    if (m_bMouseDown)
    {
        // 释放鼠标捕获
        ReleaseCapture();

        // 确保选区矩形正确
        m_ptEnd = point;
        m_rcSelection.SetRect(m_ptStart.x, m_ptStart.y, m_ptEnd.x, m_ptEnd.y);
        m_rcSelection.NormalizeRect();

        // 检查选区大小，如果太小则视为取消
        if (m_rcSelection.Width() < 10 || m_rcSelection.Height() < 10)
        {
            // 选区太小，取消截图
            OnCancel();
        }
        else
        {
            // 执行截图
            CaptureSelection();
        }

        // 完成截图
        m_bCapturing = FALSE;
        m_bMouseDown = FALSE;
    }
}

void CScreenCaptureDlg::OnCancel()
{
    // 取消截图，直接退出程序
    CDialogEx::OnCancel();
}

void CScreenCaptureDlg::CaptureSelection()
{
    // 将客户端坐标转换为屏幕坐标
    CRect rcScreen = m_rcSelection;
    ClientToScreen(&rcScreen);
    rcScreen.NormalizeRect();

    // 获取屏幕DC
    HDC     hScreenDC  = ::GetDC(NULL);
    HDC     hMemDC     = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap    = CreateCompatibleBitmap(hScreenDC, rcScreen.Width(), rcScreen.Height());
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // 复制屏幕区域到位图
    BitBlt(hMemDC, 0, 0, rcScreen.Width(), rcScreen.Height(), hScreenDC, rcScreen.left, rcScreen.top, SRCCOPY);

    // 保存到剪贴板
    if (OpenClipboard())
    {
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();
    }
    else
    {
        DeleteObject(hBitmap);
    }

    // 清理资源
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ::ReleaseDC(NULL, hScreenDC);

    // 显示成功提示
    CString strMessage;
    strMessage.Format(L"截图成功！\n尺寸: %d × %d 像素\n已复制到剪贴板", rcScreen.Width(), rcScreen.Height());
    MessageBox(strMessage, L"截图完成", MB_OK | MB_ICONINFORMATION);

    m_rcSelection.SetRectEmpty();
    Invalidate(FALSE);
    UpdateWindow();
}
