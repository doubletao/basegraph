#pragma once
#include "UIDrawer.h"
class BASEGRAPH_UICONTROL_API UIColorDrawer :
	public UIDrawer
{
public:
	UIColorDrawer(int nResID = 0);
	virtual ~UIColorDrawer();
public:
	virtual void Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText = nullptr);
};

