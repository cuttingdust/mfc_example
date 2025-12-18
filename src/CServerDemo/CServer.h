#pragma once
#include <vector>

// CServer 命令目标

class CServer : public CAsyncSocket
{
public:
    CServer();
    virtual ~CServer();

public:
    void OnReceive(int nErrorCode) override;
    void OnClose(int nErrorCode) override;

private:
    std::vector<BYTE> m_recvBuffer;          /// 累积接收数据的缓冲区
    int               m_nExpectedBytes = -1; /// 期望接收的“数据部分”字节数， -1表示正在等待包头
    void              ProcessBuffer();
};
