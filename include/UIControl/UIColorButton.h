#pragma once
#include "UIControl\UIElement.h"

class BASEGRAPH_UICONTROL_API UIColorButton : public UIElement
{
protected:
	sycolor m_color;
public:
	UIColorButton();
	virtual ~UIColorButton();
protected:
	virtual void Initialize(int nX, int nY, int nWidth, int nHeight, int nID = 0, HWND hParentWnd = nullptr, UIElement * pParent = nullptr);//初始化工作
	virtual void DrawElement(CDC * pDC);
public:
	bool Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent);
	virtual EmUIElementMsgRet HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnButtonClickDown();
	void SetColor(const sycolor & color, bool bRefresh = true);
	sycolor GetColor();
};

