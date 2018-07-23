#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UITextDrawer.h"

class BASEGRAPH_UICONTROL_API UIStatic :
	public UIElement
{
protected:
	int m_nFont;
	COLORREF m_nFontColor;
public:
	UIStatic();
	virtual ~UIStatic();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd = nullptr, UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	void SetText(LPCTSTR strText, int nFont = 0, COLORREF nFontColor = 0);
	virtual void DrawElement(CDC * pDC);
};

