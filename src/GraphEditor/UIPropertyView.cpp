#include "stdafx.h"
#include "UIPropertyView.h"

UIPropertyView::UIPropertyView()
	: m_pPropertyTabBar(nullptr)
	, m_pSpaceElement(nullptr)
	, m_strObjID()
{
}


UIPropertyView::~UIPropertyView()
{
}

void UIPropertyView::Release()
{
	UIElement::Release();
	m_pPropertyTabBar = nullptr;
}

void UIPropertyView::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
	//注册消息中心
	UIControl_RegistMsgDispatcher(this);
}

void UIPropertyView::RearrangeChildPos()
{
	int nGap = UIControl_GetIntValue(ThemeInt_UIModualGap);
	int nPropertyTabBarWdith = UIControl_GetIntValue(ThemeInt_PropretyViewTabWidth);
	if (m_pPropertyTabBar)
	{
		m_pPropertyTabBar->MoveTo(0, 0, nPropertyTabBarWdith, m_nHeight);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
}

void UIPropertyView::InitializeObject()
{
	m_pPropertyTabBar = new UIPropertyTabBar;
	m_pPropertyTabBar->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pPropertyTabBar);

	m_pSpaceElement = new UIPropertyViewElement;
	m_pSpaceElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Space, m_hParentWnd, this, BaseGraph_DataBase::SpaceParam, BaseGraph_DataBase::SpaceParam_End);
	m_vecChildElement.push_back(m_pSpaceElement);
}

CString UIPropertyView::GetCurSelObjID()
{
	//获取当前的主选中项
	CString strID = _T("");
	std::vector<CString> vecSelObjID;
	if (DataRender_GetSelectObj(vecSelObjID))
	{
		//最后一个选中项视为主选中项
		if (vecSelObjID.size() > 0)
		{
			strID = *vecSelObjID.rbegin();
		}
	}
	return strID;
}

void UIPropertyView::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_OnSelectChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			CString strObjID = GetCurSelObjID();
			if (m_strObjID != strObjID)
			{
				//对各个窗口取出其值
				m_strObjID = strObjID;
				if (m_pSpaceElement)
				{
					m_pSpaceElement->BindObject(m_strObjID);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool UIPropertyView::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, UIElementMsg_PropertyView, hParentWnd);
		InitializeObject();
	}
	return true;
}

EmUIElementMsgRet UIPropertyView::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ChildHandleMsg(nMsg, lParam, wParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue; 
}
