#pragma once
#include "UIControl\UICombinedCtrl.h"
#include "UIControl\UIEdit.h"
#include "UIControl\UIStatic.h"
#include "UIControl\UIButton.h"

class BASEGRAPH_UICONTROL_API UIEditPath
	: public UICombinedCtrl
{
protected:
	UIStatic * m_pUIStatic;
	UIEdit * m_pUIEdit;
	UIButton * m_pUIButton;
	CString m_strFilter;
public:
	UIEditPath();
	virtual ~UIEditPath();
public:
	bool Create(CString strTitle, CString strFilter, int nTitleWidth, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();
	virtual void RearrangeChildPos();
	void SetPathFilter(CString strFilter);
};