#pragma once
#include "UIElement.h"
#include "UITextDrawer.h"

class BASEGRAPH_UICONTROL_API UIStatic :
	public UIElement
{
protected:
	UITextDrawer * m_pTextDrawer;
public:
	UIStatic();
	virtual ~UIStatic();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd = nullptr, UIElement * pParent = nullptr, LPCTSTR strText = nullptr, int nFont = 0, COLORREF nFontColor = 0);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam);
	void SetText(LPCTSTR strText, int nFont = 0, COLORREF nFontColor = 0);
};

