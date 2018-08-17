#pragma once
#include "UIControl\UICombinedCtrl.h"
#include "UIControl\UIEdit.h"
#include "UIControl\UIStatic.h"

class BASEGRAPH_UICONTROL_API UIEditMutiLine
	: public UICombinedCtrl
{
protected:
	UIStatic * m_pUIStatic;
	UIEdit * m_pUIEdit;
public:
	UIEditMutiLine();
	virtual ~UIEditMutiLine();
public:
	bool Create(CString strTitle, int nX, int nY, int nWidth, int nHeight, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();
	virtual void RearrangeChildPos();
};
