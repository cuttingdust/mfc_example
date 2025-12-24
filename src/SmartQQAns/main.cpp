#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

/// 使用剪贴板进行进程间通信
BOOL SetClipboardText(const std::wstring &text)
{
    if (!::OpenClipboard(NULL))
        return FALSE;

    ::EmptyClipboard();

    /// 分配全局内存
    HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(wchar_t));
    if (!hGlobal)
    {
        ::CloseClipboard();
        return FALSE;
    }

    /// 锁定内存并复制数据
    wchar_t *pGlobal = (wchar_t *)::GlobalLock(hGlobal);
    if (pGlobal)
    {
        wcscpy_s(pGlobal, text.length() + 1, text.c_str());
        ::GlobalUnlock(hGlobal);

        /// 设置剪贴板数据，系统会接管内存
        ::SetClipboardData(CF_UNICODETEXT, hGlobal);
        ::CloseClipboard();

        /// 注意：不要释放 hGlobal，Windows 已接管
        return TRUE;
    }

    ::GlobalFree(hGlobal);
    ::CloseClipboard();
    return FALSE;
}

#define doKeyEvent(key, bUp, bExt)                                                         \
    do                                                                                     \
    {                                                                                      \
        BYTE  vsc  = ::MapVirtualKey(key, MAPVK_VK_TO_VSC);                                \
        DWORD flag = ((bExt) ? KEYEVENTF_EXTENDEDKEY : 0) | ((bUp) ? KEYEVENTF_KEYUP : 0); \
        ::keybd_event(key, vsc, flag, 0);                                                  \
        ::Sleep(10);                                                                       \
    }                                                                                      \
    while (FALSE);

#define doKeyClick(key, bExt)     \
    do                            \
    {                             \
        doKeyEvent(key, 0, bExt); \
        doKeyEvent(key, 1, bExt); \
    }                             \
    while (FALSE);

/// 实现key的组合
DWORD __stdcall SimKeyboardSave(LPVOID lParam)
{
    HWND hWnd = (HWND)lParam;
    ::SetForegroundWindow(hWnd);
    ::SetFocus(hWnd);

    /// ctrl+enter
    doKeyEvent(VK_CONTROL, 0, 1);
    doKeyClick(VK_RETURN, 0);
    doKeyEvent(VK_CONTROL, 1, 1);

    return 0;
}

int main(int argc, char *argv[])
{
    printf("欢迎使用智能QQ聊天系统...\n");
    HWND        hWnd = ::FindWindow(L"TXGuiFoundation", L"刘明星");
    std::string s;
    {
        /// 直接输入
        // std::cout << "请输入要发送的消息: ";
        // std::getline(std::cin, s);
    }

    /// 字典(文件内容)发送
    {
        std::ifstream infile(R"(assert/dictionary.txt)");
        if (!infile)
        {
            std::cerr << "无法打开文件 messages.txt" << std::endl;
            return 1;
        }
        std::string line;
        while (std::getline(infile, line))
        {
            s += line + "\r\n"; // 添加换行符
        }
        infile.close();
    }


    const char *pCStrKey = s.c_str();
    int         pSize    = ::MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t    *pContent = new wchar_t[pSize];
    ::MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pContent, pSize);
    int len = wcslen(pContent);
    if (hWnd != NULL)
    {
        printf("找到QQ好友了,请按任意键继续\n");
        getchar();

        ::SetFocus(hWnd);
        ::SetClipboardText(pContent);
        ::Sleep(200);
        ::SendMessage(hWnd, WM_PASTE, 0, 0);
        /// 发送模拟按键回车
        // ::SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0);

        /// 发送Ctrl + Enter组合键
        HANDLE hThread = ::CreateThread(NULL, 0, SimKeyboardSave, hWnd, 0, NULL);
        ::WaitForSingleObject(hThread, INFINITE);
        ::CloseHandle(hThread);
    }
    else
    {
        printf("未找到指定窗口，请确保QQ已登录并且窗口标题正确。\n");
    }
    return 0;
}
