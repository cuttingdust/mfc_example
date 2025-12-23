// CHtmlCodeViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "CHtmlCodeViewDlg.h"
#include "MainFrm.h"

#include "resource.h"

/// 获取窗体句柄对应的CWnd指针
WNDPROC lpEditProc;
HWND    hEditWnd;

// CHtmlCodeViewDlg 对话框

IMPLEMENT_DYNAMIC(CHtmlCodeViewDlg, CDialogEx)

CHtmlCodeViewDlg::CHtmlCodeViewDlg(CWnd* pParent /*=nullptr*/) :
    CDialogEx(IDD_DIALOG_CODEVIEW, pParent), m_htmlCode(_T(""))
{
}

CHtmlCodeViewDlg::~CHtmlCodeViewDlg()
{
}

//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////

/// 多字节 → 宽字符串
CString A2Wstring(const char* string)
{
    if (string == nullptr || *string == '\0')
    {
        return L"";
    }

    int requiredLen = ::MultiByteToWideChar(CP_ACP, 0, string, -1, nullptr, 0);
    if (requiredLen <= 0)
    {
        return L"";
    }

    wchar_t* wbuf   = new wchar_t[requiredLen];
    int      result = ::MultiByteToWideChar(CP_ACP, 0, string, -1, wbuf, requiredLen);

    CString resultStr;
    if (result > 0)
    {
        resultStr = wbuf;
    }

    delete[] wbuf; /// 确保内存释放
    return resultStr;
}

/// 宽字符串 → 多字节
CStringA W2Astring(const wchar_t* wstring)
{
    if (wstring == nullptr || *wstring == L'\0')
    {
        return "";
    }

    int requiredLen = ::WideCharToMultiByte(CP_ACP, 0, wstring, -1, nullptr, 0, nullptr, nullptr);
    if (requiredLen <= 0)
    {
        return "";
    }

    char* buf    = new char[requiredLen];
    int   result = ::WideCharToMultiByte(CP_ACP, 0, wstring, -1, buf, requiredLen, nullptr, nullptr);

    CStringA resultStr;
    if (result > 0)
    {
        resultStr = buf;
    }

    delete[] buf;
    return resultStr;
}

// 辅助函数：检测并转换编码
CString ConvertToUnicode(const CStringA& strHtml)
{
    if (strHtml.IsEmpty())
        return _T("");

    const char* pData   = strHtml;
    int         dataLen = strHtml.GetLength();

    // 检查BOM
    BOOL hasUtf8BOM = FALSE;
    if (dataLen >= 3 && (BYTE)pData[0] == 0xEF && (BYTE)pData[1] == 0xBB && (BYTE)pData[2] == 0xBF)
    {
        hasUtf8BOM = TRUE;
        pData += 3;
        dataLen -= 3;
    }

    // 尝试UTF-8转换
    int wlen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pData, dataLen, NULL, 0);

    if (wlen > 0 || GetLastError() != ERROR_NO_UNICODE_TRANSLATION)
    {
        // 是有效的UTF-8
        wlen = ::MultiByteToWideChar(CP_UTF8, 0, pData, dataLen, NULL, 0);
        if (wlen > 0)
        {
            wchar_t* wbuf = new wchar_t[wlen + 1];
            ::MultiByteToWideChar(CP_UTF8, 0, pData, dataLen, wbuf, wlen);
            wbuf[wlen] = L'\0';

            CString result = wbuf;
            delete[] wbuf;
            return result;
        }
    }

    // 如果不是UTF-8，尝试ANSI
    wlen = ::MultiByteToWideChar(CP_ACP, 0, pData, dataLen, NULL, 0);
    if (wlen > 0)
    {
        wchar_t* wbuf = new wchar_t[wlen + 1];
        ::MultiByteToWideChar(CP_ACP, 0, pData, dataLen, wbuf, wlen);
        wbuf[wlen] = L'\0';

        CString result = wbuf;
        delete[] wbuf;
        return result;
    }

    return _T("无法解码网页内容");
}


//////////////////////////////////////////////////////////////////


BOOL CHtmlCodeViewDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CWaitCursor      wait;
    CInternetSession session(_T("MyExplorerHTMLViewer"), 1, PRE_CONFIG_INTERNET_ACCESS);
    CStdioFile*      pFile = NULL;
    CString          sWebAddress;
    CStringA         strHtml; /// 存储原始字节数据

    /// 设置一下文本框的显示效果，添加代码的行号显示
    CFont font;
    font.CreatePointFont(100, _T("Consolas"));
    m_editHtmlBody.SetFont(&font);

    /// 设置一下edit的编辑区域去，在左边留出40像素显示行号的空间
    m_editHtmlBody.SendMessage(EM_SETMARGINS, EC_RIGHTMARGIN | EC_LEFTMARGIN, 0x000050005 + 40);
    hEditWnd   = m_editHtmlBody.GetSafeHwnd();
    lpEditProc = (WNDPROC)SetWindowLongPtr(hEditWnd, GWLP_WNDPROC, (LONG_PTR)&CHtmlCodeViewDlg::SubEditProc);

    ((CMainFrame*)GetParentFrame())->m_wndDlgBar.GetDlgItem(IDC_EDIT_ADDRESS)->GetWindowText(sWebAddress);

    try
    {
        pFile = session.OpenURL(sWebAddress);
        if (pFile != NULL)
        {
            // 方法1：使用字节读取（避免类型问题）
            CByteArray byteArray;
            BYTE       buffer[4096];
            UINT       bytesRead;

            while ((bytesRead = pFile->Read(buffer, sizeof(buffer))) > 0)
            {
                // 将字节添加到数组
                for (UINT i = 0; i < bytesRead; i++)
                {
                    byteArray.Add(buffer[i]);
                }
            }

            pFile->Close();
            delete pFile;
            pFile = NULL;

            // 转换为CStringA
            if (byteArray.GetSize() > 0)
            {
                strHtml = CStringA((LPCSTR)byteArray.GetData(), byteArray.GetSize());

                // 检测编码并转换
                m_htmlCode = ConvertToUnicode(strHtml);
                m_htmlCode.Replace(_T("\n"), _T("\r\n")); /// 处理换行符
            }
        }
    }
    catch (CInternetException* e)
    {
        e->Delete();
        m_htmlCode = _T("无法加载网页内容");
    }
    catch (...)
    {
        m_htmlCode = _T("发生未知错误");
    }

    if (pFile != NULL)
    {
        pFile->Close();
        delete pFile;
    }

    session.Close();
    UpdateData(FALSE);

    return TRUE;
}

void CHtmlCodeViewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, ID_CODEVIEW, m_htmlCode);
    DDX_Control(pDX, ID_CODEVIEW, m_editHtmlBody);
}


BEGIN_MESSAGE_MAP(CHtmlCodeViewDlg, CDialogEx)
END_MESSAGE_MAP()


// CHtmlCodeViewDlg 消息处理程序

/// 截获edit控件的wm_paint消息，绘制行号

long CHtmlCodeViewDlg::SubEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParams)
{
    PAINTSTRUCT ps;
    if (uMsg == WM_PAINT)
    {
        ::CallWindowProc(lpEditProc, hWnd, uMsg, wParam, lParams);
        ::BeginPaint(hWnd, &ps);
        {
            showLineNumber(hEditWnd);
        }

        return ::EndPaint(hWnd, &ps);
    }


    return CallWindowProc(lpEditProc, hWnd, uMsg, wParam, lParams);
}

/// 显示文本的总行数
void CHtmlCodeViewDlg::showLineNumber(HWND hEdit)
{
    RECT       ClientRect; /// 与edit控件客户区相关
    HDC        hdcEdit;    /// edit的设备环境
    HDC        hdcCpb;     ///与edit控件兼容的内存设备环境
    HBITMAP    hdcBmp;     /// 与edit控件兼容的位图
    int        CharHeight; /// 字符高度
    int        chHeight;   /// 行高
    int        FirstLine;
    int        ClientHeight;
    int        LineCount;
    wchar_t    countBuf[10]; /// 用于存储行号的缓冲区
    CHARFORMAT charFmt;

    /// 获取edit的dc
    hdcEdit = ::GetDC(hEdit);
    ::GetClientRect(hEdit, &ClientRect);
    /// 获取客户区的高度
    ClientHeight = ClientRect.bottom - ClientRect.top;
    /// 创建与edit控件兼容的内存dc
    hdcCpb = ::CreateCompatibleDC(hdcEdit);
    /// 创建一个位图dc, 用于显示行号
    hdcBmp = ::CreateCompatibleBitmap(hdcEdit, 40, ClientHeight);
    /// 将位图dc选入edit环境
    ::SelectObject(hdcCpb, hdcBmp);

    /// 填充显示行号dc的背景色
    ::FillRect(hdcCpb, &ClientRect, ::CreateSolidBrush(0x8080ff));
    ::SetBkMode(hdcCpb, TRANSPARENT);
    /// 获取当前edit中的文本的总行数
    LineCount = ::SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);

    /// 处理字符
    ::ZeroMemory(&charFmt, sizeof(charFmt));
    charFmt.cbSize = sizeof(CHARFORMAT);
    ::SendMessage(hEdit, EM_GETCHARFORMAT, TRUE, (long)&charFmt);
    /// 获取字符高度
    CharHeight = charFmt.yHeight / 15;
    chHeight   = CharHeight; /// 行高
    CharHeight = 2;
    /// 设置显示行号的前景色
    ::SetTextColor(hdcCpb, 0x000000);
    /// 获取文本的第一可见行号
    FirstLine = ::SendMessage(hEdit, EM_GETFIRSTVISIBLELINE, 0, 0);
    FirstLine++;
    while (FirstLine <= LineCount)
    {
        ::TextOut(hdcCpb, 1, CharHeight, countBuf, wsprintf(countBuf, L"%4u", FirstLine++));
        CharHeight += chHeight + 20;
        if (CharHeight > ClientHeight)
            break;
    }

    /// 将内存dc的内容拷贝到edit控件的客户区
    ::BitBlt(hdcEdit, 0, 0, 40, ClientHeight, hdcCpb, 0, 0, SRCCOPY);
    ::DeleteDC(hdcCpb);
    ::ReleaseDC(hEdit, hdcEdit);
    ::DeleteObject(hdcBmp);
}
