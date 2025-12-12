
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
    CDialogEx(IDD_FILEMGR_DIALOG, pParent), m_bHideFile(FALSE), m_bSysFile(FALSE), m_nDelOpt(FALSE)
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
    DDX_Radio(pDX, IDC_DELOPT, m_nDelOpt);
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
ON_BN_CLICKED(IDC_OPENTO, &CFileMgrDlg::OnBnClickedOpento)
ON_BN_CLICKED(IDC_COPYTO, &CFileMgrDlg::OnBnClickedCopyto)
ON_BN_CLICKED(IDC_MOVETO, &CFileMgrDlg::OnBnClickedMoveto)
ON_BN_CLICKED(IDC_DELETE, &CFileMgrDlg::OnBnClickedDelete)
ON_BN_CLICKED(IDC_DELOPT, &CFileMgrDlg::OnBnClickedDelopt)
ON_BN_CLICKED(IDC_RADIO2, &CFileMgrDlg::OnBnClickedRadio2)
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

    UpdateData(FALSE); /// 将变量值更新到界面控

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
            int       nIconIndex = 1; /// 默认文档图标
            ULONGLONG ullSize    = 0; // 大小变量
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
            m_ctrlFileList.SetItemText(nItem, 1, MakeSizeString(bDir ? 0 : finder.GetLength())); /// 计算文件夹太慢
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

ULONGLONG CFileMgrDlg::CalculateFolderSize(const CString& strFolderPath)
{
    ULONGLONG ullTotalSize = 0;
    CFileFind finder;
    CString   strPath = strFolderPath;

    if (strPath.Right(1) != L"\\")
        strPath += L"\\";

    // 查找所有文件和子文件夹
    BOOL bWorking = finder.FindFile(strPath + L"*.*");
    while (bWorking)
    {
        bWorking = finder.FindNextFile();

        // 跳过 "." 和 ".."
        if (finder.IsDots())
            continue;

        CString strFilePath = finder.GetFilePath();

        if (finder.IsDirectory())
        {
            // 递归计算子文件夹大小
            ullTotalSize += CalculateFolderSize(strFilePath);
        }
        else
        {
            // 累加文件大小
            ullTotalSize += finder.GetLength();
        }
    }
    finder.Close();

    return ullTotalSize;
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

void CFileMgrDlg::OnBnClickedOpento()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_strCurrFile.IsEmpty())
    {
        return;
    }

    ::ShellExecute(NULL, L"open", m_strCurrFile, L"", L"", SW_SHOWNORMAL);
}

BOOL CFileMgrDlg::FileOperation(UINT nOpt, CString strDest, BOOL bUndo)
{
    // 为源路径分配足够空间（MAX_PATH + 2个额外空字符）
    wchar_t lpszFromPath[MAX_PATH + 2] = { 0 };

    // ✅ 关键修复1：确保路径是完整路径
    wchar_t szFullPath[MAX_PATH] = { 0 };
    if (::GetFullPathName(m_strCurrFile, MAX_PATH, szFullPath, NULL) > 0)
    {
        ::lstrcpy(lpszFromPath, szFullPath);
        TRACE(_T("【FileOperation】完整路径: %s\n"), szFullPath);
    }
    else
    {
        ::lstrcpy(lpszFromPath, m_strCurrFile);
        TRACE(_T("【FileOperation】使用原路径: %s\n"), m_strCurrFile);
    }

    // ✅ 关键修复2：确保双空字符结尾
    int nLen = ::lstrlen(lpszFromPath);
    if (nLen > 0 && nLen < MAX_PATH)
    {
        lpszFromPath[nLen + 1] = L'\0'; // 第二个空字符
    }
    else
    {
        TRACE(_T("【FileOperation】错误：路径长度异常 nLen=%d\n"), nLen);
        return FALSE;
    }

    // 处理目标路径（如果是删除操作，strDest可能为空）
    wchar_t lpszToPath[MAX_PATH + 2] = { 0 };
    if (!strDest.IsEmpty())
    {
        // 确保目标路径也是完整路径
        if (::GetFullPathName(strDest, MAX_PATH, szFullPath, NULL) > 0)
        {
            ::lstrcpy(lpszToPath, szFullPath);
        }
        else
        {
            ::lstrcpy(lpszToPath, strDest);
        }

        // 确保目标路径双空字符结尾
        nLen = ::lstrlen(lpszToPath);
        if (nLen > 0 && nLen < MAX_PATH)
        {
            lpszToPath[nLen + 1] = L'\0';
        }
    }

    SHFILEOPSTRUCT fo;
    ::ZeroMemory(&fo, sizeof(fo));
    fo.hwnd  = m_hWnd;
    fo.wFunc = nOpt;
    fo.pFrom = lpszFromPath; // 源路径（已确保双空字符结尾）
    fo.pTo   = lpszToPath;   // 目标路径（已确保双空字符结尾）

    // ✅ 关键修复3：根据操作类型设置正确的标志
    fo.fFlags = FOF_SIMPLEPROGRESS | FOF_NOCONFIRMATION | FOF_NOERRORUI;

    // 根据删除模式设置是否允许撤销（到回收站）
    if (bUndo && nOpt == FO_DELETE)
    {
        fo.fFlags |= FOF_ALLOWUNDO;
        TRACE(_T("【FileOperation】设置 FOF_ALLOWUNDO 标志（到回收站）\n"));
    }
    else if (!bUndo && nOpt == FO_DELETE)
    {
        // 彻底删除时不设置 FOF_ALLOWUNDO
        fo.fFlags |= FOF_NOCONFIRMATION;
        TRACE(_T("【FileOperation】彻底删除，不设置 FOF_ALLOWUNDO\n"));
    }

    // 进度条标题
    if (!strDest.IsEmpty())
        fo.lpszProgressTitle = strDest;

    // 执行操作
    TRACE(_T("【FileOperation】调用 SHFileOperation，操作类型: %d\n"), nOpt);
    int nResult = ::SHFileOperation(&fo);

    // 检查结果
    if (nResult != 0)
    {
        TRACE(_T("【FileOperation】SHFileOperation 失败，错误代码: %d\n"), nResult);

        // 尝试解释错误代码
        switch (nResult)
        {
            case 2:
                TRACE(_T("  错误：文件未找到\n"));
                break;
            case 3:
                TRACE(_T("  错误：路径未找到\n"));
                break;
            case 5:
                TRACE(_T("  错误：访问被拒绝\n"));
                break;
            case 32:
                TRACE(_T("  错误：文件正在被使用\n"));
                break;
            default:
                TRACE(_T("  未知错误\n"));
                break;
        }

        return FALSE;
    }

    // 检查是否用户取消
    if (fo.fAnyOperationsAborted)
    {
        TRACE(_T("【FileOperation】用户取消了操作\n"));
        return FALSE;
    }

    TRACE(_T("【FileOperation】操作成功完成\n"));
    return TRUE; // 成功
}

void CFileMgrDlg::OnBnClickedCopyto()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_strCurrFile.IsEmpty())
        return;

    int nLen = m_strCurrFile.GetLength();
    int nPos = m_strCurrFile.ReverseFind(L'\\');

    CFileDialog dlg(FALSE, NULL, m_strCurrFile.Right(nLen - nPos - 1));
    if (dlg.DoModal() != IDOK)
        return;
    FileOperation(FO_COPY, dlg.GetPathName(), TRUE);
}

void CFileMgrDlg::OnBnClickedMoveto()
{
    // TODO: 在此添加控件通知处理程序代码
    // TODO: 在此添加控件通知处理程序代码
    if (m_strCurrFile.IsEmpty())
        return;

    int nLen = m_strCurrFile.GetLength();
    int nPos = m_strCurrFile.ReverseFind(L'\\');

    CFileDialog dlg(FALSE, NULL, m_strCurrFile.Right(nLen - nPos - 1));
    if (dlg.DoModal() != IDOK)
        return;

    // 保存移动前的当前路径，用于后续刷新
    CString strCurrentPathBeforeMove = m_strCurrPath;

    // 执行移动操作
    if (FileOperation(FO_MOVE, dlg.GetPathName(), TRUE))
    {
        // ✅ 移动成功，刷新界面
        // 1. 刷新文件列表 (显示当前目录的最新内容)
        UpdateListItem(strCurrentPathBeforeMove);

        // 2. 刷新树形控件对应节点的子项
        UpdateTreeItem(m_hSelectItem);

        // 3. 清空当前选中的文件，避免对已移动文件误操作
        m_strCurrFile.Empty();

        TRACE(_T("【OnBnClickedMoveto】文件已移动，界面已刷新\n"));
    }
}

void CFileMgrDlg::OnBnClickedDelete()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);
    if (m_strCurrFile.IsEmpty())
        return;

    // 根据删除模式准备不同的提示
    CString strPrompt, strTitle;
    if (m_nDelOpt == 0) // 到回收站
    {
        strPrompt.Format(L"是否将文件 \"%s\" 移动到回收站？", m_strCurrFile);
        strTitle = L"移动到回收站";
    }
    else // 彻底删除
    {
        strPrompt.Format(L"是否彻底删除文件 \"%s\"？\n此操作不可恢复！", m_strCurrFile);
        strTitle = L"彻底删除警告";
    }

    // 显示确认对话框
    if (MessageBox(strPrompt, strTitle, MB_YESNO | (m_nDelOpt == 0 ? MB_ICONQUESTION : MB_ICONWARNING)) != IDYES)
    {
        return; // 用户取消
    }

    // 执行删除操作
    if (FileOperation(FO_DELETE, L"", m_nDelOpt == 0))
    {
        // ✅ 删除成功后刷新界面
        UpdateListItem(m_strCurrPath);
        UpdateTreeItem(m_hSelectItem);

        // ✅ 清空当前选中的文件
        m_strCurrFile.Empty();

        // 显示操作完成提示
        CString strMsg;
        if (m_nDelOpt == 0)
            strMsg.Format(L"文件已移动到回收站");
        else
            strMsg.Format(L"文件已彻底删除");
        SetDlgItemText(IDC_MSGINFO, strMsg);

        TRACE(_T("【OnBnClickedDelete】文件已删除，界面已刷新\n"));
    }
    else
    {
        MessageBox(L"删除操作失败，请检查文件是否被占用或权限不足。", L"操作失败", MB_ICONERROR);
    }
}

void CFileMgrDlg::OnBnClickedDelopt()
{
    // TODO: 在此添加控件通知处理程序代码
    // UpdateData(TRUE); // 更新数据到变量
    TRACE(_T("选择：到回收站，m_nDelOpt=%d\n"), m_nDelOpt);
    /// 此时 m_nDelOpt 应该为 0（到回收站）
    CString strMsg;
    strMsg.Format(L"删除模式：移动到回收站");
    SetDlgItemText(IDC_MSGINFO, strMsg);
}

void CFileMgrDlg::OnBnClickedRadio2()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE); // 更新数据到变量

    // 此时 m_nDelOpt 应该为 1（彻底删除）
    CString strMsg;
    strMsg.Format(L"删除模式：彻底删除");
    SetDlgItemText(IDC_MSGINFO, strMsg);
}
