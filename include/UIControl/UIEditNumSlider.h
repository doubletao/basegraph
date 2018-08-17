#pragma once
#include "UIControl\UIEditNumSpin.h"
#include "UIControl\UISliderCtr.h"

class BASEGRAPH_UICONTROL_API UIEditNumSlider
	: public UIEditNumSpin
{
protected:
	UISliderCtr * m_pSliderCtrl;
public:
	UIEditNumSlider();
	virtual ~UIEditNumSlider();
public:
	virtual bool Create(CString strTitle, int nTitleWidth, DWORD dwStyleEs, int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent);
	virtual void RearrangeChildPos();
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateValueToUI();
	virtual void UpdateUIToValue();
};
