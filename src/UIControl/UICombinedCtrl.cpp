#include "stdafx.h"
#include "UIControl\UICombinedCtrl.h"

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
