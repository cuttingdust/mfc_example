
// CClientDemoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CClientDemo.h"
#include "CClientDemoDlg.h"
#include "afxdialogex.h"

#include <vector>

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


// CCClientDemoDlg 对话框


CCClientDemoDlg::CCClientDemoDlg(CWnd* pParent /*=nullptr*/) :
    CDialogEx(IDD_CCLIENTDEMO_DIALOG, pParent), m_sPort(0), m_strWords(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCClientDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_PORT_EDIT, m_sPort);
    DDX_Text(pDX, IDC_EDIT_WORS, m_strWords);
    DDX_Control(pDX, IDC_LINK_IPADDRESS, m_serverIP);
    DDX_Control(pDX, IDC_MSG_INFO, m_listWords);
}

BEGIN_MESSAGE_MAP(CCClientDemoDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BTN_CONNECT, &CCClientDemoDlg::OnBnClickedBtnConnect)
ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CCClientDemoDlg::OnBnClickedBtnDisconnect)
ON_BN_CLICKED(IDC_BTN_SEND, &CCClientDemoDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CCClientDemoDlg 消息处理程序

BOOL CCClientDemoDlg::OnInitDialog()
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

    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CCClientDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCClientDemoDlg::OnPaint()
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
HCURSOR CCClientDemoDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CCClientDemoDlg::OnBnClickedBtnConnect()
{
    // TODO: 在此添加控件通知处理程序代码
    /// 连接服务器
    BYTE nFild[4];
    UpdateData();
    m_serverIP.GetAddress(nFild[0], nFild[1], nFild[2], nFild[3]);
    CString sip;
    sip.Format(L"%d.%d.%d.%d", nFild[0], nFild[1], nFild[2], nFild[3]);
    m_ClientSocket.Create();
    m_ClientSocket.Connect(sip, m_sPort);
}

void CCClientDemoDlg::OnBnClickedBtnDisconnect()
{
    // TODO: 在此添加控件通知处理程序代码
    m_ClientSocket.Close();
    m_listWords.AddString(L"从服务器断开");
}

void CCClientDemoDlg::OnBnClickedBtnSend()
{
    /// 向服务器发送信息
    UpdateData();
    CString& strToSend = m_strWords;

    /// 1. 将CString转换为传输用的字节流 (Unicode)
    const BYTE* pData      = (const BYTE*)(LPCTSTR)strToSend;
    int         nDataBytes = strToSend.GetLength() * sizeof(TCHAR); /// 计算字节数

    /// 2. 准备一个包含“长度”和“数据”的完整包
    int               nTotalBytes = sizeof(int) + nDataBytes; /// 包头(int) + 数据
    std::vector<BYTE> sendBuffer(nTotalBytes);

    /// 3. 写入长度信息 (网络字节序， 解决跨平台大小端问题)
    int nDataSizeNetworkByteOrder = htonl(nDataBytes);
    memcpy(sendBuffer.data(), &nDataSizeNetworkByteOrder, sizeof(int));

    /// 4. 写入字符串数据
    memcpy(sendBuffer.data() + sizeof(int), pData, nDataBytes);

    /// 5. 发送完整数据包
    m_ClientSocket.Send(sendBuffer.data(), nTotalBytes);

    /// 6. 更新UI
    m_listWords.AddString(L"发送:" + m_strWords);
    m_listWords.SetTopIndex(m_listWords.GetCount() - 1);
}
