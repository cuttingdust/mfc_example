// CListenSocket.cpp: 实现文件
//

#include "pch.h"
#include "CServerDemo.h"
#include "CListenSocket.h"

#include "CServerDemoDlg.h"


// CListenSocket

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode)
{
    Accept(((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_serverSocket);
    ((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_serverSocket.AsyncSelect(FD_READ | FD_CLOSE);
    ((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.AddString(L"接受了一个客户端的连接请求");
    ((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))
            ->m_listWords.SetTopIndex(((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.GetCount() - 1);
    CAsyncSocket::OnAccept(nErrorCode);
}


// CListenSocket 成员函数
