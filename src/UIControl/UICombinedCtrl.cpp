#include "stdafx.h"
#include "UIControl\UICombinedCtrl.h"

void UICombinedCtrl::NotifyValueChg()
{
	EmitMsg(EmUIElementMsg_OnCombinedCtrlValueChged, m_nID, (LPARAM)this);
}
