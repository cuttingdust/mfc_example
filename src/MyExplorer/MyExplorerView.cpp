
// MyExplorerView.cpp: CMyExplorerView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MyExplorer.h"
#endif

#include "MyExplorerDoc.h"
#include "MyExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyExplorerView

IMPLEMENT_DYNCREATE(CMyExplorerView, CHtmlView)

BEGIN_MESSAGE_MAP(CMyExplorerView, CHtmlView)
// 标准打印命令
ON_COMMAND(ID_FILE_PRINT, &CHtmlView::OnFilePrint)
END_MESSAGE_MAP()

// CMyExplorerView 构造/析构

CMyExplorerView::CMyExplorerView() noexcept
{
    // TODO: 在此处添加构造代码
}

CMyExplorerView::~CMyExplorerView()
{
}

BOOL CMyExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CHtmlView::PreCreateWindow(cs);
}

void CMyExplorerView::OnInitialUpdate()
{
    CHtmlView::OnInitialUpdate();

    SetSilent(TRUE);
    Navigate2(L"http://www.microsoft.com/", NULL, NULL); // 微软官网
}


// CMyExplorerView 打印


// CMyExplorerView 诊断

#ifdef _DEBUG
void CMyExplorerView::AssertValid() const
{
    CHtmlView::AssertValid();
}

void CMyExplorerView::Dump(CDumpContext& dc) const
{
    CHtmlView::Dump(dc);
}

CMyExplorerDoc* CMyExplorerView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyExplorerDoc)));
    return (CMyExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyExplorerView 消息处理程序
