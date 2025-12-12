
// FileMgrDlg.h: 头文件
//

#pragma once


// CFileMgrDlg 对话框
class CFileMgrDlg : public CDialogEx
{
    // 构造
public:
    CFileMgrDlg(CWnd* pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_FILEMGR_DIALOG
    };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持
    BOOL PreTranslateMessage(MSG* pMsg) override;

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL    OnInitDialog();
    afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void    OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    DECLARE_MESSAGE_MAP()
public:
    CComboBox m_ctrlDirPath;
    CTreeCtrl m_ctrlDirTree;
    CListCtrl m_ctrlFileList;
    BOOL      m_bHideFile;
    BOOL      m_bSysFile;
    CButton   m_nDelete;

public:
    CBitmapButton m_ctrlOpenTo;
    CBitmapButton m_ctrlCopyTo;
    CBitmapButton m_ctrlMoveTo;
    CBitmapButton m_ctrlDelete;
    CToolTipCtrl  m_ctrlToolTip;
    CImageList    m_ctrlTreeImages;
    CImageList    m_ctrlListImages;

public:
    /// \brief 删除节点下的所有子节点
    /// \param hItems
    /// \return
    BOOL DeleteChildItems(HTREEITEM hItems);

    BOOL PathHasSubDirs(CString strPath);

    BOOL UpdateTreeItem(HTREEITEM hItem);

    CString GetItemPath(HTREEITEM hItem);

    BOOL UpdateItemDirs(HTREEITEM hItem);

    BOOL      m_bSelectFile;
    CString   m_strCurrFile;
    CString   m_strCurrPath;
    HTREEITEM m_hSelectItem; /// 选中的节点

    BOOL UpdateListItem(CString strPath);

    CString      MakeSizeString(ULONGLONG dsSize);
    afx_msg void OnTvnSelchangedDirtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTvnItemexpandingDirtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMClickFilelist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);

    HANDLE       GetNameItem(CString strName);
    afx_msg void OnBnClickedStepto();
    afx_msg void OnCbnSelchangeDirpath();
    HTREEITEM    FindTreeItemByPath(const CString& strPath);
    HTREEITEM    FindTreeItemRecursive(HTREEITEM hParent, const CString& strPath);
    afx_msg void OnBnClickedHidefile();
    afx_msg void OnBnClickedSysfile();
};
