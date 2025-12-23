
// MyExplorerView.h: CMyExplorerView 类的接口
//

#pragma once


class CMyExplorerView : public CHtmlView
{
protected: // 仅从序列化创建
	CMyExplorerView() noexcept;
	DECLARE_DYNCREATE(CMyExplorerView)

// 特性
public:
	CMyExplorerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CMyExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnViewhtml();
};

#ifndef _DEBUG  // MyExplorerView.cpp 中的调试版本
inline CMyExplorerDoc* CMyExplorerView::GetDocument() const
   { return reinterpret_cast<CMyExplorerDoc*>(m_pDocument); }
#endif

