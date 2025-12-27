#include <iostream>
#include <Windows.h>

// 全局变量
HINSTANCE g_hInstance = nullptr;
HHOOK     g_hHook     = nullptr;

// 钩子回调函数
LRESULT CALLBACK MsgHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (HC_ACTION == nCode)
    {
        // 有消息到来
        PMSG pMsg = reinterpret_cast<PMSG>(lParam);
        if (WM_CHAR == pMsg->message)
        {
            // 说明当前windows有一个可供记录的键盘按下键的信息
            // 为什么不在dll中改成多字节？
            // 因为windows在底层是unicode的
            wchar_t szKey                  = static_cast<wchar_t>(pMsg->wParam);
            wchar_t szOutput[MAX_PATH]     = { 0 };
            wchar_t szWindowName[MAX_PATH] = { 0 };

            ::GetWindowText(pMsg->hwnd, szWindowName, MAX_PATH);
            wsprintf(szOutput, L"在窗口 [%s] 中按键: %c", szWindowName, szKey);

            // 使用 debugview 观测
            ::OutputDebugString(szOutput);
        }
    }

    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// 启动钩子
extern "C" __declspec(dllexport) BOOL StartHook()
{
    if (g_hHook == nullptr)
    {
        // 安装钩子
        g_hHook = ::SetWindowsHookEx(WH_GETMESSAGE, MsgHookProc, g_hInstance, 0);

        if (g_hHook == nullptr)
        {
            DWORD dwErr = ::GetLastError();
            // 可以考虑记录错误日志
            return FALSE;
        }
    }
    return TRUE;
}

// 停止钩子
extern "C" __declspec(dllexport) BOOL StopHook()
{
    if (g_hHook != nullptr)
    {
        // 卸载钩子
        BOOL bRet = ::UnhookWindowsHookEx(g_hHook);
        g_hHook   = nullptr;
        return bRet;
    }
    return TRUE;
}

// DllMain 入口
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hModule;
            // 可以在这里禁用线程附加通知以提高性能
            ::DisableThreadLibraryCalls(hModule);
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            // 安全清理
            if (g_hHook != nullptr)
            {
                ::UnhookWindowsHookEx(g_hHook);
                g_hHook = nullptr;
            }
            break;
    }
    return TRUE;
}
