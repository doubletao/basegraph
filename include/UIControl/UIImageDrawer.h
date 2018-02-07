#pragma once
#include "UIDrawer.h"

class BASEGRAPH_UICONTROL_API UIImageDrawer :
	public UIDrawer
{
public:
	UIImageDrawer(int nResID = 0);
	virtual ~UIImageDrawer();
public:
	virtual void Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText = nullptr);
};

