#include "stdafx.h"
#include "UIPropertyViewElement.h"
#include "GraphEditorCommonDef.h"
#include "UICheckColorButton.h"

UIPropertyViewElement::UIPropertyViewElement()
	: m_strObjID()
{
}

UIPropertyViewElement::~UIPropertyViewElement()
{
}

void UIPropertyViewElement::InitializeObject()
{
	for (int i = m_nParamIDBegin + 1; i < m_nParamIDEnd; i++)
	{
		//开局填充默认值
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
		case BaseGraph_DataBase::PropertyDisp_ColorCheckButton:
		{
			pElement = new UICheckColorButton;
			((UICheckColorButton *)pElement)->Create(0, m_hParentWnd, this);
			((UICheckColorButton *)pElement)->SetValue(valueDesc.stuDefault);
		}
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

bool UIPropertyViewElement::Create(int nX, int nY, int nWidth, int nHeight, int m_nID, HWND hParentWnd, UIElement * pParent, int nParamIDBegin, int nParamIDEnd)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);
		m_nParamIDBegin = nParamIDBegin;
		m_nParamIDEnd = nParamIDEnd;
		Initialize(nX, nY, nWidth, nHeight, m_nID, hParentWnd, pParent);
		UIImageDrawer * pImgDrawer = new UIImageDrawer(ThemeImage_ElementArea);
		m_mapDrawer[0].push_back(pImgDrawer);
		InitializeObject();
		bRet = true;
	}
	return bRet;
}

bool UIPropertyViewElement::BindObject(CString strObjID)
{
	if (m_strObjID != strObjID)
	{
		m_strObjID = strObjID;
		RefreshFromData();
	}
	return true;
}

void UIPropertyViewElement::RefreshFromData()
{
	if (!m_strObjID.IsEmpty())
	{
		std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam;
		if (DataRender_GetItemProperty(m_strObjID, mapParam))
		{
			//获取物件的值赋予窗口
			SetValue(mapParam);
		}
	}
}

void UIPropertyViewElement::SetValue(const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, bool bRefresh/* = true*/)
{
	for (std::map<BaseGraph_DataBase::ParamValueDef, Value>::const_iterator it = mapParam.begin(); it != mapParam.end(); it++)
	{
		if (it->first > m_nParamIDBegin && it->first < m_nParamIDEnd)
		{
			m_mapParam[it->first] = it->second;
		}		
	}
	if (bRefresh)
	{
		UpdateDataToUI();
	}
}

void UIPropertyViewElement::SetValue(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value, bool bRefresh/* = true*/)
{
	if (paramValueDef > m_nParamIDBegin && paramValueDef < m_nParamIDEnd)
	{
		m_mapParam[paramValueDef] = value;
	}
	if (bRefresh)
	{
		UpdateDataToUI();
	}
}

void UIPropertyViewElement::UpdateToObject(BaseGraph_DataBase::ParamValueDef paramValueDef, Value value)
{
	if (!m_strObjID.IsEmpty())
	{
		DataRender_SetItemProperty(m_strObjID, paramValueDef, value);
		UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
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

void UIPropertyViewElement::SetVisible(bool bVisible, bool bRefresh)
{
	m_bVisible = bVisible;
	for (auto it : m_mapValueControl)
	{
		if (it.second != nullptr)
		{
			it.second->SetVisible(bVisible, bRefresh);
		}
	}
	if (bRefresh)
	{
		Refresh();
	}
}

EmUIElementMsgRet UIPropertyViewElement::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
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
#ifdef _DEBUG
				BaseGraph_DataBase::faceinfo info;
				if (value.ToStruct(info))
				{
					int i = 0;
					i++;
				}
#endif
				SetValue(paramValueDef, value);
				UpdateToObject(paramValueDef, value);
			}
			return true;
		}
	}
	return UIElement::OnChildMsg(emMsg, wParam, lParam);
}
