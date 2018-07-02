#include "stdafx.h"
#include "UIPropertyView.h"
#include "UIPropertyDynamicViewElement.h"

UIPropertyView::UIPropertyView()
	: m_pPropertyTabBar(nullptr)
	, m_pColorElement(nullptr)
	, m_pObjectElement(nullptr)
	, m_pSpaceElement(nullptr)
	, m_pFxElement(nullptr)
	, m_pTextElement(nullptr)
	, m_strObjID()
	, m_emCurrentTab(UIElementMsg_PropertyTabColor)
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
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd);
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
	if (m_pColorElement)
	{
		m_pColorElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
	if (m_pFxElement)
	{
		m_pFxElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
	if (m_pTextElement)
	{
		m_pTextElement->MoveTo(nPropertyTabBarWdith + nGap, 0, m_nWidth - nPropertyTabBarWdith - nGap, m_nHeight);
	}
}

void UIPropertyView::InitializeObject()
{
	m_pPropertyTabBar = new UIPropertyTabBar;
	m_pPropertyTabBar->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pPropertyTabBar);

	m_pColorElement = new UIPropertyViewElement;
	m_pColorElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Color, m_hParentWnd, this, BaseGraph_DataBase::ColorParam, BaseGraph_DataBase::ColorParam_End);
	m_vecChildElement.push_back(m_pColorElement);

	m_pObjectElement = new UIPropertyDynamicViewElement;
	m_pObjectElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Object, m_hParentWnd, this, BaseGraph_DataBase::TitlewordParam, BaseGraph_DataBase::TitlewordParam_End);
	m_vecChildElement.push_back(m_pObjectElement);

	m_pSpaceElement = new UIPropertyViewElement;
	m_pSpaceElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Space, m_hParentWnd, this, BaseGraph_DataBase::SpaceParam, BaseGraph_DataBase::SpaceParam_End);
	m_vecChildElement.push_back(m_pSpaceElement);

	//m_pFxElement = new UIPropertyViewElement;
	//m_pFxElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Fx, m_hParentWnd, this, BaseGraph_DataBase::ColorParam, BaseGraph_DataBase::ColorParam_End);
	//m_vecChildElement.push_back(m_pFxElement);

	//m_pTextElement = new UIPropertyViewElement;
	//m_pTextElement->Create(0, 0, 100, 100, UIElementMsg_PropertyView_Text, m_hParentWnd, this, BaseGraph_DataBase::SpaceParam, BaseGraph_DataBase::SpaceParam_End);
	//m_vecChildElement.push_back(m_pTextElement);
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
	case WM_MSG_OnCreateObjTool:
		//对各个窗口取出其值
		if (m_pColorElement)
		{
			m_pColorElement->BindCreateType((BaseGraph_DataBase::ObjectType)wParam);
		}
		if (m_pObjectElement)
		{
			m_pObjectElement->BindCreateType((BaseGraph_DataBase::ObjectType)wParam);
		}
		if (m_pSpaceElement)
		{
			m_pSpaceElement->BindCreateType((BaseGraph_DataBase::ObjectType)wParam);
		}
		if (m_pFxElement)
		{
			m_pFxElement->BindCreateType((BaseGraph_DataBase::ObjectType)wParam);
		}
		if (m_pTextElement)
		{
			m_pTextElement->BindCreateType((BaseGraph_DataBase::ObjectType)wParam);
		}
		break;
	case WM_MSG_OnSelectChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			CString strObjID = GetCurSelObjID();
			if (m_strObjID != strObjID)
			{
				//对各个窗口取出其值
				m_strObjID = strObjID;
				if (m_pColorElement)
				{
					m_pColorElement->BindObject(m_strObjID);
				}
				if (m_pObjectElement)
				{
					m_pObjectElement->BindObject(m_strObjID);
				}
				if (m_pSpaceElement)
				{
					m_pSpaceElement->BindObject(m_strObjID);
				}
				if (m_pFxElement)
				{
					m_pFxElement->BindObject(m_strObjID);
				}
				if (m_pTextElement)
				{
					m_pTextElement->BindObject(m_strObjID);
				}
			}
		}
		break;
	case WM_MSG_ObjectSpaceChanged:
		if (m_pSpaceElement)
		{
			m_pSpaceElement->RefreshFromID(m_strObjID);
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
		//起始在color页面
		OnBtnPropertyTabColor();
	}
	return true;
}

EmUIElementMsgRet UIPropertyView::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ChildHandleMsg(nMsg, wParam, lParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue; 
}

void UIPropertyView::OnBtnPropertyTabColor()
{
	m_emCurrentTab = UIElementMsg_PropertyTabColor;
	if (m_pColorElement)
	{
		m_pColorElement->SetVisible(true);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->SetVisible(false);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->SetVisible(false);
	}
	if (m_pFxElement)
	{
		m_pFxElement->SetVisible(false);
	}
	if (m_pTextElement)
	{
		m_pTextElement->SetVisible(false);
	}
}

void UIPropertyView::OnBtnPropertyTabObject()
{
	m_emCurrentTab = UIElementMsg_PropertyTabObject;
	if (m_pColorElement)
	{
		m_pColorElement->SetVisible(false);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->SetVisible(true);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->SetVisible(false);
	}
	if (m_pFxElement)
	{
		m_pFxElement->SetVisible(false);
	}
	if (m_pTextElement)
	{
		m_pTextElement->SetVisible(false);
	}
}

void UIPropertyView::OnBtnPropertyTabSpace()
{
	m_emCurrentTab = UIElementMsg_PropertyTabSpace;
	if (m_pColorElement)
	{
		m_pColorElement->SetVisible(false);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->SetVisible(false);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->SetVisible(true);
	}
	if (m_pFxElement)
	{
		m_pFxElement->SetVisible(false);
	}
	if (m_pTextElement)
	{
		m_pTextElement->SetVisible(false);
	}
}

void UIPropertyView::OnBtnPropertyTabFx()
{
	m_emCurrentTab = UIElementMsg_PropertyTabFx;
	if (m_pColorElement)
	{
		m_pColorElement->SetVisible(false);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->SetVisible(false);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->SetVisible(false);
	}
	if (m_pFxElement)
	{
		m_pFxElement->SetVisible(true);
	}
	if (m_pTextElement)
	{
		m_pTextElement->SetVisible(false);
	}
}

void UIPropertyView::OnBtnPropertyTabText()
{
	m_emCurrentTab = UIElementMsg_PropertyTabText;
	if (m_pColorElement)
	{
		m_pColorElement->SetVisible(false);
	}
	if (m_pObjectElement)
	{
		m_pObjectElement->SetVisible(false);
	}
	if (m_pSpaceElement)
	{
		m_pSpaceElement->SetVisible(false);
	}
	if (m_pFxElement)
	{
		m_pFxElement->SetVisible(false);
	}
	if (m_pTextElement)
	{
		m_pTextElement->SetVisible(true);
	}
}

void UIPropertyView::UpdateStatus()
{
	if (m_pPropertyTabBar)
	{
		m_pPropertyTabBar->CkeckOne(m_emCurrentTab);
	}
}

std::map<BaseGraph_DataBase::ParamValueDef, Value> UIPropertyView::GetCurrentParamValue()
{
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValueRet;
	if (m_pColorElement)
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue = m_pColorElement->GetValue();
		mapParamValueRet.insert(mapParamValue.begin(), mapParamValue.end());
	}
	if (m_pObjectElement)
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue = m_pObjectElement->GetValue();
		mapParamValueRet.insert(mapParamValue.begin(), mapParamValue.end());
	}
	if (m_pSpaceElement)
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue = m_pSpaceElement->GetValue();
		mapParamValueRet.insert(mapParamValue.begin(), mapParamValue.end());
	}
	if (m_pFxElement)
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue = m_pFxElement->GetValue();
		mapParamValueRet.insert(mapParamValue.begin(), mapParamValue.end());
	}
	if (m_pTextElement)
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParamValue = m_pTextElement->GetValue();
		mapParamValueRet.insert(mapParamValue.begin(), mapParamValue.end());
	}
	return mapParamValueRet;
}
