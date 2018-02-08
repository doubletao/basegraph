#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButtonBar.h"

class BASEGRAPH_UICONTROL_API UICheckBtnBar : public UIButtonBar
{
public:
	UICheckBtnBar();
	virtual ~UICheckBtnBar();
public:
	void UnCkeckAll(int nExceptID = 0);
};

