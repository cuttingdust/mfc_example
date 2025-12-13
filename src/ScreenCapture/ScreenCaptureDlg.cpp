
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


// 在文件开头添加以下静态成员初始化
BOOL  CScreenCaptureDlg::m_bHotkeyRegistered = FALSE;
UINT  CScreenCaptureDlg::m_nHotkeyId         = 0;
HHOOK CScreenCaptureDlg::m_hKeyboardHook     = NULL;
HWND  CScreenCaptureDlg::m_hWndInstance      = NULL;
// 在类实现中添加键盘钩子函数
LRESULT CALLBACK CScreenCaptureDlg::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT* pKbStruct = (KBDLLHOOKSTRUCT*)lParam;

        // 检测 F1 键按下
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            if (pKbStruct->vkCode == VK_F1)
            {
                // ✅ 修复：发送到特定的窗口句柄，而不是 NULL
                if (m_hWndInstance != NULL)
                {
                    ::PostMessage(m_hWndInstance, WM_HOTKEY, pKbStruct->vkCode, MAKELPARAM(0, pKbStruct->vkCode));
                }
                return 1; // 阻止默认处理
            }
        }
    }
    return CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);
}

void CScreenCaptureDlg::CreateTrayIcon()
{
    ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
    m_nid.cbSize           = sizeof(NOTIFYICONDATA);
    m_nid.hWnd             = GetSafeHwnd();
    m_nid.uID              = 1;
    m_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_USER + 100; // 自定义消息
    m_nid.hIcon            = m_hIcon;
    wcscpy_s(m_nid.szTip, L"屏幕截图工具\n快捷键: F1\nESC: 取消");

    ::Shell_NotifyIcon(NIM_ADD, &m_nid);
}

void CScreenCaptureDlg::RemoveTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nid);
    ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
}

// 处理托盘消息
LRESULT CScreenCaptureDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
    UINT uMouseMsg = (UINT)lParam;

    switch (uMouseMsg)
    {
        case WM_RBUTTONUP:
            {
                // 显示右键菜单
                CMenu menu;
                menu.LoadMenu(IDR_TRAY_MENU);
                CMenu* pSubMenu = menu.GetSubMenu(0);

                CPoint point;
                GetCursorPos(&point);

                SetForegroundWindow();
                pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

                // 必须发送这个消息，否则菜单不会消失
                PostMessage(WM_NULL, 0, 0);
            }
            break;

        case WM_LBUTTONDBLCLK:
            // 双击显示窗口
            ShowWindow(SW_SHOW);
            SetForegroundWindow();
            break;

        case WM_LBUTTONUP:
            // 单击开始截图（直接全屏截图）
            ShowWindow(SW_SHOW);
            SetForegroundWindow();
            // 或者直接开始截图流程
            // StartCapture();
            break;
    }

    return 0;
}

// 托盘菜单命令处理函数
void CScreenCaptureDlg::OnTrayCapture()
{
    ShowWindow(SW_SHOW);
    SetForegroundWindow();
}

void CScreenCaptureDlg::OnTraySettings()
{
    // 这里可以打开设置对话框
    MessageBox(L"设置功能暂未实现", L"提示", MB_OK | MB_ICONINFORMATION);
}

void CScreenCaptureDlg::OnTrayAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CScreenCaptureDlg::OnTrayExit()
{
    // 移除托盘图标
    RemoveTrayIcon();

    // 退出程序
    CDialogEx::OnOK();
}

// 修改构造函数
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
ON_WM_DESTROY()
ON_WM_HOTKEY()
ON_WM_KEYDOWN()
ON_WM_CREATE()                        // ✅ 添加这一行
ON_MESSAGE(WM_USER + 100, OnTrayIcon) // 托盘消息

// ✅ 修改这里的命令ID
ON_COMMAND(ID_TRAY_CAPTURE, OnTrayCapture)
ON_COMMAND(ID_TRAY_SETTINGS, OnTraySettings)
ON_COMMAND(ID_TRAY_ABOUT, OnTrayAbout)
ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
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

    // ✅ 2. 注册热键
    if (!m_bHotkeyRegistered)
    {
        if (RegisterHotKey(GetSafeHwnd(), 112, MOD_NOREPEAT, VK_F1))
        {
            m_bHotkeyRegistered = TRUE;
            m_nHotkeyId         = 112;
        }

        // 设置键盘钩子
        m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
    }

    // ✅ 3. 创建托盘图标
    CreateTrayIcon();

    m_hWndInstance = GetSafeHwnd();

    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CScreenCaptureDlg::PreCreateWindow(CREATESTRUCT& cs)
{
    // 在窗口创建前就设置为不可见
    cs.style &= ~WS_VISIBLE;
    return CDialogEx::PreCreateWindow(cs);
}

int CScreenCaptureDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // 确保窗口创建后就是隐藏的
    ShowWindow(SW_HIDE);
    return 0;
}

// 添加销毁处理
void CScreenCaptureDlg::OnDestroy()
{
    // 注销热键
    if (m_bHotkeyRegistered && m_nHotkeyId != 0)
    {
        UnregisterHotKey(GetSafeHwnd(), m_nHotkeyId);
        m_bHotkeyRegistered = FALSE;
        m_nHotkeyId         = 0;
    }

    // 移除键盘钩子
    if (m_hKeyboardHook)
    {
        UnhookWindowsHookEx(m_hKeyboardHook);
        m_hKeyboardHook = NULL;
    }

    RemoveTrayIcon();
    CDialogEx::OnDestroy();
}

// 处理热键消息
void CScreenCaptureDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
    if (nHotKeyId == m_nHotkeyId)
    {
        // 显示窗口（如果已隐藏）
        if (!IsWindowVisible())
        {
            ShowWindow(SW_SHOW);
            SetForegroundWindow();
            SetActiveWindow();
        }
    }

    CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

// 处理键盘按下事件（支持ESC取消）
void CScreenCaptureDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // ESC键取消截图
    if (nChar == VK_ESCAPE)
    {
        OnCancel();
    }
    else if (nChar == VK_RETURN && !m_rcSelection.IsRectEmpty())
    {
        CaptureSelection();
    }

    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
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
        // ... 图标绘制代码 ...
    }
    else
    {
        CPaintDC dc(this);
        CRect    rect;
        GetClientRect(&rect);

        // 创建内存DC
        CDC      memDC;
        CBitmap  memBitmap;
        CBitmap* pOldBitmap = NULL;

        if (memDC.CreateCompatibleDC(&dc))
        {
            if (memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
            {
                pOldBitmap = memDC.SelectObject(&memBitmap);

                // 先绘制到内存DC - 黑色半透明背景
                CBrush brushBackground(RGB(0, 0, 0));
                memDC.FillRect(&rect, &brushBackground);
                brushBackground.DeleteObject(); // ✅ 及时删除

                /// 如果正在截图，绘制选区
                if (m_bMouseDown && !m_rcSelection.IsRectNull())
                {
                    CRect rcNormalized = m_rcSelection;
                    rcNormalized.NormalizeRect();

                    // 获取屏幕内容
                    HDC hScreenDC = ::GetDC(NULL);
                    if (hScreenDC)
                    {
                        BitBlt(memDC, rcNormalized.left, rcNormalized.top, rcNormalized.Width(), rcNormalized.Height(),
                               hScreenDC, rcNormalized.left, rcNormalized.top, SRCCOPY);
                        ::ReleaseDC(NULL, hScreenDC); // ✅ 及时释放
                    }

                    /// 绘制选区边框（白色）
                    {
                        CPen  penWhite(PS_SOLID, 2, RGB(255, 255, 255));
                        CPen* pOldPen = memDC.SelectObject(&penWhite);
                        memDC.SelectStockObject(NULL_BRUSH);

                        memDC.Rectangle(&rcNormalized);

                        memDC.SelectObject(pOldPen);
                        // penWhite 会在作用域结束时自动调用 DeleteObject
                    } // ✅ penWhite 在这里自动销毁

                    /// 绘制选区尺寸信息
                    {
                        CString strSize;
                        strSize.Format(L"%d × %d", rcNormalized.Width(), rcNormalized.Height());

                        CFont font;
                        if (font.CreatePointFont(90, L"Arial"))
                        {
                            CFont* pOldFont = memDC.SelectObject(&font);

                            CSize  textSize = memDC.GetTextExtent(strSize);
                            CPoint textPos(rcNormalized.right - textSize.cx - 5, rcNormalized.bottom - textSize.cy - 5);

                            /// 绘制文字背景
                            CRect textRect(textPos.x, textPos.y, textPos.x + textSize.cx + 4,
                                           textPos.y + textSize.cy + 4);
                            memDC.FillSolidRect(&textRect, RGB(0, 0, 0));

                            /// 绘制文字
                            memDC.SetTextColor(RGB(255, 255, 255));
                            memDC.SetBkColor(RGB(0, 0, 0));
                            memDC.TextOut(textPos.x + 2, textPos.y + 2, strSize);

                            memDC.SelectObject(pOldFont);
                            // font 会在作用域结束时自动调用 DeleteObject
                        }
                    } // ✅ font 在这里自动销毁
                }

                // 将内存DC内容复制到屏幕DC
                dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

                // 恢复并清理
                if (pOldBitmap)
                    memDC.SelectObject(pOldBitmap);
            }
            // memBitmap 会在作用域结束时自动删除
        }
        // memDC 会在作用域结束时自动删除
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

    BOOL bClipboardOwnsBitmap = FALSE;

    // 保存到剪贴板
    if (OpenClipboard())
    {
        EmptyClipboard();
        // SetClipboardData 成功后，系统取得位图所有权
        if (SetClipboardData(CF_BITMAP, hBitmap))
        {
            bClipboardOwnsBitmap = TRUE; // 标记：剪贴板现在拥有位图
            hBitmap              = NULL; // ✅ 重要：置空，防止我们重复删除
        }
        else
        {
            // SetClipboardData 失败
            bClipboardOwnsBitmap = FALSE;
        }

        CloseClipboard();
    }
    else
    {
        // OpenClipboard 失败
        bClipboardOwnsBitmap = FALSE;
    }

    // 清理资源
    SelectObject(hMemDC, hOldBitmap);

    // ✅ 修复：只删除没有被剪贴板接管的位图
    if (hBitmap != NULL && !bClipboardOwnsBitmap)
    {
        DeleteObject(hBitmap);
    }

    DeleteDC(hMemDC);
    ::ReleaseDC(NULL, hScreenDC);

    // 显示成功提示
    CString strMessage;
    strMessage.Format(L"截图成功！\n尺寸: %d × %d 像素\n已复制到剪贴板", rcScreen.Width(), rcScreen.Height());
    MessageBox(strMessage, L"截图完成", MB_OK | MB_ICONINFORMATION);

    // ✅ 修改2：播放提示音让用户知道截图完成
    // MessageBeep(MB_ICONINFORMATION);

    // ✅ 修改3：隐藏窗口，等待下次F1
    ShowWindow(SW_HIDE);

    m_rcSelection.SetRectEmpty();
    // Invalidate(FALSE);
    // UpdateWindow();
}
