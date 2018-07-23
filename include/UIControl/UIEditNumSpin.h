#pragma once
#include "UIControl\UIEditNum.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIStatic.h"

class BASEGRAPH_UICONTROL_API UIEditNumSpin
	: public UIEditNum
{
protected:
	UIButton * m_pBtnUpSpin;
	UIButton * m_pBtnDownSpin;
	UIStatic * m_pUIStatic;
	float m_fStep;
public:
	UIEditNumSpin();
	virtual ~UIEditNumSpin();
public:
	virtual bool Create(CString strTitle, int nTitleWidth, DWORD dwStyleEs, int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void RearrangeChildPos();
};
