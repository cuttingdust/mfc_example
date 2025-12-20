#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#pragma comment(lib, "WS2_32")

#define BUFFER_SIZE 1024
#define SIO_RCVALL  _WSAIOW(IOC_VENDOR, 1)

typedef struct _TCP
{
    WORD  sport; /// 源端口
    WORD  dport; /// 目的端口
    DWORD seq;   /// 序列号
    DWORD ack;   /// 确认号
    BYTE  len;   /// 首部长度
    BYTE  flag;  /// 标志位
    WORD  win;   /// 窗口大小
    WORD  sum;   /// 校验和
    WORD  urp;   /// 紧急数据偏移
} TCP;

typedef TCP           *LPTCP;
typedef TCP UNALIGNED *ULPTCP; /// 在传输中 需要字节对齐

typedef struct _IP
{
    union
    {
        BYTE Version;
        BYTE HdrLen; /// IHL
    };
    BYTE serviceType; /// 服务类型
    WORD totalLen;    /// 总长度
    WORD ident;       /// 标识
    union
    {
        WORD flags;   /// 标志
        WORD fragOff; /// 片偏移
    };
    BYTE  timeToLive;  /// 生存时间
    BYTE  protocol;    /// 协议
    WORD  hdrChecksum; /// 首部校验和
    DWORD srcAddr;     /// 源地址
    DWORD destAddr;    /// 目的地址
    BYTE  opt;
} IP;

typedef IP           *LPIP;
typedef IP UNALIGNED *ULPIP; /// 在传输中 需要字节对齐

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sockRaw;

    /// 创建原始套接字
    sockRaw = ::socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    /// 设置IP头的操作选项 其中
    BOOL flag = TRUE;
    ::setsockopt(sockRaw, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(flag));
    /// 获取本机名
    char localHostName[256] = { "\0" };
    ::gethostname(localHostName, sizeof(localHostName));

    /// 获取本机IP地址
    hostent *hostEntry = ::gethostbyname(localHostName);

    /// 填充sockaddr_in结构体
    SOCKADDR_IN sockAddrIn;
    sockAddrIn.sin_family = AF_INET;
    sockAddrIn.sin_addr   = *((in_addr *)hostEntry->h_addr_list[0]);
    // sockAddrIn.sin_port   = ::htons(1004);
    sockAddrIn.sin_port = 0;
    /// 绑定套接字
    ::bind(sockRaw, (PSOCKADDR)&sockAddrIn, sizeof(SOCKADDR_IN));
    DWORD dwBytesReturned = 1;
    /// 设置混杂模式
    ::ioctlsocket(sockRaw, SIO_RCVALL, &dwBytesReturned);

    /// 开始抓包
    char          recvBuffer[BUFFER_SIZE] = { "\0" };
    std::ofstream ofs("tcp_log.txt", std::ios::out);
    while (!kbhit())
    {
        /// 接收数据
        int nBytesRead = ::recv(sockRaw, recvBuffer, BUFFER_SIZE, 0);
        if (nBytesRead == SOCKET_ERROR) // 或 nBytesRead == -1
        {
            int nError = ::WSAGetLastError();
            std::cerr << "recvfrom 失败! 错误代码: " << nError << std::endl;

            // 根据错误代码进行不同处理
            switch (nError)
            {
                case WSAEINVAL:
                    std::cerr << "错误: 套接字未绑定或未正确设置混杂模式。" << std::endl;
                    break;
                case WSAENETDOWN:
                    std::cerr << "错误: 网络子系统故障。" << std::endl;
                    break;
                case WSAEFAULT:
                    std::cerr << "错误: 缓冲区指针无效。" << std::endl;
                    break;
                default:
                    std::cerr << "未知套接字错误。" << std::endl;
                    break;
            }
            continue; // 或 break
        }
        else
        {
            /// 解析IP头
            ULPIP pIPHeader = (ULPIP)recvBuffer;


            /// 解析TCP头
            ULPTCP pTCPHeader = (ULPTCP)(recvBuffer + (pIPHeader->HdrLen & 0x0f) * 4);

            // ==============【新增】计算TCP数据长度 ==============
            // IP总长度（网络字节序）-> 转换为主机字节序
            WORD ipTotalLength = ntohs(pIPHeader->totalLen);

            // IP头部长度（单位是4字节）
            BYTE ipHeaderLength = (pIPHeader->HdrLen & 0x0f) * 4;

            // TCP头部长度（单位是4字节），存储在TCP头部的"len"字段的高4位
            BYTE tcpHeaderLength = ((pTCPHeader->len & 0xF0) >> 4) * 4;

            // TCP数据长度 = IP总长度 - IP头部长度 - TCP头部长度
            int tcpDataLength = ipTotalLength - ipHeaderLength - tcpHeaderLength;
            // ================================================

            // 在输出中添加TCP数据长度
            ofs << "Src IP: " << inet_ntoa(*(in_addr *)&pIPHeader->srcAddr) << " Src Port: " << ntohs(pTCPHeader->sport)
                << " -> Dest IP: " << inet_ntoa(*(in_addr *)&pIPHeader->destAddr)
                << " Dest Port: " << ntohs(pTCPHeader->dport) << " Seq: " << ntohl(pTCPHeader->seq)
                << " Ack: " << ntohl(pTCPHeader->ack) << " Flags: 0x" << std::hex << (int)pTCPHeader->flag << std::dec
                << " Data Len: " << tcpDataLength << " bytes" << std::endl;

            std::cout << "捕获TCP包: " << inet_ntoa(*(in_addr *)&pIPHeader->srcAddr) << ":" << ntohs(pTCPHeader->sport)
                      << " -> " << inet_ntoa(*(in_addr *)&pIPHeader->destAddr) << ":" << ntohs(pTCPHeader->dport)
                      << " 数据长度: " << tcpDataLength << "字节" << std::endl;
        }
    }

    ofs.close();
    getchar();
    return 0;
}
