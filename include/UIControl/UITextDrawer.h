#pragma once
#include "UIDrawer.h"

class BASEGRAPH_UICONTROL_API UITextDrawer :
	public UIDrawer
{
private:
	COLORREF m_nColor;
public:
	UITextDrawer(int nResID = 0);
	virtual ~UITextDrawer();
	virtual void Draw(CDC * pDC, int nX, int nY, int nWidth, int nHeight, LPCTSTR strText = nullptr);
	void SetColor(COLORREF nColor);
};

