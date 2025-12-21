// #include <windows.h>
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")


BOOL ShutDownFunc()
{
    HANDLE           hToken;
    TOKEN_PRIVILEGES tkp;
    /// 首先获得当前进程的特征
    if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return FALSE;
    }
    /// 获取关机特权的LUID
    ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount           = 1; // 设置一个特权
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    /// 获取关机特权
    ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (::GetLastError() != ERROR_SUCCESS)
    {
        printf("AdjustTokenPrivileges 失败 !\n");
        return FALSE;
    }
    /// 关机 同时关闭所有应用程序


    if (!::ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
                         SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED))
    {
        return FALSE;
    }
    ::CloseHandle(hToken);

    return TRUE;
}

int main(int argc, char *argv[])
{
    WORD    sockVer = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (::WSAStartup(sockVer, &wsaData) != 0)
    {
        return 0;
    }

    /// 创建套接字
    SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("socket 创建失败 !");
        return 0;
    }

    /// 绑定地址和端口
    sockaddr_in sockAddrIn;
    sockAddrIn.sin_family           = AF_INET;
    sockAddrIn.sin_port             = ::htons(8888);
    sockAddrIn.sin_addr.S_un.S_addr = INADDR_ANY;
    if (::bind(sock, (sockaddr *)&sockAddrIn, sizeof(sockAddrIn)) == SOCKET_ERROR)
    {
        printf("bind 失败 !");
        ::closesocket(sock);
        return 0;
    }

    if (::listen(sock, 5) == SOCKET_ERROR)
    {
        printf("listen 失败 !");
        ::closesocket(sock);
        return 0;
    }

    /// 循环等待客户端连接
    SOCKET      clientSock;
    sockaddr_in clientAddr;
    int         addrLen = sizeof(clientAddr);
    char        buffer[1024];
    while (TRUE)
    {
        printf("等待远程关机命令的到来...\n");
        clientSock = ::accept(sock, (sockaddr *)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET)
        {
            printf("accept 失败 !");
            ::closesocket(sock);
            return 0;
        }

        printf("远程连接来自: %s\n", inet_ntoa(clientAddr.sin_addr));
        /// 接收数据
        int ret = ::recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (ret > 0)
        {
            buffer[ret] = '\0';
            printf("收到命令: %s\n", buffer);
            if (strcmp(buffer, "SHUTDOWN") == 0)
            {
                printf("执行关机操作...\n");
                ShutDownFunc();
            }
            else
            {
                printf("未知命令: %s\n", buffer);
            }

            const char *senddata = "shutdown the machine right now...";
            ::send(clientSock, senddata, (int)strlen(senddata), 0);
        }
    }


    ShutDownFunc();
    return 0;
}
