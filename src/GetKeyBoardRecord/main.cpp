#include <iostream>
#include <Windows.h>

int main(int argc, char *argv[])
{
    /// 获得键盘的按下信息，因为键盘按下如果是ab字母
    /// Windows操作系统一定会把其对应的ascii信息记录下来
    /// 如果我有一个定时器，不停的去观测这个函数
    /// 是不是所有的键盘都能不被记录下来
    // while (true)
    // {
    //     std::cout << GetAsyncKeyState(VK_RETURN) << std::endl;
    //     Sleep(100);
    // }
    /// 思路:截获GetAsyncKeyState的消息，为当前的应用程序所使用


    return 0;
}
