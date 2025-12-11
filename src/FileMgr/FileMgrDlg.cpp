
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
ON_NOTIFY(TVN_SELCHANGED, IDC_DIRTREE, &CFileMgrDlg::OnTvnSelchangedDirtree)
ON_NOTIFY(TVN_ITEMEXPANDING, IDC_DIRTREE, &CFileMgrDlg::OnTvnItemexpandingDirtree)
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

    // m_ctrlOpenTo.LoadBitmaps(IDB_BITMAP1, IDB_BITMAP2);
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
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON1)); /// 0  我的电脑
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON2)); /// 1  文件夹
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON3)); /// 2  光驱
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON4)); /// 3  硬盘
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON5)); /// 4  移动设备
    m_ctrlTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICON6)); /// 5  网络
    m_ctrlDirTree.SetImageList(&m_ctrlTreeImages, TVSIL_NORMAL);

    /// 初始化树形控件根节点
    m_hSelectItem = m_ctrlDirTree.InsertItem(L"桌面", 0, 0, TVI_ROOT);
    UpdateTreeItem(m_hSelectItem);
    m_strCurrPath = GetItemPath(m_hSelectItem);
    m_ctrlDirTree.SelectItem(m_hSelectItem);
    m_ctrlDirTree.Expand(m_hSelectItem, TVE_EXPAND);


    /// 初始化列表控件图标
    m_ctrlListImages.Create(16, 16, ILC_COLOR32, 9, 9);
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON2));  /// 0  文件夹
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON10)); /// 1  文档
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON7));  /// 2  图片
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON9));  /// 3  视频
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON8));  /// 4 音频
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON3));  /// 5 光驱
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON4));  /// 6 硬盘
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON5));  /// 7 移动设备
    m_ctrlListImages.Add(AfxGetApp()->LoadIcon(IDI_ICON6));  /// 8 网络
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

BOOL CFileMgrDlg::UpdateListItem(CString strPath)
{
    int nItem = 0;

    m_ctrlFileList.DeleteAllItems();
    if (strPath.IsEmpty()) /// 为空表示是"我的电脑"的节点
    {
        /// 更新磁盘信息
        wchar_t lpszPath[MAX_PATH] = { 0 };
        ::GetLogicalDriveStrings(MAX_PATH, lpszPath);
        for (wchar_t* ws = lpszPath; *ws; ws += wcslen(ws) + 1)
        {
            if (::GetDriveType(ws) != DRIVE_FIXED)
                continue;

            m_ctrlFileList.InsertItem(nItem, L"", 2);  /// 硬盘图标插入
            m_ctrlFileList.SetItemText(nItem, 0, ws);  /// 卷标
            m_ctrlFileList.SetItemText(nItem, 1, L""); /// 大小
            m_ctrlFileList.SetItemText(nItem, 2, L"本地磁盘");
            nItem++;
        }
    }
    else
    {
        CFileFind finder;
        if (strPath.Right(1) != "\\")
            strPath += "\\";

        for (BOOL bFind = finder.FindFile(strPath + "*.*"); bFind;)
        {
            bFind = finder.FindNextFile();
            if (finder.IsDots())
                continue;
            if (finder.IsHidden() && !m_bHideFile)
                continue;
            if (finder.IsSystem() && !m_bSysFile)
                continue;
            BOOL bDir = finder.IsDirectory();
            m_ctrlFileList.InsertItem(nItem, L"", bDir ? 0 : 1);
            m_ctrlFileList.SetItemText(nItem, 0, finder.GetFileName());
            m_ctrlFileList.SetItemText(nItem, 1, MakeSizeString(finder.GetLength()));
            m_ctrlFileList.SetItemText(nItem, 2, bDir ? L"文件夹" : L"文件");
            CTime tmFile;
            finder.GetLastAccessTime(tmFile);
            m_ctrlFileList.SetItemText(nItem, 3, tmFile.Format(L"%Y年%m月%d日%H:%M"));
            nItem++;
        }
    }

    CString strMsg;
    strMsg.Format(L"共%d对象", nItem);
    SetDlgItemText(IDC_MSGINFO, strMsg);

    return TRUE;
}

CString CFileMgrDlg::MakeSizeString(DWORD dsSize)
{
    CString strSize = L"";
    if (dsSize >= 1024 * 1024)
    {
        strSize.Format(L"%.1fMB", (float)dsSize / 1024 / 1024);
    }
    else if (dsSize > 0)
    {
        strSize.Format(L"%.1fKB", (float)dsSize / 1024);
    }
    return strSize;
}

void CFileMgrDlg::OnTvnSelchangedDirtree(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    UpdateData(TRUE);

    m_hSelectItem = pNMTreeView->itemNew.hItem; /// 当前选中的节点
    m_strCurrPath = GetItemPath(m_hSelectItem);
    /// 更新列表控件
    UpdateListItem(m_strCurrPath);

    if (m_strCurrPath.IsEmpty())
    {
        m_ctrlDirPath.AddString(L"我的电脑");
    }
    else
    {
        m_ctrlDirPath.AddString(m_strCurrPath);
    }

    /// 选中该节点
    m_ctrlDirPath.SetCurSel(m_ctrlDirPath.GetCount() - 1);
    /// 展开该节点
    m_ctrlDirTree.Expand(m_hSelectItem, TVE_EXPAND); /// 展开该选中的节点
}

void CFileMgrDlg::OnTvnItemexpandingDirtree(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    UpdateData(TRUE);
    /// 需要展开的节点
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    UpdateTreeItem(hItem);
}
