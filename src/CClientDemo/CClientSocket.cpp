// CClientSocket.cpp: 实现文件
//

#include "pch.h"
#include "CClientDemo.h"
#include "CClientSocket.h"
#include "CClientDemoDlg.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnConnect(int nErrorCode)
{
    if (nErrorCode)
    {
        AfxMessageBox(L"连接失败， 请重试");
        return;
    }

    ((CCClientDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.AddString(L"连接服务器成功");
    ((CCClientDemoDlg *)(AfxGetApp()->m_pMainWnd))
            ->m_listWords.SetTopIndex(((CCClientDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.GetCount() - 1);


    CAsyncSocket::OnConnect(nErrorCode);
}


// CClientSocket 成员函数
