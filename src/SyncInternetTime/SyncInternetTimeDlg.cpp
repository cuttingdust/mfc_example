
// SyncInternetTimeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SyncInternetTime.h"
#include "SyncInternetTimeDlg.h"
#include "afxdialogex.h"

#include <afxsock.h>
#include <afxdisp.h> // COleDateTime 头文件

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 在类定义外部添加SNTP相关定义
#define NTP_PORT            123
#define NTP_TIMESTAMP_DELTA 2208988800ull // 1900年到1970年的秒数差

// SNTP报文结构（简化版）
#pragma pack(push, 1) // 1字节对齐，确保结构体与网络数据包完全匹配
struct SNTPPacket
{
    BYTE      mode : 3;       // 模式（客户端=3，服务器=4）
    BYTE      version : 3;    // 版本号（NTPv3=3，NTPv4=4）
    BYTE      li : 2;         // 闰秒指示器
    BYTE      stratum;        // 时钟层级（1=一级时钟源）
    BYTE      poll;           // 轮询间隔
    BYTE      precision;      // 精度
    DWORD     rootDelay;      // 根延迟
    DWORD     rootDispersion; // 根离散
    DWORD     refId;          // 参考标识符
    ULONGLONG refTimestamp;   // 参考时间戳
    ULONGLONG origTimestamp;  // 起始时间戳
    ULONGLONG recvTimestamp;  // 接收时间戳
    ULONGLONG transTimestamp; // 传输时间戳（我们要用的）
};
#pragma pack(pop)


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


// CSyncInternetTimeDlg 对话框


CSyncInternetTimeDlg::CSyncInternetTimeDlg(CWnd* pParent /*=nullptr*/) :
    CDialogEx(IDD_SYNCINTERNETTIME_DIALOG, pParent), m_info(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSyncInternetTimeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COM_IP, m_cmbServer);
    DDX_Text(pDX, IDC_EDIT_SHOW_STATE, m_info);
}

BEGIN_MESSAGE_MAP(CSyncInternetTimeDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_SYNC_TIME, &CSyncInternetTimeDlg::OnBnClickedSyncTime)
END_MESSAGE_MAP()


// CSyncInternetTimeDlg 消息处理程序

BOOL CSyncInternetTimeDlg::OnInitDialog()
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

    SetTimer(1001, 1000, NULL);

    // TODO: 在此添加额外的初始化代码
    m_cmbServer.AddString(L"time.windows.com");
    m_cmbServer.AddString(L"time.cloudflare.com");
    m_cmbServer.AddString(L"time-a-g.nist.gov");
    m_cmbServer.SetCurSel(0); // 默认选择第一个

    CString strServer;
    m_cmbServer.GetWindowText(strServer);

    CTime tm(1990, 1, 1, 0, 0, 0);
    GetInternetTime(&tm, strServer);

    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CSyncInternetTimeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSyncInternetTimeDlg::OnPaint()
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
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSyncInternetTimeDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSyncInternetTimeDlg::GetInternetTime(CTime* pTime, CString strServer)
{
    // 清空显示
    m_info.Empty();
    m_info = L"正在连接时间服务器: " + strServer + L"...\r\n";
    UpdateData(FALSE);

    // 初始化MFC Socket
    if (!AfxSocketInit())
    {
        m_info += L"Socket初始化失败！\r\n";
        UpdateData(FALSE);
        return FALSE;
    }

    // 创建UDP Socket - 修正第3个参数
    CSocket socket;
    if (!socket.Create(0, SOCK_DGRAM, NULL)) // 改为NULL或""
    {
        m_info += L"创建Socket失败！\r\n";
        UpdateData(FALSE);
        return FALSE;
    }

    // 设置超时（3秒）- 修正临时变量问题
    DWORD timeout = 3000; // 先定义变量
    socket.SetSockOpt(SO_RCVTIMEO, (const char*)&timeout, sizeof(DWORD));

    // 解析服务器地址
    CStringA serverA(strServer); // 转换为ANSI字符串
    hostent* pHost = gethostbyname(serverA);
    if (!pHost)
    {
        m_info += L"无法解析服务器地址！\r\n";
        UpdateData(FALSE);
        return FALSE;
    }

    // 设置服务器地址
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // 先清零
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(NTP_PORT);
    serverAddr.sin_addr   = *((in_addr*)pHost->h_addr);

    // 准备SNTP请求包
    SNTPPacket request;
    ZeroMemory(&request, sizeof(request));

    // 设置SNTP头：版本4，客户端模式
    // li=0(无警告), version=4(NTPv4), mode=3(客户端)
    request.li      = 0;
    request.version = 4;
    request.mode    = 3; // 客户端模式

    // 发送请求
    if (socket.SendTo(&request, sizeof(request), (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int     error = WSAGetLastError();
        CString strError;
        strError.Format(L"发送请求失败！错误代码: %d\r\n", error);
        m_info += strError;
        UpdateData(FALSE);
        return FALSE;
    }

    m_info += L"已发送请求，等待响应...\r\n";
    UpdateData(FALSE);

    // 接收响应
    SNTPPacket  response;
    SOCKADDR_IN fromAddr;
    int         fromLen = sizeof(fromAddr);

    int bytesReceived = socket.ReceiveFrom(&response, sizeof(response), (SOCKADDR*)&fromAddr, &fromLen, 0);

    if (bytesReceived == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        if (error == WSAETIMEDOUT)
        {
            m_info += L"接收超时！服务器无响应。\r\n";
        }
        else
        {
            CString strError;
            strError.Format(L"接收响应失败！错误代码: %d\r\n", error);
            m_info += strError;
        }
        UpdateData(FALSE);
        return FALSE;
    }

    if (bytesReceived < (int)sizeof(response))
    {
        m_info += L"接收数据不完整！\r\n";
        UpdateData(FALSE);
        return FALSE;
    }

    // 关闭Socket
    socket.Close();

    m_info += L"成功接收时间数据！\r\n";
    UpdateData(FALSE);

    // 转换网络字节序到主机字节序
    // NTP时间戳是大端序（网络字节序）
    ULONGLONG ntpTimestamp = 0;


    // 安全地复制传输时间戳
    memcpy(&ntpTimestamp, &response.transTimestamp, sizeof(ntpTimestamp));

    ntpTimestamp += dwDely;

    // 由于结构体使用了1字节对齐，需要手动处理字节序
    // 将8字节的ntpTimestamp从网络字节序转换为主机字节序
    unsigned char* bytes     = (unsigned char*)&ntpTimestamp;
    ULONGLONG      converted = 0;

    // 手动转换为小端序（如果系统是小端序）
    for (int i = 0; i < 8; i++)
    {
        converted |= ((ULONGLONG)bytes[i]) << (56 - i * 8);
    }

    ntpTimestamp = converted;

    // 提取高32位（整数秒部分）
    DWORD secondsSince1900 = (DWORD)(ntpTimestamp >> 32);

    // 转换为Unix时间戳（从1900年转到1970年）
    time_t unixTime = (time_t)(secondsSince1900 - NTP_TIMESTAMP_DELTA);

    // 显示调试信息
    CString strDebug;
    strDebug.Format(L"NTP时间戳: %u秒 (从1900年算起)\r\n", secondsSince1900);
    m_info += strDebug;

    strDebug.Format(L"Unix时间戳: %lld\r\n", (long long)unixTime);
    m_info += strDebug;

    UpdateData(FALSE);

    // 创建CTime对象
    CTime timeNow(unixTime);

    // 格式化显示
    CString strTime = timeNow.Format(L"%Y年%m月%d日 %H:%M:%S");
    m_info += L"网络时间: " + strTime + L"\r\n";
    UpdateData(FALSE);

    // 返回结果
    if (pTime)
    {
        *pTime = timeNow;
    }

    return TRUE;
}

void CSyncInternetTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    dwDely++;
    if (nIDEvent == 1001)
    {
        /// 获取本地时间
        CTime t1;
        t1 = CTime::GetCurrentTime();
        CString s;
        s = t1.Format(L"本地时间： %H:%M:%S");
        GetDlgItem(IDC_STATIC)->SetWindowText(s);
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CSyncInternetTimeDlg::OnBnClickedSyncTime()
{
    // TODO: 在此添加控件通知处理程序代码

    m_info = L"";
    UpdateData(FALSE);

    dwDely = 0;
    KillTimer(1001);
    SetTimer(1001, 1000, NULL);

    CString strServer;
    m_cmbServer.GetWindowText(strServer);

    /// 获取网络时间
    CTime tm;
    if (GetInternetTime(&tm, strServer))
    { /// 同步系统时间
        SyncSystemClock(tm);
    }
}

BOOL CSyncInternetTimeDlg::SyncSystemClock(CTime tmServer)
{
    OSVERSIONINFO osv       = { 0 }; // 建议初始化结构体
    osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&osv);

    /// 1. 提升进程权限 (仅NT平台，如Win7/8/10/11)
    if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        HANDLE           hToken;
        TOKEN_PRIVILEGES tkp;

        if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            return FALSE; // 失败
        }

        LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount           = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // 关键修正：只有 AdjustTokenPrivileges 失败才应返回 FALSE
        if (!::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0))
        {
            DWORD dwErr = ::GetLastError();
            m_info.Format(L"AdjustTokenPrivileges 调用失败！错误: %d\r\n", dwErr);
            UpdateData(FALSE);
            CloseHandle(hToken);
            return FALSE;
        }
        else
        {
            // 函数调用“成功”，检查最终状态
            DWORD dwLastError = ::GetLastError();
            if (dwLastError == ERROR_SUCCESS)
            {
                m_info += L"权限提升成功！\r\n";
                UpdateData(FALSE);
            }
            else if (dwLastError == ERROR_NOT_ALL_ASSIGNED)
            {
                // 这解释了为什么 SetLocalTime 会返回 1314
                m_info.Format(L"警告：未成功获取系统时间权限 (LastError: %d)。继续尝试...\r\n", dwLastError);
                UpdateData(FALSE);
                // 不立即返回，继续执行，但很可能失败
            }
            else
            {
                // 其他未知错误
                m_info.Format(L"AdjustTokenPrivileges 返回成功但LastError异常: %d\r\n", dwLastError);
                UpdateData(FALSE);
            }
        }
        // 可以检查GetLastError()，但根据MSDN，即使成功也可能返回ERROR_NOT_ALL_ASSIGNED，此处简化处理。
        CloseHandle(hToken);
    }

    /// 2. 正确设置系统时间
    SYSTEMTIME st = { 0 }; // 重要：初始化所有字段
    // 将CTime转换为SYSTEMTIME，确保所有字段都被正确赋值
    st.wYear         = tmServer.GetYear();
    st.wMonth        = tmServer.GetMonth();
    st.wDay          = tmServer.GetDay();
    st.wHour         = tmServer.GetHour();
    st.wMinute       = tmServer.GetMinute();
    st.wSecond       = tmServer.GetSecond();
    st.wMilliseconds = 0; // CTime精度到秒，毫秒置0
    // GetDayOfWeek 返回1-7 (1=周日)， SYSTEMTIME.wDayOfWeek 要求 0-6 (0=周日)
    // st.wDayOfWeek = tmServer.GetDayOfWeek() % 7;

    if (::SetLocalTime(&st))
    {
        m_info += L"系统时间同步成功！\r\n";
        UpdateData(FALSE);
        return TRUE;
    }
    else
    {
        DWORD dwErr = GetLastError();
        m_info.Format(L"系统时间同步失败！错误代码: %d\r\n", dwErr);
        UpdateData(FALSE);
        return FALSE;
    }
}
