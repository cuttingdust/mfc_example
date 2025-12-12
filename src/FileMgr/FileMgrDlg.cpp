
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
ON_NOTIFY(NM_CLICK, IDC_FILELIST, &CFileMgrDlg::OnNMClickFilelist)
ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, &CFileMgrDlg::OnNMDblclkFilelist)
ON_BN_CLICKED(IDC_STEPTO, &CFileMgrDlg::OnBnClickedStepto)
ON_CBN_SELCHANGE(IDC_DIRPATH, &CFileMgrDlg::OnCbnSelchangeDirpath)
ON_BN_CLICKED(IDC_HIDEFILE, &CFileMgrDlg::OnBnClickedHidefile)
ON_BN_CLICKED(IDC_SYSFILE, &CFileMgrDlg::OnBnClickedSysfile)
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
    m_ctrlFileList.InsertColumn(0, L"名称", LVCFMT_LEFT, 150);
    m_ctrlFileList.InsertColumn(1, L"大小", LVCFMT_LEFT, 200);
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
    TRACE(_T("【GetItemPath】传入节点句柄: %p\n"), hItem);

    CString strFullPath;
    CString strDir;

    HTREEITEM hCurrent = hItem;

    while (hCurrent != NULL)
    {
        strDir = m_ctrlDirTree.GetItemText(hCurrent);
        TRACE(_T("  当前节点文本: %s (句柄: %p)\n"), strDir, hCurrent);

        // 遇到“我的电脑”表示到达驱动器根目录，停止向上拼接
        if (strDir.CompareNoCase(L"我的电脑") == 0)
        {
            // 如果是“我的电脑”节点本身，返回空字符串（特殊标识）
            if (hCurrent == hItem)
            {
                return L"";
            }
            TRACE(_T("  是父节点，停止拼接\n"));
            // 否则，说明当前是驱动器（如D:），停止循环
            break;
        }

        // 遇到“桌面”或“我的文档”，用系统路径替换
        if (strDir.CompareNoCase(L"桌面") == 0)
        {
            wchar_t szPath[MAX_PATH] = { 0 };
            if (::SHGetSpecialFolderPath(m_hWnd, szPath, CSIDL_DESKTOP, FALSE))
            {
                if (!strFullPath.IsEmpty())
                {
                    // 确保路径分隔符
                    if (szPath[wcslen(szPath) - 1] != L'\\')
                        wcscat_s(szPath, L"\\");
                    strFullPath = szPath + strFullPath;
                }
                else
                {
                    strFullPath = szPath;
                }
            }
            break;
        }
        else if (strDir.CompareNoCase(L"我的文档") == 0)
        {
            wchar_t szPath[MAX_PATH] = { 0 };
            if (::SHGetSpecialFolderPath(m_hWnd, szPath, CSIDL_PERSONAL, FALSE))
            {
                if (!strFullPath.IsEmpty())
                {
                    if (szPath[wcslen(szPath) - 1] != L'\\')
                        wcscat_s(szPath, L"\\");
                    strFullPath = szPath + strFullPath;
                }
                else
                {
                    strFullPath = szPath;
                }
            }
            break;
        }

        // 对于普通文件夹或驱动器，正常拼接
        CString strNodePath = strDir;

        // 确保节点路径以反斜杠结尾（驱动器和普通文件夹处理方式不同）
        if (hCurrent == hItem) // 当前选中的节点
        {
            // 如果是驱动器（如"D:"），需要加反斜杠
            if (strNodePath.GetLength() == 2 && strNodePath[1] == L':')
            {
                strNodePath += L"\\";
            }
            // 如果是普通文件夹，暂时不加，等拼接时处理
        }
        else // 父节点
        {
            // 父节点如果不是以反斜杠结尾，则添加
            if (strNodePath.Right(1) != L"\\")
            {
                strNodePath += L"\\";
            }
        }

        // 正向拼接：父节点在前，子节点在后
        strFullPath = strNodePath + strFullPath;

        // 向上移动到父节点
        hCurrent = m_ctrlDirTree.GetParentItem(hCurrent);
    }

    // 最终处理：确保路径末尾有反斜杠（除非是空路径或已经是根目录）
    if (!strFullPath.IsEmpty())
    {
        if (strFullPath.Right(1) != L"\\")
        {
            strFullPath += L"\\";
        }
    }

    return strFullPath;
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
        HTREEITEM hSubItem = m_ctrlDirTree.InsertItem(strTemp, 1, 1, hItem);
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
    TRACE(_T("【UpdateListItem】路径：%s\n"), strPath);

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

            m_ctrlFileList.InsertItem(nItem, L"", 6);  /// 硬盘图标插入
            m_ctrlFileList.SetItemText(nItem, 0, ws);  /// 卷标
            m_ctrlFileList.SetItemText(nItem, 1, L""); /// 大小
            m_ctrlFileList.SetItemText(nItem, 2, L"本地磁盘");

            /// 获取磁盘空间信息
            ULARGE_INTEGER uliTotalBytes, uliFreeBytes, uliTotalFreeBytes;
            if (::GetDiskFreeSpaceEx(ws,
                                     &uliFreeBytes,       /// 可用空间
                                     &uliTotalBytes,      /// 总空间
                                     &uliTotalFreeBytes)) /// 调用者可用的空间
            {
                /// 格式化为易读的字符串（如 "465.7 GB"）
                CString strSize;
                strSize.Format(L"总: %s / 可用: %s", MakeSizeString(uliTotalBytes.QuadPart),
                               MakeSizeString(uliFreeBytes.QuadPart));
                m_ctrlFileList.SetItemText(nItem, 1, strSize); /// 设置大小信息
            }
            else
            {
                m_ctrlFileList.SetItemText(nItem, 1, L"N/A"); /// 获取失败
            }

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

            BOOL    bDir        = finder.IsDirectory();
            CString strFileName = finder.GetFileName();
            CString strExt      = finder.GetFileName().Mid(strFileName.ReverseFind('.') + 1).MakeLower();

            /// 动态确定图标索引
            int nIconIndex = 1; /// 默认文档图标

            if (bDir)
            {
                nIconIndex = 0; /// 文件夹图标
            }
            else
            {
                /// 可以根据扩展名分配更具体的图标
                if (strExt == L"jpg" || strExt == L"png" || strExt == L"bmp" || strExt == L"gif")
                    nIconIndex = 2; /// 图片图标
                else if (strExt == L"mp4" || strExt == L"avi" || strExt == L"mkv" || strExt == L"mov")
                    nIconIndex = 3; /// 视频图标
                else if (strExt == L"mp3" || strExt == L"wav" || strExt == L"flac")
                    nIconIndex = 4; /// 音频图标
                else
                    nIconIndex = 1; /// 文档图标
            }

            m_ctrlFileList.InsertItem(nItem, L"", nIconIndex); /// 使用动态图标索引
            m_ctrlFileList.SetItemText(nItem, 0, strFileName);
            m_ctrlFileList.SetItemText(nItem, 1, MakeSizeString(bDir ? 0 : finder.GetLength()));
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

CString CFileMgrDlg::MakeSizeString(ULONGLONG ullSize) // 改为64位参数
{
    CString strSize = L"";
    if (ullSize >= 1024ULL * 1024 * 1024 * 1024) // 1 TB
    {
        strSize.Format(L"%.2f TB", (double)ullSize / (1024ULL * 1024 * 1024 * 1024));
    }
    else if (ullSize >= 1024ULL * 1024 * 1024) /// 1 GB
    {
        strSize.Format(L"%.2f GB", (double)ullSize / (1024ULL * 1024 * 1024));
    }
    else if (ullSize >= 1024ULL * 1024) /// 1 MB
    {
        strSize.Format(L"%.1f MB", (double)ullSize / (1024ULL * 1024));
    }
    else if (ullSize >= 1024) /// 1 KB
    {
        strSize.Format(L"%.1f KB", (double)ullSize / 1024);
    }
    else if (ullSize > 0)
    {
        strSize.Format(L"%llu B", ullSize);
    }
    else
    {
        strSize = L"0 B";
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

void CFileMgrDlg::OnNMClickFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    if (pNMItemActivate->iItem == -1)
        return;

    int nItem = pNMItemActivate->iItem; /// 选中的列表项

    CString strName = m_ctrlFileList.GetItemText(nItem, 0); /// 选中项的名称
    if (!m_strCurrPath.IsEmpty() && m_strCurrPath.Right(1) != "\\")
        m_strCurrPath += "\\";

    /// 拿到全路径
    m_strCurrFile = m_strCurrPath + strName;
    m_bSelectFile = GetNameItem(strName) ? FALSE : TRUE;
    if (strName.IsEmpty())
        strName = "就绪";

    SetDlgItemText(IDC_MSGINFO, strName);
}

void CFileMgrDlg::OnNMDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    if (pNMItemActivate->iItem == -1)
        return;

    int     nItem          = pNMItemActivate->iItem;
    CString strClickedName = m_ctrlFileList.GetItemText(nItem, 0);

    HTREEITEM hFoundItem = (HTREEITEM)GetNameItem(strClickedName);

    if (hFoundItem != NULL)
    {
        /// 找到了对应的树节点
        m_hSelectItem = hFoundItem;
        m_ctrlDirTree.SelectItem(hFoundItem);
        m_ctrlDirTree.Expand(hFoundItem, TVE_EXPAND);
        /// 更新列表显示这个新路径的内容
        UpdateListItem(GetItemPath(m_hSelectItem));
    }
    else
    {
        /// 没找到对应的树节点（可能是普通文件）
        /// 可以考虑用 ShellExecute 直接打开这个文件
        // CString strFullPath = m_strCurrPath + strClickedName;
        // ShellExecute(NULL, L"open", strFullPath, NULL, NULL, SW_SHOW);
    }
}

HANDLE CFileMgrDlg::GetNameItem(CString strName)
{
    /// 直接获取当前节点的第一个子项开始遍历
    HTREEITEM hItem = m_ctrlDirTree.GetChildItem(m_hSelectItem);

    while (hItem != NULL)
    {
        /// 比较节点文本是否与目标名称一致（使用不区分大小写的比较）
        if (m_ctrlDirTree.GetItemText(hItem).CompareNoCase(strName) == 0)
        {
            return hItem; /// 找到，返回句柄
        }
        /// 没找到，继续检查下一个兄弟节点
        hItem = m_ctrlDirTree.GetNextItem(hItem, TVGN_NEXT);
    }

    /// 遍历完所有子项都没找到
    return NULL;
}

void CFileMgrDlg::OnBnClickedStepto()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateTreeItem(m_hSelectItem);
    UpdateListItem(m_strCurrPath);
}

void CFileMgrDlg::OnCbnSelchangeDirpath()
{
    // TODO: 在此添加控件通知处理程序代码
    int i = m_ctrlDirPath.GetCurSel();
    m_ctrlDirPath.GetLBText(i, m_strCurrPath);

    // 添加调试输出
    TRACE(_T("【OnCbnSelchangeDirpath】选择的路径: %s\n"), m_strCurrPath);

    // 获取新的路径对应的树节点
    HTREEITEM hItemToSelect = NULL;

    if (m_strCurrPath.CompareNoCase(L"我的电脑") == 0)
    {
        m_strCurrPath = L""; // 设置为空表示“我的电脑”视图

        // 关键修复：找到“我的电脑”树节点
        HTREEITEM hDesktop = m_ctrlDirTree.GetChildItem(TVI_ROOT); // 获取桌面节点

        TRACE(_T("  桌面节点句柄: %p\n"), hDesktop);

        if (hDesktop)
        {
            // 遍历桌面的所有子节点，找到“我的电脑”
            HTREEITEM hChild      = m_ctrlDirTree.GetChildItem(hDesktop);
            int       nChildIndex = 0;

            while (hChild)
            {
                CString strChildText = m_ctrlDirTree.GetItemText(hChild);
                TRACE(_T("  桌面子节点[%d]: %s (句柄: %p)\n"), nChildIndex++, strChildText, hChild);

                if (strChildText.CompareNoCase(L"我的电脑") == 0)
                {
                    hItemToSelect = hChild;
                    TRACE(_T("  √ 找到'我的电脑'节点\n"));
                    break;
                }
                hChild = m_ctrlDirTree.GetNextItem(hChild, TVGN_NEXT);
            }

            if (!hItemToSelect)
            {
                TRACE(_T("  × 未找到'我的电脑'节点\n"));
            }
        }
        else
        {
            TRACE(_T("  × 未找到桌面节点\n"));
        }
    }
    else if (!m_strCurrPath.IsEmpty())
    {
        // 对于普通路径，需要根据路径找到对应的树节点
        hItemToSelect = FindTreeItemByPath(m_strCurrPath);
        TRACE(_T("  查找路径对应节点: %s -> %p\n"), m_strCurrPath, hItemToSelect);
    }

    // 如果找到了对应的树节点，则选中它
    if (hItemToSelect)
    {
        m_hSelectItem = hItemToSelect;
        m_ctrlDirTree.SelectItem(hItemToSelect);
        m_ctrlDirTree.Expand(hItemToSelect, TVE_EXPAND);
        TRACE(_T("  已选中树节点: %p\n"), hItemToSelect);
    }
    else
    {
        TRACE(_T("  × 未找到对应树节点，保持当前选中项\n"));
    }

    // 更新列表显示
    UpdateListItem(m_strCurrPath);
}

HTREEITEM CFileMgrDlg::FindTreeItemByPath(const CString& strPath)
{
    // 从根节点开始递归查找
    return FindTreeItemRecursive(TVI_ROOT, strPath);
}

HTREEITEM CFileMgrDlg::FindTreeItemRecursive(HTREEITEM hParent, const CString& strPath)
{
    HTREEITEM hChild = m_ctrlDirTree.GetChildItem(hParent);
    while (hChild)
    {
        CString strItemPath = GetItemPath(hChild);
        if (strItemPath.CompareNoCase(strPath) == 0)
        {
            return hChild; // 找到匹配的节点
        }

        // 递归查找子节点
        HTREEITEM hFound = FindTreeItemRecursive(hChild, strPath);
        if (hFound)
            return hFound;

        hChild = m_ctrlDirTree.GetNextItem(hChild, TVGN_NEXT);
    }
    return NULL; // 未找到
}

void CFileMgrDlg::OnBnClickedHidefile()
{
    // TODO: 在此添加控件通知处理程序代码
    m_bHideFile = !m_bHideFile;
    UpdateTreeItem(m_hSelectItem);
    m_ctrlDirTree.SelectItem(m_hSelectItem);
    m_ctrlDirTree.Expand(m_hSelectItem, TVE_EXPAND);
    UpdateListItem(GetItemPath(m_hSelectItem));
}

void CFileMgrDlg::OnBnClickedSysfile()
{
    // TODO: 在此添加控件通知处理程序代码
    m_bSysFile = !m_bSysFile;
    UpdateTreeItem(m_hSelectItem);
    m_ctrlDirTree.SelectItem(m_hSelectItem);
    m_ctrlDirTree.Expand(m_hSelectItem, TVE_EXPAND);
    UpdateListItem(GetItemPath(m_hSelectItem));
}
