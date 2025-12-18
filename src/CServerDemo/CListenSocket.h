#pragma once

// CListenSocket 命令目标

class CListenSocket : public CAsyncSocket
{
public:
    CListenSocket();
    virtual ~CListenSocket();

public:
    void OnAccept(int nErrorCode) override;
};
