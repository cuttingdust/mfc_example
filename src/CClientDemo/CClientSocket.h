#pragma once

// CClientSocket 命令目标

class CClientSocket : public CAsyncSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

public:
    void OnConnect(int nErrorCode) override;
};
