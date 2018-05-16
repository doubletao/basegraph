#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButtonBar.h"

class BASEGRAPH_UICONTROL_API UICheckBtnBar : public UIButtonBar
{
public:
	UICheckBtnBar();
	virtual ~UICheckBtnBar();
public:
	void UnCkeckAll(int nExceptID = 0);//除指定项外其他全部uncheck
	void CkeckOne(int nCheckID = 0);//check指定项，其他全部uncheck
};

