#pragma once
#include "UIDrawer.h"

class BASEGRAPH_UICONTROL_API UIRectDrawer :
	public UIDrawer
{
public:
	UIRectDrawer(int nResID = 0);
	virtual ~UIRectDrawer();
	virtual void Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText = nullptr);
};

