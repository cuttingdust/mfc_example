// CServer.cpp: 实现文件
//

#include "pch.h"
#include "CServerDemo.h"
#include "CServer.h"
#include "CServerDemoDlg.h"

// CServer

CServer::CServer()
{
}

CServer::~CServer()
{
}

void CServer::OnReceive(int nErrorCode)
{
    char szTemp[4096];
    int  nReceived = 0;
    do
    {
        nReceived = Receive(szTemp, sizeof(szTemp));
        if (nReceived > 0)
        {
            /// 将新数据追加到缓冲区
            m_recvBuffer.insert(m_recvBuffer.end(), szTemp, szTemp + nReceived);
            /// 处理缓冲区中可能存在的完整消息
            ProcessBuffer();
        }
    }
    while (nReceived == sizeof(szTemp)); // 如果缓冲区满，可能还有数据

    CAsyncSocket::OnReceive(nErrorCode);
}

void CServer::ProcessBuffer()
{
    /// 循环处理，直到缓冲区不够一条消息
    while (true)
    {
        /// 状态1: 正在等待4字节的“长度包头”
        if (m_nExpectedBytes == -1)
        {
            if (m_recvBuffer.size() >= sizeof(int))
            {
                /// 读取网络字节序的长度并转换为主机序
                int netLen;
                memcpy(&netLen, m_recvBuffer.data(), sizeof(int));
                m_nExpectedBytes = ntohl(netLen); /// 这才是我们真正要接收的数据长度

                /// 从缓冲区移除已处理的包头
                m_recvBuffer.erase(m_recvBuffer.begin(), m_recvBuffer.begin() + sizeof(int));
            }
            else
            {
                break; /// 包头还没收齐，跳出等待
            }
        }

        /// 状态2: 已经知道数据长度，等待数据体收齐
        if (m_nExpectedBytes > 0)
        {
            if (m_recvBuffer.size() >= (size_t)m_nExpectedBytes)
            {
                /// 数据已收齐，进行转换和显示
                /// 注意：这里假设发送方发送的是宽字符 (Unicode)
                int     charCount = m_nExpectedBytes / sizeof(wchar_t);
                CString strReceived((LPCTSTR)m_recvBuffer.data(), charCount);

                CString strMsg;
                strMsg.Format(L"收到：%s", (LPCTSTR)strReceived);

                /// 显示到界面
                CCServerDemoDlg *pDlg = (CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd);
                if (pDlg && ::IsWindow(pDlg->m_hWnd))
                {
                    pDlg->m_listWords.AddString(strMsg);
                    pDlg->m_listWords.SetTopIndex(pDlg->m_listWords.GetCount() - 1);
                }

                /// 从缓冲区移除已处理的数据
                m_recvBuffer.erase(m_recvBuffer.begin(), m_recvBuffer.begin() + m_nExpectedBytes);
                m_nExpectedBytes = -1; /// 重置状态，准备读取下一个包头
            }
            else
            {
                break; /// 数据体还没收齐，跳出等待
            }
        }
    }
}

void CServer::OnClose(int nErrorCode)
{
    ((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.AddString(L"客户端断开连接");
    ((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))
            ->m_listWords.SetTopIndex(((CCServerDemoDlg *)(AfxGetApp()->m_pMainWnd))->m_listWords.GetCount() - 1);
}


// CServer 成员函数
