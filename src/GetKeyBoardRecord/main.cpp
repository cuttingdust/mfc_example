#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <string>

using HookFunc = BOOL (*)();

int main()
{
    std::cout << "键盘钩子记录程序 - 输入 'exit' 退出" << std::endl;

    /// 1. 加载DLL
    HMODULE hDll = nullptr;

    // 尝试多种方式加载DLL
    const char* dllPaths[] = {
        "SetHookDll.dll", ".\\SetHookDll.dll",
        "..\\out\\bin.x64\\SetHookDll.dll" /// 根据你的项目结构调整
    };

    for (const auto& path : dllPaths)
    {
        hDll = ::LoadLibraryA(path);
        if (hDll)
        {
            std::cout << "成功加载DLL: " << path << std::endl;
            break;
        }
    }

    if (!hDll)
    {
        DWORD error = ::GetLastError();
        std::cerr << "无法加载DLL，错误代码: " << error << std::endl;
        std::cerr << "请确保SetHookDll.dll在以下位置之一:" << std::endl;
        for (const auto& path : dllPaths)
        {
            std::cerr << "  - " << path << std::endl;
        }
        return -1;
    }

    /// 2. 获取函数地址（使用安全的类型转换）
    HookFunc pStartHook = reinterpret_cast<HookFunc>(::GetProcAddress(hDll, "StartHook"));

    HookFunc pStopHook = reinterpret_cast<HookFunc>(::GetProcAddress(hDll, "StopHook"));

    if (!pStartHook || !pStopHook)
    {
        DWORD error = ::GetLastError();
        std::cerr << "无法获取函数地址，错误代码: " << error << std::endl;
        ::FreeLibrary(hDll);
        return -1;
    }

    /// 3. 启动钩子
    std::cout << "正在启动键盘钩子..." << std::endl;
    if (!pStartHook())
    {
        std::cerr << "启动钩子失败!" << std::endl;
        ::FreeLibrary(hDll);
        return -1;
    }

    std::cout << "键盘钩子已启动，正在监听键盘输入..." << std::endl;
    std::cout << "请使用 DebugView 查看输出" << std::endl;
    std::cout << "输入 'exit' 退出程序" << std::endl;

    /// 4. 主循环
    bool shouldExit = false;
    while (!shouldExit)
    {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        //。 转换为小写进行比较
        std::string lowerInput = input;
        std::ranges::transform(lowerInput, lowerInput.begin(), ::tolower);

        if (lowerInput == "exit" || lowerInput == "quit" || lowerInput == "q")
        {
            shouldExit = true;
        }
        else if (lowerInput == "status")
        {
            std::cout << "钩子正在运行中..." << std::endl;
        }
        else if (lowerInput == "help")
        {
            std::cout << "可用命令:" << std::endl;
            std::cout << "  exit/quit/q - 退出程序" << std::endl;
            std::cout << "  status      - 查看状态" << std::endl;
            std::cout << "  help        - 显示帮助" << std::endl;
        }
        else if (!input.empty())
        {
            std::cout << "未知命令，输入 'help' 查看帮助" << std::endl;
        }
    }

    /// 5. 清理资源
    std::cout << "正在停止键盘钩子..." << std::endl;
    if (!pStopHook())
    {
        std::cerr << "停止钩子时出现警告" << std::endl;
    }

    if (!::FreeLibrary(hDll))
    {
        DWORD error = ::GetLastError();
        std::cerr << "卸载DLL失败，错误代码: " << error << std::endl;
    }

    std::cout << "程序退出" << std::endl;
    std::cout << "按任意键继续..." << std::endl;
    std::cin.get(); /// 等待用户按键

    return 0;
}
