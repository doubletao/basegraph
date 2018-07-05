#include "stdafx.h"
#include "UIControl\UICombinedCtrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BaseGraph_Common::Value UICombinedCtrl::GetValue()
{
	return m_value;
}

void UICombinedCtrl::SetValue(BaseGraph_Common::Value value, bool bRefresh)
{
	m_value = value;
	if (bRefresh)
	{
		UpdateValueToUI();
	}
}

void UICombinedCtrl::NotifyValueChg()
{
	EmitMsg(EmUIElementMsg_OnCombinedCtrlValueChged, m_nID, (LPARAM)this);
}

void UICombinedCtrl::SetVisible(bool bVisible, bool bRefresh)
{
	m_bVisible = bVisible;
	for (auto it : m_vecChildElement)
	{
		if (it)
		{
			it->SetVisible(bVisible, bRefresh);
		}
	}
	if (bRefresh)
	{
		Refresh();
	}
}
