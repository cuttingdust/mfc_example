
// FileMgrDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileMgr.h"
#include "FileMgrDlg.h"
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


// CFileMgrDlg 对话框


CFileMgrDlg::CFileMgrDlg(CWnd* pParent /*=nullptr*/) :
    CDialogEx(IDD_FILEMGR_DIALOG, pParent), m_bHideFile(FALSE), m_bSysFile(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileMgrDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DIRPATH, m_ctrlDirPath);
    DDX_Control(pDX, IDC_DIRTREE, m_ctrlDirTree);
    DDX_Control(pDX, IDC_FILELIST, m_ctrlFileList);
    DDX_Check(pDX, IDC_HIDEFILE, m_bHideFile);
    DDX_Check(pDX, IDC_SYSFILE, m_bSysFile);
    DDX_Control(pDX, IDC_DELOPT, m_nDelete);
}

BOOL CFileMgrDlg::PreTranslateMessage(MSG* pMsg)
{
    m_ctrlToolTip.RelayEvent(pMsg);
    return CDialogEx::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CFileMgrDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CFileMgrDlg 消息处理程序

BOOL CFileMgrDlg::OnInitDialog()
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

    // TODO: 在此添加额外的初始化代码
    m_ctrlOpenTo.SubclassDlgItem(IDC_OPENTO, this);
    m_ctrlCopyTo.SubclassDlgItem(IDC_COPYTO, this);
    m_ctrlMoveTo.SubclassDlgItem(IDC_MOVETO, this);
    m_ctrlDelete.SubclassDlgItem(IDC_DELETE, this);

    // m_ctrlOpenTo.LoadBitmaps(IDB_BITMAP9, IDB_BITMAP10);
    // m_ctrlDelete.LoadBitmaps(IDB_BITMAP7, IDB_BITMAP8);
    // m_ctrlCopyTo.LoadBitmaps(IDB_BITMAP3, IDB_BITMAP4);
    // m_ctrlMoveTo.LoadBitmaps(IDB_BITMAP5, IDB_BITMAP6);

    /// 初始化工具提示
    m_ctrlToolTip.Create(this);
    m_ctrlToolTip.Activate(TRUE);
    m_ctrlToolTip.SetDelayTime(300);
    m_ctrlToolTip.AddTool(GetDlgItem(IDC_COPYTO), L"复制文件到指定位置");
    m_ctrlToolTip.AddTool(GetDlgItem(IDC_MOVETO), L"移动文件到指定位置");
    m_ctrlToolTip.AddTool(GetDlgItem(IDC_OPENTO), L"在外部打开文件");
    m_ctrlToolTip.AddTool(GetDlgItem(IDC_DELETE), L"删除选中文件");

    /// 初始化树形控件图标
    m_ctrlTreeImages.Create(16, 16, ILC_COLOR32, 9, 9);
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG1)); /// 0  我的电脑
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG2)); /// 1  文件夹
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG3)); /// 2  光驱
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG4)); /// 3  硬盘
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG5)); /// 4  移动设备
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDB_PNG6)); /// 5  网络
    m_ctrlDirTree.SetImageList(&m_ctrlTreeImages, TVSIL_NORMAL);

    /// 初始化树形控件根节点
    m_hSelectItem = m_ctrlDirTree.InsertItem(L"桌面", 0, 0, TVI_ROOT);
    UpdateTreeItem(m_hSelectItem);
    m_strCurrPath = GetItemPath(m_hSelectItem);
    m_ctrlDirTree.SelectItem(m_hSelectItem);
    m_ctrlDirTree.Expand(m_hSelectItem, TVE_EXPAND);


    /// 初始化列表控件图标
    m_ctrlListImages.Create(16, 16, ILC_COLOR32, 9, 9);
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG2));  /// 0  文件夹
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG7));  /// 1  文档
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG8));  /// 2  图片
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG9));  /// 3  视频
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG10)); /// 4 音频
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG3));  /// 5 光驱
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG4));  /// 6 硬盘
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG5));  /// 7 移动设备
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDB_PNG6));  /// 8 网络
    m_ctrlFileList.SetImageList(&m_ctrlListImages, LVSIL_SMALL);

    m_ctrlFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT); /// 设置列表控件为整行选中模式
    m_ctrlFileList.InsertColumn(0, L"名称", LVCFMT_LEFT, 200);
    m_ctrlFileList.InsertColumn(1, L"大小", LVCFMT_RIGHT, 60);
    m_ctrlFileList.InsertColumn(2, L"类型", LVCFMT_CENTER, 60);
    m_ctrlFileList.InsertColumn(3, L"修改日期", LVCFMT_RIGHT, 60);

    return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileMgrDlg::OnPaint()
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
HCURSOR CFileMgrDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


BOOL CFileMgrDlg::DeleteChildItems(HTREEITEM hItems)
{
    HTREEITEM hChild;
    hChild = m_ctrlDirTree.GetChildItem(hItems);
    for (; hChild; hChild = m_ctrlDirTree.GetNextItem(hItems, TVGN_CHILD))
    {
        m_ctrlDirTree.DeleteItem(hChild);
    }

    return TRUE;
}

BOOL CFileMgrDlg::PathHasSubDirs(CString strPath)
{
    CFileFind finder;
    if (strPath.Right(1) != "\\")
    {
        strPath += "\\";
    }

    for (BOOL bFind = finder.FindFile(strPath + "*.*"); bFind;)
    {
        bFind = finder.FindNextFile();
        if (finder.IsDots())
        {
            continue;
        }

        if (!finder.IsDirectory())
        {
            continue;
        }
        else
        {
            return finder.Close(), TRUE;
        }
    }

    return finder.Close(), FALSE;
}

BOOL CFileMgrDlg::UpdateTreeItem(HTREEITEM hItem)
{
    wchar_t lpszPath[MAX_PATH] = { 0 };
    CString strItem, strPath;

    DeleteChildItems(hItem); /// 删除所有节点 /// 以遍加载


    strItem = m_ctrlDirTree.GetItemText(hItem);
    if (!strItem.CompareNoCase(L"桌面"))
    {
        UpdateItemDirs(m_ctrlDirTree.InsertItem(L"我的文档", 5, 5, hItem));
        UpdateTreeItem(m_ctrlDirTree.InsertItem(L"我的电脑", 4, 4, hItem));
        return UpdateItemDirs(hItem);
    }

    if (!strItem.CompareNoCase(L"我的电脑"))
    {
        ::GetLogicalDriveStrings(MAX_PATH, lpszPath);
        for (wchar_t* s = lpszPath; *s; s += wcslen(s) + 1)
        {
            if (::GetDriveType(s) != DRIVE_FIXED)
            {
                continue;
            }

            HTREEITEM hTemp = m_ctrlDirTree.InsertItem(s, 3, 3, hItem);
            if (PathHasSubDirs(s))
            {
                m_ctrlDirTree.InsertItem(L"", hTemp);
            }
        }
        return TRUE;
    }


    return UpdateItemDirs(hItem);
}

CString CFileMgrDlg::GetItemPath(HTREEITEM hItem)
{
    CString   strDir, strPath;
    HTREEITEM hTemp = hItem;
    while (hTemp != NULL)
    {
        strDir = m_ctrlDirTree.GetItemText(hTemp);
        hTemp  = m_ctrlDirTree.GetParentItem(hTemp);
        if (!strDir.CompareNoCase(L"我的电脑"))
        {
            break;
        }

        CString strTemp = strDir;
        if (strDir.Right(1) != "\\" && strDir.IsEmpty())
        {
            strTemp += "\\";
        }

        strPath = strTemp + strPath;
        if (!strDir.CompareNoCase(L"桌面"))
        {
            break;
        }

        if (!strDir.CompareNoCase(L"我的文档"))
        {
            break;
        }
    }

    wchar_t lpszPath[MAX_PATH] = { 0 };
    ::SHGetSpecialFolderPath(m_hWnd, lpszPath, 0, FALSE);
    strPath.Replace(L"桌面", lpszPath);

    ::SHGetSpecialFolderPath(m_hWnd, lpszPath, 5, FALSE);
    strPath.Replace(L"我的文档", lpszPath);

    return strPath;
}

BOOL CFileMgrDlg::UpdateItemDirs(HTREEITEM hItem)
{
    CFileFind finder;
    CString   strTemp = GetItemPath(hItem);
    if (strTemp.Right(1) != "\\")
    {
        strTemp += "\\";
    }

    for (BOOL bFind = finder.FindFile(strTemp + "*.*"); bFind;)
    {
        bFind = finder.FindNextFile();
        if (finder.IsDots() || !finder.IsDirectory())
        {
            continue;
        }
        if (finder.IsHidden() && !m_bHideFile)
        {
            continue;
        }

        if (finder.IsSystem() && !m_bSysFile)
        {
            continue;
        }

        strTemp            = finder.GetFileName();
        HTREEITEM hSubItem = m_ctrlDirTree.InsertItem(strTemp, 1, 2, hItem);
        if (PathHasSubDirs(finder.GetFilePath()))
        {
            m_ctrlDirTree.InsertItem(L"", hSubItem);
        }
    }
    finder.Close();

    return TRUE;
}
