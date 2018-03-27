#pragma once
#include "UIControl\UIEditNum.h"
#include "UIControl\UIButton.h"

class BASEGRAPH_UICONTROL_API UIEditNumSpin
	: public UIEditNum
{
protected:
	UIButton * m_pBtnUpSpin;
	UIButton * m_pBtnDownSpin;
	float m_fStep;
public:
	UIEditNumSpin();
	virtual ~UIEditNumSpin();
public:
	bool Create(int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void RearrangeChildPos();
};
