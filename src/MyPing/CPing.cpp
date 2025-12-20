#include "CPing.h"

USHORT CPing::s_usPacketSeq = 0;


CPing::CPing() : m_szICMPData(NULL), m_bIsInitSucc(FALSE)
{
    WSADATA wsaData;
    ::WSAStartup(MAKEWORD(2, 2), &wsaData);

    m_event           = ::WSACreateEvent();
    m_usCurrentProcID = (USHORT)::GetCurrentProcessId();

    if ((m_sockRaw = ::WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0)) != SOCKET_ERROR)
    {
        ::WSAEventSelect(m_sockRaw, m_event, FD_READ);
        m_bIsInitSucc = TRUE;
        m_szICMPData  = (char *)malloc(MAX_PACKET_SIZE + sizeof(ICMPPHeader));
        if (m_szICMPData == NULL)
        {
            m_bIsInitSucc = FALSE;
        }
    }
}


CPing::~CPing()
{
    WSACleanup();
    if (NULL != m_szICMPData)
    {
        free(m_szICMPData);
        m_szICMPData = NULL;
    }
}

BOOL CPing::Ping(DWORD dwDestIP, PingReply *pPingReply, DWORD dwTimeOut)
{
    return PingCore(dwDestIP, pPingReply, dwTimeOut);
}

BOOL CPing::Ping(const char *szDestIP, PingReply *pPingReply, DWORD dwTimeOut)
{
    if (NULL != szDestIP)
    {
        return PingCore(inet_addr(szDestIP), pPingReply, dwTimeOut);
    }
    return FALSE;
}

BOOL CPing::PingCore(DWORD dwDestIP, PingReply *pPingReply, DWORD dwTimeOut)
{
    if (!m_bIsInitSucc)
        return FALSE;

    /// 配置Sock
    sockaddr_in sockAddrDest;
    sockAddrDest.sin_family      = AF_INET;
    sockAddrDest.sin_addr.s_addr = dwDestIP;
    sockAddrDest.sin_port        = 0;
    /// 填充ICMP报文
    int nSockAddrDestSize = sizeof(sockaddr_in);

    int    nICMPDataSize   = DEF_PACKET_SIZE + sizeof(ICMPPHeader);
    ULONG  ulSendTimeStamp = GetTickCountCalibrate();
    USHORT usSeq           = ++s_usPacketSeq;
    memset(m_szICMPData, 0, nICMPDataSize);
    ICMPPHeader *pICMPHdr   = (ICMPPHeader *)m_szICMPData;
    pICMPHdr->m_byType      = ICMP_ECHO_REQUEST;
    pICMPHdr->m_byCode      = 0;
    pICMPHdr->m_usID        = m_usCurrentProcID;
    pICMPHdr->m_usSeq       = usSeq;
    pICMPHdr->m_ulTimeStamp = ulSendTimeStamp;
    pICMPHdr->m_usChecksum  = 0;
    pICMPHdr->m_usChecksum  = CalCheckSum((USHORT *)pICMPHdr, nICMPDataSize);
    /// 发送ICMP报文
    if (sendto(m_sockRaw, m_szICMPData, nICMPDataSize, 0, (struct sockaddr *)&sockAddrDest, nSockAddrDestSize) ==
        SOCKET_ERROR)
    {
        return FALSE;
    }

    /// 判断应答
    if (pPingReply == NULL)
        return TRUE;

    char recvbuf[256] = { "\0" };
    while (TRUE)
    {
        if (::WSAWaitForMultipleEvents(1, &m_event, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
        {
            WSANETWORKEVENTS netEvent;
            WSAEnumNetworkEvents(m_sockRaw, m_event, &netEvent);

            if (netEvent.lNetworkEvents & FD_READ)
            {
                ULONG nRecvTimestamp = GetTickCountCalibrate();
                int   nBytesRead = ::recvfrom(m_sockRaw, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&sockAddrDest,
                                              &nSockAddrDestSize);
                if (nBytesRead >= (ICMP_HEADER_SIZE + sizeof(IPHeader)))
                {
                    IPHeader    *pIPHdr   = (IPHeader *)recvbuf;
                    ICMPPHeader *pICMPHdr = (ICMPPHeader *)(recvbuf + (pIPHdr->m_byVerHLen & 0x0f) * 4);
                    if ((pICMPHdr->m_byType == ICMP_ECHO_REPLY) && (pICMPHdr->m_usID == m_usCurrentProcID) &&
                        (pICMPHdr->m_usSeq == usSeq))
                    {
                        DWORD dwRoundTrip         = GetTickCountCalibrate() - pICMPHdr->m_ulTimeStamp;
                        pPingReply->m_usSeq       = pICMPHdr->m_usSeq;
                        pPingReply->m_dwRoundTrip = dwRoundTrip;
                        pPingReply->m_dwBytes     = nBytesRead - sizeof(IPHeader);
                        pPingReply->m_dwTTL       = pIPHdr->m_byTTL;
                        return TRUE;
                    }
                }
            }
        }
    }

    DWORD dwStartTime = GetTickCountCalibrate();
    if (dwStartTime - ulSendTimeStamp >= dwTimeOut)
        return FALSE;
}

USHORT CPing::CalCheckSum(USHORT *pBuf, int iSize)
{
    /// 发送ICMP报文时，ICMP首部和数据部分需要进行校验和计算
    /// 数据是奇数个字节时，最后一个字节补0处理
    unsigned long ulCheckSum = 0;
    USHORT        usAnswer   = 0;
    USHORT       *pW         = pBuf;
    int           iLeft      = iSize;
    while (iLeft > 1)
    {
        ulCheckSum += *pW++;
        iLeft -= sizeof(USHORT);
    }
    if (iLeft == 1)
    {
        *((UCHAR *)&usAnswer) = *((UCHAR *)pW);
        ulCheckSum += usAnswer;
    }
    ulCheckSum = (ulCheckSum >> 16) + (ulCheckSum & 0xffff);
    ulCheckSum += (ulCheckSum >> 16);
    usAnswer = (USHORT)~ulCheckSum;
    return usAnswer;
}

/// 计算UNIX时间戳，精确到毫秒
ULONG CPing::GetTickCountCalibrate()
{
    static ULONG    s_ulFirstCallTick    = 0;
    static LONGLONG s_ullFirstCallTickMS = 0;

    SYSTEMTIME systemTime;
    FILETIME   fileTime;
    ::GetSystemTime(&systemTime);

    ::SystemTimeToFileTime(&systemTime, &fileTime);

    LARGE_INTEGER liCurrentTime;
    liCurrentTime.HighPart = fileTime.dwHighDateTime;
    liCurrentTime.LowPart  = fileTime.dwLowDateTime;

    LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000; // 转换为毫秒

    if (s_ulFirstCallTick == 0)
        s_ulFirstCallTick = ::GetTickCount();

    if (s_ullFirstCallTickMS == 0)
        s_ullFirstCallTickMS = llCurrentTimeMS;

    return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}
