
// ServMgrView.cpp: CServMgrView 类的实现
//


#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ServMgr.h"
#endif

#include "ServMgrDoc.h"
#include "ServMgrView.h"

#include "CServConfig.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServMgrView

IMPLEMENT_DYNCREATE(CServMgrView, CListView)

BEGIN_MESSAGE_MAP(CServMgrView, CListView)
ON_COMMAND(ID_START_SERVICE, &CServMgrView::OnStartService)
ON_NOTIFY_REFLECT(NM_RCLICK, &CServMgrView::OnNMRClick)
ON_COMMAND(ID_STOP_SERVICE, &CServMgrView::OnStopService)
ON_COMMAND(ID_PAUSE_SERVICE, &CServMgrView::OnPauseService)
ON_COMMAND(ID_CONTINUE_SERVICE, &CServMgrView::OnContinueService)
END_MESSAGE_MAP()

// CServMgrView 构造/析构

CServMgrView::CServMgrView() noexcept
{
    // TODO: 在此处添加构造代码
}

CServMgrView::~CServMgrView()
{
}

BOOL CServMgrView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CListView::PreCreateWindow(cs);
}

void CServMgrView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();
    ::SetWindowLongPtr(GetListCtrl().m_hWnd, GWL_STYLE,
                       ::GetWindowLongPtr(GetListCtrl().m_hWnd, GWL_STYLE) | LVS_REPORT);

    GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    GetListCtrl().InsertColumn(0, L"服务名称", LVCFMT_LEFT, 150);
    GetListCtrl().InsertColumn(1, L"服务状态", LVCFMT_LEFT, 90);
    GetListCtrl().InsertColumn(2, L"启动类型", LVCFMT_LEFT, 90);
    GetListCtrl().InsertColumn(3, L"文件路径", LVCFMT_LEFT, 530);
    GetListCtrl().InsertColumn(4, L"服务描述", LVCFMT_LEFT, 300);

    CServConfig m_ServCfg;
    CServItem*  m_pHeader = m_ServCfg.EnumServList();
    if (!m_pHeader)
        return;

    for (int idx = 0; m_pHeader != NULL; ++idx)
    {
        GetListCtrl().InsertItem(idx, L"");
        GetListCtrl().SetItemText(idx, 0, m_pHeader->m_strServDispName);
        GetListCtrl().SetItemText(idx, 1, m_ServCfg.GetStateString(m_pHeader->m_dwServStatus));
        GetListCtrl().SetItemText(idx, 2, m_ServCfg.GetStartTypeString(m_pHeader->m_dwStartType));
        GetListCtrl().SetItemText(idx, 3, m_pHeader->m_strBinPath);
        GetListCtrl().SetItemText(idx, 4, m_pHeader->m_strDescription);
        GetListCtrl().SetItemData(idx, (DWORD_PTR)m_pHeader);
        m_pHeader = m_pHeader->m_pNext;
    }
    // m_ServCfg.CtrlServStatus(L"MySQL80", SERVICE_RUNNING);
    // TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
    //  从而可以用项填充 ListView。
}


// CServMgrView 诊断

#ifdef _DEBUG
void CServMgrView::AssertValid() const
{
    CListView::AssertValid();
}

void CServMgrView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}

CServMgrDoc* CServMgrView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServMgrDoc)));
    return (CServMgrDoc*)m_pDocument;
}
#endif //_DEBUG


// CServMgrView 消息处理程序

void CServMgrView::OnStartService()
{
    // TODO: 在此添加命令处理程序代码
    // AfxMessageBox(L"启动服务");

    int nSel = GetListCtrl().GetSelectionMark();
    if (nSel >= 0)
    {
        CServConfig m_ServCfg;
        CServItem*  m_pItem = (CServItem*)GetListCtrl().GetItemData(nSel);
        ASSERT(m_pItem);
        if (!m_ServCfg.CtrlServStatus(m_pItem->m_strServName, SERVICE_RUNNING))
        {
            AfxMessageBox(L"启动服务失败");
        }
        GetListCtrl().SetItemText(nSel, 1, L"已启动");
    }
}

void CServMgrView::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (pNMItemActivate->iItem < 0)
        return;

    CMenu mMenu, *pMenu = NULL;
    mMenu.LoadMenu(IDR_R_MENU);
    pMenu = mMenu.GetSubMenu(0); /// 得到状态控制

    CPoint point;
    GetCursorPos(&point);
    CServItem* m_pItem = (CServItem*)GetListCtrl().GetItemData(pNMItemActivate->iItem);
    ASSERT(m_pItem);

    CServConfig m_ServCfg;
    DWORD       dwCurrStatus = 0;
    DWORD       dwAccepted   = m_ServCfg.GetServCtrlAccepted(m_pItem->m_strServName, &dwCurrStatus);
    if (dwAccepted == -1)
    {
        mMenu.EnableMenuItem(ID_START_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_STOP_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_PAUSE_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_CONTINUE_SERVICE, MF_GRAYED);
        goto __Track_menu;
    }

    if (!(dwAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE))
    {
        mMenu.EnableMenuItem(ID_PAUSE_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_CONTINUE_SERVICE, MF_GRAYED);
    }

    if (!(dwAccepted & SERVICE_ACCEPT_STOP))
    {
        mMenu.EnableMenuItem(ID_STOP_SERVICE, MF_GRAYED);
    }

    if (dwCurrStatus == SERVICE_RUNNING)
    {
        mMenu.EnableMenuItem(ID_START_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_CONTINUE_SERVICE, MF_GRAYED);
    }
    else if (dwCurrStatus == SERVICE_STOPPED)
    {
        mMenu.EnableMenuItem(ID_STOP_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_PAUSE_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_CONTINUE_SERVICE, MF_GRAYED);
    }
    else if (dwCurrStatus == SERVICE_PAUSED)
    {
        mMenu.EnableMenuItem(ID_PAUSE_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_START_SERVICE, MF_GRAYED);
    }
    else
    {
        mMenu.EnableMenuItem(ID_START_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_STOP_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_PAUSE_SERVICE, MF_GRAYED);
        mMenu.EnableMenuItem(ID_CONTINUE_SERVICE, MF_GRAYED);
    }

__Track_menu:
    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    *pResult = 0;
}

void CServMgrView::OnStopService()
{
    // TODO: 在此添加命令处理程序代码
    // AfxMessageBox(L"停止服务");
    int nSel = GetListCtrl().GetSelectionMark();
    if (nSel >= 0)
    {
        CServConfig m_ServCfg;
        CServItem*  m_pItem = (CServItem*)GetListCtrl().GetItemData(nSel);
        ASSERT(m_pItem);
        if (!m_ServCfg.CtrlServStatus(m_pItem->m_strServName, SERVICE_STOPPED))
        {
            AfxMessageBox(L"停止服务失败");
            return;
        }
        GetListCtrl().SetItemText(nSel, 1, L"已停止");
    }
}

void CServMgrView::OnPauseService()
{
    // TODO: 在此添加命令处理程序代码
    // AfxMessageBox(L"暂停服务");
    int nSel = GetListCtrl().GetSelectionMark();
    if (nSel >= 0)
    {
        CServConfig m_ServCfg;
        CServItem*  m_pItem = (CServItem*)GetListCtrl().GetItemData(nSel);
        ASSERT(m_pItem);
        if (!m_ServCfg.CtrlServStatus(m_pItem->m_strServName, SERVICE_PAUSED))
        {
            AfxMessageBox(L"暂停服务失败");
            return;
        }
        GetListCtrl().SetItemText(nSel, 1, L"已暂停");
    }
}

void CServMgrView::OnContinueService()
{
    // TODO: 在此添加命令处理程序代码
    // AfxMessageBox(L"继续服务");
    int nSel = GetListCtrl().GetSelectionMark();
    if (nSel >= 0)
    {
        CServConfig m_ServCfg;
        CServItem*  m_pItem = (CServItem*)GetListCtrl().GetItemData(nSel);
        ASSERT(m_pItem);
        if (!m_ServCfg.CtrlServStatus(m_pItem->m_strServName, SERVICE_RUNNING))
        {
            AfxMessageBox(L"恢复服务失败");
            return;
        }
        GetListCtrl().SetItemText(nSel, 1, L"已启动");
    }
}
