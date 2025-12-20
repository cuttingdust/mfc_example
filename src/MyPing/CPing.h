#pragma once
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")

#define DEF_PACKET_SIZE   32
#define MAX_PACKET_SIZE   1024
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY   0
#define ICMP_HEADER_SIZE  8
typedef struct
{
    BYTE   m_byVerHLen;      /// 一个字节，版本和首部长度
    BYTE   m_byTOS;          /// 一个字节，服务类型
    USHORT m_usTotalLen;     /// 两个字节，总长度
    USHORT m_usID;           /// 两个字节，标识
    USHORT m_usFragAndFlags; /// 两个字节，标志和片偏移
    BYTE   m_byTTL;          /// 一个字节，生存时间
    BYTE   m_byProtocol;     /// 一个字节，协议
    USHORT m_usHeaderXSum;   /// 两个字节，首部校验和
    ULONG  m_uSrcIP;         /// 四个字节，源IP地址
    ULONG  m_uDestIP;        /// 四个字节，目的IP地址
} IPHeader;

typedef struct
{
    BYTE   m_byType;      /// 一个字节，类型
    BYTE   m_byCode;      /// 一个字节，代码
    USHORT m_usChecksum;  /// 两个字节，校验和
    USHORT m_usID;        /// 两个字节，标识
    USHORT m_usSeq;       /// 两个字节，序列号
    ULONG  m_ulTimeStamp; /// 四个字节，时间戳
} ICMPPHeader;

typedef struct
{
    USHORT m_usSeq;       /// 两个字节，序列号
    DWORD  m_dwRoundTrip; /// 四个字节，ping 往返时间
    DWORD  m_dwBytes;     /// 四个字节，传送的字节数
    DWORD  m_dwTTL;       /// 四个字节，存活时间
} PingReply;


class CPing
{
public:
    CPing();
    virtual ~CPing();
    BOOL Ping(DWORD dwDestIP, PingReply* pPingReply = NULL, DWORD dwTimeOut = 2000);
    BOOL Ping(const char* szDestIP, PingReply* pPingReply = NULL, DWORD dwTimeOut = 2000);

private:
    BOOL   PingCore(DWORD dwDestIP, PingReply* pPingReply, DWORD dwTimeOut);
    USHORT CalCheckSum(USHORT* pBuf, int iSize);
    ULONG  GetTickCountCalibrate();

private:
    SOCKET   m_sockRaw;
    WSAEVENT m_event;
    USHORT   m_usCurrentProcID;
    char*    m_szICMPData;
    BOOL     m_bIsInitSucc;

private:
    static USHORT s_usPacketSeq;
};
