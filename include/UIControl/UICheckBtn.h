#pragma once
#include "UIControl\UIButton.h"

class BASEGRAPH_UICONTROL_API UICheckBtn :
	public UIButton
{
private:
	bool m_bCheck;
public:
	UICheckBtn();
	virtual ~UICheckBtn();
public:
	void SetCheck(bool bCheck);
	bool GetCheck();
	virtual void OnButtonClickDown();
	virtual void OnButtonClickUp();
	virtual void OnMouseHover();
	virtual void OnMouseLeave();
};

