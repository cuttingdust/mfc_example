#pragma once
#include "afxdialogex.h"
#include "afxinet.h" /// 使用WinInet库需要包含此头文件

// CHtmlCodeViewDlg 对话框

class CHtmlCodeViewDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CHtmlCodeViewDlg)

public:
    CHtmlCodeViewDlg(CWnd* pParent = nullptr); // 标准构造函数
    virtual ~CHtmlCodeViewDlg();

public:
    BOOL OnInitDialog() override;

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_DIALOG_CODEVIEW
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支持

    afx_msg void OnBnClickedBtnGetPicture();

    DECLARE_MESSAGE_MAP()
public:
    CString m_htmlCode;
    CEdit   m_editHtmlBody;

    static long SubEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParams);

    static void showLineNumber(HWND hEdit);
};
