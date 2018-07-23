#pragma once
#include "GraphEditorCommonDef.h"

class UIPropertyTabBar : public UICheckBtnBar
{
public:
	UIPropertyTabBar();
	virtual ~UIPropertyTabBar();
protected:
	virtual void InitializeObject();
	virtual void RearrangeChildPos();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent);
};

