
// MyPlayerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MyPlayer.h"
#include "MyPlayerDlg.h"
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


// CMyPlayerDlg 对话框


CMyPlayerDlg::CMyPlayerDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_MYPLAYER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyPlayerDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CMyPlayerDlg 消息处理程序

BOOL CMyPlayerDlg::OnInitDialog()
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

    // TODO: 在此添加额外的初始化代码
    CRect rect;
    GetClientRect(&rect);
    CRgn r1, r2;
    r1.CreateEllipticRgn(0, 0, rect.Width(), rect.Height());
    r2.CreateEllipticRgn(0, 0, rect.Width() / 2, rect.Height() / 2);
    m_rgn.CreateEllipticRgn(0, 0, rect.Width(), rect.Height());
    m_rgn.CombineRgn(&r1, &r2, RGN_DIFF);
    ::SetWindowRgn(GetSafeHwnd(), m_rgn, TRUE);

    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMyPlayerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int   cxIcon = GetSystemMetrics(SM_CXICON);
        int   cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CRect rect;     /// 声明一个矩形对象，用于存储窗口客户区的大小和位置
        GetClientRect(  /// 获取当前窗口的客户区矩形（不包括标题栏、边框等非客户区）
                &rect); /// 结果存储在rect中，rect.left=0, rect.top=0, rect.right=宽度, rect.bottom=高度
        CDC  memDC;     /// 创建内存设备上下文（Memory DC），用于在内存中绘制位图
        CDC* pDC;       /// 声明一个指向设备上下文的指针，初始化为NULL

        pDC = GetDC( /// 获取当前窗口的设备上下文（DC），用于实际的屏幕绘制
        );           ///  窗口设备上下文代表窗口的绘图表面

        CBitmap  bitmap;     ///  创建位图对象，用于加载和存储位图数据
        CBitmap* pOldBitmap; /// 声明指向旧位图的指针，用于保存memDC中原有的位图对象（以便后续恢复）
        BITMAP   hBitmap;    /// 声明BITMAP结构体，用于存储位图的详细信息（宽度、高度、颜色深度等）

        bitmap.LoadBitmap(    /// 从资源中加载位图，IDB_BITMAP1是资源ID
                IDB_BITMAP1); /// 位图资源通常在资源文件(.rc)中定义

        memDC.CreateCompatibleDC( /// 创建一个与屏幕设备上下文(pDC)兼容的内存设备上下文
                pDC);             /// 这意味着memDC具有与屏幕DC相同的颜色格式和分辨率

        /// 将位图对象选入内存设备上下文中，并保存原有的位图对象
        /// 这是必要的，因为每个DC只能有一个位图被选中用于绘制
        pOldBitmap = memDC.SelectObject(&bitmap);

        /// 获取位图的详细信息，填充到hBitmap结构体中
        /// 包括：宽度(bmWidth)、高度(bmHeight)、颜色深度(bmBitsPixel)等
        bitmap.GetBitmap(&hBitmap);

        /// 创建CSize对象，存储位图的原始尺寸（宽度和高度）
        /// cx = hBitmap.bmWidth, cy = hBitmap.bmHeight
        CSize size(hBitmap.bmWidth, hBitmap.bmHeight);

        /// 将内存DC中的位图拉伸绘制到窗口DC中：
        /// 参数说明：
        /// 1. 目标区域：(0, 0) 开始，宽度=rect.Width()，高度=rect.Height()
        /// 2. 源设备上下文：&memDC（包含位图的内存DC）
        /// 3. 源区域：从(0, 0)开始，宽度=size.cx，高度=size.cy
        /// 4. 光栅操作：SRCCOPY（直接将源复制到目标）
        /// 效果：将位图拉伸/压缩以适应整个客户区
        pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, size.cx, size.cy, SRCCOPY);

        /// 将原有的位图对象选回内存设备上下文
        /// 这是重要的清理步骤，避免资源泄漏和错误
        memDC.SelectObject(pOldBitmap);

        /// 释放获取的设备上下文
        /// Windows要求每次GetDC()后必须调用ReleaseDC()，否则会导致资源泄漏
        ReleaseDC(pDC);

        // ============== 内存泄漏风险提示 ==============
        // 注意：这里存在潜在的内存泄漏问题！
        // bitmap对象和memDC对象没有被显式删除。
        // 在MFC中，当这些对象超出作用域时，析构函数会自动调用，
        // 但如果有异常发生，可能导致资源未正确释放。

        // ============== 建议的改进代码 ==============
        /*
// 使用RAII风格的改进版本
CRect rect;
GetClientRect(&rect);

CClientDC dc(this);  // 自动管理DC生命周期，无需ReleaseDC

CDC memDC;
if (!memDC.CreateCompatibleDC(&dc))
{
    AfxMessageBox(L"创建内存DC失败");
    return;
}

CBitmap bitmap;
if (!bitmap.LoadBitmap(IDB_BITMAP1))
{
    AfxMessageBox(L"加载位图失败");
    return;
}

CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

BITMAP bmpInfo;
bitmap.GetBitmap(&bmpInfo);

// 高质量拉伸绘制选项
dc.SetStretchBltMode(HALFTONE);  // 使用高质量拉伸模式
dc.SetBrushOrg(0, 0);

// 拉伸绘制位图
dc.StretchBlt(0, 0, rect.Width(), rect.Height(),
              &memDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
              SRCCOPY);

// 恢复原有位图
if (pOldBitmap)
    memDC.SelectObject(pOldBitmap);
    
// 注意：memDC和bitmap对象会在作用域结束时自动析构
// CClientDC也会自动释放DC，无需手动调用ReleaseDC
*/

        // ============== 代码功能总结 ==============
        // 这段代码的功能：将IDB_BITMAP1位图拉伸/压缩绘制到窗口的整个客户区
        // 应用场景：窗口背景图、全屏位图显示、位图平铺等

        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyPlayerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

/// 常见于自定义界面的播放器、聊天窗口等
LRESULT CMyPlayerDlg::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDialogEx::OnNcHitTest(point);

    /// 无边框窗口：让客户区任何位置都可拖拽
    if (hit == HTCLIENT)
        return HTCAPTION;

    return hit;
}
