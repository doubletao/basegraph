#include "..\..\include\UIControl\UIElement.h"
#include "stdafx.h"
#include "UIControl\UIElement.h"


void UIElement::Release()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * & pChild = m_vecChildElement[i];
		if (pChild)
		{
			delete pChild;
			pChild = nullptr;
		}
	}
	m_vecChildElement.clear();

	for (std::map<int, std::vector<UIDrawer *>>::iterator it = m_mapDrawer.begin(); it != m_mapDrawer.end(); it++)
	{
		std::vector<UIDrawer *> & vecDrawer = it->second;
		for (size_t i = 0; i < vecDrawer.size(); i++)
		{
			UIDrawer * & pDrawer = vecDrawer[i];
			if (pDrawer)
			{
				delete pDrawer;
				pDrawer = nullptr;
			}
		}
	}
	m_mapDrawer.clear();
	m_nX = 0;
	m_nY = 0;
	m_nWidth = 1;
	m_nHeight = 1;
	m_nID = 0;
	m_hParentWnd = nullptr;
	m_nState = 0;
	m_bCreated = false;
}

void UIElement::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd/* = nullptr*/, UIElement * pParent/* = nullptr*/)
{
	m_nX = nX;
	m_nY = nY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nID = nID;
	m_hParentWnd = hParentWnd;
	m_pParentElement = pParent;
	m_nState = 0;
	m_bCreated = true;
}

void UIElement::SetText(CString strText)
{
	m_strText = strText;
	Refresh();
}

void UIElement::Refresh()
{
	if (m_pParentElement)
	{
		//如果有父物件，就通知父物件刷新
		m_pParentElement->Refresh();
	}
	else
	{
		//如果已经是顶层，则刷新自己的区域
		InvalidateRect(m_hParentWnd, CRect(m_nX, m_nY, m_nX + m_nWidth, m_nY + m_nHeight), false);
	}
	EmitMsg(EmUIElementMsg_BtnRefresh, m_nID, 0);
}

void UIElement::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * pChild = m_vecChildElement[i];
		if (pChild)
		{
			pChild->HandleMsg(nMsg, lParam, wParam);
		}
	}
}

void UIElement::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& pChild = m_vecChildElement[i];
		if (pChild && pChild->HitTest(lParam))
		{
			pChild->OnMouseHover();
		}
		else
		{
			pChild->OnMouseLeave();
		}
	}
}

int UIElement::GetRealX()
{
	if (m_pParentElement)
	{
		return m_nX + m_pParentElement->GetRealX();
	}
	return m_nX;
}

int UIElement::GetRealY()
{
	if (m_pParentElement)
	{
		return m_nY + m_pParentElement->GetRealY();
	}
	return m_nY;
}

bool UIElement::EmitMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		::SendMessage(m_hParentWnd, emMsg, wParam, lParam);
	}
	if (m_pParentElement)
	{
		m_pParentElement->OnChildMsg(emMsg, wParam, lParam);
	}
	return true;
}

bool UIElement::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pParentElement)
	{
		//默认就把消息往上扔
		return m_pParentElement->OnChildMsg(emMsg, wParam, lParam);
	}
	return false;
}

UIElement::UIElement()
	: m_hParentWnd(nullptr)
	, m_pParentElement(nullptr)
	, m_nID(0)
	, m_nX(0)
	, m_nY(0)
	, m_nWidth(1)
	, m_nHeight(1)
	, m_nState(0)
	, m_strText(_T(""))
	, m_bCreated(false)
{
}


UIElement::~UIElement()
{
	Release();
}

void UIElement::RearrangeChildPos()
{
}

EmUIElementMsgRet UIElement::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccContinue;
}

bool UIElement::IsSucceed(EmUIElementMsgRet emMsgRet)
{
	if (emMsgRet | UIElementMsgSucc)
	{
		return true;
	}
	return false;
}

bool UIElement::NeedContinue(EmUIElementMsgRet emMsgRet)
{
	if (emMsgRet | UIElementMsgContinue)
	{
		return true;
	}
	return false;
}

bool UIElement::HitTest(CPoint point)
{
	if (false)
	{
		CRect rtTest = GetRect();
		BOOL bTest = rtTest.PtInRect(point);
		int i = 0;
		i++;
	}
	if (GetRect().PtInRect(point))
	{
		return true;
	}
	return false;
}

bool UIElement::HitTest(int nX, int nY)
{
	return HitTest(CPoint(nX, nY));
}

bool UIElement::HitTest(LPARAM lParam)
{
	return HitTest(LOWORD(lParam), HIWORD(lParam));
}

void UIElement::DrawElement(CDC * pDC)
{
	if (false)
	{
		CString strDebug;
		strDebug.Format(_T("m_nState:%d, m_nID:%d, m_strText:%s, GetRealX():%d, GetRealY():%d\n"), m_nState, m_nID, m_strText, GetRealX(), GetRealY());
		OutputDebugString(strDebug);
	}
	//先画自己
	if (m_mapDrawer.count(m_nState))
	{
		std::vector<UIDrawer *> & vecDrawer = m_mapDrawer[m_nState];
		for (size_t i = 0; i < vecDrawer.size(); i++)
		{
			UIDrawer * & pDrawer = vecDrawer[i];
			if (pDrawer)
			{
				pDrawer->Draw(pDC, GetRealX(), GetRealY(), m_nWidth, m_nHeight, m_strText);
			}
		}
	}
	//再画子
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * & pChild = m_vecChildElement[i];
		if (pChild)
		{
			pChild->DrawElement(pDC);
		}
	}
	if (false)
	{
		CImage img;
		img.Attach((HBITMAP)pDC->GetCurrentBitmap()->GetSafeHandle());
		img.Save(_T("F:\\test.bmp"));
		img.Detach();
	}
}

void UIElement::OnMouseHover()
{
}

void UIElement::OnMouseLeave()
{
}

int UIElement::GetMsgID()
{
	return m_nID;
}

int UIElement::GetWidth()
{
	return m_nWidth;
}

int UIElement::GetHeight()
{
	return m_nHeight;
}

CRect UIElement::GetRect()
{
	int nX = GetRealX();
	int nY = GetRealY();
	return CRect(nX, nY, nX + m_nWidth, nY + m_nHeight);
}

inline void UIElement::MoveTo(CRect rtNew, bool bRefresh/* = true*/)
{
	m_nX = rtNew.left;
	m_nY = rtNew.top;
	m_nWidth = rtNew.Width();
	m_nHeight = rtNew.Height();
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
}

inline void UIElement::MoveTo(int nX, int nY, bool bRefresh/* = true*/)
{
	m_nX = nX;
	m_nY = nY;
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
}

inline void UIElement::MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh/* = true*/)
{
	m_nX = nX;
	m_nY = nY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
}

