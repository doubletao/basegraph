#include "stdafx.h"
#include "UIPropertyViewElement.h"

UIPropertyViewElement::UIPropertyViewElement()
{
}

UIPropertyViewElement::~UIPropertyViewElement()
{
}

void UIPropertyViewElement::InitializeObject()
{
	for (int i = m_nParamIDBegin + 1; i < m_nParamIDEnd; i++)
	{
		//填充默认值
		BaseGraph_DataBase::ParamValueDef paramValueDef = (BaseGraph_DataBase::ParamValueDef)i;
		BaseGraph_DataBase::ValueDescriptor valueDesc = BaseGraph_DataBase::GetValueDescriptor(paramValueDef);
		m_mapParam[paramValueDef] = valueDesc.stuDefault;
		int nTitleWidth = UIControl_GetIntValue(ThemeInt_PropertyViewCtrlTitleWidth);

		//生成控件
		UICombinedCtrl * pElement = nullptr;
		BaseGraph_DataBase::PropertyDispType controlType = valueDesc.emDispType;
		switch (controlType)
		{
		case BaseGraph_DataBase::PropertyDisp_StrSingleEdit:
			break;
		case BaseGraph_DataBase::PropertyDisp_StrMutiEdit:
			break;
		case BaseGraph_DataBase::PropertyDisp_StrPathEdit:
			break;
		case BaseGraph_DataBase::PropertyDisp_NumEdit:
			break;
		case BaseGraph_DataBase::PropertyDisp_SpinNumEdit:
		{
			pElement = new UIEditNumSpin;
			int nMin = 0;
			int nMax = 100;
			if (valueDesc.vecValueLimit.size() > 1)
			{
				nMin = valueDesc.vecValueLimit[0].first;
				nMax = valueDesc.vecValueLimit[1].first;
			}
			((UIEditNumSpin *)pElement)->Create(valueDesc.strDispName, nTitleWidth, 0, 10, nMin, nMax, valueDesc.nPrecision, valueDesc.fStep, 0, m_hParentWnd, this);
			((UIEditNumSpin *)pElement)->SetValue(valueDesc.stuDefault);
		}
			break;
		case BaseGraph_DataBase::PropertyDisp_SliderNumEdit:
		{
			pElement = new UIEditNumSlider;
			int nMin = 0;
			int nMax = 100;
			if (valueDesc.vecValueLimit.size() > 1)
			{
				nMin = valueDesc.vecValueLimit[0].first;
				nMax = valueDesc.vecValueLimit[1].first;
			}
			((UIEditNumSlider *)pElement)->Create(valueDesc.strDispName, nTitleWidth, 0, 10, nMin, nMax, valueDesc.nPrecision, valueDesc.fStep, 0, m_hParentWnd, this);
			((UIEditNumSlider *)pElement)->SetValue(valueDesc.stuDefault);
		}
			break;
		case BaseGraph_DataBase::PropertyDisp_CheckBmp:
			break;
		case BaseGraph_DataBase::PropertyDisp_RatioBmp:
			break;
		default:
			break;
		}
		if (pElement)
		{
			m_mapValueControl[paramValueDef] = pElement;
			m_mapControlValue[pElement] = paramValueDef;
			m_vecChildElement.push_back(pElement);
		}
	}
}

void UIPropertyViewElement::RearrangeChildPos()
{
	int nLeftTopGap = UIControl_GetIntValue(ThemeInt_PropretyViewControlLeftTopGap);
	int nCtrlGap = UIControl_GetIntValue(ThemeInt_PropertyViewCtrlGap);
	int nYPos = nLeftTopGap;
	//纵向从上往下排
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * pElement = m_vecChildElement[i];
		if (pElement)
		{
			//高度保持不变，宽度给两边留下缝隙
			int nHeight = pElement->GetHeight();
			pElement->MoveTo(nLeftTopGap, nYPos, m_nWidth - nLeftTopGap * 2, nHeight);
			nYPos += nHeight + nCtrlGap;
		}
	}
}

bool UIPropertyViewElement::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent, int nParamIDBegin, int nParamIDEnd)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);
		m_nParamIDBegin = nParamIDBegin;
		m_nParamIDEnd = nParamIDEnd;
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd, pParent);
		UIImageDrawer * pImgDrawer = new UIImageDrawer(ThemeImage_ElementArea);
		m_mapDrawer[0].push_back(pImgDrawer);
		InitializeObject();
		bRet = true;
	}
	return bRet;
}

void UIPropertyViewElement::SetValue(const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, bool bRefresh/* = true*/)
{
	for (std::map<BaseGraph_DataBase::ParamValueDef, Value>::const_iterator it = mapParam.begin(); it != mapParam.end(); it++)
	{
		m_mapParam[it->first] = it->second;
	}
	if (bRefresh)
	{
		UpdateDataToUI();
	}
}

void UIPropertyViewElement::SetValue(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value, bool bRefresh/* = true*/)
{
	m_mapParam[paramValueDef] = value;
	if (bRefresh)
	{
		UpdateDataToUI();
	}
}

std::map<BaseGraph_DataBase::ParamValueDef, Value> UIPropertyViewElement::GetValue()
{
	return m_mapParam;
}

Value UIPropertyViewElement::GetValue(BaseGraph_DataBase::ParamValueDef paramValueDef)
{
	if (m_mapParam.count(paramValueDef))
	{
		return m_mapParam[paramValueDef];
	}
	return Value();
}

void UIPropertyViewElement::UpdateDataToUI()
{
	for (int i = m_nParamIDBegin + 1; i < m_nParamIDEnd; i++)
	{
		BaseGraph_DataBase::ParamValueDef paramValueDef = (BaseGraph_DataBase::ParamValueDef)i;
		Value value = GetValue(paramValueDef);
		if (m_mapValueControl.count(paramValueDef) > 0
			&& m_mapValueControl[paramValueDef])
		{
			m_mapValueControl[paramValueDef]->SetValue(value);
		}
	}
}

void UIPropertyViewElement::UpdateUIToData()
{
	for (int i = m_nParamIDBegin + 1; i < m_nParamIDEnd; i++)
	{
		BaseGraph_DataBase::ParamValueDef paramValueDef = (BaseGraph_DataBase::ParamValueDef)i;
		if (m_mapValueControl.count(paramValueDef) > 0
			&& m_mapValueControl[paramValueDef])
		{
			m_mapValueControl[paramValueDef]->UpdateUIToValue();
			Value value = m_mapValueControl[paramValueDef]->GetValue();
			SetValue(paramValueDef, value);
		}
	}
}

EmUIElementMsgRet UIPropertyViewElement::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
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

bool UIPropertyViewElement::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnCombinedCtrlValueChged)
	{
		UICombinedCtrl * pCombinedCtrl = (UICombinedCtrl *)lParam;
		if (pCombinedCtrl)
		{
			if (m_mapControlValue.count(pCombinedCtrl) > 0)
			{
				BaseGraph_DataBase::ParamValueDef paramValueDef = m_mapControlValue[pCombinedCtrl];
				Value value = pCombinedCtrl->GetValue();
				SetValue(paramValueDef, value);
			}
			return true;
		}
	}
	return UIElement::OnChildMsg(emMsg, wParam, lParam);
}
