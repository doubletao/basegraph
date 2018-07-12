#pragma once
#include "UIControl\UIElement.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIScrollBar.h"

class BASEGRAPH_UICONTROL_API UIFlexibleScrollThumbButton :
	public UIScrollThumbButton
{
public:
	UIFlexibleScrollThumbButton();
	~UIFlexibleScrollThumbButton();
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, bool bHorScroll, HWND hParentWnd, UIElement * pParent);
	virtual void DrawElement(CDC * pDC);
};

class BASEGRAPH_UICONTROL_API UIFlexibleScrollBar :
	public UIScrollBar
{
public:
	UIFlexibleScrollBar();
	virtual ~UIFlexibleScrollBar();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	virtual bool ThumbAreaMsgHandler(UINT nMsg, LPARAM lParam, WPARAM wParam);
public:
	virtual bool Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID = 0, bool bHorizon = false, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);
	virtual bool OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam);
};
