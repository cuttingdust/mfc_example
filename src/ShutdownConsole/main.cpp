// #include <windows.h>
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
/// 192.168.1.221
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
    sockAddrIn.sin_addr.S_un.S_addr = inet_addr("192.168.1.221");
    if (::connect(sock, (sockaddr *)&sockAddrIn, sizeof(sockAddrIn)) == SOCKET_ERROR)
    {
        printf("connect 失败 !");
        ::closesocket(sock);
        return 0;
    }

    const char *sendData      = "Hello";
    const char *shutdown_data = "SHCTDOWN2";
    ::send(sock, sendData, strlen(sendData), 0);
    ::send(sock, shutdown_data, strlen(shutdown_data), 0);

    char recvData[255];
    int  ret = ::recv(sock, recvData, sizeof(recvData) - 1, 0);
    if (ret > 0)
    {
        recvData[ret] = 0;
        printf("recv data : %s \n", recvData);
    }


    ::closesocket(sock);
    ::WSACleanup();
    return 0;
}
